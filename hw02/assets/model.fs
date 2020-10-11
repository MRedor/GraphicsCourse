#version 330 core

out vec4 o_frag_color;

in vec3 position_world;
in vec3 normal;

uniform samplerCube u_skybox;
uniform vec3 u_camera;
uniform float u_refraction;

void main()
{
  vec3 N = normalize(normal);
  vec3 V = normalize(abs(position_world - u_camera));

  float ratio = 1.0 / u_refraction;
  vec3 refl_v = reflect(V, N);
  vec3 refr_v = refract(V, N, ratio);

  vec3 refl_c = texture(u_skybox, refl_v).rgb;
  vec3 refr_c = texture(u_skybox, refr_v).rgb;

  float t = dot(refr_v, -N);
  float i = dot(-V, N);
  float A = ((ratio * i - t) / (ratio * i + t)) * ((ratio * i - t) / (ratio * i + t));
  float B = ((ratio * t - i) / (ratio * t + i)) * ((ratio * t - i) / (ratio * t + i));
  float C = (A + B) / 2;

  vec4 o_frag_color_1 = vec4(refl_c * C + (1-C) * refr_c, 1.0);
  o_frag_color = vec4(refl_c * C + (1-C) * refr_c, 1.0);//vec4(texture(u_skybox, refl_v).rgb, 1.0);
}


