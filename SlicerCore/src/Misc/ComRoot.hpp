#pragma once
#include <atlbase.h>
#include <atlcom.h>
#include <Unknwn.h>

// Generic COM object root that aggregates the free-threaded marshaler
// so the object can be used from any apartment.
template <class Interface>
class ComRoot : public CComObjectRootEx<CComMultiThreadModel>, public Interface
{
public:
    BEGIN_COM_MAP(ComRoot)
        COM_INTERFACE_ENTRY(Interface)
        COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_ftm)
    END_COM_MAP()

    HRESULT FinalConstruct()
    {
        return CoCreateFreeThreadedMarshaler(GetUnknown(), &m_ftm);
    }

private:
    CComPtr<IUnknown> m_ftm;
};

// Helper to create an instance of a CComObject-derived class and
// return it with a reference count of 1.
template <class T>
inline HRESULT createInstance(CComPtr<CComObject<T>>& ptr)
{
    ptr = nullptr;
    CComObject<T>* raw = nullptr;
    HRESULT hr = CComObject<T>::CreateInstance(&raw);
    if (FAILED(hr))
        return hr;
    ptr.Attach(raw); // CreateInstance returns object with ref count 0
    return S_OK;
}
