#version 450

in vec2 uv;
//in vec2 fragCoord;
out vec4 FragColor;

// Misc. Uniforms
uniform float _Time;
uniform vec2 _Resolution;

// BG Colour Uniforms
uniform vec3 bgClrTop;
uniform vec3 bgClrBtm;
uniform vec3 bgClrTop2;	// Goal is to have the second colours blend with the first colours based on time, and then have those 2 new colors be blended as the bg
uniform vec3 bgClrBtm2;

// Sun Uniforms
uniform float sunRadius;
uniform float sunInner;
uniform vec3 sunClr;

// Wave 1 Uniforms
uniform vec3 wave1ClrTop;
uniform vec3 wave1ClrBtm;
uniform float wave1F;
uniform float wave1A;
uniform float wave1O;

struct Background {
	vec2 uv;
	vec3 clrTop1, clrBtm1, clrTop2, clrBtm2;
	float time;

	vec3 Color() {
		vec3 clr = mix(clrBtm1, clrTop1, uv.y + sin(time));
		return clr;
	}

	vec3 ColorAdvance() {
		vec3 clr1 = mix(clrBtm1, clrBtm2, sin(time));
		vec3 clr2 = mix(clrTop2, clrTop2, sin(time));
		vec3 clr = mix(clr1, clr2, uv.y + sin(time));
		return clr;
	}
};

struct Sun {
	float radius, inner;
	vec2 uv;
	vec3 clr;
	float time;	//This struct can't access _Time?

	// Used as start of range for mix. This isn't needed lol
	vec3 Color() {
		return clr;
	}

	// Used as interpolation value for mix
	float Lerp() {
		float sunLerp = distance(uv, vec2(0.0, -.5 + sin(time))); 
		sunLerp = smoothstep(inner + radius, inner, sunLerp);
		return sunLerp;
	}
};

struct Wave {
	float f, a, offset;
	vec2 uv;
	vec3 clrBtm, clrTop;
	float time; //Yea _Time doesn't work

	// Used as start of range for mix.
	vec3 FGColor() {
		return mix(clrTop, clrBtm, -uv.y);
	}

	// Used as interpolation value for mix
	float Lerp() {
		float waveLerp = offset + sin(uv.x * f + time) * a;
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
	Background bg = Background(uv, bgClrTop, bgClrBtm, bgClrTop2, bgClrBtm2, _Time);
	vec3 color = bg.Color();
	//vec3 color = bg.ColorAdvance();
	//vec3 color = mix(bgClrBtm, bgClrTop, uv.y + sin(_Time)); //Replaced by bg.Color()

	// Sun
	Sun sun = Sun(sunRadius, sunInner, uv, sunClr, _Time);

	//float sunRadius = .2;
	//float sunInner = .3;    
	//float sunOutter = sunInner + sunRadius; 
	// Replaced by all of Sun struct
	// Coords of circle origin, this needs to be a uniform
	float sunLerp = distance(uv, vec2(0.0, -.5 + sin(_Time))); 
	sunLerp = smoothstep(sunInner + sunRadius, sunInner, sunLerp);
	//vec3 sunClr = vec3(1.0,1.0,0.0); 


	// Wave 1
	//Wave wave1 = Wave(3.0, 0.2 * sin(_Time), -0.2, uv, wave1ClrBtm, wave1ClrTop); 
	Wave wave1 = Wave(wave1F, wave1A * sin(_Time), wave1O, uv, wave1ClrBtm, wave1ClrTop, _Time); 

	// Mixing Colours
	color = mix(color, sun.clr, sun.Lerp());
	//color = mix(color, sun.clr, sunLerp);
	//color = mix(color, sunClr, sunLerp);
	color = mix(wave1.FGColor(), color, wave1.Lerp());

	// Output
	FragColor = vec4(color,1.0);
}