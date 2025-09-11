#pragma once
#include "../../pch.h"

class iMousePicker
{
	virtual HRESULT InitializePickerBuffers() = 0;
	virtual HRESULT Pick(int x, int y ) = 0;
	virtual HRESULT Render() = 0;
	virtual HRESULT DownloadPrimitiveId(int& pickId) = 0;
};
