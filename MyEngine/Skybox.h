#ifndef _SKYBOX_
#define _SKYBOX_

#include "Sphere.h"

class SkyBox
{
public:
	SkyBox(){}

	void CreateSkyBox();

	void UpdateSkyBox(Camera & camera);

	void DrawSkyBox(Camera & camera, cbPerObject & _cbPerObj);

	void Clean();

	//Sky box objects
	ID3D11VertexShader* SKYMAP_VS;
	ID3D11PixelShader* SKYMAP_PS;
	ID3D10Blob* SKYMAP_VS_Buffer;
	ID3D10Blob* SKYMAP_PS_Buffer;

	ID3D11ShaderResourceView* smrv;

	ID3D11DepthStencilState* DSLessEqual;

	Sphere sphere;
};


#endif