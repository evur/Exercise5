#pragma once

struct Mesh {
	int numFaces;
	int numVertices;
	int numUVs;

	float* vertices;
	int* indices;
	float* uvs;

	// very private
	float* curVertex;
	int* curIndex;
	float* curUV;
};

Mesh* loadObj(const char* filename);
