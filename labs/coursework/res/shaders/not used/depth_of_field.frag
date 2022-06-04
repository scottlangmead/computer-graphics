#version 410

// Previous pass capture
uniform sampler2D tex;
// Initial render capture
uniform sampler2D sharp;
// Initial render depth capture
uniform sampler2D depth;

// The range value of the dof
uniform float range;
// The focus distance of the dof
uniform float focus;

// Incoming texture coordinate
layout(location = 0) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {
  // *********************************
  // Sample sharp texture
  vec4 s = texture(sharp, tex_coord);
  // Sample blur texture
  vec4 b = texture(tex, tex_coord);
  // Calculate distance from the camera - based on depth sample
  vec4 d = texture(depth, tex_coord);
  float factor = clamp(range * abs(focus - d.z), 0.0, 1.0);
  // Mix samples together based on distance
  colour = mix(s, b, factor);
  // Ensure alpha is 1.0
  colour.a = 1.0;
  // *********************************
}