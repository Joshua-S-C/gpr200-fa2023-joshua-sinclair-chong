#pragma once
#include "../ew/ewMath/vec3.h"
#include "../ew/shader.h"


namespace jsc {
	struct Wave {
		float f, a;		// Frequency and Amplitude
		ew::Vec3 clr;

		Wave::Wave(float frequency, float amplitude, ew::Vec3 color);
	};

}