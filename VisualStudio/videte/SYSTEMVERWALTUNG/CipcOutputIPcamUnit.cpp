/////////////////////////////////////////////////////////////////////////////
// PROJECT:		USBCameraUnit
// FILE:		$Workfile: CipcOutputIPcamUnit.cpp $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/CipcOutputIPcamUnit.cpp $
// CHECKIN:		$Date: 30.08.06 10:36 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 30.08.06 10:06 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "systemverwaltung.h"
#include "CameraParameterPage.h"

#include "CIPCOutputIPcamUnit.h"
#include "CIPCExtraMemory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
	#define DBG_TRACE(S) WK_TRACE(_T("%s\n%s(%d):\n"), S, _T(__FILE__), __LINE__);
#else
	#define DBG_TRACE // 
#endif


CConfirmValue::CConfirmValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwUserData)
{
	m_id = id;
	m_sKey = sKey;
	m_sValue = sValue;
	m_dwUserData = dwUserData;
}


CIPCOutputIPcamUnit::CIPCOutputIPcamUnit(CCameraParameterPage* pMainWnd, 
							 LPCTSTR shmName,
							 BOOL basMaster)
	: CIPCOutput(shmName, basMaster)
{
	m_wGroupID			= SECID_NO_GROUPID;				// Noch keine GroupID

	m_pParamDlg = pMainWnd;
	StartThread();
}

//////////////////////////////////////////////////////////////////////////////////////
CIPCOutputIPcamUnit::~CIPCOutputIPcamUnit()
{
	// TRACE(_T("CIPCOutputIPcamUnit::~CIPCOutputIPcamUnit\n"));
	StopThread();
}
//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputIPcamUnit::CheckGroupID(WORD wGid, LPCTSTR sFunction)
{
	if (wGid != m_wGroupID)
	{
		WK_TRACE_ERROR(_T("%s: Wrong GroupID\n"), sFunction);
		CSecID CId(m_wGroupID, SECID_NO_SUBID);
		DoIndicateError(0, CId, CIPC_ERROR_INVALID_GROUP, 0);	
		return FALSE;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputIPcamUnit::OnRequestSetGroupID(WORD wGroupID)
{
	TRACE(_T("OnRequestSetGroupID(%x)\n"), wGroupID);
	m_wGroupID = wGroupID;
	DoConfirmSetGroupID(m_wGroupID);
}
//////////////////////////////////////////////////////////////////////////
BOOL CIPCOutputIPcamUnit::IsConnected()
{
	return (GetIPCState() == CIPC_STATE_CONNECTED) ? TRUE : FALSE;
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputIPcamUnit::OnReadChannelFound()
{
	if (GetIsMaster())
	{
		DoRequestConnection();
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputIPcamUnit::OnConfirmConnection()
{
	if (GetIsMaster())
	{
		DoRequestSetGroupID(m_wGroupID);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputIPcamUnit::OnConfirmSetGroupID(WORD wGroupID)
{
	if (m_pParamDlg)
	{
		m_pParamDlg->PostMessage(WM_USER, WM_INITDIALOG);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputIPcamUnit::OnConfirmSetValue(CSecID id, const CString &sKey,
						const CString &sValue,
						DWORD dwUserData)
{
//	TRACE(_T("OnConfirmSetValue(%s, %s, %x)\n"), sKey, sValue, dwUserData);
	if (m_pParamDlg)
	{
		CConfirmValue *pcv = new CConfirmValue(id, sKey, sValue, dwUserData);
		m_pParamDlg->PostMessage(WM_USER, MAKELONG(WM_USER, ONCONFIRMSETVALUE), (LPARAM)pcv);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputIPcamUnit::OnConfirmGetValue(CSecID id, const CString &sKey,
						const CString &sValue,
						DWORD dwUserData)
{
//	TRACE(_T("OnConfirmGetValue(%s, %s, %x)\n"), sKey, sValue, dwUserData);
	if (m_pParamDlg)
	{
		CConfirmValue *pcv = new CConfirmValue(id, sKey, sValue, dwUserData);
		m_pParamDlg->PostMessage(WM_USER, MAKELONG(WM_USER, ONCONFIRMGETVALUE), (LPARAM)pcv);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputIPcamUnit::OnRequestDisconnect()
{
	if (m_pParamDlg && m_pParamDlg->GetSafeHwnd())
	{
		m_pParamDlg->PostMessage(WM_USER, WM_CLOSE);
	}
}
//////////////////////////////////////////////////////////////////////
