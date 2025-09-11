#pragma once
#include "../../pch.h"
#include "MousePicker.h"
#include "../../../Public/iGeometry.h"

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

class GeometryPicker final : public iMousePicker
{
// iMousePicker
public:
	HRESULT Pick(int x, int y) override;
private: 
	HRESULT InitializePickerBuffers() override;
	HRESULT Render() override;
	HRESULT DownloadPrimitiveId(int& pickId) override;

public:
	GeometryPicker(std::unordered_map<GUID, iGeometry*, GuidHash, GuidEqual>& geometryMap, RenderState& renderStates);
	void ReadGeometry(GUID& geometryId);
private:
	bool m_isInitilized = false;
	
	ComPtr<ID3D11Texture2D> m_tex;
	ComPtr<ID3D11Texture2D> m_texStage;
	ComPtr<ID3D11DepthStencilView> m_dsv;
	ComPtr<ID3D11RenderTargetView> m_rtv;

	std::unordered_map<GUID, iGeometry*, GuidHash, GuidEqual>& geometryMap;
	RenderState& renderState;

	GUID topPrimitiveGuid;

	bool renderdocLoaded = false;
};

