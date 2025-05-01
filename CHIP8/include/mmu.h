#ifndef MMU_H
#define MMU_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint8_t* ram; //4096 byte RAM
    uint8_t* vram; //64x32 byte VRAM
} MMU;


extern void initMMU(MMU** mmu);
extern void freeMMU(MMU** mmu);

extern void bufferMMU(MMU* mmu, const uint8_t* data, size_t offset, size_t size);
extern void setMMU(MMU* mmu, const uint8_t data, size_t index);

extern void vramBufferMMU(MMU* mmu, const uint8_t* data, size_t offset, size_t size);
extern void vramSetMMU(MMU* mmu, const uint8_t data, size_t index);

#endif