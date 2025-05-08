#include <stdio.h>
#include <stdlib.h>
#include <chip8.h>

int main(int argc, char *argv[]){
   CHIP8* chip8;
   initCHIP8(&chip8, 20);
   loadROM(chip8, "./spacejam.ch8");

   while(!chip8End(chip8)) {
      runCHIP8(chip8);
   }

   freeCHIP8(&chip8);
   return 0;
}