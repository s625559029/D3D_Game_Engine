#ifndef _OBJECTSPOOL_
#define _OBJECTSPOOL_

#include "D3DHeader.h"

class ObjectsPool {
public:
	static ObjectsPool* getInstance();
	void clean();

	//Global Declarations//
	IDXGISwapChain* SwapChain;
	ID3D11Device* d3d11Device;
	ID3D11DeviceContext* d3d11DevCon;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11Buffer* squareIndexBuffer;
	ID3D11Buffer* squareVertBuffer;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11VertexShader* VS;
	ID3D11PixelShader* PS;
	ID3D10Blob* VS_Buffer;
	ID3D10Blob* PS_Buffer;
	ID3D11InputLayout* vertLayout;
	ID3D11Buffer* cbPerObjectBuffer;
	ID3D11ShaderResourceView* CubesTexture;
	ID3D11SamplerState* CubesTexSamplerState;

	//Blend state
	ID3D11BlendState* Transparency;
	ID3D11RasterizerState* CCWcullMode;
	ID3D11RasterizerState* CWcullMode;

	//FPS Printer objects
	ID3D10Device1 *d3d101Device;
	IDXGIKeyedMutex *keyedMutex11;
	IDXGIKeyedMutex *keyedMutex10;
	ID2D1RenderTarget *D2DRenderTarget;
	ID2D1SolidColorBrush *Brush;
	ID3D11Texture2D *sharedTex11;
	ID3D11Buffer *d2dVertBuffer;
	ID3D11Buffer *d2dIndexBuffer;
	ID3D11ShaderResourceView *d2dTexture;
	IDWriteFactory *DWriteFactory;
	IDWriteTextFormat *TextFormat;

private:
	static ObjectsPool* instance;

protected:
	ObjectsPool();
};

#endif
