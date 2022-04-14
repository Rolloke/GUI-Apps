// Sample2MemRenderer.cpp: implementation of CSample2MemRenderer class.
// 
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include <streams.h>     // DirectShow
#include <initguid.h>    // DEFINE_GUID to declare an EXTERN_C const.
#include <stdio.h>



#include "Samp2MemInputPin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////////
CSamp2MemInputPin::CSamp2MemInputPin(CBaseRenderer *pRenderer, HRESULT *phr, LPCWSTR pPinName) :
    CRendererInputPin(pRenderer, phr, pPinName)
{

}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSamp2MemInputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	CheckPointer(ppv,E_POINTER);
	if (riid == IID_IAMStreamConfig) 
	{
	   return GetInterface((IAMStreamConfig *) this, ppv);
	}
	else if (riid == IID_IOverlay) 
	{
	   return GetInterface((IOverlay *) this, ppv);
	}
	else if (riid == IID_IKsPropertySet) 
	{
	   return GetInterface((IKsPropertySet *) this, ppv);
	}
	else
	{
		return CRendererInputPin::NonDelegatingQueryInterface(riid, ppv);
	}
		 
}
// IAMStreamConfig interface functions
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSamp2MemInputPin::GetFormat(AM_MEDIA_TYPE **pmt)
{
	SIMPLE_TRACE(_T("IAMStreamConfig::GetFormat\n"));
	if (!IsConnected()) return VFW_E_NOT_CONNECTED;
	if (pmt == NULL)    return E_POINTER;
	*pmt = CreateMediaType(&m_mt);
	if (*pmt == NULL)   return E_OUTOFMEMORY;
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSamp2MemInputPin::GetNumberOfCapabilities(int *piCount, int *piSize)
{
	SIMPLE_TRACE(_T("IAMStreamConfig::GetNumberOfCapabilities\n"));
	return E_NOTIMPL;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSamp2MemInputPin::GetStreamCaps(int iIndex, AM_MEDIA_TYPE **pmt, BYTE *pSCC)
{
	SIMPLE_TRACE(_T("IAMStreamConfig::GetStreamCaps\n"));
	return E_NOTIMPL;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSamp2MemInputPin::SetFormat(AM_MEDIA_TYPE *pmt)
{
	SIMPLE_TRACE(_T("IAMStreamConfig::SetFormat\n"));
	return E_NOTIMPL;
}
// IOverlay interface functions
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSamp2MemInputPin::GetPalette(DWORD *pdwColors, PALETTEENTRY **ppPalette)
{
	SIMPLE_TRACE(_T("IOverlay::GetPalette\n"));
	return E_NOTIMPL;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSamp2MemInputPin::SetPalette(DWORD dwColors, PALETTEENTRY *pPalette)
{
	SIMPLE_TRACE(_T("IOverlay::SetPalette\n"));
	return E_NOTIMPL;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSamp2MemInputPin::GetDefaultColorKey(COLORKEY *pColorKey)
{
	SIMPLE_TRACE(_T("IOverlay::GetDefaultColorKey\n"));
	return E_NOTIMPL;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSamp2MemInputPin::GetColorKey(COLORKEY *pColorKey)
{
	SIMPLE_TRACE(_T("IOverlay::GetColorKey\n"));
	return E_NOTIMPL;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSamp2MemInputPin::SetColorKey(COLORKEY *pColorKey)
{
	SIMPLE_TRACE(_T("IOverlay::SetColorKey\n"));
	return E_NOTIMPL;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSamp2MemInputPin::GetWindowHandle(HWND *pHwnd)
{
	SIMPLE_TRACE(_T("IOverlay::GetWindowHandle\n"));
	return E_NOTIMPL;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSamp2MemInputPin::GetClipList(RECT *pSourceRect,RECT *pDestinationRect, RGNDATA **ppRgnData)
{
	SIMPLE_TRACE(_T("IOverlay::GetClipList\n"));
	return E_NOTIMPL;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSamp2MemInputPin::GetVideoPosition(RECT *pSourceRect, RECT *pDestinationRect)
{
	SIMPLE_TRACE(_T("IOverlay::GetVideoPosition\n"));
	return E_NOTIMPL;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSamp2MemInputPin::Advise(IOverlayNotify *pOverlayNotify, DWORD dwInterests)
{
	SIMPLE_TRACE(_T("IOverlay::Advise\n"));
	return VFW_E_NOT_OVERLAY_CONNECTION;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSamp2MemInputPin::Unadvise(void)
{
	SIMPLE_TRACE(_T("IOverlay::Unadvise\n"));
	return E_NOTIMPL;
}
//////////////////////////////////////////////////////////////////////////
CString GetString(GUID guid)
{
	CString str;
	str.Format(_T("{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}"),
		guid.Data1,	guid.Data2,	guid.Data3,
		guid.Data4[0],	guid.Data4[1],	guid.Data4[2],	guid.Data4[3],
		guid.Data4[4],	guid.Data4[5],	guid.Data4[6],	guid.Data4[7]);
	return str;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSamp2MemInputPin::Set(REFGUID guidPropSet,DWORD dwPropID,
												LPVOID pInstanceData, DWORD cbInstanceData,
												LPVOID pPropData,DWORD cbPropData)
{
	SIMPLE_TRACE(_T("CSamp2MemInputPin::Set"));
	CString str = GetString(guidPropSet);
	SIMPLE_TRACE(str);
	SIMPLE_TRACE(_T("\n"));
	return E_PROP_SET_UNSUPPORTED;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSamp2MemInputPin::Get(REFGUID guidPropSet, DWORD dwPropID, LPVOID pInstanceData, DWORD cbInstanceData, LPVOID pPropData, DWORD cbPropData, DWORD *pcbReturned)
{
	SIMPLE_TRACE(_T("CSamp2MemInputPin::Get"));
	CString str = GetString(guidPropSet);
	SIMPLE_TRACE(str);
	SIMPLE_TRACE(_T("\n"));
	return E_PROP_SET_UNSUPPORTED;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSamp2MemInputPin::QuerySupported(REFGUID guidPropSet,DWORD dwPropID,DWORD *pTypeSupport)
{
	SIMPLE_TRACE(_T("CSamp2MemInputPin::QuerySupported"));
	CString str = GetString(guidPropSet);
	SIMPLE_TRACE(str);
	SIMPLE_TRACE(_T("\n"));
	return E_PROP_SET_UNSUPPORTED;
}
