#ifndef _CAMERA_
#define _CAMERA_

#include "D3DHeader.h"
#include "Config.h"

class Camera
{
public:
	Camera(XMVECTOR pos, XMVECTOR tar, XMVECTOR up, FLOAT _fov,
		FLOAT _near, FLOAT _far)
		: camPosition(pos), camTarget(tar), camUp(up),
		  FOV(_fov), nearPlane(_near), farPlane(_far)
	{}

	XMMATRIX getCamView()
	{
		return XMMatrixLookAtLH(camPosition, camTarget, camUp);
	}

	XMMATRIX getCamProjection()
	{
		return XMMatrixPerspectiveFovLH(FOV, (float)Width / Height, nearPlane, farPlane);
	}

	XMVECTOR camPosition;
	XMVECTOR camTarget;
	XMVECTOR camUp;
	FLOAT FOV;
	FLOAT nearPlane;
	FLOAT farPlane;
};

#endif // !_CAMERA_