#version 330 core

out vec4 FragColor;

#define PI 3.14159265359

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;


float random (vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233)))*
    43758.5453123);
}

float random(float x) {
    return fract(sin(x)*43758.5453123);
}

void main() {
    vec2 st = gl_FragCoord.xy/u_resolution.xy;

    float rnd = pow(random( st * (mod(u_time, 100.0) + 1) ), 5.);
    vec3 color = vec3(rnd);

    gl_FragColor = vec4(color, 1.0);
}
