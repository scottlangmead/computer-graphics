#include <glm\glm.hpp>
#include <graphics_framework.h>
#include "main.h"
#include "lights.h"

using namespace std;
using namespace graphics_framework;
using namespace glm;

// Sets lighting values
void loadLights()
{
  light.set_ambient_intensity(vec4(0.3f, 0.3f, 0.3f, 1.0f));
  light.set_light_colour(vec4(1.0f, 1.0f, 1.0f, 1.0f));
  light.set_direction(vec3(1.0f, 1.0f, -1.0f));
}