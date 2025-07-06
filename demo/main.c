#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "tiny3d.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif


int main() {
    // Create a canvas for the clock face
    const int CANVAS_WIDTH = 800;
    const int CANVAS_HEIGHT = 800;

    // Initialize the canvas
    canvas_t *clock_canvas = canvas_create(CANVAS_WIDTH, CANVAS_HEIGHT);
    if (!clock_canvas) {
        fprintf(stderr, "Failed to create clock canvas.\n");
        return 1;
    }

    // Get center and radius for the clock face
    float cx = CANVAS_WIDTH / 2.0f;       
    float cy = CANVAS_HEIGHT / 2.0f;   
    float radius = (CANVAS_WIDTH < CANVAS_HEIGHT ? CANVAS_WIDTH : CANVAS_HEIGHT) * 0.45f;

    // Clear the canvas to white
    for (int deg = 0; deg < 360; deg += 15) {
        float rad = deg * M_PI / 180.0f;
        float x1 = cx + radius * cosf(rad);
        float y1 = cy + radius * sinf(rad);
        color_t white = {255, 255, 255};   // White color for the clock face
        draw_line_f(clock_canvas, cx, cy, x1, y1, 4.0f, white);     // Draw hour markers (black color)
    }

    // Draw the clock face outline as pgm
    canvas_save_pgm(clock_canvas, "clockface.pgm");
    printf("Saved clockface.pgm\n");
    canvas_destroy(clock_canvas);

    return 0;
}
