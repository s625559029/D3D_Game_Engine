#include "stdafx.h"
#include "Config.h"
#include "FPSCamera.h"
#include "ObjectsPool.h"
#include "Player.h"

void FPSCamera::UpdateCamera(Player & player)
{
	float moveBackForward = player.keyboardForwardBack * moveSpeed;
	float moveLeftRight = player.keyboardLeftRight * moveSpeed;

	float camYaw = player.mouseLeftRight * camSpeed;
	float camPitch = player.mouseUpDown * camSpeed;

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

	Camera::UpdateCamera();
}