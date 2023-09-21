#version 450

out vec2 uv;
//out vec2 fragCoord;

layout(location = 0) in vec3 _pos;
layout(location = 1) in vec2 _uv;
layout(location = 2) in vec3 _Color;
layout(location = 3) in float _Time;

void main(){
	uv = _uv;
//	fragCoord = gl_FragCoord.xy;
	vec3 offset = vec3(cos(_pos.y + _Time * 2), sin(_pos.x + _Time), 0) * .5;
	gl_Position = vec4(_pos,1.0);
}