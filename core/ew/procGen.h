/*
	Author: Eric Winebrenner
*/


#pragma once
#include "mesh.h"
#include "texture.h"
#include "external/glad.h"
#include "external/stb_image.h"

namespace ew {
	MeshData createCube(float size);
	MeshData createPlane(float width, float height, int subdivisions);
	MeshData createTerrain(float width, float height, int subdivisions);
	MeshData createSphere(float radius, int subdivisions);
	MeshData createCylinder(float radius, float height, int subdivisions);
	MeshData createHeightmap(const char* filePath, float yScale = .15, float yShift = 32.0f);

}