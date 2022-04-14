// IPCServerControlDVClient.cpp: implementation of the CIPCServerControlDVClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dvclient.h"
#include "IPCServerControlDVClient.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCServerControlDVClient::CIPCServerControlDVClient(LPCTSTR shmName)
	: CIPCServerControl(shmName,TRUE)
{
	WK_TRACE(_T("starting CIPCServerControlDVClient\n"));
	SetConnectRetryTime(100);
	StartThread();
}
//////////////////////////////////////////////////////////////////////////////////////
CIPCServerControlDVClient::~CIPCServerControlDVClient()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCServerControlDVClient::OnRequestAlarmConnection(const CConnectionRecord &c)
{
	const CString sSourceHost = c.GetSourceHost();
	const CString &shmInputName = c.GetInputShm();
	const CString &shmOutputName = c.GetOutputShm();
	CSecID	camID = c.GetCamID();

	WK_TRACE(_T("Request Alarm Connection\n"));
	WK_TRACE(_T("source     = <%s>\n"),sSourceHost);
	WK_TRACE(_T("camera     = <%lx>\n"),camID.GetID());
	WK_TRACE(_T("dest       = <%s>\n"),c.GetDestinationHost());
	
	CStringArray sa;
	c.GetAllFields(sa);
	int i,d;

	d = sa.GetSize();
	for (i=0;i<d;i++)
	{
		if (-1 != sa[i].Find(CSD_PIN))
		{
			WK_TRACE(_T("param%d = <%s>\n"),i,sa[i]);
		}
		else
		{
			TRACE(_T("param%d = <%s>\n"),i,sa[i]);
		}
	}

	if (theApp.GetServer() == NULL)
	{
		if (!c.GetInputShm().IsEmpty() &&
			!c.GetOutputShm().IsEmpty())
		{
			WK_TRACE(_T("confirming Alarm connection\n"));
			DWORD dwOptions = SCO_WANT_RELAYS | SCO_WANT_CAMERAS_COLOR|SCO_IS_DV;
			if (theApp.GetIntValue(_T("CanGOP"), 1))
			{
				dwOptions |= SCO_CAN_GOP;
			}
			DoConfirmAlarmConnection(shmInputName, shmOutputName, _T(""), _T(""), dwOptions
#ifdef _UNICODE
				, MAKELONG(CODEPAGE_UNICODE, 0)
#endif				
				);
		}
		
		theApp.DoAlarmConnection(c);
	}
	else
	{
		WK_TRACE(_T("rejecting Alarm connection\n"));
		DoIndicateError(SRV_CONTROL_REQUEST_ALARM_CONNECTION,
						SECID_NO_ID, 
						CIPC_ERROR_UNABLE_TO_CONNECT,
						0);
	}
}
