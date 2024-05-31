#include <kinc/graphics4/graphics.h>
#include <kinc/graphics4/indexbuffer.h>
#include <kinc/graphics4/pipeline.h>
#include <kinc/graphics4/texture.h>
#include <kinc/graphics4/vertexbuffer.h>
#include <kinc/image.h>
#include <kinc/input/keyboard.h>
#include <kinc/input/mouse.h>
#include <kinc/io/filereader.h>
#include <kinc/math/core.h>
#include <kinc/system.h>

#include "Memory.h"
#include "ObjLoader.h"

#define WIDTH 1024
#define HEIGHT 768

int window;
double start_time;
kinc_g4_shader_t* vertex_shader;
kinc_g4_shader_t* fragment_shader;
kinc_g4_pipeline_t* pipeline;
kinc_g4_vertex_buffer_t* vertex_buffer;
kinc_g4_index_buffer_t* index_buffer;
struct Mesh* mesh;
kinc_g4_texture_t* image;
kinc_g4_texture_unit_t tex;

void update(void* data) {
	float t = (float)(kinc_time() - start_time);

	kinc_g4_begin(window);
	kinc_g4_clear(KINC_G4_CLEAR_COLOR|KINC_G4_CLEAR_DEPTH, 0xff000000, 0.0f, 0);

	kinc_g4_set_pipeline(pipeline);
	kinc_g4_set_texture(tex, image);
	kinc_g4_set_vertex_buffer(vertex_buffer);
	kinc_g4_set_index_buffer(index_buffer);
	kinc_g4_draw_indexed_vertices();

	/************************************************************************/
	/* Exercise 5                                                           */
	/************************************************************************/
	/* Set values in your shader using the constant locations you defined, e.g.
		* kinc_g4_set_matrix4(location, value);
	*/

	kinc_g4_end(window);
	kinc_g4_swap_buffers();
}

void key_down(int code, void* data) {
	if (code == KINC_KEY_LEFT) {
		// ...
	}
}

void key_up(int code, void* data) {
	if (code == KINC_KEY_LEFT) {
		// ...
	}
}

void mouse_move(int window, int x, int y, int movementX, int movementY, void* data) {
	// ...
}

void mouse_press(int window, int button, int x, int y, void* data) {
	// ...
}

void mouse_release(int window, int button, int x, int y, void* data) {
	// ...
}

void load_shader(kinc_g4_shader_t* shader, const char* filename, kinc_g4_shader_type_t shader_type) {
	kinc_file_reader_t file;
	kinc_file_reader_open(&file, filename, KINC_FILE_TYPE_ASSET);
	size_t data_size = kinc_file_reader_size(&file);
	void* data = memory_allocate(data_size);
	kinc_file_reader_read(&file, data, data_size);
	kinc_g4_shader_init(shader, data, data_size, shader_type);
}

void init(void) {
	memory_init();
	mesh = load_obj("tiger.obj");
	uint8_t* memory = memory_allocate(10 * 1024 * 1024);
	kinc_image_t im;
	kinc_image_init_from_file(&im, memory, "tiger-atlas.jpg");
	image = memory_allocate(sizeof(*image));
	kinc_g4_texture_init_from_image(image, &im);

	vertex_shader = memory_allocate(sizeof(*vertex_shader));
	load_shader(vertex_shader, "shader.vert", KINC_G4_SHADER_TYPE_VERTEX);
	fragment_shader = memory_allocate(sizeof(*fragment_shader));
	load_shader(fragment_shader, "shader.frag", KINC_G4_SHADER_TYPE_FRAGMENT);

	// This defines the structure of your vertex buffer
	kinc_g4_vertex_structure_t* structure = memory_allocate(sizeof(*structure));
	kinc_g4_vertex_structure_init(structure);
	kinc_g4_vertex_structure_add(structure, "pos", KINC_G4_VERTEX_DATA_FLOAT3);
	kinc_g4_vertex_structure_add(structure, "tex", KINC_G4_VERTEX_DATA_FLOAT2);
	kinc_g4_vertex_structure_add(structure, "nor", KINC_G4_VERTEX_DATA_FLOAT3);

	pipeline = memory_allocate(sizeof(*pipeline));
	kinc_log(KINC_LOG_LEVEL_INFO, "pipeline: %p\n", pipeline);
	kinc_g4_pipeline_init(pipeline);
	kinc_log(KINC_LOG_LEVEL_INFO, "pipeline (initialized): %p\n", pipeline);
	pipeline->depth_write = true;
	pipeline->depth_mode = KINC_G4_COMPARE_LESS;
	pipeline->input_layout[0] = structure;
	pipeline->input_layout[1] = NULL;
	pipeline->vertex_shader = vertex_shader;
	pipeline->fragment_shader = fragment_shader;
	kinc_g4_pipeline_compile(pipeline);

	tex = kinc_g4_pipeline_get_texture_unit(pipeline, "tex");

	/************************************************************************/
	/* Exercise 5                                                           */
	/************************************************************************/
	/* Get constant locations from your shader here, e.g.
		* location = kinc_g4_pipeline_get_constant_location(pipeline, "bla"); 
		*/
	

	// Set this to 1.0f when you do your transformations in the vertex shader
	float scale = 0.4f;

	vertex_buffer = memory_allocate(sizeof(*vertex_buffer));
	kinc_g4_vertex_buffer_init(vertex_buffer, mesh->num_vertices, structure, KINC_G4_USAGE_STATIC, 0);
	{
		float* vertices = kinc_g4_vertex_buffer_lock_all(vertex_buffer);
		for (int i = 0; i < mesh->num_vertices; ++i) {
			vertices[i * 8 + 0] = mesh->vertices[i * 8 + 0] * scale;
			vertices[i * 8 + 1] = mesh->vertices[i * 8 + 1] * scale;
			vertices[i * 8 + 2] = mesh->vertices[i * 8 + 2] * scale;
			vertices[i * 8 + 3] = mesh->vertices[i * 8 + 3];
			vertices[i * 8 + 4] = 1.0f - mesh->vertices[i * 8 + 4];
			vertices[i * 8 + 5] = mesh->vertices[i * 8 + 5];
			vertices[i * 8 + 6] = mesh->vertices[i * 8 + 6];
			vertices[i * 8 + 7] = mesh->vertices[i * 8 + 7];
		}
		kinc_g4_vertex_buffer_unlock_all(vertex_buffer);
	}

	{
		index_buffer = memory_allocate(sizeof(*index_buffer));
		kinc_g4_index_buffer_init(index_buffer, mesh->num_faces * 3, KINC_G4_INDEX_BUFFER_FORMAT_32BIT, KINC_G4_USAGE_STATIC);
		int* indices = kinc_g4_index_buffer_lock_all(index_buffer);
		for (int i = 0; i < mesh->num_faces * 3; ++i) {
			indices[i] = mesh->indices[i];
		}
		kinc_g4_index_buffer_unlock_all(index_buffer);
	}
}

int kickstart(int argc, char** argv) {
	window = kinc_init("Exercise 5", WIDTH, HEIGHT, NULL, NULL);

	init();

	kinc_set_update_callback(update, NULL);

	start_time = kinc_time();

	kinc_keyboard_set_key_down_callback(key_down, NULL);
	kinc_keyboard_set_key_up_callback(key_up, NULL);
	kinc_mouse_set_move_callback(mouse_move, NULL);
	kinc_mouse_set_press_callback(mouse_press, NULL);
	kinc_mouse_set_release_callback(mouse_release, NULL);

	kinc_start();

	return 0;
}
