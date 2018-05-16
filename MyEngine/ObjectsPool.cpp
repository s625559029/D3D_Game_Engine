#include "stdafx.h"
#include "ObjectsPool.h"

ObjectsPool* ObjectsPool::instance = new ObjectsPool();

ObjectsPool* ObjectsPool::getInstance()
{
	return instance;
}

ObjectsPool::ObjectsPool()
{

}

void ObjectsPool::clean()
{
	SwapChain->Release();
	d3d11Device->Release();
	d3d11DevCon->Release();
	renderTargetView->Release();
	squareVertBuffer->Release();
	squareIndexBuffer->Release();
	VS->Release();
	PS->Release();
	VS_Buffer->Release();
	PS_Buffer->Release();
	vertLayout->Release();
	depthStencilView->Release();
	depthStencilBuffer->Release();
	cbPerObjectBuffer->Release();

	//Clean PFS Printer objects
	d3d101Device->Release();
	keyedMutex11->Release();
	keyedMutex10->Release();
	D2DRenderTarget->Release();
	Brush->Release();
	sharedTex11->Release();
	DWriteFactory->Release();
	TextFormat->Release();
	d2dTexture->Release();

	//Clean light objects
	cbPerFrameBuffer->Release();
	D2D_PS->Release();
	D2D_PS_Buffer->Release();

	//Clean direct input objects
	DIKeyboard->Unacquire();
	DIMouse->Unacquire();
	DirectInput->Release();
}