#ifndef MATH3D_H
#define MATH3D_H

#include <math.h>

#define DEG_TO_RAD (M_PI / 180.0)
#define RAD_TO_DEG (180.0 / M_PI)

typedef struct {
    // Cartesian
    double x, y, z;

    // Spherical
    double r;     // radius
    double theta; // polar angle (from +Z axis) [0, pi]
    double phi;   // azimuthal angle (from +X axis) [0, 2pi]
} vec3_t;

// --- Internal Helpers ---
static void cartesian_to_spherical(vec3_t* v) {
    v->r = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
    if (v->r == 0) {
        v->theta = 0;
        v->phi = 0;
    } else {
        v->theta = acos(v->z / v->r);                  // [0, pi]
        v->phi = atan2(v->y, v->x);                    // [-pi, pi]
        if (v->phi < 0) v->phi += 2 * M_PI;            // [0, 2pi]
    }
}

static void spherical_to_cartesian(vec3_t* v) {
    v->x = v->r * sin(v->theta) * cos(v->phi);
    v->y = v->r * sin(v->theta) * sin(v->phi);
    v->z = v->r * cos(v->theta);
}

// --- Public API ---
static inline void vec3_set_cartesian(vec3_t* v, double x, double y, double z) {
    v->x = x;
    v->y = y;
    v->z = z;
    cartesian_to_spherical(v);
}

static inline void vec3_set_spherical(vec3_t* v, double r, double theta, double phi) {
    v->r = r;
    v->theta = theta;
    v->phi = phi;
    spherical_to_cartesian(v);
}

#endif // MATH3D_H
