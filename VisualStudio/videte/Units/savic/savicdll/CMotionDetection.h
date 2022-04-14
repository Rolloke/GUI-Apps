/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: CMotionDetection.h $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicDll/CMotionDetection.h $
// CHECKIN:		$Date: 2.12.02 12:37 $
// VERSION:		$Revision: 16 $
// LAST CHANGE:	$Modtime: 2.12.02 12:29 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <ddraw.h>

#if !defined(AFX_CMOTIONDETECTION_H__95831A43_8CC8_11D1_BC01_00A024D29804__INCLUDED_)
#define AFX_CMOTIONDETECTION_H__95831A43_8CC8_11D1_BC01_00A024D29804__INCLUDED_

#if _MSC_VER >= 1000
#pragma once		    
#endif // _MSC_VER >= 1000

#include "CVideoInlay.h"

// Maximalwert der Helligkeit eines 8x8-Blockes
#define UPPER_BOUNDARY_LUMI  8*8*256	// 256 = 2^8
#define LOVER_BOUNDARY_LUMI  0

// Grenzwert der Maske
#define UPPER_BOUNDARY_MASK	 1000
#define LOWER_BOUNDARY_MASK	 0

typedef struct
{
	int** nAdaptiveMask;
	int** nPermanentMask;
}Mask;

class CCodec;
class CBT878;
class CDirectDraw;
class CMDConfigDlgBase;
class CMDPoints;

#include "IPCActivityMask.h"	// Added by ClassView

class CMotionDetection  
{			  
public:
	CMotionDetection(CBT878 *pBT878, const CString &sIniName);

	virtual void ClearPermanentMask(WORD wSubID);
	virtual void ClearAdaptiveMask(WORD sUbID);
	virtual void DeactivateHistogramm(WORD wSubID);
	virtual void ActivateHistogramm(WORD wSubID);
	virtual void ActivateFilter(WORD wSubID);
	virtual void DeactivateFilter(WORD wSubID);
	virtual ~CMotionDetection();
	virtual BOOL Init(CCodec* pCodec);
	virtual BOOL MotionCheck(CAPTURE_STRUCT& Capture, CMDPoints &Points);
	virtual void ClearMask(WORD wSubID);
	virtual void InvertPermanentMask(WORD wSubID);
	virtual void InvertAdaptiveMask(WORD wSubID);
	virtual BOOL SaveMask(WORD wSubID, MaskType eMaskType);
	virtual BOOL LoadMask(WORD wSubID);
	virtual BOOL LoadMaskX(WORD wSubID);

	virtual BOOL GetMask(WORD wSubID, CIPCActivityMask& mask);
	virtual BOOL SetMask(WORD wSubID, const CIPCActivityMask& mask);

	virtual BOOL OpenConfigDlg(WORD wSubID);
	virtual void LoadParameter(WORD wSubID);
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
	virtual BOOL ActivateMotionDetection(WORD wSource);
	virtual BOOL DeactivateMotionDetection(WORD wSource);
	virtual BOOL SetMDMaskSensitivity(WORD wSource, const CString &sLevel);
	virtual BOOL SetMDAlarmSensitivity(WORD wSource, const CString &sLevel);
	virtual CString GetMDMaskSensitivity(WORD wSource);
	virtual CString GetMDAlarmSensitivity(WORD wSource);
	virtual BOOL EnableDIBIndication(WORD wSubID);
	virtual BOOL DisableDIBIndication(WORD wSubID);
	
protected:
	virtual BOOL CompareImages(WORD wSubID, CMDPoints &Points);
	virtual void ShowOriginalFrame(WORD wSubID);
	virtual void ShowDifferenceFrame(WORD wSubID);
	virtual void SetPixel(int nID, int nX, int nY, COLORREF col);

	// Randbedingung: siSrc muß ein geradzahlig vielfaches von siDst sein!
	BOOL ScaleDownYUV422(CAPTURE_STRUCT& Capture, Y8* pFrame);

	virtual int CheckIf15Or16BitsPerPixel(int nBitsPerPixel);
	inline  int Betrag(int nY1, int nY2);

	virtual BOOL Histogramm(Y8* pSrc);
	virtual BOOL Filter(Y8* pSrc);
	virtual void FastZeroMem(void* pDestination, DWORD dwLength);
	virtual HANDLE CreateDIB(WORD wSubID);

	void CreateRiscPrg(const CRect& rc);
	BOOL EnableRisc();
	BOOL DisableRisc();

private:
	BOOL IsValid() const;
	// Das GDI unterscheidet nicht zwischen 15Bit und 16Bit, es liefert
	// immer 16Bit zurück. Deshalb muß dies extra untersucht werden.

	CBT878*		m_pBT878;
	CCodec*		m_pCodec;
	int			m_nHStart;
	int			m_nHEnd;
	int			m_nVStart;
	int			m_nVEnd;
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
	BOOL		m_bDIBIndication[MAX_CAMERAS];					// TRUE -> DIB wird verschickt.
	BOOL		m_bOK;
		  
	DWORD		m_dwTimeStamp[MAX_CAMERAS];						// Zeitpunkt des letzten Grabs
	Y8*			m_pFrameBuffers[MAX_CAMERAS][2];				// Framebuffeer
	int			m_wField[MAX_CAMERAS];							// Indiziert 1. und 2. Grab
	BOOL		m_bValidFrame[MAX_CAMERAS][2];					// TRUE->Bild gültig

	int**		m_nDiffs;										// 8x8-Differenzbild
	Mask		m_Mask[MAX_CAMERAS];							// Masken
	int			m_nBetrag[2*256];								// Absolutwerte.
	int*		m_pBetrag;										// Pointer auf Tabelle
	Y8*			m_pFilterFrameBuffer;							// TempBuffer zur Filterung;

	BOOL		m_bShowMask;									// TRUE-> Maske im Bild
	CString		m_sIniName;
	
	CWindowDC*	m_pDCInlayWnd;
	CWindowDC*	m_pDCDiffWnd;
	CMDConfigDlgBase* m_pMDConfigDlg;
	CCriticalSection	m_csMD;					
	CString		m_sMaskPath ;

	// Quellbild und Differenzbild zur Anzeige im MDConfigDialog.
	BITMAPINFO				m_bmiInlay;
    BYTE*					m_pDibImageDataInlay;
	BITMAPINFO				m_bmiDiff;
    BYTE*					m_pDibImageDataDiff;

	// 'Division mit neun' Array für das Tiefpassfilter
	BYTE   m_byDiv9[9*256];

	CWK_PerfMon* m_pPerf1;
	CWK_PerfMon* m_pPerf2;

	int m_nCaptureSizeH;		// Außmaße de Videoinlays
	int m_nCaptureSizeV;		//	"
	int	m_nCaptureBitsPerPixel;
	int m_nCaptureBytesPerPixel;
	int m_nCaptureBytesPerLine;

	int m_nScaledDownSizeH;		// Ausmaße des runterskallierten Bildes 
	int m_nScaledDownSizeV;		// für die MD
};

#endif // !defined(AFX_CMOTIONDETECTION_H__95831A43_8CC8_11D1_BC01_00A024D29804__INCLUDED_)
