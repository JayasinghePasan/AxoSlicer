#pragma once
#include "GeometryPicker.h"
#include "../RenderBasics/Direct3D.h"

#define RENDERDOC_NOHELPER 
#include "../renderdoc_app.h"
static RENDERDOC_API_1_6_0* g_rdoc = nullptr;

void InitRenderDocAPI()
{
	HMODULE mod = GetModuleHandleA("renderdoc.dll");
	if (!mod)
	{
		mod = LoadLibraryA("renderdoc.dll");
	}

	if (mod)
	{
		pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
		if (RENDERDOC_GetAPI)
			RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_6_0, (void**)&g_rdoc);
	}
}


using namespace Direct3D;

GeometryPicker::GeometryPicker(std::unordered_map<GUID, iGeometry*, GuidHash, GuidEqual>& map, RenderState& rs)
	:geometryMap(map), renderState(rs)
{
	InitializePickerBuffers();
}

HRESULT GeometryPicker::InitializePickerBuffers()
{
	if (m_isInitilized)
		return S_OK;
	
	HRESULT hr;

	// texture
	CD3D11_TEXTURE2D_DESC txDesc{ DXGI_FORMAT_R32_UINT, 1, 1, 1, 1, D3D11_BIND_RENDER_TARGET };
	hr = device11->CreateTexture2D(&txDesc, nullptr, &m_tex);
	if (FAILED(hr))
		return hr;

	// texture staging
	CD3D11_TEXTURE2D_DESC txStgDesc{ DXGI_FORMAT_R32_UINT, 1, 1, 1, 1, 0, D3D11_USAGE_STAGING, D3D11_CPU_ACCESS_READ };
	hr = device11->CreateTexture2D(&txStgDesc, nullptr, &m_texStage);
	if (FAILED(hr))
		return hr;

	// render target
	CD3D11_RENDER_TARGET_VIEW_DESC rtvDesc{ D3D11_RTV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R32_UINT };
	hr = device11->CreateRenderTargetView(m_tex.Get(), &rtvDesc, &m_rtv);
	if (FAILED(hr))
		return hr;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthTexture;
	CD3D11_TEXTURE2D_DESC txDepthDesc{ DXGI_FORMAT_D32_FLOAT, 1,1,1,1, D3D11_BIND_DEPTH_STENCIL };
	hr = device11->CreateTexture2D(&txDepthDesc, nullptr, &depthTexture);
	if (FAILED(hr))
		return hr;

	CD3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{ D3D11_DSV_DIMENSION_TEXTURE2D, DXGI_FORMAT_D32_FLOAT };
	device11->CreateDepthStencilView(depthTexture.Get(), &dsvDesc, &m_dsv);
	if (FAILED(hr))
		return hr;

	m_isInitilized = true;

	return S_OK;
}

HRESULT GeometryPicker::Pick(int x, int y)
{
	if (!m_isInitilized)
		return S_FALSE;

	// Save current render targets and viewport so picking does not interfere with other views using the same device context
	CComPtr<ID3D11RenderTargetView> oldRTV;
	CComPtr<ID3D11DepthStencilView> oldDSV;
	context->OMGetRenderTargets(1, &oldRTV, &oldDSV);
	UINT num = 1;
	D3D11_VIEWPORT oldVP;
	context->RSGetViewports(&num, &oldVP);
	CComPtr<ID3D11BlendState> oldBlendState;
	FLOAT blendFactor[4];
	UINT sampleMask;
	context->OMGetBlendState(&oldBlendState, blendFactor, &sampleMask);
	context->OMSetBlendState(nullptr, nullptr, 0xffffffff);

	// clear  rtv
	float clearCol[4] = {};
	context->ClearRenderTargetView(m_rtv.Get(), clearCol);
	context->ClearDepthStencilView(m_dsv.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->OMSetRenderTargets(1, m_rtv.GetAddressOf(), m_dsv.Get());
	
	// setup viewport
	D3D11_VIEWPORT pixelViewPort;
	pixelViewPort.Width = renderState.width;
	pixelViewPort.Height = renderState.height;
	pixelViewPort.TopLeftX = (float)-x;
	pixelViewPort.TopLeftY = (float)-y;
	pixelViewPort.MinDepth = 0;
	pixelViewPort.MaxDepth = 1;
	context->RSSetViewports(1, &pixelViewPort);

	// setting IA and shaders
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0, 0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"NORMAL"  ,0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0}
	};

	HRESULT hr = Direct3D::BindShadersFromCSO(L"SimpleVS.cso", L"GeometryPickPS.cso", layout, _countof(layout), nullptr, nullptr, nullptr);
	if (FAILED(hr))
		return hr;

	Render();

	// Restore previous render targets and viewport
	ID3D11RenderTargetView* rtvRestore = oldRTV.p;
	context->OMSetRenderTargets(1, &rtvRestore, oldDSV.p);
	context->RSSetViewports(1, &oldVP);
	context->OMSetBlendState(oldBlendState.p, blendFactor, sampleMask);

	return S_OK;
}

HRESULT GeometryPicker::Render()
{
	if (!renderdocLoaded)
	{
		InitRenderDocAPI();
		renderdocLoaded = true;
	}

	if (g_rdoc) g_rdoc->StartFrameCapture(device11, nullptr);

	int primId = -1;
	for (auto it : geometryMap)
	{
		it.second->Render();
		int newId = -1;
		DownloadPrimitiveId(newId);
		if (primId != newId)
			topPrimitiveGuid = it.first;
		primId = newId;
	}

	if (g_rdoc) g_rdoc->EndFrameCapture(Direct3D::device11, nullptr);

	return S_OK;
}

HRESULT GeometryPicker::DownloadPrimitiveId(int& pickId)
{
	// unbind temporarily to read
	ID3D11RenderTargetView* rtv = nullptr;
	context->OMSetRenderTargets(1, &rtv, nullptr);
	context->CopyResource(m_texStage.Get(), m_tex.Get());

	D3D11_MAPPED_SUBRESOURCE map;
	HRESULT hr = context->Map(m_texStage.Get(), 0, D3D11_MAP_READ, 0, &map);
	if (FAILED(hr))
		return hr;

	const uint32_t* const primID = (uint32_t*)map.pData;
	pickId = static_cast<int>(*primID) - 1;

	context->Unmap(m_texStage.Get(), 0);

	context->OMSetRenderTargets(1, m_rtv.GetAddressOf(), m_dsv.Get());

	return S_OK;
}

void GeometryPicker::ReadGeometry(GUID& geometryId)
{
	geometryId = topPrimitiveGuid;
}