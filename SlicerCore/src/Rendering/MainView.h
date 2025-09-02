#pragma once
#include "../Misc/ComRoot.hpp"
#include "../../Public/iMainView.h"
#include "../../src/Rendering/RenderBasics/ViewBase.h"

class MainView : public ComRoot<iMainView>, public ViewBase 
{
	CComPtr<iGeometryManager> geometryManager;
public:
    // --- iRenderView Methods ---
	HRESULT __stdcall resize(const int widthPixels, const int heightPixels, const float dpiScale) override;
	HRESULT __stdcall getSurface(void** pp) override;
	HRESULT __stdcall render() override;

	HRESULT __stdcall setGeometryManager(iGeometryManager* nativeGeometryManager) override;

};
