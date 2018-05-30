#ifndef _AABB_
#define _AABB_

#include "D3DHeader.h"

class AABB
{
public:
	AABB() {}

	void CreateAABB(std::vector<XMFLOAT3> &vertPosArray);    // The array containing our models vertex positions

	void UpdateAABB(XMMATRIX& worldSpace);

	std::vector<XMFLOAT3> boundingBoxVerts;
	std::vector<DWORD> boundingBoxIndex;

	XMVECTOR objectCenterOffset;

	XMVECTOR max;
	XMVECTOR min;
};

#endif // !_AABB_

