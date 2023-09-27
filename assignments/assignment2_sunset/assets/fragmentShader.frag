#version 450

in vec2 uv;
out vec4 FragColor;

// Misc. Uniforms
uniform float _Time;
uniform vec2 _Resolution;

// BG Colour Uniforms
uniform vec3 bgClrTop;
uniform vec3 bgClrBtm;
uniform vec3 bgClrTop2;	
uniform vec3 bgClrBtm2;

// Sun Uniforms
uniform float sunSpeed;
uniform float sunBlur;
uniform float sunInner;
uniform vec3 sunClr;
uniform vec3 sunClr2;

// Wave 1 Uniforms
uniform vec3 wave1ClrTop;
uniform vec3 wave1ClrBtm;
uniform float wave1F;	// Frequency
uniform float wave1A;	// Amplitude
uniform float wave1O;	// Verticle Offset

// Wave 2 Uniforms
uniform vec3 wave2ClrTop;
uniform vec3 wave2ClrBtm;
uniform float wave2F;
uniform float wave2A;
uniform float wave2O;

// Wave 2-Comound Uniforms
uniform float wave2CF;	
uniform float wave2CA;

struct Background {
	vec3 clrTop1, clrBtm1, clrTop2, clrBtm2;
	vec2 uv;
	float time; // I think *need* to pass in time.

/*	Commenting out member functions cuz they break on some machines :(

	// [Unused] Used as the first output colour
	vec3 Color() {
		vec3 clr = mix(clrBtm1, clrTop1, uv.y + sin(time));
		return clr;
	}

	// Used as first output colour. Blends Top and Bottom colours then each other
	vec3 ColorAdvance() {
		vec3 clr1 = mix(clrBtm1, clrBtm2, sin(time));
		vec3 clr2 = mix(clrTop1, clrTop2, sin(time));
		vec3 clr = mix(clr1, clr2, uv.y + sin(time));
		return clr;
	}
*/
};

vec3 BG_Color_Advance(Background bg) {
		vec3 clr1 = mix(bg.clrBtm1, bg.clrBtm2, sin(_Time));
		vec3 clr2 = mix(bg.clrTop1, bg.clrTop2, sin(_Time));
		vec3 clr = mix(clr1, clr2, uv.y + sin(_Time));
		return clr;
}

struct Sun {
	float speed, radius, inner;
	vec3 clr1, clr2;
	vec2 uv;
	float time;	

/*	Commenting out member functions cuz they break on some machines

	// Used as start of range for mix.
	vec3 Color() {
		return mix(clr2, clr1, uv.y + sin(time));
	}

	// Interpolation value for mix
	float Lerp() {
		float sunLerp = distance(uv, vec2(0.0, -.5 + sin(time * speed))); 
		sunLerp = smoothstep(inner + radius, inner, sunLerp);
		return sunLerp;
	}
*/
};

// Used as start of range for mix
vec3 Sun_Color(Sun sun) {
		return mix(sun.clr2, sun.clr1, sun.uv.y + sin(_Time));
	}

// Interpolation value for mix
float Sun_Lerp(Sun sun) {
	float sunLerp = distance(sun.uv, vec2(0.0, -.5 + sin(_Time * sun.speed))); 
	sunLerp = smoothstep(sun.inner + sun.radius, sun.inner, sunLerp);
	return sunLerp;
}


struct Wave {
	float f, a, offset;
	vec3 clrBtm, clrTop;
	vec2 uv;
	float time;

/*	Commenting out member functions cuz they break on some machines

	// Used as start of range for mix.
	vec3 Color() {
		return mix(clrTop, clrBtm, -uv.y / (3.0 + offset)); // Dividing uv.y makes it so that the distribution of the top and bottom colours is more even across the visible part of the wave
	}

	// Used as interpolation value for mix
	float Lerp() {
		float waveLerp = offset + sin(uv.x * f + time) * (a * sin(time));
		waveLerp = step(waveLerp,uv.y);
		return waveLerp;
	}

	// Used as interpolation value for mix, with 2 waves
	float LerpCompound(float wave2Lerp) {
		float waveLerp = offset + sin(uv.x * f + time) * (a * sin(time));
		waveLerp += wave2Lerp;
		waveLerp = step(waveLerp,uv.y);
		return waveLerp;
	}

	// [Unused] Originally used for adding waves w/ step(waveLerp, uv.y) on sums. I just made LearpCompound tho
	float LerpNoStep() {
		float waveLerp = offset + sin(uv.x * f + time) * (a * sin(time));
		return waveLerp;
	}
*/
};

	// Used as start of range for mix.
	vec3 Wave_Color(Wave wave) {
		return mix(wave.clrTop, wave.clrBtm, -wave.uv.y / (3.0 + wave.offset));
	}

	// Used as interpolation value for mix
	float Wave_Lerp(Wave wave) {
		float waveLerp = wave.offset + sin(wave.uv.x * wave.f + wave.time) * (wave.a * sin(wave.time));
		waveLerp = step(waveLerp,wave.uv.y);
		return waveLerp;
	}

	// Used as interpolation value for mix, with 2 waves
	float Wave_Lerp_Compound(Wave wave, float wave2Lerp) {
		float waveLerp = wave.offset + sin(wave.uv.x * wave.f + wave.time) * (wave.a * sin(wave.time));
		waveLerp += wave2Lerp;
		waveLerp = step(waveLerp,wave.uv.y);
		return waveLerp;
	}

// Struct used to add onto waves in LerpCompound function
// Basically a wave without colours or offset
struct WaveCompound {
	float f, a;
	vec2 uv;
	float time;

/*	Commenting out member functions cuz they break on some machines

	// Create lerp for Wave::LerpCompound()
	float Compound() {
		return sin(uv.x * (f + sin(time))) * (a * cos(time));
	}
*/
};

// Create lerp for Wave_Lerp_Compound()
	float Compound(WaveCompound waveC) {
		return sin(waveC.uv.x * (waveC.f + sin(waveC.time))) * (waveC.a * cos(waveC.time));
	}



void main(){
	// Fix screen and remap UV coords
	vec2 uv = gl_FragCoord.xy/_Resolution;
	uv = uv * 2.0 - 1.0;
	float aspectRatio = _Resolution.x / _Resolution.y;
	uv.x *= aspectRatio;

	// Final output color
	vec3 color;

	// Sky
	Background bg = Background(bgClrTop, bgClrBtm, bgClrTop2, bgClrBtm2, uv, _Time);
	color = BG_Color_Advance(bg);

	// Sun
	Sun sun = Sun(sunSpeed, sunBlur, sunInner, sunClr, sunClr2, uv, _Time);

	// Wave 1
	Wave wave1 = Wave(wave1F, wave1A, wave1O, wave1ClrBtm, wave1ClrTop, uv, _Time); 

	// Wave 2. Compound Wave
	Wave wave2 = Wave(wave2F, wave2A, wave2O, wave2ClrBtm, wave1ClrTop, uv, _Time);
	WaveCompound w2C = WaveCompound(wave2CF, wave2CA, uv, _Time);

	// Mixing Colours
	color = mix(color, Sun_Color(sun), Sun_Lerp(sun));
	color = mix(Wave_Color(wave1), color, Wave_Lerp(wave1));
	color = mix(Wave_Color(wave2), color, Wave_Lerp_Compound(wave2, Compound(w2C)));

	// Output
	FragColor = vec4(color,1.0);
}