#include "unihd_card_app_i.h"

static void (*const on_enter[])(void*) = {
    unihd_main_menu_scene_on_enter,
    unihd_read_scene_on_enter,
    unihd_load_scene_on_enter,
    unihd_results_scene_on_enter,
    unihd_about_scene_on_enter,
};

static bool (*const on_event[])(void*, SceneManagerEvent) = {
    unihd_main_menu_scene_on_event,
    unihd_read_scene_on_event,
    unihd_load_scene_on_event,
    unihd_results_scene_on_event,
    unihd_about_scene_on_event,
};

static void (*const on_exit[])(void*) = {
    unihd_main_menu_scene_on_exit,
    unihd_read_scene_on_exit,
    unihd_load_scene_on_exit,
    unihd_results_scene_on_exit,
    unihd_about_scene_on_exit,
};

static const SceneManagerHandlers scene_handlers = {
    .on_enter_handlers = on_enter,
    .on_event_handlers = on_event,
    .on_exit_handlers = on_exit,
    .scene_num = UniHdSceneCount,
};

static bool custom_event_callback(void* context, uint32_t event) {
    App* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool back_event_callback(void* context) {
    App* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

void unihd_show_error(App* app, const char* msg) {
    dialog_message_set_text(app->dialog_message, msg, 64, 26, AlignCenter, AlignCenter);
    dialog_message_set_buttons(app->dialog_message, NULL, "OK", NULL);
    dialog_message_show(app->dialogs, app->dialog_message);
}

static App* app_alloc(void) {
    App* app = malloc(sizeof(App));
    memset(app, 0, sizeof(App));

    app->scene_manager = scene_manager_alloc(&scene_handlers, app);
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, custom_event_callback);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, back_event_callback);

    app->submenu = submenu_alloc();
    app->textbox = text_box_alloc();
    app->popup = popup_alloc();
    app->widget = widget_alloc();

    view_dispatcher_add_view(app->view_dispatcher, UniHdViewSubmenu, submenu_get_view(app->submenu));
    view_dispatcher_add_view(app->view_dispatcher, UniHdViewTextBox, text_box_get_view(app->textbox));
    view_dispatcher_add_view(app->view_dispatcher, UniHdViewPopup, popup_get_view(app->popup));
    view_dispatcher_add_view(app->view_dispatcher, UniHdViewWidget, widget_get_view(app->widget));

    app->dialogs = furi_record_open(RECORD_DIALOGS);
    app->dialog_message = dialog_message_alloc();
    app->notifications = furi_record_open(RECORD_NOTIFICATION);

    app->nfc = nfc_alloc();
    app->file_path = furi_string_alloc_set(EXT_PATH("nfc"));
    unihd_card_reset(&app->card_info);

    return app;
}

static void app_free(App* app) {
    if(app->nfc_poller) nfc_poller_free(app->nfc_poller);
    if(app->nfc_device) nfc_device_free(app->nfc_device);

    nfc_free(app->nfc);
    furi_string_free(app->file_path);

    dialog_message_free(app->dialog_message);
    furi_record_close(RECORD_DIALOGS);
    furi_record_close(RECORD_NOTIFICATION);

    view_dispatcher_remove_view(app->view_dispatcher, UniHdViewWidget);
    view_dispatcher_remove_view(app->view_dispatcher, UniHdViewPopup);
    view_dispatcher_remove_view(app->view_dispatcher, UniHdViewTextBox);
    view_dispatcher_remove_view(app->view_dispatcher, UniHdViewSubmenu);

    widget_free(app->widget);
    popup_free(app->popup);
    text_box_free(app->textbox);
    submenu_free(app->submenu);

    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    free(app);
}

int32_t unihd_card_app(void* p) {
    UNUSED(p);
    App* app = app_alloc();

    Gui* gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);

    scene_manager_next_scene(app->scene_manager, UniHdSceneMainMenu);
    view_dispatcher_run(app->view_dispatcher);

    app_free(app);
    return 0;
}
