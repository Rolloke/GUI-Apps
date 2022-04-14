/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCOutputVisionDecoder.cpp $
// ARCHIVE:		$Archive: /Project/Clients/Vision/CIPCOutputVisionDecoder.cpp $
// CHECKIN:		$Date: 25.02.04 12:31 $
// VERSION:		$Revision: 7 $
// LAST CHANGE:	$Modtime: 25.02.04 12:05 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$

#include "stdafx.h"

#include "Vision.h"

#include "CIPCOutputVisionDecoder.h"

#include "Server.h"
#include "CIPCOutputVision.h"
#include "CIPCDataBaseVision.h"
#include "VisionDoc.h"
#include "VisionView.h"
#include "DisplayWindow.h"
#include "mainfrm.h"


//////////////////////////////////////////////////////////////////////////////////////
CIPCOutputVisionDecoder::CIPCOutputVisionDecoder(LPCTSTR shmName, CVisionDoc* pDoc)
	: CIPCOutput(shmName, FALSE)
{
	m_pVisionDoc = pDoc;
	m_crKeyColor = RGB(255,0,255);
	StartThread();
}
//////////////////////////////////////////////////////////////////////////////////////
CIPCOutputVisionDecoder::~CIPCOutputVisionDecoder()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputVisionDecoder::OnReadChannelFound()
{
	DoRequestConnection();
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputVisionDecoder::OnConfirmConnection()
{
	if (((CMainFrame*)theApp.m_pMainWnd)->IsActive())
	{
		DoIndicateClientActive(TRUE,SECID_NO_GROUPID);
	}
	DoRequestInfoOutputs(SECID_NO_GROUPID);	// all groups
	AfxGetMainWnd()->PostMessage(WM_USER);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputVisionDecoder::OnConfirmInfoOutputs(WORD wGroupID, int iNumGroups, int numRecords, const CIPCOutputRecord records[])
{
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
void CIPCOutputVisionDecoder::OnRequestDisconnect()
{
	AfxGetMainWnd()->PostMessage(WM_USER);
	DelayedDelete();
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputVisionDecoder::OnConfirmGetValue(CSecID id,const CString &sKey,const CString &sValue,DWORD dwServerData)
{
	if (sKey==CSD_MAX_OVERLAY_RECT)
	{
		m_sMaxRect = sValue;
		theApp.m_MaxMegraRect = StringToRect(m_sMaxRect);
	}
	if (sKey==CSD_COLORKEY)
	{
		DWORD dw;
		if (1 == _stscanf((LPCTSTR)sValue, _T("%08lx"), &dw))
		{
			m_crKeyColor = (COLORREF)dw;
		}
		AfxGetMainWnd()->PostMessage(WM_USER);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputVisionDecoder::OnConfirmMpegDecoding(WORD wGroupID, DWORD dwUserData)
{
}
