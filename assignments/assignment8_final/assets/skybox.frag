#version 450
out vec4 FragColor;

in vec3 texCoords;

uniform samplerCube _Texture;

//in Surface{
//	vec2 UV;
//	vec3 WorldPos;
//	vec3 WorldNorm;
//}fs_in;

void main(){
	FragColor = texture(_Texture,texCoords);
}
