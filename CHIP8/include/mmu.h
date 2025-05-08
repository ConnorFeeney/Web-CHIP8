#ifndef MMU_H
#define MMU_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint8_t* ram; //4096 byte RAM
    uint8_t* vram; //64x32 byte VRAM
} MMU;

extern void initMMU(MMU** mmu, size_t ramSize, size_t vramWidth, size_t vramHeight);
extern void freeMMU(MMU** mmu);

extern void bufferRAM(MMU* mmu, const uint8_t* data, size_t offset, size_t size);
extern void clearRAM(MMU* mmu);

extern void clearVRAM(MMU* mmu);

extern void hexDumpMMU(MMU* mmu);

#endif