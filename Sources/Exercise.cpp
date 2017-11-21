#include "pch.h"

#include <Kore/System.h>
#include <Kore/IO/FileReader.h>
#include <Kore/Math/Core.h>
#include <Kore/System.h>
#include <Kore/Input/Keyboard.h>
#include <Kore/Input/Mouse.h>
#include <Kore/Graphics4/Graphics.h>
#include <Kore/Graphics4/PipelineState.h>

#include "ObjLoader.h"
#include "Memory.h"

using namespace Kore;

namespace {
	const int width = 1024;
	const int height = 768;
	double startTime;
	Graphics4::Shader* vertexShader;
	Graphics4::Shader* fragmentShader;
	Graphics4::PipelineState* pipeline;
	Graphics4::VertexBuffer* vertexBuffer;
	Graphics4::IndexBuffer* indexBuffer;
	Mesh* mesh;
	Graphics4::Texture* image;
	Graphics4::TextureUnit tex;

	void update() {
		float t = (float)(System::time() - startTime);
		
		Graphics4::begin();
		Graphics4::clear(Graphics4::ClearColorFlag | Graphics4::ClearDepthFlag, 0xff000000);
		
		Graphics4::setPipeline(pipeline);
		Graphics4::setTexture(tex, image);
		Graphics4::setVertexBuffer(*vertexBuffer);
		Graphics4::setIndexBuffer(*indexBuffer);
		Graphics4::drawIndexedVertices();

		/************************************************************************/
		/* Exercise 5                                                           */
		/************************************************************************/
		/* Set values in your shader using the constant locations you defined, e.g.
		 * Graphics::setMatrix(location, value);
		*/


		Graphics4::end();
		Graphics4::swapBuffers();
	}

	void keyDown(KeyCode code) {
		if (code == KeyLeft) {
			// ...
		}
	}

	void keyUp(KeyCode code) {
		if (code == KeyLeft) {
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
		Memory::init();
		mesh = loadObj("tiger.obj");
		image = new Graphics4::Texture("tiger-atlas.jpg", true);

		FileReader vs("shader.vert");
		FileReader fs("shader.frag");
		vertexShader = new Graphics4::Shader(vs.readAll(), vs.size(), Graphics4::VertexShader);
		fragmentShader = new Graphics4::Shader(fs.readAll(), fs.size(), Graphics4::FragmentShader);

		// This defines the structure of your Vertex Buffer
		Graphics4::VertexStructure structure;
		structure.add("pos", Graphics4::Float3VertexData);
		structure.add("tex", Graphics4::Float2VertexData);
		structure.add("nor", Graphics4::Float3VertexData);

		pipeline = new Graphics4::PipelineState;
		pipeline->depthWrite = true;
		pipeline->depthMode = Graphics4::ZCompareLess;
		pipeline->inputLayout[0] = &structure;
		pipeline->inputLayout[1] = nullptr;
		pipeline->vertexShader = vertexShader;
		pipeline->fragmentShader = fragmentShader;
		pipeline->compile();

		tex = pipeline->getTextureUnit("tex");

		/************************************************************************/
		/* Exercise 5                                                           */
		/************************************************************************/
		/* Get constant locations from your shader here, e.g.
		 * location = pipeline->getConstantLocation("bla"); 
		 */
		

		// Set this to 1.0f when you do your transformations in the vertex shader
		float scale = 0.4f;

		vertexBuffer = new Graphics4::VertexBuffer(mesh->numVertices, structure, 0);
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
			indexBuffer = new Graphics4::IndexBuffer(mesh->numFaces * 3);
			int* indices = indexBuffer->lock();
			for (int i = 0; i < mesh->numFaces * 3; ++i) {
				indices[i] = mesh->indices[i];
			}
			indexBuffer->unlock();
		}
	}
}

int kore(int argc, char** argv) {
	Kore::System::init("Exercise 5", width, height);
	
	init();

	Kore::System::setCallback(update);

	startTime = System::time();
	
	Keyboard::the()->KeyDown = keyDown;
	Keyboard::the()->KeyUp = keyUp;
	Mouse::the()->Move = mouseMove;
	Mouse::the()->Press = mousePress;
	Mouse::the()->Release = mouseRelease;

	Kore::System::start();

	return 0;
}
