#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;

out Surface{
	vec2 UV;
	vec3 WorldPos;
	vec3 WorldNorm;
}vs_out;

uniform mat4 _Model;
uniform mat4 _ViewProjection;
uniform vec3 _WorldNorm;

void main(){
	vs_out.UV = vUV;
	vs_out.WorldPos = vec3(_Model  * vec4(vPos, 1.0));
	vs_out.WorldNorm = transpose(inverse(mat3(_Model))) * vNormal;

	gl_Position = _ViewProjection * _Model * vec4(vPos,1.0);
}