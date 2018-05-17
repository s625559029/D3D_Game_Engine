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
		moveLeftRight(0.0f),
		moveBackForward(0.0f),
		camYaw(0.0f),
		camPitch(0.0f)
	{
		
	}

	void DetectInput(double time);
	void UpdateCamera();

	float camYaw;
	float camPitch;

private:
	XMVECTOR DefaultForward;
	XMVECTOR DefaultRight;

	XMVECTOR camForward;
	XMVECTOR camRight;

	float moveLeftRight;
	float moveBackForward;

	XMMATRIX camRotationMatrix;

	DIMOUSESTATE mouseLastState;
};

#endif // !_FPSCAMERA_

