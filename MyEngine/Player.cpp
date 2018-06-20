#include "stdafx.h"
#include "Player.h"
#include "Config.h"
#include "ObjectsPool.h"

bool Player::InitDirectInput(HINSTANCE hInstance)
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
	objects_pool->DIMouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

	return true;
}

void Player::DetectInput(double time)
{
	ObjectsPool * pool = ObjectsPool::getInstance();

	keyboardForwardBack = 0.0f;
	keyboardLeftRight = 0.0f;
	isJump = false;

	DIMOUSESTATE mouseCurrState;

	BYTE keyboardState[256];

	pool->DIKeyboard->Acquire();
	pool->DIMouse->Acquire();

	pool->DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);

	pool->DIKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

	if (keyboardState[DIK_ESCAPE] & 0x80)
		PostMessage(hwnd, WM_DESTROY, 0, 0);

	//Check keyboard input
	if (keyboardState[DIK_A] & 0x80)
	{
		keyboardLeftRight -= time;
	}
	if (keyboardState[DIK_D] & 0x80)
	{
		keyboardLeftRight += time;
	}
	if (keyboardState[DIK_W] & 0x80)
	{
		keyboardForwardBack += time;
	}
	if (keyboardState[DIK_S] & 0x80)
	{
		keyboardForwardBack -= time;
	}
	if (keyboardState[DIK_SPACE] & 0x80)
	{
		isJump = true;
	}
	//Check mouse movement
	if ((mouseCurrState.lX != mouseLastState.lX) || (mouseCurrState.lY != mouseLastState.lY))
	{
		mouseLeftRight += mouseCurrState.lX;

		mouseUpDown += mouseCurrState.lY;

		mouseLastState = mouseCurrState;
	}

	if (mouseUpDown < -1569) mouseUpDown = -1569;
	if (mouseUpDown > 1569) mouseUpDown = 1569;

	//Check mouse left button click
	if (mouseCurrState.rgbButtons[0])
	{
		if (isShoot == false)
		{
			POINT mousePos;

			GetCursorPos(&mousePos);
			ScreenToClient(hwnd, &mousePos);

			mouseX = mousePos.x;
			mouseY = mousePos.y;

			isShoot = true;
		}
	}
	if (!mouseCurrState.rgbButtons[0])
	{
		isShoot = false;
	}

	return;
}