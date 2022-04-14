/////////////////////////////////////////////////////////////////////////////
// PROJECT:		JaCobDll
// FILE:		$Workfile: CMotionDetection.h $
// ARCHIVE:		$Archive: /Project/Units/GenericUnit/CMotionDetection.h $
// CHECKIN:		$Date: 23.11.00 10:41 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 23.11.00 10:21 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <ddraw.h>

#if !defined(AFX_CMOTIONDETECTION_H__95831A43_8CC8_11D1_BC01_00A024D29804__INCLUDED_)
#define AFX_CMOTIONDETECTION_H__95831A43_8CC8_11D1_BC01_00A024D29804__INCLUDED_

#if _MSC_VER >= 1000
#pragma once		    
#endif // _MSC_VER >= 1000

//#include "CVideoInlay.h"

// Maximalwert der Helligkeit eines 8x8-Blockes
#define UPPER_BOUNDARY_LUMI  8*8*442	// 442 = sqrt(3*255^2)
#define LOVER_BOUNDARY_LUMI  0

// Grenzwert der Maske
#define UPPER_BOUNDARY_MASK	 1000
#define LOWER_BOUNDARY_MASK	 0

typedef struct
{
	int nAdaptiveMask[CAPTURE_SIZE_H/8][CAPTURE_SIZE_V/8];
	int nPermanentMask[CAPTURE_SIZE_H/8][CAPTURE_SIZE_V/8];
}Mask;

class CCodec;
class CMDConfigDlg;
class CDirectDraw;
class CJpeg;
class CMotionDetection  
{			  
public:
	virtual void ClearPermanentMask(WORD wSubID);
	virtual void ClearAdaptiveMask(WORD sUbID);
	virtual void DeactivateHistogramm(WORD wSubID);
	virtual void ActivateHistogramm(WORD wSubID);
	virtual void ActivateFilter(WORD wSubID);
	virtual void DeactivateFilter(WORD wSubID);
	CMotionDetection(const CString &sIniName);
	virtual ~CMotionDetection();
	virtual BOOL Init(CCodec* pCodec);
	virtual BOOL MotionCheck(WORD wSource, CPoint &Point);
	virtual void ClearMask(WORD wSubID);
	virtual void InvertPermanentMask(WORD wSubID);
	virtual void InvertAdaptiveMask(WORD wSubID);
	virtual BOOL SaveMask(WORD wSubID);
	virtual BOOL LoadMask(WORD wSubID);
	virtual void LoadParameter(WORD wSubID);
	virtual void OpenConfigDlg();
	virtual void SetPixelInMask(WORD wSubID, int nX, int nY, BOOL bFlag);
	virtual void ToggleShowMask();
	virtual void ToggleAdaptiveMaskActivation(WORD wSubID);
	virtual void ToggleMDActivation(WORD wSubID);
	virtual void ToggleHistogramm(WORD wSubID);
	virtual void ToggleFilter(WORD wSubID);
	virtual void DeactivateAdaptiveMask(WORD wSubID);
	virtual void ActivateAdaptiveMask(WORD wSubID);

	virtual BOOL GetAdaptiveMaskStatus(WORD wSubID){return m_bUseAdaptiveMask[wSubID];}
	virtual BOOL GetActivationMDStatus(WORD wSubID){return m_bActivateMD[wSubID];}
	virtual BOOL GetHistogrammStatus(WORD wSubID){return m_bHistogramm[wSubID];}
	virtual BOOL GetFilterStatus(WORD wSubID){return m_bFilter[wSubID];}
	virtual void ActivateMotionDetection(WORD wSource);
	virtual void DeactivateMotionDetection(WORD wSource);
	virtual void SetMDMaskSensitivity(WORD wSource, const CString &sLevel);
	virtual void SetMDAlarmSensitivity(WORD wSource, const CString &sLevel);
	virtual CString GetMDMaskSensitivity(WORD wSource);
	virtual CString GetMDAlarmSensitivity(WORD wSource);

protected:
	virtual BOOL CompareImages(WORD wSubID, CPoint &Point);
	virtual void ShowOriginalFrame(WORD wSubID);
	virtual void ShowDifferenceFrame(WORD wSubID);
	virtual void SetPixel(int nID, int nX, int nY, COLORREF col);
	virtual BOOL CaptureFrame(WORD wSubID, BGR24* pFrame);
	virtual int CheckIf15Or16BitsPerPixel(int nBitsPerPixel);
	inline  int Betrag(int nR, int nG, int nB);

private:
	BOOL DACaptureFrame(WORD wSubID, CAPTURE_STRUCT& Capture);
	BOOL ScaleDown(CAPTURE_STRUCT& Capture);

	BOOL IsValid() const;
	// Das GDI unterscheidet nicht zwischen 15Bit und 16Bit, es liefert
	// immer 16Bit zurück. Deshalb muß dies extra untersucht werden.

	int			m_nMinTime[MAX_CAMERAS];						// Mindestzeit in ms 
	int			m_nMaxTime[MAX_CAMERAS];						// Maximalzeit in ms 
	int			m_nMaskInc[MAX_CAMERAS];						// AufbauIncrement
	int			m_nMaskDec[MAX_CAMERAS];						// AbbauDecrement
	int			m_nUpperMaskTreshHold[MAX_CAMERAS];				// Oberer Masken-Schwellwert
	int			m_nLowerMaskTreshHold[MAX_CAMERAS];				// Unterer Masken-Schwellwert
	int			m_nAlarmTreshHold[MAX_CAMERAS];					// Alarm-Schwellwert

	BOOL		m_bActivateMD[MAX_CAMERAS];						// TRUE -> MD aktiviert
	BOOL		m_bUseAdaptiveMask[MAX_CAMERAS];				// TRUE -> Adaptive Maske ein
	BOOL		m_bHistogramm[MAX_CAMERAS];					    // TRUE -> Histogrammausgleich ein
	BOOL		m_bFilter[MAX_CAMERAS];							// TRUE -> Filter ein
	
	BOOL		m_bOK;
		  
	DWORD		m_dwTimeStamp[MAX_CAMERAS];						// Zeitpunkt des letzten Grabs
	BGR24*		m_pFrameBuffers[MAX_CAMERAS][2];				// Framebuffeer
	int			m_wField[MAX_CAMERAS];							// Indiziert 1. und 2. Grab
	BOOL		m_bValidFrame[MAX_CAMERAS][2];					// TRUE->Bild gültig
	int			m_nDiffs[CAPTURE_SIZE_H/8][CAPTURE_SIZE_V/8];	// 8x8-Differenzbild
	Mask		m_Mask[MAX_CAMERAS];							// Masken
	int			m_nQuad[2*256];									// Quadrate
	int			m_nSqrt[3*256*256];								// Wurzel
	int*		m_pQuad;										// Pointer auf Tabelle
	
	HDEVICE		m_hDevice;										// Devicehandle
	BOOL		m_bShowMask;									// TRUE-> Maske im Bild
	CString		m_sIniName;
	
	CWindowDC*	m_pDCInlayWnd;
	CWindowDC*	m_pDCDiffWnd;
	CMDConfigDlg* m_pMDConfigDlg;
	CCriticalSection	m_csMD;					
	CString		m_sMaskPath ;
	CCodec*		m_pCodec;
	CJpeg*		m_pCJpeg;

	CDirectDraw*	m_pDirectDraw;
	DWORD	m_dwScreenBaseAddr;	// Bildschirmspeicheradresse
	int		m_nHScreenRes;		// Horizontale Bildschirmauflösung
	int		m_nVScreenRes;		// Vertikale Bildschirmauflösung
	DWORD	m_dwBytesPerLine;	// Bytes pro Zeile
	DWORD	m_dwBytesPerPixel;	// Bytes pro Pixel
	int		m_nBitsPerPixel;	// Bits pro Pixel
	int		m_nPlanes;			// Anzahl der Planes
	int		m_nBitCount;		// Bits pro Pixel
	WORD	m_wRBits;			// Anzahl der Rot-Bits	
	WORD	m_wGBits;			// Anzahl der Grün-Bits
	WORD	m_wBBits;			// Anzahl der Blau-Bits
};

#endif // !defined(AFX_CMOTIONDETECTION_H__95831A43_8CC8_11D1_BC01_00A024D29804__INCLUDED_)
