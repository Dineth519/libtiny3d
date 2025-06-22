#ifndef MATH3D_H
#define MATH3D_H

typedef struct {
    float x, y, z;
    float r, theta, phi;
    
} vec3;

typedef struct {
    float m[16]; // column-major order
} mat4;

vec3 vec3_from_spherical(r, theta, phi);

vec3 vec3_normalize_fast(vec3 v);

vec3 vec3_slerp(vec3 a, vec3 b, float t);

mat4 mat4_identity(void);

mat4 mat4_translate(float tx, float ty, float tz);

mat4 mat4_scale(float sx, float sy, float sz);

mat4 mat4_rotate_xyz(float rx, float ry, float rz);

mat4 mat4_perspective(float l, float r, float b, float t, float n, float f);

mat4 mat4_mul(mat4 a, mat4 b);

#endif 