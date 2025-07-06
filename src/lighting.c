#include <math.h>  
#include "tiny3d.h"

// Function to compute the lighting intensity for an edge defined by two vertices
float compute_edge_lighting(vec3 v1, vec3 v2, vec3* light_dirs, int num_lights) {
    
    // Ensure we have at least two vertices to define an edge
    vec3 edge_dir = vec3_normalize_fast(vec3_sub(v2, v1));
    float intensity = 0.0f;

    // Iterate through all provided light sources
    for (int i = 0; i < num_lights; ++i) {

        // Normalize the light direction to ensure it is a unit vector
        intensity += fmaxf(0.0f, vec3_dot(edge_dir, light_dirs[i]));
    }

    // If there are no lights, return 0.0 intensity
    if (num_lights > 0) {
        intensity /= (float)num_lights;
    }

    return intensity;
    const float ambient = 0.2f;  // Minimum base light
    return fminf(1.0f, ambient + intensity * (1.0f - ambient));
}