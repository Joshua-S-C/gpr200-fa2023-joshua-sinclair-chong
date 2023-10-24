/*
	Author: Eric Winebrenner
*/


#pragma once
#include "mesh.h"

namespace ew {
	MeshData createCube(float size);
	MeshData createPlane(float size, int subdivs);
	MeshData createCylinder(float radius, float height, int segments);
}
