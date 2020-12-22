#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 tex_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 norm;
out vec3 texture_coords;

void main() {
    texture_coords = tex_coords;
    gl_Position = projection * view * model * vec4(position, 1.0);
}
