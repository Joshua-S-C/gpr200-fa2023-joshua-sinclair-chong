#version 450
out vec4 FragColor;

in Surface{
	vec2 UV;
	vec3 WorldPos;
	vec3 WorldNorm;
	vec3 WaveClr;
}fs_in;

uniform float alpha;
uniform float blend;

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

#define MAX_LIGHTS 4
uniform Light _Lights[MAX_LIGHTS];

uniform Material _Material;

uniform vec3 _ViewPos;
uniform bool _Phong;
uniform int _NumLights;

void main(){
	// Lit Shading ----------------------------------------------------------*/
	vec3 normal = normalize(fs_in.WorldNorm);
	vec3 result = {0,0,0};

	for (int i = 0; i < _NumLights; i++) {
		// Ambient
		float ambient = _Material.ambientK;

		// Diffuse
		vec3 lightDir = normalize(_Lights[i].pos - fs_in.WorldPos);
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

		result += (ambient + diffuse + specular) * _Lights[i].clr;
	}

	FragColor = vec4(fs_in.WaveClr, alpha) * vec4(result, blend);
	//FragColor = texture(_Texture,fs_in.UV) * vec4(result, 1.0);
}