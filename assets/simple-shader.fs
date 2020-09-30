#version 330 core

out vec4 o_frag_color;

uniform int steps;
uniform vec2 mouse;
uniform float zoom;
uniform sampler1D u_tex;

float mandelbrot(vec2 c) {
  vec2 z = c;
  for (float i = 0.0; i < steps; i++) {
    if (abs(dot(z, z)) > 4.0) {
      return (i / steps);
    }
    z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;
  }
  return 1.0;
}

void main() {
  vec2 Position = vec2(gl_FragCoord.xy);
  float result = mandelbrot((Position - vec2(640.0, 360.0)) / zoom - mouse);
  o_frag_color = texture(u_tex, result);
}
