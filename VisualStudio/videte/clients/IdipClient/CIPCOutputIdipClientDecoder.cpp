/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCOutputIdipClientDecoder.cpp $
// ARCHIVE:		$Archive: /Project/Clients/IdipClient/CIPCOutputIdipClientDecoder.cpp $
// CHECKIN:		$Date: 19.07.05 12:30 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 19.07.05 12:11 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"

#include "IdipClient.h"

#include "CIPCOutputIdipClientDecoder.h"

#include "Server.h"
#include "CIPCOutputIdipClient.h"
#include "CIPCDatabaseIdipClient.h"
#include "IdipClientDoc.h"
#include "ViewIdipClient.h"
#include "WndLive.h"
#include "mainfrm.h"


//////////////////////////////////////////////////////////////////////////////////////
CIPCOutputIdipClientDecoder::CIPCOutputIdipClientDecoder(LPCTSTR shmName, CIdipClientDoc* pDoc)
	: CIPCOutput(shmName, FALSE)
{
	m_pDoc = pDoc;
//	m_crKeyColor = SKIN_COLOR_KEY;
	StartThread();
}
//////////////////////////////////////////////////////////////////////////////////////
CIPCOutputIdipClientDecoder::~CIPCOutputIdipClientDecoder()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputIdipClientDecoder::OnReadChannelFound()
{
	DoRequestConnection();
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputIdipClientDecoder::OnConfirmConnection()
{
	if (((CMainFrame*)theApp.m_pMainWnd)->IsActive())
	{
		DoIndicateClientActive(TRUE,SECID_NO_GROUPID);
	}
	DoRequestInfoOutputs(SECID_NO_GROUPID);	// all groups
	AfxGetMainWnd()->PostMessage(WM_USER);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputIdipClientDecoder::OnConfirmInfoOutputs(WORD wGroupID, int iNumGroups, int numRecords, const CIPCOutputRecord records[])
{
	theApp.EndResetting();
	int i,c;
	c = numRecords;

	for (i=0;i<c;i++)
	{
		if (records[i].IsCamera())
		{
			if (records[i].CameraHasColor() && !records[i].CameraDoesJpeg())
			{
				WK_TRACE(_T("Request %s %lx\n"),CSD_MAX_OVERLAY_RECT,records[i].GetID().GetID());
				DoRequestGetValue(records[i].GetID(),CSD_MAX_OVERLAY_RECT);
				DoRequestGetValue(records[i].GetID(),CSD_COLORKEY);
				return;
			}
		}
	}
	WK_TRACE(_T("Request %s no GroupID %d\n"),CSD_MAX_OVERLAY_RECT,c);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputIdipClientDecoder::OnRequestDisconnect()
{
	AfxGetMainWnd()->PostMessage(WM_USER);
	DelayedDelete();
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputIdipClientDecoder::OnConfirmGetValue(CSecID id,const CString &sKey,const CString &sValue,DWORD dwServerData)
{
	if (sKey==CSD_MAX_OVERLAY_RECT)
	{
		m_sMaxRect = sValue;
		theApp.m_MaxMegraRect = StringToRect(m_sMaxRect);
	}
/*
	if (sKey==CSD_COLORKEY)
	{
		DWORD dw;
		if (1 == _stscanf((LPCTSTR)sValue, _T("%08lx"), &dw))
		{
			m_crKeyColor = (COLORREF)dw;
		}
		AfxGetMainWnd()->PostMessage(WM_USER);
	}
*/
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputIdipClientDecoder::OnConfirmMpegDecoding(WORD wGroupID, DWORD dwUserData)
{
}
