#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"

#define COLORMAP_SIZE 20


struct DoubleBuffer* initialize_screen(int, int);
int read_pixel(int*,int,int,int,int);
void write_pixel(int*,int,int,int,int);
void print_screen(struct DoubleBuffer*,int,int,Color*);
void compute_buffer(struct DoubleBuffer*,int,int);
void swap_buffers(struct DoubleBuffer*);
Color* read_colormap(int);

typedef struct DoubleBuffer {
    int *front_buffer;
    int *back_buffer;
    int width;
    int height;
} DoubleBuffer;



int main(void)
{

    const int screenWidth = 1000;
    const int screenHeight = 650;

    const Image icon = LoadImage("C:/Users/gabri/Desktop/perlin/assets/icon.png");


    InitWindow(screenWidth, screenHeight, "test");
    SetWindowIcon(icon);
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

    DoubleBuffer* db = initialize_screen(screenWidth, screenHeight);
    Color* colormap = read_colormap(COLORMAP_SIZE);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
    //---------------------Updating variables---------------
        compute_buffer(db, screenWidth, screenHeight);

        swap_buffers(db);

        //--------------------Drawing----------------------
        BeginDrawing();

            print_screen(db, screenWidth, screenHeight,colormap);
            DrawFPS(10,10);

        EndDrawing();

        //-----------------INPUTS-------------------------------
        if (IsKeyDown(KEY_SPACE)) {
            TakeScreenshot("test.png");
        }
    }

    CloseWindow();
    free(colormap);
    free(db);
    return 0;
}

DoubleBuffer* initialize_screen(const int width, const int height) {
    DoubleBuffer* db = (DoubleBuffer*)malloc(sizeof(DoubleBuffer));

    if (db == NULL) {
        printf("malloc failed");
        exit(EXIT_FAILURE);
    }


    db->front_buffer = malloc(sizeof(int) * width * height);
    db->back_buffer = malloc(sizeof(int) * width * height);
    db->width = width;
    db->height = height;

    if (db->front_buffer == NULL || db->back_buffer == NULL) {
        free(db->front_buffer);
        free(db->back_buffer);
        free(db);
        printf("malloc failed");
        exit(EXIT_FAILURE);
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            write_pixel(db->front_buffer, width, x, y, rand() % COLORMAP_SIZE);
        }
    }

    return db;

}

int read_pixel(int* screen, const int width, const int height, const int x, const int y) {
    int real_x = x, real_y = y;

    if (x == -1) {
        real_x = width -1;
    }else if (x == width) {
        real_x = 0;
    }
    if (y == -1) {
        real_y = height - 1;
    } else if (y == height) {
        real_y = 0;
    }

    return screen[real_y * width + real_x];
}

void write_pixel(int* buffer,const int width, const int x, const int y, const int value) {
    buffer[y * width + x] = value;
}

void print_screen(DoubleBuffer* db,const int width, const int height, Color* colormap) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int pixel = read_pixel(db->front_buffer, width,height, x, y);
            Color color = colormap[pixel];
            DrawPixel(x, y, color);
        }
    }
}

void compute_buffer(DoubleBuffer* db,const int width, const int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            int current = read_pixel(db->front_buffer, width,height, x, y);

            int sum = 0;
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    if (!(i == 0 && j == 0) && (read_pixel(db->front_buffer, width,height,  x+i, y+j) == (current + 1)%COLORMAP_SIZE)) {
                        sum ++;
                    }
                }
            }


            if (sum >= 1) {
                db->back_buffer[y * width + x] = (current + 1) % COLORMAP_SIZE;
            } else {
                db->back_buffer[y * width + x] = current;
            }
        }
    }
}

void swap_buffers(DoubleBuffer *db) {
    void *temp = db->front_buffer;
    db->front_buffer = db->back_buffer;
    db->back_buffer = temp;
}

Color* read_colormap(const int size) {
    Color* colormap = (Color*)malloc(size * sizeof(Color));
    if (colormap == NULL) {
        printf("malloc failed");
        exit(EXIT_FAILURE);
    }

    FILE* file = fopen("C:\\Users\\gabri\\Desktop\\perlin\\assets\\colormap.txt", "r");

    if (file == NULL) {
        printf("fopen failed");
        exit(EXIT_FAILURE);
    }

    int r,g,b;

    for (int i = 0; i < size; i++) {
        fscanf(file, "%d %d %d", &r, &g, &b);
        colormap[i].r = r;
        colormap[i].g = g;
        colormap[i].b = b;
        colormap[i].a = 255;

    }

    for (int i = 0; i < size; i++) {
        printf("%d %d %d %d \n", colormap[i].r, colormap[i].g, colormap[i].b, i);
    }
    return colormap;
}