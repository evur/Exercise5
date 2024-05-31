#pragma once

struct Mesh {
	int num_faces;
	int num_vertices;
	int num_uvs;
	int num_normals;

	float* vertices;
	int* indices;
	float* uvs;
	float* normals;

	// very private
	float* cur_vertex;
	int* cur_index;
	float* cur_uv;
	float* cur_normal;
};

struct Mesh* load_obj(const char* filename);
