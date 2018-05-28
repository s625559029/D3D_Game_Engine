#ifndef _FPSCAMERA_
#define _FPSCAMERA_

#include "Camera.h"

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
	{
		
	}

	void UpdateCamera();

	float moveSpeed;
	float camSpeed;

private:
	XMVECTOR DefaultForward;
	XMVECTOR DefaultRight;

	XMVECTOR camForward;
	XMVECTOR camRight;

	XMMATRIX camRotationMatrix;

	DIMOUSESTATE mouseLastState;
};

#endif // !_FPSCAMERA_

