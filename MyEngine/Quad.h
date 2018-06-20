#ifndef _QUAD_
#define _QUAD_

#include "D3DHeader.h"
#include "SurfaceMaterial.h"
#include "Camera.h"
#include "cbPerObject.h"
#include "AABB.h"

class Quad
{
public:
	Quad() {}
	
	void load(std::wstring filename);

	ID3D11ShaderResourceView* leafTexture;
	ID3D11Buffer *quadVertBuffer;
	ID3D11Buffer *quadIndexBuffer;
};

#endif // !_QUAD_

