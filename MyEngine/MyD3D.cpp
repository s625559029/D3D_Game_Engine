#include "stdafx.h"
#include "Config.h"
#include "MyD3D.h"
#include "Camera.h"
#include "Cube.h"
#include "Vertex.h"
#include "Timer.h"
#include "ObjectsPool.h"

ObjectsPool* objects_pool;

std::wstring printText;

//FPS Printer functions
bool InitD2D_D3D101_DWrite(IDXGIAdapter1 *Adapter);
void InitD2DScreenTexture();
void RenderText(std::wstring text, int inInt);

const extern float red;
const extern float green;
const extern float blue;
const extern int colormodr;
const extern int colormodg;
const extern int colormodb;

XMMATRIX WVP;
XMMATRIX World;

//Camera information
Camera camera(XMVectorSet(0.0f, 3.0f, -8.0f, 0.0f),
	XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
	XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),
	0.4f*3.14f, 1.0f, 1000.0f);

//Objects information
Cube cube1;
Cube cube2;

float rot = 0.01f;

struct cbPerObject
{
	XMMATRIX WVP;
};

cbPerObject _cbPerObj;

D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

UINT numElements = ARRAYSIZE(layout);

bool InitializeDirect3d11App(HINSTANCE hInstance)
{
	objects_pool = ObjectsPool::getInstance();
	//Create buffer description
	DXGI_MODE_DESC bufferDesc;

	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

	bufferDesc.Width = Width;
	bufferDesc.Height = Height;
	bufferDesc.RefreshRate.Numerator = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//Create swap chain description
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	IDXGIFactory1 *DXGIFactory;

	HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&DXGIFactory);

	// Use the first adapter    
	IDXGIAdapter1 *Adapter;

	hr = DXGIFactory->EnumAdapters1(0, &Adapter);

	DXGIFactory->Release();

	//Create our Direct3D 11 Device and SwapChain//////////////////////////////////////////////////////////////////////////
	hr = D3D11CreateDeviceAndSwapChain(Adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		NULL, NULL, D3D11_SDK_VERSION, &swapChainDesc, &(objects_pool->SwapChain), &(objects_pool->d3d11Device), NULL, &(objects_pool->d3d11DevCon));

	//Initialize Direct2D, Direct3D 10.1, DirectWrite
	InitD2D_D3D101_DWrite(Adapter);

	//Release the Adapter interface
	Adapter->Release();

	//Create our BackBuffer
	ID3D11Texture2D* BackBuffer;
	objects_pool->SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);

	//Create our Render Target
	objects_pool->d3d11Device->CreateRenderTargetView(BackBuffer, NULL, &(objects_pool->renderTargetView));
	BackBuffer->Release();

	//Set our Render Target
	objects_pool->d3d11DevCon->OMSetRenderTargets(1, &(objects_pool->renderTargetView), NULL);

	//Describe our Depth/Stencil Buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = Width;
	depthStencilDesc.Height = Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	//Create the Depth/Stencil View
	objects_pool->d3d11Device->CreateTexture2D(&depthStencilDesc, NULL, &(objects_pool->depthStencilBuffer));
	objects_pool->d3d11Device->CreateDepthStencilView(objects_pool->depthStencilBuffer, NULL, &(objects_pool->depthStencilView));

	//Set our Render Target
	objects_pool->d3d11DevCon->OMSetRenderTargets(1, &(objects_pool->renderTargetView), objects_pool->depthStencilView);

	return true;
}

void ReleaseObjects()
{
	
}

bool InitScene()
{
	//Init FPS Printer
	InitD2DScreenTexture();

	//Shader process
	D3DX11CompileFromFile(L"Effects.fx", 0, 0, "VS", "vs_4_0", 0, 0, 0, &(objects_pool->VS_Buffer), 0, 0);
	D3DX11CompileFromFile(L"Effects.fx", 0, 0, "PS", "ps_4_0", 0, 0, 0, &(objects_pool->PS_Buffer), 0, 0);

	objects_pool->d3d11Device->CreateVertexShader(objects_pool->VS_Buffer->GetBufferPointer(), 
		objects_pool->VS_Buffer->GetBufferSize(), NULL, &(objects_pool->VS));
	objects_pool->d3d11Device->CreatePixelShader(objects_pool->PS_Buffer->GetBufferPointer(), 
		objects_pool->PS_Buffer->GetBufferSize(), NULL, &(objects_pool->PS));

	objects_pool->d3d11DevCon->VSSetShader(objects_pool->VS, 0, 0);
	objects_pool->d3d11DevCon->PSSetShader(objects_pool->PS, 0, 0);

	//Create the vertex buffer
	Vertex v[] =
	{
		// Front Face
		Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
		Vertex(1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
		Vertex(1.0f, -1.0f, -1.0f, 1.0f, 1.0f),

		// Back Face
		Vertex(-1.0f, -1.0f, 1.0f, 1.0f, 1.0f),
		Vertex(1.0f, -1.0f, 1.0f, 0.0f, 1.0f),
		Vertex(1.0f,  1.0f, 1.0f, 0.0f, 0.0f),
		Vertex(-1.0f,  1.0f, 1.0f, 1.0f, 0.0f),

		// Top Face
		Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f),
		Vertex(-1.0f, 1.0f,  1.0f, 0.0f, 0.0f),
		Vertex(1.0f, 1.0f,  1.0f, 1.0f, 0.0f),
		Vertex(1.0f, 1.0f, -1.0f, 1.0f, 1.0f),

		// Bottom Face
		Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f),
		Vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		Vertex(1.0f, -1.0f,  1.0f, 0.0f, 0.0f),
		Vertex(-1.0f, -1.0f,  1.0f, 1.0f, 0.0f),

		// Left Face
		Vertex(-1.0f, -1.0f,  1.0f, 0.0f, 1.0f),
		Vertex(-1.0f,  1.0f,  1.0f, 0.0f, 0.0f),
		Vertex(-1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
		Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f),

		// Right Face
		Vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		Vertex(1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
		Vertex(1.0f,  1.0f,  1.0f, 1.0f, 0.0f),
		Vertex(1.0f, -1.0f,  1.0f, 1.0f, 1.0f),
	};

	DWORD indices[] = {
		// Front Face
		0,  1,  2,
		0,  2,  3,

		// Back Face
		4,  5,  6,
		4,  6,  7,

		// Top Face
		8,  9, 10,
		8, 10, 11,

		// Bottom Face
		12, 13, 14,
		12, 14, 15,

		// Left Face
		16, 17, 18,
		16, 18, 19,

		// Right Face
		20, 21, 22,
		20, 22, 23
	};

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * 12 * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA indexBufferData;

	indexBufferData.pSysMem = indices;
	objects_pool->d3d11Device->CreateBuffer(&indexBufferDesc, &indexBufferData, &(objects_pool->squareIndexBuffer));

	objects_pool->d3d11DevCon->IASetIndexBuffer(objects_pool->squareIndexBuffer, DXGI_FORMAT_R32_UINT, 0);


	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 24;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = v;
	objects_pool->d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &(objects_pool->squareVertBuffer));

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	objects_pool->d3d11DevCon->IASetVertexBuffers(0, 1, &(objects_pool->squareVertBuffer), &stride, &offset);

	//Create input layout
	objects_pool->d3d11Device->CreateInputLayout(layout, numElements, objects_pool->VS_Buffer->GetBufferPointer(),
		objects_pool->VS_Buffer->GetBufferSize(), &(objects_pool->vertLayout));

	//Set input layout
	objects_pool->d3d11DevCon->IASetInputLayout(objects_pool->vertLayout);

	//Set primitive topology
	objects_pool->d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Create the viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = Width;
	viewport.Height = Height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//Set the Viewport
	objects_pool->d3d11DevCon->RSSetViewports(1, &viewport);

	//Create the buffer to send to the cbuffer in effect file
	D3D11_BUFFER_DESC cbbd;
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerObject);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	objects_pool->d3d11Device->CreateBuffer(&cbbd, NULL, &(objects_pool->cbPerObjectBuffer));

	D3DX11CreateShaderResourceViewFromFile(objects_pool->d3d11Device, L"diablo.jpg", NULL, NULL, &(objects_pool->CubesTexture), NULL );

	//Create blend description
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	// Describe the Sample State
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//Create the Sample State
	objects_pool->d3d11Device->CreateSamplerState(&sampDesc, &(objects_pool->CubesTexSamplerState));

	//Create blend state
	objects_pool->d3d11Device->CreateBlendState(&blendDesc, &(objects_pool->Transparency));

	D3D11_RASTERIZER_DESC cmdesc;

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;
	cmdesc.FrontCounterClockwise = true;
	objects_pool->d3d11Device->CreateRasterizerState(&cmdesc, &(objects_pool->CCWcullMode));

	cmdesc.FrontCounterClockwise = false;

	objects_pool->d3d11Device->CreateRasterizerState(&cmdesc, &(objects_pool->CWcullMode));

	return true;
}

void UpdateScene(double time)
{
	rot += 1.0f * time;
	if (rot > 6.28f)
		rot = 0.0f;

	XMVECTOR rotaxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	cube1.Rotation = XMMatrixRotationAxis(rotaxis, rot);
	cube1.Translation = XMMatrixTranslation(0.0f, 0.0f, 4.0f);
	cube1.Scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);

	cube2.Rotation = XMMatrixRotationAxis(rotaxis, -rot);
	cube2.Translation = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	cube2.Scale = XMMatrixScaling(1.3f, 1.3f, 1.3f);
}

void DrawScene()
{
	D3DXCOLOR bgColor(red, green, blue, 1.0f);

	objects_pool->d3d11DevCon->ClearRenderTargetView(objects_pool->renderTargetView, bgColor);

	objects_pool->d3d11DevCon->ClearDepthStencilView(objects_pool->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//Set our Render Target
	objects_pool->d3d11DevCon->OMSetRenderTargets(1, &(objects_pool->renderTargetView), objects_pool->depthStencilView);

	//Set the default blend state (no blending) for opaque objects
	objects_pool->d3d11DevCon->OMSetBlendState(0, 0, 0xffffffff);

	//Set the cubes index buffer
	objects_pool->d3d11DevCon->IASetIndexBuffer(objects_pool->squareIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//Set the cubes vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	objects_pool->d3d11DevCon->IASetVertexBuffers(0, 1, &(objects_pool->squareVertBuffer), &stride, &offset);

	//Set the World/View/Projection matrix, then send it to constant buffer in effect file
	//Draw cube1
	WVP = cube1.getTransformMatrix() * camera.getCamView() * camera.getCamProjection();

	_cbPerObj.WVP = XMMatrixTranspose(WVP);

	objects_pool->d3d11DevCon->UpdateSubresource(objects_pool->cbPerObjectBuffer, 0, NULL, &_cbPerObj, 0, 0);

	objects_pool->d3d11DevCon->VSSetConstantBuffers(0, 1, &(objects_pool->cbPerObjectBuffer));

	//Set texture for cube1
	objects_pool->d3d11DevCon->PSSetShaderResources(0, 1, &(objects_pool->CubesTexture));
	objects_pool->d3d11DevCon->PSSetSamplers(0, 1, &(objects_pool->CubesTexSamplerState));

	objects_pool->d3d11DevCon->DrawIndexed(36, 0, 0);

	//Draw cube2
	WVP = cube2.getTransformMatrix() * camera.getCamView() * camera.getCamProjection();

	_cbPerObj.WVP = XMMatrixTranspose(WVP);

	objects_pool->d3d11DevCon->UpdateSubresource((objects_pool->cbPerObjectBuffer), 0, NULL, &_cbPerObj, 0, 0);

	objects_pool->d3d11DevCon->VSSetConstantBuffers(0, 1, &(objects_pool->cbPerObjectBuffer));

	//Set texture for cube2
	objects_pool->d3d11DevCon->PSSetShaderResources(0, 1, &(objects_pool->CubesTexture));
	objects_pool->d3d11DevCon->PSSetSamplers(0, 1, &(objects_pool->CubesTexSamplerState));

	objects_pool->d3d11DevCon->DrawIndexed(36, 0, 0);

	RenderText(L"FPS: ", fps);

	objects_pool->SwapChain->Present(0, 0);
}

bool InitD2D_D3D101_DWrite(IDXGIAdapter1 *Adapter)
{
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
	//Create the vertex buffer
	Vertex v[] =
	{
		// Front Face
		Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
		Vertex(1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
		Vertex(1.0f, -1.0f, -1.0f, 1.0f, 1.0f),
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

	WVP = XMMatrixIdentity();
	_cbPerObj.WVP = XMMatrixTranspose(WVP);
	objects_pool->d3d11DevCon->UpdateSubresource(objects_pool->cbPerObjectBuffer, 0, NULL, &_cbPerObj, 0, 0);
	objects_pool->d3d11DevCon->VSSetConstantBuffers(0, 1, &(objects_pool->cbPerObjectBuffer));
	objects_pool->d3d11DevCon->PSSetShaderResources(0, 1, &(objects_pool->d2dTexture));
	objects_pool->d3d11DevCon->PSSetSamplers(0, 1, &(objects_pool->CubesTexSamplerState));

	objects_pool->d3d11DevCon->RSSetState(objects_pool->CWcullMode);
	//Draw the second cube
	objects_pool->d3d11DevCon->DrawIndexed(6, 0, 0);
}