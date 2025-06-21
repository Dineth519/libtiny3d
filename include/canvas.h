#ifndef CANVAS_H
#define CANVAS_H

typedef struct {
    int width;                // canvas width in pixels
    int height;               // canvas height in pixels
    float **pixels;            // 2d array for representing brightness at each pixel   
}canvas_t;

// Create a new canvas with given width and height
canvas_t *canvas_create(int width, int height);

// Free the memory used by the canvas
void canvas_destroy(canvas_t *canvas);

// Intensity is spread to 4 nearest pixels using bilinear filtering
void set_pixel_f(canvas_t *canvas, float x, float y, float intensity);

// Uses DDA algorithm to draw a smooth line from (x0,y0) to (x1,y1) with given thickness
void draw_line_f(canvas_t *canvas, float x0, float y0, float x1, float y1, float thickness);

#endif