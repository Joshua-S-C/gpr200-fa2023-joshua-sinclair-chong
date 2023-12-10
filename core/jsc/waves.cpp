#include "waves.h"


namespace jsc {
	Wave::Wave(float wavelength, float amplitude, float speed, ew::Vec3 color) {
		l = wavelength;
		a = amplitude;
		s = speed;
		clr = color;
	}
	
	Wave::Wave(float wavelength, float amplitude, float speed, float _alpha, float lightBlend, ew::Vec3 color) {
		l = wavelength;
		a = amplitude;
		s = speed;
		alpha = _alpha;
		blend = lightBlend;
		clr = color;
	}

	GWave::GWave(float wavelength, float steepness, ew::Vec2 direction, ew::Vec3 color){
		l = wavelength;
		s = steepness;
		dir = direction;
		clr = color;
	}
	
	GWave::GWave(float wavelength, float steepness, ew::Vec2 direction, ew::Vec3 color, float _alpha, float lightBlend){
		l = wavelength;
		s = steepness;
		dir = direction;
		alpha = _alpha;
		blend = lightBlend;
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