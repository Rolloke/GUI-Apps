/////////////////////////////////////////////////////////////////////////////
// PROJECT:		USBCamUnit
// FILE:		$Workfile: cmotiondetection.h $
// ARCHIVE:		$Archive: /Project/Units/USBCAM/MotionDetection/cmotiondetection.h $
// CHECKIN:		$Date: 26.08.03 16:00 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 26.08.03 15:56 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <ddraw.h>
#include "cipcstringdefs.h"

#if !defined(AFX_CMOTIONDETECTION_H__95831A43_8CC8_11D1_BC01_00A024D29804__INCLUDED_)
#define AFX_CMOTIONDETECTION_H__95831A43_8CC8_11D1_BC01_00A024D29804__INCLUDED_

#if _MSC_VER >= 1000
#pragma once		    
#endif // _MSC_VER >= 1000

// Maximalwert der Helligkeit eines 8x8-Blockes
#define UPPER_BOUNDARY_LUMI  8*8*256	// 256 = 2^8
#define LOVER_BOUNDARY_LUMI  0

// Grenzwert der Maske
#define UPPER_BOUNDARY_MASK	 1000
#define LOWER_BOUNDARY_MASK	 0

#define MAX_CAMERAS	8

typedef struct
{
//	HDEVICE	hDevice;			// Identifiziert das Device
	DWORD	dwProzessID;		// ProzessID
	DWORD	dwPics;				// Anzahl der gewünschten Bilder
	WORD	wSource;			// Videoquelle (VIDEO_SOURCE0...VIDEO_SOURCE7)
	WORD	wPortYC;			// Physikalischer Kameraeingang	(Composite-Signal)
	WORD	wFormat;			// Bildformat (ENCODER_RES_HIGH, ENCODER_RES_LOW)
	WORD	wBrightness;		// Helligkeit
	WORD	wContrast;			// Kontrast
	WORD	wSaturation;		// Farbsättigung
	WORD	wHue;				// Hue
	DWORD	dwBPF;				// BytesPro Field
	BOOL	bDoubleField;		// TRUE -> Vollbild (2Fields)
	WORD	wCompressionType;	// Art des gewünschten Bildes (Jpeg, DIB,...)
	BOOL	bSwitch;		
}SOURCE_SELECT_STRUCT;

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
	PVOID pPhysAddr;	// Physikalische Adresse
	PVOID pLinAddr;		// Lineare Adresse
	DWORD dwLen;
} MEMSTRUCT;

typedef struct		
{
	BOOL	bValid;		// TRUE: Zeitangabe ist gültig, ansonsten FALSE
	WORD	wYear;
	WORD	wMonth;
	WORD	wDay;
	WORD	wHour;
	WORD	wMinute;
	WORD	wSecond;
	WORD	wMSecond;
}TIME_STRUCT;

typedef struct		
{
	DWORD				hDevice;			// Identifiziert das Device
	DWORD				dwProzessID;		// Eindeutige ProzeßID
	WORD				wSource;			// Videoquelle zu den die Bilddaten gehören
	TIME_STRUCT			TimeStamp;			// TimeStamp
	DWORD				dwFieldID;			// FieldCounter
	WORD				wField;				// 0->Even Field 1->Odd Field
	BYTE*				pImageData;			// Adresse der Bilddaten
	DWORD				dwImageLen;			// Länge der Bilddaten.
	BYTE*				pHeaderData;		// Adresse des Headers
	DWORD				dwHeaderLen;		// Länge des Headers.
	WORD				wSizeH;				// Breite des Bildes
	WORD				wSizeV;				// Höhe des Bildes
	WORD				wCompressionType;	// Art des gewünschten Bildes (Jpeg, DIB,...)
	BOOL				bValid;				// TRUE -> Bildaten können abgeholt werden
} IMAGE;

typedef struct		
{
//	DWORD				hDevice;			// Identifiziert das Device
//	DWORD				dwProzessID;		// Eindeutige ProzeßID
	WORD				wSource;			// Videoquelle zu den die Bilddaten gehören
	MEMSTRUCT			Buffer;				// Buffer für die Bilddaten
//	PVOID				pMdl;				// MDL für den Zugriff aus dem Usermode.
//	PVOID				pUserBuffer;		// Usermode-Zeiger auf die Bilddaten,
	DWORD				dwLen;				// Länge der Bilddaten.
	WORD				wSizeH;				// Breite des Bildes
	WORD				wSizeV;				// Höhe des Bildes
//	WORD				wSourceType;		// Art des Quellbildes (Derzeit nur YUV422)
//	TIME_STRUCT			TimeStamp;			// TimeStamp
//	DWORD				dwFieldID;			// FieldCounter
//	WORD				wField;				// 0->Even Field 1->Odd Field
	WORD				wCompressionType;	// Art des gewünschten Bildes (Jpeg, DIB,...)
	BOOL				bValid;				// TRUE -> Bildaten können abgeholt werden
} CAPTURE_STRUCT;

typedef struct
{
	int** nAdaptiveMask;
	int** nPermanentMask;
}Mask;


#define MIN_BRIGHTNESS				0
#define MAX_BRIGHTNESS				255

// Wertebereich des Kontrastes
#define MIN_CONTRAST				0
#define MAX_CONTRAST				511

// Wertebereich des Farbsättigung
#define MIN_SATURATION				0
#define MAX_SATURATION				511

// Mögliche Bildformate
#define ENCODER_RES_LOW				0
#define ENCODER_RES_HIGH			1
#define ENCODER_RES_UNKNOWN			0xffff

// Encoder und Decoder Status
#define ENCODER_ON					1
#define ENCODER_OFF					2

// Capturesize für MotionDetection
#define CAPTURE_SIZE_H			(768/4)
#define CAPTURE_SIZE_V			(576/4)


#define MD_POINTS_RECEIVED	TRUE
#define REQUEST_TOO_EARLY  2
#define PICTURE_TOO_OLD    3
#define SCALE_DOWN_ERROR   4

class CDirectDraw;
class CMDConfigDlgBase;
class CMDPoints;

#include "IPCActivityMask.h"	// Added by ClassView

class AFX_EXT_CLASS CMotionDetection  
{			  
public:
	CMotionDetection(const CString &sIniName, CSize&, LPCTSTR);

	void ClearPermanentMask(WORD wSubID);
	void ClearAdaptiveMask(WORD sUbID);
	void DeactivateHistogramm(WORD wSubID);
	void ActivateHistogramm(WORD wSubID);
	void ActivateFilter(WORD wSubID);
	void DeactivateFilter(WORD wSubID);
	~CMotionDetection();
	BOOL Init();
	BOOL MotionCheck(CAPTURE_STRUCT& Capture, CMDPoints &Points);
	void ClearMask(WORD wSubID);
	void InvertPermanentMask(WORD wSubID);
	void InvertAdaptiveMask(WORD wSubID);
	BOOL SaveMask(WORD wSubID, MaskType eMaskType);
	BOOL LoadMask(WORD wSubID);
	BOOL LoadMaskX(WORD wSubID);

	BOOL GetMask(WORD wSubID, CIPCActivityMask& mask);
	BOOL SetMask(WORD wSubID, const CIPCActivityMask& mask);

	BOOL OpenConfigDlg(WORD wSubID);
	void LoadParameter(WORD wSubID);
	void SetPixelInMask(WORD wSubID, int nX, int nY, BOOL bFlag);
	void ToggleShowMask();
	void ToggleAdaptiveMaskActivation(WORD wSubID);
	void ToggleMDActivation(WORD wSubID);
	void ToggleHistogramm(WORD wSubID);
	void ToggleFilter(WORD wSubID);
	void DeactivateAdaptiveMask(WORD wSubID);
	void ActivateAdaptiveMask(WORD wSubID);

	BOOL GetAdaptiveMaskStatus(WORD wSubID){return m_bUseAdaptiveMask[wSubID];}
	BOOL GetActivationMDStatus(WORD wSubID){return m_bActivateMD[wSubID];}
	BOOL GetHistogrammStatus(WORD wSubID){return m_bHistogramm[wSubID];}
	BOOL GetFilterStatus(WORD wSubID){return m_bFilter[wSubID];}
	BOOL ActivateMotionDetection(WORD wSource);
	BOOL DeactivateMotionDetection(WORD wSource);
	BOOL SetMDMaskSensitivity(WORD wSource, const CString &sLevel);
	BOOL SetMDAlarmSensitivity(WORD wSource, const CString &sLevel);
	CString GetMDMaskSensitivity(WORD wSource);
	CString GetMDAlarmSensitivity(WORD wSource);
	BOOL EnableDIBIndication(WORD wSubID);
	BOOL DisableDIBIndication(WORD wSubID);
	
protected:
	BOOL CompareImages(WORD wSubID, CMDPoints &Points);
	void ShowOriginalFrame(WORD wSubID);
	void ShowDifferenceFrame(WORD wSubID);
	void SetPixel(int nID, int nX, int nY, COLORREF col);

	// Randbedingung: siSrc muß ein geradzahlig vielfaches von siDst sein!
	BOOL ScaleDownYUV422(CAPTURE_STRUCT& Capture, Y8* pFrame);

	int CheckIf15Or16BitsPerPixel(int nBitsPerPixel);
	inline  int Betrag(int nY1, int nY2);

	BOOL Histogramm(Y8* pSrc);
	BOOL Filter(Y8* pSrc);
	void FastZeroMem(void* pDestination, DWORD dwLength);
	HANDLE CreateDIB(WORD wSubID);

private:
	BOOL IsValid() const;
	// Das GDI unterscheidet nicht zwischen 15Bit und 16Bit, es liefert
	// immer 16Bit zurück. Deshalb muß dies extra untersucht werden.

	int			m_nHStart;
	int			m_nHEnd;
	int			m_nVStart;
	int			m_nVEnd;
	int			m_nMinTime[MAX_CAMERAS];						// Mindestzeit in ms 
	int			m_nMaxTime[MAX_CAMERAS];						// Maximalzeit in ms 
	int			m_nMaskInc[MAX_CAMERAS];						// AufbauIncrement
	int			m_nMaskDec[MAX_CAMERAS];						// AbbauDecrement
	int			m_nUpperMaskTreshHold[MAX_CAMERAS];			// Oberer Masken-Schwellwert
	int			m_nLowerMaskTreshHold[MAX_CAMERAS];			// Unterer Masken-Schwellwert
	int			m_nAlarmTreshHold[MAX_CAMERAS];				// Alarm-Schwellwert

	BOOL		m_bActivateMD[MAX_CAMERAS];						// TRUE -> MD aktiviert
	BOOL		m_bUseAdaptiveMask[MAX_CAMERAS];					// TRUE -> Adaptive Maske ein
	BOOL		m_bHistogramm[MAX_CAMERAS];					   // TRUE -> Histogrammausgleich ein
	BOOL		m_bFilter[MAX_CAMERAS];								// TRUE -> Filter ein
	BOOL		m_bDIBIndication[MAX_CAMERAS];					// TRUE -> DIB wird verschickt.
	BOOL		m_bOK;
		  
	DWORD		m_dwTimeStamp[MAX_CAMERAS];						// Zeitpunkt des letzten Grabs
	Y8*			m_pFrameBuffers[MAX_CAMERAS][2];				// Framebuffeer
	int			m_wField[MAX_CAMERAS];							// Indiziert 1. und 2. Grab
	BOOL		m_bValidFrame[MAX_CAMERAS][2];					// TRUE->Bild gültig

	int**		m_nDiffs;												// 8x8-Differenzbild
	Mask		m_Mask[MAX_CAMERAS];									// Masken
	int			m_nBetrag[2*256];									// Absolutwerte.
	int*		m_pBetrag;												// Pointer auf Tabelle
	Y8*			m_pFilterFrameBuffer;							// TempBuffer zur Filterung;

	BOOL		m_bShowMask;											// TRUE-> Maske im Bild
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

//	CWK_PerfMon* m_pPerf1;
//	CWK_PerfMon* m_pPerf2;

	int m_nCaptureSizeH;		// Außmaße de Videoinlays
	int m_nCaptureSizeV;		//	"
	int	m_nCaptureBitsPerPixel;
	int m_nCaptureBytesPerPixel;
	int m_nCaptureBytesPerLine;

	int m_nScaledDownSizeH;		// Ausmaße des runterskallierten Bildes 
	int m_nScaledDownSizeV;		// für die MD
};

#endif // !defined(AFX_CMOTIONDETECTION_H__95831A43_8CC8_11D1_BC01_00A024D29804__INCLUDED_)
