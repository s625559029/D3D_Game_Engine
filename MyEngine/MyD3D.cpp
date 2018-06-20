#include "stdafx.h"
#include "Config.h"
#include "MyD3D.h"
#include "FPSCamera.h"
#include "FreeCamera.h"
#include "Cube.h"
#include "Vertex.h"
#include "Timer.h"
#include "ObjectsPool.h"
#include "FPSPrinter.h"
#include "Light.h"
#include "Player.h"
#include "cbPerFrame.h"
#include "cbPerObject.h"
#include "Skybox.h"
#include "Mesh.h"
#include "Instance.h"
#include "Ray.h"
#include "AABB.h"
#include "Bounding.h"
#include "Movable.h"
#include "Collision.h"
#include "HeightMap.h"
#include "Quad.h"

ObjectsPool* objects_pool;

const extern float red;
const extern float green;
const extern float blue;
const extern int colormodr;
const extern int colormodg;
const extern int colormodb;

XMMATRIX WVP;
XMMATRIX World;

Player player;

//Camera information
FPSCamera camera(XMVectorSet(0.0f, 5.0f, -8.0f, 0.0f),
	XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
	XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),
	0.4f*3.14f, 1.0f, 1000.0f);

float rot = 0.01f;

cbPerObject _cbPerObj;

cbPerFrame _cbPerFrame;

Light light;

//Declare sky box
SkyBox sky_box;

Mesh ground;
//Mesh bottle;
HeightMap h_ground;

//Movable bullet;

std::vector<Mesh *> mesh_lists;

int numBottles = 100;
XMMATRIX bottleWorld[100];
int* bottleHit = new int[100];

float bottleBoundingSphere = 0.0f;
std::vector<XMFLOAT3> bottleBoundingBoxVertPosArray;
std::vector<DWORD> bottleBoundingBoxVertIndexArray;
XMVECTOR bottleCenterOffset;

//Bounding strategy
int pickWhat = 1;

AABB bbox;

//Collidale objects soup
std::vector<XMFLOAT3> collidableGeometryPositions;
std::vector<DWORD> collidableGeometryIndices;

//Declare input layout
D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{ "BITANGENT", 0,  DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	// Data from the instance buffer
	{ "INSTANCEPOS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
};
UINT numElements = ARRAYSIZE(layout);

//trees
const int numLeavesPerTree = 800;
const int numTrees = 50;

D3D11_INPUT_ELEMENT_DESC leafLayout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "BITANGENT", 0,  DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	// Instance elements
	// last parameter (InstanceDataStepRate) is set to the number of leaves per tree. InstanceDataStepRate is the number
	// of instances to draw before moving on to the next element in the instance buffer, in this case, the next tree position.
	// We want to make sure that ALL the leaves are drawn for the current tree before moving to the next trees position
	{ "INSTANCEPOS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, numLeavesPerTree }
};
UINT numLeafElements = ARRAYSIZE(leafLayout);

Instance tree;
Quad leaf;

struct cbPerScene
{
	XMMATRIX leafOnTree[numLeavesPerTree];
};
cbPerScene cbPerInst;

ID3D11InputLayout* leafVertLayout;

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
	sky_box.Clean();
	//bottle.Clean();
	//bullet.Clean();
	ground.Clean();
	h_ground.Clean();
}

int vertexOffset;

bool InitScene(HINSTANCE & hInstance)
{
	player.InitDirectInput(hInstance);

	//Init FPS Printer
	InitD2DScreenTexture();

	//Matrix used for initialize objects
	XMMATRIX Rotation;
	XMMATRIX Scale;
	XMMATRIX Translation;

	if (!ground.LoadObjModel(L"ground.obj", true, true))
		return false;

	//Define ground's world space matrix
	Rotation = XMMatrixRotationY(3.14f);
	Scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	Translation = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	ground.meshWorld = Rotation * Scale * Translation;

	// Set up the tree positions then instance buffer
	std::vector<InstanceData> inst(numTrees);
	XMVECTOR tempPos;
	srand(100);
	// We are just creating random positions for the trees, between the positions of (-100, 0, -100) to (100, 0, 100)
	// then storing the position in our instanceData array
	for (int i = 0; i < numTrees; i++)
	{
		float randX = ((float)(rand() % 2000) / 10) - 100;
		float randZ = ((float)(rand() % 2000) / 10) - 100;
		tempPos = XMVectorSet(randX, 0.0f, randZ, 0.0f);

		XMStoreFloat3(&inst[i].pos, tempPos);
	}
	tree.LoadObjModel(L"tree.obj", true, true, numTrees, inst);

	leaf.load(L"leaf.png");

	// Here we create the leaf world matrices, that will be the leafs
	// position and orientation on the tree each individual tree. We will create an array of matrices
	// for the leaves that we will send to the shaders in the cbPerInstance constant buffer
	// This matrix array is used "per tree", so that each tree gets the exact same number of leaves,
	// with the same orientation, position, and scale as all of the other trees
	// Start by initializing the matrix array
	srand(100);
	XMFLOAT3 fTPos;
	XMMATRIX rotationMatrix;
	XMMATRIX tempMatrix;
	for (int i = 0; i < numLeavesPerTree; i++)
	{
		float rotX = (rand() % 2000) / 500.0f; // Value between 0 and 4 PI (two circles, makes it slightly more mixed)
		float rotY = (rand() % 2000) / 500.0f;
		float rotZ = (rand() % 2000) / 500.0f;

		// the rand() function is slightly more biased towards lower numbers, which would make the center of
		// the leaf "mass" be more dense with leaves than the outside of the "sphere" of leaves we are making.
		// We want the outside of the "sphere" of leaves to be more dense than the inside, so the way we do this
		// is getting a distance value between 0 and 4, we then subtract that value from 6, so that the very center
		// does not have any leaves. then below you can see we are checking to see if the distance is greater than 4
		// (because the tree branches are approximately 4 units radius from the center of the tree). If the distance
		// is greater than 4, then we set it at 4, which will make the edge of the "sphere" of leaves more densly
		// populated than the center of the leaf mass
		float distFromCenter = 6.0f - ((rand() % 1000) / 250.0f);

		if (distFromCenter > 4.0f)
			distFromCenter = 4.0f;

		// Now we create a vector with the length of distFromCenter, by simply setting it's x component as distFromCenter.
		// We will now rotate the vector, which will give us the "sphere" of leaves after we have rotated all the leaves.
		// We do not want a perfect sphere, more like a half sphere to cover the branches, so we check to see if the y
		// value is less than -1.0f (giving us slightly more than half a sphere), and if it is, negate it so it is reflected
		// across the xz plane
		tempPos = XMVectorSet(distFromCenter, 0.0f, 0.0f, 0.0f);
		rotationMatrix = XMMatrixRotationRollPitchYaw(rotX, rotY, rotZ);
		tempPos = XMVector3TransformCoord(tempPos, rotationMatrix);

		if (XMVectorGetY(tempPos) < -1.0f)
			tempPos = XMVectorSetY(tempPos, -XMVectorGetY(tempPos));

		// Now we create our leaves "tree" matrix (this is not the leaves "world matrix", because we are not
		// defining the leaves position, orientation, and scale in world space, but instead in "tree" space
		XMStoreFloat3(&fTPos, tempPos);

		Scale = XMMatrixScaling(0.25f, 0.25f, 0.25f);
		Translation = XMMatrixTranslation(fTPos.x, fTPos.y + 8.0f, fTPos.z);
		tempMatrix = Scale * rotationMatrix * Translation;

		// To make things simple, we just store the matrix directly into our cbPerInst structure
		cbPerInst.leafOnTree[i] = XMMatrixTranspose(tempMatrix);
	}

	//if (!bottle.LoadObjModel(L"bottle.obj", true, true))
		//return false;

	//if (!bullet.LoadObjModel(L"bottle.obj", true, true))
		//return false;

	if (!h_ground.LoadHeightMap("heightmap.bmp"))
		return false;
	
	Rotation = XMMatrixRotationY(1.0f);
	Scale = XMMatrixScaling(10.0f, 10.0f, 10.0f);
	Translation = XMMatrixTranslation(-200.0f, -30.0f, 0.0f);
	h_ground.meshWorld = Rotation * Scale * Translation;

	//Init collision soup 
	vertexOffset = collidableGeometryPositions.size();    // Vertex offset (each "mesh" will be added to the end of the positions array)

	// Temp arrays because we need to store the geometry in world space
	XMVECTOR tempVertexPosVec;
	XMFLOAT3 tempVertF3;

	// Push back vertex positions to the polygon soup
	for (int i = 0; i < h_ground.v.size(); i++)
	{
		tempVertexPosVec = XMLoadFloat3(&h_ground.v[i].pos);
		tempVertexPosVec = XMVector3TransformCoord(tempVertexPosVec, h_ground.meshWorld);
		XMStoreFloat3(&tempVertF3, tempVertexPosVec);
		collidableGeometryPositions.push_back(tempVertF3);
	}

	for (int i = 0; i < h_ground.indices.size(); i++)
	{
		collidableGeometryIndices.push_back(h_ground.indices[i] + vertexOffset);
	}

	//////////////
	/*float bottleXPos = -30.0f;
	float bottleZPos = 30.0f;
	float bxadd = 0.0f;
	float bzadd = 0.0f;

	for (int i = 0; i < numBottles; i++)
	{
		bxadd++;

		if (bxadd == 10)
		{
			bzadd -= 1.0f;
			bxadd = 0;
		}

		Rotation = XMMatrixRotationY(3.14f);
		Scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
		Translation = XMMatrixTranslation(bottleXPos + bxadd*10.0f, 4.0f, bottleZPos + bzadd*10.0f);

		bottleWorld[i] = Rotation * Scale * Translation;

		bottleHit[i] = 0;
	}*/
	/////////////

	//CreateBoundingSphere(bottle.vertPosArray, bottleBoundingSphere, bottleCenterOffset);

	//bottle.bounding_sphere_radius = bottleBoundingSphere;
	//bottle.center_offset = bottleCenterOffset;
	//bullet.bounding_sphere_radius = bottleBoundingSphere;
	//bullet.center_offset = bottleCenterOffset;

	//bbox.CreateAABB(bottle.vertPosArray);
	//bottle.bbox = bbox;
	//bbox.CreateAABB(bullet.vertPosArray);
	//bullet.bbox = bbox;

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

	CreateLight();

	//Create input layout
	objects_pool->d3d11Device->CreateInputLayout(layout, numElements, objects_pool->VS_Buffer->GetBufferPointer(),
		objects_pool->VS_Buffer->GetBufferSize(), &(objects_pool->vertLayout));

	objects_pool->d3d11Device->CreateInputLayout(leafLayout, numLeafElements, objects_pool->VS_Buffer->GetBufferPointer(),
		objects_pool->VS_Buffer->GetBufferSize(), &leafVertLayout);

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

	//Create the buffer to send to the cbuffer per instance in effect file
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	// We have already defined how many elements are in our leaf matrix array inside the cbPerScene structure,
	// so we only need the size of the entire structure here, because the number of leaves per tree will not
	// change throughout the scene.
	cbbd.ByteWidth = sizeof(cbPerScene);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	objects_pool->d3d11Device->CreateBuffer(&cbbd, NULL, &objects_pool->cbPerInstanceBuffer);

	objects_pool->d3d11DevCon->UpdateSubresource(objects_pool->cbPerInstanceBuffer, 0, NULL, &cbPerInst, 0, 0);

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

	//Create blend state
	objects_pool->d3d11Device->CreateBlendState(&blendDesc, &(objects_pool->Transparency));

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

	D3D11_RASTERIZER_DESC cmdesc;

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;
	cmdesc.FrontCounterClockwise = true;
	objects_pool->d3d11Device->CreateRasterizerState(&cmdesc, &(objects_pool->CCWcullMode));

	cmdesc.FrontCounterClockwise = false;

	objects_pool->d3d11Device->CreateRasterizerState(&cmdesc, &(objects_pool->CWcullMode));

	//Create rasterize state for sky box
	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_NONE;
	objects_pool->d3d11Device->CreateRasterizerState(&cmdesc, &(objects_pool->RSCullNone));

	//Create sky box
	sky_box.CreateSkyBox();

	return true;
}

bool preShoot = false;

void CheckRayIntersect()
{
	if (player.isShoot && !preShoot)
	{
		/*pickRayVector(camera, player);

		float tempDist;
		float closestDist = FLT_MAX;
		int hitIndex;

		double pickOpStartTime = GetTime();        // Get the time before we start our picking operation

		for (int i = 0; i < numBottles; i++)
		{
			if (bottleHit[i] == 0) // No need to check bottles already hit
			{
				tempDist = FLT_MAX;

				if (pickWhat == 0)
				{
					float pRToPointDist = 0.0f; // Closest distance from the pick ray to the objects center

					XMVECTOR bottlePos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
					XMVECTOR pOnLineNearBottle = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

					//Add offset for bounding to actual center
					bottlePos = XMVector3TransformCoord(bottlePos, bottleWorld[i]) + bottleCenterOffset;

					// This equation gets the point on the pick ray which is closest to bottlePos
					pOnLineNearBottle = pickRayInWorldSpacePos 
						+ XMVector3Dot((bottlePos - pickRayInWorldSpacePos), pickRayInWorldSpaceDir)
						/ XMVector3Dot(pickRayInWorldSpaceDir, pickRayInWorldSpaceDir) * pickRayInWorldSpaceDir;

					//Get distance from point to ray              
					pRToPointDist = XMVectorGetX(XMVector3Length(pOnLineNearBottle - bottlePos));
					
					//If inside sphere
					if (pRToPointDist < bottleBoundingSphere)
					{
						// This line is the distance to the pick ray intersection with the sphere
						//tempDist = XMVectorGetX(XMVector3Length(pOnLineNearBottle - prwsPos));

						// Check for picking with the actual model now
						tempDist = pick(bottle.vertPosArray, bottle.vertIndexArray, bottleWorld[i]);
					}
				}

				// Bounding Box picking test
				if (pickWhat == 1)
					tempDist = pick(bbox.boundingBoxVerts, bbox.boundingBoxIndex, bottleWorld[i]);

				// Check for picking directly with the model without bounding volumes testing first
				if (pickWhat == 2)
					tempDist = pick(bottle.vertPosArray, bottle.vertIndexArray, bottleWorld[i]);

				if (tempDist < closestDist)
				{
					closestDist = tempDist;
					hitIndex = i;
				}
			}
		}

		if (closestDist < FLT_MAX)
		{
			bottleHit[hitIndex] = 1;
		}*/

		/*bullet.isMoving = true;
		bullet.meshWorld = XMMatrixIdentity();
		bullet.meshWorld = XMMatrixTranslation(XMVectorGetX(camera.camPosition), XMVectorGetY(camera.camPosition), XMVectorGetZ(camera.camPosition));
		bullet.meshDir = camera.camTarget - camera.camPosition;*/

		preShoot = true;
	}
	if (!player.isShoot)
	{
		preShoot = false;
	}
}

void HandleCollisions()
{
	/*bullet.bbox.UpdateAABB(bullet.meshWorld);
	for (int i = 0; i < numBottles; i++)
	{
		bottle.meshWorld = bottleWorld[i];
		bottle.bbox.UpdateAABB(bottle.meshWorld);
		if (bottleHit[i] == 0)
		{
			if (CollisionDetect(bullet, bottle, 1))
			{
				bullet.isMoving = false;
				bottleHit[i] = 1;
			}
		}
	}*/
}

void UpdateScene(double time)
{
	player.DetectInput(time);
	
	camera.UpdateCamera(player, collidableGeometryPositions, collidableGeometryIndices);

	sky_box.UpdateSkyBox(camera);

	//CheckRayIntersect();

	//bullet.Update(time);

	//HandleCollisions();
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

	//Reset render targets
	objects_pool->d3d11DevCon->OMSetRenderTargets(1, &objects_pool->renderTargetView, objects_pool->depthStencilView);

	//Reset the default blend state (no blending)
	//Draw non transparent part
	objects_pool->d3d11DevCon->OMSetBlendState(0, 0, 0xffffffff);

	//ground.Draw(camera, _cbPerObj, false);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	/*for (int i = 0; i < numBottles; i++)
	{
		if (bottleHit[i] == 0) 
		{
			bottle.meshWorld = bottleWorld[i];
			bottle.Draw(camera, _cbPerObj, false);
		}
	}*/

	//if (bullet.isMoving) bullet.Draw(camera, _cbPerObj, false);

	h_ground.Draw(camera, _cbPerObj);

	DrawLeaves();
	tree.Draw(camera, _cbPerObj, numTrees);

	sky_box.DrawSkyBox(camera, _cbPerObj);

	//Draw transparent part
	//objects_pool->d3d11DevCon->OMSetBlendState(bottle.Transparency, NULL, 0xffffffff);
	//Our bottle has no transparent part so we won't draw anything here
	
	RenderText(L"FPS: ", fps);

	objects_pool->SwapChain->Present(0, 0);
}

void CreateLight()
{
	//Create the light
	light = Light();
	light.dir = XMFLOAT3(0.0f, 1.0f, -1.0f);
	light.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	light.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}

void DrawLeaves()
{
	///***Draw INSTANCED Leaf Models***///
	// We are now binding two buffers to the input assembler, one for the vertex data,
	// and one for the instance data, so we will have to create a strides array, offsets array
	// and buffer array.
	UINT strides[2] = { sizeof(Vertex), sizeof(InstanceData) };
	UINT offsets[2] = { 0, 0 };

	// Store the vertex and instance buffers into an array
	// The leaves will use the same instance buffer as the trees, because we need each leaf
	// to go to a certain tree
	ID3D11Buffer* vertInstBuffers[2] = { leaf.quadVertBuffer, tree.instanceBuff };

	// Set the leaf input layout. This is where we will set our special input layout for our leaves
	objects_pool->d3d11DevCon->IASetInputLayout(leafVertLayout);

	//Set the models index buffer (same as before)
	objects_pool->d3d11DevCon->IASetIndexBuffer(leaf.quadIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set the models vertex and isntance buffer using the arrays created above
	objects_pool->d3d11DevCon->IASetVertexBuffers(0, 2, vertInstBuffers, strides, offsets);

	//Set the WVP matrix and send it to the constant buffer in effect file
	WVP = tree.meshWorld * camera.camView * camera.camProj;
	_cbPerObj.WVP = XMMatrixTranspose(WVP);
	_cbPerObj.World = XMMatrixTranspose(tree.meshWorld);
	_cbPerObj.hasTexture = true;        // We'll assume all md5 subsets have textures
	_cbPerObj.hasNormMap = false;    // We'll also assume md5 models have no normal map (easy to change later though)
	_cbPerObj.isInstance = true;        // Tell shaders if this is instanced data so it will know to use instance data or not
	_cbPerObj.isLeaf = true;        // Tell shaders if this is the leaf instance so it will know to the cbPerInstance data or not
	objects_pool->d3d11DevCon->UpdateSubresource(objects_pool->cbPerObjectBuffer, 0, NULL, &_cbPerObj, 0, 0);

	// We are sending two constant buffers to the vertex shader now, wo we will create an array of them
	ID3D11Buffer* vsConstBuffers[2] = { objects_pool->cbPerObjectBuffer, objects_pool->cbPerInstanceBuffer };
	objects_pool->d3d11DevCon->VSSetConstantBuffers(0, 2, vsConstBuffers);
	objects_pool->d3d11DevCon->PSSetConstantBuffers(1, 1, &objects_pool->cbPerObjectBuffer);
	objects_pool->d3d11DevCon->PSSetShaderResources(0, 1, &leaf.leafTexture);
	objects_pool->d3d11DevCon->PSSetSamplers(0, 1, &objects_pool->CubesTexSamplerState);

	objects_pool->d3d11DevCon->RSSetState(objects_pool->RSCullNone);
	objects_pool->d3d11DevCon->DrawIndexedInstanced(6, numLeavesPerTree * numTrees, 0, 0, 0);

	// Reset the default Input Layout
	objects_pool->d3d11DevCon->IASetInputLayout(objects_pool->vertLayout);
}