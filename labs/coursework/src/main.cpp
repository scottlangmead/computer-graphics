#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

// Geometry
geometry geom;
// Mesh map
map<string, mesh> meshes;
// Transformation hierarchy
map<mesh*, mesh*> hierarchy;
// Cameras
free_camera cam;
// Textures array
array<texture, 3> texs;
// Effects array
array<effect, 3> effs;
// Skybox
mesh skybox;
cubemap skybox_cubemap;

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

  // Holds triangle data that makes up the terrain
  vector<vec3> terrain{};

  // Holds texture coordinate data for the terrain
  vector<vec2> tex_coords;

  for (int x = 0; x < 127; x++)		// Loop through tiles on x-axis
	for (int z = 0; z < 127; z++)	// Loop through tiles on z-axis
	{
	  //// Triangle 1
	  // Terrain geometry
	  terrain.push_back(vec3(float(x),		(heights[x][z] * 30),			float(z)));
	  terrain.push_back(vec3(float(x),		(heights[x][z + 1] * 30),		float(z + 1)));
	  terrain.push_back(vec3(float(x + 1),	(heights[x + 1][z + 1] * 30),	float(z + 1)));
	  // Terrain texture
	  tex_coords.push_back(vec2(float(x * 0.1f),		float(z * 0.1f)));
	  tex_coords.push_back(vec2(float(x * 0.1f),		float((z + 1) * 0.1f)));
	  tex_coords.push_back(vec2(float((x + 1) * 0.1f),	float((z + 1) * 0.1f)));
	  //// Triangle 2
	  // Terrain geometry
	  terrain.push_back(vec3(float(x),		(heights[x][z] * 30),			float(z)));
	  terrain.push_back(vec3(float(x + 1),	(heights[x + 1][z + 1] * 30),	float(z + 1)));
	  terrain.push_back(vec3(float(x + 1),	(heights[x + 1][z] * 30),		float(z)));
	  // Terrain texture
	  tex_coords.push_back(vec2(float(x * 0.1f),		float(z * 0.1f)));
	  tex_coords.push_back(vec2(float((x + 1) * 0.1f),	float((z + 1) * 0.1f)));
	  tex_coords.push_back(vec2(float((x + 1) * 0.1f),	float(z * 0.1f)));
	}
  // Add to the geometry buffer
  geom.add_buffer(terrain, BUFFER_INDEXES::POSITION_BUFFER);
  // Add textures to the buffer
  geom.add_buffer(tex_coords, BUFFER_INDEXES::TEXTURE_COORDS_0);
}

void load_skybox() {
	// Create box geometry for skybox 
	skybox = mesh(geometry_builder::create_box());
	// Scale box by 100
	skybox.get_transform().scale *= 100.0f;
	
}

bool load_content()
{
  //// Scene meshes
  // Terrain
  Image copy(0, 0);
  copy.create_terrain_geometry("res/textures/terrain/heightmap.bmp");
  meshes["terrain"] = mesh(geom);
  meshes["rock_1"] = mesh(geometry("res/models/rock_a.obj"));
  meshes["rock_2"] = mesh(geometry("res/models/rock_d.obj"));
  meshes["rock_3"] = mesh(geometry("res/models/rock_c.obj"));
  meshes["rock_4"] = mesh(geometry("res/models/rock_b.obj"));
  meshes["rock_5"] = mesh(geometry("res/models/rock_a.obj"));
  meshes["rock_6"] = mesh(geometry("res/models/rock_b.obj"));
  // Skybox
  skybox = mesh(geometry_builder::create_box());

  //// Transformations
  // Terrain
  meshes["terrain"].get_transform().scale = vec3(2.0f, 2.0f, 2.0f);
  hierarchy[&meshes["rock_1"]] = &meshes["terrain"];	// Transform hierarchy
  meshes["rock_1"].get_transform().position = vec3(10.0f, -58.5f, 23.0f);
  meshes["rock_1"].get_transform().scale = vec3(6.0f, 5.0f, 6.0f);
  hierarchy[&meshes["rock_2"]] = &meshes["terrain"];	// Transform hierarchy
  meshes["rock_2"].get_transform().position = vec3(60.0f, -38.5f, 75.0f);
  meshes["rock_2"].get_transform().scale = vec3(4.0f, 4.0f, 4.0f);
  hierarchy[&meshes["rock_3"]] = &meshes["terrain"];	// Transform hierarchy
  meshes["rock_3"].get_transform().position = vec3(18.5f, -100.0f, 120.0f);
  meshes["rock_3"].get_transform().scale = vec3(8.0f, 6.0f, 8.0f);
  meshes["rock_3"].get_transform().orientation = vec3((quarter_pi<float>() * 3), (pi<float>() * 0.5), (pi<float>() * 0.5));
  hierarchy[&meshes["rock_4"]] = &meshes["terrain"];	// Transform hierarchy
  meshes["rock_4"].get_transform().position = vec3(92.0f, -68.0f, 35.0f);
  meshes["rock_4"].get_transform().scale = vec3(5.5f, 6.0f, 10.0f);
  hierarchy[&meshes["rock_5"]] = &meshes["terrain"];	// Transform hierarchy
  meshes["rock_5"].get_transform().position = vec3(45.0f, -37.0f, 103.5f);
  meshes["rock_5"].get_transform().scale = vec3(6.0f, 3.5f, 4.0f);
  hierarchy[&meshes["rock_6"]] = &meshes["terrain"];	// Transform hierarchy
  meshes["rock_6"].get_transform().position = vec3(50.0f, -43.5f, 20.0f);
  meshes["rock_6"].get_transform().scale = vec3(8.0f, 4.0f, 5.5f);

  //// Load textures
  texs[0] = texture("res/textures/terrain/mud.jpg");
  texs[1] = texture("res/textures/terrain/grass.jpg");
  texs[2] = texture("res/textures/terrain/rock.jpg");

  //// Load skybox cubemap
  skybox_cubemap = cubemap({"res/textures/skybox/skybox_ft.jpg", "res/textures/skybox/skybox_bk.jpg", "res/textures/skybox/skybox_up.jpg",
							"res/textures/skybox/skybox_dn.jpg", "res/textures/skybox/skybox_rt.jpg", "res/textures/skybox/skybox_lf.jpg" });

  //// Load in shaders
  // Terrain
  effs[0].add_shader("res/shaders/terrain.vert", GL_VERTEX_SHADER);
  effs[0].add_shader("res/shaders/terrain.frag", GL_FRAGMENT_SHADER);
  // Basic
  effs[1].add_shader("res/shaders/main.vert", GL_VERTEX_SHADER);
  effs[1].add_shader("res/shaders/main.frag", GL_FRAGMENT_SHADER);
  // Skybox
  effs[2].add_shader("res/shaders/skybox.vert", GL_VERTEX_SHADER);
  effs[2].add_shader("res/shaders/skybox.frag", GL_FRAGMENT_SHADER);

  // Build effects
  effs[0].build();
  effs[1].build();
  effs[2].build();

  // Set camera properties
  cam.set_position(vec3(100.0f, 40.0f, 5.0f));
  cam.rotate(pi<float>(), -0.2f);
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
	cam.move(vec3(0.0f, 0.0f, 0.3f));
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_S))
	cam.move(vec3(0.0f, 0.0f, -0.3f));
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_A))
	cam.move(vec3(-0.3f, 0.0f, 0.0f));
  if (glfwGetKey(renderer::get_window(), GLFW_KEY_D))
	cam.move(vec3(0.3f, 0.0f, 0.0f));

  // Update the camera
  cam.update(delta_time);

  // Update cursor position
  cursor_x = current_x;
  cursor_y = current_y;

  skybox.get_transform().position = cam.get_position();

  return true;
}

void render_skybox()
{
  // Disable depth test,depth mask,face culling
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);
  glDisable(GL_CULL_FACE);

  // Bind skybox effect
  renderer::bind(effs[2]);

  // Calculate MVP for the skybox
  auto M = skybox.get_transform().get_transform_matrix();
  auto V = cam.get_view();
  auto P = cam.get_projection();
  auto MVP = P * V * M;

  // Set MVP matrix uniform
  glUniformMatrix4fv(effs[2].get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

  // Set cubemap uniform
  renderer::bind(skybox_cubemap, 0);
  glUniform1i(effs[2].get_uniform_location("cubemap"), 0);

  // Render skybox
  renderer::render(skybox);

  // Enable depth test,depth mask,face culling
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glEnable(GL_CULL_FACE);
}

bool render()
{
  // Render skybox
  render_skybox();

  // Render meshes
  for (auto &e : meshes) {
	auto m = e.second;
	
	// Create MVP matrix
	mat4 M;

	// If rendering a rock (part of transform hierarchy)
	if (e.first.substr(0, 4) == "rock")
	  M = (*hierarchy[&e.second]).get_transform().get_transform_matrix() * m.get_transform().get_transform_matrix();
	else
	  M = m.get_transform().get_transform_matrix();

	auto V = cam.get_view();
	auto P = cam.get_projection();
	auto MVP = P * V * M;

	// If rendering the terrain
	if (e.first == "terrain")
	{
	  // Bind effect
	  renderer::bind(effs[0]);
	  // Set MVP matrix uniform
	  glUniformMatrix4fv(effs[0].get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

	  // Bind textures
	  renderer::bind(texs[0], 0);
	  renderer::bind(texs[1], 1);

	  // Set the uniform values for textures
	  static int tex_indices[] = { 0, 1 };
	  glUniform1iv(effs[0].get_uniform_location("tex"), 2, tex_indices);

	  // Render mesh
	  renderer::render(m);
	}
	// If rendering a regular mesh
	else
	{
	  // Bind effect
	  renderer::bind(effs[1]);
	  // Set MVP matrix uniform
	  glUniformMatrix4fv(effs[1].get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

	  // Bind texture to renderer
	  renderer::bind(texs[2], 0);
	  // Set the texture value for the shader here
	  glUniform1i(effs[1].get_uniform_location("tex"), 0);

	  // Render mesh
	  renderer::render(m);
	}
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