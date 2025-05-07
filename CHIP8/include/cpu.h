#ifndef CPU_H
#define CPU_H

#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <raylib.h>

#include <mmu.h>

extern uint8_t keyboard[0xF + 1];

typedef struct {
    uint8_t v[16]; //Vx register
    uint16_t I; //Index register
    uint8_t vF; //Flag register

    uint8_t dt; //Delay timer (60Hz)
    uint8_t st; //Sound timer (60Hz)

    uint16_t pc; //Program counter
    uint8_t sp; //Stack pointer

    uint16_t stack[16]; //Program stack

    uint16_t opCode;

    MMU* mmu;
} CPU;

typedef void (*opcode)(CPU* cpu);
extern opcode table[0xF + 1];
extern opcode table0[0xE + 1];
extern opcode table8[0xE + 1];
extern opcode tableE[0xE + 1];
extern opcode tableF[0x65 + 1];

extern void initCPU(CPU** cpu, MMU* mmu);
extern void freeCPU(CPU** cpu);

extern void cycle(CPU* cpu);

//Table 0
extern void Table0(CPU* cpu);
extern void op00E0(CPU* cpu); //cls
extern void op00EE(CPU* cpu); //ret

//General Table
extern void op1NNN(CPU* cpu); //jp NNN
extern void op2NNN(CPU* cpu); //call NNN
extern void op3XNN(CPU* cpu); //se vX,NN
extern void op4XNN(CPU* cpu); //sne vX,NN
extern void op5XY0(CPU* cpu); //se vX,vY
extern void op6XNN(CPU* cpu); //ld vX,NN
extern void op7XNN(CPU* cpu); //add vX,NN

//Table 8
extern void Table8(CPU* cpu);
extern void op8XY0(CPU* cpu); //ld vX,vY
extern void op8XY1(CPU* cpu); //or vX,vY
extern void op8XY2(CPU* cpu); //and vX,vY
extern void op8XY3(CPU* cpu); //xor vX,vY
extern void op8XY4(CPU* cpu); //add vX,vY
extern void op8XY5(CPU* cpu); //sub vX,vY
extern void op8XY6(CPU* cpu); //shr vX{,vY}
extern void op8XY7(CPU* cpu); //subn vX,vY
extern void op8XYE(CPU* cpu); //shl vX{,vY}

//General Table
extern void op9XY0(CPU* cpu); //sne vX,vY
extern void opANNN(CPU* cpu); //ld i,NNN
extern void opBNNN(CPU* cpu); //jp v0,NNN
extern void opCXNN(CPU* cpu); //rnd vX,NN
extern void opDXYN(CPU* cpu); //drw vX, vY, N

//Table E
extern void TableE(CPU* cpu);
extern void opEX9E(CPU* cpu); //skp vX
extern void opEXA1(CPU* cpu); //sknp vX

//Table F
extern void TableF(CPU* cpu);
extern void opFX07(CPU* cpu); //ld vX,dt
extern void opFX0A(CPU* cpu); //ld vX,k
extern void opFX15(CPU* cpu); //ld dt,vX
extern void opFX18(CPU* cpu); //ld st,vX
extern void opFX1E(CPU* cpu); //add i,vX
extern void opFX29(CPU* cpu); //ld lf,vX
extern void opFX33(CPU* cpu); //ld b,vX
extern void opFX55(CPU* cpu); //ld [i],vX
extern void opFX65(CPU* cpu); //ld vX,[i]

#endif