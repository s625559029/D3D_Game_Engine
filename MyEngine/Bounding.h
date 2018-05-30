#ifndef _BOUNDING_
#define _BOUNDING_

#include "D3DHeader.h"

void CreateBoundingSphere(std::vector<XMFLOAT3> &vertPosArray,    // The array containing our models vertex positions
	float &boundingSphere,                                            // The float containing the radius of our bounding sphere
	XMVECTOR &objectCenterOffset)                                    // A vector containing the distance between the models actual center and (0, 0, 0) in model space
{
	D3DXVECTOR3 minVertex = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
	D3DXVECTOR3 maxVertex = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (UINT i = 0; i < vertPosArray.size(); i++)	{
		// The minVertex and maxVertex will most likely not be actual vertices in the model, but vertices
		// that use the smallest and largest x, y, and z values from the model to be sure ALL vertices are
		// covered by the bounding volume

		//Get the smallest vertex 
		minVertex.x = min(minVertex.x, vertPosArray[i].x);    // Find smallest x value in model
		minVertex.y = min(minVertex.y, vertPosArray[i].y);    // Find smallest y value in model
		minVertex.z = min(minVertex.z, vertPosArray[i].z);    // Find smallest z value in model

															  //Get the largest vertex 
		maxVertex.x = max(maxVertex.x, vertPosArray[i].x);    // Find largest x value in model
		maxVertex.y = max(maxVertex.y, vertPosArray[i].y);    // Find largest y value in model
		maxVertex.z = max(maxVertex.z, vertPosArray[i].z);    // Find largest z value in model
	}

	// Compute distance between maxVertex and minVertex
	float distX = (maxVertex.x - minVertex.x) / 2.0f;
	float distY = (maxVertex.y - minVertex.y) / 2.0f;
	float distZ = (maxVertex.z - minVertex.z) / 2.0f;

	// Now store the distance between (0, 0, 0) in model space to the models real center
	objectCenterOffset = XMVectorSet(maxVertex.x - distX, maxVertex.y - distY, maxVertex.z - distZ, 0.0f);

	// Compute bounding sphere (distance between min and max bounding box vertices)
	// boundingSphere = sqrt(distX*distX + distY*distY + distZ*distZ) / 2.0f;
	boundingSphere = XMVectorGetX(XMVector3Length(XMVectorSet(distX, distY, distZ, 0.0f)));
}

#endif // !_BOUNDING_
