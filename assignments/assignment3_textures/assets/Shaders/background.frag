#version 450
out vec4 FragColor;
in vec2 UV;

// Uniforms
uniform float _Time;
uniform float _DistortionStrength;
uniform float _DistortionSpeed;
uniform float _Text2Alpha;

uniform vec3 _NoiseColour;
uniform float _NoiseAlpha;

// Textures
uniform sampler2D _Texture;
uniform sampler2D _Texture2;
uniform sampler2D _Texture3;


void main(){
	float noise = texture(_Texture3, UV + _Time * _DistortionSpeed).r;
	vec2 uv = UV + noise * (_DistortionStrength);
	vec4 colorA = texture(_Texture, uv);
	vec4 colorB = texture(_Texture2, uv);

	//FragColor = texture(_Texture3,UV);
	//FragColor = mix(texture(_Texture,UV), texture(_Texture2, UV), 0.3);
	//FragColor = mix(FragColor, texture(_Texture3, UV), 0.5);

	vec3 color = mix(colorA.rgb, colorB.rbg, colorB.a * _Text2Alpha);
	color = mix(color, _NoiseColour, noise * _NoiseAlpha);

	//vec3 color = vec3(noise, 0.0, 0.0);	// Renders distortion
	FragColor = vec4(color, 1.0f);
	//FragColor = vec4(mix(vec3(0.0, 0.0, 0.0), _NoiseColour, noise * _NoiseAlpha), 1.0f);
}