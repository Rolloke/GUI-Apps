/////////////////////////////////////////////////////////////////////////////
// PROJECT:		USBCameraUnit
// FILE:		$Workfile: CipcInputIPcamUnit.cpp $
// ARCHIVE:		$Archive: /Project/Units/IPCameraUnit/CipcInputIPcamUnit.cpp $
// CHECKIN:		$Date: 17.08.06 9:15 $
// VERSION:		$Revision: 7 $
// LAST CHANGE:	$Modtime: 16.08.06 11:44 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IPCameraUnit.h"
#include "IPCameraUnitDlg.h"
#include "CIPCInputIpCamUnit.h"

CIPCInputIpCamUnit::CIPCInputIpCamUnit(CIpCameraUnitDlg* pDlg, LPCTSTR shmName)
	: CIPCInput(shmName,FALSE)
{
//	TRACE("CIPCInputIpCamUnit::CIPCInputIpCamUnit\n");
	m_pDlg			= pDlg;
	m_wGroupID		= SECID_NO_GROUPID;
	m_bOK			= TRUE;
	m_bReset		= FALSE;
	m_bCheck		= FALSE;
	m_bMDinput		= FALSE;
	m_dwAlarmState	= 0L;					// Alarmstatus
	m_dwEdge		= 0xFFFFFFFF;

	StartThread();
}

//////////////////////////////////////////////////////////////////////////////////////
CIPCInputIpCamUnit::~CIPCInputIpCamUnit()
{
//	TRACE("CIPCInputIpCamUnit::~CIPCInputIpCamUnit\n");
	m_bOK	= FALSE;
	StopThread();
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCInputIpCamUnit::IsValid()
{
	return m_bOK;
}
//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCInputIpCamUnit::Run(DWORD dwTimeOut)
{
//	dwTimeOut = 100;
	if (m_bCheck && !m_bMDinput)
	{
		WORD i, n = (WORD)m_saInputs.GetCount();
		DWORD dwMask, dwActive = m_dwFree;
		for (i=0, dwMask=1; i<n; i++, dwMask<<=1)
		{
			if (dwActive & dwMask)
			{
				BOOL bOld = m_dwAlarmState & dwMask ? TRUE : FALSE;
				BOOL bNew = DoRequestInputs(m_saInputs[i]);
				if (bOld != bNew)
				{
					TRACE(_T("Alarmstate%d changed=%d\n"), i+1, bNew);
					m_pDlg->NotifyIOList(CSecID(m_wGroupID, i), bNew);
					AlarmStateChanged(i, bNew);
				}
			}
		}
		m_bCheck = FALSE;
	}
	return CIPCInput::Run(dwTimeOut);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIpCamUnit::OnRequestSetGroupID(WORD wGroupID)
{
	m_wGroupID = wGroupID;
	DoConfirmSetGroupID(m_wGroupID);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIpCamUnit::OnRequestHardware(WORD wGroupID)
{
	if (!CheckGroupID(wGroupID, _T(__FUNCTION__)))
	{
		return;
	}

	DoConfirmHardware(m_wGroupID, 0);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIpCamUnit::OnRequestSetEdge(WORD wGroupID, DWORD dwEdgeMask)
{
	TRACE(_T("CIPCInputIpCamUnit::OnRequestSetEdge(%04x, %08x)\n"), wGroupID, dwEdgeMask);

	if (!CheckGroupID(wGroupID, _T(__FUNCTION__)))
	{
		return;
	}

	m_dwEdge = dwEdgeMask;

	DoConfirmEdge(m_wGroupID, dwEdgeMask);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIpCamUnit::InitInputStrings()
{
	m_saInputs.RemoveAll();
	m_dwAlarmState = 0;

	CWK_Profile wkp;
	CString sSection, sInputs;
	sSection.Format(_T("%s\\") SEC_NAME_GROUP, SEC_NAME_INPUTGROUPS, m_wGroupID);
	int i;
	DWORD dwMask;

	for (i=1, dwMask=1; i<=32; i++, dwMask<<=1)
	{
		sInputs   = wkp.GetString(sSection, i, NULL);
		if (sInputs.IsEmpty())
		{
			break;
		}
		if (DoRequestInputs(sInputs))
		{
			m_dwAlarmState |= dwMask;
		}
		m_saInputs.Add(sInputs);
	}
	m_pDlg->NotifyIOList(CSecID(m_wGroupID, SECID_NO_SUBID), TRUE);
}
//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCInputIpCamUnit::DoRequestInputs(const CString& sInputs)
{
	BOOL bOK = FALSE;
	CString sType, sUrl, sHTTPRequest;
	int nPort, nNr;
	BOOL    bPost = FALSE;
	sType = CWK_Profile::GetStringFromString(sInputs, INI_INPUT_TYPE, NULL);
	sUrl  = CWK_Profile::GetStringFromString(sInputs, INI_URL, NULL);
	nPort = CWK_Profile::GetNrFromString(sInputs, INI_PORT, 0);
	nNr   = CWK_Profile::GetNrFromString(sInputs, INI_INPUT_NR, 1);
	if (sType.CompareNoCase(CSD_CAM_AXIS) == 0)
	{
		sHTTPRequest.Format(_T("axis-cgi/io/input.cgi?check=%d"), nNr);
	}
	else if (sType.CompareNoCase(CSD_CAM_PANASONIC) == 0)
	{
		// TODO! RKE: implement Panasonic
	}
	// TODO! RKE: implement other input protocolls
	if (!sHTTPRequest.IsEmpty())
	{
		DWORD dwResult;
		CString sAnswer = m_pDlg->HTTPRequest(sUrl, nPort, sHTTPRequest, dwResult, bPost);
		if (dwResult == HTTP_STATUS_OK)
		{
			bOK = sAnswer.Find(_T("=1")) != -1;
		}
	}
	return bOK;
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIpCamUnit::OnRequestSetFree(WORD wGroupID, DWORD dwOpenMask)
{
	TRACE(_T("CIPCInputIpCamUnit::OnRequestSetFree(%04x, %08x)\n"), wGroupID, dwOpenMask);

	if (!CheckGroupID(wGroupID, _T(__FUNCTION__)))
	{
		return;
	}

	m_dwFree = dwOpenMask;

	DoConfirmFree(m_wGroupID, dwOpenMask);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIpCamUnit::OnRequestReset(WORD wGroupID)	
{
	TRACE(_T("OnRequestReset(%s)\n"), GetShmName());
	if (!CheckGroupID(wGroupID, _T(__FUNCTION__)))
	{
		return;
	}
	m_bReset = TRUE;
	m_pDlg->InitResetTimer();
	if (!m_bMDinput)
	{
		InitInputStrings();
		if (m_dwFree)
		{
			m_pDlg->StartInputRequestTimer();
		}
		else
		{
			m_pDlg->StopInputRequestTimer();
		}
	}
	ConfirmReset();
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIpCamUnit::OnRequestAlarmState(WORD wGroupID)
{
	TRACE(_T("CIPCInputIpCamUnit::OnRequestAlarmState(%04x)\n"), wGroupID);
 
	if (!CheckGroupID(wGroupID, _T(__FUNCTION__)))
	{
		return;
	}

	DoConfirmAlarmState(m_wGroupID, m_dwAlarmState);
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIpCamUnit::OnResponseAlarmState(WORD wGroupID)
{
	TRACE(_T("CIPCInputIpCamUnit::OnResponseAlarmState(%04x) Not Yet \n"));

	if (!CheckGroupID(wGroupID, _T(__FUNCTION__)))
	{
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIpCamUnit::OnResponseInputs(WORD wGroupID)
{
	TRACE(_T("CIPCInputIpCamUnit::OnResponseInputs(%04x) Not Yet\n"));

	if (!CheckGroupID(wGroupID, _T(__FUNCTION__)))
	{
		return;
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIpCamUnit::AlarmStateChanged(WORD wInput, BOOL bSet)
{
	DWORD dwNewState = m_dwAlarmState;

	if (bSet)
	{
		dwNewState |= ((DWORD)1 << wInput);
	}
	else
	{
		dwNewState &= ~((DWORD)1 << wInput);
	}
	//	WK_TRACE(_T("CIPCInputIpCamUnit::AlarmStateChanged (0x%x,0x%x)\n"), dwNewState, m_dwAlarmState);

	DoIndicateAlarmState(m_wGroupID, dwNewState, dwNewState^m_dwAlarmState);

	m_dwAlarmState = dwNewState;
}
//////////////////////////////////////////////////////////////////////////////////////
BOOL CIPCInputIpCamUnit::CheckGroupID(WORD wGid, LPCTSTR sFunction)
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
DWORD CIPCInputIpCamUnit::GetAlarmState()
{
	return m_dwAlarmState;
}
//////////////////////////////////////////////////////////////////////////
BOOL CIPCInputIpCamUnit::IsAlarmActive(int nInput)
{
	if (IsBetween(nInput, 0, MAX_CAMERAS-1))
	{
		return (m_dwFree & (1 << nInput)) ?  TRUE : FALSE;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIpCamUnit::OnRequestDisconnect()
{
	m_wGroupID = SECID_NO_GROUPID;
	m_bReset = FALSE;
}
//////////////////////////////////////////////////////////////////////////
void CIPCInputIpCamUnit::ConfirmReset()
{
	m_bReset = FALSE;
	DoConfirmReset(m_wGroupID);
}
