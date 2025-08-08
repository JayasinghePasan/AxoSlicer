#pragma once

#include "iModel.h"
#include <Unknwn.h>

// Manages and keeps the models in the backend
__interface __declspec(uuid("b8c243af-ad61-41e5-a8a0-89579b988b1d")) iModelManager : public IUnknown
{
	HRESULT __stdcall AddModel(iModel* model);
	HRESULT __stdcall RemoveModel(iModel* model);
};