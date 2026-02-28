#pragma once

#include <nfc/nfc_device.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    UniHdCardLoadOk,
    UniHdCardLoadErrNotNfc,
    UniHdCardLoadErrNotDesfire,
} UniHdCardLoadStatus;

typedef struct {
    bool loaded;
    uint8_t uid[10];
    size_t uid_len;
    uint8_t atqa[2];
    uint8_t sak;
    uint8_t ats_tl;
    uint8_t ats_t0;
    uint8_t ats_ta1;
    uint8_t ats_tb1;
    uint8_t ats_tc1;
    bool has_ta1;
    bool has_tb1;
    bool has_tc1;
    uint32_t app_count;
    uint32_t bytes_free;
    bool has_free_memory;
    uint8_t hw_major;
    uint8_t hw_minor;
    uint8_t sw_major;
    uint8_t sw_minor;
    uint8_t hw_storage;
    uint8_t sw_storage;
} UniHdCardInfo;

void unihd_card_reset(UniHdCardInfo* info);
UniHdCardLoadStatus unihd_card_load_from_device(UniHdCardInfo* info, const NfcDevice* device);
const char* unihd_card_load_error_to_string(UniHdCardLoadStatus status);
void unihd_card_format_summary(const UniHdCardInfo* info, char* out, size_t out_size);
