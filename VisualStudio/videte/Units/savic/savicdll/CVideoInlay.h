/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: cvideoinlay.h $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicDll/cvideoinlay.h $
// CHECKIN:		$Date: 27.08.02 11:06 $
// VERSION:		$Revision: 12 $
// LAST CHANGE:	$Modtime: 27.08.02 10:59 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <ddraw.h>

#if !defined(AFX_CVIDEOINLAY_H__95831A43_8CC8_11D1_BC01_00A024D29804__INCLUDED_)
#define AFX_CVIDEOINLAY_H__95831A43_8CC8_11D1_BC01_00A024D29804__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define DISPLAYTYPE_UNKNOWN -1
#define VIDEO_OVERLAY		1
#define VIDEO_INLAY			2
#define VIDEO_OFFSCREEN		3
#define VIDEO_PRIMARY		4

typedef struct
{
	int nHStart;
	int nHEnd;
	int nVStart;
	int nVEnd;

}  VideoTiming;

class CPCIDevice;
class CDirectDraw;
class CBT878;
class CCodec;

class CVideoInlay  
{
public:
	CVideoInlay(CBT878* pBT878);
	virtual ~CVideoInlay();
	virtual BOOL Init(CCodec* pCodec);
	// Initialisiert das VideoInlay.
	// VidTiming = Struktur, die einige notwendige Timingparameter enthält.

	virtual BOOL SetOutputWindow(const CRect &rc);
	// Setzt die Größe und die Position des Videofensters.

	virtual BOOL Activate();
	// Schaltet das VideoInlay ein

	virtual BOOL Deactivate();
	// Schaltet das VideoInlay aus

private:
	BOOL IsValid() const;
	void CheckIf15Or16BitsPerPixel(int& nBitsPerPixel) const;
	// Das GDI unterscheidet nicht zwischen 15Bit und 16Bit, es liefert
	// immer 16Bit zurück. Deshalb muß dies extra untersucht werden.

	BOOL EnableRisc();
	BOOL DisableRisc();

	void CreateRiscPrg(const CRect& rc);
	void SetPllFreq(unsigned int fin, unsigned int fout);
	
	CBT878*	m_pBT878;

	DWORD		m_dwScreenBaseAddr;
	DWORD		m_dwBytesPerLine;
	DWORD		m_dwBytesPerPixel;
	int			m_nBitsPerPixel;
	int			m_nHRes;
	int			m_nVRes;
	int			m_nPlanes;

	int			m_nCaptureSizeH;		// Außmaße de Videoinlays
	int			m_nCaptureSizeV;		//	"

	BOOL		m_bOK;
	WORD		m_wOutputFormat;
	WORD		m_wRBits;
	WORD		m_wGBits;
	WORD		m_wBBits;
	CDirectDraw*	m_pDirectDraw;
	int				m_nDisplayType;

	DWORD*		m_pRiscProgram;
	HDEVICE		m_hDevice;

	WORD		m_wPllFreq;
};

#endif // !defined(AFX_CVIDEOINLAY_H__95831A43_8CC8_11D1_BC01_00A024D29804__INCLUDED_)
