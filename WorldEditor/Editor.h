#pragma once

#include "imgui/imgui.h"
#include <stdlib.h>

bool moveMode = false;
float light_icons_scaling = 0.8f;


void editor_start_frame();
void editor_end_frame();
void editor_initialize(float viewportWidth, float viewportHeight);
void editor_update();
void editor_terminate();
void editor_render_gui(Camera& camera);
void show_entity_controls(int entityId);


