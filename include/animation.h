#ifndef ANIMATION_H
#define ANIMATION_H

#include "math3d.h" // Include math3d.h for vec3

// Function to calculate a point on a cubic Bézier curve.
// p0: Start point
// p1: Control point 1
// p2: Control point 2
// p3: End point
// t: Parameter from 0.0 to 1.0 (0.0 at p0, 1.0 at p3)
vec3 bezier_cubic(vec3 p0, vec3 p1, vec3 p2, vec3 p3, float t);

#endif // ANIMATION_H
