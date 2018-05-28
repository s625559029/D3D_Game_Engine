#ifndef _RAY_
#define _RAY

#include "D3DHeader.h"
#include "FPSCamera.h"
#include "MouseAndKeyboard.h"

extern XMVECTOR pickRayInWorldSpacePos;
extern XMVECTOR pickRayInWorldSpaceDir;

void pickRayVector(FPSCamera & cam);

float pick(std::vector<XMFLOAT3>& vertPosArray, std::vector<DWORD>& indexPosArray, XMMATRIX& worldSpace);

#endif
