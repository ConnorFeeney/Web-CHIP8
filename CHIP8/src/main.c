#include <stdio.h>
#include <stdlib.h>
#include <chip8.h>

int main(int argc, char *argv[]){
   if(argc < 1) {
      printf("Args: Rom Name");
      return -1;
   }

   CHIP8* chip8;
   initCHIP8(&chip8, 20);

   char path[256] = "./roms/";
   strcat(path, argv[1]);
   strcat(path, ".ch8");

   loadROM(chip8, path);

   while(!chip8End(chip8)) {
      runCHIP8(chip8);
   }

   freeCHIP8(&chip8);
   return 0;
}