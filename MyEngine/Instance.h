#ifndef _INSTANCE_
#define _INSTANCE_

#include "D3DHeader.h"
#include "Mesh.h"

struct InstanceData
{
	XMFLOAT3 pos;
};

class Instance : public Mesh
{
public:
	Instance() {}

	virtual bool LoadObjModel(std::wstring filename,        //.obj filename
		bool isRHCoordSys,							//true if model was created in right hand coord system
		bool computeNormals,
		int instanceNum,
		std::vector<InstanceData> & inst);						//true to compute the normals, false to use the files normals
	
	void Clean();
	void Draw(Camera & cam, cbPerObject & _cbPerObj, int instanceNum);

	ID3D11Buffer* instanceBuff;
};

#endif
