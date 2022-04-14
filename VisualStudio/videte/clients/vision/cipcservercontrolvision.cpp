/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCServerControlVision.cpp $
// ARCHIVE:		$Archive: /Project/Clients/Vision/CIPCServerControlVision.cpp $
// CHECKIN:		$Date: 25.02.04 12:31 $
// VERSION:		$Revision: 21 $
// LAST CHANGE:	$Modtime: 25.02.04 11:26 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$

#include "stdafx.h"
#include "Vision.h"

#include "CIPCServerControlVision.h"
#include "VisionDoc.h"

//////////////////////////////////////////////////////////////////////////////////////
CIPCServerControlVision::CIPCServerControlVision(LPCTSTR shmName, CVisionDoc* pDoc)
	: CIPCServerControl(shmName,TRUE)
{
	m_pVisionDoc = pDoc;
	SetConnectRetryTime(100);
	StartThread();
}
//////////////////////////////////////////////////////////////////////////////////////
CIPCServerControlVision::~CIPCServerControlVision()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCServerControlVision::OnRequestAlarmConnection(const CConnectionRecord &c)
{
	const CString sSourceHost = c.GetSourceHost();	// NEW HEDU
	CSecID	camID = c.GetCamID();

	WK_TRACE(_T("Request Alarm Connection\n"));
	WK_TRACE(_T("source     = <%s>\n"),sSourceHost);
	WK_TRACE(_T("camera     = <%lx>\n"),camID.GetID());
	WK_TRACE(_T("user       = <%s>\n"),c.GetUser());
	WK_TRACE(_T("permission = <%s>\n"),c.GetPermission());
	WK_TRACE(_T("dest       = <%s>\n"),c.GetDestinationHost());
	WK_TRACE(_T("time       = <%d>\n"),c.GetTimeout());

	CStringArray sa;
	c.GetAllFields(sa);
	int i,d;

	d = sa.GetSize();
	WK_TRACE(_T("%d extra parameters\n"),d);
	for (i=0;i<d;i++)
	{
		if (-1 == sa[i].Find(CSD_PIN))
		{
			WK_TRACE(_T("param%d = <%s>\n"),i,sa[i]);
		}
	}

	m_pVisionDoc->CSConnectionDlgLockFunction(TRUE);
	COEMConnectionDialog* pDlg = m_pVisionDoc->GetConnectionDlg();
	if (pDlg)
	{
		pDlg->PostMessage(WM_COMMAND, MAKELONG(IDCANCEL,1), NULL);
	}
	m_pVisionDoc->CSConnectionDlgLockFunction(FALSE);
	m_pVisionDoc->AddConnectionRecord(c);
}
