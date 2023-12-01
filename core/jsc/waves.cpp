#include "waves.h"


namespace jsc {
	Wave::Wave(float frequency, float amplitude, float speed, ew::Vec3 color) {
		f = frequency;
		a = amplitude;
		s = speed;
		clr = color;
	}

	// Moved to shader.cpp
	//void setWave(ew::Shader* shader, const char* name, jsc::Wave){
	//	shader->setFloat(name,wave.a);
	//	shader->setFloat(name,wave.f);
	//	shader->setVec3(name,wave.clr);
	//}

}