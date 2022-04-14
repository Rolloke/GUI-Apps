//------------------------------------------------------------------------------
// File: Samp2MemInputPin.h
//

#ifndef __SAMP2MEMINPUTPIN_H__
#define __SAMP2MEMINPUTPIN_H__


class CSamp2MemInputPin :
	public CRendererInputPin,
	public IAMStreamConfig,
	public IOverlay,
	public IKsPropertySet
{
public:
	DECLARE_IUNKNOWN;

   CSamp2MemInputPin(CBaseRenderer *pRenderer, HRESULT *phr, LPCWSTR Name);

	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

// IAMStreamConfig interface
	STDMETHODIMP GetFormat(AM_MEDIA_TYPE **pmt);
	STDMETHODIMP GetNumberOfCapabilities(int *piCount, int *piSize);
	STDMETHODIMP GetStreamCaps(int iIndex, AM_MEDIA_TYPE **pmt, BYTE *pSCC);
	STDMETHODIMP SetFormat(AM_MEDIA_TYPE *pmt);
// IOverlay interface
	STDMETHODIMP GetPalette(DWORD *pdwColors, PALETTEENTRY **ppPalette);
	STDMETHODIMP SetPalette(DWORD dwColors, PALETTEENTRY *pPalette);
	STDMETHODIMP GetDefaultColorKey(COLORKEY *pColorKey);
	STDMETHODIMP GetColorKey(COLORKEY *pColorKey);
	STDMETHODIMP SetColorKey(COLORKEY *pColorKey);
	STDMETHODIMP GetWindowHandle(HWND *pHwnd);
	STDMETHODIMP GetClipList(RECT *pSourceRect,RECT *pDestinationRect, RGNDATA **ppRgnData);
	STDMETHODIMP GetVideoPosition(RECT *pSourceRect, RECT *pDestinationRect);
	STDMETHODIMP Advise(IOverlayNotify *pOverlayNotify, DWORD dwInterests);
	STDMETHODIMP Unadvise(void);
// IKsPropertySet interface
	STDMETHODIMP Set(REFGUID guidPropSet,DWORD dwPropID, LPVOID pInstanceData,DWORD cbInstanceData, LPVOID pPropData,DWORD cbPropData);
	STDMETHODIMP Get(REFGUID guidPropSet,DWORD dwPropID,LPVOID pInstanceData,DWORD cbInstanceData,LPVOID pPropData,DWORD cbPropData,DWORD *pcbReturned);
   STDMETHODIMP QuerySupported(REFGUID guidPropSet,DWORD dwPropID,DWORD *pTypeSupport);
};

#endif // __SAMP2MEMINPUTPIN_H__

