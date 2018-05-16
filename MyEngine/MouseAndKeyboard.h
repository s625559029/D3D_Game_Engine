#ifndef _MOUSEANDKEYBOARD_
#define _MOUSEANDKEYBOARD_

#include "D3DHeader.h"

extern float rotx;
extern float rotz;
extern float scaleX;
extern float scaleY;

bool InitDirectInput(HINSTANCE hInstance);
void DetectInput(double time);

#endif // !_MOUSEANDKEYBOARD_
