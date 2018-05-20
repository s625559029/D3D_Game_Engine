#ifndef _SPHERE_
#define _SPHERE_

#include "D3DHeader.h"

class Sphere
{
public:
	Sphere() {}
	Sphere(int LatLines, int LongLines);

	void cleanSphere();

	int NumSphereVertices;
	int NumSphereFaces;

	XMMATRIX sphereWorld;

	ID3D11Buffer* sphereIndexBuffer;
	ID3D11Buffer* sphereVertBuffer;
};

#endif // !_SPHERE_

