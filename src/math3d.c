#include <math.h>
#include <string.h> 
#include "tiny3d.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846f // Use 'f' suffix for float literal
#endif

// =======================
// Helper Functions
// =======================

// Update spherical coordinates from Cartesian coordinates
static void update_spherical_from_cartesian(vec3 *v) {
    float x = v->x, y = v->y, z = v->z;
    v->r = sqrtf(x * x + y * y + z * z);
    if (v->r != 0.0f) {
        v->theta = acosf(z / v->r);
        v->phi = atan2f(y, x);
    } else {
        v->theta = 0;
        v->phi = 0;
    }
    v->spherical_valid = true;
}

// Update Cartesian coordinates from spherical coordinates
static void update_cartesian_from_spherical(vec3 *v) {
    float r = v->r, theta = v->theta, phi = v->phi;
    v->x = r * sinf(theta) * cosf(phi);
    v->y = r * sinf(theta) * sinf(phi);
    v->z = r * cosf(theta);
    v->cartesian_valid = true;
}


// =======================
// Vector Constructors
// =======================

// Create a vec3 from Cartesian coordinates
vec3 vec3_from_cartesian(float x, float y, float z) {
    vec3 v = {x, y, z, 0, 0, 0, true, false};
    vec3_update_spherical(&v);
    return v;
}

// Create a vec3 from spherical coordinates
vec3 vec3_from_spherical(float r, float theta, float phi) {
    vec3 v = {0, 0, 0, r, theta, phi, false, true};
    vec3_update_cartesian(&v);
    return v;
}

// Create a vec4 from a vec3 with a specified w component
vec4 vec4_from_vec3(vec3 v, float w) {
    vec3_update_cartesian(&v);      // Ensure Cartesian is valid
    return (vec4){v.x, v.y, v.z, w};
}


// =======================
// Vector Setters
// =======================

// Set Cartesian coordinates for a vec3
void vec3_set_cartesian(vec3 *v, float x, float y, float z) {
    v->x = x;
    v->y = y;
    v->z = z;
    v->cartesian_valid = true;
    v->spherical_valid = false;
}

// Set spherical coordinates for a vec3
void vec3_set_spherical(vec3 *v, float r, float theta, float phi) {
    v->r = r;
    v->theta = theta;
    v->phi = phi;
    v->spherical_valid = true;
    v->cartesian_valid = false;
}


// =======================
// Coordinate Sync
// =======================

// Update spherical coordinates if they are not valid
void vec3_update_spherical(vec3 *v) {
    if (!v->spherical_valid && v->cartesian_valid) {
        update_spherical_from_cartesian(v);
    }
}

// Update Cartesian coordinates if they are not valid
void vec3_update_cartesian(vec3 *v) {
    if (!v->cartesian_valid && v->spherical_valid) {
        update_cartesian_from_spherical(v);
    }
}


// =======================
// Vector Operations
// =======================

// Add two vec3 vectors
vec3 vec3_add(vec3 a, vec3 b) {
    vec3_update_cartesian(&a);
    vec3_update_cartesian(&b);
    return vec3_from_cartesian(a.x + b.x, a.y + b.y, a.z + b.z);
}

// Subtract two vec3 vectors
vec3 vec3_sub(vec3 a, vec3 b) {
    vec3_update_cartesian(&a);
    vec3_update_cartesian(&b);
    return vec3_from_cartesian(a.x - b.x, a.y - b.y, a.z - b.z);
}

// Calculate the dot product of two vec3 vectors
float vec3_dot(vec3 a, vec3 b) {
    vec3_update_cartesian(&a);
    vec3_update_cartesian(&b);
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Calculate the cross product of two vec3 vectors
vec3 vec3_cross(vec3 a, vec3 b) {
    vec3_update_cartesian(&a);
    vec3_update_cartesian(&b);
    return vec3_from_cartesian(      // Cross product formula
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

// Calculate the length of a vec3 vector
float vec3_length(vec3 v) {
    vec3_update_cartesian(&v);
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

// Normalize a vec3 vector using a fast method
vec3 vec3_normalize_fast(vec3 v) {
    vec3_update_cartesian(&v);
    float len = vec3_length(v);
    if (len > 1e-6f) {      // Avoid division by zero
        return vec3_scale(v, 1.0f / len);
    }
    return v; // Return original if length is zero or very small
}

// Normalize a vec3 vector using the fast method
vec3 vec3_normalize(vec3 v) {
    return vec3_normalize_fast(v); // Using the existing fast implementation
}

// Scale a vec3 vector by a scalar
vec3 vec3_scale(vec3 v, float s) {
    vec3_update_cartesian(&v);
    return vec3_from_cartesian(v.x * s, v.y * s, v.z * s);
}

// Spherical linear interpolation (SLERP) between two vec3 vectors
vec3 vec3_slerp(vec3 a, vec3 b, float t) {
    vec3_update_cartesian(&a);
    vec3_update_cartesian(&b);
    float dot = vec3_dot(a, b);
    dot = fmaxf(fminf(dot, 1.0f), -1.0f); // Clamp dot product to valid range for acosf
    float theta = acosf(dot) * t;

    vec3 relative = vec3_sub(b, vec3_scale(a, dot));
    relative = vec3_normalize_fast(relative);
    
    vec3 result_cartesian = vec3_add(vec3_scale(a, cosf(theta)), vec3_scale(relative, sinf(theta)));
    return vec3_from_cartesian(result_cartesian.x, result_cartesian.y, result_cartesian.z);
}

// Extract a vec3 from a vec4 (ignoring w component)
vec3 vec3_from_vec4(vec4 v) {
    return vec3_from_cartesian(v.x, v.y, v.z);
}


// =======================
// Matrix Operations
// =======================

// Create a 4x4 identity matrix
mat4 mat4_identity(void) {
    mat4 m;
    memset(m.m, 0, sizeof(m.m));
    m.m[0] = 1.0f;
    m.m[5] = 1.0f;
    m.m[10] = 1.0f;
    m.m[15] = 1.0f;
    return m;
}

// Create a translation matrix
mat4 mat4_translate(float tx, float ty, float tz) {
    mat4 m = mat4_identity();
    m.m[12] = tx;
    m.m[13] = ty;
    m.m[14] = tz;
    return m;
}

// Create a scaling matrix
mat4 mat4_scale(float sx, float sy, float sz) {
    mat4 m = mat4_identity();
    m.m[0] = sx;
    m.m[5] = sy;
    m.m[10] = sz;
    return m;
}

// Create a rotation matrix around the X axis
mat4 mat4_rotate_x(float angle) {
    float c = cosf(angle);
    float s = sinf(angle);
    mat4 m = mat4_identity();
    m.m[5] = c;
    m.m[6] = s;
    m.m[9] = -s;
    m.m[10] = c;
    return m;
}

// Create a rotation matrix around the Y axis
mat4 mat4_rotate_y(float angle) {
    float c = cosf(angle);
    float s = sinf(angle);
    mat4 m = mat4_identity();
    m.m[0] = c;
    m.m[2] = -s;
    m.m[8] = s;
    m.m[10] = c;
    return m;
}

// Create a rotation matrix around the Z axis
mat4 mat4_rotate_z(float angle) {
    float c = cosf(angle);
    float s = sinf(angle);
    mat4 m = mat4_identity();
    m.m[0] = c;
    m.m[1] = s;
    m.m[4] = -s;
    m.m[5] = c;
    return m;
}

// Create a rotation matrix combining rotations around X, Y, and Z axes
mat4 mat4_rotate_xyz(float rx, float ry, float rz) {
    // Combine in Z -> Y -> X order (common for model rotations)
    mat4 rot_x = mat4_rotate_x(rx);
    mat4 rot_y = mat4_rotate_y(ry);
    mat4 rot_z = mat4_rotate_z(rz);
    return mat4_mul(mat4_mul(rot_z, rot_y), rot_x);
}

// Create a perspective projection matrix
mat4 mat4_perspective(float l, float r, float b, float t, float n, float f) {
    mat4 m = {{0}};
    m.m[0] = (2.0f * n) / (r - l);
    m.m[5] = (2.0f * n) / (t - b);
    m.m[8] = (r + l) / (r - l);
    m.m[9] = (t + b) / (t - b);
    m.m[10] = -(f + n) / (f - n);
    m.m[11] = -1.0f; // This is crucial for perspective projection
    m.m[14] = -(2.0f * f * n) / (f - n);
    return m;
}

// Multiply two 4x4 matrices
mat4 mat4_mul(mat4 a, mat4 b) {
    mat4 result;
    for (int c = 0; c < 4; ++c) { // Column of result
        for (int r = 0; r < 4; ++r) { // Row of result
            result.m[c * 4 + r] =
                a.m[0 * 4 + r] * b.m[c * 4 + 0] +
                a.m[1 * 4 + r] * b.m[c * 4 + 1] +
                a.m[2 * 4 + r] * b.m[c * 4 + 2] +
                a.m[3 * 4 + r] * b.m[c * 4 + 3];
        }
    }
    return result;
}

// mat4_mul_vec3 is now mat4_mul_vec4 for homogeneous coordinates
vec4 mat4_mul_vec4(mat4 m, vec4 v) {
    vec4 result;
    result.x = m.m[0] * v.x + m.m[4] * v.y + m.m[8] * v.z + m.m[12] * v.w;
    result.y = m.m[1] * v.x + m.m[5] * v.y + m.m[9] * v.z + m.m[13] * v.w;
    result.z = m.m[2] * v.x + m.m[6] * v.y + m.m[10] * v.z + m.m[14] * v.w;
    result.w = m.m[3] * v.x + m.m[7] * v.y + m.m[11] * v.z + m.m[15] * v.w;
    return result;
}