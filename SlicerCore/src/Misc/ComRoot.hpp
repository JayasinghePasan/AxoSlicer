#pragma once

#include <Unknwn.h>
#include <atomic>
#include <atlcom.h>
#include <atlbase.h>

template<class I>
class ComRoot : public CComObjectRootEx<CComMultiThreadModel>, public I
{
public:
	ComRoot() = default;
	virtual ~ComRoot() = default;

	BEGIN_COM_MAP(ComRoot)
		COM_INTERFACE_ENTRY(I)
		COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, m_ftm)
	END_COM_MAP()

	HRESULT FinalConstruct()
	{
		// https://stackoverflow.com/a/34978626/126995
		IUnknown* pUnk = GetUnknown();
		return CoCreateFreeThreadedMarshaler(pUnk, &m_ftm);
	}

private:

	CComPtr<IUnknown> m_ftm;
};

template<class CoClass>
inline HRESULT createInstance(CComPtr<CComObject<CoClass>>& p)
{
	p = nullptr;
	CComObject<CoClass>** pp = &p;
	CHECK(CComObject<CoClass>::CreateInstance(pp));
	(*pp)->AddRef();	// Unlike CoCreateInstance, CComObject::CreateInstance creates an object with reference counter = 0
	return S_OK;
}