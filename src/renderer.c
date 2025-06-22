// renderer.c – implements 3D rendering pipeline
#include <math.h>
#include <stdbool.h>
#include "canvas.h"
#include "math3d.h"
#include "renderer.h"

// Projects a 3D vertex through model, view, projection to screen space
vec3_t project_vertex(vec3_t v, mat4_t model, mat4_t view, mat4_t proj, int width, int height) {
    vec4_t pos = vec4(v.x, v.y, v.z, 1.0f);
    vec4_t world = mat4_mul_vec4(model, pos);
    vec4_t viewspace = mat4_mul_vec4(view, world);
    vec4_t clip = mat4_mul_vec4(proj, viewspace);

    if (clip.w == 0.0f) clip.w = 0.0001f; // avoid div by zero

    float x_ndc = clip.x / clip.w;
    float y_ndc = clip.y / clip.w;
    float z_ndc = clip.z / clip.w;

    vec3_t screen;
    screen.x = (x_ndc + 1.0f) * width / 2.0f;
    screen.y = (1.0f - y_ndc) * height / 2.0f; // Flip Y
    screen.z = z_ndc;
    return screen;
}

// Clipping function: returns true if inside circular viewport
bool clip_to_circular_viewport(canvas_t *c, float x, float y) {
    float cx = c->width / 2.0f;
    float cy = c->height / 2.0f;
    float r = fminf(cx, cy);
    float dx = x - cx, dy = y - cy;
    return (dx * dx + dy * dy <= r * r);
}

// Renders a 3D object's wireframe onto the canvas
void render_wireframe(canvas_t *c, object3d_t *obj, mat4_t model, mat4_t view, mat4_t proj) {
    vec3_t projected[obj->num_vertices];
    for (int i = 0; i < obj->num_vertices; i++) {
        projected[i] = project_vertex(obj->vertices[i], model, view, proj, c->width, c->height);
    }

    for (int i = 0; i < obj->num_edges; i++) {
        int a = obj->edges[i].a;
        int b = obj->edges[i].b;
        vec3_t va = projected[a];
        vec3_t vb = projected[b];

        if (clip_to_circular_viewport(c, va.x, va.y) || clip_to_circular_viewport(c, vb.x, vb.y)) {
            draw_line_f(c, va.x, va.y, vb.x, vb.y, 1.0f);
        }
    }
}

// main.c – test and demo
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "canvas.h"
#include "math3d.h"
#include "renderer.h"

#define WIDTH 800
#define HEIGHT 800

object3d_t generate_soccer_ball();

int main() {
    canvas_t canvas = canvas_create(WIDTH, HEIGHT);
    object3d_t ball = generate_soccer_ball();

    float angle = 0;
    while (true) {
        canvas_clear(&canvas);

        mat4_t model = mat4_rotate_xyz(angle, angle, 0);
        mat4_t view = mat4_translate(0, 0, -3);
        mat4_t proj = mat4_frustum(-1, 1, -1, 1, 1, 10);

        render_wireframe(&canvas, &ball, model, view, proj);

        canvas_present(&canvas);
        angle += 0.02f;

        struct timespec t = {0, 16000000}; // ~60 FPS
        nanosleep(&t, NULL);
    }

    canvas_destroy(&canvas);
    return 0;
}

// Generate a basic soccer ball (truncated icosahedron approximation)
object3d_t generate_soccer_ball() {
    object3d_t obj;
    obj.num_vertices = 60;
    obj.num_edges = 90;

    for (int i = 0; i < 60; i++) {
        float theta = (i % 20) * (2 * M_PI / 20);
        float phi = ((i / 20) * M_PI / 3.0);
        obj.vertices[i] = vec3_from_spherical(1.0f, theta, phi);
    }
    int k = 0;
    for (int i = 0; i < 60 && k < 90; i++) {
        for (int j = i+1; j < 60 && k < 90; j++) {
            float dist = vec3_distance(obj.vertices[i], obj.vertices[j]);
            if (dist < 1.1f) {
                obj.edges[k++] = (edge_t){i, j};
            }
        }
    }
    return obj;
}
