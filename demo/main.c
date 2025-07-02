#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "canvas.h"
#include "math3d.h"
#include "renderer.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ------------------------------------------------------------
// Function to generate the icosahedron geometry (vertices + edges)
// ------------------------------------------------------------
void generate_icosahedron(object3d_t *obj) {
    float positions[][3] = {
        {-0.8506508, 0.0, 0.5257311}, {0.0, 0.5257311, 0.8506508}, {-0.5257311, 0.8506508, 0.0},
        {0.5257311, 0.8506508, 0.0}, {0.0, 0.5257311, -0.8506508}, {-0.8506508, 0.0, -0.5257311},
        {0.8506508, 0.0, 0.5257311}, {0.0, -0.5257311, 0.8506508}, {-0.5257311, -0.8506508, 0.0},
        {0.0, -0.5257311, -0.8506508}, {0.8506508, 0.0, -0.5257311}, {0.5257311, -0.8506508, 0.0}
    };

    int cells[][3] = {
        {0,1,2}, {1,3,2}, {3,4,2}, {4,5,2}, {5,0,2}, {1,6,3}, {0,7,1}, {5,8,0},
        {4,9,5}, {3,10,4}, {6,7,11}, {7,8,11}, {8,9,11}, {9,10,11}, {10,6,11},
        {6,1,7}, {7,0,8}, {8,5,9}, {9,4,10}, {10,3,6}
    };

    obj->num_vertices = 12;
    obj->vertices = malloc(obj->num_vertices * sizeof(vec3));
    if (!obj->vertices) {
        fprintf(stderr, "Failed to allocate vertices memory\n");
        exit(1);
    }
    for (int i = 0; i < obj->num_vertices; ++i) {
        obj->vertices[i].x = positions[i][0];
        obj->vertices[i].y = positions[i][1];
        obj->vertices[i].z = positions[i][2];
    }

    int temp_indices[90]; 
    int edge_count = 0;

    for (int i = 0; i < 20; ++i) {
        int v0 = cells[i][0], v1 = cells[i][1], v2 = cells[i][2];
        int edges[][2] = {{v0, v1}, {v1, v2}, {v2, v0}};
        for (int j = 0; j < 3; ++j) {
            int a = edges[j][0], b = edges[j][1];
            if (a > b) { int t = a; a = b; b = t; }
            int exists = 0;
            for (int k = 0; k < edge_count; k += 2) {
                if (temp_indices[k] == a && temp_indices[k+1] == b) {
                    exists = 1;
                    break;
                }
            }
            if (!exists) {
                temp_indices[edge_count++] = a;
                temp_indices[edge_count++] = b;
            }
        }
    }

    obj->num_indices = edge_count;
    obj->indices = malloc(obj->num_indices * sizeof(int));
    if (!obj->indices) {
        fprintf(stderr, "Failed to allocate indices memory\n");
        free(obj->vertices);
        exit(1);
    }
    memcpy(obj->indices, temp_indices, obj->num_indices * sizeof(int));
}

int main() {
    const int CANVAS_WIDTH = 800;
    const int CANVAS_HEIGHT = 800;

    // ==========================================================
    // PART 1: DRAW CLOCK FACE (YOUR ORIGINAL CODE)
    // ==========================================================
    canvas_t *canvas = canvas_create(CANVAS_WIDTH, CANVAS_HEIGHT);
    if (!canvas) {
        fprintf(stderr, "Failed to create canvas.\n");
        return 1;
    }

    float cx = CANVAS_WIDTH / 2.0f;
    float cy = CANVAS_HEIGHT / 2.0f;
    float radius = (CANVAS_WIDTH < CANVAS_HEIGHT ? CANVAS_WIDTH : CANVAS_HEIGHT) * 0.45f;

    // Loop through angles from 0 to 345 degrees in steps of 15
    for (int deg = 0; deg < 360; deg += 15) {
        float rad = deg * M_PI / 180.0f; // Convert to radians
        float x1 = cx + radius * cosf(rad);
        float y1 = cy + radius * sinf(rad);
        draw_line_f(canvas, cx, cy, x1, y1, 4.0f); // Draw line from center
    }

    canvas_save_pgm(canvas, "clockface.pgm");
    printf("Saved clockface.pgm\n");

    // ==========================================================
    // PART 2: DRAW ICOSAHEDRON ANIMATION
    // ==========================================================
    object3d_t icosahedron;
    generate_icosahedron(&icosahedron);
    printf("Generated icosahedron with %d vertices and %d edges.\n",
           icosahedron.num_vertices, icosahedron.num_indices / 2);

    mat4 view_matrix = mat4_translate(0, 0, -3);
    mat4 projection_matrix = mat4_perspective(-1, 1, -1, 1, 1, 100);

    const int NUM_FRAMES = 120;
    for (int frame = 0; frame < NUM_FRAMES; ++frame) {
        canvas_clear(canvas);

        float angle = 2.0f * M_PI * frame / NUM_FRAMES;
        mat4 model_matrix = mat4_rotate_xyz(angle * 0.3f, angle, 0);

        render_wireframe(canvas, &icosahedron, model_matrix, view_matrix, projection_matrix, 1.0f);

        char filename[64];
        snprintf(filename, sizeof(filename), "frame_%03d.pgm", frame);
        canvas_save_pgm(canvas, filename);
        printf("Saved %s\n", filename);
    }

    // ==========================================================
    // CLEANUP
    // ==========================================================
    free(icosahedron.vertices);
    free(icosahedron.indices);
    canvas_destroy(canvas);

    printf("Done. Rendered clock face and animation frames.\n");
    return 0;
}