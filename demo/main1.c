#include <stdio.h>
#include <stdlib.h>
#include <math.h>     
#include <string.h>   
#include "tiny3d.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

// Function to calculate the distance between two 3D points
float calculateDistance(const float p1[3], const float p2[3]) {
    float dx = p2[0] - p1[0];
    float dy = p2[1] - p1[1];
    float dz = p2[2] - p1[2];
    return sqrtf(dx*dx + dy*dy + dz*dz); // Use sqrtf for float precision
}

// Generates truncated icosahedron by defining its vertices and explicitly finding its edges
void generate_soccerball(object3d_t *obj) {
    
    // Define the vertices of a truncated icosahedron (soccer ball)
    float positions[][3] = {
        {-0.97943199f, 0.0f, -0.201774f}, {-0.97943199f, 0.0f, 0.201774f}, {-0.854729f, -0.32647699f, -0.403548f},
        {-0.854729f, -0.32647699f, 0.403548f}, {-0.854729f, 0.32647699f, -0.403548f}, {-0.854729f, 0.32647699f, 0.403548f},
        {-0.73002601f, -0.652955f, -0.201774f}, {-0.73002601f, -0.652955f, 0.201774f}, {-0.73002601f, 0.652955f, -0.201774f},
        {-0.73002601f, 0.652955f, 0.201774f}, {-0.652955f, -0.201774f, -0.73002601f}, {-0.652955f, -0.201774f, 0.73002601f},
        {-0.652955f, 0.201774f, -0.73002601f}, {-0.652955f, 0.201774f, 0.73002601f}, {-0.403548f, -0.854729f, -0.32647699f},
        {-0.403548f, -0.854729f, 0.32647699f}, {-0.403548f, 0.854729f, -0.32647699f}, {-0.403548f, 0.854729f, 0.32647699f},
        {-0.32647699f, -0.403548f, -0.854729f}, {-0.32647699f, -0.403548f, 0.854729f}, {-0.32647699f, 0.403548f, -0.854729f},
        {-0.32647699f, 0.403548f, 0.854729f}, {-0.201774f, -0.97943199f, 0.0f}, {-0.201774f, -0.73002601f, -0.652955f},
        {-0.201774f, -0.73002601f, 0.652955f}, {-0.201774f, 0.73002601f, -0.652955f}, {-0.201774f, 0.73002601f, 0.652955f},
        {-0.201774f, 0.97943199f, 0.0f}, {0.0f, -0.201774f, -0.97943199f}, {0.0f, -0.201774f, 0.97943199f},
        {0.0f, 0.201774f, -0.97943199f}, {0.0f, 0.201774f, 0.97943199f}, {0.201774f, -0.97943199f, 0.0f},
        {0.201774f, -0.73002601f, -0.652955f}, {0.201774f, -0.73002601f, 0.652955f}, {0.201774f, 0.73002601f, -0.652955f},
        {0.201774f, 0.73002601f, 0.652955f}, {0.201774f, 0.97943199f, 0.0f}, {0.32647699f, -0.403548f, -0.854729f},
        {0.32647699f, -0.403548f, 0.854729f}, {0.32647699f, 0.403548f, -0.854729f}, {0.32647699f, 0.403548f, 0.854729f},
        {0.403548f, -0.854729f, -0.32647699f}, {0.403548f, -0.854729f, 0.32647699f}, {0.403548f, 0.854729f, -0.32647699f},
        {0.403548f, 0.854729f, 0.32647699f}, {0.652955f, -0.201774f, -0.73002601f}, {0.652955f, -0.201774f, 0.73002601f},
        {0.652955f, 0.201774f, -0.73002601f}, {0.652955f, 0.201774f, 0.73002601f}, {0.73002601f, -0.652955f, -0.201774f},
        {0.73002601f, -0.652955f, 0.201774f}, {0.73002601f, 0.652955f, -0.201774f}, {0.73002601f, 0.652955f, 0.201774f},
        {0.854729f, -0.32647699f, -0.403548f}, {0.854729f, -0.32647699f, 0.403548f}, {0.854729f, 0.32647699f, -0.403548f},
        {0.854729f, 0.32647699f, 0.403548f}, {0.97943199f, 0.0f, -0.201774f}, {0.97943199f, 0.0f, 0.201774f}
    };
    const int NUM_VERTICES_ARRAY = 60; // Total number of vertices

    // Allocate memory for vertices in the object3d_t struct
    obj->num_vertices = NUM_VERTICES_ARRAY;
    obj->vertices = (vec3*)malloc(obj->num_vertices * sizeof(vec3));
    if (!obj->vertices) {
        fprintf(stderr, "Failed to allocate vertices memory\n");
        exit(1); // Exit if memory allocation fails
    }

    // Copy vertex data from the local array to the object's vertices
    for (int i = 0; i < obj->num_vertices; ++i) {
        obj->vertices[i].x = positions[i][0];
        obj->vertices[i].y = positions[i][1];
        obj->vertices[i].z = positions[i][2];
    }

    // Allocate memory for indices (edges) in the object3d_t struct
    const int EXPECTED_NUM_EDGES = 90;
    obj->num_indices = 0; // Initialize the count of indices for edges
    // Pre-allocate memory for all expected edges (90 edges * 2 indices per edge)
    obj->indices = (int*)malloc(EXPECTED_NUM_EDGES * 2 * sizeof(int));
    if (!obj->indices) {
        fprintf(stderr, "Failed to allocate indices memory for edges!\n");
        free(obj->vertices); // Clean up previously allocated memory
        exit(1);
    }

    // Initialize the number of indices to 0
    float edgeLength = calculateDistance(positions[0], positions[1]);
    // A small tolerance is needed for floating-point comparisons
    float tolerance = 0.0001f;

    // Iterate through all unique pairs of vertices to find edges
    for (int i = 0; i < NUM_VERTICES_ARRAY; ++i) {
        for (int j = i + 1; j < NUM_VERTICES_ARRAY; ++j) { // Start from i+1 to avoid duplicates (A-B is same as B-A) and self-connections (A-A)
            float dist = calculateDistance(positions[i], positions[j]);

            // If the distance is approximately equal to the edge length, then these two vertices form an edge
            if (fabsf(dist - edgeLength) < tolerance) {
                
                // Store the edge indices in the indices array
                if (obj->num_indices + 2 > EXPECTED_NUM_EDGES * 2) {
                    
                    // If we exceed the expected capacity, reallocate more space
                    fprintf(stderr, "Warning: Exceeded expected edge capacity. Reallocating (should not happen for known geometry).\n");
                    int new_capacity = obj->num_indices + 2 + 10; // Add some buffer
                    int* new_indices = (int*)realloc(obj->indices, new_capacity * sizeof(int));
                    if (new_indices == NULL) {
                        fprintf(stderr, "Memory re-allocation failed for indices during edge finding!\n");
                        free(obj->vertices);
                        free(obj->indices);
                        exit(1);
                    }
                    obj->indices = new_indices;
                }
                obj->indices[obj->num_indices++] = i; // Store first vertex index of the edge
                obj->indices[obj->num_indices++] = j; // Store second vertex index of the edge
            }
        }
    }

    printf("Identified %d unique edges.\n", obj->num_indices / 2); // Each edge consists of 2 indices
}


int main() {
    const int CANVAS_WIDTH = 800;
    const int CANVAS_HEIGHT = 800;
    const int NUM_FRAMES = 240; // Increased number of frames for smoother animation
    const int NUM_OBJECTS = 2; // Number of objects to animate

    // Create Canvas
    canvas_t *canvas = canvas_create(CANVAS_WIDTH, CANVAS_HEIGHT);
    if (!canvas) {
        fprintf(stderr, "Failed to create canvas.\n");
        return 1;
    }

    // Make objects array to hold multiple objects
    object3d_t objects[NUM_OBJECTS];
    for (int i = 0; i < NUM_OBJECTS; ++i) {
        generate_soccerball(&objects[i]);
        printf("Generated object %d with %d vertices and %d edges.\n",
               i, objects[i].num_vertices, objects[i].num_indices / 2);
    }

    // Setup camera and projection matrices
    mat4 view_matrix = mat4_translate(0, 0, -3.5f); // Move camera back along Z-axis
    // Use a perspective projection matrix
    mat4 projection_matrix = mat4_perspective(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 100.0f);

    // Define light sources
    vec3 light_directions[] = {
        vec3_normalize((vec3){1.0f, 1.0f, 1.0f}),   // Light from top-right-front
        vec3_normalize((vec3){-1.0f, -0.5f, 0.5f})  // Light from bottom-left-front
    };
    int num_scene_lights = sizeof(light_directions) / sizeof(light_directions[0]);

    // Define Bézier curve control points for the two objects
    vec3 bezier_p0_obj1 = {-1.5f, -0.5f, 0.0f}; // Start point (shifted left)
    vec3 bezier_p1_obj1 = {-0.5f,  1.0f, -1.0f}; // Control point 1
    vec3 bezier_p2_obj1 = {-0.5f,  1.0f,  1.0f}; // Control point 2
    vec3 bezier_p3_obj1 = {-1.5f, -0.5f, 0.0f}; // End point (same as start for smooth loop)

    // Define Bézier curve control points for the second object
    vec3 bezier_p0_obj2 = { 1.5f,  0.5f, 0.0f}; // Start point (shifted right)
    vec3 bezier_p1_obj2 = { 0.5f, -1.0f, 1.0f};
    vec3 bezier_p2_obj2 = { 0.5f, -1.0f, -1.0f};
    vec3 bezier_p3_obj2 = { 1.5f,  0.5f, 0.0f}; // End point (same as start for smooth loop)

    // Scaling factor for the smaller object
    const float SMALL_OBJECT_SCALE = 0.6f; // Make one object 60% of its original size


    // Render Animation Loop
    for (int frame = 0; frame < NUM_FRAMES; ++frame) {
        canvas_clear(canvas); // Clear the canvas for the new frame

        // Calculate linear animation parameter t (0.0 to 1.0 over the animation loop)
        float t_anim = (float)frame / (NUM_FRAMES - 1);

        // Smooth the animation using a cosine easing function
        float t_smooth = (1.0f - cosf(t_anim * M_PI)) / 2.0f;

        // Animate and render Object 1
        vec3 obj1_position = bezier_cubic(bezier_p0_obj1, bezier_p1_obj1, bezier_p2_obj1, bezier_p3_obj1, t_anim);
        // Use the smoothed time 't_smooth' for rotation calculation
        float obj1_rotation_angle = 2.0f * M_PI * t_smooth;
        mat4 obj1_model_matrix = mat4_translate(obj1_position.x, obj1_position.y, obj1_position.z);
        obj1_model_matrix = mat4_mul(obj1_model_matrix, mat4_rotate_xyz(obj1_rotation_angle * 0.5f, obj1_rotation_angle, 0.0f));
        // UPDATED: Increased line thickness for smoother appearance
        render_wireframe(canvas, &objects[0], obj1_model_matrix, view_matrix, projection_matrix, 1.5f, light_directions, num_scene_lights);

        // Animate and render Object 2 (smaller and on a non-colliding path)
        vec3 obj2_position = bezier_cubic(bezier_p0_obj2, bezier_p1_obj2, bezier_p2_obj2, bezier_p3_obj2, t_anim);
        // Use the smoothed time 't_smooth' for the second object's rotation as well
        float obj2_rotation_angle = -2.0f * M_PI * t_smooth;
        mat4 obj2_model_matrix = mat4_translate(obj2_position.x, obj2_position.y, obj2_position.z);
        // Apply scaling for Object 2
        obj2_model_matrix = mat4_mul(obj2_model_matrix, mat4_scale(SMALL_OBJECT_SCALE, SMALL_OBJECT_SCALE, SMALL_OBJECT_SCALE));
        obj2_model_matrix = mat4_mul(obj2_model_matrix, mat4_rotate_xyz(obj2_rotation_angle * 0.7f, obj2_rotation_angle, 0.0f));
        // UPDATED: Increased line thickness for smoother appearance
        render_wireframe(canvas, &objects[1], obj2_model_matrix, view_matrix, projection_matrix, 1.5f, light_directions, num_scene_lights);


        // Save each frame as a PGM image
        char filename[64];
        snprintf(filename, sizeof(filename), "../../tests/visual_tests/frame_%03d.pgm", frame);
        canvas_save_pgm(canvas, filename);

        printf("Saved frame %d/%d\n", frame + 1, NUM_FRAMES);
    }

    // Free allocated memory for all objects
    for (int i = 0; i < NUM_OBJECTS; ++i) {
        free(objects[i].vertices);
        free(objects[i].indices);
    }
    canvas_destroy(canvas);

    printf("Done. Rendered frames are in the tests/visual_tests directory.\n");

    return 0;
}
