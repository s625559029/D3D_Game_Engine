#ifndef _MOUSEANDKEYBOARD_
#define _MOUSEANDKEYBOARD_

#include "D3DHeader.h"

namespace DirectInput {

	extern float keyboardLeftRight;
	extern float keyboardForwardBack;
	extern float mouseLeftRight;
	extern float mouseUpDown;
	extern bool isShoot;

	extern int mouseX;
	extern int mouseY;

	bool InitDirectInput(HINSTANCE hInstance);
	void DetectInput(double time);

}

#endif // !_MOUSEANDKEYBOARD_
