#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "canvas.h"
#include "math3d.h"
#include "renderer.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

float calculateDistance(const float p1[3], const float p2[3]) {
    float dx = p2[0] - p1[0];
    float dy = p2[1] - p1[1];
    float dz = p2[2] - p1[2];
    return sqrtf(dx*dx + dy*dy + dz*dz);
}

void generate_soccerball(object3d_t *obj) {
    float positions[][3] = { 
        {-0.97943199f, 0.0f, -0.201774f}, {-0.97943199f, 0.0f, 0.201774f}, 
        {-0.854729f, -0.32647699f, -0.403548f}, {-0.854729f, -0.32647699f, 0.403548f},
        {-0.854729f, 0.32647699f, -0.403548f}, {-0.854729f, 0.32647699f, 0.403548f},
        {-0.73002601f, -0.652955f, -0.201774f}, {-0.73002601f, -0.652955f, 0.201774f},
        {-0.73002601f, 0.652955f, -0.201774f}, {-0.73002601f, 0.652955f, 0.201774f},
        {-0.652955f, -0.201774f, -0.73002601f}, {-0.652955f, -0.201774f, 0.73002601f},
        {-0.652955f, 0.201774f, -0.73002601f}, {-0.652955f, 0.201774f, 0.73002601f},
        {-0.403548f, -0.854729f, -0.32647699f}, {-0.403548f, -0.854729f, 0.32647699f},
        {-0.403548f, 0.854729f, -0.32647699f}, {-0.403548f, 0.854729f, 0.32647699f},
        {-0.32647699f, -0.403548f, -0.854729f}, {-0.32647699f, -0.403548f, 0.854729f},
        {-0.32647699f, 0.403548f, -0.854729f}, {-0.32647699f, 0.403548f, 0.854729f},
        {-0.201774f, -0.97943199f, 0.0f}, {-0.201774f, -0.73002601f, -0.652955f},
        {-0.201774f, -0.73002601f, 0.652955f}, {-0.201774f, 0.73002601f, -0.652955f},
        {-0.201774f, 0.73002601f, 0.652955f}, {-0.201774f, 0.97943199f, 0.0f},
        {0.0f, -0.201774f, -0.97943199f}, {0.0f, -0.201774f, 0.97943199f},
        {0.0f, 0.201774f, -0.97943199f}, {0.0f, 0.201774f, 0.97943199f},
        {0.201774f, -0.97943199f, 0.0f}, {0.201774f, -0.73002601f, -0.652955f},
        {0.201774f, -0.73002601f, 0.652955f}, {0.201774f, 0.73002601f, -0.652955f},
        {0.201774f, 0.73002601f, 0.652955f}, {0.201774f, 0.97943199f, 0.0f},
        {0.32647699f, -0.403548f, -0.854729f}, {0.32647699f, -0.403548f, 0.854729f},
        {0.32647699f, 0.403548f, -0.854729f}, {0.32647699f, 0.403548f, 0.854729f},
        {0.403548f, -0.854729f, -0.32647699f}, {0.403548f, -0.854729f, 0.32647699f},
        {0.403548f, 0.854729f, -0.32647699f}, {0.403548f, 0.854729f, 0.32647699f},
        {0.652955f, -0.201774f, -0.73002601f}, {0.652955f, -0.201774f, 0.73002601f},
        {0.652955f, 0.201774f, -0.73002601f}, {0.652955f, 0.201774f, 0.73002601f},
        {0.73002601f, -0.652955f, -0.201774f}, {0.73002601f, -0.652955f, 0.201774f},
        {0.73002601f, 0.652955f, -0.201774f}, {0.73002601f, 0.652955f, 0.201774f},
        {0.854729f, -0.32647699f, -0.403548f}, {0.854729f, -0.32647699f, 0.403548f},
        {0.854729f, 0.32647699f, -0.403548f}, {0.854729f, 0.32647699f, 0.403548f},
        {0.97943199f, 0.0f, -0.201774f}, {0.97943199f, 0.0f, 0.201774f}
    };
    const int NUM_VERTICES = 60;

    obj->num_vertices = NUM_VERTICES;
    obj->vertices = (vec3*)malloc(NUM_VERTICES * sizeof(vec3));
    for (int i = 0; i < NUM_VERTICES; ++i) {
        obj->vertices[i].x = positions[i][0];
        obj->vertices[i].y = positions[i][1];
        obj->vertices[i].z = positions[i][2];
    }

    float edgeLength = calculateDistance(positions[0], positions[1]);
    float tolerance = 0.0001f;
    obj->indices = (int*)malloc(180 * sizeof(int));
    obj->num_indices = 0;
    for (int i = 0; i < NUM_VERTICES; ++i) {
        for (int j = i + 1; j < NUM_VERTICES; ++j) {
            if (fabsf(calculateDistance(positions[i], positions[j]) - edgeLength) < tolerance) {
                obj->indices[obj->num_indices++] = i;
                obj->indices[obj->num_indices++] = j;
            }
        }
    }
    printf("Soccerball: %d vertices, %d edges\n", obj->num_vertices, obj->num_indices/2);
}

int main() {
    const int CANVAS_WIDTH = 800;
    const int CANVAS_HEIGHT = 800;

    // =========================
    // PART 1: DRAW CLOCK FACE
    // =========================
    canvas_t *clock_canvas = canvas_create(CANVAS_WIDTH, CANVAS_HEIGHT);
    if (!clock_canvas) {
        fprintf(stderr, "Failed to create clock canvas.\n");
        return 1;
    }

    float cx = CANVAS_WIDTH / 2.0f;
    float cy = CANVAS_HEIGHT / 2.0f;
    float radius = (CANVAS_WIDTH < CANVAS_HEIGHT ? CANVAS_WIDTH : CANVAS_HEIGHT) * 0.45f;

    for (int deg = 0; deg < 360; deg += 15) {
        float rad = deg * M_PI / 180.0f;
        float x1 = cx + radius * cosf(rad);
        float y1 = cy + radius * sinf(rad);
        draw_line_f(clock_canvas, cx, cy, x1, y1, 4.0f);
    }
    canvas_save_pgm(clock_canvas, "clockface.pgm");
    printf("Saved clockface.pgm\n");
    canvas_destroy(clock_canvas);

    // =========================
    // PART 2: RENDER SOCCERBALL
    // =========================
    canvas_t *soccer_canvas = canvas_create(CANVAS_WIDTH, CANVAS_HEIGHT);
    if (!soccer_canvas) {
        fprintf(stderr, "Failed to create soccer canvas.\n");
        return 1;
    }

    object3d_t soccerball;
    generate_soccerball(&soccerball);

    mat4 view_matrix = mat4_translate(0, 0, -2.5f);
    mat4 projection_matrix = mat4_perspective(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 100.0f);

    const int NUM_FRAMES = 120;
    for (int frame = 0; frame < NUM_FRAMES; ++frame) {
        canvas_clear(soccer_canvas);
        float angle = 2.0f * M_PI * frame / NUM_FRAMES;
        mat4 model_matrix = mat4_rotate_xyz(angle * 0.3f, angle, 0.0f);

        render_wireframe(soccer_canvas, &soccerball, model_matrix, view_matrix, projection_matrix, 1.0f);

        char filename[64];
        snprintf(filename, sizeof(filename), "frame_%03d.pgm", frame);
        canvas_save_pgm(soccer_canvas, filename);
        printf("Saved %s\n", filename);
    }

    free(soccerball.vertices);
    free(soccerball.indices);
    canvas_destroy(soccer_canvas);

    printf("Done!\n");
    return 0;
}
