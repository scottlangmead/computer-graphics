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
map<string, effect> effects;

// Textures
texture tex;

// Lighting
point_light point;
shadow_map shadow;

// Post processing
frame_buffer frames[2];
frame_buffer temp_frame;
unsigned int current_frame = 0;
geometry screen_quad;
free_camera cam;

// Cursor position
double cursor_x = 0.0;
double cursor_y = 0.0;

// Controls fire light level
float range = 20.0f;
float MAX_RANGE = 25.0f;
float MIN_RANGE = 15.0f;
// Colour of the light
vec4 LIGHT_COLOUR = vec4(1.0f, 0.0f, 0.0f, 1.0f);

// Strength of motion blur
float BLUR_FACTOR = 0.6f;


bool initialise()
{
  // Set input mode, hides the cursor
  glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // Capture initial mouse position
  glfwGetCursorPos(renderer::get_window(), &cursor_x, &cursor_y);
  return true;
}

bool loadContent()
{
  // Create 2 frame buffers, use screen width and height
  frames[0] = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());
  frames[1] = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());
  // Create a temp framebuffer
  temp_frame = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());
  // Create screen quad
  vector<vec3> positions{ vec3(-1.0f, -1.0f, 0.0f), vec3(1.0f, -1.0f, 0.0f), vec3(-1.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f) };
  vector<vec2> tex_coords{ vec2(0.0, 0.0), vec2(1.0f, 0.0f), vec2(0.0f, 1.0f), vec2(1.0f, 1.0f) };
  screen_quad.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
  screen_quad.add_buffer(tex_coords, BUFFER_INDEXES::TEXTURE_COORDS_0);
  screen_quad.set_type(GL_TRIANGLE_STRIP);
  // Create shadow map
  shadow = shadow_map(renderer::get_screen_width(), renderer::get_screen_height());

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

  std::cerr << "---------- CONTENT LOADED ----------" << std::endl;
  return true;
}

bool update(float delta_time)
{
  // Flip frame
  current_frame = (current_frame + 1) % 2;

  // Use mouse to rotate the camera
  rotateFreeCamera(delta_time);
  // Use keyboard to move the camera
  moveFreeCamera();

  // Randomly increases or decreases the range on the campfire light
  //campfireFlicker();

  // Update the shadow map light_position from the spot light
  shadow.light_position = point.get_position();
  return true;
}

void main()
{
  // Create the application
  app application("Graphics Coursework");
  // Initialise, load content, update and render the scene
  application.set_initialise(initialise);
  application.set_load_content(loadContent);
  application.set_update(update);
  application.set_render(render);
  // Run the application
  application.run();
}