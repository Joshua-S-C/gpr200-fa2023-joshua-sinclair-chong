#version 450
out vec4 FragColor;

in Surface{
	vec2 UV;
	vec3 WorldPos;
	vec3 WorldNorm;
	vec3 WaveClr;
}fs_in;

//struct Light {
//	vec3 pos; // World Space
//	vec3 clr; // RBG
//};
//
//struct Material {
//	// K = Coefficients. 0-1.
//	float ambientK;
//	float diffuseK; 
//	float specularK;
//	float shininess;
//};
//
//#define MAX_LIGHTS 4
//uniform Light _Lights[MAX_LIGHTS];

uniform sampler2D _Texture;

uniform vec3 _ViewPos;

uniform bool _UseTexture;

void main(){
	vec3 normal = normalize(fs_in.WorldNorm);
	vec3 result = {0,0,0};

	if (_UseTexture) {
		FragColor = texture(_Texture,fs_in.UV);
	} else {
		float height = ((fs_in.WorldPos.y + 10) / 40);
		FragColor = vec4(0, height, 0, 1.0);
	}
}