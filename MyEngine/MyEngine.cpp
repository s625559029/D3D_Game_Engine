#include "stdafx.h"
#include "MyD3D.h"
#include "MyEngine.h"
#include "Config.h"

const extern int Width;		//window width
const extern int Height;		//window height

LPCTSTR WndClassName = L"firstwindow";		//Define window class name
HWND hwnd = NULL;		//Windows handle

bool InitializeWindow(HINSTANCE hInstance,
	int ShowWnd,
	int width, int height,
	bool windowed)
{
	WNDCLASSEX window_class;	//Create a window class

	//Initialize window class
	window_class.cbSize = sizeof(WNDCLASSEX);    //Size of our windows class
	window_class.style = CS_HREDRAW | CS_VREDRAW;    //class styles
	window_class.lpfnWndProc = WndProc;    //Default windows procedure function
	window_class.cbClsExtra = NULL;    //Extra bytes after our wc structure
	window_class.cbWndExtra = NULL;    //Extra bytes after our windows instance
	window_class.hInstance = hInstance;    //Instance to current application
	window_class.hIcon = LoadIcon(NULL, IDI_WINLOGO);    //Title bar Icon
	window_class.hCursor = LoadCursor(NULL, IDC_ARROW);    //Default mouse Icon
	window_class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);    //Window bg color
	window_class.lpszMenuName = NULL;    //Name of the menu attached to our window
	window_class.lpszClassName = WndClassName;    //Name of our windows class
	window_class.hIconSm = LoadIcon(NULL, IDI_WINLOGO); //Icon in your taskbar

	if (!RegisterClassEx(&window_class))    //Register our windows class
	{
		//if registration failed, display error
		MessageBox(NULL, L"Error registering class",
			L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	hwnd = CreateWindowEx(    //Create our Extended Window
		NULL,    //Extended style
		WndClassName,    //Name of our windows class
		L"Window Title",    //Name in the title bar of our window
		WS_OVERLAPPEDWINDOW,    //style of our window
		CW_USEDEFAULT, CW_USEDEFAULT,    //Top left corner of window
		width,    //Width of our window
		height,    //Height of our window
		NULL,    //Handle to parent window
		NULL,    //Handle to a Menu
		hInstance,    //Specifies instance of current program
		NULL    //used for an MDI client window
	);

	if (!hwnd)	//Detect window has been created
	{
		MessageBox(NULL, L"Error creating window",
			L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	ShowWindow(hwnd, ShowWnd);
	UpdateWindow(hwnd);

	return true;
}

int messageloop() {
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (true)
	{
		//Detect windows message
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);

			DispatchMessage(&msg);
		}
		//Do D3D stuff
		else
		{
			UpdateScene();
			DrawScene();
		}
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd,    //Default windows procedure
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (msg)
	{
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE) {
				if (MessageBox(0, L"Are you sure you want to exit?",
					L"Really?", MB_YESNO | MB_ICONQUESTION) == IDYES)

					DestroyWindow(hwnd);
			}
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}

	return DefWindowProc(hwnd,
		msg,
		wParam,
		lParam);
}

int WINAPI WinMain(HINSTANCE hInstance,    //Main windows function
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd)
{
	//Initialize Window//
	if (!InitializeWindow(hInstance, nShowCmd, Width, Height, true))
	{
		//If initialization failed, display an error message
		MessageBox(0, L"Window Initialization - Failed",
			L"Error", MB_OK);
		return 0;
	}

	if (!InitializeDirect3d11App(hInstance))    //Initialize Direct3D
	{
		MessageBox(0, L"Direct3D Initialization - Failed",
			L"Error", MB_OK);
		return 0;
	}

	if (!InitScene())    //Initialize our scene
	{
		MessageBox(0, L"Scene Initialization - Failed",
			L"Error", MB_OK);
		return 0;
	}

	messageloop();    

	ReleaseObjects();

	return 0;
}
