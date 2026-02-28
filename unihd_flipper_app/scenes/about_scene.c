#include "about_scene.h"
#include "../unihd_card_app_i.h"

void unihd_about_scene_on_enter(void* context) {
    App* app = context;
    text_box_reset(app->textbox);
    text_box_set_text(app->textbox, UNIHD_ABOUT_TEXT);
    view_dispatcher_switch_to_view(app->view_dispatcher, UniHdViewTextBox);
}

bool unihd_about_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void unihd_about_scene_on_exit(void* context) {
    App* app = context;
    text_box_reset(app->textbox);
}
