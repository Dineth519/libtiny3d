#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "tiny3d.h"

#define NUM_VERTICES 8
#define NUM_EDGES 12
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Define the cube vertices and edges
float positions[NUM_VERTICES][3] = {
    {-1, -1, -1}, { 1, -1, -1}, { 1,  1, -1}, {-1,  1, -1},
    {-1, -1,  1}, { 1, -1,  1}, { 1,  1,  1}, {-1,  1,  1}
};

// Define the edges connecting the vertices
int edges[NUM_EDGES * 2] = {
    0,1, 1,2, 2,3, 3,0,
    4,5, 5,6, 6,7, 7,4,
    0,4, 1,5, 2,6, 3,7
};

int main() {
    // Create a canvas to draw on
    const int WIDTH = 800;
    const int HEIGHT = 800;

    // Initialize the canvas
    canvas_t *canvas = canvas_create(WIDTH, HEIGHT);
    if (!canvas) {
        fprintf(stderr, "Failed to create canvas\n");
        return 1;
    }

    // Setup camera/view and projection
    mat4 view = mat4_translate(0, 0, -5.0f);
    mat4 proj = mat4_perspective(-1, 1, -1, 1, 1, 100);

    // Model rotation angle
    float angle = M_PI / 4; // 45 degrees
    mat4 model = mat4_rotate_xyz(angle, angle * 0.5f, 0);

    // Project vertices to screen coords
    float projected_x[NUM_VERTICES];
    float projected_y[NUM_VERTICES];
    int visible[NUM_VERTICES];

    // Initialize visibility and projection arrays
    for (int i = 0; i < NUM_VERTICES; ++i) {
        vec3 v = vec3_from_cartesian(positions[i][0], positions[i][1], positions[i][2]);
        vec4 clip_coords = project_vertex(v, model, view, proj);

        // perspective divide
        if (clip_coords.w == 0) {
            visible[i] = 0;
            projected_x[i] = -1000;
            projected_y[i] = -1000;
            continue;
        }
        float ndc_x = clip_coords.x / clip_coords.w;
        float ndc_y = clip_coords.y / clip_coords.w;
        float ndc_z = clip_coords.z / clip_coords.w;

        // Check if point is in front of camera (usually ndc_z in [-1,1])
        visible[i] = (clip_coords.w > 0 && ndc_z >= -1 && ndc_z <= 1);

        // Map NDC to screen pixels
        projected_x[i] = (ndc_x + 1.0f) * 0.5f * WIDTH;
        projected_y[i] = (1.0f - (ndc_y + 1.0f) * 0.5f) * HEIGHT; // flip y axis for screen space
    }

    canvas_clear(canvas);

    color_t white = {255, 255, 255};

    // Draw edges
    for (int e = 0; e < NUM_EDGES; ++e) {
        int i0 = edges[e * 2];
        int i1 = edges[e * 2 + 1];

        if (!visible[i0] || !visible[i1]) continue;

        draw_line_f(canvas, projected_x[i0], projected_y[i0], projected_x[i1], projected_y[i1], 1.0f, white);
    }

    canvas_save_pgm(canvas, "cube.pgm");
    printf("Save cube.pgm\n");

    canvas_destroy(canvas);

    return 0;
}