#ifndef  _FPSPRINTER_
#define _FPSPRINTER_

#include "D3DHeader.h"

//FPS Printer functions
bool InitD2D_D3D101_DWrite(IDXGIAdapter1 *Adapter);
void InitD2DScreenTexture();
void RenderText(std::wstring text, int inInt);

#endif // ! _FPSPRINTER_

