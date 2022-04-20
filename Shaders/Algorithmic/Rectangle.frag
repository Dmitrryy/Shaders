#version 330 core

out vec4 FragColor;

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

void main() {
    vec2 st = gl_FragCoord.xy/u_resolution.xy;
    vec3 color = vec3(0.0);

    // ---
    // red
    vec2 r1 = step(vec2(0.0, 0.9), st) * (1.0 - step(vec2(0.05, 1.0), st));
    color += vec3(r1.x * r1.y) * vec3(0.456, 0.0, 0.1);

    r1 = step(vec2(0.0, 0.8), st) * (1.0 - step(vec2(0.05, 0.89), st));
    color += vec3(r1.x * r1.y) * vec3(0.456, 0.0, 0.1);

    r1 = step(vec2(0.06, 0.9), st) * (1.0 - step(vec2(0.12, 1.0), st));
    color += vec3(r1.x * r1.y) * vec3(0.456, 0.0, 0.1);

    r1 = step(vec2(0.06, 0.8), st) * (1.0 - step(vec2(0.12, 0.89), st));
    color += vec3(r1.x * r1.y) * vec3(0.456, 0.0, 0.1);
    // ---
    
    // ---
    // white
    r1 = step(vec2(0.13, 0.3), st) * (1.0 - step(vec2(0.65, 0.79), st));
    color += vec3(r1.x * r1.y) * vec3(0.89, 0.87, 0.87);

    r1 = step(vec2(0.13, 0.9), st) * (1.0 - step(vec2(0.65, 1.0), st));
    color += vec3(r1.x * r1.y) * vec3(0.89, 0.87, 0.87);

    r1 = step(vec2(0.13, 0.8), st) * (1.0 - step(vec2(0.65, 0.89), st));
    color += vec3(r1.x * r1.y) * vec3(0.89, 0.87, 0.87);

    r1 = step(vec2(0.13, 0.0), st) * (1.0 - step(vec2(0.65, 0.29), st));
    color += vec3(r1.x * r1.y) * vec3(0.89, 0.87, 0.87);

    r1 = step(vec2(0.0, 0.0), st) * (1.0 - step(vec2(0.12, 0.79), st));
    color += vec3(r1.x * r1.y) * vec3(0.89, 0.87, 0.87);


    gl_FragColor = vec4(color,1.0);
}