#include <stdio.h>
#include <stdlib.h>
#include <chip8.h>

int main(int argc, char *argv[]){
   CHIP8* chip8;
   initCHIP8(&chip8, 20);

   loadROM(chip8, "./spacejam.ch8");

   setVRAM(chip8->mmu, 1, 2, 2);

   hexDumpMMU(chip8->mmu);

   printf("\n\n");
   cycle(chip8->cpu);
   renderBuffer(chip8->display);

   getchar(); //Pause excecution

   freeCHIP8(&chip8);
   return 0;
}