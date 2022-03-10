#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

geometry geom;
effect eff;
target_camera cam;
float theta = 0.0f;
float rho = 0.0f;
vec3 pos(0.0f, 0.0f, 0.0f);
float s = 1.0f;

bool load_content() {
  // Create cube data - twelve triangles triangles
  // Positions

	// Cube corners
	vec3 v0 = vec3(-1.0f, -1.0f, 1.0f);	// Front-top-left
	vec3 v1 = vec3(1.0f, -1.0f, 1.0f);	// Front-top-right
	vec3 v2 = vec3(-1.0f, -1.0f, -1.0f);// Front-bottom-left
	vec3 v3 = vec3(1.0f, -1.0f, -1.0f);	// Front-bottom-right
	vec3 v4 = vec3(-1.0f, 1.0f, 1.0f);	// Back-top-left
	vec3 v5 = vec3(1.0f, 1.0f, 1.0f);	// Back-top-right
	vec3 v6 = vec3(-1.0f, 1.0f, -1.0f);	// Back-bottom-left
	vec3 v7 = vec3(1.0f, 1.0f, -1.0f);	// Back-bottom-right

  vector<vec3> positions{
      // *********************************
      // Add the position data for triangles here, (6 verts per side)

	  // Front
	  v1, v0, v2,	// Tri 1
	  v2, v3, v1,	// Tri 2

	  // Back
	  v5, v6, v4,	// Tri 1
	  v5, v7, v6,	// Tri 2

	  // Right
	  v1, v3, v7,	// Tri 1
	  v7, v5, v1,	// Tri 2

	  // Left
	  v6, v2, v0,	// Tri 1
	  v0, v4, v6,	// Tri 2

	  // Top
	  v4, v0, v1,	// Tri 1
	  v1, v5, v4,	// Tri 2

	  // Bottom
	  v3, v2, v6,	// Tri 1
	  v6, v7, v3	// Tri 2

      // *********************************
  };
  // Colours
  vector<vec4> colours;
  for (auto i = 0; i < positions.size(); ++i) {
    colours.push_back(vec4(i % 2, 0.6, 0.0f, 1.0f)); // Notice how I got those Rad colours?
  }
  // Add to the geometry
  geom.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
  geom.add_buffer(colours, BUFFER_INDEXES::COLOUR_BUFFER);

  // Load in shaders
  eff.add_shader("shaders/basic.vert", GL_VERTEX_SHADER);
  eff.add_shader("shaders/basic.frag", GL_FRAGMENT_SHADER);
  // Build effect
  eff.build();

  // Set camera properties
  cam.set_position(vec3(10.0f, 10.0f, 10.0f));
  cam.set_target(vec3(0.0f, 0.0f, 0.0f));
  auto aspect = static_cast<float>(renderer::get_screen_width()) / static_cast<float>(renderer::get_screen_height());
  cam.set_projection(quarter_pi<float>(), aspect, 2.414f, 1000.0f);
  return true;
}

bool update(float delta_time) {
  // *********************************
  // Use keys to update transform values

  // WSAD - movement

	// Inwards
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_W)) {
		pos += vec3(0.0f, 0.0f, -5.0f) * delta_time;
	}
	// Outwards
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_S)) {
		pos += vec3(0.0f, 0.0f, 5.0f) * delta_time;
	}
	// Left
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_A)) {
		pos += vec3(-5.0f, 0.0f, 0.0f) * delta_time;
	}
	// Right
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_D)) {
		pos += vec3(5.0f, 0.0f, 0.0f) * delta_time;
	}

  // Arrow Keys - rotation

	// Up
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_UP)) {
		theta -= pi<float>() * delta_time;
	}
	// Down
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_DOWN)) {
		theta += pi<float>() * delta_time;
	}
	// Left
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_LEFT)) {
		rho -= pi<float>() * delta_time;
	}
	// Right
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_RIGHT)) {
		rho += pi<float>() * delta_time;
	}

  // O decrease scale, P increase scale

	// Scale up
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_O)) {
		s += 1.0f * delta_time;
	}
	// Scale down
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_P)) {
		s -= 1.0f * delta_time;
	}

  // *********************************
  // Update the camera
  cam.update(delta_time);
  return true;
}

bool render() {
  // Bind effect
  renderer::bind(eff);
  mat4 T, R, S, M;
  // *********************************
  // Create transformation matrix

	R = eulerAngleXZ(theta, rho);

	S = scale(mat4(1.0f), vec3(s, s, s));

	T = translate(mat4(1.0f), pos);

	M = T * R * S;

  // *********************************
  // Create MVP matrix
  auto V = cam.get_view();
  auto P = cam.get_projection();
  auto MVP = P * V * M;
  // Set MVP matrix uniform
  glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
  // Render geometry
  renderer::render(geom);
  return true;
}

void main() {
  // Create application
  app application("20_Transformed_Cube");
  // Set load content, update and render methods
  application.set_load_content(load_content);
  application.set_update(update);
  application.set_render(render);
  // Run application
  application.run();
}