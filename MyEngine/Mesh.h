#ifndef _MESH_
#define _MESH_

#include "D3DHeader.h"
#include "SurfaceMaterial.h"
#include "Camera.h"
#include "cbPerObject.h"
#include "ObjectsPool.h"
#include "AABB.h"

class Mesh
{
public:

	Mesh() {}

	//Define LoadObjModel function after we create surfaceMaterial structure
	virtual bool LoadObjModel(std::wstring filename,        //.obj filename
		bool isRHCoordSys,							//true if model was created in right hand coord system
		bool computeNormals);						//true to compute the normals, false to use the files normals

	virtual void Clean();

	virtual void Update();

	void Draw(Camera & cam, cbPerObject & _cbPerObj, bool transparent);

	ID3D11BlendState* Transparency;

	ID3D11Buffer* meshVertBuff;
	ID3D11Buffer* meshIndexBuff;

	XMMATRIX meshWorld;

	int meshSubsets = 0;

	std::vector<int> meshSubsetIndexStart;
	std::vector<int> meshSubsetTexture;

	std::vector<ID3D11ShaderResourceView*> meshSRV;
	std::vector<std::wstring> textureNameArray;

	std::vector<SurfaceMaterial> material;

	XMMATRIX rotation;
	XMMATRIX scale;
	XMMATRIX translation;

	float bounding_sphere_radius;
	XMVECTOR center_offset;

	//Used for bounding box
	std::vector<XMFLOAT3> vertPosArray;
	std::vector<DWORD> vertIndexArray;

	AABB bbox;
};


#endif // !_MESH_

