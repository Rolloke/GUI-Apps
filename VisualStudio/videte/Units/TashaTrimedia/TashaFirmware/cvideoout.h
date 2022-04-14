/////////////////////////////////////////////////////////////////////////////
// PROJECT:		JaCobDll
// FILE:		$Workfile: cvideoout.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/cvideoout.h $
// CHECKIN:		$Date: 11.12.01 8:14 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 11.12.01 8:13 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CVIDEOOUT_H__
#define __CVIDEOOUT_H__

#if defined(__cplusplus)
extern "C" {
#endif

class CVideoOut
{
public:
	CVideoOut();	    
	virtual ~CVideoOut();

	BOOL voOpenAPI();
	BOOL voStop();

	BOOL voYUVAPI(voYUVModes_t mode, exVideoBuffer* pVBuff, Int imageVertOffset, Int imageHorzOffset);
	void SetAdapterType(tmVideoAnalogAdapter_t outAdapterType);

public:						 
	volatile Int			m_voISRCount;

protected:
	static void voTestISR();
	static void _voTestISR(Pointer pThis);

private:

	static	CVideoOut*		m_pThis;
	Int						m_nVoInst;
	voInstanceSetup_t		m_voInstSup;
	tmVideoAnalogStandard_t m_videoStandard;
	tmVideoAnalogAdapter_t  m_outAdapterType;

	voOverlaySetup_t		m_voOverlaySup;
	voYUVSetup_t			m_voYUVSup;
	exVideoBuffer*			m_genBuf;
	exVideoBuffer*			m_bkBuf;
	Int						m_nVoNum;


};

#if defined(__cplusplus)
}
#endif

#endif // __CVIDEOOUT_H__
