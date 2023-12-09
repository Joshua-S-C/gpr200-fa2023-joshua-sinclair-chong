#pragma once
#include "../ew/ewMath/vec3.h"
#include "../ew/shader.h"


namespace jsc {
	struct Wave {
		float l, a, s;		// Wavelength, Amplitude, Speed
		float alpha, blend;
		ew::Vec3 clr;

		Wave::Wave(float wavelength, float amplitude, float speed, ew::Vec3 color);
		Wave::Wave(float wavelength, float amplitude, float speed, float _alpha, float lightBlend, ew::Vec3 color);
	};

}