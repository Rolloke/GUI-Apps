/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicUnit
// FILE:		$Workfile: CVideoControl.h $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicUnit/CVideoControl.h $
// CHECKIN:		$Date: 24.06.02 13:52 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 24.06.02 13:22 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __CVIDEO_CONTROL_H__ 
#define __CVIDEO_CONTROL_H__

class CCodec;

class CVideoControl
{
public:
	CVideoControl(CCodec* pCodec, DWORD dwVideoRecheckTime);
	~CVideoControl();
	void  ResetVideoControl();
	BOOL  HasVideoStateChanged(WORD wSubID, BOOL bCurrentState);
	BOOL  TimeOutCheck(WORD wSubID);

private:
	CCodec*				m_pCodec;
					 
	// Videosignalüberwachung
	BOOL				m_bVideoState[MAX_CAMERAS];
	DWORD				m_dwTimeStamp[MAX_CAMERAS];
	DWORD				m_dwVideoRecheckTime;
	int					m_nNoVideoCounter;		// Anzahl der ausgefallenen Kameras
	CCriticalSection	m_csVCtrl;

};

#endif // __CVIDEO_CONTROL_H__
