#include "waves.h"


namespace jsc {
	Wave::Wave(float waveLength, float amplitude, float speed, ew::Vec3 color) {
		l = waveLength;
		a = amplitude;
		s = speed;
		clr = color;
	}
	
	Wave::Wave(float waveLength, float amplitude, float speed, float _alpha, float _lightBlend, ew::Vec3 color) {
		l = waveLength;
		a = amplitude;
		s = speed;
		alpha = _alpha;
		blend = _lightBlend;
		clr = color;
	}

	// TODO: Wave presets maybe

	// Moved to shader.cpp
	//void setWave(ew::Shader* shader, const char* name, jsc::Wave){
	//	shader->setFloat(name,wave.a);
	//	shader->setFloat(name,wave.f);
	//	shader->setVec3(name,wave.clr);
	//}

}