#include <time.h>
#include <stdio.h>
#include "IO_API/IO_API.h"
#include "engine.h"

#include <Windows.h>

clock_t c;

struct Texture testTexture;

void gameStart() {
	c = clock();
	loadLevel();
	getTexture("test.png", &testTexture);
}

void gameEnd() {
}

void render() {
	drawTexture(&testTexture, 50, 50);
}
void gameTick() {
	double duration = (clock() - c) / (double)CLOCKS_PER_SEC;

	if (duration < (double)1 / 60) {
		if (duration < (double)1 / 80)
			Sleep(((double)1 / 120));

		render();
		return;
	}
	c = clock();
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