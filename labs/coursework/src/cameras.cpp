#include <glm\glm.hpp>
#include <graphics_framework.h>
#include "main.h"
#include "cameras.h"

using namespace std;
using namespace graphics_framework;
using namespace glm;

// Sets free camera properties
void setFreeCameraProperties()
{
  // Position
  cam.set_position(vec3(0.0f, 10.0f, 0.0f));

  // Pointing towards
  cam.set_target(vec3(0.0f, 0.0f, 0.0f));

  // Rotation
  cam.rotate(0.0f, -20.0f);

  // Projection
  cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
}

// Use mouse to rotate the camera
void rotateFreeCamera(float delta_time)
{
  // The ratio of pixels to rotation - remember the fov
  static double ratio_width = quarter_pi<float>() / static_cast<float>(renderer::get_screen_width());
  static double ratio_height =
    (quarter_pi<float>() *
	(static_cast<float>(renderer::get_screen_height()) / static_cast<float>(renderer::get_screen_width()))) /
	static_cast<float>(renderer::get_screen_height());

  // Current position
  double current_x, current_y;

  // Get the current cursor position
  glfwGetCursorPos(renderer::get_window(), &current_x, &current_y);

  // Calculate delta of cursor positions from last frame
  double delta_x = current_x - cursor_x,
    delta_y = current_y - cursor_y;

  // Multiply deltas by ratios - gets actual change in orientation
  delta_x = delta_x * ratio_width;
  delta_y = delta_y * ratio_height;

  // Rotate camera by delta
  cam.rotate(delta_x, -delta_y);

  // Update the camera
  cam.update(delta_time);

  // Update cursor position
  cursor_x = current_x;
  cursor_y = current_y;
}

// Use keyboard to move the camera
void moveFreeCamera()
{
  // Forward
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_W))
	cam.move(vec3(0.0f, 0.0f, 0.1f));

  // Backward
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_S))
	cam.move(vec3(0.0f, 0.0f, -0.1f));

  // Left
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_A))
	cam.move(vec3(-0.1f, 0.0f, 0.0f));

  // Right
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_D))
	cam.move(vec3(0.1f, 0.0f, 0.0f));
}