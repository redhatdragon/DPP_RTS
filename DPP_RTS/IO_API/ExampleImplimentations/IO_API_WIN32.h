#pragma once

/*************************************************************************
				[PUBLIC DOMAIN]

				WIN32 IO API

Build:	Build in VS or your favorite compiler as a win32 application.
You may compile as a console application by using
#define IS_CONSOLE_APPLICATION
For VS you must disable CRT WARNINGS by going into the 
project properties->C/C++->General->SDL checks and "set to no".

Designed with windows needs in mind, this file conforms to the IO_API api.
No additional code tweeks required.

This lib should come with a folder called "Texture" for png loading.

TODO:
Create functions for file, audio, and networking IO.

TESTED WITH:
Windows 7 64 Ultimate
	-With Visual Studio 2017 x86 release mode
*************************************************************************/

#include "IO_API.h"
#include "Texture/picoPNG/picoPNG.h"
#include <Windows.h>
#include <time.h>
#include <stdio.h>

uint32_t canvas[1024 * 768];
uint16_t cWidth = 0;
uint16_t cHeight = 0;



void drawTextureFromFile(const char *fileName, int x, int y) {
	uint32_t *buffer = 0;
	unsigned int width, height;
	getPNGData(fileName, &buffer, &width, &height);
	for (unsigned int i = 0; i < width; i++) {
		for (unsigned int j = 0; j < height; j++) {
			if (!((uint8_t*)&buffer[i + j * width])[4])
				continue;

			unsigned int px = i + x, py = j + y;

			if (px < 0 || py < 0 || px > cWidth - 1 || py > cHeight - 1)
				continue;

			canvas[px + py * cWidth] = buffer[i + j * width];
		}
	}
	free(buffer);
}
void drawTextureFromFileUnsafe(const char *fileName, int x, int y) {
	uint32_t *buffer = NULL;
	unsigned int width, height;
	getPNGData(fileName, &buffer, &width, &height);
	for (unsigned int i = 0; i < height; i++) {
		uint32_t *loc = &canvas[x + (i + y) * cWidth];
		memcpy(loc, &buffer[i*width], width * sizeof(uint32_t));
	}
	free(buffer);
}
void drawTexture(const struct Texture *texture, int _x, int _y) {
	uint32_t *buffer = texture->data;
	unsigned int width = texture->w, height = texture->h;
	for (unsigned int i = 0; i < width; i++) {
		for (unsigned int j = 0; j < height; j++) {
			int x = i + _x, y = j + _y;
			if (!((uint8_t*)&buffer[i + j * width])[4])
				continue;
			if (x < 0 || y < 0 || x > cWidth - 1 || y > cHeight - 1)
				continue;

			canvas[x + y * cWidth] = buffer[i + j * width];
		}
	}
}
void getTexture(const char *fileName, struct Texture *texture) {
	uint32_t *buffer = NULL;
	getPNGData(fileName, &buffer, &texture->w, &texture->h);
	texture->data = buffer;
}

bool getFileData(const char* fileName, uint8_t* dataBuffer, uint32_t startPos, uint32_t numBytes) {
	FILE *file = fopen(fileName, "r+");
	if (!file) {
		printf("getFileData: Failed to open file.");
		return false;
	}
	fseek(file, startPos, SEEK_SET);
	size_t bytesRead = fread(dataBuffer, 1, numBytes, file);
	fclose(file);
	if (bytesRead)
		return true;
	printf("getFileData: failed to read file.");
	return false;
}



void PaintWindow(HWND hWnd) {  //Probably needs a clean and tune up.
	/*PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);

	RECT rect;
	GetClientRect(hWnd, &rect); // To get size of window

	HDC hDCMem = CreateCompatibleDC(hdc); // Create a device context in memory
	int savedDC = SaveDC(hDCMem);


	//SetPixel(hdc, 10, 10, RGB(0, 0xFF, 0));  // green
	//unsigned int canvasLength = cWidth * cHeight;
	for (unsigned int i = 0; i < cWidth; i++) {
		for (unsigned int j = 0; j < cHeight; j++) {
			SetPixel(hdc, i, j, canvas[i+j*cWidth]);
			//SetPixel(hDCMem, i, j, canvas[i + j * cWidth]);
		}
	}

	// Now BitBlt the stuff from memory buffer to screen
	BitBlt(hdc, 0, 0, rect.right, rect.bottom, hDCMem, 0, 0, SRCCOPY);

	// Cleanup time
	RestoreDC(hDCMem, savedDC);
	DeleteDC(hDCMem);

	EndPaint(hWnd, &ps);*/

	/*RECT Client_Rect;
	GetClientRect(hWnd, &Client_Rect);
	int win_width = Client_Rect.right - Client_Rect.left;
	int win_height = Client_Rect.bottom + Client_Rect.left;
	PAINTSTRUCT ps;
	HDC Memhdc;
	HDC hdc;
	HBITMAP Membitmap;
	hdc = BeginPaint(hWnd, &ps);
	Memhdc = CreateCompatibleDC(hdc);
	Membitmap = CreateCompatibleBitmap(hdc, win_width, win_height);

	SelectObject(Memhdc, Membitmap);

	//drawing code goes in here
	for (unsigned int i = 0; i < win_width; i++) {
		for (unsigned int j = 0; j < win_height; j++) {
			SetPixel(Memhdc, i, j, canvas[i + j * cWidth]);
		}
	}


	BitBlt(hdc, 0, 0, win_width, win_height, Memhdc, 0, 0, SRCCOPY);
	DeleteObject(Membitmap);
	DeleteDC(Memhdc);
	DeleteDC(hdc);
	EndPaint(hWnd, &ps);*/



	//CDC* pDC = GetDC();
	//HDC hDC = *pDC;
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(hWnd, &ps);
	HDC hDCMem = CreateCompatibleDC(hDC);

	unsigned char* lpBitmapBits;

	BITMAPINFO bi;
	ZeroMemory(&bi, sizeof(BITMAPINFO));
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = cWidth;
	bi.bmiHeader.biHeight = -cHeight;  //negative so (0,0) is at top left
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;


	HBITMAP bitmap = CreateDIBSection(hDCMem, &bi, DIB_RGB_COLORS, (VOID**)&lpBitmapBits, NULL, 0);
	HGDIOBJ oldbmp = SelectObject(hDCMem, bitmap);

	BitBlt(hDCMem, 0, 0, cWidth, cHeight, hDC, 0, 0, SRCCOPY);

	memcpy(lpBitmapBits, canvas, cWidth*cHeight*4);

	BitBlt(hDC, 0, 0, cWidth, cHeight, hDCMem, 0, 0, SRCCOPY);


	SelectObject(hDCMem, oldbmp);
	DeleteDC(hDCMem);
	DeleteObject(bitmap);

	EndPaint(hWnd, &ps);
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
	case WM_KEYDOWN:	return 0;  // same as pressing the X button:
	case WM_CLOSE:		DestroyWindow(hWnd); return 0;
	case WM_DESTROY:	end(); exit(0); return 0;
						//PostQuitMessage(0);  //May not be needed.
	case WM_PAINT:		PaintWindow(hWnd);   return 0;
	}

	return DefWindowProc(hWnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {
	#ifndef IS_CONSOLE_APPLICATION
	AllocConsole();
	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);
	#endif

	start();

	WNDCLASSW wc = { 0 };
	//wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hbrBackground = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = hInst;
	wc.lpszClassName = L"myWindowClass";
	wc.lpfnWndProc = WindowProcedure;

	if (!RegisterClassW(&wc))
		return -1;

	HWND hWnd = CreateWindowW(L"myWindowClass", 0, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0, 0, cWidth, cHeight, NULL, NULL, NULL, NULL);

	if (IsWindow(hWnd)) {
		DWORD dwStyle = GetWindowLongPtr(hWnd, GWL_STYLE);
		DWORD dwExStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
		HMENU menu = GetMenu(hWnd);
		RECT rc = { 0, 0, cWidth, cHeight };
		AdjustWindowRectEx(&rc, dwStyle, menu ? TRUE : FALSE, dwExStyle);
		SetWindowPos(hWnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOMOVE);
	}


	MSG msg = { 0 };
	//while (GetMessage(&msg, NULL, NULL, NULL)) {
	//while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
	clock_t timeOfLastFrame = 0;
	while (true) {
		if (clock() >= timeOfLastFrame + CLOCKS_PER_SEC / 60) {
			RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
			UpdateWindow(hWnd);
			timeOfLastFrame = clock();
		}

		while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		appLoop();
	}

	return 0;
}

#ifdef IS_CONSOLE_APPLICATION
int main() {
	return WinMain(GetModuleHandle(NULL), NULL, GetCommandLineA(), SW_SHOWNORMAL);
}
#endif