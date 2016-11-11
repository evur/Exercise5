#include "pch.h"

#include <Kore/System.h>
#include <Kore/IO/FileReader.h>
#include <Kore/Math/Core.h>
#include <Kore/System.h>
#include <Kore/Input/Keyboard.h>
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
		Graphics::clear(Graphics::ClearColorFlag | Graphics::ClearDepthFlag, 0xff000000);
		
		program->set();
		Graphics::setTexture(tex, image);
		Graphics::setVertexBuffer(*vertexBuffer);
		Graphics::setIndexBuffer(*indexBuffer);
		Graphics::drawIndexedVertices();

		/************************************************************************/
		/* Exercise 5                                                           */
		/************************************************************************/
		/* Set values in your shader using the constant locations you defined, e.g.
		 * Graphics::setMatrix(ConstantLocation, Value);
		*/


		Graphics::end();
		Graphics::swapBuffers();
	}

	void keyDown(KeyCode code, wchar_t character) {
		if (code == Key_Left) {
			// ...
		}
	}

	void keyUp(KeyCode code, wchar_t character) {
		if (code == Key_Left) {
			// ...
		}
	}

	void mouseMove(int window, int x, int y, int movementX, int movementY) {

	}
	
	void mousePress(int window, int button, int x, int y) {

	}

	void mouseRelease(int window, int button, int x, int y) {

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
		structure.add("nor", Float3VertexData);

		program = new Program;
		program->setVertexShader(vertexShader);
		program->setFragmentShader(fragmentShader);
		program->link(structure);

		tex = program->getTextureUnit("tex");

		/************************************************************************/
		/* Exercise 5                                                           */
		/************************************************************************/
		/* Get constant locations from your shader here, e.g.
		 * program->getConstantLocation("bla"); 
		 */
		

		// Set this to 1.0f when you do your transformations in the vertex shader
		float scale = 0.4f;

		vertexBuffer = new VertexBuffer(mesh->numVertices, structure, 0);
		{
			float* vertices = vertexBuffer->lock();
			for (int i = 0; i < mesh->numVertices; ++i) {
				vertices[i * 8 + 0] = mesh->vertices[i * 8 + 0] * scale;
				vertices[i * 8 + 1] = mesh->vertices[i * 8 + 1] * scale;
				vertices[i * 8 + 2] = mesh->vertices[i * 8 + 2] * scale;
				vertices[i * 8 + 3] = mesh->vertices[i * 8 + 3];
				vertices[i * 8 + 4] = 1.0f - mesh->vertices[i * 8 + 4];
				vertices[i * 8 + 5] = mesh->vertices[i * 8 + 5];
				vertices[i * 8 + 6] = mesh->vertices[i * 8 + 6];
				vertices[i * 8 + 7] = mesh->vertices[i * 8 + 7];
			}
			vertexBuffer->unlock();
		}

		{
			indexBuffer = new IndexBuffer(mesh->numFaces * 3);
			int* indices = indexBuffer->lock();
			for (int i = 0; i < mesh->numFaces * 3; ++i) {
				indices[i] = mesh->indices[i];
			}
			indexBuffer->unlock();
		}

		Graphics::setRenderState(DepthTest, true);
		Graphics::setRenderState(DepthTestCompare, ZCompareLess);
	}
}

int kore(int argc, char** argv) {
	Kore::System::init("Exercise 5", width, height);
	
	init();

	Kore::System::setCallback(update);

	startTime = System::time();
	Kore::Mixer::init();
	Kore::Audio::init();
	//Kore::Mixer::play(new SoundStream("back.ogg", true));
	
	Keyboard::the()->KeyDown = keyDown;
	Keyboard::the()->KeyUp = keyUp;
	Mouse::the()->Move = mouseMove;
	Mouse::the()->Press = mousePress;
	Mouse::the()->Release = mouseRelease;

	Kore::System::start();

	return 0;
}
