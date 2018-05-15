#ifndef _VERTEX_
#define _VERTEX_

#include "D3DHeader.h"

struct Vertex
{
	Vertex() {}
	Vertex(float x, float y, float z,
		float u, float v)
		: pos(x, y, z), texCoord(u, v) 
	{}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
};

#endif
