#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "renderer.h"
#include "canvas.h"
#include "math3d.h"


// Projects a 3D point (vertex) from local space to screen space.
vec3 project_vertex(vec3 vertex, mat4 model_matrix, mat4 view_matrix, mat4 projection_matrix, int canvas_width, int canvas_height) {
    
    // Combine model and view matrices to transform to camera space
    mat4 model_view_matrix = mat4_mul(view_matrix, model_matrix);
    vec3 world_vertex = mat4_mul_vec3(model_view_matrix, vertex);

    // Apply projection transformation (perspective or orthographic)
    vec3 projected_vertex = mat4_mul_vec3(projection_matrix, world_vertex);    // Now in clip space

    // Convert to screen space (Normalized Device Coordinates → screen pixels)
    vec3 screen_vertex;
    screen_vertex.x = (projected_vertex.x + 1.0f) * 0.5f * canvas_width;
    screen_vertex.y = (1.0f - projected_vertex.y) * 0.5f * canvas_height; // Invert Y-axis for screen coordinates (Y-down)
    screen_vertex.z = projected_vertex.z; // Keep Z for depth sorting

    // Update spherical coordinates for completeness, although not strictly needed for screen coords
    vec3_update_spherical(&screen_vertex); 

    return screen_vertex;
}


// Structure to hold line data for depth sorting
typedef struct {
    vec3 start_screen;
    vec3 end_screen;
    float average_z; // Average Z of the two projected vertices for sorting
} render_line_t;


// Comparison function for qsort to sort lines by average Z (back to front)
int compare_render_lines(const void *a, const void *b) {
    render_line_t *line_a = (render_line_t *)a;
    render_line_t *line_b = (render_line_t *)b;

    if (line_a->average_z < line_b->average_z) {
        return -1; // line_a comes before line_b (further away)
    } else if (line_a->average_z > line_b->average_z) {
        return 1;  // line_b comes before line_a (closer)
    } else {
        return 0;  // same depth
    }
}


// Checks whether a point lies inside a circular viewpor
bool clip_to_circular_viewport(canvas_t *canvas, float x, float y) {
    float cx = canvas->width / 2.0f;
    float cy = canvas->height / 2.0f;
    float r = (canvas->width < canvas->height ? canvas->width : canvas->height) / 2.0f;
    return ((x - cx)*(x - cx) + (y - cy)*(y - cy)) < (r * r);
}


// Renders a 3D object as a wireframe on the given canvas.
// Uses draw_line_f to draw each edge.
// Lines are sorted by depth (Z) for proper rendering (back to front).
void render_wireframe(canvas_t *canvas, object3d_t *object, mat4 model_matrix, mat4 view_matrix, mat4 projection_matrix, float line_thickness) {
    if (canvas == NULL || object == NULL || object->vertices == NULL || object->indices == NULL) {
        return;
    }

    // Allocate memory to store projected lines for sorting
    render_line_t *lines_to_render = malloc(object->num_indices / 2 * sizeof(render_line_t));
    if (lines_to_render == NULL) {
        perror("Failed to allocate memory for lines_to_render");
        return;
    }
    int line_count = 0;

    // Project all vertices and store lines
    for (int i = 0; i < object->num_indices; i += 2) {
        int v_idx0 = object->indices[i];
        int v_idx1 = object->indices[i + 1];

        if (v_idx0 < 0 || v_idx0 >= object->num_vertices ||
            v_idx1 < 0 || v_idx1 >= object->num_vertices) {
            fprintf(stderr, "Warning: Invalid vertex index in object->indices\n");
            continue;
        }

        // Get the vertex positions in object space
        vec3 v0_object = object->vertices[v_idx0];
        vec3 v1_object = object->vertices[v_idx1];

        // Project them to screen space
        vec3 p0_screen = project_vertex(v0_object, model_matrix, view_matrix, projection_matrix, canvas->width, canvas->height);
        vec3 p1_screen = project_vertex(v1_object, model_matrix, view_matrix, projection_matrix, canvas->width, canvas->height);

        // Store the line and its average Z for sorting
        lines_to_render[line_count].start_screen = p0_screen;
        lines_to_render[line_count].end_screen = p1_screen;
        lines_to_render[line_count].average_z = (p0_screen.z + p1_screen.z) / 2.0f;
        line_count++;
    }

    // Sort lines by depth (Z-value) from back to front
    qsort(lines_to_render, line_count, sizeof(render_line_t), compare_render_lines);

    // Draw sorted lines
    for (int i = 0; i < line_count; ++i) {
        render_line_t current_line = lines_to_render[i];
        
        // Draw lines with fixed intensity of 1.0f (as per canvas.c)
        draw_line_f(canvas, current_line.start_screen.x, current_line.start_screen.y, 
                     current_line.end_screen.x, current_line.end_screen.y, 
                     line_thickness);
    }

    free(lines_to_render);
}


// Renders a 3D object as a series of points (particles) on the given canvas.
// It projects each vertex and draws a small circle at that screen location.
void render_object_as_points(canvas_t *canvas, object3d_t *object, mat4 model_matrix, mat4 view_matrix, mat4 projection_matrix, float point_size) {
    if (canvas == NULL || object == NULL || object->vertices == NULL) {
        return;
    }

    // We don't need depth sorting for points unless specific occlusion is required,
    // as individual points don't occlude each other in the same way lines do.
    // So, we'll just iterate through vertices and draw them.
    for (int i = 0; i < object->num_vertices; ++i) {
        vec3 vertex_object = object->vertices[i];

        // Project the vertex to screen coordinates
        vec3 p_screen = project_vertex(vertex_object, model_matrix, view_matrix, projection_matrix, canvas->width, canvas->height);

        // Draw a point (small filled circle) at the projected location
        // We use the thickness parameter of draw_line_f as the point_size,
        // drawing a "line" from the point to itself.
        // This leverages the circle drawing logic within draw_line_f.
        draw_line_f(canvas, p_screen.x, p_screen.y, p_screen.x, p_screen.y, point_size);
    }
}