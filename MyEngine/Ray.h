#ifndef _RAY_
#define _RAY

#include "D3DHeader.h"
#include "FPSCamera.h"
#include "Player.h"

extern XMVECTOR pickRayInWorldSpacePos;
extern XMVECTOR pickRayInWorldSpaceDir;

void pickRayVector(FPSCamera & cam, Player & player);

float pick(std::vector<XMFLOAT3>& vertPosArray, std::vector<DWORD>& indexPosArray, XMMATRIX& worldSpace);

#endif
