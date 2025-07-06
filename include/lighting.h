#ifndef LIGHTING_H
#define LIGHTING_H

#include "tiny3d.h" // Assumes math3d.h defines vec3

// Compute Lambertian lighting intensity for a single edge given multiple light directions
float compute_edge_lighting(vec3 v1, vec3 v2, vec3* light_dirs, int num_lights);

#endif