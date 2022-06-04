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
  point.set_light_colour(LIGHT_COLOUR);
  point.set_position(vec3(30.0f, 20.0f, 0.0f));
  point.set_range(50.0f);
}

// Randomly increases or decreases the range on the campfire light
void campfireFlicker()
{
  // The change in light intensity
  float change = (float(rand() % 10 + 1) / 10);

  // If increasing light intensity (50/50)
  if(rand() % 2 + 1 == 1)
  {
	// If change doesn't exceed the limit
	if ((range + change) < MAX_RANGE)
	  range += change;
	// Set range to the upper limit
	else
	  range = MAX_RANGE;
  }
  // If decreasing light intensity
  else
  {
	// If change doesn't exceed the limit
	if ((range - change) > MIN_RANGE)
	  range -= change;
	// Set range to the lower limit
	else
	  range = MIN_RANGE;
  }

  // Apply new range to point light
  point.set_range(range);
}