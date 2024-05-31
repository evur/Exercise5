#pragma once

#include <stdint.h>

// Clears the screen to a single color
void clear(float red, float green, float blue);

// Draws 32-bit RGBA pixels (for examples the ones loaded via kinc_image_init_from_file())
void draw_image(uint8_t* image, int image_width, int image_height, int x, int y);

// Draws a beautiful, red triangle
void draw_triangle(float x1, float y1, float z1, float u1, float v1, float x2, float y2, float z2, float u2, float v2, float x3, float y3, float z3, float u3, float v3);

// Reads a pixel at position (x,y) and returns RGB values
void read_pixel(uint8_t* image, int image_width, int image_height, int x, int y, float* red, float* green, float* blue);
