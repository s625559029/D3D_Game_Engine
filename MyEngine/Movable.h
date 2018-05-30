#ifndef _MOVABLE_
#define _MOVABLE_

#include "Mesh.h"

class Movable : public Mesh
{
public:
	Movable() : speed(20.0f), isMoving(false) {}

	void Update(double time);

	void Clean();

	XMVECTOR meshDir;
	float speed;
	bool isMoving;
};

#endif _MOVABLE_