#version 450
out vec4 FragColor;
in vec2 UV;

uniform sampler2D _Texture;
uniform sampler2D _Texture2;

void main(){
	FragColor = mix(texture(_Texture,UV), texture(_Texture2, UV), 0.2);
}