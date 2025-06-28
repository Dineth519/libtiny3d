#include <stdio.h>
#include <math.h>
#include "canvas.h"

#define PI 3.14159265358979323846

int main() {
    int width = 400, height = 400;

    // Create a canvas of size 400x400 pixels
    canvas_t *canvas = canvas_create(width, height);

    // Calculate the center of the canvas
    float cx = width / 2.0f;
    float cy = height / 2.0f;

    // Set the radius to 45% of the smaller dimension
    float radius = (width < height ? width : height) * 0.45f;

    // Loop through angles from 0 to 345 degrees in steps of 15
    for (int deg = 0; deg < 360; deg += 15) {
        float rad = deg * PI / 180.0f;          // Convert angle from degrees to radians
        
        // Calculate the endpoint of the line on the circle
        float x1 = cx + radius * cosf(rad);
        float y1 = cy + radius * sinf(rad);

        // Draw a line from the center to the point on the circle (thickness = 4.0)
        draw_line_f(canvas, cx, cy, x1, y1, 4.0f); // thickness = 4.0
    }

    // Save the canvas to a PGM file
    canvas_save_pgm(canvas, "clockface.pgm");

    // Free the memory used by the canvas
    canvas_destroy(canvas);

    printf("Saved clockface.pgm\n");
 
    return 0;
}