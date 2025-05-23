#include <chip8.h>

const clock_t frameTime = CLOCKS_PER_SEC / 60;
const clock_t lastTime;

static const uint8_t fontset[80] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0,		// 0
	0x20, 0x60, 0x20, 0x20, 0x70,		// 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0,		// 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0,		// 3
	0x90, 0x90, 0xF0, 0x10, 0x10,		// 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0,		// 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0,		// 6
	0xF0, 0x10, 0x20, 0x40, 0x40,		// 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0,		// 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0,		// 9
	0xF0, 0x90, 0xF0, 0x90, 0x90,		// A
	0xE0, 0x90, 0xE0, 0x90, 0xE0,		// B
	0xF0, 0x80, 0x80, 0x80, 0xF0,		// C
	0xE0, 0x90, 0x90, 0x90, 0xE0,		// D
	0xF0, 0x80, 0xF0, 0x80, 0xF0,		// E
	0xF0, 0x80, 0xF0, 0x80, 0x80		// F
};

void initCHIP8(CHIP8** chip8, uint8_t renderScale) {
    *chip8 = (CHIP8*)malloc(sizeof(CHIP8));

    MMU* mmu;
    CPU* cpu;
    Display* display;

    initMMU(&mmu, 4096, 64, 32);
    initCPU(&cpu, mmu);
    initDisplay(&display, mmu, 64, 32, renderScale);

    (*chip8)->cpu = cpu;
    (*chip8)->mmu = mmu;
    (*chip8)->display = display;

    (*chip8)->frameTime = CLOCKS_PER_SEC / 60;
    (*chip8)->lastTime = clock();

    bufferRAM((*chip8)->mmu, fontset, 0, sizeof(fontset)); //Buffer font to memory
}

void freeCHIP8(CHIP8** chip8) {
    if(chip8 == NULL || *chip8 == NULL) return;

    freeMMU(&(*chip8)->mmu);
    freeCPU(&(*chip8)->cpu);
    freeDisplay(&(*chip8)->display);

    free(*chip8);
    *chip8 = NULL;
}

void loadROM(CHIP8* chip8, const char* rom) {
    FILE* fileptr; //Pointer to file
    size_t fileSize; //Size of file

    uint8_t* buffer; //Temp binary buffer
    size_t bufferSize; //Size of temp binary buffer

    //Open file in binary mode
    errno_t err = fopen_s(&fileptr, rom, "rb");
    if(err) {
        printf("Could not open ROM file");
        return;
    }

    //Get file size
    if(fseek(fileptr, 0, SEEK_END) != 0){
        printf("Could not SEEK file end");
        return;
    }
    fileSize = ftell(fileptr);
    if(fileSize == -1) {
        printf("Could not get file size");
        return;
    }
    rewind(fileptr);

    //Load file to RAM
    bufferSize = fileSize * sizeof(uint8_t);
    buffer = (uint8_t*)malloc(bufferSize);
    fread(buffer, sizeof(uint8_t), fileSize, fileptr);

    //Buffer ROM to virtual RAM
    bufferRAM(chip8->mmu, buffer, 0x200, bufferSize);

    //Free temp buffer and fileptr
    fclose(fileptr);
    free(buffer);
}

void runCHIP8(CHIP8* chip8) {
    clock_t currentTime = clock();

    if(currentTime - chip8->lastTime >= chip8->frameTime) {
        if(chip8->cpu->dt > 0){
            chip8->cpu->dt--;
        }
        if(chip8->cpu->st > 0){
            chip8->cpu->st--;
        }
        renderBuffer(chip8->display);
        chip8->lastTime = currentTime;
    }
    cycle(chip8->cpu);

    if(IsKeyPressed(KEY_EQUAL)) {
        hexDumpMMU(chip8->mmu);
    }
}

int chip8End(CHIP8* chip8) {
    return WindowShouldClose();
}