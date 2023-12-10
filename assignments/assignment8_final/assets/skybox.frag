#version 450
out vec4 FragColor;

in vec3 texCoords;

uniform samplerCube _Texture;


void main(){
	FragColor = texture(_Texture,texCoords);
}
