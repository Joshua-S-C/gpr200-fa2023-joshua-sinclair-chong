/*
	Author: Eric Winebrenner
*/


#pragma once
#include "mesh.h"

namespace ew {
	MeshData createCube(float size);
	MeshData createPlane(float size, int subdivs);
	MeshData createCylinder(float radius, float height, int segments);
	MeshData createSphere(float radius, int segments);
	MeshData createTorus(float radius, float ringRadius, int segments, int ringSegments);
}
