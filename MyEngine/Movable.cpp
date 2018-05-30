#include "stdafx.h"
#include "Movable.h"
#include "Timer.h"
#include "Collision.h"

void Movable::Update(double time)
{
	if (isMoving)
	{
		XMVECTOR tempBottlePos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		tempBottlePos = XMVector3TransformCoord(tempBottlePos, meshWorld) + (meshDir * time * speed);
		XMMATRIX Rotation = XMMatrixRotationY(3.14f);

		meshWorld = XMMatrixIdentity();
		XMMATRIX Translation = XMMatrixTranslation(XMVectorGetX(tempBottlePos), XMVectorGetY(tempBottlePos), XMVectorGetZ(tempBottlePos));

		meshWorld = Translation;
	}
}

void Movable::Clean()
{

}