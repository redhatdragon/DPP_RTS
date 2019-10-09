#include <time.h>
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <thread>
#include "IO_API/IO_API.h"
#include "engine.h"

#include <Windows.h>

clock_t c;

struct Texture testTexture;

void gameStart() {
	c = clock();
	levelLoad();
	getTexture("test.png", &testTexture);
}

void gameEnd() {

}

void render() {
	memset(canvas, 0, sizeof(uint32_t) * MAX_CANVAS_SIZE);
	//drawTexture(&testTexture, 50, 50);
	levelDisplay();
}
/*void gameTick() {
	double duration = ((double)clock() - c) / (double)CLOCKS_PER_SEC;

	if (duration < (double)1 / 60) {
		if (duration < (double)1 / 80) {
			Sleep((DWORD)((double)1 / 120));
		}

		uint8_t keyStates[256];
		getKeyboardState(keyStates);
		static int timer = 0;
		constexpr int timer_reset = 60;
		if (timer > 0) {
			timer--;
		} else {
			if (keyStates['T']) {
				std::cout << duration << std::endl;
			}
			timer = timer_reset;
		}
		render();
		levelRun();
		return;
	}
	c = clock();
}*/
double frames = 60;
void gameTick() {
	double timeStart = ((double)clock()) / (double)CLOCKS_PER_SEC;
	uint8_t keyStates[256];
	getKeyboardState(keyStates);
	if (!keyStates['E']) {
		render();
	}
	levelRun();

	double duration = (((double)clock()) / (double)CLOCKS_PER_SEC) - timeStart;

	while (1) {
		if ((double)1 / duration <= 60) {
			if (keyStates['T']) {
				std::cout << "FPS: " << ((double)1) / duration << std::endl;
				std::cout << "GameObject count: " << levelGetGameObjectCount() << std::endl;
				if (keyStates['E']) {
					std::cout << "Drawing disabled." << std::endl;
					break;
				}
				std::cout << "Drawing enabled." << std::endl;
			}
			break;
		}
		duration = (((double)clock()) / (double)CLOCKS_PER_SEC) - timeStart;
	}
}

void start() {
	cWidth = 640;
	cHeight = 480;
	gameStart();
}
void appLoop() {
	gameTick();
}
void end() {
	gameEnd();
}