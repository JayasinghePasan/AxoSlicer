#pragma once
#include <Unknwn.h>

struct BoundingBox;

__interface __declspec(uuid("83398f8d-cd43-4388-a5f4-1401721aaa56")) iGeometry : public IUnknown
{
	HRESULT __stdcall GetGuid(GUID& guid);
	HRESULT __stdcall Render();
	HRESULT __stdcall GetBoundingBox(BoundingBox& box);
	HRESULT __stdcall Translate(float dx, float dy, float dz);
};