// How should I organize this? Im thinking structs / classes

#version 450

in vec2 uv;
//in vec2 fragCoord;
out vec4 FragColor;

uniform vec2 _Resolution;
uniform vec3 _Color;
uniform float _Brightness;
uniform float _Time;

// BG Colour Uniforms
uniform vec3 bgClrTop;
uniform vec3 bgClrBtm;

// Wave 1 Uniforms
uniform vec3 wave1ClrTop;
uniform vec3 wave1ClrBtm;

float createWave(float f, float a, float offset, vec2 uv) {
	float waveLerp = offset + sin(uv.x * f + _Time) * a;
	waveLerp = step(waveLerp,uv.y);
	return waveLerp;
}

struct Wave {
	float frequency, amplitude, offset;
	vec2 uv;
};

void main(){
	// Fix screen and remap UV coords
	vec2 uv = gl_FragCoord.xy/_Resolution;
	uv = uv * 2.0 - 1.0;
	float aspectRatio = _Resolution.x / _Resolution.y;
	uv.x *= aspectRatio;

	// BG
	//vec3 bgClrTop = vec3(.4 + sin(_Time) * .2,0.0,0.3);
	//vec3 bgClrBtm = vec3(0.0,1.0,1.0);
	vec3 color = mix(bgClrBtm, bgClrTop, uv.y + sin(_Time));

	// Sun
	float sunRadius = .2;
	float sunInner = .3;    
	float sunOutter = sunInner + sunRadius; 

	// Coords of circle origin, this needs to be a uniform
	float sunLerp = distance(uv, vec2(0.0, -.5 + sin(_Time))); 
	sunLerp = smoothstep(sunOutter, sunInner, sunLerp);
	vec3 sunClr = vec3(1.0,1.0,0.0); 

	// Wave 1
	//vec3 wave1ClrTop;
	//vec3 wave1ClrTop = vec3(0.2,0.50,0.80);
	//vec3 wave1ClrBtm = vec3(0.30,0.65,1.00);
	float wave1Lerp = createWave(3.0, 0.2 * sin(_Time), -0.2, uv);
	vec3 wave1FgClr = mix(wave1ClrTop, wave1ClrBtm, -uv.y); 

	// Mixing Colours
	color = mix(color, sunClr, sunLerp);
	color = mix(wave1FgClr, color, wave1Lerp);

	// Output
	FragColor = vec4(color,1.0);
}