#pragma once
#include "iRenderView.h"
#include "iMainView.h"
#include "iGeometryManager.h"
#include "iGeometry.h"
#include <Windows.h>

HRESULT __stdcall createMainView(HWND hwnd, iMainView** ppRenderView);
HRESULT __stdcall createGeometryManager(iGeometryManager** ppGeometryManager);
HRESULT __stdcall createGeometryFromSTL(const void* buffer, size_t length, iGeometry** ppGeometry);