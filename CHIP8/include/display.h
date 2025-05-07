#ifndef DISPLAY_H
#define DISPLAY_H

#include <mmu.h>
#include <raylib.h>
#include <stdlib.h>

typedef struct {
    MMU* mmu;
    int width;
    int height;

    int scale;
} Display;

extern void initDisplay(Display** display, MMU* mmu,int width, int height, int scale);
extern void freeDisplay(Display** display);
extern void renderBuffer(Display* display);

#endif