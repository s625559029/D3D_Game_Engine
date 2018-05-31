#ifndef _HEIGHTMAP_
#define _HEIGHTMAP_

#include "D3DHeader.h"
#include "Mesh.h"

struct HeightMapInfo {        // Heightmap structure
	int terrainWidth;        // Width of heightmap
	int terrainHeight;        // Height (Length) of heightmap
	XMFLOAT3 *heightMap;    // Array to store terrain's vertex positions
};

class HeightMap : public Mesh
{
public:
	HeightMap() {}

	bool LoadHeightMap(char* filename);
	void InitHeightMap();
	void Draw(Camera & cam, cbPerObject & _cbPerObj);

	int NumFaces = 0;
	int NumVertices = 0;

	HeightMapInfo hminfo;
};



#endif