#include <glm\glm.hpp>
#include <graphics_framework.h>
#include "main.h"
#include "lights.h"
#include "cameras.h"
#include "shaders.h"
#include "meshes.h"
#include "textures.h"
#include "render.h"

using namespace std;
using namespace graphics_framework;
using namespace glm;

// Meshes
map<string, mesh> meshes;
// Effects
effect eff, blur, depthOfField;
// Textures
texture tex;


directional_light light;
frame_buffer frames[2];
frame_buffer first_pass;
geometry screen_quad;
free_camera cam;
frame_buffer frame;


// Cursor position
double cursor_x = 0.0;
double cursor_y = 0.0;



bool initialise()
{
  // Set input mode - hide the cursor
  glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // Capture initial mouse position
  glfwGetCursorPos(renderer::get_window(), &cursor_x, &cursor_y);

  return true;
}

bool loadContent()
{
  // Create 2 frame buffers - use screen width and height
  frames[0] = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());
  frames[1] = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());
  // Create a temp framebuffer
  first_pass = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());
  // Create screen quad
  vector<vec3> positions{ vec3(-1.0f, -1.0f, 0.0f), vec3(1.0f, -1.0f, 0.0f), vec3(-1.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f) };
  vector<vec2> tex_coords{ vec2(0.0, 0.0), vec2(1.0f, 0.0f), vec2(0.0f, 1.0f), vec2(1.0f, 1.0f) };
  screen_quad.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
  screen_quad.add_buffer(tex_coords, BUFFER_INDEXES::TEXTURE_COORDS_0);
  screen_quad.set_type(GL_TRIANGLE_STRIP);

  // Create frame buffer - use screen width and height POSSIBLY REPLACEABLE WITH FIRST_PASS
  frame = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());



  // Meshes
  createMeshes();
  transformObjects();
  setMaterials();

  // Textures
  loadTextures();

  // Lighting
  loadLights();

  // Effects
  loadShaders();
  buildShaders();

  // Cameras
  setFreeCameraProperties();

  return true;
}

bool update(float delta_time)
{
  // Use mouse to rotate the camera
  rotateFreeCamera(delta_time);

  // Use keyboard to move the camera
  moveFreeCamera();
  
  return true;;
}

bool render()
{
  // !!!!!!!!!!!!!!!!!!!! FIRST PASS !!!!!!!!!!!!!!!!!!!!
  // Set render target to first_pass
  renderer::set_render_target(first_pass);
  // Clear frame
  renderer::clear();


  // Render meshes
  for (auto &e : meshes)
	renderMesh(e.second);


  // !!!!!!!!!!!!!!!!!!!! SECOND PASS !!!!!!!!!!!!!!!!!!!!
  frame_buffer last_pass = first_pass;
  // Set MVP matrix uniform, identity
  auto MVP = mat4(1.0f);
  // Perform blur twice
  for (int i = 0; i < size(frames); i++)
  {
	// Set render target to frames[i]
	renderer::set_render_target(frames[i]);
	// Clear frame
	renderer::clear();
	// Bind motion blur effect
	renderer::bind(blur);
	// Set MVP matrix uniform
	glUniformMatrix4fv(blur.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
	// Bind frames
	renderer::bind(last_pass.get_frame(), 0);
	// Set texture
	glUniform1i(blur.get_uniform_location("tex"), 0);
	// Set inverse width
	glUniform1f(blur.get_uniform_location("inverse_width"), 1.0f / renderer::get_screen_width());
	// Set inverse height
	glUniform1f(blur.get_uniform_location("inverse_height"), 1.0f / renderer::get_screen_height());
	// Render screen quad
	renderer::render(screen_quad);
	// Set last pass to this pass
	last_pass = frames[i];
  }

  // !!!!!!!!!!!!!!!!!!!! SCREEN PASS !!!!!!!!!!!!!!!!!!!!
  // Set render target back to the screen
  renderer::set_render_target();
  // Bind depth of field effect
  renderer::bind(depthOfField);
  glUniformMatrix4fv(depthOfField.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
  // Bind texture from last pass, 0
  renderer::bind(last_pass.get_frame(), 0);
  // Set the uniform, 0
  glUniform1i(depthOfField.get_uniform_location("tex"), 0);
  // Sharp texture is taken from first pass
  // Bind first pass, 1
  renderer::bind(first_pass.get_frame(), 1);
  // Set sharp tex uniform, 1
  glUniform1i(depthOfField.get_uniform_location("sharp"), 1);
  // Depth also taken from first pass
  // Bind first pass **depth** to TU 2
  renderer::bind(first_pass.get_depth(), 2);
  // Set depth tex uniform, 2
  glUniform1i(depthOfField.get_uniform_location("depth"), 2);
  // Set range and focus values
  glUniform1f(depthOfField.get_uniform_location("range"), distance(cam.get_position(), vec3(0.0f, 0.0f, 0.0f)));
  glUniform1f(depthOfField.get_uniform_location("focus"), 0.5f);
  // Render the screen quad
  renderer::render(screen_quad);

  return true;
}

void main()
{
  // Create application
  app application("Graphics Coursework");
  // Initialise, load content, update and render
  application.set_initialise(initialise);
  application.set_load_content(loadContent);
  application.set_update(update);
  application.set_render(render);
  // Run application
  application.run();
}