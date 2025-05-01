#include <stdio.h>
#include <stdlib.h>
#include <chip8.h>

int main(int argc, char *argv[]){
   CHIP8* chip8;
   initCHIP8(&chip8);

   //Print ram in hexcedecimal bytes
   for(int i = 0; i < 4096; i++){
      printf("%02x\n", chip8->mmu->ram[i]);
   }

   getchar(); //Pause excecution

   freeCHIP8(&chip8);
   return 0;
}