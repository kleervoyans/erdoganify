#pragma once

#include <gui/scene_manager.h>

#define UNIHD_ABOUT_TEXT \
    "UniHD Card\\n\\n" \
    "Reads MIFARE DESFire cards and\\n" \
    "shows Uni Heidelberg relevant\\n" \
    "metadata (UID/ATQA/SAK/ATS,\\n" \
    "DESFire version, apps count).\\n\\n" \
    "No card modification supported."

void unihd_about_scene_on_enter(void* context);
bool unihd_about_scene_on_event(void* context, SceneManagerEvent event);
void unihd_about_scene_on_exit(void* context);
