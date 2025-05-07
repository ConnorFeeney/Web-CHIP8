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
    if((cpu->v[x] & 0x1) == 1){
        cpu->vF = 1;
    } else {
        cpu->vF = 0;
    }
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
    if(((cpu->v[x] & 0x80) >> 7) == 1) {
        cpu->vF = 1;
    } else {
        cpu->vF = 0;
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
    uint8_t address = (cpu->opCode & 0X0FFF);
    cpu->I = address;
}

void opBNNN(CPU* cpu) {
    uint8_t address = (cpu->opCode & 0X0FFF) + cpu->v[0];
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

// ==== Table E ====

void opEX9E(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 4;
    if(IsKeyPressed(keyboard[cpu->v[x]])) {
        cpu->pc += 2;
    }
}

void opEXA1(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 4;
    if(!IsKeyDown(keyboard[cpu->v[x]])) {
        cpu->pc += 2;
    }
}

// ==== Table F ====

void opFX07(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    cpu->v[x] = cpu->dt;
}

void opFX0A(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;

    int keypressed = 0;
    while(!keypressed) {
        for(uint8_t i = 0; i < 0xF + 1; i++) {
            if(IsKeyPressed(keyboard[i])) {
                keypressed = 1;
                cpu->v[x] = i;
                break;
            }
        }
    }
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
    cpu->I = (cpu->v[x] & 0x0F);
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
    for(int i = 0; i <= x; i++) {
        cpu->I += i;
        cpu->v[i] = cpu->mmu->ram[cpu->I];
    }
}

void opFX65(CPU* cpu) {
    uint8_t x = (cpu->opCode & 0x0F00) >> 8;
    for(int i = 0; i <= x; i++) {
        cpu->I += i;
        cpu->mmu->ram[cpu->I] = cpu->v[i];
    }
}