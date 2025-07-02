#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#include "canvas.h"
#include "math3d.h"
#include "renderer.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// --- Soccer Ball Generation ---

// Helper function to add a vertex to a list, ensuring uniqueness.
// Returns the index of the vertex (either new or existing).
static int add_unique_vertex(vec3* vertices, int* vertex_count, vec3 v) {
    for (int i = 0; i < *vertex_count; i++) {
        if (vec3_length(vec3_sub(vertices[i], v)) < 1e-5f) {
            return i;
        }
    }
    vertices[*vertex_count] = v;
    return (*vertex_count)++;
}

// Generates the geometry for a truncated icosahedron (soccer ball).
void generate_soccer_ball(object3d_t *obj) {
    // Vertices of a regular icosahedron (used for truncation)
    const float t = (1.0f + sqrtf(5.0f)) / 2.0f;
    vec3 ico_verts[] = {
        {-1, t, 0}, {1, t, 0}, {-1, -t, 0}, {1, -t, 0},
        {0, -1, t}, {0, 1, t}, {0, -1, -t}, {0, 1, -t},
        {t, 0, -1}, {t, 0, 1}, {-t, 0, -1}, {-t, 0, 1}
    };
    for(int i=0; i<12; ++i) ico_verts[i] = vec3_normalize_fast(ico_verts[i]);
    
    vec3 temp_vertices[60];
    int vertex_count = 0;

    // Truncate by creating two vertices on each of the 30 icosahedron edges.
    for(int i=0; i<12; ++i) {
        for(int j=i+1; j<12; ++j) {
            float dist = vec3_length(vec3_sub(ico_verts[i], ico_verts[j]));
            // Find pairs of vertices that form an edge on a normalized icosahedron (distance is ~2.0)
            if (dist < 2.1f && dist > 1.9f) { 
                 vec3 p1 = vec3_add(vec3_scale(ico_verts[i], 2.0f/3.0f), vec3_scale(ico_verts[j], 1.0f/3.0f));
                 vec3 p2 = vec3_add(vec3_scale(ico_verts[i], 1.0f/3.0f), vec3_scale(ico_verts[j], 2.0f/3.0f));
                 add_unique_vertex(temp_vertices, &vertex_count, p1);
                 add_unique_vertex(temp_vertices, &vertex_count, p2);
            }
        }
    }

    // Allocate final vertices
    obj->num_vertices = vertex_count;
    obj->vertices = malloc(obj->num_vertices * sizeof(vec3));
    memcpy(obj->vertices, temp_vertices, obj->num_vertices * sizeof(vec3));

    // Generate the 90 edges by connecting close vertices
    int edge_count = 0;
    int* temp_indices = malloc(180 * sizeof(int)); // 90 edges * 2 indices
    float edge_threshold_sq = 0.45f; // Heuristic based on 1/3 edge length

    for (int i = 0; i < obj->num_vertices; i++) {
        for (int j = i + 1; j < obj->num_vertices; j++) {
            float dist_sq = vec3_length(vec3_sub(obj->vertices[i], obj->vertices[j]));
            dist_sq *= dist_sq;
            if (dist_sq < edge_threshold_sq) {
                temp_indices[edge_count++] = i;
                temp_indices[edge_count++] = j;
            }
        }
    }
    
    obj->num_indices = edge_count;
    obj->indices = malloc(obj->num_indices * sizeof(int));
    memcpy(obj->indices, temp_indices, obj->num_indices * sizeof(int));
    
    free(temp_indices);
}

int main() {
    const int CANVAS_WIDTH = 800;
    const int CANVAS_HEIGHT = 800;
    const int NUM_FRAMES = 120; // Number of frames for the animation

    // 1. Setup Canvas
    canvas_t *canvas = canvas_create(CANVAS_WIDTH, CANVAS_HEIGHT);
    if (!canvas) {
        fprintf(stderr, "Failed to create canvas.\n");
        return 1;
    }

    // 2. Create 3D Object
    object3d_t soccer_ball;
    generate_soccer_ball(&soccer_ball);
    printf("Generated soccer ball with %d vertices and %d edges.\n", soccer_ball.num_vertices, soccer_ball.num_indices / 2);

    // 3. Setup Transformation Matrices
    // CHANGE: Moved camera from -5 to -3 on the Z-axis to make the object appear larger.
    mat4 view_matrix = mat4_translate(0, 0, -3); 
    mat4 projection_matrix = mat4_perspective(-1, 1, -1, 1, 1, 100);

    // 4. Animation Loop
    for (int frame = 0; frame < NUM_FRAMES; ++frame) {
        canvas_clear(canvas);

        // Animate rotation
        float angle = 2.0f * M_PI * frame / NUM_FRAMES;
        mat4 model_matrix = mat4_rotate_xyz(angle * 0.3f, angle, 0);

        // Render the object
        render_wireframe(canvas, &soccer_ball, model_matrix, view_matrix, projection_matrix, 1.0f);

        // Save frame to file
        char filename[64];
        snprintf(filename, sizeof(filename), "frame_%03d.pgm", frame); // Save in current folder
        canvas_save_pgm(canvas, filename);

        printf("Saved frame %d/%d\n", frame + 1, NUM_FRAMES);
    }

    // 5. Cleanup
    free(soccer_ball.vertices);
    free(soccer_ball.indices);
    canvas_destroy(canvas);

    printf("Done. Rendered frames can be found in the current directory.\n");

    return 0;
}
