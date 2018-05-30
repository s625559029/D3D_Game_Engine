#include "stdafx.h"
#include "D3DHeader.h"
#include "ObjectsPool.h"
#include "Camera.h"
#include "cbPerObject.h"
#include "Vertex.h"
#include "Skybox.h"

void SkyBox::CreateSkyBox()
{
	sphere = Sphere(10, 10);

	ObjectsPool * objects_pool = ObjectsPool::getInstance();

	//Create sky sphere
	Sphere sphere(10, 10);

	D3DX11CompileFromFile(L"Effects.fx", 0, 0, "SKYMAP_VS", "vs_4_0", 0, 0, 0, &SKYMAP_VS_Buffer, 0, 0);
	D3DX11CompileFromFile(L"Effects.fx", 0, 0, "SKYMAP_PS", "ps_4_0", 0, 0, 0, &SKYMAP_PS_Buffer, 0, 0);

	objects_pool->d3d11Device->CreateVertexShader(SKYMAP_VS_Buffer->GetBufferPointer(),
		SKYMAP_VS_Buffer->GetBufferSize(), NULL, &SKYMAP_VS);
	objects_pool->d3d11Device->CreatePixelShader(SKYMAP_PS_Buffer->GetBufferPointer(),
		SKYMAP_PS_Buffer->GetBufferSize(), NULL, &SKYMAP_PS);

	//Create sky box shader
	//Tell D3D we will be loading a cube texture
	D3DX11_IMAGE_LOAD_INFO loadSMInfo;
	loadSMInfo.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	//Load the texture
	ID3D11Texture2D* SMTexture = 0;
	D3DX11CreateTextureFromFile(objects_pool->d3d11Device, L"skymap.dds",
		&loadSMInfo, 0, (ID3D11Resource**)&SMTexture, 0);

	//Create the sky box textures description
	D3D11_TEXTURE2D_DESC SMTextureDesc;
	SMTexture->GetDesc(&SMTextureDesc);

	//Tell D3D We have a cube texture, which is an array of 2D textures
	D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc;
	SMViewDesc.Format = SMTextureDesc.Format;
	SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	SMViewDesc.TextureCube.MipLevels = SMTextureDesc.MipLevels;
	SMViewDesc.TextureCube.MostDetailedMip = 0;

	//Create the Resource view for sky box
	objects_pool->d3d11Device->CreateShaderResourceView(SMTexture, &SMViewDesc, &smrv);

	//Create depth stencil state for sky box
	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	objects_pool->d3d11Device->CreateDepthStencilState(&dssDesc, &(DSLessEqual));
}

void SkyBox::UpdateSkyBox(Camera & camera)
{
	//Reset sphereWorld
	sphere.sphereWorld = XMMatrixIdentity();

	//Define sphereWorld's world space matrix
	XMMATRIX Scale = XMMatrixScaling(5.0f, 5.0f, 5.0f);
	//Make sure the sphere is always centered around camera
	XMMATRIX Translation = XMMatrixTranslation(XMVectorGetX(camera.camPosition),
		XMVectorGetY(camera.camPosition),
		XMVectorGetZ(camera.camPosition));
	//Set sphereWorld's world space using the transformations
	sphere.sphereWorld = Scale * Translation;
}

void SkyBox::DrawSkyBox(Camera & camera, cbPerObject & _cbPerObj)
{
	ObjectsPool * objects_pool = ObjectsPool::getInstance();

	//Draw sky box
	//Set the spheres index buffer
	objects_pool->d3d11DevCon->IASetIndexBuffer(sphere.sphereIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//Set the spheres vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	objects_pool->d3d11DevCon->IASetVertexBuffers(0, 1, &(sphere.sphereVertBuffer), &stride, &offset);

	//Set the WVP matrix and send it to the constant buffer in effect file
	XMMATRIX WVP;
	WVP = sphere.sphereWorld * camera.camView * camera.camProj;
	_cbPerObj.WVP = XMMatrixTranspose(WVP);
	_cbPerObj.World = XMMatrixTranspose(sphere.sphereWorld);

	objects_pool->d3d11DevCon->UpdateSubresource(objects_pool->cbPerObjectBuffer, 0, NULL, &_cbPerObj, 0, 0);
	objects_pool->d3d11DevCon->VSSetConstantBuffers(0, 1, &(objects_pool->cbPerObjectBuffer));
	//Send our skymap resource view to pixel shader
	objects_pool->d3d11DevCon->PSSetShaderResources(0, 1, &(smrv));
	objects_pool->d3d11DevCon->PSSetSamplers(0, 1, &(objects_pool->CubesTexSamplerState));

	//Set the new VS and PS shaders
	objects_pool->d3d11DevCon->VSSetShader(SKYMAP_VS, 0, 0);
	objects_pool->d3d11DevCon->PSSetShader(SKYMAP_PS, 0, 0);
	//Set the new depth/stencil and RS states
	objects_pool->d3d11DevCon->OMSetDepthStencilState(DSLessEqual, 0);
	objects_pool->d3d11DevCon->RSSetState(objects_pool->RSCullNone);
	objects_pool->d3d11DevCon->DrawIndexed(sphere.NumSphereFaces * 3, 0, 0);
	
	//Set the default VS shader and depth/stencil state
	objects_pool->d3d11DevCon->VSSetShader(objects_pool->VS, 0, 0);
	objects_pool->d3d11DevCon->PSSetShader(objects_pool->PS, 0, 0);
	objects_pool->d3d11DevCon->OMSetDepthStencilState(NULL, 0);
}

void SkyBox::Clean()
{
	//Clean sky box objects
	sphere.cleanSphere();

	SKYMAP_VS->Release();
	SKYMAP_PS->Release();
	SKYMAP_VS_Buffer->Release();
	SKYMAP_PS_Buffer->Release();

	smrv->Release();

	DSLessEqual->Release();
}