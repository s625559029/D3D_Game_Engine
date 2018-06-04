#ifndef _CONFIG_
#define _CONFIG_

#include "D3DHeader.h"

extern HWND hwnd;

const int Width = 1440;	//window width
const int Height = 900;		//window height

//Back ground color
const float red = 0.0f;
const float green = 0.0f;
const float blue = 0.0f;
const int colormodr = 1;
const int colormodg = 1;
const int colormodb = 1;

const float unitsPerMeter = 100.0f;

const XMVECTOR gravity = XMVectorSet(0.0f, -0.2f, 0.0f, 0.0f);

#endif	// !_CONFIG_