#include <glm\glm.hpp>
#include <graphics_framework.h>
#include "main.h"
#include "shaders.h"

using namespace std;
using namespace graphics_framework;
using namespace glm;

// Loads in shaders
void loadShaders()
{
  std::cerr << "---------- LOADING EFFECTS ----------" << std::endl;

  // Load main effect
  std::cerr << "Main..." << std::endl;
  effects["main"].add_shader("res/shaders/main.vert", GL_VERTEX_SHADER);
  effects["main"].add_shader({ "res/shaders/main.frag","res/shaders/part_point.frag", "res/shaders/part_shadow.frag" }, GL_FRAGMENT_SHADER);

  // Load shadow effect
  std::cerr << "Shadows..." << std::endl;
  effects["shadows"].add_shader("res/shaders/point.vert", GL_VERTEX_SHADER);
  effects["shadows"].add_shader("res/shaders/point.frag", GL_FRAGMENT_SHADER);

  // Load motion blur effect
  std::cerr << "Motion Blur..." << std::endl;
  effects["motion_blur"].add_shader("res/shaders/simple.vert", GL_VERTEX_SHADER);
  effects["motion_blur"].add_shader("res/shaders/motion_blur.frag", GL_FRAGMENT_SHADER);

  // Load texture effect
  std::cerr << "Texture..." << std::endl;
  effects["texture"].add_shader("res/shaders/simple.vert", GL_VERTEX_SHADER);
  effects["texture"].add_shader("res/shaders/simple.frag", GL_FRAGMENT_SHADER);
}

// Build effects
void buildShaders()
{
  std::cerr << "---------- BUILDING EFFECTS ----------" << std::endl;

  // Build main effect
  std::cerr << "Main..." << std::endl;
  effects["main"].build();

  // Build shadow effect
  std::cerr << "Shadows..." << std::endl;
  effects["shadows"].build();

  // Build motion blur effect
  std::cerr << "Motion Blur..." << std::endl;
  effects["motion_blur"].build();

  // Build texture effect
  std::cerr << "Texture..." << std::endl;
  effects["texture"].build();
}