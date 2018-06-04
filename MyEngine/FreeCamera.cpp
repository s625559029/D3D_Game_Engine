#include "stdafx.h"
#include "FreeCamera.h"
#include "Collision.h"

void FreeCamera::UpdateCamera(Player & player,
	std::vector<XMFLOAT3>& collidableGeometryPositions,
	std::vector<DWORD>& collidableGeometryIndices)
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

	//camPosition += moveLeftRight*camRight;
	//camPosition += moveBackForward*camForward;

	//////////////
	CollisionPacket cameraCP;
	cameraCP.ellipsoidSpace = XMVectorSet(1.0f, 3.0f, 1.0f, 0.0f);
	cameraCP.w_Position = camPosition;
	cameraCP.w_Velocity = (moveLeftRight*camRight) + (moveBackForward*camForward);

	camPosition = CollisionSlide(cameraCP,
		collidableGeometryPositions,
		collidableGeometryIndices);
	//////////////

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;

	camTarget = camPosition + camTarget;

	Camera::UpdateCamera();
}