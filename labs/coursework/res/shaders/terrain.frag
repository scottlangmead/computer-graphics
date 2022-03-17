#version 440

// Main textures
uniform sampler2D tex[2];

// Incoming texture coordinate
layout(location = 0) in vec2 tex_coord;
// Incoming vertex height
layout (location = 10) in float height;

// Outgoing fragment colour
layout(location = 0) out vec4 colour;

void main()
{
	// Sample two main textures
	vec4 c1 = texture(tex[0], tex_coord);
	vec4 c2 = texture(tex[1], tex_coord);
	// Mix based on the vertex height
	colour = mix(c1, c2, (height / 32));
}