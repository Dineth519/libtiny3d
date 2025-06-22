#include <stdio.h>
#include "math3d.h"

// This demo applies transform to a cube and prints the projected vertices
int main() {
    vec3 cube_vertices[8] = {
        {-1,-1,-1}, {1,-1,-1}, {1,1,-1}, {-1,1,-1},
        {-1,-1,1},  {1,-1,1},  {1,1,1},  {-1,1,1},
    };

    mat4 model = mat4_translate(0, 0, -5);
    mat4 view = mat4_identity();
    mat4 proj = mat4_perspective(-1, 1, -1, 1, 1, 10);

    mat4 mvp = mat4_mul(proj, mat4_mul(view, model));

    for (int i = 0; i < 8; ++i) {
        vec3 v = cube_vertices[i];
        float x = v.x, y = v.y, z = v.z;
        float px = mvp.m[0]*x + mvp.m[4]*y + mvp.m[8]*z + mvp.m[12];
        float py = mvp.m[1]*x + mvp.m[5]*y + mvp.m[9]*z + mvp.m[13];
        float pz = mvp.m[2]*x + mvp.m[6]*y + mvp.m[10]*z + mvp.m[14];
        float pw = mvp.m[3]*x + mvp.m[7]*y + mvp.m[11]*z + mvp.m[15];

        px /= pw;
        py /= pw;
        pz /= pw;

        printf("Projected Vertex %d: (%f, %f, %f)\n", i, px, py, pz);
    }
    return 0;
}
