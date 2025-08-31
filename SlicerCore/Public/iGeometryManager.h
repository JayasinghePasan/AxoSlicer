#pragma once
#include "iGeometry.h"
#include <Unknwn.h>

// Manages and keeps the Geometrys in the backend
__interface __declspec(uuid("b8c243af-ad61-41e5-a8a0-89579b988b1d")) iGeometryManager : public IUnknown
{
	HRESULT __stdcall AddGeometry(iGeometry* Geometry);
	HRESULT __stdcall RemoveGeometry(iGeometry* Geometry);
};