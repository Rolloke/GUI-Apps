// IPCGemosToVision.cpp: implementation of the CIPCGemosToVision class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GemosUnit.h"
#include "IPCGemosToVision.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIPCGemosToVision::CIPCGemosToVision(LPCTSTR szShmName)
	: CIPC(szShmName, TRUE)
{
	StartThread();
}
//////////////////////////////////////////////////////////////////////
CIPCGemosToVision::~CIPCGemosToVision()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////
void CIPCGemosToVision::OnRequestDisconnect()
{
	DelayedDelete();
	CWnd* pWnd = AfxGetMainWnd();
	if (WK_IS_WINDOW(pWnd))
	{
		pWnd->PostMessage(WM_USER);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCGemosToVision::OnRequestGetValue(CSecID id, // might be used as group ID only
								   const CString &sKey,
								   DWORD dwServerData)
{
	WK_TRACE(_T("OnRequestGetValue(%08lx,%s,%d)\n"),id.GetID(),sKey,dwServerData);
}
//////////////////////////////////////////////////////////////////////
void CIPCGemosToVision::OnConfirmGetValue(CSecID id, // might be used as group ID only
								   const CString &sKey,
								   const CString &sValue,
								   DWORD dwServerData)
{
	WK_TRACE(_T("OnConfirmGetValue(%08lx,%s,%s,%d)\n"),id.GetID(),sKey,sValue,dwServerData);
}
//////////////////////////////////////////////////////////////////////
void CIPCGemosToVision::OnConfirmSetValue(CSecID id, // might be used as group ID only
								   const CString &sKey,
								   const CString &sValue,
								   DWORD dwServerData)
{
	WK_TRACE(_T("OnConfirmSetValue(%08lx,%s,%s,%d)\n"),id.GetID(),sKey,sValue,dwServerData);
	if (id == SECID_NO_ID)
	{
		DWORD dwSubID = 0;
		if (1 == _stscanf(sValue,_T("%d"),&dwSubID))
		{
			if (0 == sKey.CompareNoCase(_T("alarm")))
			{
				switch ((GemosInputState)dwServerData)
				{
					case Off:
						theApp.DoConfirmLine(ALARM_LINE, (WORD)dwSubID,0x08);
						break;
					case On:
						theApp.DoConfirmLine(ALARM_LINE, (WORD)dwSubID,0x09);
						break;
					case Disabled:
						theApp.DoConfirmLine(ALARM_LINE, (WORD)dwSubID,0x14);
						break;
					case Enabled:
						theApp.DoConfirmLine(ALARM_LINE, (WORD)dwSubID,0x16);
						break;
				}
			}
			else if (0 == sKey.CompareNoCase(_T("cam")))
			{
				switch ((GemosInputState)dwServerData)
				{
					case Off:
						theApp.DoConfirmLine(CAM_LINE, (WORD)dwSubID,0x0a);
						break;
					case On:
						theApp.DoConfirmLine(CAM_LINE, (WORD)dwSubID,0x0b);
						break;
					case Disabled:
						theApp.DoConfirmLine(CAM_LINE, (WORD)dwSubID,0x14);
						break;
					case Enabled:
						theApp.DoConfirmLine(CAM_LINE, (WORD)dwSubID,0x16);
						break;
				}
			}
			else if (0 == sKey.CompareNoCase(_T("relay")))
			{
				switch ((GemosInputState)dwServerData)
				{
					case Off:
						theApp.DoConfirmLine(RELAY_LINE, (WORD)dwSubID,0x0a);
						break;
					case On:
						theApp.DoConfirmLine(RELAY_LINE, (WORD)dwSubID,0x0b);
						break;
					case Disabled:
						theApp.DoConfirmLine(RELAY_LINE, (WORD)dwSubID,0x14);
						break;
					case Enabled:
						theApp.DoConfirmLine(RELAY_LINE, (WORD)dwSubID,0x16);
						break;
				}
			}
			else if (0 == sKey.CompareNoCase(_T("err")))
			{
				theApp.DoConfirmLine(CAM_LINE, (WORD)dwSubID,0x19);
			}
			else
			{
				WK_TRACE_ERROR(_T("invalid key %s,%s\n"), sKey, sValue);
			}
		}
		else
		{
			theApp.DoConfirmLine(CAM_LINE, 0,0x19);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCGemosToVision::OnRequestSetValue(CSecID id, // might be used as group ID only
								   const CString &sKey,
								   const CString &sValue,
								   DWORD dwServerData)
{
	WK_TRACE(_T("OnRequestSetValue(%08lx,%s,%s,%d)\n"), id.GetID(), sKey, sValue, dwServerData);
}
