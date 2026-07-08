#include <stdint.h>

#include "z64.h"

#define SRAM_PHYS_BASE 0x08000000
#define SRAM_SLOT0_OFFSET 0x20
#define SRAM_SLOT_STRIDE 0x1450
#define FILE_NAME_BYTES 8
#define DMA_ALIGN 8

static int32_t restore_active = 0;
static int32_t restore_file_index = 0;

typedef struct SramContext {
    uint8_t* readBuff;
} SramContext;

static uint8_t sram_buffer[FILE_NAME_BYTES + DMA_ALIGN] __attribute__((aligned(DMA_ALIGN)));

static void read_slot_name(uint8_t* dst, int32_t file_index) {
    const uint32_t fname_offset = SRAM_PHYS_BASE + SRAM_SLOT0_OFFSET + (file_index * SRAM_SLOT_STRIDE) + offsetof(z64_file_t, file_name);
    const uint32_t aligned_offset = fname_offset & ~(DMA_ALIGN - 1);
    const uint32_t buf_offset = fname_offset - aligned_offset;

    z64_Io(aligned_offset, sram_buffer, sizeof(sram_buffer), 0);

    for (uint32_t i = 0; i < FILE_NAME_BYTES; ++i)
        dst[i] = sram_buffer[buf_offset + i];
}

void shared_save_prepare(SramContext* sramCtx) {
    restore_active = 0;
    if (z64_file.file_index == 0)
        return;

    restore_file_index = z64_file.file_index;

    z64_file.file_index = 0;
    read_slot_name((uint8_t*)z64_file.file_name, 0);

    restore_active = 1;
}

void shared_save_restore(SramContext* sramCtx) {
    if (!restore_active)
        return;

    z64_file.file_index = restore_file_index;
    read_slot_name((uint8_t*)z64_file.file_name, restore_file_index);

    restore_active = 0;
}

void shared_load_prepare(SramContext* sramCtx) {
    restore_active = 0;
    if (z64_file.file_index == 0)
        return;

    restore_file_index = z64_file.file_index;

    z64_file.file_index = 0;

    restore_active = 1;
}

void shared_load_restore(SramContext* sramCtx) {
    if (!restore_active)
        return;

    z64_file.file_index = restore_file_index;
    read_slot_name((uint8_t*)z64_file.file_name, z64_file.file_index);

    restore_active = 0;
}
