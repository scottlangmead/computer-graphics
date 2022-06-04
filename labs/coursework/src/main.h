#pragma once

#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

extern map<string, mesh> meshes;
extern map<string, effect> effects;
extern texture tex;
extern frame_buffer frames[2];
extern frame_buffer temp_frame;
extern unsigned int current_frame;
extern geometry screen_quad;
extern free_camera cam;
extern point_light point;
extern shadow_map shadow;
extern double cursor_x, cursor_y;
extern float range;
extern float MAX_RANGE;
extern float MIN_RANGE;
extern float BLUR_FACTOR;
extern vec4 LIGHT_COLOUR;