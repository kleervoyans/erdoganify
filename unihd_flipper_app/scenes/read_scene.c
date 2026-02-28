#include "read_scene.h"
#include "../unihd_card_app_i.h"

typedef enum {
    UniHdReadEventSuccess,
    UniHdReadEventFail,
} UniHdReadEvent;

static NfcCommand unihd_read_scene_poller_callback(NfcGenericEvent event, void* context) {
    App* app = context;
    if(event.protocol != NfcProtocolMfDesfire) return NfcCommandContinue;

    const MfDesfirePollerEvent* desfire_event = event.event_data;

    if(desfire_event->type == MfDesfirePollerEventTypeReadSuccess) {
        nfc_device_set_data(app->nfc_device, NfcProtocolMfDesfire, nfc_poller_get_data(app->nfc_poller));
        view_dispatcher_send_custom_event(app->view_dispatcher, UniHdReadEventSuccess);
        return NfcCommandStop;
    }

    if(desfire_event->type == MfDesfirePollerEventTypeReadFailed) {
        view_dispatcher_send_custom_event(app->view_dispatcher, UniHdReadEventFail);
        return NfcCommandReset;
    }

    return NfcCommandContinue;
}

void unihd_read_scene_on_enter(void* context) {
    App* app = context;

    popup_reset(app->popup);
    popup_set_header(app->popup, "Reading", 64, 5, AlignCenter, AlignTop);
    popup_set_text(app->popup, "Place Uni HD card\non Flipper back", 64, 24, AlignCenter, AlignTop);
    view_dispatcher_switch_to_view(app->view_dispatcher, UniHdViewPopup);

    unihd_card_reset(&app->card_info);

    app->nfc_device = nfc_device_alloc();
    app->nfc_poller = nfc_poller_alloc(app->nfc, NfcProtocolMfDesfire);
    nfc_poller_start(app->nfc_poller, unihd_read_scene_poller_callback, app);
    notification_message(app->notifications, &sequence_blink_start_cyan);
}

bool unihd_read_scene_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    if(event.type != SceneManagerEventTypeCustom) return false;

    if(event.event == UniHdReadEventFail) {
        notification_message(app->notifications, &sequence_blink_stop);
        unihd_show_error(app, "Could not read DESFire card.");
        scene_manager_previous_scene(app->scene_manager);
        return true;
    }

    if(event.event == UniHdReadEventSuccess) {
        notification_message(app->notifications, &sequence_blink_stop);
        UniHdCardLoadStatus status = unihd_card_load_from_device(&app->card_info, app->nfc_device);
        if(status != UniHdCardLoadOk) {
            unihd_show_error(app, unihd_card_load_error_to_string(status));
            scene_manager_previous_scene(app->scene_manager);
        } else {
            scene_manager_next_scene(app->scene_manager, UniHdSceneResults);
        }
        return true;
    }

    return false;
}

void unihd_read_scene_on_exit(void* context) {
    App* app = context;
    if(app->nfc_poller) {
        nfc_poller_stop(app->nfc_poller);
        nfc_poller_free(app->nfc_poller);
        app->nfc_poller = NULL;
    }
    if(app->nfc_device) {
        nfc_device_free(app->nfc_device);
        app->nfc_device = NULL;
    }
    popup_reset(app->popup);
    notification_message(app->notifications, &sequence_blink_stop);
}
