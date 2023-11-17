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
uniform bool _Phong;

void main(){
	vec3 normal = normalize(fs_in.WorldNorm);

	// Ambient
	float ambient = _Material.ambientK;

	// Diffuse
	vec3 lightDir = normalize(_Light.pos - fs_in.WorldPos);
	float diffuse = _Material.diffuseK * max(dot(normal, lightDir), 0.0);
	
	// Specular
	float specular;
	vec3 viewDir = normalize(_ViewPos - fs_in.WorldPos);

	if (_Phong) {
		// Phong
		vec3 reflectVec = 2 * dot(lightDir, normal) * normal - lightDir;
		specular = _Material.specularK * pow(max(dot(reflectVec, normal), 0.0), _Material.shininess);
	} else {
		// Blin Phong
		vec3 halfVec = normalize(lightDir + viewDir);
		specular = _Material.specularK * pow(max(dot(halfVec, normal), 0.0), _Material.shininess);
	}

	vec3 result = (ambient + diffuse + specular) * _Light.clr;
	FragColor = texture(_Texture,fs_in.UV) * vec4(result, 1.0);
}