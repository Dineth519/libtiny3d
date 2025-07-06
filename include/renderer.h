#include "tiny3d.h"

#ifndef RENDERER_H
#define RENDERER_H


// Structure to represent a 3D object for wireframe rendering
typedef struct {
    vec3* vertices;     // Array of vertices in object space
    int num_vertices;
    int* indices;       // Array of vertex indices defining edges (pairs of indices)
    int num_indices;    // Total number of indices (should be even)
} object3d_t;

// Structure to hold line data for depth sorting
vec4 project_vertex(vec3 vertex, mat4 model_matrix, mat4 view_matrix, mat4 projection_matrix);

// Comparison function for qsort to sort lines by average Z (back to front)
void render_wireframe(canvas_t *canvas, object3d_t *object, mat4 model_matrix, mat4 view_matrix, mat4 projection_matrix, float line_thickness, vec3* light_dirs, int num_lights);

// Renders a 3D object as a series of points (particles) on the given canvas.
void render_object_as_points(canvas_t *canvas, object3d_t *object, mat4 model_matrix, mat4 view_matrix, mat4 projection_matrix, float point_size);

// Draws a point (small filled circle) at the projected location
void draw_line_f(canvas_t *canvas, float x0, float y0, float x1, float y1, float thickness, color_t color);
#endif 