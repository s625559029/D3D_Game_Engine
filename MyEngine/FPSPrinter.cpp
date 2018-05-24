#include "stdafx.h"
#include "FPSPrinter.h"
#include "Vertex.h"
#include "Config.h"
#include "ObjectsPool.h"
#include "cbPerObject.h"

bool InitD2D_D3D101_DWrite(IDXGIAdapter1 *Adapter)
{
	ObjectsPool* objects_pool = ObjectsPool::getInstance();

	//Create our Direc3D 10.1 Device///////////////////////////////////////////////////////////////////////////////////////
	D3D10CreateDevice1(Adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_DEBUG | D3D10_CREATE_DEVICE_BGRA_SUPPORT,
		D3D10_FEATURE_LEVEL_9_3, D3D10_1_SDK_VERSION, &(objects_pool->d3d101Device));

	//Create Shared Texture that Direct3D 10.1 will render on//////////////////////////////////////////////////////////////
	D3D11_TEXTURE2D_DESC sharedTexDesc;

	ZeroMemory(&sharedTexDesc, sizeof(sharedTexDesc));

	sharedTexDesc.Width = Width;
	sharedTexDesc.Height = Height;
	sharedTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sharedTexDesc.MipLevels = 1;
	sharedTexDesc.ArraySize = 1;
	sharedTexDesc.SampleDesc.Count = 1;
	sharedTexDesc.Usage = D3D11_USAGE_DEFAULT;
	sharedTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	sharedTexDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;

	objects_pool->d3d11Device->CreateTexture2D(&sharedTexDesc, NULL, &(objects_pool->sharedTex11));

	// Get the keyed mutex for the shared texture (for D3D11)///////////////////////////////////////////////////////////////
	objects_pool->sharedTex11->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&(objects_pool->keyedMutex11));

	// Get the shared handle needed to open the shared texture in D3D10.1///////////////////////////////////////////////////
	IDXGIResource *sharedResource10;
	HANDLE sharedHandle10;

	objects_pool->sharedTex11->QueryInterface(__uuidof(IDXGIResource), (void**)&sharedResource10);

	sharedResource10->GetSharedHandle(&sharedHandle10);

	sharedResource10->Release();

	// Open the surface for the shared texture in D3D10.1///////////////////////////////////////////////////////////////////
	IDXGISurface1 *sharedSurface10;

	objects_pool->d3d101Device->OpenSharedResource(sharedHandle10, __uuidof(IDXGISurface1), (void**)(&sharedSurface10));

	sharedSurface10->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&(objects_pool->keyedMutex10));

	// Create D2D factory///////////////////////////////////////////////////////////////////////////////////////////////////
	ID2D1Factory *D2DFactory;
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), (void**)&D2DFactory);

	D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties;

	ZeroMemory(&renderTargetProperties, sizeof(renderTargetProperties));

	renderTargetProperties.type = D2D1_RENDER_TARGET_TYPE_HARDWARE;
	renderTargetProperties.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);

	D2DFactory->CreateDxgiSurfaceRenderTarget(sharedSurface10, &renderTargetProperties, &(objects_pool->D2DRenderTarget));

	sharedSurface10->Release();
	D2DFactory->Release();

	// Create a solid color brush to draw something with        
	objects_pool->D2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 0.0f, 1.0f), &(objects_pool->Brush));

	//DirectWrite///////////////////////////////////////////////////////////////////////////////////////////////////////////
	DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&(objects_pool->DWriteFactory)));

	objects_pool->DWriteFactory->CreateTextFormat(
		L"Script",
		NULL,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		24.0f,
		L"en-us",
		&(objects_pool->TextFormat)
	);

	objects_pool->TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	objects_pool->TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	objects_pool->d3d101Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	return true;
}

void InitD2DScreenTexture()
{
	ObjectsPool* objects_pool = ObjectsPool::getInstance();

	//Create the vertex buffer
	Vertex v[] =
	{
		// Front Face
		Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f,-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f),
		Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f,-1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f),
		Vertex(1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f),
		Vertex(1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f),
	};

	DWORD indices[] = {
		// Front Face
		0,  1,  2,
		0,  2,  3,
	};

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * 2 * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = indices;
	objects_pool->d3d11Device->CreateBuffer(&indexBufferDesc, &iinitData, &(objects_pool->d2dIndexBuffer));


	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 4;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = v;
	objects_pool->d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &(objects_pool->d2dVertBuffer));

	//Create A shader resource view from the texture D2D will render to,
	//So we can use it to texture a square which overlays our scene
	objects_pool->d3d11Device->CreateShaderResourceView(objects_pool->sharedTex11, NULL, &(objects_pool->d2dTexture));
}

void RenderText(std::wstring text, int inInt)
{
	ObjectsPool* objects_pool = ObjectsPool::getInstance();
	
	objects_pool->d3d11DevCon->PSSetShader(objects_pool->D2D_PS, 0, 0);

	//Release the D3D 11 Device
	objects_pool->keyedMutex11->ReleaseSync(0);

	//Use D3D10.1 device
	objects_pool->keyedMutex10->AcquireSync(0, 5);

	//Draw D2D content        
	objects_pool->D2DRenderTarget->BeginDraw();

	//Clear D2D Background
	objects_pool->D2DRenderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

	//Create our string
	std::wostringstream printString;
	printString << text << inInt;

	std::wstring printText;
	printText = printString.str();

	//Set the Font Color
	D2D1_COLOR_F FontColor = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);

	//Set the brush color D2D will use to draw with
	objects_pool->Brush->SetColor(FontColor);

	//Create the D2D Render Area
	D2D1_RECT_F layoutRect = D2D1::RectF(0, 0, Width, Height);

	//Draw the Text
	objects_pool->D2DRenderTarget->DrawText(
		printText.c_str(),
		wcslen(printText.c_str()),
		objects_pool->TextFormat,
		layoutRect,
		objects_pool->Brush
	);

	objects_pool->D2DRenderTarget->EndDraw();

	//Release the D3D10.1 Device
	objects_pool->keyedMutex10->ReleaseSync(1);

	//Use the D3D11 Device
	objects_pool->keyedMutex11->AcquireSync(1, 5);

	//Use the shader resource representing the direct2d render target
	//to texture a square which is rendered in screen space so it
	//overlays on top of our entire scene. We use alpha blending so
	//that the entire background of the D2D render target is "invisible",
	//And only the stuff we draw with D2D will be visible (the text)

	//Set the blend state for D2D render target texture objects
	objects_pool->d3d11DevCon->OMSetBlendState(objects_pool->Transparency, NULL, 0xffffffff);

	//Set the d2d Index buffer
	objects_pool->d3d11DevCon->IASetIndexBuffer(objects_pool->d2dIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//Set the d2d vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	objects_pool->d3d11DevCon->IASetVertexBuffers(0, 1, &(objects_pool->d2dVertBuffer), &stride, &offset);

	XMMATRIX WVP;
	WVP = XMMatrixIdentity();
	cbPerObject _cbPerObj;
	_cbPerObj.World = XMMatrixTranspose(WVP);
	_cbPerObj.WVP = XMMatrixTranspose(WVP);
	objects_pool->d3d11DevCon->UpdateSubresource(objects_pool->cbPerObjectBuffer, 0, NULL, &_cbPerObj, 0, 0);
	objects_pool->d3d11DevCon->VSSetConstantBuffers(0, 1, &(objects_pool->cbPerObjectBuffer));
	objects_pool->d3d11DevCon->PSSetShaderResources(0, 1, &(objects_pool->d2dTexture));
	objects_pool->d3d11DevCon->PSSetSamplers(0, 1, &(objects_pool->CubesTexSamplerState));

	objects_pool->d3d11DevCon->RSSetState(objects_pool->CWcullMode);
	//Draw the second cube
	objects_pool->d3d11DevCon->DrawIndexed(6, 0, 0);
}