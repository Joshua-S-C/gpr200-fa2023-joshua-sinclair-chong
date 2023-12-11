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
	float l, a, s;
	vec3 clr;
};

uniform Wave _wave;


void main(){
	vs_out.UV = vUV;
	vs_out.WaveClr = _wave.clr;
	vs_out.WorldPos = vec3(_Model  * vec4(vPos, 1.0));
	
	float k = 2 * radians(180) / _wave.l;		// Wave Number
	float f = k * (vPos.x - _Time * _wave.s);

	// Normal Calc ----------------------------------------------------------*/
	vec3 tangent = normalize(vec3(
		1, 
		k * _wave.a * cos(f), 
		0));
	vs_out.WorldNorm = vec3(-tangent.y, tangent.x, 0);

	// Undulation -----------------------------------------------------------*/
	vec3 _vPos = vec3(vs_out.WorldPos.x, 
	_wave.a * sin(f), 
	vs_out.WorldPos.z);


	gl_Position = _ViewProjection * _Model * vec4(_vPos,1.0);
}