#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

geometry geom;
map<string, mesh> meshes;
effect eff;
array<texture, 4> texs;
free_camera cam;

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

/*
The following bitmap reading code has been created with the help of "Designed by Hugo"'s tutorial,
https://www.youtube.com/watch?v=NcEE5xmpgQ0
*/

// Represents the colour of pixels
struct Color {
  float r, g, b;

  Color();	// Default constructor
  Color(float r, float g, float b);	// Constructor
};
// Represents bitmap image
class Image
{
public:
  Image(int width, int height);	// Constructor
  void create_terrain_geometry(const char* path);	// Creates terrain 

private:
  int m_width;
  int m_height;
  vector<Color> m_colors;
};
// Costructors
Image::Image(int width, int height) : m_width(width), m_height(height), m_colors(vector<Color>(width * height)) {}
Color::Color() : r(0), g(0), b(0) {}	// Default constructor
Color::Color(float r, float g, float b) : r(0), g(0), b(0) {}

// Creates uneven terrain, with a mound in the centre
void Image::create_terrain_geometry(const char* path)
{
  // Open bitmap file
  ifstream f;
  f.open(path, ios::out | ios::binary);

  // Header sizes
  const int fileHeaderSize = 14;
  const int informationHeaderSize = 40;
	
  // File Header
  unsigned char fileHeader[fileHeaderSize];
  f.read(reinterpret_cast<char*>(fileHeader), fileHeaderSize);

  // Information Header
  unsigned char informationHeader[informationHeaderSize];
  f.read(reinterpret_cast<char*>(informationHeader), informationHeaderSize);

  // File size
  int fileSize = fileHeader[2] + (fileHeader[3] << 8) + (fileHeader[4] << 16) + (fileHeader[5] << 24);
  m_width = informationHeader[4] + (informationHeader[5] << 8) + (informationHeader[6] << 16) + (informationHeader[7] << 24);
  m_height = informationHeader[8] + (informationHeader[9] << 8) + (informationHeader[10] << 16) + (informationHeader[11] << 24);

  // Resize colors vector to match actual width and height of bitmap
  m_colors.resize(m_width * m_height);

  // Calculate padding bytes
  const int paddingAmount = ((4 - (m_width * 3) % 4) % 4);

  // 2D array to hold terrain heights
  float heights[128][128];

  // Loop through each pixel
  for (int y = 0; y < m_height; y++)
  {
	for (int x = 0; x < m_width; x++)
	{
	  // Read colour values
	  unsigned char color[3];
	  f.read(reinterpret_cast<char*>(color), 3);

	  // Calculate and store rgb values
	  m_colors[y * m_width + x].r = static_cast<float>(color[2]) / 255.0f;
	  m_colors[y * m_width + x].g = static_cast<float>(color[1]) / 255.0f;
	  m_colors[y * m_width + x].b = static_cast<float>(color[0]) / 255.0f;

	  // Add height value to array based on rgb value
	  heights[x][y] = (m_colors[y * m_width + x].r + m_colors[y * m_width + x].g + m_colors[y * m_width + x].b) / 3;
	}
	// Ignore padding
	f.ignore(paddingAmount);
  }

  // Close bitmap file
  f.close();

  // Adding geometry

  // Holds all triangle data
  vector<vec3> terrain{};

  for (int x = 0; x < 127; x++)		// Loop through tiles on x-axis
	for (int z = 0; z < 127; z++)	// Loop through tiles on z-axis
	{
	  // Triangle 1
	  terrain.push_back(vec3(float(x),		(heights[x][z] * 10),			float(z)));
	  terrain.push_back(vec3(float(x),		(heights[x][z + 1] * 10),		float(z + 1)));
	  terrain.push_back(vec3(float(x + 1),	(heights[x + 1][z + 1] * 10),	float(z + 1)));
	  // Triangle 2
	  terrain.push_back(vec3(float(x),		(heights[x][z] * 10),			float(z)));
	  terrain.push_back(vec3(float(x + 1),	(heights[x + 1][z + 1] * 10),	float(z + 1)));
	  terrain.push_back(vec3(float(x + 1),	(heights[x + 1][z] * 10),		float(z)));
	}

  // Colours
  vector<vec4> colours{ vec4(1.0f, 0.0f, 0.0f, 1.0f), vec4(1.0f, 0.0f, 0.0f, 1.0f), vec4(1.0f, 0.0f, 0.0f, 1.0f) };
  // Add to the geometry
  geom.add_buffer(terrain, BUFFER_INDEXES::POSITION_BUFFER);
  geom.add_buffer(colours, BUFFER_INDEXES::COLOUR_BUFFER);
}

bool load_content()
{
  //// Scene meshes
  // Castle gate
  meshes["gate_pillar1"] = mesh(geometry_builder::create_box(vec3(2.0f, 6.0f, 2.0f)));
  meshes["gate_pillar2"] = mesh(geometry_builder::create_box(vec3(2.0f, 6.0f, 2.0f)));
  meshes["gate_pillar_connection_top"] = mesh(geometry_builder::create_box(vec3(6.0f, 1.0f, 2.0f)));
  meshes["tower_close_left"] = mesh(geometry_builder::create_cylinder(20, 20));
  // Sample
  meshes["box"] = mesh(geometry_builder::create_box());
  meshes["tetra"] = mesh(geometry_builder::create_tetrahedron());
  meshes["pyramid"] = mesh(geometry_builder::create_pyramid());
  meshes["disk"] = mesh(geometry_builder::create_disk(20));
  meshes["cylinder"] = mesh(geometry_builder::create_cylinder(20, 20));
  meshes["sphere"] = mesh(geometry_builder::create_sphere(20, 20));
  meshes["torus"] = mesh(geometry_builder::create_torus(20, 20, 1.0f, 5.0f));
  // Terrain creation via heightmap
  Image copy(0, 0);
  copy.create_terrain_geometry("res/textures/heightmap.bmp");
  meshes["terrain"] = mesh(geom);

  //// Mesh transformations
  // Castle gate
  meshes["gate_pillar1"].get_transform().translate(vec3(-4.0f, 3.0f, 0.0f));
  meshes["gate_pillar2"].get_transform().translate(vec3(4.0f, 3.0f, 0.0f));
  meshes["gate_pillar_connection_top"].get_transform().translate(vec3(0.0f, 5.5f, 0.0f));
  meshes["tower_close_left"].get_transform().scale = vec3(25.0f, 50.0f, 25.0f);
  meshes["tower_close_left"].get_transform().translate(vec3(50.0f, 25.0f, 0.0f));
  // Sample
  meshes["box"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
  meshes["box"].get_transform().translate(vec3(-10.0f, 2.5f, -30.0f));
  meshes["tetra"].get_transform().scale = vec3(4.0f, 4.0f, 4.0f);
  meshes["tetra"].get_transform().translate(vec3(-30.0f, 10.0f, -10.0f));
  meshes["pyramid"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
  meshes["pyramid"].get_transform().translate(vec3(-10.0f, 7.5f, -30.0f));
  meshes["disk"].get_transform().scale = vec3(3.0f, 1.0f, 3.0f);
  meshes["disk"].get_transform().translate(vec3(-10.0f, 11.5f, -30.0f));
  meshes["disk"].get_transform().rotate(vec3(half_pi<float>(), 0.0f, 0.0f));
  meshes["cylinder"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
  meshes["cylinder"].get_transform().translate(vec3(-25.0f, 2.5f, -25.0f));
  meshes["sphere"].get_transform().scale = vec3(2.5f, 2.5f, 2.5f);
  meshes["sphere"].get_transform().translate(vec3(-25.0f, 10.0f, -25.0f));
  meshes["torus"].get_transform().translate(vec3(-25.0f, 10.0f, -25.0f));
  meshes["torus"].get_transform().rotate(vec3(half_pi<float>(), 0.0f, 0.0f));

  // Load textures
  texs[0] = texture("res/textures/check_1.png");
  texs[1] = texture("res/textures/check_2.png");
  texs[2] = texture("res/textures/check_3.png");
  texs[3] = texture("res/textures/check_4.png");

  // Load in shaders
  eff.add_shader("res/shaders/simple_texture.vert", GL_VERTEX_SHADER);
  eff.add_shader("res/shaders/simple_texture.frag", GL_FRAGMENT_SHADER);

  // Build effect
  eff.build();

  // Set camera properties
  cam.set_position(vec3(0.0f, 10.0f, 0.0f));
  cam.set_target(vec3(0.0f, 0.0f, 0.0f));
  cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
  return true;
}

bool update(float delta_time)
{
  // The ratio of pixels to rotation
  static double ratio_width = quarter_pi<float>() / static_cast<float>(renderer::get_screen_width());
  static double ratio_height = (quarter_pi<float>() * (static_cast<float>(renderer::get_screen_height()) / static_cast<float>(renderer::get_screen_width()))) / static_cast<float>(renderer::get_screen_height());

  double current_x;
  double current_y;

  // Get the current cursor position
  glfwGetCursorPos(renderer::get_window(), &current_x, &current_y);

  // Calculate delta of cursor positions from last frame
  double delta_x = current_x - cursor_x;
  double delta_y = current_y - cursor_y;

  // Multiply deltas by ratios - gets actual change in orientation
  delta_x = delta_x * ratio_width;
  delta_y = delta_y * ratio_height;

  // Rotate cameras by delta
  cam.rotate(delta_x, -delta_y);

  // Use keyboard to move the camera - WSAD
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_W))
	cam.move(vec3(0.0f, 0.0f, 0.1f));
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_S))
	cam.move(vec3(0.0f, 0.0f, -0.1f));
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_A))
	cam.move(vec3(-0.1f, 0.0f, 0.0f));
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_D))
	cam.move(vec3(0.1f, 0.0f, 0.0f));

  // Update the camera
  cam.update(delta_time);

  // Update cursor position
  cursor_x = current_x;
  cursor_y = current_y;

  return true;
}

bool render()
{
  // Render meshes
  for (auto &e : meshes) {
	auto m = e.second;
	// Bind effect
	renderer::bind(eff);
	// Create MVP matrix
	auto M = m.get_transform().get_transform_matrix();
	auto V = cam.get_view();
	auto P = cam.get_projection();
	auto MVP = P * V * M;
	// Set MVP matrix uniform
	glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

	// Bind and set texture
	renderer::bind(texs[0], 0);
	glUniform1i(eff.get_uniform_location("tex"), 0);

	// Render mesh
	renderer::render(m);
  }

  return true;
}

void main()
{
  // Create application
  app application("Graphics Coursework");
  // Set load content, update and render methods
  application.set_load_content(load_content);
  application.set_initialise(initialise);
  application.set_update(update);
  application.set_render(render);
  // Run application
  application.run();
}