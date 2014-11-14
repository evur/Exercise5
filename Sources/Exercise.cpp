#include "pch.h"

#include <Kore/Application.h>
#include <Kore/IO/FileReader.h>
#include <Kore/Math/Core.h>
#include <Kore/System.h>
#include <Kore/Input/Keyboard.h>
#include <Kore/Input/KeyEvent.h>
#include <Kore/Input/Mouse.h>
#include <Kore/Audio/Mixer.h>
#include <Kore/Graphics/Image.h>
#include <Kore/Graphics/Graphics.h>
#include "ObjLoader.h"

using namespace Kore;

namespace {
	const int width = 1024;
	const int height = 768;
	double startTime;
	Shader* vertexShader;
	Shader* fragmentShader;
	Program* program;
	VertexBuffer* vertexBuffer;
	IndexBuffer* indexBuffer;
	Mesh* mesh;
	Texture* image;
	TextureUnit tex;

	void update() {
		float t = (float)(System::time() - startTime);
		Kore::Audio::update();
		
		Graphics::begin();
		Graphics::clear(Graphics::ClearColorFlag, 0xff000000);

		program->set();
		image->set(tex);
		vertexBuffer->set();
		indexBuffer->set();
		Graphics::drawIndexedVertices();

		Graphics::end();
		Graphics::swapBuffers();
	}

	void keyDown(KeyEvent* event) {
		if (event->keycode() == Key_Left) {
			// ...
		}
	}

	void keyUp(KeyEvent* event) {
		if (event->keycode() == Key_Left) {
			// ...
		}
	}

	void mouseMove(int x, int y) {

	}
	
	void mousePress(int button, int x, int y) {

	}

	void mouseRelease(int button, int x, int y) {

	}

	void init() {
		mesh = loadObj("tiger.obj");
		image = new Texture("tiger-atlas.jpg", true);

		FileReader vs("shader.vert");
		FileReader fs("shader.frag");
		vertexShader = new Shader(vs.readAll(), vs.size(), VertexShader);
		fragmentShader = new Shader(fs.readAll(), fs.size(), FragmentShader);

		// This defines the structure of your Vertex Buffer
		VertexStructure structure;
		structure.add("pos", Float3VertexData);
		structure.add("tex", Float2VertexData);

		program = new Program;
		program->setVertexShader(vertexShader);
		program->setFragmentShader(fragmentShader);
		program->link(structure);

		tex = program->getTextureUnit("tex");

		// Set this to 1.0f when you do your transformations in the vertex shader
		float scale = 0.005f;

		vertexBuffer = new VertexBuffer(mesh->numVertices, structure);
		float* vertices = vertexBuffer->lock();
		for (int i = 0; i < mesh->numVertices; ++i) {
			vertices[i * 5 + 0] = mesh->vertices[i * 5 + 0] * scale;
			vertices[i * 5 + 1] = mesh->vertices[i * 5 + 1] * scale;
			vertices[i * 5 + 2] = mesh->vertices[i * 5 + 2] * scale;
			vertices[i * 5 + 3] = mesh->vertices[i * 5 + 3];
			vertices[i * 5 + 4] = mesh->vertices[i * 5 + 4];
		}
		vertexBuffer->unlock();

		indexBuffer = new IndexBuffer(mesh->numFaces * 3);
		int* indices = indexBuffer->lock();
		for (int i = 0; i < mesh->numFaces * 3; ++i) {
			indices[i] = mesh->indices[i];
		}
		indexBuffer->unlock();
	}
}

int kore(int argc, char** argv) {
	Application* app = new Application(argc, argv, width, height, false, "Exercise3");
	
	init();

	app->setCallback(update);

	startTime = System::time();
	Kore::Mixer::init();
	Kore::Audio::init();
	//Kore::Mixer::play(new SoundStream("back.ogg", true));
	
	Keyboard::the()->KeyDown = keyDown;
	Keyboard::the()->KeyUp = keyUp;
	Mouse::the()->Move = mouseMove;
	Mouse::the()->Press = mousePress;
	Mouse::the()->Release = mouseRelease;

	app->start();

	delete app;
	
	return 0;
}
