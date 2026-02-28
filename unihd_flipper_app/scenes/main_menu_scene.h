#pragma once

#include <gui/scene_manager.h>

typedef enum {
    UniHdMainMenuItemRead,
    UniHdMainMenuItemLoad,
    UniHdMainMenuItemAbout,
} UniHdMainMenuItem;

void unihd_main_menu_scene_on_enter(void* context);
bool unihd_main_menu_scene_on_event(void* context, SceneManagerEvent event);
void unihd_main_menu_scene_on_exit(void* context);
