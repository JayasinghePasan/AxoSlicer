#pragma once
#include "../Misc/ComRoot.hpp"
#include "../../Public/iMainView.h"
#include "../../src/Rendering/RenderBasics/ViewBase.h"
#include <DirectXMath.h>

class MainView : public ComRoot<iMainView>, public ViewBase 
{
private:
	CComPtr<iGeometryManager> geometryManager;
	RenderState renderState;
	bool renderdocLoaded = false;
	

public:
    // --- iRenderView Methods ---
	HRESULT __stdcall resize(const int widthPixels, const int heightPixels, const float dpiScale) override;
	HRESULT __stdcall getSurface(void** pp) override;
	HRESULT __stdcall render() override;

	// --- iMainView Methods ---
	HRESULT __stdcall setGeometryManager(iGeometryManager* nativeGeometryManager) override;
	HRESULT __stdcall zoom(float delta) override;
	HRESULT __stdcall rotate(float dx, float dy) override;
	HRESULT __stdcall pan(float dx, float dy) override;
	HRESULT __stdcall resetView() override;
	HRESULT __stdcall setProjection(int mode) override;

	int GeomCount();

};
