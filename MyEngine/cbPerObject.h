#ifndef _CBPEROBJECTS_
#define _CBPEROBJECTS_

#include "D3DHeader.h"

//Constant buffer for objects
struct cbPerObject
{
	XMMATRIX WVP;
	XMMATRIX World;

	XMFLOAT4 difColor;
	bool hasTexture = false;
};

#endif // !_CBPEROBJECTS_
