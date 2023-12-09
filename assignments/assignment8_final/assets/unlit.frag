#version 450
out vec4 FragColor;

/* Shading Modes ---------------------------------------------------------
0 : uniform color 
1 : normals
2 : UVs																	*/
uniform int _Mode;
uniform vec3 _UnshadedColor;
uniform sampler2D _Texture;


in Surface{
	vec2 UV;
	vec3 WorldPos;
	vec3 WorldNorm;
}fs_in;

void main(){
	// Special Shading Modes ------------------------------------------------*/
	switch (_Mode) {
		case 0: 
			FragColor = vec4(_UnshadedColor,1.0);
			return;
		case 1:
			vec3 normal = normalize(fs_in.WorldNorm);
			FragColor = vec4(abs(normal),1.0);
			return;
		case 2:
			FragColor = vec4(fs_in.UV,0.0,1.0);
			return;
		case 3: 
			FragColor = texture(_Texture,fs_in.UV);
			return;
		default:
			break;
	}
}
