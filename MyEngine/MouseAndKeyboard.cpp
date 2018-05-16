#include "stdafx.h"
#include "MouseAndKeyboard.h"
#include "Config.h"
#include "ObjectsPool.h"

float rotx = 0;
float rotz = 0;
float scaleX = 1.0f;
float scaleY = 1.0f;

DIMOUSESTATE mouseLastState;

bool InitDirectInput(HINSTANCE hInstance)
{
	ObjectsPool* objects_pool = ObjectsPool::getInstance();
	DirectInput8Create(hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&(objects_pool->DirectInput),
		NULL);

	objects_pool->DirectInput->CreateDevice(GUID_SysKeyboard,
		&(objects_pool->DIKeyboard),
		NULL);

	objects_pool->DirectInput->CreateDevice(GUID_SysMouse,
		&(objects_pool->DIMouse),
		NULL);

	objects_pool->DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	objects_pool->DIKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	objects_pool->DIMouse->SetDataFormat(&c_dfDIMouse);
	objects_pool->DIMouse->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

	return true;
}

void DetectInput(double time)
{
	ObjectsPool* objects_pool = ObjectsPool::getInstance();

	DIMOUSESTATE mouseCurrState;

	BYTE keyboardState[256];
	
	objects_pool->DIKeyboard->Acquire();
	objects_pool->DIMouse->Acquire();

	objects_pool->DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);

	objects_pool->DIKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

	if (keyboardState[DIK_ESCAPE] & 0x80)
		PostMessage(hwnd, WM_DESTROY, 0, 0);

	if (keyboardState[DIK_LEFT] & 0x80)
	{
		rotz -= 1.0f * time;
	}
	if (keyboardState[DIK_RIGHT] & 0x80)
	{
		rotz += 1.0f * time;
	}
	if (keyboardState[DIK_UP] & 0x80)
	{
		rotx += 1.0f * time;
	}
	if (keyboardState[DIK_DOWN] & 0x80)
	{
		rotx -= 1.0f * time;
	}
	if (mouseCurrState.lX != mouseLastState.lX)
	{
		scaleX -= (mouseCurrState.lX * 0.001f);
	}
	if (mouseCurrState.lY != mouseLastState.lY)
	{
		scaleY -= (mouseCurrState.lY * 0.001f);
	}

	if (rotx > 6.28)
		rotx -= 6.28;
	else if (rotx < 0)
		rotx = 6.28 + rotx;

	if (rotz > 6.28)
		rotz -= 6.28;
	else if (rotz < 0)
		rotz = 6.28 + rotz;

	mouseLastState = mouseCurrState;

	return;
}