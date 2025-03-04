/*
	Author: Eric Winebrenner
	Edited: Joshua Sinclair Chong
*/


#include "procGen.h"
#include <stdlib.h>

namespace jsc {
	/// <summary>
	/// Helper function for createCube. Note that this is not meant to be used standalone
	/// </summary>
	/// <param name="normal">Normal direction of the face</param>
	/// <param name="size">Width/height of the face</param>
	/// <param name="mesh">ew::MeshData struct to fill</param>
	static void createCubeFace(ew::Vec3 normal, float size, ew::MeshData* mesh) {
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
			ew::Vertex vertex;// = &mesh->vertices[mesh->vertices.size()];
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
	ew::MeshData createCube(float size) {
		ew::MeshData mesh;
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
	ew::MeshData createPlane(float size, int subdivs) {
		ew::MeshData mesh;
		int rows = subdivs + 1, cols = subdivs + 1;
		mesh.vertices.reserve(rows * cols);	// Subdiv+1 ^ 2 vertices
		mesh.indices.reserve(subdivs *  subdivs);	// Subdivisions^2 indices
		
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
	ew::MeshData createCylinder(float radius, float height, int segments) {
		ew::MeshData mesh;

		float topY = height / 2;	// Height of top plane
		float botY = -topY;			// Height of bottom plane
		float step = 2 * ew::PI / segments;	// Angle per increment

		// This seems like it's probably right
		mesh.vertices.reserve(segments * 4 + 4);	// 4 rings + 2 dupes + 2 centres
		mesh.indices.reserve(segments * 3 + 2);		// 

		// Top Centre Vert
		ew::Vertex topVert;
		topVert.pos = { 0, topY, 0 };
		topVert.normal = { 0, 1, 0 };
		topVert.uv = { 0.5, 0.5 };
		mesh.vertices.push_back(topVert);

		// Bottom Centre Vert
		ew::Vertex botVert;
		botVert.pos = { 0, botY, 0 };
		botVert.normal = { 0, -1, 0 };
		botVert.uv = { 0.5, 0.5 };
		mesh.vertices.push_back(botVert);

	// Top Vertices ---------------------------------------------------------*/
		// Top Ring Pointing Up
		for (int i = 0; i <= segments; i++) {	// <= to dupe first vert
			ew::Vec3 pos;
			pos.y = topY;
			pos.z = radius * sin(step * i);
			pos.x = radius * cos(step * i);

			// First ring pointing up
			ew::Vec3 norm = { 0,1,0 };

			ew::Vec2 uv;	// UV's remapped to a 0-1 range
			uv.x = (sin(step * i) + 1) / 2;
			uv.y = (cos(step * i) + 1) / 2;

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
			uv.x = (float)i / (float)segments;
			uv.y = 1;


			ew::Vertex vert;
			vert.pos = pos;
			vert.normal = norm;
			vert.uv = uv;
			mesh.vertices.push_back(vert);
		}

	// Bottom Vertices ------------------------------------------------------*/
		// Bottom Ring Pointing Out
		for (int i = 0; i <= segments; i++) {
			ew::Vec3 pos;
			pos.y = botY;
			pos.z = radius * sin(step * i);
			pos.x = radius * cos(step * i);

			ew::Vec3 norm = Normalize(botVert.pos - pos);

			ew::Vec2 uv;
			uv.x = (float)i / (float)segments; // x_x forgor it;s int division
			uv.y = 0;

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
			uv.x = (sin(step * i) + 1) / 2;
			uv.y = (cos(step * i) + 1) / 2;

			ew::Vertex vert;
			vert.pos = pos;
			vert.normal = norm;
			vert.uv = uv;
			mesh.vertices.push_back(vert);
		}

	// Indices --------------------------------------------------------------*/
		// Top Indices
		unsigned int centre = 0;// Top Centre
		unsigned int start = 2;	// Index of first ring vert
		
		// Cap Indices
		for (int i = 0; i < segments; i++) {
			mesh.indices.push_back(start + i);
			mesh.indices.push_back(centre);
			mesh.indices.push_back(start + i + 1);
		}

		// Side Indices
		unsigned int sideStart = segments + start;	// Index of first top ring (outward) vert
		unsigned int cols = segments + 2;	// Difference top and bottom verts. Offset of 2 to account for centre verts
		// They're the same cuz I added both centre verts before all others

		for (int i = 0; i < cols; i++) {
			start = sideStart + i;
			
			mesh.indices.push_back(start);
			mesh.indices.push_back(start + 1);
			mesh.indices.push_back(start + cols);
			
			mesh.indices.push_back(start + 1);
			mesh.indices.push_back(start + 1 + cols);
			mesh.indices.push_back(start + cols);
		}

		// Bottom Cap Indices
		centre = 1;	// Bottom Centre
		start = segments * 3 + 5;	// Start of bottom (down) vertices. 3 rings + 3 dupes + 2 centre verts

		for (int i = 0; i < segments; i++) {
			mesh.indices.push_back(start + i + 1);
			mesh.indices.push_back(centre);
			mesh.indices.push_back(start + i);
		}

		printf("Created cylinder\n");

		return mesh;
	}

	/// <summary>
	/// Creates a polar sphere
	/// </summary>
	/// <param name="radius">Distance of verts from center</param>
	/// <param name="segments">Number of verts per ring</param>
	ew::MeshData createSphere(float radius, int segments) {
		ew::MeshData mesh;

		//				Theta					Phi
		ew::Vec2 step = { (2 * ew::PI / segments) , (ew::PI / segments) };

		// 
		mesh.vertices.reserve(segments * 2 + ((segments+1) * (segments+1)));		// 2*seg for poles. seg+1 ^ 2 for rows
		mesh.indices.reserve(segments * 2 + 2);		// 2 Caps, segment # of rows and cols

	// Vertices -------------------------------------------------------------*/
		// Top Centre Vert (creates dupes)
		ew::Vertex topVert;
		topVert.pos = { 0, radius, 0 };
		topVert.normal = { 0, 1, 0 };
		for (int i = 0; i < segments; i++) {
			topVert.uv.x = (float)i/segments;
			topVert.uv.y = 1;
			mesh.vertices.push_back(topVert);
		}

		// Converge at poles
		for (float row = 0; row <= segments; row++) {
			float phi = row * step.y;
			// Dupe col for each row
			for (float col = 0; col <= segments; col++) {
				float theta = col * step.x;
				
				ew::Vec3 pos;
				pos.x = radius * sin(phi) * cos(theta);
				pos.y = radius * cos(phi);
				pos.z = radius * sin(theta) * sin(phi);

				ew::Vec3 norm = Normalize(pos);

				ew::Vec2 uv;
				uv.x = col / segments;
				uv.y = 1 - row / segments;

				ew::Vertex vert;
				vert.pos = pos;
				vert.normal = norm;
				vert.uv = uv;
				mesh.vertices.push_back(vert);
			}
		}

		// Bottom Centre Vert (dupes)
		ew::Vertex botVert;
		botVert.pos = { 0, -radius, 0 };
		topVert.normal = { 0, -1, 0 };
		topVert.uv = 0;
		for (int i = 0; i < segments; i++) {
			topVert.uv.x = (float)i / segments;
			topVert.uv.y = 0;
			mesh.vertices.push_back(botVert);
		}

	// Indices --------------------------------------------------------------*/
		// Top Cap
		int poleStart = 0;					// First pole vertex
		int sideStart = segments * 2 + 1;	// First side index

		for (int i = 0; i < segments; i++) {
			mesh.indices.push_back(sideStart + i); // Side vert
			mesh.indices.push_back(poleStart + i); // Pole dupes
			mesh.indices.push_back(sideStart + i + 1); // Next side vert
		}

		// Rows, doesn't draw caps
		int cols = segments + 1;
		for (int row = 1; row < segments - 1; row++) {
			for (int col = 0; col < segments; col++) {
				int start = row * cols + col + segments;

				mesh.indices.push_back(start);
				mesh.indices.push_back(start + 1);
				mesh.indices.push_back(start + cols);

				mesh.indices.push_back(start + 1);
				mesh.indices.push_back(start + cols + 1);
				mesh.indices.push_back(start + cols);
			}
		}

		// Bottom Cap
		poleStart = segments * segments + 2 * segments;	// Bottom pole starting vertex
		sideStart = segments * segments + segments - 1;	// Last side index

		for (int i = 0; i < segments; i++) {
			mesh.indices.push_back(sideStart + i + 1);
			mesh.indices.push_back(poleStart+i);
			mesh.indices.push_back(sideStart + i);
		}

		printf("Created sphere\n");

		return mesh;
	}
	
	/// <summary>
	/// Creates a torus
	/// </summary>
	/// <param name="radius">Radius of inside</param>
	/// <param name="tubeRadius">Radius of tube</param>
	/// <param name="segments">Number rings drawn</param>
	/// <param name="ringSegments">Subdivs of circles</param>
	ew::MeshData createTorus(float radius, float ringRadius, int segments, int ringSegments) {
		ew::MeshData mesh;

		float ringStep = ew::TAU / ringSegments;
		float torusStep = ew::TAU / segments;

		mesh.vertices.reserve(segments * ringSegments);		//
		mesh.indices.reserve(segments * ringSegments);		// 

		
	// Vertices -------------------------------------------------------------*/
		// Creating tube
		for (float i = 0; i <= segments; i++)
		{
			float torusTheta = i * torusStep;

			// Centre of Rings
			ew::Vec3 posOffset;
			posOffset.x = radius * cos(i * torusStep);
			posOffset.z = radius * sin(i * torusStep);
			posOffset.y = 0;

			//// Horizontal ring visual
			//ew::Vertex vert;
			//vert.pos = posOffset;
			//mesh.vertices.push_back(vert);

			// Create the circle
			for (float j = 0; j <= ringSegments; j++) {
				ew::Vec3 pos;
				float ringTheta = j * ringStep;
				pos.x = cos(torusTheta) * (radius + cos(ringTheta) * ringRadius);
				pos.z = sin(torusTheta) * (radius + cos(ringTheta) * ringRadius);
				pos.y = sin(ringTheta) * ringRadius;

				ew::Vec3 norm = Normalize(posOffset - pos);

				ew::Vec2 uv;
				uv.x = j / ringSegments;
				uv.y = i / ringSegments;

				ew::Vertex vert;
				vert.pos = pos;
				vert.normal = norm;
				vert.uv = uv;
				mesh.vertices.push_back(vert);
			}
		}


	// Indices --------------------------------------------------------------*/
		// Around Torus (Horizontal)
		for (int i = 0; i < segments; i++)
			// Around Rings (Vert)
			for (int j = 0; j < ringSegments; j++) {
				int _rs = ringSegments + 1; // On extra to account for dupe verts
				mesh.indices.push_back(j+1	+ ((i+1) * _rs));
				mesh.indices.push_back(j	+ ((i+1) * _rs));
				mesh.indices.push_back(j	+ (i * _rs));

				mesh.indices.push_back(j+1	+ (i * _rs));
				mesh.indices.push_back(j+1	+ ((i+1) * _rs));
				mesh.indices.push_back(j	+ (i * _rs));
			}

		printf("Created torus\n");

		return mesh;
	}

}