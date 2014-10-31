#include "pch.h"

#include <Kore/Application.h>
#include <Kore/IO/FileReader.h>
#include <Kore/Math/Core.h>
#include <Kore/System.h>
#include <Kore/Audio/Mixer.h>
#include "SimpleGraphics.h"

using namespace Kore;

namespace {
	double startTime;

	void update() {
		float t = (float)(System::time() - startTime);
		Kore::Audio::update();
		
		startFrame();

		clear(0, 0, 0);
		// Add some fancy rendering
		drawTriangle(10, 10, 100, 50, 10, 150);

		endFrame();
	}
}

int kore(int argc, char** argv) {
	Application* app = new Application(argc, argv, width, height, false, "Exercise3");
	
	initGraphics();
	app->setCallback(update);

	startTime = System::time();
	Kore::Mixer::init();
	Kore::Audio::init();
	//Kore::Mixer::play(new SoundStream("back.ogg", true));

	app->start();

	delete app;
	
	return 0;
}
