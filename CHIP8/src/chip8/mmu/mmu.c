#include <mmu.h>

void initMMU(MMU** mmu, size_t ramSize, size_t vramWidth, size_t vramHeight) {
    *mmu = (MMU*)malloc(sizeof(MMU));

    void* raw = (void*)malloc(sizeof(uint8_t) * ramSize + sizeof(size_t)); //Get memory block
    ((size_t*)raw)[0] = ramSize; //Hide ram size
    (*mmu)->ram = (uint8_t*)((size_t*)raw + 1); //Compute shifted pointer
    memset((*mmu)->ram, 0, ramSize); //Zero memory

    void* vramRaw = (void*)malloc((sizeof(uint8_t) * ramSize) + (2 * sizeof(size_t)));
    ((size_t*)vramRaw)[0] = vramWidth;
    ((size_t*)vramRaw)[1] = vramHeight;
    (*mmu)->vram = (uint8_t*)((size_t*)vramRaw + 2);
    memset((*mmu)->vram, 0, vramWidth * vramHeight);
}

void freeMMU(MMU** mmu) {
    if(!mmu || !(*mmu)) return;

    free((void*)((size_t*)(*mmu)->ram - 1)); //Free ram (meta-data included)
    (*mmu)->ram = NULL;

    free((void*)((size_t*)(*mmu)->vram - 2)); //Free vram (meta-data included)
    (*mmu)->vram = NULL;

    free(*mmu);
    *mmu = NULL;
}

void bufferRAM(MMU* mmu, const uint8_t* data, size_t offset, size_t size) {
    //Null checks
    if(!mmu || !mmu->ram || !data) return;

    //Overflow check
    size_t ramSize = *((size_t*)mmu->ram - 1);
    if((ramSize - offset) < (size / sizeof(uint8_t))) return;

    uint8_t* des = mmu->ram + offset; //Compute offset pointer
    memcpy(des, data, size); //Copy data
}

void setRAM(MMU* mmu, const uint8_t data, size_t index) {
    //Null checks
    if(!mmu || !mmu->ram) return;

    //Overflow checks
    size_t ramSize = *((size_t*)mmu->ram - 1);
    if(ramSize < index) return;

    mmu->ram[index] = data; //Set data
}

void clearRAM(MMU* mmu) {
    if(!mmu || !mmu->ram) return;

    size_t ramSize = *((size_t*)mmu->ram - 1);
    memset(mmu->ram, 0, ramSize);
}

void setVRAM(MMU* mmu, const uint8_t data, size_t x, size_t y) {
    if(!mmu || !mmu->vram) return;

    size_t vramWidth = *((size_t*)mmu->vram - 2);
    size_t vramHeight = *((size_t*)mmu->vram - 1);
    if(vramWidth * vramHeight < x * y) return;

    mmu->vram[(y * 64) + x] = data;
}

void clearVRAM(MMU* mmu) {
    if(!mmu || !mmu->vram) return;

    size_t vramWidth = *((size_t*)mmu->vram - 2);
    size_t vramHeight = *((size_t*)mmu->vram - 1);
    memset(mmu->vram, 0, vramWidth * vramHeight);
}

void hexDumpMMU(MMU* mmu) {
    if(!mmu || !mmu->vram) return;

    //Dump RAM
    printf("RAM DUMP (HEX): \n");
    size_t ramSize = *((size_t*)mmu->ram - 1);
    for (int i = 0; i < ramSize; i++) {
        uint8_t val = mmu->ram[i];
        uint8_t prev = (i > 0) ? mmu->ram[i - 1] : 0;
    
        if (val != 0)
            printf("\x1b[32m%02x \x1b[0m", val);
        else if (prev != 0)
            printf("\x1b[33m%02x \x1b[0m", val);
        else
            printf("\x1b[90m%02x \x1b[0m", val);
    }

    //Dum VRAM
    printf("\n\nVRAM DUMP (HEX): \n");
    size_t vramWidth = *((size_t*)mmu->vram - 2);
    size_t vramHeight = *((size_t*)mmu->vram - 1);
    size_t vramSize = vramWidth * vramHeight;
    for (int i = 0; i < vramSize; i++) {
        uint8_t val = mmu->vram[i];
        uint8_t prev = (i > 0) ? mmu->vram[i - 1] : 0;
    
        if (val != 0)
            printf("\x1b[32m%02x \x1b[0m", val);
        else
            printf("\x1b[90m%02x \x1b[0m", val);
    }
}