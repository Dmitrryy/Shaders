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

    // two rows
    st.y *= 2.0;
    
    float seed = random(floor(u_time / 3));
    float speed = seed * 8;
    float offset = sign(st.y - 1.) + (u_time - seed) / u_time / speed;
    vec2 uv = vec2(st.x + offset, st.y);

    float rnd = step(random(uv.x * seed), 0.05) * step(abs(random(uv.x)), abs(uv.x)) 
        * (1 - step(abs(random(uv.x) + random(uv.x) * 10), abs(uv.x)));

    vec3 color = vec3(mod(rnd + 1.0, 2.0));
    
    //color = vec3(fract(st * 10), 0.8);

    gl_FragColor = vec4(color, 1.0);
}
