#version 410

// Cubemap texture
uniform samplerCube cubemap;

// Incoming texture coordinate
layout(location = 0) in vec3 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main()
{
  colour = texture(cubemap, tex_coord);
}