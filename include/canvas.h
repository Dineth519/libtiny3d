#ifndef CANVAS_H
#define CANVAS_H

#include <stdbool.h>

// Define a color structure for RGB values
typedef struct {
    unsigned char r, g, b;
} color_t;

// Define a color_t with 0 intensity for black
typedef struct {
    int width;                
    int height; 
    color_t **pixels;         // 2d array for representing color at each pixel
} canvas_t;

// Create a new canvas with given width and height
canvas_t *canvas_create(int width, int height);

// Free the memory used by the canvas
void canvas_destroy(canvas_t *canvas);

// Clear the canvas to black (0.0 intensity for all channels)
void canvas_clear(canvas_t *canvas);

// Sets a pixel with bilinear filtering, spreading intensity to 4 nearest pixels
void set_pixel_f(canvas_t *canvas, float x, float y, color_t color);

// draw_line_f now takes a color_t
// Uses DDA algorithm to draw a smooth line from (x0,y0) to (x1,y1) with given thickness
void draw_line_f(canvas_t *canvas, float x0, float y0, float x1, float y1, float thickness, color_t color);

// Save the canvas to a PGM file (will convert RGB to grayscale)
void canvas_save_pgm(canvas_t *canvas, const char *filename);

#endif