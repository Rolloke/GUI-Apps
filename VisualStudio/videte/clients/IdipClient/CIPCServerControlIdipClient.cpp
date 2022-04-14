/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCServerControlIdipClient.cpp $
// ARCHIVE:		$Archive: /Project/Clients/IdipClient/CIPCServerControlIdipClient.cpp $
// CHECKIN:		$Date: 5.01.06 12:18 $
// VERSION:		$Revision: 12 $
// LAST CHANGE:	$Modtime: 5.01.06 11:31 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"
#include "IdipClient.h"

#include "CIPCServerControlIdipClient.h"
#include "IdipClientDoc.h"
#include "oemgui\DlgLoginNew.h"

//////////////////////////////////////////////////////////////////////////////////////
CIPCServerControlIdipClient::CIPCServerControlIdipClient(LPCTSTR szShmName, CIdipClientDoc* pDoc)
	: CIPCServerControl(szShmName, TRUE)
{
	m_pDoc = pDoc;
	SetConnectRetryTime(100);
	StartThread();
}
//////////////////////////////////////////////////////////////////////////////////////
CIPCServerControlIdipClient::~CIPCServerControlIdipClient()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCServerControlIdipClient::OnRequestAlarmConnection(const CConnectionRecord &c)
{
	const CString sSourceHost = c.GetSourceHost();	// NEW HEDU
	CSecID	camID = c.GetCamID();

	// ConnectionDlg would hinder us at the connection
	theApp.GetMainWnd()->PostMessage(WM_USER, WPARAM_CLOSE_CONNECT_DLG, 0);
	Sleep(50);

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

	m_pDoc->AddConnectionRecord(c);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCServerControlIdipClient::OnRequestServerReset(const CConnectionRecord &c)
{
	DoConfirmServerReset();
	theApp.DoReset();
}
