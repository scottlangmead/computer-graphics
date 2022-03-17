#version 410

// MVP transformation matrix
uniform mat4 MVP;

// Declare incoming values
layout(location = 0) in vec3 position;

// Outgoing value
layout(location = 0) out vec3 tex_coord;

void main()
{
  // Calculate screen position of vertex
  gl_Position = MVP * vec4(position, 1.0);

  // Output texture coordinate to fragement shader
  tex_coord = position;
}