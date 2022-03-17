#version 440

// Model view projection matrix
uniform mat4 MVP;

// Declare incoming values
layout (location = 0) in vec3 position;
layout (location = 10) in vec2 tex_coord_in;

// Outgoing value
layout (location = 0) out vec2 tex_coord_out;
layout (location = 10) out float height;

void main()
{
  // Pass through vertex height to fragment
  height = position.y;

  // Calculate screen position of vertex
  gl_Position = MVP * vec4(position, 1.0);

  // Output texture coordinate to fragement shader
  tex_coord_out = tex_coord_in;
} 