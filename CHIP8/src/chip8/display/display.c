#include <display.h>

void initDisplay(Display** display, MMU* mmu, int width, int height, int scale) {
    *display = (Display*)malloc(sizeof(Display));
    (*display)->width = width;
    (*display)->height = height;
    (*display)->scale = scale;
    (*display)->mmu = mmu;

    InitWindow(width * scale, height * scale, "CHIP8 Emu");
}

void freeDisplay(Display** display) {
    CloseWindow();
    free(*display);
    *display = NULL;
}

void renderBuffer(Display* display) {
    uint8_t* vram = display->mmu->vram;
    
    size_t vramWidth = *((size_t*)vram - 2);
    size_t vramHeight = *((size_t*)vram - 1);

    BeginDrawing();
    ClearBackground(BLACK);
    for(int row = 0; row < vramHeight; row++) {
        for(int col = 0; col < vramWidth; col++) {
            if(vram[(row * vramWidth) + col]) {
                DrawRectangle(col * display->scale, row * display->scale, display->scale, display->scale, WHITE);
            }
        }
    }
    EndDrawing();
}