#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"

int* initialize_screen(int, int);
int read_pixel(int*,int,int,int);
void write_pixel(int*,int,int,int,int);
void print_screen(int*,int,int);
void compute_buffer(int*,int*,int,int);
void transpose_buffer(int*,int*,int,int);

int main(void)
{

    const int screenWidth = 1000;
    const int screenHeight = 650;

    const Image icon = LoadImage("C:/Users/gabri/Desktop/perlin/assets/icon.png");


    InitWindow(screenWidth, screenHeight, "test");
    SetWindowIcon(icon);
    SetTargetFPS(10);               // Set our game to run at 60 frames-per-second

    int* screen = initialize_screen(screenWidth, screenHeight);
    int* buffer = initialize_screen(screenWidth, screenHeight);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
    //---------------------Updating variables---------------
        compute_buffer(screen, buffer, screenWidth, screenHeight);
        transpose_buffer(screen, buffer, screenWidth, screenHeight);

        //--------------------Drawing----------------------
        BeginDrawing();

            print_screen(screen, screenWidth, screenHeight);
            DrawFPS(10,10);

        EndDrawing();

    }

    CloseWindow();

    free(screen);
    free(buffer);
    return 0;
}

int* initialize_screen(const int width, const int height) {
    int* screen = (int*)malloc(width * height * sizeof(int));

    if (screen == NULL) {
        printf("malloc failed");
        exit(EXIT_FAILURE);
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            write_pixel(screen, width, x, y, rand() % 4);
        }
    }
    return screen;

}

int read_pixel(int* screen,int width, int x, int y) {
    return screen[y * width + x];
}

void write_pixel(int* screen,const int width, const int x, const int y, const int value) {
    screen[y * width + x] = value;
}

void print_screen(int* screen,const int width, const int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Color color;
            int pixel = read_pixel(screen, width, x, y);
            switch (pixel) {
                case 0: color = RED; break;
                case 1: color = BLUE; break;
                case 2: color = GREEN; break;
                case 3: color = YELLOW; break;
                default: color = WHITE; break;
            }
            DrawPixel(x, y, color);
        }
    }
}

void compute_buffer(int* screen,int* buffer,const int width, const int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            int prev;
            int prevx; int prevy;

            if (x == 0) {prevx = width - 1;} else {prevx = x-1;}
            if (y == 0) {prevy = height - 1;} else {prevy = y-1;}

            prev = read_pixel(screen, width, prevx, prevy);
            buffer[y * width + x] = prev;
        }
    }
}

void transpose_buffer(int* screen,int* buffer,const int width, const int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            screen[y * width + x] = buffer[y * width + x];
        }
    }
}