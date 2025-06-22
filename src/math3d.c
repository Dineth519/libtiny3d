#include <math.h>
#include <stdint.h>
#include "math3d.h"


// Convert cartesian coordinates into spherical coordinates
static void cartesian_to_spherical(vec3 *v) {
    float x = v->x, y = v->y, z = v->z;
    v->r = sqrtf(x*x + y*y + z*z);      // Calculate radius (lenght)
    if (v->r < 1e-6f) {               // Avoid division by zero (very small vector)
        v->theta = 0.0f;
        v->phi = 0.0f;
    } else {
        v->theta = acosf(z / v->r); 
        v->phi = atan2f(y, x);      
    }
    v->spherical_valid = true;             // Mark spherical data as valid
}


// Convert spherical coordinates into cartesian coordinates
static void spherical_to_cartesian(vec3 *v) {
    float r = v->r, theta = v->theta, phi = v->phi;
    v->x = r * sinf(theta) * cosf(phi);
    v->y = r * sinf(theta) * sinf(phi);
    v->z = r * cosf(theta);
    v->cartesian_valid = true;          // Mark cartesian data as valid
}


// Create a vector from spherical coordinates
vec3 vec3_from_spherical(float r, float theta, float phi) {
    vec3 v = {0, 0, 0, r, theta, phi, false, true};        // Initialize the structure
    spherical_to_cartesian(&v);         // Convert to cartesian
    return v;
}


// Set vector from cartesian coordinates
void vec3_set_cartesian(vec3 *v, float x, float y, float z) {
    v->x = x; v->y = y; v->z = z;
    v->cartesian_valid = true;
    cartesian_to_spherical(v);
}


// Set vector from spherical coordinates
void vec3_set_spherical(vec3 *v, float r, float theta, float phi) {
    v->r = r; v->theta = theta; v->phi = phi;
    v->spherical_valid = true;
    spherical_to_cartesian(v);
}


// If spherical is valid, then update it
void vec3_update_spherical(vec3 *v) {
    if (!v->spherical_valid)
        cartesian_to_spherical(v);
}


// If cartesian is valid, then update it
void vec3_update_cartesian(vec3 *v) {
    if (!v->cartesian_valid)
        spherical_to_cartesian(v);
}


// Fast inverse square root trick
static float Q_rsqrt(float number) {
    union { float f; uint32_t i; } conv = {number};
    conv.i = 0x5f3759df - (conv.i >> 1);      // Magic number trick
    float y = conv.f;
    y = y * (1.5f - (number * 0.5f * y * y));    // 1st iteration of Newton-Raphson
    return y;
}


// Fast normalize a vector using Q_rsqrt
vec3 vec3_normalize_fast(vec3 v) {
    float len_sq = v.x*v.x + v.y*v.y + v.z*v.z;        // Calculate "lenght^2"
    if (len_sq == 0) {     
        vec3 v1 = {0, 0, 0, 0, 0, 0, true, true};              // Return null vector
        return v1;
    } else {
        float invLen = Q_rsqrt(len_sq);            // Get inverse of lenght (1/lenght)
        vec3 v2 = {v.x*invLen, v.y*invLen, v.z*invLen, 0, 0, 0, true, false};
        cartesian_to_spherical(&v2);      // Update spherical
        return v2;
    }
}


// Spherical linear interpolation (slerp) between directions
vec3 vec3_slerp(vec3 a, vec3 b, float t) {

    // Normalize input vectors
    vec3 an = vec3_normalize_fast(a);
    vec3 bn = vec3_normalize_fast(b);

    float dot = a.x*b.x + a.y*b.y + a.z*b.z;    // Get dot product

    // If vectors are nearly aligned, use linear interpolation
    if (dot > 0.9995f) { 
        vec3 v3 = {a.x + (b.x - a.x)*t, a.y + (b.y - a.y)*t, a.z + (b.z - a.z)*t, 0,0,0,true,false};
        cartesian_to_spherical(&v3);
        return vec3_normalize_fast(v3);
    }

    // Clamp dot product
    if (dot < -1.0f) {
        dot = -1.0f;
    }

    // Clamp dot product
    if (dot > 1.0f) {
        dot = 1.0f;
    }

    float theta = acosf(dot);       // Angle between vectors
    float st = sinf(theta);

    // Compute interpolation weights
    float w1 = sinf((1.0f - t)*theta) / st;
    float w2 = sinf(t*theta) / st;

    // Interpolate
    vec3 v3 = {a.x*w1 + b.x*w2, a.y*w1 + b.y*w2, a.z*w1 + b.z*w2, 0,0,0,true,false};
    cartesian_to_spherical(&v3);
    return v3;
}


// Return identity matrix
mat4 mat4_identity(void) {
    mat4 m = { {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    }};
    return m;
}


// Create translation matrix
mat4 mat4_translate(float tx, float ty, float tz) {
    mat4 m = mat4_identity();         // Start with identity
    m.m[12] = tx;
    m.m[13] = ty;
    m.m[14] = tz;
    return m;
}


// Create scaling matrix
mat4 mat4_scale(float sx, float sy, float sz) {
    mat4 m = mat4_identity();
    m.m[0] = sx;
    m.m[5] = sy;
    m.m[10] = sz;
    return m;
}


// Create rotation matrix (Rx * Ry * Rz)
mat4 mat4_rotate_xyz(float rx, float ry, float rz) {
    float cx = cosf(rx), sx = sinf(rx);
    float cy = cosf(ry), sy = sinf(ry);
    float cz = cosf(rz), sz = sinf(rz);

    // Rotation about X
    mat4 mx = { {
        1,0,0,0,
        0,cx,sx,0,
        0,-sx,cx,0,
        0,0,0,1
    }};

    // Rotation about Y
    mat4 my = { {
        cy,0,-sy,0,
        0,1,0,0,
        sy,0,cy,0,
        0,0,0,1
    }};

    // Rotation about Z
    mat4 mz = { {
        cz,sz,0,0,
        -sz,cz,0,0,
        0,0,1,0,
        0,0,0,1
    }};
    
    // Combined rotation: Mz * My * Mx
    return mat4_mul(mat4_mul(mz, my), mx);
}


// Perspective projection matrix
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


// Multiply two 4x4 matrices
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


// Multiply two 4x4 matrices
vec3 mat4_mul_vec3(mat4 m, vec3 v) {
    float x = v.x, y = v.y, z = v.z, w = 1.0f;
    float rx = m.m[0]*x + m.m[4]*y + m.m[8]*z + m.m[12]*w;
    float ry = m.m[1]*x + m.m[5]*y + m.m[9]*z + m.m[13]*w;
    float rz = m.m[2]*x + m.m[6]*y + m.m[10]*z + m.m[14]*w;
    float rw = m.m[3]*x + m.m[7]*y + m.m[11]*z + m.m[15]*w;

    if (fabsf(rw) > 1e-6f) {
        rx /= rw; ry /= rw; rz /= rw;        // Perspective divide
    }
    vec3 v4 = {rx, ry, rz, 0,0,0,true,false};
    cartesian_to_spherical(&v4);         // Update spherical form
    return v4;
}