#include "main_menu_scene.h"
#include "../unihd_card_app_i.h"

static void unihd_main_menu_callback(void* ctx, uint32_t index) {
    App* app = ctx;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void unihd_main_menu_scene_on_enter(void* context) {
    App* app = context;
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "UniHD Card");
    submenu_add_item(app->submenu, "Read card", UniHdMainMenuItemRead, unihd_main_menu_callback, app);
    submenu_add_item(app->submenu, "Load dump", UniHdMainMenuItemLoad, unihd_main_menu_callback, app);
    submenu_add_item(app->submenu, "About", UniHdMainMenuItemAbout, unihd_main_menu_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, UniHdViewSubmenu);
}

bool unihd_main_menu_scene_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    if(event.type != SceneManagerEventTypeCustom) return false;

    if(event.event == UniHdMainMenuItemRead) {
        scene_manager_next_scene(app->scene_manager, UniHdSceneRead);
        return true;
    }

    if(event.event == UniHdMainMenuItemLoad) {
        scene_manager_next_scene(app->scene_manager, UniHdSceneLoad);
        return true;
    }

    if(event.event == UniHdMainMenuItemAbout) {
        scene_manager_next_scene(app->scene_manager, UniHdSceneAbout);
        return true;
    }

    return false;
}

void unihd_main_menu_scene_on_exit(void* context) {
    App* app = context;
    submenu_reset(app->submenu);
}
