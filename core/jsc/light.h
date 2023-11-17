#pragma once
#include "../ew/ewMath/mat4.h"
#include "../ew/ewMath/vec3.h"
#include "../ew/ewMath/ewMath.h"
#include "../ew/transform.h"

namespace jsc {
	struct Light {
		ew::Transform transform; // World Space
		ew::Vec3 clr; // RBG
	};

	struct Material {
		// Coefficients. 0-1
		float ambientK = .2;
		float diffuseK = .5;
		float specularK = .5;
		float shininess = 32;
	};
}