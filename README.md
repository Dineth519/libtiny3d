# libtiny3d
*A minimal 3D software renderer in pure C (no OpenGL/DirectX)*

## Overview

**libtiny3d** is a tiny, self-contained 3D software renderer written from scratch in C. It provides a basic environment to render 3D geometric objects onto a 2D canvas, without relying on hardware acceleration or external graphics libraries. All rendering, transformations, and lighting are handled in software using simple algorithms and data structures.

## Features

- **Canvas Drawing:** Draw RGB pixels and lines with sub-pixel precision and optional thickness.
- **3D Transformations:** Translate, rotate, and scale using 4×4 matrices (homogeneous coordinates).
- **Projection Pipeline:** Complete model → view → projection → screen mapping.
- **Geometric Objects:** Built-in support for cubes and truncated icosahedrons (soccer balls).
- **Wireframe Rendering:** Uses Bresenham or DDA algorithms for fast line drawing.
- **Lambertian Lighting:** Optional per-edge intensity using dot product (Lambertian reflectance).
- **Animation Loop:** Animate and export frame sequences as PGM images.
- **Modular Structure:** Clean separation of canvas, math, rendering, lighting, and animation code.

## Build & Run

### Prerequisites

- Linux environment (tested on Ubuntu)
- GCC compiler
- `make` (optional, recommended)

### Quick Build (with Make)

Use the provided `Makefile`:

```sh
make
```

This compiles object files, creates `libtiny3d.a`, and builds demo executables inside `build/`.

### Manual Build (example)

```sh
gcc -Iinclude -Wall -o build/demo/soccer_ball \
    src/canvas.c src/math3d.c src/renderer.c \
    src/lighting.c src/animation.c \
    demo/main1.c -lm

gcc -Iinclude -Wall -o build/demo/clock_face \
    src/canvas.c src/math3d.c 
    demo/main.c -lm
```

## Running Visual Tests

After building, run the test executables to render animations as a series of PGM images.

### Running clock face

```sh
./build/demo/clock_face
```

### Render a Rotating Soccer Ball

```sh
./build/demo/soccer_ball
```

## Directory Structure

```
libtiny3d/
├── src/
│   ├── canvas.c, math3d.c, renderer.c, lighting.c, animation.c
├── include/
│   ├── tiny3d.h, canvas.h, math3d.h, renderer.h, lighting.h, animation.h
├── tests/
│   ├── test_math.c, test_pipeline.c, cube_visualize.c
│   └── visual_tests/ (output PGM images & GIFs)
├── demo/
│   ├── main.c, main1.c
├── build/
│   ├── demo/, libtiny3d.a, clock_face, soccer_ball
├── documentation/
│   └── Group65_report.pdf
├── Makefile
└── README.md

```

## Authors

- [Dineth519](https://github.com/Dineth519)
- [senath16](https://github.com/senath16)