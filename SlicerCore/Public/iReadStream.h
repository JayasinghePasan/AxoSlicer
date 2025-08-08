#pragma once
#include <Unknwn.h>

// Interface for readable streams of bytes
__interface __declspec(uuid("92fa75ac-426c-4e96-97f9-b24ccb1fd5c3")) iReadStream : public IUnknown
{
	HRESULT __stdcall Read(void* pBuffer, ULONG bytesToRead, ULONG* bytesRead);
};
