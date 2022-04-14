// CCommInput.cpp
// Author : Uwe Freiwald

// Implementation of CCommInput
// 

#include "stdafx.h"
#include "CommUnit.h"
#include "CommInput.h"

#include "CameraCommandRecord.h"
#include "cipcstringdefs.h"

#include "ControlRecordUni1.h"

//extern CCommUnitApp theApp;

//////////////////////////////////////////////////////////////////////////////////////
// Constructor
CCommInput::CCommInput(LPCTSTR shmName)
	: CIPCInput(shmName,FALSE)
{
	m_wGroupID = SECID_NO_GROUPID;
	m_dwBlockNr = 0L;
	m_pWriteBuffer = NULL;
	m_dwBufferLen = 0;
	StartThread();
}
//////////////////////////////////////////////////////////////////////////////////////
// Destructor
CCommInput::~CCommInput()
{
	StopThread();
	DeleteWriteBuffer();
}
//////////////////////////////////////////////////////////////////////////////////////
void CCommInput::OnRequestSetGroupID(WORD wGroupID)
{
	m_wGroupID = wGroupID; 
	DoConfirmSetGroupID(wGroupID);
};
//////////////////////////////////////////////////////////////////////////////////////
void CCommInput::OnRequestHardware(WORD wGroupNr)
{
	DoConfirmHardware(m_wGroupID,0);
}
//////////////////////////////////////////////////////////////////////////////////////
void CCommInput::OnRequestSetEdge(WORD wGroupID,DWORD edgeMask)	// 1 positive, 0 negative
{
	DoConfirmEdge(m_wGroupID, edgeMask);
}
//////////////////////////////////////////////////////////////////////////////////////
void CCommInput::OnRequestSetFree(WORD wGroupID,DWORD openMask)	// 1 open, 0 closed
{
	DoConfirmFree(m_wGroupID, openMask);
}
//////////////////////////////////////////////////////////////////////////////////////
void CCommInput::OnRequestReset(WORD wGroupID)
{
	AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_RESET);
}
//////////////////////////////////////////////////////////////////////////////////////
void CCommInput::OnRequestAlarmState(WORD wGroupID)
{
	DoConfirmAlarmState(wGroupID,0);	// required by server
}

//////////////////////////////////////////////////////////////////////////////////////
void CCommInput::OnRequestDisconnect()
{
}
//////////////////////////////////////////////////////////////////////////////////////
void CCommInput::IndicateData(LPVOID pBuffer,DWORD dwLen)
{
	if ((pBuffer!=NULL) && (dwLen>0))
	{
		CIPCExtraMemory data;
		if (data.Create(this,dwLen,pBuffer))
		{
			CSecID id(m_wGroupID,0);
			DoIndicateCommData(id,data,m_dwBlockNr);
			m_dwBlockNr++;
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CCommInput::OnWriteCommData(CSecID id,const CIPCExtraMemory &data,	DWORD dwBlockNr)
{
	DeleteWriteBuffer();
	m_dwBufferLen = data.GetLength();
	m_pWriteBuffer = malloc(m_dwBufferLen);
	CopyMemory(m_pWriteBuffer,data.GetAddress(),m_dwBufferLen);
	AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_WRITE_DATA);
}
/////////////////////////////////////////////////////////////////////////////
void CCommInput::DeleteWriteBuffer()
{
	if (m_pWriteBuffer)
	{
		free(m_pWriteBuffer);
		m_pWriteBuffer = NULL;
	}
	m_dwBufferLen = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CCommInput::OnRequestCameraControl(CSecID commID, CSecID camID,CameraControlCmd cmd,DWORD dwValue)
{
	CCameraCommandRecord* pCCR;
	pCCR = new CCameraCommandRecord(commID,camID,cmd,dwValue);
	AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_CAMERA_COMMAND,(LPARAM)pCCR);
}
/////////////////////////////////////////////////////////////////////////////
void CCommInput::OnRequestGetValue(CSecID id, // might be used as group ID only
								   const CString &sKey,
								   DWORD dwServerData)
{
	CString sValue;
	DWORD dwid;

	if (1 == _stscanf(sKey, CSD_CAM_CONTROL_TYPE_FMT, &dwid))
	{
		sValue = NameOfEnum(theApp.GetControlType(dwid));
		WK_TRACE(_T("Confirm CameraControlType id %08x CCT %s\n"), dwid, sValue);
		DoConfirmGetValue(id, sKey, sValue, dwServerData);
	}
	else if (1 == _stscanf(sKey, CSD_CAM_CONTROL_FKT_FMT,&dwid))
	{
		sValue.Format(_T("%08x"),theApp.GetControlFunctions(dwid));
		WK_TRACE(_T("Confirm CameraControlFunctions id %08x CCT %s\n"), dwid, sValue);
		DoConfirmGetValue(id, sKey, sValue, dwServerData);
	}
	else if (1 == _stscanf(sKey, CSD_CAM_CONTROL_FKTEX_FMT, &dwid))
	{
		sValue.Format(_T("%08x"),theApp.GetControlFunctionsEx(dwid));
		WK_TRACE(_T("Confirm CameraControlFunctions id %08x CCT %s\n"), dwid, sValue);
		DoConfirmGetValue(id, sKey, sValue, dwServerData);
	}
	else
	{
		CString sID = sKey.Mid(sKey.GetLength()-10);
		if (1 == _stscanf(sID, _T("(%08x)"), &dwid))
		{
			CControlRecord*pCR = NULL;
			CCameraControlRecord *pCCR=NULL;
			CameraControlType cct = theApp.GetControlType(dwid, &pCR, &pCCR);
			int nCom = 0;
			if (pCR)
			{
				nCom = pCR->GetCOMNumber();
			}
			else if (pCCR && pCCR->GetRS232())
			{
				nCom = pCCR->GetRS232()->GetCOMNumber();
			}
			if (nCom)
			{
				CWK_Profile wkp;
				CControlRecordUni1 cr(nCom, FALSE, cct);
				CString sReg = cr.GetRegPath();
				CString sRegValue = sKey.Left(sKey.GetLength()-10);
				sValue = wkp.GetString(sReg, sRegValue, NULL);
				if (!sValue.IsEmpty())
				{
//					sValue = COemGuiApi::TestMultilangString(sValue, FALSE);
					WK_TRACE(_T("Confirm %s id %08x %s\n"), sRegValue, dwid, sValue);
					DoConfirmGetValue(id, sKey, sValue, dwServerData);
				}
			}
		}
	}
}

