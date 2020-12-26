#version 330 core

out vec4 o_frag_color;

in vec3 position_world;

uniform sampler2D u_tex;

void main() {
    vec3 texture = texture(u_tex, position_world.xy).rgb;
    o_frag_color = vec4(texture,1.0);
}


