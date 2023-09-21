#version 450

in vec2 uv;
//in vec2 fragCoord;
out vec4 FragColor;

// Misc. Uniforms
uniform vec2 _Resolution;
uniform vec3 _Color;
uniform float _Brightness;
uniform float _Time;

// BG Colour Uniforms
uniform vec3 bgClrTop;
uniform vec3 bgClrBtm;

// Sun Uniforms
uniform float sunRadius;
uniform float sunInner;
uniform vec3 sunClr;

// Wave 1 Uniforms
uniform vec3 wave1ClrTop;
uniform vec3 wave1ClrBtm;

struct Sun {
	float radius, inner;
	vec2 uv;
	vec3 clr;
	float time;

	// Used as start of range for mix. This isn't needed lol
	vec3 Color() {
		return clr;
	}

	// Used as interpolation value for mix
	float Lerp() {
		float sunLerp = distance(uv, vec2(0.0, -.5 + sin(_Time))); 
		sunLerp = smoothstep(inner + radius, inner, sunLerp);
		return sunLerp;
	}
};

struct Wave {
	float f, a, offset;
	vec2 uv;
	vec3 clrBtm, clrTop;

	// Used as start of range for mix
	vec3 FGColor() {
		return mix(clrTop, clrBtm, -uv.y);
	}

	// Used as interpolation value for mix
	float Lerp() {
		float waveLerp = offset + sin(uv.x * f + _Time) * a;
		waveLerp = step(waveLerp,uv.y);
		return waveLerp;
	}
};


void main(){
	// Fix screen and remap UV coords
	vec2 uv = gl_FragCoord.xy/_Resolution;
	uv = uv * 2.0 - 1.0;
	float aspectRatio = _Resolution.x / _Resolution.y;
	uv.x *= aspectRatio;

	// BG
	vec3 color = mix(bgClrBtm, bgClrTop, uv.y + sin(_Time));

	// Sun
	Sun sun = Sun(sunRadius, sunInner, uv, sunClr, _Time);

	// Currrently the refactored way doesn't work. Something wrong with sun.Lerp()
	// I think Time is not updating or someting cuz the sun stays in one place
	// Yea that was it
	//float sunRadius = .2;
	//float sunInner = .3;    
	//float sunOutter = sunInner + sunRadius; 
	// Coords of circle origin, this needs to be a uniform
	float sunLerp = distance(uv, vec2(0.0, -.5 + sin(_Time))); 
	sunLerp = smoothstep(sunInner + sunRadius, sunInner, sunLerp);
	//vec3 sunClr = vec3(1.0,1.0,0.0); 


	// Wave 1
	Wave wave1 = Wave(3.0, 0.2 * sin(_Time), -0.2, uv, wave1ClrBtm, wave1ClrTop); 

	// Mixing Colours
	color = mix(color, sun.clr, sun.Lerp());
	//color = mix(color, sun.clr, sunLerp);
	//color = mix(color, sunClr, sunLerp);
	color = mix(wave1.FGColor(), color, wave1.Lerp());

	// Output
	FragColor = vec4(color,1.0);
}