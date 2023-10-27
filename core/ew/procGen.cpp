/*
	Author: Eric Winebrenner
*/


#include "procGen.h"
#include <stdlib.h>

namespace ew {
	/// <summary>
	/// Helper function for createCube. Note that this is not meant to be used standalone
	/// </summary>
	/// <param name="normal">Normal direction of the face</param>
	/// <param name="size">Width/height of the face</param>
	/// <param name="mesh">MeshData struct to fill</param>
	static void createCubeFace(ew::Vec3 normal, float size, MeshData* mesh) {
		unsigned int startVertex = mesh->vertices.size();
		ew::Vec3 a = ew::Vec3(normal.z, normal.x, normal.y); //U axis
		ew::Vec3 b = ew::Cross(normal, a); //V axis
		for (int i = 0; i < 4; i++)
		{
			int col = i % 2;
			int row = i / 2;

			ew::Vec3 pos = normal * size * 0.5f;
			pos -= (a + b) * size * 0.5f;
			pos += (a * col + b * row) * size;
			Vertex vertex;// = &mesh->vertices[mesh->vertices.size()];
			vertex.pos = pos;
			vertex.normal = normal;
			vertex.uv = ew::Vec2(col, row);
			mesh->vertices.push_back(vertex);
		}

		//Indices
		mesh->indices.push_back(startVertex);
		mesh->indices.push_back(startVertex + 1);
		mesh->indices.push_back(startVertex + 3);
		mesh->indices.push_back(startVertex + 3);
		mesh->indices.push_back(startVertex + 2);
		mesh->indices.push_back(startVertex);
	}
	
	/// <summary>
	/// Creates a cube of uniform size
	/// </summary>
	/// <param name="size">Total width, height, depth</param>
	/// <param name="mesh">MeshData struct to fill. Will be cleared.</param>
	MeshData createCube(float size) {
		MeshData mesh;
		mesh.vertices.reserve(24); //6 x 4 vertices
		mesh.indices.reserve(36); //6 x 6 indices
		createCubeFace(ew::Vec3{ +0.0f,+0.0f,+1.0f }, size, &mesh); //Front
		createCubeFace(ew::Vec3{ +1.0f,+0.0f,+0.0f }, size, &mesh); //Right
		createCubeFace(ew::Vec3{ +0.0f,+1.0f,+0.0f }, size, &mesh); //Top
		createCubeFace(ew::Vec3{ -1.0f,+0.0f,+0.0f }, size, &mesh); //Left
		createCubeFace(ew::Vec3{ +0.0f,-1.0f,+0.0f }, size, &mesh); //Bottom
		createCubeFace(ew::Vec3{ +0.0f,+0.0f,-1.0f }, size, &mesh); //Back
		
		printf("Created cube\n");

		return mesh;
	}

	/// <summary>
	/// Creates a plane of size
	/// </summary>
	/// <param name="size">Total width, and depth</param>
	/// <param name="subdivs">Number of individual planes</param>
	/// <param name="mesh">MeshData struct to fill. Will be cleared.</param>
	MeshData createPlane(float size, int subdivs) {
		MeshData mesh;
		int rows = subdivs + 1, cols = subdivs + 1;
		mesh.vertices.reserve(rows * cols);	// Subdiv+1 ^ 2 vertices
		mesh.indices.reserve(rows * cols);	// Subdivisions+1 ^ 2 indices
		
		// Vertices
		for (float rowNum = 0; rowNum <= subdivs; rowNum++) {
			for (float colNum = 0; colNum <= subdivs; colNum++) {

				ew::Vec3 pos;
				pos.x = size * (colNum / subdivs);
				pos.z = -size * (rowNum / subdivs);
				pos.y = 0;

				ew::Vec3 norm = { 0,1,0 };

				ew::Vec2 uv;
				uv.x = colNum / subdivs;
				uv.y = rowNum / subdivs;

				ew::Vertex vert;
				vert.pos = pos;
				vert.normal = norm;
				vert.uv = uv;
				mesh.vertices.push_back(vert);				
			}
		}

		// Indices
		for (float rowNum = 0; rowNum < subdivs; rowNum++) {
			for (float colNum = 0; colNum < subdivs; colNum++) {
				unsigned int start = rowNum * cols + colNum;

				// Bottom Right Triangle
				mesh.indices.push_back(start);
				mesh.indices.push_back(start+1);
				mesh.indices.push_back(start+1+cols);

				// Top Left Triangle
				mesh.indices.push_back(start);
				mesh.indices.push_back(start + 1 + cols);
				mesh.indices.push_back(start + cols);
			}
		}

		printf("Created plane\n");
		return mesh;
	}

	/// <summary>
	/// Creates a cylinder
	/// </summary>
	/// <param name="radius">Distance of outter verts from center</param>
	/// <param name="height">Distance from bototm to top rings</param>
	/// <param name="segments">Number of verts per ring</param>
	/// <param name="mesh">MeshData struct to fill. Will be cleared.</param>
	MeshData createCylinder(float radius, float height, int segments) {
		MeshData mesh;

		float topY = height / 2;	// Height of top plane
		float botY = -topY;			// Height of bottom plane
		float step = 2 * PI / segments;	// Angle per increment

		// This seems like it's probablt right
		mesh.vertices.reserve(segments * 4);	// vertices
		mesh.indices.reserve(segments * 2);		// indices

// Top Vertices ---------------------------------------------------------*/
		// Top Centre Vert
		ew::Vertex topVert = { 0, topY, 0 };
		mesh.vertices.push_back(topVert);

		// Top Ring Pointing Up
		for (int i = 0; i <= segments; i++) {
			ew::Vec3 pos;
			pos.y = topY;
			pos.z = radius * sin(step * i);
			pos.x = radius * cos(step * i);

			// First ring pointing up
			ew::Vec3 norm = { 0,1,0 };

			ew::Vec2 uv;

			ew::Vertex vert;
			vert.pos = pos;
			vert.normal = norm;
			vert.uv = uv;
			mesh.vertices.push_back(vert);
		}

		// Top Ring Pointing Out
		// Second ring of verts with normals pointing up for hard lighting on the cap
		// Pointing out: subtract positions of center and vert, then normalize
		for (int i = 0; i <= segments; i++) {
			ew::Vec3 pos;
			pos.y = topY;
			pos.z = radius * sin(step * i);
			pos.x = radius * cos(step * i);

			ew::Vec3 norm = ew::Normalize(topVert.pos - pos);	

			ew::Vec2 uv;

			ew::Vertex vert;
			vert.pos = pos;
			vert.normal = norm;
			vert.uv = uv;
			mesh.vertices.push_back(vert);
		}

// Bottom Vertices ------------------------------------------------------*/
		// Bottom Centre Vert
		ew::Vertex botVert = { 0, botY, 0 };
		mesh.vertices.push_back(botVert);

		// Bottom Ring Pointing Out
		for (int i = 0; i <= segments; i++) {
			ew::Vec3 pos;
			pos.y = botY;
			pos.z = radius * sin(step * i);
			pos.x = radius * cos(step * i);

			ew::Vec3 norm = Normalize(botVert.pos - pos);

			ew::Vec2 uv;

			ew::Vertex vert;
			vert.pos = pos;
			vert.normal = norm;
			vert.uv = uv;
			mesh.vertices.push_back(vert);
		}

		// Bottom Ring Pointing Down
		for (int i = 0; i <= segments; i++) {
			ew::Vec3 pos;
			pos.y = botY;
			pos.z = radius * sin(step * i);
			pos.x = radius * cos(step * i);

			ew::Vec3 norm = { 0,-1,0 };

			ew::Vec2 uv;

			ew::Vertex vert;
			vert.pos = pos;
			vert.normal = norm;
			vert.uv = uv;
			mesh.vertices.push_back(vert);
		}

// Indices --------------------------------------------------------------*/
		// Top Indices
		unsigned int start = 1;
		unsigned int centre = 0;	// Index of top center

		for (int i = 0; i < segments; i++) {
		}

		// Side Indices
		unsigned int sideStart = 2;	// Index of first top ring vert
		int cols = segments + 1;

		for (int i = 0; i < cols; i++) {

		}

		// Bottom Center



		// Indices
		

		return mesh;
	}
}