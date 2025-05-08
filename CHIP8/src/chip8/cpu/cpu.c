#include <cpu.h>

uint8_t keyboard[0xF + 1] = {
    (uint8_t)KEY_ONE,
    (uint8_t)KEY_TWO,
    (uint8_t)KEY_THREE,
    (uint8_t)KEY_FOUR,
    (uint8_t)KEY_Q,
    (uint8_t)KEY_W,
    (uint8_t)KEY_E,
    (uint8_t)KEY_R,
    (uint8_t)KEY_A,
    (uint8_t)KEY_S,
    (uint8_t)KEY_D,
    (uint8_t)KEY_F,
    (uint8_t)KEY_Z,
    (uint8_t)KEY_X,
    (uint8_t)KEY_C,
    (uint8_t)KEY_V
};

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
    (*cpu)->v[0xF] = 0;

    (*cpu)->dt = 0;
    (*cpu)->st = 0;

    (*cpu)->pc = 0x200; //Programs start at 0x200
    (*cpu)->sp = 0;

    (*cpu)->waitForKey = 0;

    memset((*cpu)->v, 0, sizeof((*cpu)->v));
    memset((*cpu)->stack, 0, sizeof((*cpu)->stack));

    for(int i = 0; i <= 0xF; i++) {
        table[i] = opNULL;
    }

    for(int i = 0; i <= 0xE; i++) {
        table0[i] = opNULL;
        table8[i] = opNULL;
        tableE[i] = opNULL;
    }

    for(int i = 0; i <= 0x65; i++) {
        tableF[i] = opNULL;
    }

    table[0x0] = Table0;
    table0[0x0] = op00E0;
    table0[0xE] = op00EE;

    table[0x1] = op1NNN;
    table[0x2] = op2NNN;
    table[0x3] = op3XNN;
    table[0x4] = op4XNN;
    table[0x5] = op5XY0;
    table[0x6] = op6XNN;
    table[0x7] = op7XNN;

    table[0x8] = Table8;
    table8[0x0] = op8XY0;
    table8[0x1] = op8XY1;
    table8[0x2] = op8XY2;
    table8[0x3] = op8XY3;
    table8[0x4] = op8XY4;
    table8[0x5] = op8XY5;
    table8[0x6] = op8XY6;
    table8[0x7] = op8XY7;
    table8[0xE] = op8XYE;

    table[0x9] = op9XY0;
    table[0xA] = opANNN;
    table[0xB] = opBNNN;
    table[0xC] = opCXNN;
    table[0xD] = opDXYN;

    table[0xE] = TableE;
    tableE[0x1] = opEXA1;
    tableE[0xE] = opEX9E;

    table[0xF] = TableF;
    tableF[0x07] = opFX07;
    tableF[0x0A] = opFX0A;
    tableF[0x15] = opFX15;
    tableF[0x18] = opFX18;
    tableF[0x1E] = opFX1E;
    tableF[0x29] = opFX29;
    tableF[0x33] = opFX33;
    tableF[0x55] = opFX55;
    tableF[0x65] = opFX65;

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
    if(cpu->waitForKey) {
        for(uint8_t i = 0; i <= 0xF; i++) {
            if(IsKeyPressed(keyboard[i])) {
                cpu->v[cpu->waitRegister] = i;
                cpu->waitForKey = 0;
                break;
            }
        }
    }
    
    if(cpu->waitForKey) return;

    fetchOpCode(cpu);

    printf("OPCODE: %04x\nPC: %02x\n", cpu->opCode, cpu->pc);
    uint8_t hi = (cpu->opCode & 0xF000) >> 12;
    table[hi](cpu);
}

void opNULL(CPU* cpu) {
    printf("NULL OPCODE\n");
}

// ==== Table 0 ====
void Table0(CPU* cpu) {
    uint8_t lo = cpu->opCode & 0x000F;
    table0[lo](cpu);
}

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
void Table8(CPU* cpu) {
    uint8_t lo = cpu->opCode & 0x000F;
    table8[lo](cpu);
}

void op8XY0(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    uint8_t y = (cpu->opCode & 0x00F0) >> 4;

    cpu->v[x] = cpu->v[y];
    cpu->v[0xF] = 0;
}

void op8XY1(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    uint8_t y = (cpu->opCode & 0x00F0) >> 4;

    cpu->v[x] |= cpu->v[y];
    cpu->v[0xF] = 0;
}

void op8XY2(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    uint8_t y = (cpu->opCode & 0x00F0) >> 4;

    cpu->v[x] &= cpu->v[y];
    cpu->v[0xF] = 0;
}

void op8XY3(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    uint8_t y = (cpu->opCode & 0x00F0) >> 4;

    cpu->v[x] ^= cpu->v[y];
    cpu->v[0xF] = 0;
}

void op8XY4(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    uint8_t y = (cpu->opCode & 0x00F0) >> 4;

    uint16_t sum = cpu->v[x] + cpu->v[y];

    if(sum > 255) {
        cpu->v[0xF] = 1;
    } else {
        cpu->v[0xF] = 0;
    }

    cpu->v[x] = sum & 0xFF;
}

void op8XY5(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    uint8_t y = (cpu->opCode & 0x00F0) >> 4;

    if(cpu->v[x] > cpu->v[y]) {
        cpu->v[0xF] = 1;
    } else {
        cpu->v[0xF] = 0;
    }

    cpu->v[x] -= cpu->v[y];
}

void op8XY6(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    if((cpu->v[x] & 0x1) == 1){
        cpu->v[0xF] = 1;
    } else {
        cpu->v[0xF] = 0;
    }
    cpu->v[x] >>= 1;
}

void op8XY7(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    uint8_t y = (cpu->opCode & 0x00F0) >> 4;

    if(cpu->v[y] > cpu->v[x]) {
        cpu->v[0xF] = 1;
    } else {
        cpu->v[0xF] = 0;
    }

    cpu->v[x] = cpu->v[y] - cpu->v[x];
}

void op8XYE(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    if(((cpu->v[x] & 0x80) >> 7) == 1) {
        cpu->v[0xF] = 1;
    } else {
        cpu->v[0xF] = 0;
    }

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
    uint16_t address = (cpu->opCode & 0X0FFF);
    cpu->I = address;
}

void opBNNN(CPU* cpu) {
    uint16_t address = (cpu->opCode & 0X0FFF) + cpu->v[0];
    cpu->pc = address;
}

void opCXNN(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    uint8_t val = (cpu->opCode & 0x00FF);
    uint8_t r = rand() % 256;
    cpu->v[x] = (r & val);
}

void opDXYN(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    uint8_t y = (cpu->opCode & 0x00F0) >> 4;
    uint8_t n = (cpu->opCode & 0x000F);

    uint8_t xPos = cpu->v[x];
    uint8_t yPos = cpu->v[y];
    cpu->v[0xF] = 0;

    for(int row = 0; row < n; row++) {
        uint8_t byte = cpu->mmu->ram[cpu->I + row];
        for(int col = 0; col < 8; col++) {
            if((byte & (0x80 >> col)) != 0) {
                size_t vramX = (xPos + col) % 64;
                size_t vramY = (yPos + row) % 32;
                size_t idx = vramY * 64 + vramX;
                if(cpu->mmu->vram[idx]) {
                    cpu->v[0xF] = 1;
                }
                cpu->mmu->vram[idx] ^= 1;
            }
        }
    }
}

// ==== Table E ====
void TableE(CPU* cpu) {
    uint8_t lo = cpu->opCode & 0x000F;
    tableE[lo](cpu);
}

void opEX9E(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    if(IsKeyPressed(keyboard[cpu->v[x]])) {
        cpu->pc += 2;
    }
}

void opEXA1(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    if(!IsKeyDown(keyboard[cpu->v[x]])) {
        cpu->pc += 2;
    }
}

// ==== Table F ====
void TableF(CPU* cpu) {
    uint8_t lo = cpu->opCode & 0x00FF;
    tableF[lo](cpu);
}

void opFX07(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    cpu->v[x] = cpu->dt;
}

void opFX0A(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;

    cpu->waitForKey = true;
    cpu->waitRegister = x;
}

void opFX15(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    cpu->dt = cpu->v[x];
}

void opFX18(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    cpu->st = cpu->v[x];
}

void opFX1E(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    cpu->I += cpu->v[x];
}

void opFX29(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    cpu->I = (cpu->v[x] * 5);
}

void opFX33(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    uint8_t val = cpu->v[x];

    int i = 2;
    for(int j = 0; j < 3; j++, i--) {
        cpu->mmu->ram[cpu->I + i] = val % 10;
        val /= 10;
    }
}

void opFX55(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    for (int i = 0; i <= x; i++) {
        cpu->mmu->ram[cpu->I + i] = cpu->v[i];
    }
}

void opFX65(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    for (int i = 0; i <= x; i++) {
        cpu->v[i] = cpu->mmu->ram[cpu->I + i];
    }
}