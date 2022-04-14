/////////////////////////////////////////////////////////////////////////////
// PROJECT:		JaCobDll
// FILE:		$Workfile: cvideoin.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/cvideoin.h $
// CHECKIN:		$Date: 14.12.01 12:40 $
// VERSION:		$Revision: 7 $
// LAST CHANGE:	$Modtime: 14.12.01 12:39 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CVIDEOIN_H__
#define __CVIDEOIN_H__

#include "helper.h"	// Added by ClassView
#if defined(__cplusplus)
extern "C" {
#endif

class CVideoIn
{
public:
	CVideoIn(ULONG SemBufferMMReady);
	virtual ~CVideoIn();

	BOOL viOpenAPI();

	BOOL viYUVAPI(viYUVModes_t mode, Bool capField, exVideoBuffer* pVBuff, Int nStartX, Int nStartY);

	BOOL viStop();

	void SetAdapterType(tmVideoAnalogAdapter_t inAdapterType);
	void SetVideoStandard(tmVideoAnalogStandard_t videoStandard);

	BOOL SetContrast(WORD wSource, int nValue);
	BOOL SetBrightness(WORD wSource, int nValue);
	BOOL SetSaturation(WORD wSource, int nValue);

	BOOL GetContrast(WORD wSource, int& nValue);
	BOOL GetBrightness(WORD wSource, int& nValue);
	BOOL GetSaturation(WORD wSource, int& nValue);

	BOOL SetInputSource(const SOURCE_SELECTION& SrcSel);
	BOOL GetInputSource(UInt& nSource);
protected:
	static void viTestISR();
	static void _viTestISR(Pointer pThis);

private:
	static CVideoIn*		m_pThis;
	viInstanceSetup_t		m_viInstSup;
	tmVideoAnalogStandard_t m_videoStandard;
	tmVideoAnalogAdapter_t  m_inAdapterType;
	viYUVSetup_t			m_viYUVSup;
	BOOL					m_bInvertField;
	Int						m_nViNum;
	Int						m_nViInst;

	BOOL					m_bCaptureFrame;
	BOOL					m_bFirstField;
	exVideoBuffer*			m_genBuf;
	ULONG					m_SemBufferMMReady;
	COLOR_SETTINGS			m_ColorSettings[MAX_CAMERAS];
	WORD					m_wSource;
};


#if defined(__cplusplus)
}
#endif

#endif // __CVIDEOIN_H__
