#include <math.h>
#include "animation.h"
#include "math3d.h" 

// Function to evaluate a cubic Bézier curve at parameter t
vec3 bezier_cubic(vec3 p0, vec3 p1, vec3 p2, vec3 p3, float t) {
    // Ensure t is clamped between 0 and 1
    t = fmaxf(0.0f, fminf(1.0f, t));

    float one_minus_t = 1.0f - t;

    // Calculate the blending factors for each control point
    float b0 = one_minus_t * one_minus_t * one_minus_t; // (1-t)^3
    float b1 = 3.0f * one_minus_t * one_minus_t * t;    // 3 * (1-t)^2 * t
    float b2 = 3.0f * one_minus_t * t * t;              // 3 * (1-t) * t^2
    float b3 = t * t * t;                               // t^3

    // Scale each control point by its blending factor and sum them up
    vec3 term0 = vec3_scale(p0, b0);
    vec3 term1 = vec3_scale(p1, b1);
    vec3 term2 = vec3_scale(p2, b2);
    vec3 term3 = vec3_scale(p3, b3);

    // Sum the terms to get the final point on the curve
    vec3 result = vec3_add(term0, vec3_add(term1, vec3_add(term2, term3)));

    return result;
}
