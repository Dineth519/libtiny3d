#ifndef MATH3D_H
#define MATH3D_H
#include <stdbool.h>

// =======================
// 3D Vector Struct
// =======================

typedef struct {
    float x, y, z;                  // Cartesian coordinates
    float r, theta, phi;            // Spherical coordinates
    bool cartesian_valid, spherical_valid;
} vec3;

// =======================
// 4D Vector Struct
// =======================
typedef struct {
    float x, y, z, w;
} vec4;

// =======================
// 4x4 Matrix Struct
// =======================

typedef struct {
    // Stored in column-major order
    float m[16];
} mat4;

// =======================
// Vector Constructors
// =======================

vec3 vec3_from_cartesian(float x, float y, float z);
vec3 vec3_from_spherical(float r, float theta, float phi);
vec4 vec4_from_vec3(vec3 v, float w);

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
vec3 vec3_normalize(vec3 v);
vec3 vec3_slerp(vec3 a, vec3 b, float t);
vec3 vec3_scale(vec3 v, float s);
vec3 vec3_from_vec4(vec4 v);

// =======================
// Matrix Operations
// =======================

mat4 mat4_identity(void);
mat4 mat4_translate(float tx, float ty, float tz);
mat4 mat4_scale(float sx, float sy, float sz);
mat4 mat4_rotate_x(float angle);
mat4 mat4_rotate_y(float angle);
mat4 mat4_rotate_z(float angle);
mat4 mat4_rotate_xyz(float rx, float ry, float rz);
mat4 mat4_perspective(float l, float r, float b, float t, float n, float f);
vec4 mat4_mul_vec4(mat4 m, vec4 v);
mat4 mat4_mul(mat4 a, mat4 b);

#endif