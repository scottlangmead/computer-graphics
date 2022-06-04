#include <glm\glm.hpp>
#include <graphics_framework.h>
#include "main.h"
#include "render.h"

using namespace std;
using namespace graphics_framework;
using namespace glm;

// Render shadows in scene
void renderShadows(mat4 LightProjectionMat)
{
  // Set render target to shadow map
  renderer::set_render_target(shadow);
  // Clear depth buffer bit
  glClear(GL_DEPTH_BUFFER_BIT);
  // Set face cull mode to front
  glCullFace(GL_FRONT);

  // Bind shader
  renderer::bind(effects["shadows"]);

  // Loop through meshes
  for(auto &e : meshes)
  {
	auto m = e.second;
	// Create MVP matrix
	auto M = m.get_transform().get_transform_matrix();
	// View matrix taken from shadow map
	auto V = shadow.get_view();
	auto MVP = LightProjectionMat * V * M;
	// Set MVP matrix uniform
	glUniformMatrix4fv(effects["shadows"].get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
	// Render mesh
	renderer::render(m);
  }

  // Set render target back to the screen
  renderer::set_render_target();
  // Set face cull mode to back
  glCullFace(GL_BACK);
}

// Render meshes in scene
void renderMeshes(mat4 LightProjectionMat)
{
  // Bind shader
  renderer::bind(effects["main"]);

  // Loop through meshes
  for(auto &e : meshes)
  {
	auto m = e.second;
	// Create MVP matrix
	auto M = m.get_transform().get_transform_matrix();
	auto V = cam.get_view();
	auto P = cam.get_projection();
	auto MVP = P * V * M;
	// Set MVP matrix uniform
	glUniformMatrix4fv(effects["main"].get_uniform_location("MVP"),// Location of uniform
	  1,                                  // Number of values - 1 mat4
	  GL_FALSE,                           // Transpose the matrix?
	  value_ptr(MVP));                    // Pointer to matrix data
	// Set M matrix uniform
	glUniformMatrix4fv(effects["main"].get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
	// Set N matrix uniform
	glUniformMatrix3fv(effects["main"].get_uniform_location("N"), 1, GL_FALSE,
	  value_ptr(m.get_transform().get_normal_matrix()));
	// Set lightMVP uniform, using:
	// Viewmatrix from the shadow map
	auto lightV = shadow.get_view();
	// Multiply together with LightProjectionMat
	auto lightMVP = LightProjectionMat * lightV * M;
	// Set uniform
	glUniformMatrix4fv(effects["main"].get_uniform_location("lightMVP"),
	  1,
	  GL_FALSE,
	  value_ptr(lightMVP));
	// Bind material
	renderer::bind(m.get_material(), "mat");
	// Bind spot light
	renderer::bind(point, "point");
	// Bind texture
	renderer::bind(tex, 0);
	// Set tex uniform
	glUniform1i(effects["main"].get_uniform_location("tex"), 0);
	// Set eye position
	glUniform3fv(effects["main"].get_uniform_location("eye_pos"),
	  1,
	  value_ptr(cam.get_position()));
	// Bind shadow map texture, use texture unit 1
	renderer::bind(shadow.buffer->get_depth(), 1);
	// Set the shadow map uniform
	glUniform1i(effects["main"].get_uniform_location("shadow_map"), 1);
	// Render mesh
	renderer::render(m);
  }
}

// Render motion blur effect
void renderMotionBlur()
{
  // MVP identity matrix
  mat4 MVP = mat4(1.0);
  // Set render target to current frame
  renderer::set_render_target(frames[current_frame]);
  // Clear frame
  renderer::clear();
  // Bind motion blur effect
  renderer::bind(effects["motion_blur"]);
  // Set MVP matrix uniform
  glUniformMatrix4fv(effects["motion_blur"].get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
  // Bind tempframe to TU 0
  renderer::bind(temp_frame.get_frame(), 0);
  // Bind frames[(current_frame + 1) % 2] to TU 1
  renderer::bind(frames[(current_frame + 1) % 2].get_frame(), 1);
  // Set tex uniforms
  glUniform1i(effects["motion_blur"].get_uniform_location("tex"), 0);
  glUniform1i(effects["motion_blur"].get_uniform_location("previous_frame"), 1);
  // Set blend factor
  glUniform1f(effects["motion_blur"].get_uniform_location("blend_factor"), BLUR_FACTOR);
  // Render screen quad
  renderer::render(screen_quad);

  // Set render target back to the screen
  renderer::set_render_target();
  renderer::bind(effects["texture"]);
  // Set MVP matrix uniform
  glUniformMatrix4fv(effects["texture"].get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
  // Bind texture from frame buffer
  renderer::bind(frames[current_frame].get_frame(), 0);
  // Set the uniform
  glUniform1i(effects["texture"].get_uniform_location("tex"), 1);
  // Render the screen quad
  renderer::render(screen_quad);
}

// Render scene
bool render()
{
  // Set render target to temp frame
  renderer::set_render_target(temp_frame);
  // Clear frame
  renderer::clear();
  // Projection material with an FOV of 90
  mat4 LightProjectionMat = perspective<float>(90.0f, renderer::get_screen_aspect(), 0.1f, 1000.f);

  // Meshes
  renderMeshes(LightProjectionMat);
  // Shadows
  renderShadows(LightProjectionMat);
  // Motion blur
  renderMotionBlur();
  return true;
}