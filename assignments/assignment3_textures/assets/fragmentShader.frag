#version 450
out vec4 FragColor;
in vec2 UV;

uniform sampler2D _BrickTexture;
uniform sampler2D _NoiseTexture;

void main(){
	FragColor = texture(_BrickTexture,UV);
}