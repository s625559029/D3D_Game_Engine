#include "stdafx.h"
#include "Config.h"
#include "MyD3D.h"
#include "FPSCamera.h"
#include "Cube.h"
#include "Vertex.h"
#include "Timer.h"
#include "ObjectsPool.h"
#include "FPSPrinter.h"
#include "Light.h"
#include "MouseAndKeyboard.h"
#include "cbPerFrame.h"
#include "cbPerObject.h"
#include "Skybox.h"

ObjectsPool* objects_pool;

const extern float red;
const extern float green;
const extern float blue;
const extern int colormodr;
const extern int colormodg;
const extern int colormodb;

XMMATRIX WVP;
XMMATRIX World;

//Camera information
FPSCamera camera(XMVectorSet(0.0f, 5.0f, -8.0f, 0.0f),
	XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
	XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),
	0.4f*3.14f, 1.0f, 1000.0f);

//Objects information
Cube cube1;

float rot = 0.01f;

cbPerObject _cbPerObj;

cbPerFrame _cbPerFrame;

Light light;

//Declare sky box
SkyBox sky_box;

//Declare input layout
D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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
	swapChainDesc.Windowed = true;
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
	objects_pool->clean();
	sky_box.cleanSkyBox();
}

bool InitScene()
{
	//Init FPS Printer
	InitD2DScreenTexture();

	//Shader process
	D3DX11CompileFromFile(L"Effects.fx", 0, 0, "VS", "vs_4_0", 0, 0, 0, &(objects_pool->VS_Buffer), 0, 0);
	D3DX11CompileFromFile(L"Effects.fx", 0, 0, "PS", "ps_4_0", 0, 0, 0, &(objects_pool->PS_Buffer), 0, 0);
	D3DX11CompileFromFile(L"Effects.fx", 0, 0, "D2D_PS", "ps_4_0", 0, 0, 0, &(objects_pool->D2D_PS_Buffer), 0, 0);

	objects_pool->d3d11Device->CreateVertexShader(objects_pool->VS_Buffer->GetBufferPointer(), 
		objects_pool->VS_Buffer->GetBufferSize(), NULL, &(objects_pool->VS));
	objects_pool->d3d11Device->CreatePixelShader(objects_pool->PS_Buffer->GetBufferPointer(), 
		objects_pool->PS_Buffer->GetBufferSize(), NULL, &(objects_pool->PS));
	objects_pool->d3d11Device->CreatePixelShader(objects_pool->D2D_PS_Buffer->GetBufferPointer(),
		objects_pool->D2D_PS_Buffer->GetBufferSize(), NULL, &(objects_pool->D2D_PS));

	objects_pool->d3d11DevCon->VSSetShader(objects_pool->VS, 0, 0);
	objects_pool->d3d11DevCon->PSSetShader(objects_pool->PS, 0, 0);

	//Create the light
	light.dir = XMFLOAT3(0.0f, 1.0f, 0.0f);
	light.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	light.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//Create the ground
	Vertex v[] =
	{
		// Bottom Face
		Vertex(-1.0f, -1.0f, -1.0f, 100.0f, 100.0f, 0.0f, 1.0f, 0.0f),
		Vertex(1.0f, -1.0f, -1.0f,   0.0f, 100.0f, 0.0f, 1.0f, 0.0f),
		Vertex(1.0f, -1.0f,  1.0f,   0.0f,   0.0f, 0.0f, 1.0f, 0.0f),
		Vertex(-1.0f, -1.0f,  1.0f, 100.0f,   0.0f, 0.0f, 1.0f, 0.0f),
	};

	DWORD indices[] = {
		0,  1,  2,
		0,  2,  3,
	};

	//Create index buffer
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * 2 * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA indexBufferData;

	indexBufferData.pSysMem = indices;
	objects_pool->d3d11Device->CreateBuffer(&indexBufferDesc, &indexBufferData, &(objects_pool->squareIndexBuffer));

	//Create vertex buffer
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
	objects_pool->d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &(objects_pool->squareVertBuffer));

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

	//Create the buffer to send to the cbuffer per object in effect file
	D3D11_BUFFER_DESC cbbd;
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerObject);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	objects_pool->d3d11Device->CreateBuffer(&cbbd, NULL, &(objects_pool->cbPerObjectBuffer));

	//Create the buffer to send to the cbuffer per frame in effect file
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerFrame);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	objects_pool->d3d11Device->CreateBuffer(&cbbd, NULL, &(objects_pool->cbPerFrameBuffer));

	D3DX11CreateShaderResourceViewFromFile(objects_pool->d3d11Device, L"grass.jpg", NULL, NULL, &(objects_pool->CubesTexture), NULL );

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

	//Create sky box
	sky_box.CreateSkyBox();

	return true;
}

void UpdateScene(double time)
{
	camera.DetectInput(time);

	//Define cube1's world space matrix
	cube1.Translation = XMMatrixTranslation(0.0f, 10.0f, 0.0f);
	cube1.Scale = XMMatrixScaling(500.0f, 10.0f, 500.0f);

	sky_box.UpdateSkyBox(camera);
}

void DrawScene()
{
	D3DXCOLOR bgColor(red, green, blue, 1.0f);

	objects_pool->d3d11DevCon->ClearRenderTargetView(objects_pool->renderTargetView, bgColor);
	objects_pool->d3d11DevCon->ClearDepthStencilView(objects_pool->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//Update constant buffer for light
	_cbPerFrame.light = light;
	objects_pool->d3d11DevCon->UpdateSubresource(objects_pool->cbPerFrameBuffer, 0, NULL, &_cbPerFrame, 0, 0);
	objects_pool->d3d11DevCon->PSSetConstantBuffers(0, 1, &(objects_pool->cbPerFrameBuffer));

	//Reset Vertex and Pixel Shaders
	objects_pool->d3d11DevCon->VSSetShader(objects_pool->VS, 0, 0);
	objects_pool->d3d11DevCon->PSSetShader(objects_pool->PS, 0, 0);

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
	WVP = cube1.Scale * cube1.Translation * camera.getCamView() * camera.getCamProjection();

	_cbPerObj.World = XMMatrixTranspose(cube1.Scale * cube1.Translation);
	_cbPerObj.WVP = XMMatrixTranspose(WVP);

	objects_pool->d3d11DevCon->UpdateSubresource(objects_pool->cbPerObjectBuffer, 0, NULL, &_cbPerObj, 0, 0);

	objects_pool->d3d11DevCon->VSSetConstantBuffers(0, 1, &(objects_pool->cbPerObjectBuffer));

	//Set texture for cube1
	objects_pool->d3d11DevCon->PSSetShaderResources(0, 1, &(objects_pool->CubesTexture));
	objects_pool->d3d11DevCon->PSSetSamplers(0, 1, &(objects_pool->CubesTexSamplerState));

	objects_pool->d3d11DevCon->RSSetState(objects_pool->CCWcullMode);
	objects_pool->d3d11DevCon->DrawIndexed(6, 0, 0);

	sky_box.DrawSkyBox(camera, _cbPerObj);

	RenderText(L"FPS: ", fps);

	objects_pool->SwapChain->Present(0, 0);
}