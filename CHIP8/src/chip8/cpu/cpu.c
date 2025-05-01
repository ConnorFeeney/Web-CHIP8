#include <cpu.h>

opcode table[0xF + 1];
opcode table0[0xE + 1];
opcode table8[0xE + 1];
opcode tableE[0xE + 1];
opcode tableF[0x65 + 1];

void initCPU(CPU** cpu, MMU* mmu) {
    *cpu = (CPU*)malloc(sizeof(CPU));
    //Store mmu pointer
    (*cpu)->mmu = mmu;

    //0 out flags and registries
    (*cpu)->I = 0;
    (*cpu)->VF = 0;

    (*cpu)->dt = 0;
    (*cpu)->st = 0;

    (*cpu)->pc = 0x200; //Programs start at 0x200
    (*cpu)->sp = 0;

    memset((*cpu)->V, 0, sizeof((*cpu)->V));
    memset((*cpu)->stack, 0, sizeof((*cpu)->stack));
}

void freeCPU(CPU** cpu) {
    free(*cpu);
    *cpu = NULL;
}