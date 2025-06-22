#ifndef MATH3D_H
#define MATH3D_H
#include <stdbool.h>


// =======================
// 3D Vector Struct
// =======================

typedef struct {
    float x, y, z;          // Cartesian coordinates
    float r, theta, phi;      // Spherical coordinates
    bool cartesian_valid, spherical_valid;    // Check cartesian and spherical data valid?
} vec3;


// =======================
// 4x4 Matrix Struct
// =======================

typedef struct {
    float m[16]; // column-major order
} mat4;


// =======================
// Vector Constructors
// =======================

vec3 vec3_from_cartesian(float x, float y, float z);
vec3 vec3_from_spherical(float r, float theta, float phi);


// =======================
// Vector Setters
// =======================

void vec3_set_cartesian(vec3 *v, float x, float y, float z);
void vec3_set_spherical(vec3 *v, float r, float theta, float phi);


// =======================
// Coordinate Sync
// =======================

void vec3_update_spherical(vec3 *v);
void vec3_update_cartesian(vec3 *v);


// =======================
// Vector Operations
// =======================

vec3 vec3_add(vec3 a, vec3 b);
vec3 vec3_sub(vec3 a, vec3 b);
float vec3_dot(vec3 a, vec3 b);
vec3 vec3_cross(vec3 a, vec3 b);
float vec3_length(vec3 v);
vec3 vec3_normalize_fast(vec3 v);
vec3 vec3_slerp(vec3 a, vec3 b, float t);


// =======================
// Matrix Operations
// =======================

mat4 mat4_identity(void);
mat4 mat4_translate(float tx, float ty, float tz);
mat4 mat4_scale(float sx, float sy, float sz);
mat4 mat4_rotate_xyz(float rx, float ry, float rz);
mat4 mat4_perspective(float l, float r, float b, float t, float n, float f);
vec3 mat4_mul_vec3(mat4 m, vec3 v); // assumes w=1, returns projected (divided by w)
mat4 mat4_mul(mat4 a, mat4 b);

#endif 