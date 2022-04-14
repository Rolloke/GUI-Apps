// IPCControlAudioUnit.cpp: implementation of the CIPCControlAudioUnit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "systemverwaltung.h"
#include "IPCControlAudioUnit.h"

#include "AudioPage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIPCControlAudioUnit::CIPCControlAudioUnit(LPCTSTR shmName, BOOL asMaster, CAudioPage *pDlg) : CIPCMedia(shmName, asMaster)
{
	ASSERT(pDlg != NULL);
	m_pDlg = pDlg;
	m_SecID.SetGroupID((WORD)(SECID_GROUP_MEDIA+m_pDlg->m_nPageNo));
	m_SecID.SetSubID(1);
	StartThread();
}
//////////////////////////////////////////////////////////////////////
CIPCControlAudioUnit::~CIPCControlAudioUnit()
{

}
//////////////////////////////////////////////////////////////////////
void CIPCControlAudioUnit::OnConfirmHardware(CSecID secID, int errorCode, DWORD dwFlags, DWORD dwRequestID, const CIPCExtraMemory *pEM)
{
	ASSERT(secID.GetGroupID() == m_SecID.GetGroupID());
	if (m_pDlg)
	{
		m_pDlg->OnConfirmHardware(errorCode, dwFlags, dwRequestID, pEM);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCControlAudioUnit::OnReadChannelFound()
{
	if (GetIsMaster())
	{
		DoRequestConnection();
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCControlAudioUnit::OnConfirmConnection()
{
	DoRequestSetGroupID(m_SecID.GetGroupID());
	DoRequestHardware(m_SecID, 0);
}
//////////////////////////////////////////////////////////////////////
void CIPCControlAudioUnit::OnConfirmValues(CSecID mediaID, DWORD dwCmd, DWORD dwID, DWORD dwValue, const CIPCExtraMemory*pData)
{
	if (m_pDlg)
	{
		m_pDlg->OnConfirmValues(mediaID, dwCmd, dwID, dwValue, pData);
	}
}
//////////////////////////////////////////////////////////////////////////
void CIPCControlAudioUnit::OnRequestDisconnect()
{
	if (m_pDlg)
	{
		m_pDlg->OnRequestDisconnect();
	}
}
//////////////////////////////////////////////////////////////////////////
void CIPCControlAudioUnit::OnConfirmSetGroupID(WORD wGroup)
{
	
}
