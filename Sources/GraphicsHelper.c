#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

#include <kinc/graphics1/graphics.h>
#include <kinc/graphics4/graphics.h>
#include <kinc/image.h>
#include <kinc/math/core.h>

#include "GraphicsHelper.h"
#include "Memory.h"

void shade_pixel(int x, int y, float z, float u, float v) {}

void clear(float red, float green, float blue) {
	for (int y = 0; y < kinc_g1_height(); ++y) {
		for (int x = 0; x < kinc_g1_width(); ++x) {
			kinc_g1_set_pixel(x, y, red, green, blue);
		}
	}
}

void read_pixel(uint8_t* image, int image_width, int image_height, int x, int y, float* red, float* green, float* blue) {
	if (x < 0 || x > image_width || y < 0 || y > image_height) {
		return;
	}
	int col = *(int*)&(image)[image_width * 4 * y + x * 4];
	*blue = ((col & 0xff0000) >> 16) / 255.0f;
	*green = ((col & 0xff00) >> 8) / 255.0f;
	*red = (col & 0xff) / 255.0f;
}

void draw_image(uint8_t* image, int image_width, int image_height, int x, int y) {
	int ystart = max(0, -y);
	int xstart = max(0, -x);
	int h = min(image_height, kinc_g1_height() - y);
	int w = min(image_width, kinc_g1_width() - x);
	for (int yy = ystart; yy < h; ++yy) {
		for (int xx = xstart; xx < w; ++xx) {
			float r, g, b;
			read_pixel(image, image_width, image_height, xx, yy, &r, &g, &b);
			kinc_g1_set_pixel(x + xx, y + yy, r, g, b);
		}
	}
}

struct Edge {
	int x1, y1, x2, y2;
	float z1, z2;
	float u1, v1, u2, v2;
};

struct Edge new_edge(int x1, int y1, float z1, float u1, float v1, int x2, int y2, float z2, float u2, float v2) {
	struct Edge edge;
	if (y1 < y2) {
		edge.x1 = x1;
		edge.y1 = y1;
		edge.z1 = z1;
		edge.u1 = u1;
		edge.v1 = v1;
		edge.x2 = x2;
		edge.y2 = y2;
		edge.z2 = z2;
		edge.u2 = u2;
		edge.v2 = v2;
	} else {
		edge.x1 = x2;
		edge.y1 = y2;
		edge.z1 = z2;
		edge.u1 = u2;
		edge.v1 = v2;
		edge.x2 = x1;
		edge.y2 = y1;
		edge.z2 = z1;
		edge.u2 = u1;
		edge.v2 = v1;
	}
	return edge;
}

struct Span {
	int x1, x2;
	float z1, z2;
	float u1, u2;
	float v1, v2;
};

struct Span new_span(int x1, int x2, float z1, float z2, float u1, float u2, float v1, float v2) {
	struct Span span;
	if (x1 < x2) {
		span.x1 = x1;
		span.x2 = x2;
		span.z1 = z1;
		span.z2 = z2;
		span.u1 = u1;
		span.v1 = v1;
		span.u2 = u2;
		span.v2 = v2;
	} else {
		span.x1 = x2;
		span.x2 = x1;
		span.z1 = z2;
		span.z2 = z1;
		span.u1 = u2;
		span.v1 = v2;
		span.u2 = u1;
		span.v2 = v1;
	}
	return span;
}

void draw_span(const struct Span span, int y) {
	int xdiff = span.x2 - span.x1;
	if (xdiff == 0) return;
	
	float zdiff = span.z2 - span.z1;
	float udiff = span.u2 - span.u1;
	float vdiff = span.v2 - span.v1;
	
	float factor = 0.0f;
	float factor_step = 1.0f / xdiff;
	
	int x_min = max(0, span.x1);
	int x_max = min(span.x2, kinc_g1_width());
	
	factor += factor_step * -min(0, span.x1);
	
	for (int x = x_min; x < x_max; ++x) {
		float z = span.z1 + zdiff * factor;
		float u = span.u1 + udiff * factor;
		float v = span.v1 + vdiff * factor;
		shade_pixel(x, y, z, u, 1 - v);
		factor += factor_step;
	}
}

void draw_spans_between_edges(const struct Edge e1, const struct Edge e2) {
	float e1ydiff = (float)(e1.y2 - e1.y1);
	if (e1ydiff == 0.0f) return;
	
	float e2ydiff = (float)(e2.y2 - e2.y1);
	if (e2ydiff == 0.0f) return;
	
	float e1xdiff = (float)(e1.x2 - e1.x1);
	float e2xdiff = (float)(e2.x2 - e2.x1);
	float z1diff = e1.z2 - e1.z1;
	float z2diff = e2.z2 - e2.z1;
	float e1udiff = e1.u2 - e1.u1;
	float e1vdiff = e1.v2 - e1.v1;
	float e2udiff = e2.u2 - e2.u1;
	float e2vdiff = e2.v2 - e2.v1;
	
	float factor1 = (float)(e2.y1 - e1.y1) / e1ydiff;
	float factor_step_1 = 1.0f / e1ydiff;
	float factor2 = 0.0f;
	float factor_step_2 = 1.0f / e2ydiff;
	
	int y_min = max(0, e2.y1);
	int y_max = min(e2.y2, kinc_g1_height());
	
	factor1 += factor_step_1 * -min(0, e2.y1);
	factor2 += factor_step_2 * -min(0, e2.y1);
	
	for (int y = y_min; y < y_max; ++y) {
		struct Span span = new_span(
			e1.x1 + (int)(e1xdiff * factor1), e2.x1 + (int)(e2xdiff * factor2),
			e1.z1 + z1diff * factor1, e2.z1 + z2diff * factor2,
			e1.u1 + e1udiff * factor1, e2.u1 + e2udiff * factor2,
			e1.v1 + e1vdiff * factor1, e2.v1 + e2vdiff * factor2);
		draw_span(span, y);
		factor1 += factor_step_1;
		factor2 += factor_step_2;
	}
}

void draw_triangle(float x1, float y1, float z1, float u1, float v1, float x2, float y2, float z2, float u2, float v2, float x3, float y3, float z3, float u3, float v3) {
	struct Edge edges[3] = {
		new_edge((int)round(x1), (int)round(y1), z1, u1, v1, (int)round(x2), (int)round(y2), z2, u2, v2),
		new_edge((int)round(x2), (int)round(y2), z2, u2, v2, (int)round(x3), (int)round(y3), z3, u3, v3),
		new_edge((int)round(x3), (int)round(y3), z3, u3, v3, (int)round(x1), (int)round(y1), z1, u1, v1)
	};
	
	int max_length = 0;
	int long_edge = 0;
	
	for (int i = 0; i < 3; ++i) {
		int length = edges[i].y2 - edges[i].y1;
		if (length > max_length) {
			max_length = length;
			long_edge = i;
		}
	}
	
	int short_edge_1 = (long_edge + 1) % 3;
	int short_edge_2 = (long_edge + 2) % 3;
	
	draw_spans_between_edges(edges[long_edge], edges[short_edge_1]);
	draw_spans_between_edges(edges[long_edge], edges[short_edge_2]);
}
