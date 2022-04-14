/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicUnit
// FILE:		$Workfile: CVideoControl.cpp $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicUnit/CVideoControl.cpp $
// CHECKIN:		$Date: 20.06.03 13:15 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 20.06.03 12:20 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CVideoControl.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
CVideoControl::CVideoControl(CCodec* pCodec, DWORD dwVideoRecheckTime)
{
	m_pCodec		= pCodec;
	m_dwVideoRecheckTime = dwVideoRecheckTime;
	ResetVideoControl();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
CVideoControl::~CVideoControl()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CVideoControl::ResetVideoControl()
{
	m_csVCtrl.Lock();

	// Status aller Videoquellen OK
	for (WORD wI = 0; wI < MAX_CAMERAS; wI++)
	{
		m_bVideoState[wI] = TRUE;
		m_dwTimeStamp[wI] = GetTickCount();
	}

	m_csVCtrl.Unlock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CVideoControl::HasVideoStateChanged(WORD wSubID, BOOL bVideoState)
{
	m_csVCtrl.Lock();

	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR(_T("CVideoControl::HasVideoStateChanged\tWrong CamID\n"));
		m_csVCtrl.Unlock();
		return FALSE;
	}

	// Videostatus der Kamera geändert?
	if (bVideoState == m_bVideoState[wSubID])
	{
		m_csVCtrl.Unlock();
		return FALSE;
	}
	else
	{
		// Neuen Videostatus sichern
		m_bVideoState[wSubID] = bVideoState;
		m_dwTimeStamp[wSubID] = GetTickCount();
	}

	m_csVCtrl.Unlock();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////
BOOL CVideoControl::TimeOutCheck(WORD wSubID)
{
	m_csVCtrl.Lock();

	if (wSubID >= MAX_CAMERAS)
	{
		WK_TRACE_ERROR(_T("CVideoControl::GetTimeStamp\tWrong CamID\n"));
		m_csVCtrl.Unlock();
		return FALSE;
	}

	DWORD dwDelta = GetTimeSpan(m_dwTimeStamp[wSubID]);

	if (dwDelta > m_dwVideoRecheckTime)
		m_dwTimeStamp[wSubID] = GetTickCount();

	// Liefere TRUE, wenn das Kamerasignal ok, oder die Zeit abgelaufen ist
	BOOL bRet = (BOOL)(m_bVideoState[wSubID] || dwDelta > m_dwVideoRecheckTime);

	m_csVCtrl.Unlock();

	return bRet;
}
