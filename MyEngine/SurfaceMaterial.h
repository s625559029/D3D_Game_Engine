#ifndef _SURFACEMATERIAL_
#define _SURFACEMATERIAL_

struct SurfaceMaterial
{
	std::wstring matName;
	XMFLOAT4 difColor;
	int texArrayIndex;
	bool hasTexture;
	bool transparent;
};

#endif // !_SURFACEMATERIAL_
