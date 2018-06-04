#ifndef _FPSCAMERA_
#define _FPSCAMERA_

#include "Camera.h"
#include "Player.h"

class FPSCamera : public Camera
{
public:
	FPSCamera(XMVECTOR pos, XMVECTOR tar, XMVECTOR up, FLOAT _fov,
		FLOAT _near, FLOAT _far) :
		Camera(pos, tar, up, _fov, _near, _far),
		DefaultForward(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)),
		DefaultRight(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f)),
		camForward(DefaultForward),
		camRight(DefaultRight),
		moveSpeed(15.0f),
		camSpeed(0.001f)
	{}

	void UpdateCamera(Player & player,
		std::vector<XMFLOAT3>& collidableGeometryPositions,
		std::vector<DWORD>& collidableGeometryIndices);

	float moveSpeed;
	float camSpeed;

	XMVECTOR DefaultForward;
	XMVECTOR DefaultRight;

	XMVECTOR camForward;
	XMVECTOR camRight;

	XMMATRIX camRotationMatrix;
};

#endif // !_FPSCAMERA_

