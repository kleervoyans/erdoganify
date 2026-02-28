#include "results_scene.h"
#include "../unihd_card_app_i.h"

void unihd_results_scene_on_enter(void* context) {
    App* app = context;

    widget_reset(app->widget);

    char summary[512] = {0};
    unihd_card_format_summary(&app->card_info, summary, sizeof(summary));

    widget_add_string_multiline_element(app->widget, 0, 0, AlignLeft, AlignTop, FontSecondary, summary);

    if(app->card_info.has_free_memory) {
        char free_line[64];
        snprintf(free_line, sizeof(free_line), "Bytes free: %lu", (unsigned long)app->card_info.bytes_free);
        widget_add_string_element(app->widget, 0, 56, AlignLeft, AlignBottom, FontSecondary, free_line);
    }

    view_dispatcher_switch_to_view(app->view_dispatcher, UniHdViewWidget);
}

bool unihd_results_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void unihd_results_scene_on_exit(void* context) {
    App* app = context;
    widget_reset(app->widget);
}
