#version 440

vec4 weighted_texture(in sampler2D tex[4], in vec2 tex_coord, in vec4 weights) {
  vec4 tex_colour = vec4(0, 0, 0, 1);
  // *********************************
  // Sample all Five textures based on weight
  tex_colour += texture(tex[0], tex_coord) * weights.x;
  tex_colour += texture(tex[1], tex_coord) * weights.y;
  tex_colour += texture(tex[2], tex_coord) * weights.z;
  tex_colour += texture(tex[3], tex_coord) * weights.a;
  tex_colour.a = 1;
  // *********************************
  return tex_colour;
}