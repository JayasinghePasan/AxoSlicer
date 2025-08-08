#pragma once
#include "iRenderView.h"
#include "iMainView.h"
#include "iModelManager.h"
#include "iModel.h"
#include "iReadStream.h"
#include <Windows.h>

HRESULT __stdcall createMainView(HWND hwnd, iMainView** ppRenderView);
HRESULT __stdcall createModelManager(iModelManager** ppModelManager);
HRESULT __stdcall createModelFromSTL(iReadStream* pStream, iModel** ppModel);