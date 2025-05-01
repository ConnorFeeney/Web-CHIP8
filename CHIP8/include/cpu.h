#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <string.h>

#include <mmu.h>

typedef void (*opcode)();
extern opcode table[0xF + 1];
extern opcode table0[0xE + 1];
extern opcode table8[0xE + 1];
extern opcode tableE[0xE + 1];
extern opcode tableF[0x65 + 1];

typedef struct {
    uint8_t V[16]; //Vx register
    uint16_t I; //Index register
    uint8_t VF; //Flag register

    uint8_t dt; //Delay timer (60Hz)
    uint8_t st; //Sound timer (60Hz)

    uint16_t pc; //Program counter
    uint8_t sp; //Stack pointer

    uint16_t stack[16]; //Program stack

    MMU* mmu;
} CPU;

extern void initCPU(CPU** cpu, MMU* mmu);
extern void freeCPU(CPU** cpu);

extern void cycle(CPU* cpu);

#endif