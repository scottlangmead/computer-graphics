#include <glm\glm.hpp>
#include <graphics_framework.h>
#include "main.h"
#include "render.h"

using namespace std;
using namespace graphics_framework;
using namespace glm;

// Renders a single mesh
void renderMesh(mesh m)
{
	// Bind effect
	renderer::bind(eff);

	// Create MVP matrix
	auto M = m.get_transform().get_transform_matrix();
	auto V = cam.get_view();
	auto P = cam.get_projection();
	auto MVP = P * V * M;

	// Set MVP matrix uniform
	glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

	// Create MV matrix
	auto MV = V * M;

	// Set MV, M and N matrix uniforms
	glUniformMatrix4fv(eff.get_uniform_location("MV"), 1, GL_FALSE, value_ptr(MV));
	glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
	glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(m.get_transform().get_normal_matrix()));

	// Bind material
	renderer::bind(m.get_material(), "mat");

	// Bind light
	renderer::bind(light, "light");

	// Bind texture
	renderer::bind(tex, 0);

	// Set texture uniform
	glUniform1i(eff.get_uniform_location("tex"), 0);

	// Set eye position
	glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(cam.get_position()));

	// Render mesh
	renderer::render(m);
}