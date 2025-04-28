#include<stdio.h>

#include <raylib.h>

int main(int argc, char *argv[]){
    InitWindow(640, 320, "CHIP-8 Emulator");
    
    if(!IsWindowReady()){
        return 1;
    }

    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Hello, CHIP-8!", 190, 140, 20, BLACK);
        EndDrawing();
    }
    CloseWindow();
}