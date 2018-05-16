#ifndef _VERTEX_
#define _VERTEX_

#include "D3DHeader.h"

struct Vertex
{
	Vertex() {}
	Vertex(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz)
		: pos(x, y, z), 
		  texCoord(u, v) ,
		  normal(nx, ny, nz)
	{}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
	XMFLOAT3 normal;
};

#endif
