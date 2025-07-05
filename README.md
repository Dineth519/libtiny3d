# libtiny3d

A lightweight 3D software renderer written entirely in C, with **no dependencies on OpenGL, DirectX, Vulkan, or any external graphics library**. Instead, `libtiny3d` relies on math and direct pixel manipulation to output grayscale images in the PGM format. This project is ideal for learning and experimenting with the basics of 3D graphics from the ground up.

## Authors

- [Dineth519](https://github.com/Dineth519)
- [senath16](https://github.com/senath16)

## Features

- **Simple Canvas & Line Drawing Routines**
  - Draws directly to an in-memory grayscale pixel buffer.
  - Includes anti-aliased line drawing functions.
- **3D Transformations**
  - Supports 3D rotation, translation, and perspective projection using 4x4 matrices.
- **Render Static Shapes**
  - Example: Generates a clock face by drawing lines at regular angles from the center.
- **Animated Wireframe Rendering**
  - Renders an animated icosahedron (20-sided figure).
  - Renders an animated soccer ball (truncated icosahedron), rotating in 3D space.
- **Frame Output as PGM Images**
  - All renders are saved as standard PGM grayscale image files.

## Demo Output

When you run the provided demo, the following files are generated:
- `clockface.pgm` — a static image of a clock face.
- `frame_000.pgm` to `frame_119.pgm` — 120 frames of an animated, rotating soccer ball wireframe.

## Build Instructions

### Prerequisites
- GCC (or compatible C compiler)
- GNU Make

### Building

Clone the repository:
```sh
git clone https://github.com/Dineth519/libtiny3d.git
cd libtiny3d
```

Build the demo:
```sh
make
```
This compiles everything into the `demo_combined` executable.

### Running

Run the demo:
```sh
./demo_combined
```
This will generate the output images (`clockface.pgm` and `frame_###.pgm`) in the current directory.

> **Note:** You can view `.pgm` images with most image viewers, or convert them to PNG using tools like ImageMagick:
> ```sh
> convert frame_000.pgm frame_000.png
> ```

## Project Structure

- `/src` — core library source (canvas, math, renderer)
- `/demo` — demo programs (`main.c`, etc.)
- `/include` — public header files

## Future Goals

- **Polygon filling** (solid and wireframe rendering)
- **Basic shading** (flat, Gouraud, Phong)
- **Export to PNG or GIF** (using external tools or lightweight libraries)
- **Interactive camera controls** (orbit, zoom, pan)
- **Mesh loading from external files**

## Credits

- Boilerplate math and file I/O code generated with the assistance of **GitHub Copilot**

---

**Explore, learn, and contribute — libtiny3d is a fun playground for low-level 3D rendering!**