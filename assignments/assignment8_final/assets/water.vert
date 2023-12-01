#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;

out Surface{
	vec2 UV;
	vec3 WorldPos;
	vec3 WorldNorm;
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
	vs_out.WorldPos = vec3(_Model  * vec4(vPos, 1.0));

//	vs_out.WorldPos += 
//	vec3(vs_out.WorldPos.x, 
//	vs_out.WorldPos.y + sin(vs_out.WorldPos.x + _Time), 
//	vs_out.WorldPos.z + sin(vs_out.WorldPos.z + _Time));

	vs_out.WorldNorm = transpose(inverse(mat3(_Model))) * vNormal;

	// Undulation
	float yUndulate = _wave1.a * sin(_wave1.f * (vs_out.WorldPos.x - _wave1.s * _Time));

	vec3 _vPos = vec3(vs_out.WorldPos.x, 
	vs_out.WorldPos.y + yUndulate, 
	vs_out.WorldPos.z);


	gl_Position = _ViewProjection * _Model * vec4(_vPos,1.0);
}