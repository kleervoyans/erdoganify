#pragma once

#include <gui/scene_manager.h>

void unihd_read_scene_on_enter(void* context);
bool unihd_read_scene_on_event(void* context, SceneManagerEvent event);
void unihd_read_scene_on_exit(void* context);
