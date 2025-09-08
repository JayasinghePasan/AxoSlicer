#pragma once
#include <Unknwn.h>
#include "iRenderView.h"

__interface __declspec(uuid("3b72ec08-c4b7-4a0d-bc3d-8b4382b8eefa")) iViewCube : public iRenderView
{
    HRESULT __stdcall rotate(float dx, float dy);
    HRESULT __stdcall pick(int x, int y, int* faceId);
    HRESULT __stdcall setHighlight(unsigned int faceMask);
    HRESULT __stdcall resetView();
    HRESULT __stdcall setViewMode(int mode);
};