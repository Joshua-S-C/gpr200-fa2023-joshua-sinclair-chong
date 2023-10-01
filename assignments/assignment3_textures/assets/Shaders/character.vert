#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vUV;
out vec2 UV;

// Uniforms
uniform float _Time;
uniform float _Scale;
uniform float _Speed;
uniform float _DistanceX;
uniform float _DistanceY;

void main(){
	UV = vUV;
	vec3 offset = vec3(vPos.x + _DistanceX * cos(_Time),abs(sin(_Time)) - _Scale/2,0);
	gl_Position = vec4((vPos + offset) * _Scale,1.0);
	//gl_Position = vec4(vPos,1.0);	// Default
}