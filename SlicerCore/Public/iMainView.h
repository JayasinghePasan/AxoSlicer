#pragma once
#include "iRenderView.h"

__interface iGeometry;
__interface iGeometryManager;

__interface __declspec(uuid("d2f8c1b4-3e5a-4b0c-9f6d-7e1c8f3b2a1e")) iMainView : public iRenderView
{
	HRESULT __stdcall setGeometryManager(iGeometryManager* nativeGeometryManager);
	HRESULT __stdcall zoom(float delta);
	HRESULT __stdcall rotate(float dx, float dy);
	HRESULT __stdcall pan(float dx, float dy);
	HRESULT __stdcall resetView();
	HRESULT __stdcall setProjection(int mode);
	HRESULT __stdcall setViewMode(int mode);
};