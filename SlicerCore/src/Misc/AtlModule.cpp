#include <atlbase.h>
#include <atlcom.h>
#include <objbase.h>

class CAxoSlicerModule : public ATL::CAtlDllModuleT<CAxoSlicerModule> {};

CAxoSlicerModule g_AtlModule;

extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD reason, LPVOID reserved)
{
    (void)hInstance; 

    if (reason == DLL_PROCESS_ATTACH)
    {
        CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    }
    else if (reason == DLL_PROCESS_DETACH)
    {
        CoUninitialize();
    }

    return g_AtlModule.DllMain(reason, reserved);
}