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
    (*cpu)->vF = 0;

    (*cpu)->dt = 0;
    (*cpu)->st = 0;

    (*cpu)->pc = 0x200; //Programs start at 0x200
    (*cpu)->sp = 0;

    memset((*cpu)->v, 0, sizeof((*cpu)->v));
    memset((*cpu)->stack, 0, sizeof((*cpu)->stack));

    srand(time(NULL));
}

void freeCPU(CPU** cpu) {
    if(cpu == NULL || *cpu == NULL) return;
    free(*cpu);
    *cpu = NULL;
}

static void fetchOpCode(CPU* cpu) {
    //TODO: add check for program counter reaching out of program bounds
    uint8_t hi = cpu->mmu->ram[cpu->pc]; //Get opcode hi byte
    uint8_t low = cpu->mmu->ram[cpu->pc + 1]; //Get opcode low byte

    cpu->opCode =(hi << 8) | low; //Compress opcode

    cpu->pc += 2; //Forward program counter
}

void cycle(CPU* cpu) {
    fetchOpCode(cpu);
    printf("%04x\n", cpu->opCode);
}

// ==== Table 0 ====

void op00E0(CPU* cpu) {
    clearVRAM(cpu->mmu);
}

void op00EE(CPU* cpu) {
    cpu->sp--;
    cpu->pc = cpu->stack[cpu->sp];
}

// ==== General Table ====

void op1NNN(CPU* cpu) {
    uint16_t address = cpu->opCode & 0x0FFF;
    cpu->pc = address;
}

void op2NNN(CPU* cpu) {
    uint16_t address = cpu->opCode & 0x0FFF;

    cpu->stack[cpu->sp] = cpu->pc;
    cpu->sp++;
    cpu->pc = address;
}

void op3XNN(CPU* cpu) {
    uint8_t val = cpu->opCode & 0x00FF;
    uint8_t index = (cpu->opCode & 0X0F00) >> 8;

    if(val == cpu->v[index]) {
        cpu->pc += 2;
    }
}

void op4XNN(CPU* cpu) {
    uint8_t val = cpu->opCode & 0x00FF;
    uint8_t index = (cpu->opCode & 0x0F00) >> 8;

    if(val != cpu->v[index]) {
        cpu->pc += 2;
    }
}

void op5XY0(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    uint8_t y = (cpu->opCode & 0x00F0) >> 4;

    if(cpu->v[x] == cpu->v[y]) {
        cpu->pc += 2;
    }
}

void op6XNN(CPU* cpu) {
    uint8_t val = cpu->opCode & 0x00FF;
    uint8_t index = (cpu->opCode & 0x0F00) >> 8;

    cpu->v[index] = val;
}

void op7XNN(CPU* cpu) {
    uint8_t val = cpu->opCode & 0x00FF;
    uint8_t index = (cpu->opCode & 0x0F00) >> 8;

    cpu->v[index] += val;
}

// ==== Table 8 ====

void op8XY0(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    uint8_t y = (cpu->opCode & 0x00F0) >> 4;

    cpu->v[x] = cpu->v[y];
    cpu->vF = 0;
}

void op8XY1(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    uint8_t y = (cpu->opCode & 0x00F0) >> 4;

    cpu->v[x] |= cpu->v[y];
    cpu->vF = 0;
}

void op8XY2(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    uint8_t y = (cpu->opCode & 0x00F0) >> 4;

    cpu->v[x] &= cpu->v[y];
    cpu->vF = 0;
}

void op8XY3(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    uint8_t y = (cpu->opCode & 0x00F0) >> 4;

    cpu->v[x] ^= cpu->v[y];
    cpu->vF = 0;
}

void op8XY4(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    uint8_t y = (cpu->opCode & 0x00F0) >> 4;

    uint16_t sum = cpu->v[x] + cpu->v[y];

    if(sum > 255) {
        cpu->vF = 1;
    } else {
        cpu->vF = 0;
    }

    cpu->v[x] = sum & 0xFF;
}

void op8XY5(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    uint8_t y = (cpu->opCode & 0x00F0) >> 4;

    if(cpu->v[x] > cpu->v[y]) {
        cpu->vF = 1;
    } else {
        cpu->vF = 0;
    }

    cpu->v[x] -= cpu->v[y];
}

void op8XY6(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    uint8_t y = (cpu->opCode & 0x00F0) >> 4;

    cpu->v[x] = cpu->v[y];
    
    cpu->vF = (cpu->v[x] & 0x1);

    cpu->v[x] >>= 1;
}

void op8XY7(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    uint8_t y = (cpu->opCode & 0x00F0) >> 4;

    if(cpu->v[y] > cpu->v[x]) {
        cpu->vF = 1;
    } else {
        cpu->vF = 0;
    }

    cpu->v[x] = cpu->v[y] - cpu->v[x];
}

void op8XYE(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    uint8_t y = (cpu->opCode & 0x00F0) >> 4;

    cpu->v[x] = cpu->v[y];
    
    cpu->vF = (cpu->v[x] & 0x80);

    cpu->v[x] <<= 1;
}

// ==== General Table ====

void op9XY0(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    uint8_t y = (cpu->opCode & 0x00F0) >> 4;

    if(cpu->v[x] != cpu->v[y]) {
        cpu->pc += 2;
    }
}

void opANNN(CPU* cpu) {
    uint8_t address = (cpu->opCode & 0X0FFF);
    cpu->I = address;
}

void opBNNN(CPU* cpu) {
    uint8_t address = (cpu->opCode & 0X0FFF) + cpu->v[0];
    cpu->pc = address;
}

void CXNN(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    uint8_t val = (cpu->opCode & 0x00FF);
    uint8_t r = rand() % 256;
    cpu->v[x] = (r & val);
}

void DXYN(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    uint8_t y = (cpu->opCode & 0x00F0) >> 4;

    uint8_t n = (cpu->opCode & 0x000F);

    uint8_t xPos = cpu->v[x];
    uint8_t yPos = cpu->v[y];

    for(int row = 0; row < n; row++) {
        uint8_t byte = cpu->mmu->ram[cpu->I + row];
        for(int col = 0; col < 8; col ++) {
            uint8_t pixel = byte & (0x80 >> col);
            if(pixel) {
                setVRAM(cpu->mmu, pixel, xPos + col, yPos + row);
            }
        }
    }
}