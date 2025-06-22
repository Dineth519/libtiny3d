#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include "math3d.h"


vec3 vec3_from_spherical(float r, float theta, float phi) {

    // Initialize the structure
    vec3 v = {0, 0, 0, r, theta, phi};

    float r = v->r, theta = v->theta, phi = v->phi;

    // Converts spherical coordinates into x, y, z 
    v->x = r * sinf(theta) * cosf(phi);
    v->y = r * sinf(theta) * sinf(phi);
    v->z = r * cosf(theta);

    return v;
}


static float Q_rsqrt(float number) {
    union { float f; uint32_t i; } conv = {number};
    conv.i = 0x5f3759df - (conv.i >> 1);
    float y = conv.f;
    y = y * (1.5f - (number * 0.5f * y * y));
    return y;
}


vec3 vec3_normalize_fast(vec3 v) {
    float len_sq = v.x*v.x + v.y*v.y + v.z*v.z;        // Calculate "lenght^2"
    if (len_sq == 0) {     
        vec3 v1 = {0, 0, 0, 0, 0, 0};                     // 
        return v1;
    } else {
        float invLen = Q_rsqrt(len_sq);            // Get inverse of lenght
        vec3 v2 = {v.x*invLen, v.y*invLen, v.z*invLen, 0, 0, 0};
        return v2;
    }
}


// Spherical linear interpolation (slerp) between directions
vec3 vec3_slerp(vec3 a, vec3 b, float t) {
    vec3 an = vec3_normalize_fast(a);
    vec3 bn = vec3_normalize_fast(b);
    float dot = vec3_dot(an, bn);
    if (dot > 0.9995f) { // If very close, lerp
       
        vec3 v4 = {an.x + (bn.x - an.x)*t, an.y + (bn.y - an.y)*t, an.z + (bn.z - an.z)*t, 0, 0, 0};
        return vec3_normalize_fast(v4);

    dot = fmaxf(fminf(dot, 1.0f), -1.0f);
    float theta = acosf(dot) * t;

    vec3 proj = {an.x*dot, an.y*dot, an.z*dot, 0, 0, 0};
    vec3 rel = vec3_normalize_fast(bn.x - proj.x, bn.y - proj.y, bn.z - proj.z, 0, 0, 0) 
    // an*cos(theta) + rel*sin(theta)
    float cost = cosf(theta), sint = sinf(theta);
    
    vec3 result = {an.x*cost + rel.x*sint,
        an.y*cost + rel.y*sint,
        an.z*cost + rel.z*sint, 0, 0, 0};
    return vec3_normalize_fast(result);
}
}


mat4 mat4_identity(void) {
    mat4 m = { {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    }};
    return m;
}

mat4 mat4_translate(float tx, float ty, float tz) {
    mat4 m = mat4_identity();
    m.m[12] = tx;
    m.m[13] = ty;
    m.m[14] = tz;
    return m;
}
mat4 mat4_scale(float sx, float sy, float sz) {
    mat4 m = mat4_identity();
    m.m[0] = sx;
    m.m[5] = sy;
    m.m[10] = sz;
    return m;
}
mat4 mat4_rotate_xyz(float rx, float ry, float rz) {
    float cx = cosf(rx), sx = sinf(rx);
    float cy = cosf(ry), sy = sinf(ry);
    float cz = cosf(rz), sz = sinf(rz);
    mat4 mx = { {
        1,0,0,0,
        0,cx,sx,0,
        0,-sx,cx,0,
        0,0,0,1
    }};
    mat4 my = { {
        cy,0,-sy,0,
        0,1,0,0,
        sy,0,cy,0,
        0,0,0,1
    }};
    mat4 mz = { {
        cz,sz,0,0,
        -sz,cz,0,0,
        0,0,1,0,
        0,0,0,1
    }};
    // R = Mz * My * Mx
    return mat4_mul(mat4_mul(mz, my), mx);
}
mat4 mat4_perspective(float l, float r, float b, float t, float n, float f) {
    mat4 m = { {0} };
    m.m[0] = 2*n/(r-l);
    m.m[5] = 2*n/(t-b);
    m.m[8] = (r+l)/(r-l);
    m.m[9] = (t+b)/(t-b);
    m.m[10] = -(f+n)/(f-n);
    m.m[11] = -1;
    m.m[14] = -2*f*n/(f-n);
    return m;
}


mat4 mat4_mul(mat4 a, mat4 b) {
    mat4 r = { {0} };
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            float sum = 0.0f;
            for (int k = 0; k < 4; ++k)
                sum += a.m[k*4+row] * b.m[col*4+k];
            r.m[col*4+row] = sum;
        }
    }
    return r;
}