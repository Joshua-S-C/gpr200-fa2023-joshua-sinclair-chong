#version 450

in vec2 uv;
//in vec2 fragCoord;
out vec4 FragColor;

uniform vec2 _Resolution;
uniform vec3 _Color;
uniform float _Brightness;
uniform float _Time;

void main(){
    // Fix screen and remap UV coords
    vec2 uv = gl_FragCoord.xy/_Resolution;
    uv = uv * 2.0 - 1.0;
    float aspectRatio = _Resolution.x / _Resolution.y;
    uv.x *= aspectRatio;

    // BG
    vec3 bgClrTop = vec3(.4 + sin(_Time) * .2,0.0,0.3);
    vec3 bgClrBtm = vec3(0.0,1.0,1.0);
    vec3 color = mix(bgClrTop, bgClrBtm,uv.y + sin(_Time));

    // Sun
    float sunRadius = .2;
    float sunInner = .3;    
    float sunOutter = sunInner + sunRadius; 

    // Coords of circle origin, this needs to be a uniform
    float sunLerp = distance(uv, vec2(0.0, -.5 + sin(_Time))); 
    sunLerp = smoothstep(sunOutter, sunInner, sunLerp);
    vec3 sunClr = vec3(1.0,1.0,0.0); 

    // Mixing Colours
    color = mix(color, sunClr, sunLerp);

	//FragColor = vec4(UV,sin(_Time),1.0);
	FragColor = vec4(color * _Brightness,1.0);
}