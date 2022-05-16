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
  // Load phong effect
  eff.add_shader("res/shaders/phong.vert", GL_VERTEX_SHADER);
  eff.add_shader("res/shaders/phong.frag", GL_FRAGMENT_SHADER);

  // Load blur effect
  blur.add_shader("res/shaders/main.vert", GL_VERTEX_SHADER);
  blur.add_shader("res/shaders/blur.frag", GL_FRAGMENT_SHADER);

  // Load depth of field effect
  depthOfField.add_shader("res/shaders/main.vert", GL_VERTEX_SHADER);
  depthOfField.add_shader("res/shaders/depth_of_field.frag", GL_FRAGMENT_SHADER);
}

// Build effects
void buildShaders()
{
  eff.build();
  blur.build();
  depthOfField.build();
}