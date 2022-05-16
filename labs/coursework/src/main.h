#pragma once

#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

// Meshes
extern map<string, mesh> meshes;
// Effects
extern effect eff, blur, depthOfField;
// Textures
extern texture tex;


extern directional_light light;
extern frame_buffer frames[2];
extern frame_buffer first_pass;
extern geometry screen_quad;
extern free_camera cam;
extern frame_buffer frame;


// Cursor position
extern double cursor_x, cursor_y;