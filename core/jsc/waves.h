#pragma once
#include "../ew/ewMath/vec3.h"
#include "../ew/shader.h"


namespace jsc {
	struct Wave {
		float f, a, s;		// Frequency, Amplitude, Speed
		ew::Vec3 clr;

		Wave::Wave(float frequency, float amplitude, float speed, ew::Vec3 color);
	};

}