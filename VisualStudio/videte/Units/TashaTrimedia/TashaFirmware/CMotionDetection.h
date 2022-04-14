/////////////////////////////////////////////////////////////////////////////
// PROJECT:		Tashafirmare
// FILE:		$Workfile: CMotionDetection.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/CMotionDetection.h $
// CHECKIN:		$Date: 11.12.01 8:13 $
// VERSION:		$Revision: 6 $
// LAST CHANGE:	$Modtime: 11.12.01 8:13 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////
// Ausführungszeit auf Trimedia 1300 180MHz
// Scale: 720x288 -> 192x144 = 804 ys
// MD:	  192x144 -> 2,7ms
// ==>	  Summe: 3.5ms
///////////////////////////////////////////////
// Scale: 720x576 -> 384x288 = 3.03 ms
// MD:	  192x144 -> 10,68 ms
// ==>	  Summe: 13.7ms
///////////////////////////////////////////////

#define CAPTURE_SIZE_H	(768/4)		// Muß durch 8 teilbar sein
#define CAPTURE_SIZE_V	(576/4)		// Muß durch 8 teilbar sein

// Maximalwert der Helligkeit eines 8x8-Blockes
//#define UPPER_BOUNDARY_LUMI  8*8*442	// 442 = sqrt(3*255^2)
#define UPPER_BOUNDARY_LUMI  8*8*255
#define LOVER_BOUNDARY_LUMI  0

// Grenzwert der Maske
#define UPPER_BOUNDARY_MASK	 1000
#define LOWER_BOUNDARY_MASK	 0

typedef struct
{
	int nAdaptiveMask[CAPTURE_SIZE_H/8][CAPTURE_SIZE_V/8];
	int nPermanentMask[CAPTURE_SIZE_H/8][CAPTURE_SIZE_V/8];
}Mask;

typedef enum eMDLevel
{
	CSD_OFF		= 1,
	CSD_LOW		= 2,
	CSD_NORMAL	= 3,
	CSD_HIGH	= 4
}eMDLevel;

class CMDPoints;
class CPerfMon;
class CMotionDetection  
{			  

public:
	CMotionDetection();
	virtual ~CMotionDetection();

	virtual void ClearPermanentMask(WORD wSubID);
	virtual void ClearAdaptiveMask(WORD sUbID);
	virtual void DeactivateHistogramm(WORD wSubID);
	virtual void ActivateHistogramm(WORD wSubID);
	virtual void ActivateFilter(WORD wSubID);
	virtual void DeactivateFilter(WORD wSubID);
	virtual BOOL MotionCheck(WORD wSource, const exVideoBuffer* pVBuffer, CMDPoints& Points);
	virtual BOOL CaptureFrame(WORD wSubID, BYTE* pDest, const exVideoBuffer* pVBuffer);

	virtual void ClearMask(WORD wSubID);
	virtual void InvertPermanentMask(WORD wSubID);
	virtual void InvertAdaptiveMask(WORD wSubID);
	virtual BOOL SaveMask(WORD wSubID) const;
	virtual BOOL LoadMask(WORD wSubID);
	virtual void LoadParameter(WORD wSubID);
	virtual void SetPixelInMask(WORD wSubID, int nX, int nY, BOOL bFlag);
	virtual void ToggleShowMask();
	virtual void ToggleAdaptiveMaskActivation(WORD wSubID);
	virtual void ToggleMDActivation(WORD wSubID);
	virtual void ToggleHistogramm(WORD wSubID);
	virtual void ToggleFilter(WORD wSubID);
	virtual void DeactivateAdaptiveMask(WORD wSubID);
	virtual void ActivateAdaptiveMask(WORD wSubID);

	virtual BOOL GetAdaptiveMaskStatus(WORD wSubID) const {return m_bUseAdaptiveMask[wSubID];}
	virtual BOOL GetActivationMDStatus(WORD wSubID) const {return m_bActivateMD[wSubID];}
	virtual BOOL GetHistogrammStatus(WORD wSubID) const {return m_bHistogramm[wSubID];}
	virtual BOOL GetFilterStatus(WORD wSubID) const {return m_bFilter[wSubID];}
	virtual void ActivateMotionDetection(WORD wSource);
	virtual void DeactivateMotionDetection(WORD wSource);
	virtual void SetMDMaskSensitivity(WORD wSource, eMDLevel Level);
	virtual void SetMDAlarmSensitivity(WORD wSource, eMDLevel Level);
	virtual eMDLevel GetMDMaskSensitivity(WORD wSource) const;
	virtual eMDLevel GetMDAlarmSensitivity(WORD wSource) const;

protected:
	virtual BOOL CompareImages(WORD wSubID, CMDPoints& Points);
	virtual void ShowOriginalFrame(WORD wSubID) const;
	virtual void ShowDifferenceFrame(WORD wSubID) const;
	virtual BOOL TPFilter(const exVideoBuffer* pVBufferOut, const exVideoBuffer* pVBufferIn);


private:
	BOOL IsValid() const;

	int		m_nMinTime[MAX_CAMERAS];						// Mindestzeit in ms 
	int		m_nMaxTime[MAX_CAMERAS];						// Maximalzeit in ms 
	int		m_nMaskInc[MAX_CAMERAS];						// AufbauIncrement
	int		m_nMaskDec[MAX_CAMERAS];						// AbbauDecrement
	int		m_nUpperMaskTreshHold[MAX_CAMERAS];				// Oberer Masken-Schwellwert
	int		m_nLowerMaskTreshHold[MAX_CAMERAS];				// Unterer Masken-Schwellwert
	int		m_nAlarmTreshHold[MAX_CAMERAS];					// Alarm-Schwellwert

	BOOL	m_bActivateMD[MAX_CAMERAS];						// TRUE -> MD aktiviert
	BOOL	m_bUseAdaptiveMask[MAX_CAMERAS];				// TRUE -> Adaptive Maske ein
	BOOL	m_bHistogramm[MAX_CAMERAS];					    // TRUE -> Histogrammausgleich ein
	BOOL	m_bFilter[MAX_CAMERAS];							// TRUE -> Filter ein
	
	DWORD	m_dwTimeStamp[MAX_CAMERAS];						// Zeitpunkt des letzten Grabs
	BYTE*	m_pFrameBuffers[MAX_CAMERAS][2];				// Framebuffeer
	WORD	m_wField[MAX_CAMERAS];							// Indiziert 1. und 2. Grab
	BOOL	m_bValidFrame[MAX_CAMERAS][2];					// TRUE->Bild gültig
	int		m_nDiffs[CAPTURE_SIZE_H/8][CAPTURE_SIZE_V/8];	// 8x8-Differenzbild
	Mask	m_Mask[MAX_CAMERAS];							// Masken
	
	const exVideoBuffer*	m_pVBuffer;									// Pointer auf die videodaten
	
	BOOL		m_bShowMask;				// TRUE-> Maske im Bild
	BOOL		m_bShowOriginal;
	BOOL		m_bShowDifference;

	CPerfMon*	m_pPerf1;
	CPerfMon*	m_pPerf2;
	CPerfMon*	m_pPerf3;
};
