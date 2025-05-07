#ifndef CHIP8_H
#define CHIP8_H

#include <stdio.h>
#include <stdint.h>

#include <cpu.h>
#include <mmu.h>
#include <display.h>

typedef struct {
    CPU* cpu;
    MMU* mmu;
    Display* display;
} CHIP8;

extern void initCHIP8(CHIP8** chip8, uint8_t renderScale);
extern void freeCHIP8(CHIP8** chip8);

extern void loadROM(CHIP8* chip8, const char* rom);
extern void runCHIP8(CHIP8* chip8);
extern int chip8End(CHIP8* chip8);
#endif