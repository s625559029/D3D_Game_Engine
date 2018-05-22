#ifndef _POINTLIGHT_
#define _POINTLIGHT_

#include "Light.h"

class PointLight : public Light
{
public:
	PointLight(){}

	XMFLOAT3 pos;
	float range;
	XMFLOAT3 att;
	float pad2;
};

#endif // !_POINTLIGHT_

