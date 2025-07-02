#ifndef RENDERER_H
#define RENDERER_H

#include "canvas.h"
#include "math3d.h"
#include <stdbool.h>

// Structure to represent a 3D object for wireframe rendering
typedef struct {
    vec3* vertices;     // Array of vertices in object space
    int num_vertices;
    int* indices;       // Array of vertex indices defining edges (pairs of indices)
    int num_indices;    // Total number of indices (should be even)
} object3d_t;

// Projects a 3D point (vertex) from local space to screen space.
// Applies Local -> World -> Camera -> Projection -> Screen transformations.
vec3 project_vertex(vec3 vertex, mat4 model_matrix, mat4 view_matrix, mat4 projection_matrix, int canvas_width, int canvas_height);

// Renders a 3D object as a wireframe on the given canvas.
// Uses draw_line_f to draw each edge.
// Lines are sorted by depth (Z) for proper rendering (back to front).
void render_wireframe(canvas_t *canvas, object3d_t *object, mat4 model_matrix, mat4 view_matrix, mat4 projection_matrix, float line_thickness);

/**
 * @brief Renders a 3D object as a series of points (particles) on the given canvas.
 *
 * This function projects each vertex of the object to screen space and draws a point
 * at that location. It does NOT draw lines between vertices.
 *
 * @param canvas The canvas to draw on.
 * @param object The 3D object to render.
 * @param model_matrix The object's model (local to world) transformation matrix.
 * @param view_matrix The camera's view (world to camera) transformation matrix.
 * @param projection_matrix The projection (camera to clip) transformation matrix.
 * @param point_size The size of each point in pixels.
 */

void render_object_as_points(canvas_t *canvas, object3d_t *object, mat4 model_matrix, mat4 view_matrix, mat4 projection_matrix, float point_size);


/**
 * @brief Checks if a screen-space point lies within a circular viewport.
 *
 * Useful for masking rendering to a circular region of the canvas.
 *
 * @param canvas The rendering canvas.
 * @param x X coordinate of the point in pixels.
 * @param y Y coordinate of the point in pixels.
 * @return true if the point lies within the circular viewport; false otherwise.
 */

bool clip_to_circular_viewport(canvas_t *canvas, float x, float y);

#endif // RENDERER_H