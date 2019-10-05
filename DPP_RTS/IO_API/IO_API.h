#pragma once

/*************************************************************************
				[PUBLIC DOMAIN]

				IO_API
An api intended to help build applications in an os agnostic way.
By forcing the application to only touch the below data and function
prototypes; the only thing one will need to port their code to a different
platform will be a cooresponding IO_API_ header specific to their platorm.
If one does not exist this helps make it easy to build such a file for
their application's needs.  All they must do is define the function
prototypes with the necessary logic.  It's designed to work with pure C
applications (optionally C++ of course).

BUILD:  Compilation should be very straight forward and easy.  All you must
do is include the right IO_API_ header in IO_API.c which contains your
platform's specific needs.  Then compile as you would other projects.
No dynamic or static libraries involved.

This API is in ALPHA and will likely change over time to suit broader
uses.  It was designed with game dev in mind but may change over time
to be more general purpose.  It's meant to be a little versitile but
mostly as minimal as possible to avoid "porting hell".

Please feel free to contribute with merge requests or a new IO_API_
implimentations at ____

TODO:
-Debate about using an uint32_t or uint8_t array for the canvas data.
Or have it modifiable.
-Debate on using uint16_t or uint32_t for cWidth and cHeight.
-Debate on using uint32_t* or void* for Texture::data.
-Debate on using uint8_t* or void* for Audio::data.
-Should recvPacketUDP return a PacketUDP* through a parameter?
-Should there be other functionality such as drawing primitive shapes or
storing/displaying fonts.
-Should there be 3D support?
-Should there be a way to get things like screen data?
-Should there be a way to get/resize the window outside the canvas?
-Should we even expose the canvas buffer?  Or have it accessed through a
function?

*************************************************************************/

#include <stdint.h>

#ifndef _STDBOOL
#define _STDBOOL
typedef uint8_t bool;
#define true 1
#define false 0
#endif


#define MAX_CANVAS_SIZE 1024*768 //Change to suit your largest possible resolution
extern uint32_t canvas[MAX_CANVAS_SIZE];
extern uint16_t cWidth, cHeight;



struct Texture {
	uint32_t *data;
	uint32_t w, h;
};
struct Audio {
	uint8_t *data;
	uint32_t playAtTimeMS;
	uint32_t stopAtTimeMS;
	bool canPlay, looping;
};
struct AudioChain {
	uint16_t playAtTrack;
	uint16_t stopAtTrack;
	bool paused, looping;
	uint16_t audioCount;
	struct Audio audio[];
};
struct PacketUDP {
	uint32_t length;
	uint32_t capacity;
	uint8_t data[];
};



void drawTextureFromFile(const char *fileName, int x, int y);
void drawTextureFromFileUnsafe(const char *fileName, int x, int y);
void drawTexture(const struct Texture *texture, int x, int y);
void getTexture(const char *fileName, struct Texture *texture);
void drawText(const char *str, int x, int y);

bool getFileData(const char *fileName, uint8_t *dataBuffer, uint32_t startPos, uint32_t numBytes);
bool getFileText(const char *fileName, char *strBuffer);
bool writeFileData(const char *fileName, uint8_t *data);
bool writeFileText(const char *fileName, char *str);
bool appendToFileData(const char *fileName, uint8_t *data);
bool appendToFileText(const char *fileName, char *str);

bool playAudioFile(const char *fileName);
bool getAudioFromFile(const char *fileName, struct Audio *audioBuffer);
bool playAudio(struct Audio *audio);

bool sendPacketUDP(struct PacketUDP *packet);
struct PacketUDP *recvPacketUDP();



//Define these in your application to be called by the IO_API_ implimentations.
void start();
void appLoop();
void end();