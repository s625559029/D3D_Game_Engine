#ifndef _PICKABLE_
#define _PICKABLE_

#include "Mesh.h"

class Pickable : public Mesh
{
public:
	Pickable() {}

	bool LoadObjModel(std::wstring filename,
		bool isRHCoordSys,
		bool computeNormals);

	void CleanPickable();

	std::vector<XMFLOAT3> vertPosArray;
	std::vector<DWORD> vertIndexArray;
};

#endif