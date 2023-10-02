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
uniform sampler2D _Texture;		// Background
uniform sampler2D _Texture2;	// Object
uniform sampler2D _Texture3;	// Noise


void main(){
	float noise = texture(_Texture3, UV + _Time * _DistortionSpeed).r;
	vec2 uv = UV + noise * (_DistortionStrength);

	vec4 color1 = texture(_Texture, uv);
	vec4 color2 = texture(_Texture2, uv);
	vec3 color = mix(color1.rgb, color2.rbg, color2.a * _Text2Alpha);
	color = mix(color, _NoiseColour, noise * _NoiseAlpha);

	FragColor = vec4(color, 1.0f);
}

	//vec3 color = vec3(noise, 0.0, 0.0);	// Renders distortion
	//FragColor = texture(_Texture3,UV);
	//FragColor = mix(texture(_Texture,UV), texture(_Texture2, UV), 0.3);
	//FragColor = mix(FragColor, texture(_Texture3, UV), 0.5);
	//FragColor = vec4(mix(vec3(0.0, 0.0, 0.0), _NoiseColour, noise * _NoiseAlpha), 1.0f);