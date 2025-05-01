#include <mmu.h>

void initMMU(MMU** mmu) {
    *mmu = (MMU*)malloc(sizeof(MMU));
    //Setup RAM
    size_t ramSize = 4096;

    void* raw = (void*)malloc(sizeof(uint8_t) * ramSize + sizeof(size_t)); //Get memory block
    ((size_t*)raw)[0] = ramSize; //Hide ram size

    (*mmu)->ram = (uint8_t*)((size_t*)raw + 1); //Compute shifted pointer
    memset((*mmu)->ram, 0, ramSize);
}

void freeMMU(MMU** mmu) {
    free((void*)((size_t*)(*mmu)->ram - 1)); //Free ram (meta-data included)
    (*mmu)->ram = NULL;

    free(*mmu);
    *mmu = NULL;
}

void bufferMMU(MMU* mmu, const uint8_t* data, size_t offset, size_t size) {
    //Null checks
    if(!mmu || !mmu->ram) return;

    //Overflow check
    size_t ramSize = *((size_t*)mmu->ram - 1);
    if((ramSize - offset) < (size / sizeof(uint8_t))) return;

    uint8_t* des = mmu->ram + offset; //Compute offset pointer
    memcpy(des, data, size); //Copy data
}

void setMMU(MMU* mmu, const uint8_t data, size_t index) {
    //Null checks
    if(!mmu || !mmu->ram) return;

    //Overflow checks
    size_t ramSize = *((size_t*)mmu->ram - 1);
    if(ramSize < index) return;

    mmu->ram[index] = data; //Set data
}