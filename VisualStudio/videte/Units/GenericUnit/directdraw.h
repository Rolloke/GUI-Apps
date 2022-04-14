/////////////////////////////////////////////////////////////////////////////
// PROJECT:		JaCobDll
// FILE:		$Workfile: directdraw.h $
// ARCHIVE:		$Archive: /Project/Units/GenericUnit/directdraw.h $
// CHECKIN:		$Date: 23.11.00 10:41 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 9.07.99 13:31 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRECTDRAW_H__E83CE110_E431_11D2_8F00_F80B7E039853__INCLUDED_)
#define AFX_DIRECTDRAW_H__E83CE110_E431_11D2_8F00_F80B7E039853__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <ddraw.h>

class CDirectDraw  
{
public:
	CDirectDraw(int nHSize, int nVSize);
	virtual ~CDirectDraw();

	virtual BOOL	Create();
	virtual BOOL	CreatePrimarySurface();
	virtual BOOL	CreateOverlaySurface(DWORD dwColorKey);
	virtual BOOL	CreateClipper(HWND hWnd);

	virtual BOOL	UpdateOverlay(const CRect &rc);

	virtual void*	LockPrimarySurface(RECT *pRect = NULL);
	virtual BOOL	UnlockPrimarySurface();
	virtual void*	LockOverlaySurface(RECT *pRect = NULL);
	virtual BOOL	UnlockOverlaySurface();
	virtual HRESULT CheckValue(HRESULT ddrval);
protected:
	
private:
	LPDIRECTDRAW			m_lpDD;
	LPDIRECTDRAWSURFACE		m_lpDDSPrimary;
	LPDIRECTDRAWSURFACE		m_lpDDSOverlay;
	LPDIRECTDRAWCLIPPER		m_lpDDSClipper;

	DWORD					m_dwColorKey;
	void*					m_pPrimarySurface;
	void*					m_pOverlaySurface;
	void*					m_pOffscreenSurface;
	DWORD					m_dwStretchFactor1000;
	DWORD					m_dwDestSizeAlign;
	DWORD					m_dwSrcSizeAlign;
	int						m_nHSize;
	int						m_nVSize;
};

#endif // !defined(AFX_DIRECTDRAW_H__E83CE110_E431_11D2_8F00_F80B7E039853__INCLUDED_)
