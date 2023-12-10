#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;

out Surface{
	vec2 UV;
	vec3 WorldPos;
	vec3 WorldNorm;
	vec3 WaveClr;
}vs_out;

uniform float _Time;
uniform mat4 _Model;
uniform mat4 _ViewProjection;
uniform vec3 _WorldNorm;

struct Wave {
	float l, s;
	vec2 dir;
	vec3 clr;
};

uniform Wave _wave;
uniform Wave _waves[10];

vec3 gerstner(in Wave wave, in vec3 vPos, inout vec3 tangent, inout vec3 binormal) {
	// Used in Calcs --------------------------------------------------------*/
	float k = 2 * radians(180) / wave.l;				// Wave Number
	float c = sqrt(9.8 / k);							// Speed
	vec2 d = normalize(wave.dir);						// Direction normalized
	float f = k * (dot(d, vPos.xz) - _Time * _wave.s);	// Frequency
	float a = _wave.s / k;								// Amplitude

	// Normal Calc ----------------------------------------------------------*/
	tangent += vec3(
		1 - d.x * d.x * (_wave.s * sin(f)), 
		d.x * (_wave.s * cos(f)), 
		-d.x * d.y * (_wave.s * sin(f))
		);

	binormal += vec3(
		-d.x * d.y * (_wave.s * sin(f)),
		d.y * (_wave.s * cos(f)),
		1 - d.y * d.y * (_wave.s * sin(f))
		);

	// Undulation -----------------------------------------------------------*/
	return vec3(
		d.x * (a * cos(f)), 
		a * sin(f), 
		d.y * (a * (cos(f)))
		);
}

void main(){
	vs_out.UV = vUV;
	vs_out.WaveClr = _wave.clr;
	vs_out.WorldPos = vec3(_Model  * vec4(vPos, 1.0));
	
	{
	
	// Used in Calcs --------------------------------------------------------*/
//	float k = 2 * radians(180) / _wave.l;				// Wave Number
//	float c = sqrt(9.8 / k);							// Speed
//	vec2 d = normalize(_wave.dir);						// Direction normalized
//	float f = k * (dot(d, vPos.xz) - _Time * _wave.s);	// Frequency
//	float a = _wave.s / k;								// Amplitude
//
	// Normal Calc ----------------------------------------------------------*/
//	vec3 tangent = vec3(
//		1 - d.x * d.x * (_wave.s * sin(f)), 
//		d.x * (_wave.s * cos(f)), 
//		-d.x * d.y * (_wave.s * sin(f))
//		);
//
//	vec3 binormal = vec3(
//		-d.x * d.y * (_wave.s * sin(f)),
//		d.y * (_wave.s * cos(f)),
//		1 - d.y * d.y * (_wave.s * sin(f))
//		);
//
//	vs_out.WorldNorm = normalize(cross(binormal, tangent));
//	
	}
	// Undulation -----------------------------------------------------------*/

	vec3 tangent = {1, 0, 0};
	vec3 binormal = {0, 0, 1};
	vec3 undulate = {0,0,0};

	undulate += gerstner(_waves[0], vPos, tangent, binormal);


//	for(int i = 0; i < 5; i++){
//		undulate += gerstner(_waves[i], vPos, tangent, binormal);
//		}


	vs_out.WorldNorm = normalize(cross(binormal, tangent));
	vec3 _vPos = vec3(
		vs_out.WorldPos.x + undulate.x, 
		undulate.y, 
		vs_out.WorldPos.z + undulate.z
		);

	gl_Position = _ViewProjection * _Model * vec4(_vPos,1.0);
}