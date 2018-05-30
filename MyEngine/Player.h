#ifndef _PLAYER_
#define _PLAYER_

#include "D3DHeader.h"

class Player
{
public:
	Player() : keyboardForwardBack(0),
	keyboardLeftRight(0),
	mouseLeftRight(1.0f),
	mouseUpDown(1.0f),
	isShoot(false),
	mouseX(0), 
	mouseY(0)
	{}

	bool InitDirectInput(HINSTANCE hInstance);
	void DetectInput(double time);

	float keyboardForwardBack;
	float keyboardLeftRight;
	float mouseLeftRight;
	float mouseUpDown;
	bool isShoot;

	int mouseX;
	int mouseY;

	DIMOUSESTATE mouseLastState;
};

#endif // !_PLAYER
