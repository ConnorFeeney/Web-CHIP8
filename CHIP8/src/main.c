#include <stdio.h>
#include <stdlib.h>
#include <chip8.h>

int main(int argc, char *argv[]){
   CHIP8* chip8;
   initCHIP8(&chip8, 20);
   loadROM(chip8, "./spacejam.ch8");

   while(!chip8End(chip8)) {
      runCHIP8(chip8);
      printf("PC: %04x\n", chip8->cpu->pc);
      //hexDumpMMU(chip8->mmu);
      printf("\n\n");
      renderBuffer(chip8->display);
   }

   freeCHIP8(&chip8);
   return 0;
}