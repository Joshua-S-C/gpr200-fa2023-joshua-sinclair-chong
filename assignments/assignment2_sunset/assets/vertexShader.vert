#version 450

out vec2 uv;

layout(location = 0) in vec3 _pos;
layout(location = 1) in vec2 _uv;

void main(){
	uv = _uv;
	gl_Position = vec4(_pos,1.0);
}