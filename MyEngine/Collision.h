#ifndef _COLLISION_
#define _COLLISION_

#include "D3DHeader.h"
#include "Movable.h"

struct CollisionPacket {
	// Information about ellipsoid (in world space)
	XMVECTOR ellipsoidSpace;
	XMVECTOR w_Position;
	XMVECTOR w_Velocity;

	// Information about ellipsoid (in ellipsoid space)
	XMVECTOR e_Position;
	XMVECTOR e_Velocity;
	XMVECTOR e_normalizedVelocity;

	// Collision Information
	bool foundCollision;
	float nearestDistance;
	XMVECTOR intersectionPoint;
	int collisionRecursionDepth;
};

bool BoundingSphereCollision(float firstObjBoundingSphere,
	XMVECTOR firstObjCenterOffset,
	XMMATRIX& firstObjWorldSpace,
	float secondObjBoundingSphere,
	XMVECTOR secondObjCenterOffset,
	XMMATRIX& secondObjWorldSpace);

bool BoundingBoxCollision(AABB & bbox_1, AABB & bbox_2);

bool CollisionDetect(Movable & m_1, Mesh & m_2, int method);

// Collision Detection and Response Function Prototypes
XMVECTOR CollisionSlide(CollisionPacket& cP,            // Pointer to a CollisionPacket object (expects ellipsoidSpace, w_Position and w_Velocity to be filled)
	std::vector<XMFLOAT3>& vertPos,                        // An array holding the polygon soup vertex positions
	std::vector<DWORD>& indices);                        // An array holding the polygon soup indices (triangles)

XMVECTOR CollideWithWorld(CollisionPacket& cP,            // Same arguments as the above function
	std::vector<XMFLOAT3>& vertPos,
	std::vector<DWORD>& indices);

bool SphereCollidingWithTriangle(CollisionPacket& cP,    // Pointer to a CollisionPacket object    
	XMVECTOR &p0,                                        // First vertex position of triangle
	XMVECTOR &p1,                                        // Second vertex position of triangle
	XMVECTOR &p2,                                        // Third vertex position of triangle 
	XMVECTOR &triNormal);                                // Triangle's Normal

// Checks if a point (inside the triangle's plane) is inside the triangle
bool checkPointInTriangle(const XMVECTOR& point, const XMVECTOR& triV1, const XMVECTOR& triV2, const XMVECTOR& triV3);

// Solves the quadratic eqation, and returns the lowest root if equation is solvable, returns false if not solvable
bool getLowestRoot(float a, float b, float c, float maxR, float* root);

#endif