#ifndef _MYWINDOW_
#define _MYWINDOW_

#include <windows.h>

extern int ClientWidth;
extern int ClientHeight;

bool InitializeWindow(HINSTANCE hInstance,		//Initialize window
	int ShowWnd,
	int width, int height,
	bool windowed);

int messageloop();

LRESULT CALLBACK WndProc(HWND hWnd,		//Windows callback procedure
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance,		//Main windows function
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd);

#endif