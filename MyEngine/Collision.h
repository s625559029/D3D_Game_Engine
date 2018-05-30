#ifndef _COLLISION_
#define _COLLISION_

#include "D3DHeader.h"
#include "Movable.h"

bool BoundingSphereCollision(float firstObjBoundingSphere,
	XMVECTOR firstObjCenterOffset,
	XMMATRIX& firstObjWorldSpace,
	float secondObjBoundingSphere,
	XMVECTOR secondObjCenterOffset,
	XMMATRIX& secondObjWorldSpace);

bool BoundingBoxCollision(AABB & bbox_1, AABB & bbox_2);

bool CollisionDetect(Movable & m_1, Mesh & m_2, int method);

#endif