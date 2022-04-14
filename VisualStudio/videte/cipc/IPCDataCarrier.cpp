// IPCDataCarrier.cpp: implementation of the CIPCDataCarrier class.
//
//////////////////////////////////////////////////////////////////////

#include "stdcipc.h"
#include "CipcExtraMemory.h"
#include "IPCDataCarrier.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIPCDataCarrier::CIPCDataCarrier(LPCTSTR shmName, BOOL asMaster)
	: CIPC(shmName, asMaster)
{
	// nothing todo
}
//////////////////////////////////////////////////////////////////////
CIPCDataCarrier::~CIPCDataCarrier()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCDataCarrier::HandleCmd(DWORD dwCmd,
							   DWORD dwParam1, 
							   DWORD dwParam2,
							   DWORD dwParam3, 
							   DWORD dwParam4,
							   const CIPCExtraMemory *pExtraMem)
{
	switch (dwCmd)
	{
	case CIPC_DC_REQUEST_VOLUME_INFOS:
		OnRequestVolumeInfos();
		break;
	case CIPC_DC_CONFIRM_VOLUME_INFOS:
		if (pExtraMem)
		{
			int iNumDrives = dwParam1;
			CIPCDrive *pData = CIPCDrives::DrivesFromBubble(iNumDrives,pExtraMem);
			OnConfirmVolumeInfos(iNumDrives,pData);
			WK_DELETE_ARRAY(pData);
		}
		else 
		{
			OnMissingExtraMemory(dwCmd);
		}
		break;
	case CIPC_DC_REQUEST_SESSION:
		if (pExtraMem)
		{
			CStringArray* pSA = pExtraMem->GetStrings();
			CString sVolume;
			CStringArray sa;
			sVolume = pSA->GetAt(0);
			for (int i=1;i<pSA->GetSize();i++)
			{
				sa.Add(pSA->GetAt(i));
			}
			OnRequestSession(dwParam1,sVolume,sa,dwParam2);
			WK_DELETE(pSA);
		}
		else
		{
			OnMissingExtraMemory(dwCmd);
		}
		break;
	case CIPC_DC_CONFIRM_SESSION:
		OnConfirmSession(dwParam1);
		break;
	case CIPC_DC_INDICATE_SESSION_PROGRESS:
		OnIndicateSessionProgress(dwParam1,(int)dwParam2);
		break;
	case CIPC_DC_REQUEST_CANCEL_SESSION:
		OnRequestCancelSession(dwParam1);
		break;
	case CIPC_DC_CONFIRM_CANCEL_SESSION:
		OnConfirmCancelSession(dwParam1);
		break;
	case CIPC_DC_REQUEST_DELETE_VOLUME:
		if (pExtraMem)
		{
			CString sVolume;
			sVolume = pExtraMem->GetString();
			OnRequestDeleteVolume(sVolume);
		}
		else
		{
			OnMissingExtraMemory(dwCmd);
		}
		break;
	case CIPC_DC_CONFIRM_DELETE_VOLUME:
		if (pExtraMem)
		{
			CString sVolume;
			sVolume = pExtraMem->GetString();
			OnConfirmDeleteVolume(sVolume);
		}
		else
		{
			OnMissingExtraMemory(dwCmd);
		}
		break;
		break;
	default:
		return CIPC::HandleCmd(dwCmd,dwParam1,dwParam2,dwParam3,dwParam4,pExtraMem);
		break;
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrier::DoRequestVolumeInfos()
{
	WriteCmd(CIPC_DC_REQUEST_VOLUME_INFOS);
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrier::OnRequestVolumeInfos()
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrier::DoConfirmVolumeInfos(const CIPCDrives& drives)
{
	CIPCExtraMemory *pBubble = drives.BubbleFromDrives(this);
	WriteCmdWithExtraMemory(pBubble,CIPC_DC_CONFIRM_VOLUME_INFOS,(DWORD)drives.GetSize());
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrier::OnConfirmVolumeInfos(int iNumDrives,
										   const CIPCDrive drives[])
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrier::DoRequestSession(DWORD dwSessionID,
									   const CString& sVolume,
									   const CStringArray& sPathnames,
									   DWORD dwFlags)
{
	CStringArray sa;
	sa.Add(sVolume);
	sa.Append(sPathnames);
	CIPCExtraMemory *pBubble = new CIPCExtraMemory;
	if (pBubble->Create(this,sa))
	{
		WriteCmdWithExtraMemory(pBubble,CIPC_DC_REQUEST_SESSION,dwSessionID,dwFlags);
	}
	else
	{
		WK_DELETE(pBubble);
		OnCommandSendError(CIPC_DC_REQUEST_SESSION);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrier::OnRequestSession(DWORD dwSessionID,
								       const CString& sVolume,
									   const CStringArray& sPathnames,
									   DWORD dwFlags)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrier::DoIndicateSessionProgress(DWORD dwSessionID, int iProgress)
{
	WriteCmd(CIPC_DC_INDICATE_SESSION_PROGRESS,dwSessionID,(DWORD)iProgress);
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrier::OnIndicateSessionProgress(DWORD dwSessionID, int iProgress)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrier::DoConfirmSession(DWORD dwSessionID)
{
	WriteCmd(CIPC_DC_CONFIRM_SESSION,dwSessionID);
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrier::OnConfirmSession(DWORD dwSessionID)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrier::DoRequestCancelSession(DWORD dwSessionID)
{
	WriteCmd(CIPC_DC_REQUEST_CANCEL_SESSION,dwSessionID);
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrier::OnRequestCancelSession(DWORD dwSessionID)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrier::DoConfirmCancelSession(DWORD dwSessionID)
{
	WriteCmd(CIPC_DC_CONFIRM_CANCEL_SESSION,dwSessionID);
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrier::OnConfirmCancelSession(DWORD dwSessionID)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrier::DoRequestDeleteVolume(const CString& sVolume)
{
	CIPCExtraMemory *pBubble = new CIPCExtraMemory;
	if (pBubble->Create(this,sVolume))
	{
		WriteCmdWithExtraMemory(pBubble,CIPC_DC_REQUEST_DELETE_VOLUME);
	}
	else
	{
		WK_DELETE(pBubble);
		OnCommandSendError(CIPC_DC_REQUEST_DELETE_VOLUME);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrier::OnRequestDeleteVolume(const CString& sVolume)
{
	OverrideError();
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrier::DoConfirmDeleteVolume(const CString& sVolume)
{
	CIPCExtraMemory *pBubble = new CIPCExtraMemory;
	if (pBubble->Create(this,sVolume))
	{
		WriteCmdWithExtraMemory(pBubble,CIPC_DC_CONFIRM_DELETE_VOLUME);
	}
	else
	{
		WK_DELETE(pBubble);
		OnCommandSendError(CIPC_DC_CONFIRM_DELETE_VOLUME);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrier::OnConfirmDeleteVolume(const CString& sVolume)
{
	OverrideError();
}
