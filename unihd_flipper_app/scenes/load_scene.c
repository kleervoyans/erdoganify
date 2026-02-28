#include "load_scene.h"
#include "../unihd_card_app_i.h"

void unihd_load_scene_on_enter(void* context) {
    App* app = context;

    DialogsFileBrowserOptions options;
    dialog_file_browser_set_basic_options(&options, ".nfc", NULL);
    options.base_path = EXT_PATH("nfc");
    options.hide_dot_files = true;

    if(!dialog_file_browser_show(app->dialogs, app->file_path, app->file_path, &options)) {
        scene_manager_previous_scene(app->scene_manager);
        return;
    }

    NfcDevice* loaded = nfc_device_alloc();
    bool ok = nfc_device_load(loaded, furi_string_get_cstr(app->file_path));
    if(!ok) {
        nfc_device_free(loaded);
        unihd_show_error(app, "Failed to load dump.");
        scene_manager_previous_scene(app->scene_manager);
        return;
    }

    UniHdCardLoadStatus status = unihd_card_load_from_device(&app->card_info, loaded);
    nfc_device_free(loaded);

    if(status != UniHdCardLoadOk) {
        unihd_show_error(app, unihd_card_load_error_to_string(status));
        scene_manager_previous_scene(app->scene_manager);
        return;
    }

    scene_manager_next_scene(app->scene_manager, UniHdSceneResults);
}

bool unihd_load_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void unihd_load_scene_on_exit(void* context) {
    UNUSED(context);
}
