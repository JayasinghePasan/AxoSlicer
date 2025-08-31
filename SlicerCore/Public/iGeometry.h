#pragma once
#include <Unknwn.h>

__interface __declspec(uuid("83398f8d-cd43-4388-a5f4-1401721aaa56")) iGeometry : public IUnknown
{
	HRESULT __stdcall GetGuid(GUID);
	HRESULT __stdcall Render();
};