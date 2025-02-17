#version 440

// MVP transformation
uniform mat4 MVP;

// Layout of incoming data
layout(triangles) in;
// Layout of outgoing data
layout(line_strip, max_vertices = 6) out;

// Incoming normals for vertexs
layout(location = 0) flat in vec3 normal[];

void main() {
  // Calculate for each vertex
  for (int i = 0; i < 3; ++i) {
    // *********************************
    // Get vertex pos
    vec3 vertex_pos = (gl_in[i].gl_Position).xyz;
    // Ensure normal is normalized
    vec3 n = normal[i];
    n = normalize(n);
    // Output normal position for start of line
    // - remember to transform
    gl_Position = MVP * vec4(vertex_pos, 1.0);
    // Emit
    EmitVertex();
    // Output position + normal for end of line
    // - remember to transform
    gl_Position = MVP * vec4((vertex_pos + n), 1.0);
    // Emit
    EmitVertex();
    // End the primitive
    EndPrimitive();
    // *********************************
  }
}