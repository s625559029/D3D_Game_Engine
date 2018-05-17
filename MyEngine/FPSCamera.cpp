#include "stdafx.h"
#include "Config.h"
#include "FPSCamera.h"
#include "ObjectsPool.h"

void FPSCamera::DetectInput(double time)
{
	ObjectsPool * pool = ObjectsPool::getInstance();
	DIMOUSESTATE mouseCurrState;

	BYTE keyboardState[256];

	pool->DIKeyboard->Acquire();
	pool->DIMouse->Acquire();

	pool->DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);

	pool->DIKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

	if (keyboardState[DIK_ESCAPE] & 0x80)
		PostMessage(hwnd, WM_DESTROY, 0, 0);

	float speed = 15.0f * time;

	if (keyboardState[DIK_A] & 0x80)
	{
		moveLeftRight -= speed;
	}
	if (keyboardState[DIK_D] & 0x80)
	{
		moveLeftRight += speed;
	}
	if (keyboardState[DIK_W] & 0x80)
	{
		moveBackForward += speed;
	}
	if (keyboardState[DIK_S] & 0x80)
	{
		moveBackForward -= speed;
	}
	if ((mouseCurrState.lX != mouseLastState.lX) || (mouseCurrState.lY != mouseLastState.lY))
	{
		camYaw += mouseCurrState.lX * 0.001f;

		camPitch += mouseCurrState.lY * 0.001f;
		if (camPitch > 1.57f) camPitch = 1.57f;
		if (camPitch < -1.57f) camPitch = -1.57f;

		mouseLastState = mouseCurrState;
	}

	UpdateCamera();

	return;
}

void FPSCamera::UpdateCamera()
{
	camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
	camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camTarget = XMVector3Normalize(camTarget);

	XMMATRIX RotateYTempMatrix;
	RotateYTempMatrix = XMMatrixRotationY(camYaw);

	camRight = XMVector3TransformCoord(DefaultRight, RotateYTempMatrix);
	camUp = XMVector3TransformCoord(camUp, RotateYTempMatrix);
	camForward = XMVector3TransformCoord(DefaultForward, RotateYTempMatrix);

	camPosition += moveLeftRight*camRight;
	camPosition += moveBackForward*camForward;

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;

	camTarget = camPosition + camTarget;
}