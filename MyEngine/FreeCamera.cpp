#include "stdafx.h"
#include "FreeCamera.h"

void FreeCamera::UpdateCamera(Player & player)
{
	float moveBackForward = player.keyboardForwardBack * moveSpeed;
	float moveLeftRight = player.keyboardLeftRight * moveSpeed;

	float camYaw = player.mouseLeftRight * camSpeed;
	float camPitch = player.mouseUpDown * camSpeed;

	camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
	camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camTarget = XMVector3Normalize(camTarget);

	camRight = XMVector3TransformCoord(DefaultRight, camRotationMatrix);
	camForward = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camUp = XMVector3Cross(camForward, camRight);

	camPosition += moveLeftRight*camRight;
	camPosition += moveBackForward*camForward;

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;

	camTarget = camPosition + camTarget;

	Camera::UpdateCamera();
}