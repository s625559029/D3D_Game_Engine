#ifndef _CUBE_
#define _CUBE_

#include "D3DHeader.h"

class Cube
{
public:
	Cube() {
		Translation = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
		Rotation = XMMatrixRotationAxis(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),0.0f);
		Scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	}
	Cube(XMMATRIX t,
		XMMATRIX r,
		XMMATRIX s)
		: Translation(t),
		Rotation(r),
		Scale(s)
	{}

	/*XMMATRIX getTransformMatrix()
	{
		return Translation * Rotation * Scale;
	}*/

	XMMATRIX Translation;
	XMMATRIX Rotation;
	XMMATRIX Scale;
};

#endif
