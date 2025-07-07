#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "tiny3d.h"

#define NUM_VERTICES 8
#define NUM_EDGES 12
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Generate a cube geometry with vertices and edges
void generate_cube(object3d_t *obj) {
    float positions[NUM_VERTICES][3] = {
        {-1, -1, -1}, { 1, -1, -1}, { 1,  1, -1}, {-1,  1, -1},
        {-1, -1,  1}, { 1, -1,  1}, { 1,  1,  1}, {-1,  1,  1}
    };

    // Initialize the object
    obj->num_vertices = NUM_VERTICES;
    obj->vertices = malloc(NUM_VERTICES * sizeof(vec3));
    for (int i = 0; i < NUM_VERTICES; ++i)
        obj->vertices[i] = vec3_from_cartesian(positions[i][0], positions[i][1], positions[i][2]);

    // Define edges connecting the vertices
    int edges[NUM_EDGES * 2] = {
        0,1, 1,2, 2,3, 3,0,    // bottom face edges
        4,5, 5,6, 6,7, 7,4,    // top face edges
        0,4, 1,5, 2,6, 3,7     // vertical edges connecting top and bottom
    };

    // Initialize indices for edges
    obj->num_indices = NUM_EDGES * 2;
    obj->indices = malloc(obj->num_indices * sizeof(int));
    memcpy(obj->indices, edges, sizeof(edges));
}

int main() {
    // Set up canvas dimensions and number of frames
    const int WIDTH = 800;
    const int HEIGHT = 800;
    const int NUM_FRAMES = 120;

    // Create a canvas for rendering
    canvas_t *canvas = canvas_create(WIDTH, HEIGHT);
    if (!canvas) {
        fprintf(stderr, "Failed to create canvas.\n");
        return 1;
    }

    // Set the background color to white
    object3d_t cube;
    generate_cube(&cube);

    // Camera: move back along z-axis
    mat4 view = mat4_translate(0, 0, -5.0f);

    // Perspective projection
    mat4 proj = mat4_perspective(-1, 1, -1, 1, 1, 100);

    // Lighting direction: simple directional light pointing from camera to scene
    vec3 light_dirs[1] = { vec3_from_cartesian(0.0f, 0.0f, -1.0f) };
    int num_lights = 1;

    // Render frames of the rotating cube
    for (int frame = 0; frame < NUM_FRAMES; ++frame) {
        canvas_clear(canvas);

        // Calculate rotation angle based on frame number
        float angle = 2.0f * M_PI * frame / NUM_FRAMES;
        mat4 model = mat4_rotate_xyz(angle * 0.5f, angle, 0);

        // Render the cube wireframe with thickness 1.0 and lighting
        render_wireframe(canvas, &cube, model, view, proj, 1.0f, light_dirs, num_lights);

        // Save each frame as a PGM image file
        char filename[64];
        snprintf(filename, sizeof(filename), "cube_frame_%03d.pgm", frame);
        canvas_save_pgm(canvas, filename);
        printf("Saved %s\n", filename);
    }

    free(cube.vertices);
    free(cube.indices);
    canvas_destroy(canvas);

    printf("Done rendering rotating cube.\n");
    return 0;
}