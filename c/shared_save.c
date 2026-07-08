#include <stdint.h>

#include "z64.h"

#define SRAM_SLOT0_OFFSET 0x20
#define SRAM_SLOT_STRIDE 0x1450
#define FILE_NAME_BYTES 8

static int32_t restore_active = 0;
static int32_t restore_file_index = 0;
static uint8_t restore_file_name[FILE_NAME_BYTES];

static uint8_t file0_name[FILE_NAME_BYTES];

typedef struct SramContext {
    uint8_t* readBuff;
} SramContext;

static void read_slot_name(uint8_t* dst, SramContext* sramCtx, int32_t file_index) {
    uint32_t fname_offset = SRAM_SLOT0_OFFSET + (file_index * SRAM_SLOT_STRIDE) + offsetof(z64_file_t, file_name);
    for (uint32_t i = 0; i < FILE_NAME_BYTES; ++i)
        dst[i] = sramCtx->readBuff[fname_offset + i];
}

static inline void mcpy(uint8_t* dst, const uint8_t* src, uint32_t size) {
    for (uint32_t i = 0; i < size; ++i)
        dst[i] = src[i];
}

void shared_save_prepare(void) {
    restore_active = 0;
    if (z64_file.file_index == 0)
        return;

    restore_file_index = z64_file.file_index;
    mcpy(restore_file_name, (uint8_t*)z64_file.file_name, FILE_NAME_BYTES);

    z64_file.file_index = 0;
    mcpy((uint8_t*)z64_file.file_name, file0_name, FILE_NAME_BYTES);

    restore_active = 1;
}

void shared_save_restore(void) {
    if (!restore_active)
        return;

    z64_file.file_index = restore_file_index;
    mcpy((uint8_t*)z64_file.file_name, restore_file_name, FILE_NAME_BYTES);

    restore_active = 0;
}

void shared_load_prepare(SramContext* sramCtx) {
    restore_active = 0;
    if (z64_file.file_index == 0)
        return;

    read_slot_name(file0_name, sramCtx, 0);

    restore_file_index = z64_file.file_index;
    z64_file.file_index = 0;

    restore_active = 1;
}

void shared_load_restore(SramContext* sramCtx) {
    if (!restore_active)
        return;

    z64_file.file_index = restore_file_index;
    read_slot_name((uint8_t*)z64_file.file_name, sramCtx, restore_file_index);

    restore_active = 0;
}
