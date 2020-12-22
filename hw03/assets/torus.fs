#version 330 core

in vec3 texture_coords;

uniform sampler2D tex;

void main() {
    vec4 color = texture(tex, texture_coords.xy);
    gl_FragColor = color;
}
