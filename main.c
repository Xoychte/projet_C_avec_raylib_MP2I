#include "raylib.h"


int main(void)
{

    const int screenWidth = 800;
    const int screenHeight = 450;

    const Image icon = LoadImage("C:/Users/gabri/Desktop/perlin/assets/icon.png");


    InitWindow(screenWidth, screenHeight, "test");
    SetWindowIcon(icon);
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {

        BeginDrawing();

            ClearBackground(RAYWHITE);


        EndDrawing();

    }

    CloseWindow();


    return 0;
}