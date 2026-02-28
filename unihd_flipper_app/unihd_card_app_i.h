#pragma once

#include "unihd_card_app.h"
#include <furi.h>
#include <gui/gui.h>
#include <gui/modules/popup.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_box.h>
#include <gui/modules/widget.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>
#include <dialogs/dialogs.h>
#include <notification/notification_messages.h>
#include <nfc/nfc.h>
#include <nfc/nfc_device.h>
#include <nfc/nfc_poller.h>
#include <nfc/protocols/mf_desfire/mf_desfire_poller.h>

#include "helpers/unihd_card.h"

#include "scenes/main_menu_scene.h"
#include "scenes/read_scene.h"
#include "scenes/load_scene.h"
#include "scenes/results_scene.h"
#include "scenes/about_scene.h"

typedef struct {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    TextBox* textbox;
    Popup* popup;
    Widget* widget;
    DialogsApp* dialogs;
    DialogMessage* dialog_message;
    NotificationApp* notifications;

    Nfc* nfc;
    NfcPoller* nfc_poller;
    NfcDevice* nfc_device;

    FuriString* file_path;

    UniHdCardInfo card_info;
} App;

typedef enum {
    UniHdViewSubmenu,
    UniHdViewTextBox,
    UniHdViewPopup,
    UniHdViewWidget,
} UniHdView;

typedef enum {
    UniHdSceneMainMenu,
    UniHdSceneRead,
    UniHdSceneLoad,
    UniHdSceneResults,
    UniHdSceneAbout,
    UniHdSceneCount,
} UniHdScene;

void unihd_show_error(App* app, const char* msg);
