#version 450
out vec4 FragColor;
in vec2 UV;
in vec3 Normal;


void main(){
	FragColor = vec4(abs(Normal),1.0);
}