#ifndef  _LIGHT_
#define _LIGHT_

#include "D3DHeader.h"

struct Light
{
	Light()
	{
		ZeroMemory(this, sizeof(Light));
	}
	XMFLOAT3 dir;
	float pad;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
};

#endif // ! _LIGHT_

