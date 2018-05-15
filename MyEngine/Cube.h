#ifndef _CUBE_
#define _CUBE_

#include "D3DHeader.h"

class Cube
{
public:
	Cube() {}
	Cube(XMMATRIX t,
		XMMATRIX r,
		XMMATRIX s)
		: Translation(t),
		Rotation(r),
		Scale(s)
	{}

	XMMATRIX getTransformMatrix()
	{
		return Translation * Rotation * Scale;
	}

	XMMATRIX Translation;
	XMMATRIX Rotation;
	XMMATRIX Scale;
};

#endif
