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

	Wave Wave::derive() {
		jsc::Wave derived(l / 2, a / 3, s * 1.3, clr);
		return derived;
	}

	// Gerstner Waves
	GWave::GWave() {
		l = 10;
		s = 0.3;
		g = 9.8;
		dir = ew::Vec2{ 0.0, 0.5 };
		clr = ew::Vec3{ .5f,.8f,1 };
		alpha = 1;
		alpha = 1;
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

	GWave GWave::derive() {
		jsc::GWave derived(l / 1.7, s / 3, dir + ew::Vec2(0.3, .4), clr);
		return derived;
	}

	void GWave::populate(GWave waves[], int numWaves)
	{
		GWave derived(l, s, dir, clr);
		for (int i = 0; i < numWaves; i++) {
			waves[i] = derived;
			derived = derived.derive();
		}
	}

	// TODO: Wave presets maybe

	// Moved to main.cpp
	// TODO: Put in shader.cpp plz
	//void setWave(ew::Shader* shader, const char* name, jsc::Wave){
	//	shader->setFloat(name,wave.a);
	//	shader->setFloat(name,wave.f);
	//	shader->setVec3(name,wave.clr);
	//}

}