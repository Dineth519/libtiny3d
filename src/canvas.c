#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "canvas.h"


canvas_t *canvas_create(int width, int height) {
    canvas_t *canvas = malloc(sizeof(canvas_t));              // Allocate the canvas  dynamically
    canvas->width = width;
    canvas->height = height;
    canvas->pixels = malloc(height * sizeof(float *));        // Allocate memory for an array of pointers
    
    // Set the y-th row to contain width number of elements, all initialized to 0
    for (int y = 0; y < height; y++) {
        canvas->pixels[y] = calloc(width, sizeof(float));
    }

    return canvas;       // Return the created canvas
}


// Frees all memory allocated
void canvas_destroy(canvas_t *canvas) {
    for (int y = 0; y < canvas->height; ++y) {
        free(canvas->pixels[y]);
    }
    free(canvas->pixels);
    free(canvas);
}


// Clamp value between 0.0 and 1.0
static float clamp(float x) {
    if (x < 0.0f) {
        return 0.0f;
    } else if (x > 1.0f) {
        return 1.0f;
    } else {
        return x;
    }    
}


// Spreads the brightness across nearly 4 pixels using bilinear filtering
void set_pixel_f(canvas_t *canvas, float x, float y, float intensity) {
    
    //
    if (canvas == NULL) {
        return;
    }

    int x0 = (int)floorf(x);        // Get lowest integer value of x
    int y0 = (int)floorf(y);        // Get lowest integer value of y
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    // Calculate the fractional distance between the point (x, y)
    float dx = x - x0;
    float dy = y - y0;

    // Calculate the bilinear interpolation weights for the four surrounding pixels
    float w00 = (1 - dx) * (1 - dy);
    float w10 = dx * (1 - dy);
    float w01 = (1 - dx) * dy;
    float w11 = dx * dy;

    // Add intensity (clamped) to 4 neighbors
    if (x0 >= 0 && x0 < canvas->width && y0 >= 0 && y0 < canvas->height)
        canvas->pixels[y0][x0] = clamp(canvas->pixels[y0][x0] + intensity * w00);
    if (x1 >= 0 && x1 < canvas->width && y0 >= 0 && y0 < canvas->height)
        canvas->pixels[y0][x1] = clamp(canvas->pixels[y0][x1] + intensity * w10);
    if (x0 >= 0 && x0 < canvas->width && y1 >= 0 && y1 < canvas->height)
        canvas->pixels[y1][x0] = clamp(canvas->pixels[y1][x0] + intensity * w01);
    if (x1 >= 0 && x1 < canvas->width && y1 >= 0 && y1 < canvas->height)
        canvas->pixels[y1][x1] = clamp(canvas->pixels[y1][x1] + intensity * w11);
}


// Draw a smooth line using DDA (Digital Differential Analyzer) algorithm
void draw_line_f(canvas_t *canvas, float x0, float y0, float x1, float y1, float thickness) {
    int steps;

    // Calculate the distance
    float dx = x1 - x0;
    float dy = y1 - y0;

    // Calculate the length (Euclidean distance) of line 
    float length = sqrtf(dx * dx + dy * dy);
    
    if (length == 0) {
        return;
    }    

    // Choose the greater distance (dx or dy) as number of the steps
    if (dx > dy) {
        steps = dx;
    } else {
        steps = dy;
    }

    // Prevent division by zero
    if (steps == 0) {
        steps = 1;
    }

    // Calculate how much to increment x and y in each step
    float x_inc = dx / steps;
    float y_inc = dy / steps;

    // Step through each point along the line using the increments
    for (int i = 0; i <= steps; ++i) {
        float x = x0 + i * x_inc;
        float y = y0 + i * y_inc;

        // Draw a filled circle with center (x, y) and given thickness
        int r = (int)ceilf(thickness / 2.0f);
        for (int oy = -r; oy <= r; ++oy) {
            for (int ox = -r; ox <= r; ++ox) {
                float dist = sqrtf(ox * ox + oy * oy);
                if (dist <= thickness / 2.0f) {
                    set_pixel_f(canvas, x + ox, y + oy, 1.0f);
                }
            }
        }
    }
}