#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "tiny3d.h"
#include "canvas.h"

// Define a color_t struct to represent RGB colors
canvas_t *canvas_create(int width, int height) {
    canvas_t *canvas = malloc(sizeof(canvas_t));
    if (!canvas) return NULL;

    // Initialize canvas dimensions
    canvas->width = width;
    canvas->height = height;

    // Allocate for color_t pixels
    canvas->pixels = malloc(height * sizeof(color_t *));
    if (!canvas->pixels) {
        free(canvas);
        return NULL;
    }
    
    // Allocate each row of pixels
    for (int y = 0; y < height; y++) {
        // calloc for color_t, initializing to 0 (black)
        canvas->pixels[y] = calloc(width, sizeof(color_t));
        if (!canvas->pixels[y]) {
            // Rollback allocation on failure
            for(int k=0; k<y; ++k) free(canvas->pixels[k]);
            free(canvas->pixels);
            free(canvas);
            return NULL;
        }
    }
    return canvas;
}

// Free the memory used by the canvas
void canvas_destroy(canvas_t *canvas) {
    if (!canvas) return;
    for (int y = 0; y < canvas->height; ++y) {
        free(canvas->pixels[y]);
    }
    free(canvas->pixels);
    free(canvas);
}

// Clear the canvas to black (0.0 intensity for all channels)
void canvas_clear(canvas_t *canvas) {
    if (!canvas) return;
    // Clear all color channels to 0 (black)
    for (int y = 0; y < canvas->height; ++y) {
        memset(canvas->pixels[y], 0, canvas->width * sizeof(color_t));
    }
}


// Helper to clamp unsigned char values to [0, 255]
static unsigned char clamp_uchar(float x) {
    return (unsigned char)fminf(255.0f, fmaxf(0.0f, x));
}

// Check if a pixel is within a circular viewport centered in the canvas
static bool is_pixel_in_circular_viewport(canvas_t *canvas, int px, int py) {
    if (canvas == NULL) return false;
    float center_x = canvas->width / 2.0f;
    float center_y = canvas->height / 2.0f;
    float radius = fminf(center_x, center_y);

    float dist_sq = (px - center_x) * (px - center_x) + (py - center_y) * (py - center_y);
    return dist_sq <= (radius * radius);
}

// Helper to add weighted color to a pixel
static void add_weighted_color(canvas_t *c, int px, int py, color_t src_color, float weight) {
    if (px >= 0 && px < c->width && py >= 0 && py < c->height) {
        if (is_pixel_in_circular_viewport(c, px, py)) { // Check viewport for each sub-pixel
            c->pixels[py][px].r = clamp_uchar(c->pixels[py][px].r + src_color.r * weight);
            c->pixels[py][px].g = clamp_uchar(c->pixels[py][px].g + src_color.g * weight);
            c->pixels[py][px].b = clamp_uchar(c->pixels[py][px].b + src_color.b * weight);
        }
    }
}

// set_pixel_f now takes a color_t
void set_pixel_f(canvas_t *canvas, float x, float y, color_t color) {
    if (canvas == NULL) return;

    int x0 = (int)floorf(x);
    int y0 = (int)floorf(y);
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    float dx = x - x0;
    float dy = y - y0;

    float w00 = (1 - dx) * (1 - dy);
    float w10 = dx * (1 - dy);
    float w01 = (1 - dx) * dy;
    float w11 = dx * dy;

    add_weighted_color(canvas, x0, y0, color, w00);
    add_weighted_color(canvas, x1, y0, color, w10);
    add_weighted_color(canvas, x0, y1, color, w01);
    add_weighted_color(canvas, x1, y1, color, w11);
}

// draw_line_f now takes a color_t
void draw_line_f(canvas_t *canvas, float x0, float y0, float x1, float y1, float thickness, color_t color) {
    
    float dx = x1 - x0;
    float dy = y1 - y0;

    // Use DDA algorithm to determine the number of steps needed
    int steps = (int)(fabsf(dx) > fabsf(dy) ? fabsf(dx) : fabsf(dy)) + 1;
    
    float x_inc = dx / steps;
    float y_inc = dy / steps;

    float x = x0;
    float y = y0;
    for (int i = 0; i <= steps; ++i) {
        set_pixel_f(canvas, x, y, color); // Draw with the provided color
        x += x_inc;
        y += y_inc;
    }
}

// Save the canvas to a PGM file (will convert RGB to grayscale)
void canvas_save_pgm(canvas_t *canvas, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "Error: Could not open file %s for writing.\n", filename);
        return;
    }
    fprintf(f, "P2\n%d %d\n255\n", canvas->width, canvas->height); // P2 is for grayscale ASCII PGM

    for (int y = 0; y < canvas->height; ++y) {
        for (int x = 0; x < canvas->width; ++x) {
            // Convert RGB to grayscale for PGM output
            // Simple averaging for grayscale conversion
            int gray_val = (int)((canvas->pixels[y][x].r + canvas->pixels[y][x].g + canvas->pixels[y][x].b) / 3.0f);
            fprintf(f, "%d ", gray_val);
        }
        fprintf(f, "\n");
    }
    fclose(f);
}