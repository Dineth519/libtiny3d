#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h> 
#include "tiny3d.h"

#define LIGHT_BOOST_EXPONENT 0.5f

// Define a small epsilon for floating-point comparisons to avoid division by zero or near-zero w
#define W_CLIP_EPSILON FLT_EPSILON * 100.0f // A slightly larger epsilon for robustness

// Convert a vec3 to vec4 with a specified w component
vec4 project_vertex(vec3 vertex, mat4 model_matrix, mat4 view_matrix, mat4 projection_matrix) {
    
    // Local to World (Model Matrix)
    vec4 v_local_homogeneous = vec4_from_vec3(vertex, 1.0f);
    vec4 v_world_homogeneous = mat4_mul_vec4(model_matrix, v_local_homogeneous);

    // World to Camera (View Matrix)
    vec4 v_camera_homogeneous = mat4_mul_vec4(view_matrix, v_world_homogeneous);

    // Camera to Clip (Projection Matrix)
    vec4 v_clip_homogeneous = mat4_mul_vec4(projection_matrix, v_camera_homogeneous);

    return v_clip_homogeneous;
}

// Structure to hold line data for depth sorting
typedef struct {
    vec3 start_screen;
    vec3 end_screen;
    float average_z; // Average Z of the two projected vertices for sorting
    color_t color;   // Store color for the line
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

// render_wireframe now accepts light_dirs and num_lights
void render_wireframe(canvas_t *canvas, object3d_t *object, mat4 model_matrix, mat4 view_matrix, mat4 projection_matrix, float line_thickness, vec3* light_dirs, int num_lights) {
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

        vec3 v0_object = object->vertices[v_idx0];
        vec3 v1_object = object->vertices[v_idx1];

        // Project vertices to clip space (homogeneous coordinates)
        vec4 p0_clip = project_vertex(v0_object, model_matrix, view_matrix, projection_matrix);
        vec4 p1_clip = project_vertex(v1_object, model_matrix, view_matrix, projection_matrix);

        // Check if either endpoint is behind or at the near clipping plane (w <= 0)
        // If so, skip drawing this line to avoid artifacts.
        if (p0_clip.w <= W_CLIP_EPSILON || p1_clip.w <= W_CLIP_EPSILON) {
            continue; // Skip this line
        }

        // Perform perspective divide to get Normalized Device Coordinates (NDC)
        vec3 p0_ndc = vec3_from_cartesian(p0_clip.x / p0_clip.w, p0_clip.y / p0_clip.w, p0_clip.z / p0_clip.w);
        vec3 p1_ndc = vec3_from_cartesian(p1_clip.x / p1_clip.w, p1_clip.y / p1_clip.w, p1_clip.z / p1_clip.w);

        // Convert NDC to screen coordinates
        vec3 p0_screen;
        p0_screen.x = (p0_ndc.x + 1.0f) * 0.5f * canvas->width;
        p0_screen.y = (1.0f - p0_ndc.y) * 0.5f * canvas->height; // Invert Y-axis for screen coordinates (Y-down)
        p0_screen.z = p0_ndc.z; // Keep Z for depth sorting

        vec3 p1_screen;
        p1_screen.x = (p1_ndc.x + 1.0f) * 0.5f * canvas->width;
        p1_screen.y = (1.0f - p1_ndc.y) * 0.5f * canvas->height; // Invert Y-axis for screen coordinates (Y-down)
        p1_screen.z = p1_ndc.z; // Keep Z for depth sorting

        // Calculate world-space positions for lighting
        vec3 p0_world = vec3_from_vec4(mat4_mul_vec4(model_matrix, vec4_from_vec3(v0_object, 1.0f)));
        vec3 p1_world = vec3_from_vec4(mat4_mul_vec4(model_matrix, vec4_from_vec3(v1_object, 1.0f)));

        // Calculate lighting intensity for this edge
        float intensity = compute_edge_lighting(p0_world, p1_world, light_dirs, num_lights);

        // Clamp intensity to [0, 1]
        intensity = fmaxf(0.0f, fminf(1.0f, intensity));

        // Apply a power function to boost lower intensities, making them more visible.
        intensity = powf(intensity, LIGHT_BOOST_EXPONENT);

        // Map intensity to a grayscale color
        unsigned char color_val = (unsigned char)(intensity * 255.0f);
        color_t edge_color = {color_val, color_val, color_val};

        // Store the line and its average Z for sorting, along with its color
        lines_to_render[line_count].start_screen = p0_screen;
        lines_to_render[line_count].end_screen = p1_screen;
        lines_to_render[line_count].average_z = (p0_screen.z + p1_screen.z) / 2.0f;
        lines_to_render[line_count].color = edge_color; // Store the calculated color
        line_count++;
    }

    // Sort lines by depth (Z-value) from back to front
    qsort(lines_to_render, line_count, sizeof(render_line_t), compare_render_lines);

    // Draw sorted lines
    for (int i = 0; i < line_count; ++i) {
        render_line_t current_line = lines_to_render[i];
        
        // Pass the calculated color to draw_line_f
        draw_line_f(canvas, current_line.start_screen.x, current_line.start_screen.y, 
                     current_line.end_screen.x, current_line.end_screen.y, 
                     line_thickness, current_line.color);
    }

    free(lines_to_render);
}


// Renders a 3D object as a series of points (particles) on the given canvas.
// It projects each vertex and draws a small circle at that screen location.
void render_object_as_points(canvas_t *canvas, object3d_t *object, mat4 model_matrix, mat4 view_matrix, mat4 projection_matrix, float point_size) {
    if (canvas == NULL || object == NULL || object->vertices == NULL) {
        return;
    }

    // Iterate through each vertex in the object
    for (int i = 0; i < object->num_vertices; ++i) {
        vec3 vertex_object = object->vertices[i];

        // Project the vertex to clip space
        vec4 p_clip = project_vertex(vertex_object, model_matrix, view_matrix, projection_matrix);

        // NEW: Check if the point is behind or at the near clipping plane.
        if (p_clip.w <= W_CLIP_EPSILON) {
            continue; // Skip this point
        }

        // Perform perspective divide
        vec3 p_ndc = vec3_from_cartesian(p_clip.x / p_clip.w, p_clip.y / p_clip.w, p_clip.z / p_clip.w);

        // Convert NDC to screen coordinates
        vec3 p_screen;
        p_screen.x = (p_ndc.x + 1.0f) * 0.5f * canvas->width;
        p_screen.y = (1.0f - p_ndc.y) * 0.5f * canvas->height; // Invert Y-axis for screen coordinates (Y-down)
        p_screen.z = p_ndc.z; // Keep Z for depth sorting

        // For points, we'll use a fixed white color, or you could add point lighting
        color_t point_color = {255, 255, 255}; // White color for points

        // Draw a point (small filled circle) at the projected location
        draw_line_f(canvas, p_screen.x, p_screen.y, p_screen.x, p_screen.y, point_size, point_color);
    }
}