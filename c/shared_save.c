#include <stdint.h>

#include "z64.h"

#define SRAM_PHYS_BASE 0x08000000
#define SRAM_SLOT0_OFFSET 0x20
#define SRAM_SLOT_STRIDE 0x1450
#define FILE_NAME_BYTES 8

static int32_t restore_active = 0;
static int32_t restore_file_index = 0;

static inline void read_slot_name(uint8_t* dst, int32_t file_index) {
    z64_Io(SRAM_PHYS_BASE + SRAM_SLOT0_OFFSET + (file_index * SRAM_SLOT_STRIDE) + offsetof(z64_file_t, file_name), dst, FILE_NAME_BYTES, 0);
}

void shared_save_prepare(void) {
    restore_active = 0;
    if (z64_file.file_index == 0)
        return;

    restore_file_index = z64_file.file_index;

    z64_file.file_index = 0;
    read_slot_name((uint8_t*)z64_file.file_name, 0);

    restore_active = 1;
}

void shared_save_restore(void) {
    if (!restore_active)
        return;

    z64_file.file_index = restore_file_index;
    read_slot_name((uint8_t*)z64_file.file_name, restore_file_index);

    restore_active = 0;
}

void shared_load_prepare(void) {
    restore_active = 0;
    if (z64_file.file_index == 0)
        return;

    restore_file_index = z64_file.file_index;

    z64_file.file_index = 0;

    restore_active = 1;
}

void shared_load_restore(void) {
    if (!restore_active)
        return;

    z64_file.file_index = restore_file_index;
    read_slot_name((uint8_t*)z64_file.file_name, z64_file.file_index);

    restore_active = 0;
}
