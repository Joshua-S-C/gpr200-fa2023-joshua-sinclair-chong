/*
	Author: Eric Winebrenner
	Edited: Joshua Sinclair Chong
*/


#pragma once
#include "../ew/mesh.h"

namespace jsc {
	ew::MeshData createCube(float size);
	ew::MeshData createPlane(float size, int subdivs);
	ew::MeshData createCylinder(float radius, float height, int segments);
	ew::MeshData createSphere(float radius, int segments);
	ew::MeshData createTorus(float radius, float ringRadius, int segments, int ringSegments);
}
