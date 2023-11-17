#version 450
out vec4 FragColor;

in Surface{
	vec2 UV;
	vec3 WorldPos;
	vec3 WorldNorm;
}fs_in;

struct Light {
	vec3 pos; // World Space
	vec3 clr; // RBG
};


struct Material {
	// K = Coefficients. 0-1.
	float ambientK;
	float diffuseK; 
	float specularK;
	float shininess;
};

//#define MAX_LIGHTS 4
//uniform Light _Lights[MAX_LIGHTS];

uniform Light _Light;
uniform Material _Material;
uniform sampler2D _Texture;
uniform vec3 _ViewPos;

void main(){
	vec3 normal = normalize(fs_in.WorldNorm);

	// Ambient
	float ambient = _Material.ambientK;

	// Diffuse
	vec3 lightDir = normalize(_Light.pos - fs_in.WorldPos);
	float diffuse = _Material.diffuseK * max(dot(normal, lightDir), 0.0);
	
	// Specular
	vec3 viewDir = normalize(_ViewPos - fs_in.WorldPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float specular = _Material.specularK * pow(max(dot(viewDir, reflectDir), 0.0), _Material.shininess);
	
	vec3 result = (diffuse) * _Light.clr;
	FragColor = texture(_Texture,fs_in.UV) * vec4(result, 1.0);
}