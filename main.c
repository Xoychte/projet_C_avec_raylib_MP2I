#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "raylib.h"

#define COLORMAP_SIZE 256
#define COLORMAP_RESTRICTION 256



struct DoubleBuffer* initialize_screen(int, int);
int read_pixel(int*,int,int,int,int);
void write_pixel(int*,int,int,int,int);
void print_screen(struct DoubleBuffer*,int,int,Color*);
void compute_buffer(struct DoubleBuffer*,int,int);
void swap_buffers(struct DoubleBuffer*);
Color* read_colormap(int);
Color get_color(Color*,int);
double rand_noise(int);
double noise_2d(int,int);
int noise_to_val(int,int,int);

typedef struct DoubleBuffer {
    int *front_buffer;
    int *back_buffer;
    int width;
    int height;
} DoubleBuffer;



int main(void)
{



    const int screenWidth = 2000;
    const int screenHeight = 1250;


    const int gridHeight = 200;
    const int gridWidth = 200;
    const Image icon = LoadImage("C:/Users/gabri/Desktop/perlin/assets/icon.png");


    InitWindow(screenWidth, screenHeight, "test");
    SetWindowIcon(icon);
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

    Camera camera = { 0 };
    camera.position = (Vector3){ 100.0f, 300.0f, 100.0f };    // Camera position
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 60.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    int cameraMode = CAMERA_FIRST_PERSON;


    DoubleBuffer* db = initialize_screen(gridWidth, gridHeight);
    Color* colormap = read_colormap(COLORMAP_SIZE);




    int paused = 0;


    DisableCursor();
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {

        //compute_buffer(db,gridWidth,gridHeight);
        //swap_buffers(db);

        if (IsKeyDown(KEY_SPACE)) {
            camera.position.y+= 0.5f;
            camera.target.y+= 0.5f;

        }
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            camera.position.y -= 0.5f;
            camera.target.y -= 0.5f;
        }
        UpdateCamera(&camera, cameraMode);


        //----------------------------------------------DRAWING-------------------------------------------------//
        BeginDrawing();
        DrawFPS(0, 0);
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);



        print_screen(db,gridWidth,gridWidth,colormap);



        EndMode3D();
        EndDrawing();

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
            write_pixel(db->front_buffer, width, x, y, noise_to_val(x,y,COLORMAP_RESTRICTION));
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
            Color color = get_color(colormap, pixel);
            DrawCube((Vector3){x,pixel/2.0f,y}, 1.0f,(float)pixel,1.0f,color);
            DrawPixel(x, y, color);
        }
    }
}

void compute_buffer(DoubleBuffer* db,const int width, const int height) {
    if (db->back_buffer == NULL || db->front_buffer == NULL) {
        printf("Pointeur nul, comme toi\n");
        exit(EXIT_FAILURE);
    }
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            int current = read_pixel(db->front_buffer, width,height, x, y);

            int sum = 0;
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    if (!(i == 0 && j == 0) && (read_pixel(db->front_buffer, width,height,  x+i, y+j) == (current + 1)%COLORMAP_RESTRICTION)) {
                        sum ++;
                    }
                }
            }


            if (sum >= 3 /* && current < COLORMAP_RESTRICTION - 1 */) {
                db->back_buffer[y * width + x] = (current + 1) % COLORMAP_RESTRICTION;
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

    FILE* file = fopen("C:\\Users\\gabri\\Desktop\\perlin\\assets\\viridis_colormap.txt", "r");

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


    /* Uncomment this to print colormap
    for (int i = 0; i < size; i++) {
        printf("%d %d %d %d \n", colormap[i].r, colormap[i].g, colormap[i].b, i);
    }
    */
    return colormap;
}

Color get_color (Color* colormap, int index) {
    Color color = BLACK;
    if (index >= 0 && index < COLORMAP_SIZE) {
        color = colormap[index * (COLORMAP_SIZE/COLORMAP_RESTRICTION)];
    }
    return color;
}

