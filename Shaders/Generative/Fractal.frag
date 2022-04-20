#version 330 core

out vec4 FragColor;

#define PI 3.14159265359

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;


void main() {
    vec2 norm_coordinates = gl_FragCoord.xy / u_resolution;
    vec2 c = (norm_coordinates - vec2(0.5)) * 0.03 - vec2(0.76, 0.1);

    vec2 z = vec2(0.0, 0.0);
    float i;
    for (i = 0.0; i < 1.0; i += 0.005) {
        z = vec2(
            z.x * z.x - z.y * z.y + c.x,
            z.y * z.x + z.x * z.y + c.y
        );

        if (length(z) > 4.0) {
            break;
        }
    }

    vec4 to_write = vec4(vec3(i), 1.0);
    gl_FragColor = to_write;
}
			
