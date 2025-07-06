#include <stdio.h>
#include <math.h>
#include "tiny3d.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int main() {
    printf("=== Testing math3d ===\n");

    // ===========================================
    // Test 1: Spherical to Cartesian conversion
    // ===========================================
    vec3 v = vec3_from_spherical(1.0f, M_PI/4, M_PI/4);
    printf("vec3_from_spherical(1, 45°,45°): (%.3f, %.3f, %.3f)\n", v.x, v.y, v.z);

    // ===========================================
    // Test 2: Normalization
    // ===========================================
    vec3 v2 = vec3_from_cartesian(3, 4, 0);
    vec3 n = vec3_normalize(v2);
    printf("normalize(3,4,0): (%.3f, %.3f, %.3f) len=%.3f\n",
           n.x, n.y, n.z, vec3_length(n));

    // ===========================================
    // Test 3: Dot product
    // ===========================================
    float d = vec3_dot(v2, vec3_from_cartesian(1,0,0));
    printf("dot((3,4,0),(1,0,0)) = %.3f\n", d);

    // ===========================================
    // Test 4: Cross product
    // ===========================================
    vec3 c = vec3_cross(vec3_from_cartesian(1,0,0), vec3_from_cartesian(0,1,0));
    printf("cross((1,0,0),(0,1,0)) = (%.3f, %.3f, %.3f)\n", c.x, c.y, c.z);

    // ===========================================
    // Test 5: Matrix translate * vector
    // ===========================================
    mat4 t = mat4_translate(1,2,3);
    vec4 p = vec4_from_vec3(vec3_from_cartesian(1,1,1), 1);
    vec4 pt = mat4_mul_vec4(t, p);
    printf("translate(1,2,3)*(1,1,1,1) = (%.3f, %.3f, %.3f, %.3f)\n",
           pt.x, pt.y, pt.z, pt.w);

    // ===========================================
    // Test 6: Rotation matrix
    // ===========================================
    mat4 rx = mat4_rotate_x(M_PI/2);
    vec4 prx = mat4_mul_vec4(rx, p);
    printf("rotate_x(90°)*(1,1,1,1) = (%.3f, %.3f, %.3f, %.3f)\n",
           prx.x, prx.y, prx.z, prx.w);

    // ===========================================
    // Test 7: Manual cube transform & projection
    // ===========================================
    printf("\n=== Testing cube transform ===\n");

    // -------------------------------------------
    // 8 vertices of a unit cube centered at origin
    // -------------------------------------------
    vec3 cube[8] = {
        vec3_from_cartesian(-1,-1,-1),
        vec3_from_cartesian( 1,-1,-1),
        vec3_from_cartesian( 1, 1,-1),
        vec3_from_cartesian(-1, 1,-1),
        vec3_from_cartesian(-1,-1, 1),
        vec3_from_cartesian( 1,-1, 1),
        vec3_from_cartesian( 1, 1, 1),
        vec3_from_cartesian(-1, 1, 1)
    };

    // Build Model-View-Projection matrix
    mat4 model = mat4_rotate_xyz(0.5, 0.5, 0);
    mat4 view = mat4_translate(0,0,-5);
    mat4 proj = mat4_perspective(-1,1,-1,1,1,10);

    // Full MVP
    mat4 mvp = mat4_mul(proj, mat4_mul(view, model));

    // -------------------------------------------
    // Project each cube vertex and print
    // -------------------------------------------
    for (int i = 0; i < 8; i++) {
        vec4 p = vec4_from_vec3(cube[i], 1.0f);
        vec4 projected = mat4_mul_vec4(mvp, p);

        // Print raw homogeneous coordinates (x,y,z,w)
        printf("Projected vertex %d → (%.3f, %.3f, %.3f, %.3f)\n", i, projected.x, projected.y, projected.z, projected.w);
    }

    return 0;
}