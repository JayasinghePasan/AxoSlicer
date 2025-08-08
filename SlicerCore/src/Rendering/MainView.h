#pragma once
#include <Windows.h>
#include "../../Public/iMainView.h"
#include "../../src/Misc/ComRoot.hpp"
#include "../../src/Rendering/RenderBasics/ViewBase.h"

class MainView : public ComRoot<iMainView>, public ViewBase 
{
public:

    // --- iRenderView Methods ---
	HRESULT __stdcall resize(const CSize& sizePixels, float dpiScale) override;
	HRESULT __stdcall getSurface(void** pp) override;
	HRESULT __stdcall frontBufferAvailableChanged(BOOL nowAvailable) override;
	HRESULT __stdcall render() override;



};
