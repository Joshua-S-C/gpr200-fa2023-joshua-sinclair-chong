#pragma once
#include "../ew/ewMath/vec3.h"
#include "../ew/shader.h"


namespace jsc {
	/// <summary>
	/// Simple waves for use with simpleSin.vert/frag
	/// </summary>
	/// <param name="l">Wavelength</param>
	/// <param name="a">Amplitude</param>
	/// <param name="s">Speed</param>
	struct Wave {
		float l, a, s;
		float alpha, blend;
		ew::Vec3 clr;

		Wave::Wave(float wavelength, float amplitude, float speed, ew::Vec3 color);
		Wave::Wave(float wavelength, float amplitude, float speed, float _alpha, float lightBlend, ew::Vec3 color);

		/// <summary>
		/// Creates a smaller wave from a given wave so I don't have to do it manually.
		/// </summary>
		Wave derive();
	};

	/// <summary>
	/// Gerstner Waves. 
	/// </summary>
	/// <param name="l">Wavelength</param>
	/// <param name="s">Steepness</param>
	/// <param name="dir">Direction wave is traveling in</param>
	struct GWave {
		float l, s;	
		ew::Vec2 dir;
		float alpha, blend;
		ew::Vec3 clr;

		GWave::GWave(float wavelength, float steepness, ew::Vec2 direction, ew::Vec3 color);
		GWave::GWave(float wavelength, float steepness, ew::Vec2 direction, ew::Vec3 color, float _alpha, float lightBlend);

		/// <summary>
		/// Creates a smaller wave from a given wave so I don't have to do it manually.
		/// </summary>
		GWave derive();

		/// <summary>
		/// Fills array of GWaves with wave and derivatives
		/// </summary>
		void populate(GWave waves[], int numWaves);
	};
}