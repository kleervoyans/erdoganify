#include "unihd_card.h"

#include <lib/nfc/protocols/mf_desfire/mf_desfire.h>
#include <lib/nfc/protocols/iso14443_4a/iso14443_4a.h>
#include <lib/nfc/protocols/iso14443_3a/iso14443_3a.h>
#include <lib/toolbox/simple_array.h>
#include <stdio.h>
#include <string.h>

void unihd_card_reset(UniHdCardInfo* info) {
    memset(info, 0, sizeof(UniHdCardInfo));
}

UniHdCardLoadStatus unihd_card_load_from_device(UniHdCardInfo* info, const NfcDevice* device) {
    if(!device) return UniHdCardLoadErrNotNfc;
    if(nfc_device_get_protocol(device) != NfcProtocolMfDesfire) return UniHdCardLoadErrNotDesfire;

    const MfDesfireData* data = nfc_device_get_data(device, NfcProtocolMfDesfire);
    if(!data || !data->iso14443_4a_data || !data->iso14443_4a_data->iso14443_3a_data) {
        return UniHdCardLoadErrNotDesfire;
    }

    unihd_card_reset(info);

    const Iso14443_3aData* base = data->iso14443_4a_data->iso14443_3a_data;
    info->uid_len = base->uid_len;
    memcpy(info->uid, base->uid, info->uid_len);
    info->atqa[0] = base->atqa[0];
    info->atqa[1] = base->atqa[1];
    info->sak = base->sak;

    const Iso14443_4aAtsData* ats = &data->iso14443_4a_data->ats_data;
    info->ats_tl = ats->tl;
    info->ats_t0 = ats->t0;
    info->has_ta1 = (ats->t0 & 0x10) != 0;
    info->has_tb1 = (ats->t0 & 0x20) != 0;
    info->has_tc1 = (ats->t0 & 0x40) != 0;
    info->ats_ta1 = ats->ta_1;
    info->ats_tb1 = ats->tb_1;
    info->ats_tc1 = ats->tc_1;

    info->app_count = simple_array_get_count(data->application_ids);
    info->has_free_memory = data->free_memory.is_present;
    info->bytes_free = data->free_memory.bytes_free;

    info->hw_major = data->version.hw_major;
    info->hw_minor = data->version.hw_minor;
    info->sw_major = data->version.sw_major;
    info->sw_minor = data->version.sw_minor;
    info->hw_storage = data->version.hw_storage;
    info->sw_storage = data->version.sw_storage;

    info->loaded = true;
    return UniHdCardLoadOk;
}

const char* unihd_card_load_error_to_string(UniHdCardLoadStatus status) {
    switch(status) {
    case UniHdCardLoadErrNotNfc:
        return "Not an NFC file.";
    case UniHdCardLoadErrNotDesfire:
        return "Not a DESFire card.";
    default:
        return "Unknown error.";
    }
}

static void append_hex_bytes(const uint8_t* bytes, size_t len, char* out, size_t out_size) {
    size_t used = strlen(out);
    for(size_t i = 0; i < len && used + 4 < out_size; i++) {
        int n = snprintf(out + used, out_size - used, "%02X%s", bytes[i], i + 1 < len ? " " : "");
        if(n < 0) break;
        used += (size_t)n;
    }
}

void unihd_card_format_summary(const UniHdCardInfo* info, char* out, size_t out_size) {
    if(!info->loaded) {
        snprintf(out, out_size, "No card loaded.");
        return;
    }

    char uid[64] = {0};
    append_hex_bytes(info->uid, info->uid_len, uid, sizeof(uid));

    snprintf(
        out,
        out_size,
        "UID: %s\n"
        "ATQA: %02X %02X\n"
        "SAK: %02X\n"
        "Apps: %lu\n"
        "Free: %s\n"
        "ATS TL/T0: %02X/%02X\n"
        "HW v%u.%u storage 0x%02X\n"
        "SW v%u.%u storage 0x%02X",
        uid,
        info->atqa[0],
        info->atqa[1],
        info->sak,
        (unsigned long)info->app_count,
        info->has_free_memory ? "present" : "n/a",
        info->ats_tl,
        info->ats_t0,
        info->hw_major,
        info->hw_minor,
        info->hw_storage,
        info->sw_major,
        info->sw_minor,
        info->sw_storage);
}
