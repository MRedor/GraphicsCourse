#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;

out vec3 position_world;
out vec3 normal;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform mat4 u_mvp;

void main()
{
  gl_Position = u_mvp * vec4(in_position, 1.0);
  position_world = (u_model * vec4(in_position, 1.0)).xyz;
  normal = mat3(transpose(inverse(u_model))) * in_normal;
}
