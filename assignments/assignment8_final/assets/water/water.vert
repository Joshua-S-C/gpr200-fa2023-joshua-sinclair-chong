#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;

out Surface{
	vec2 UV;
	vec3 WorldPos;
	vec3 WorldNorm;
	vec3 WaveClr;
}vs_out;

uniform float _Time;

uniform mat4 _Model;
uniform mat4 _ViewProjection;
uniform vec3 _WorldNorm;

struct Wave {
	float f, a, s;
	vec3 clr;
};

uniform Wave _wave1;


void main(){
	vs_out.UV = vUV;
	vs_out.WaveClr = _wave1.clr;
	vs_out.WorldPos = vec3(_Model  * vec4(vPos, 1.0));
	
	float waveLength = 2 * _wave1.f; // frequency = 2 / wavelength
	float k = 2 * radians(180) / waveLength;
	float f = k * (vPos.x - _Time * _wave1.s);

	// Normal Calc ----------------------------------------------------------*/
	vec3 tangent = normalize(vec3(
		1, 
		k * _wave1.a * cos(f), 
		0));
	vs_out.WorldNorm = vec3(-tangent.y, tangent.x, 0);

	// Undulation -----------------------------------------------------------*/
	
	//vec3 undulate;
	//vPos.y = _wave1.a * sin(k * vPos.x);
	//undulate.y = _wave1.a * sin(_wave1.f * (vs_out.WorldPos.x - _Time * _wave1.s));
	//undulate.x = _wave1.a * cos(_wave1.f * (vs_out.WorldPos.x - _Time * _wave1.s));

	vec3 _vPos = vec3(vs_out.WorldPos.x, 
	_wave1.a * sin(f), 
	vs_out.WorldPos.z);


	gl_Position = _ViewProjection * _Model * vec4(_vPos,1.0);
}