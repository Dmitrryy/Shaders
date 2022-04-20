#version 330 core

out vec4 FragColor;

#define PI 3.14159265359

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;


void main() {
    vec2 st = gl_FragCoord.xy / u_resolution;

	vec3 resColor;
	float z = u_time;

    // original code
	// for(int i = 0; i < 3; i++) {
	// 	vec2 uv = st; 
    //     vec2 p = st - 0.5;
	
	// 	p.x *= u_resolution.x / u_resolution.y;
	// 	//z += 0.07;
	// 	float l = length(p);
	// 	uv += p / l * (sin(z) + 1.0) * abs(sin(l * 9.0 - z*2.0));
	// 	resColor[i] = 0.01 / length(mod(uv, 1.0) - 0.5);
	// }

    // shifting the zero coordinates to the center
    vec2 p = st - 0.5;

    // removes dependence on the permission of the parties.
    // the picture ceases to be flattened
    p.x *= u_resolution.x / u_resolution.y;

    float l = length(p);
    p = normalize(p);

    // st - is just a circle in the center
    // (sin(z) + 1.0) - adds the effect of disappearing and changing the image
    vec2 uv = st + p / l * 0.5 * (sin(z) + 1.0) * abs(sin(l * 9.0 - z * 2.0));
	resColor = vec3(0.01 / length(mod(uv, 1.0) - 0.5)) + vec3(0.0, 0.07, 0.14);

	gl_FragColor=vec4(resColor / l, u_time);
}