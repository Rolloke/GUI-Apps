/////////////////////////////////////////////////////////////////////////////
// PROJECT:		QUnit
// FILE:		$Workfile: CMotionDetection.h $
// ARCHIVE:		$Archive: /Project/Units/QUnit/CMotionDetection.h $
// CHECKIN:		$Date: 1.12.05 16:09 $
// VERSION:		$Revision: 7 $
// LAST CHANGE:	$Modtime: 1.12.05 15:04 $
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

// Auflösung mit der die Bewegungserkennung arbeitet.
// Intern wird auf diese Auflösung skaliert
#define CAPTURE_SIZE_H (768/4)
#define CAPTURE_SIZE_V (576/4)

// Maximalwert der Helligkeit eines 8x8-Blockes
#define UPPER_BOUNDARY_LUMI  8*8*256	// 256 = 2^8
#define LOVER_BOUNDARY_LUMI  0

// Grenzwert der Maske
#define UPPER_BOUNDARY_MASK	 1000
#define LOWER_BOUNDARY_MASK	 0

#ifndef BGR24a
#define BGR24a				    
// 24Bit RGB
typedef struct
{
	BYTE bB;
	BYTE bG;
	BYTE bR;
}BGR24;
#endif

typedef struct
{
	BYTE bY1;
	BYTE bU;
	BYTE bY2;
	BYTE bV;
}YUV422;

typedef struct
{
	BYTE bY;
}Y8;

typedef struct
{
	int** nAdaptiveMask;
	int** nPermanentMask;
}Mask;

class CUDP;
class CDirectDraw;
class CMDConfigDlgBase;
class CMDPoints;

#include "IPCActivityMask.h"	// Added by ClassView

class CMotionDetection  
{			  
public:
	CMotionDetection(CUDP* pUDP, const CString &sIniName);

	virtual void ClearPermanentMask(int nCamera);
	virtual void ClearAdaptiveMask(int nCamera);
	virtual void DeactivateHistogramm(int nCamera);
	virtual void ActivateHistogramm(int nCamera);
	virtual void ActivateFilter(int nCamera);
	virtual void DeactivateFilter(int nCamera);
	virtual ~CMotionDetection();
	virtual BOOL Init();
	virtual BOOL MotionCheck(QIMAGE* pQImage);
	virtual void ClearMask(int nCamera);
	virtual void InvertPermanentMask(int nCamera);
	virtual void InvertAdaptiveMask(int nCamera);
	virtual BOOL SaveMask(int nCamera, MaskType eMaskType);
	virtual BOOL LoadMask(int nCamera);
	virtual BOOL LoadMaskX(int nCamera);

	virtual BOOL GetMask(int nCamera, CIPCActivityMask& mask);
	virtual BOOL SetMask(int nCamera, const CIPCActivityMask& mask);

	virtual BOOL OpenConfigDlg(HWND hWndParent);
	virtual void LoadParameter(int nCamera);
	virtual void SetPixelInMask(int nCamera, int nX, int nY, BOOL bFlag);
	virtual void ToggleShowMask();
	virtual void ToggleAdaptiveMaskActivation(int nCamera);
	virtual void ToggleMDActivation(int nCamera);
	virtual void ToggleHistogramm(int nCamera);
	virtual void ToggleFilter(int nCamera);
	virtual void DeactivateAdaptiveMask(int nCamera);
	virtual void ActivateAdaptiveMask(int nCamera);

	virtual BOOL GetAdaptiveMaskStatus(int nCamera){return m_bUseAdaptiveMask[nCamera];}
	virtual BOOL GetActivationMDStatus(int nCamera){return m_bActivateMD[nCamera];}
	virtual BOOL GetHistogrammStatus(int nCamera){return m_bHistogramm[nCamera];}
	virtual BOOL GetFilterStatus(int nCamera){return m_bFilter[nCamera];}
	virtual BOOL ActivateMotionDetection(int nCamera);
	virtual BOOL DeactivateMotionDetection(int nCamera);
	virtual BOOL SetMDMaskSensitivity(int nCamera, const CString &sLevel);
	virtual BOOL SetMDAlarmSensitivity(int nCamera, const CString &sLevel);
	virtual CString GetMDMaskSensitivity(int nCamera);
	virtual CString GetMDAlarmSensitivity(int nCamera);
	virtual BOOL EnableDIBIndication(int nCamera);
	virtual BOOL DisableDIBIndication(int nCamera);
	
protected:
	virtual BOOL CompareImages(int nCamera, CMDPoints* pPoints);
	virtual void ShowOriginalFrame(int nCamera);
	virtual void ShowDifferenceFrame(int nCamera);
	virtual void SetPixel(int nID, int nX, int nY, COLORREF col);

	// Randbedingung: siSrc muß ein geradzahlig vielfaches von siDst sein!
	BOOL ScaleDownYUV422(QIMAGE* pQImage, Y8* pFrame);

	virtual int CheckIf15Or16BitsPerPixel(int nBitsPerPixel);
	inline  int Betrag(int nY1, int nY2);

	virtual BOOL Histogramm(Y8* pSrc);
	virtual BOOL Filter(Y8* pSrc);
	virtual void FastZeroMem(void* pDestination, DWORD dwLength);
	virtual HANDLE CreateDIB(int nCamera);

private:
	BOOL IsValid() const;
	// Das GDI unterscheidet nicht zwischen 15Bit und 16Bit, es liefert
	// immer 16Bit zurück. Deshalb muß dies extra untersucht werden.
	
	CUDP*		m_pUDP;
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

	int m_nCaptureSizeH;		// Außmaße de Videoinlays
	int m_nCaptureSizeV;		//	"
	int	m_nCaptureBitsPerPixel;
	int m_nCaptureBytesPerPixel;
	int m_nCaptureBytesPerLine;

	int m_nScaledDownSizeH;		// Ausmaße des runterskallierten Bildes 
	int m_nScaledDownSizeV;		// für die MD
};

#endif // !defined(AFX_CMOTIONDETECTION_H__95831A43_8CC8_11D1_BC01_00A024D29804__INCLUDED_)
