#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vUV;
out vec2 UV;

// Uniforms
uniform float _Time;
uniform vec2 _Scale;
uniform float _ScaleRatio;
uniform float _Speed;
uniform vec2 _Distance;
uniform float _VertOffset;

void main(){
	UV = vUV;
	
	vec3 offset = vec3(
		vPos.x + (_Distance.x * cos(_Time * _Speed)),
		abs(_Distance.y * sin(_Time * _Speed)), 
		0);

	offset += vPos;

	gl_Position = vec4(
		vec2(offset.x * _Scale.x * _ScaleRatio,
		(offset.y + _VertOffset) * _Scale.y * _ScaleRatio), 
		1.0, 1.0);
}

	//gl_Position = vec4(vPos,1.0);	// Default

/*
Scale	Offset
.25		-3
.5		-1
.75		.33?
1		0
*/