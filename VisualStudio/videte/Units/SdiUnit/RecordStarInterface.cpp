#include "stdafx.h"
#include "SDIUnit.h"
#include "SDIWindow.h"
#include "starinterface.h"
#include "recordstarinterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


///////////////////////////////////////////////////////////////////////////////////////
CRecordStarInterface::CRecordStarInterface(SDIControlType type,
										   int iCom,
										   BOOL bTraceAscii,
										   BOOL bTraceHex,
										   BOOL bTraceEvents)
 : CSDIControlRecord(type,iCom,0,NULL,bTraceAscii,bTraceEvents,bTraceHex)
{
	m_bComOpen = FALSE;
}
///////////////////////////////////////////////////////////////////////////////////////
CRecordStarInterface::~CRecordStarInterface()
{
	if (m_bComOpen)
	{
		CStarInterface* pCtrl = GetStarInterfaceCtrl();
		if (pCtrl)
		{
			pCtrl->PollingStop();
			pCtrl->CommPortClose();
		}
	}
	m_StarDevices.DeleteAll();
}
///////////////////////////////////////////////////////////////////////////////////////
CStarInterface* CRecordStarInterface::GetStarInterfaceCtrl()
{
	CStarInterface* pStarInterface = NULL;
	CSDIWindow* pSDIWindow = (CSDIWindow*)theApp.GetMainWnd();
	if (WK_IS_WINDOW(pSDIWindow))
	{
		pStarInterface = pSDIWindow->GetStarInterfaceCtrl();
	}

	return pStarInterface;
}
///////////////////////////////////////////////////////////////////////////////////////
BOOL CRecordStarInterface::Create(CWK_Profile& wkp, const CString& sSectionParameter)
{
	CString sSection;
	CString sCom;
	CStarInterface* pCtrl = GetStarInterfaceCtrl();
	WORD wStart = 0;
	WORD wEnd = 0;
	CString sSMName;
	CSDIInput* pInput = NULL;
	WORD wBoardIndex = 0;

	if (WK_IS_WINDOW(pCtrl))
	{
		sSection.Format(_T("SDIUnit\\COM%d\\Devices"),GetCOMNumber());
		m_StarDevices.Load(wkp,sSection);
		sCom.Format(_T("COM%d"),GetCOMNumber());

		for(int i=0;i<m_StarDevices.GetSize();i++)
		{
			CSDIStarDevice* pSDIStarDevice = m_StarDevices.GetAtFast(i);
			wBoardIndex = (WORD)pSDIStarDevice->GetID();
			pCtrl->SetWorkIndex((WORD)i);
			if (!m_bComOpen)
			{
				m_bComOpen = pCtrl->CommPortOpen(sCom,CBR_9600);
				if (m_bComOpen)
				{
					WK_TRACE(_T("STARINTERFACE com port opened %s\n"),sCom);
				}
			}
			if (m_bComOpen)
			{
				CString sBoardIndex;

				if (wBoardIndex >= wEnd)
				{
					wEnd = wBoardIndex;
				}
				if (wBoardIndex<=wStart)
				{
					wStart = wBoardIndex;
				}

				sBoardIndex.Format(_T("%02d"),wBoardIndex);
				pCtrl->SetBoardIndex(sBoardIndex);
				
				CString d = pCtrl->DateTimeUpload();
				WK_TRACE(_T("STARINTERFACE COM%d B%02d Date: %s\n"),GetCOMNumber(),wBoardIndex,d);
				
				d = pCtrl->GetFinger();
				WK_TRACE(_T("STARINTERFACE COM%d B%02d Finger: %s\n"),GetCOMNumber(),wBoardIndex,d);

				d = pCtrl->ModeUpload();
				WK_TRACE(_T("STARINTERFACE COM%d B%02d Mode: %s\n"),GetCOMNumber(),wBoardIndex,d);
			}
		}
		sSMName.Format(_T("%s%d_%02d"),SM_SDIUNIT_INPUT,GetCOMNumber(),wBoardIndex);
		pInput = new CSDIInput(sSMName,this);
		WK_TRACE(_T("Created SDIControl Typ=%s COM=%d %s\n"),CSDIControl::NameOfEnum(m_Type), GetCOMNumber(), (LPCTSTR)m_sComment);
		m_Inputs.Add(pInput);

		if (m_bComOpen)
		{
			pCtrl->PollingStart(wStart,wEnd);
		}
	}

	return m_bComOpen;

}
///////////////////////////////////////////////////////////////////////////////////////
BOOL CRecordStarInterface::IsValid() const
{
	return m_bComOpen;
}
///////////////////////////////////////////////////////////////////////////////////////
void CRecordStarInterface::SwitchToConfigMode(const CString& sPath, BOOL bReceive)
{
}
///////////////////////////////////////////////////////////////////////////////////////
void CRecordStarInterface::InitializeResponses()
{
}
///////////////////////////////////////////////////////////////////////////////////////
void CRecordStarInterface::OnReceivedData(LPBYTE pData, DWORD dwLen)
{
}
///////////////////////////////////////////////////////////////////////////////////////
BOOL CRecordStarInterface::CheckForNewAlarmData()
{
	// immer nur Einzelalarme
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////////////
void CRecordStarInterface::OnCardEventStarinterfacectrl(short nIndex, LPCTSTR BoardIndex, LPCTSTR ReaderIndex, LPCTSTR IDNumber, LPCTSTR EventDate, LPCTSTR EventTime, LPCTSTR EventStatus, LPCTSTR FunctionKey)
{
	WK_TRACE(_T("Card Event %d %s %s ID=%s Date=%s Time=%s ES=%s F=%s\n"),
		nIndex,BoardIndex,ReaderIndex,IDNumber,EventDate, EventTime, EventStatus, FunctionKey);
	
	int iEvent = 0;
	DWORD dwBoardIndex = 0;
	CSDIInput* pInput = NULL;
	CSDIStarDevice* pSDIStarDevice = NULL;

	if (   1==_stscanf(EventStatus,_T("%d"),&iEvent)
		&& 1==_stscanf(BoardIndex,_T("%d"),&dwBoardIndex))
	{
		for (int i=0;i<m_StarDevices.GetSize();i++)
		{
			pSDIStarDevice = m_StarDevices.GetAtFast(i);
			if (pSDIStarDevice->GetID() == dwBoardIndex)
			{
				pInput = m_Inputs.GetAt(i);
				break;
			}
		}
		if (pInput)
		{
			m_sSingleData = IDNumber;
			StoreAccount();

			m_sSingleData = EventDate;
			StoreDate(SDI_DATA_FORMAT_YYYYMMDD);

			m_sSingleData = EventTime;
			StoreTime(SDI_DATA_FORMAT_HHMMSS);

			IndicateAlarm((WORD)iEvent+1);

			ClearAllData();
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////
void CRecordStarInterface::OnReceiveDataStarinterfacectrl(short nIndex, LPCTSTR ReceiveString)
{
	if (GetTraceRS232DataAscii())
	{
		TraceData((LPVOID)ReceiveString,wcslen(ReceiveString),TRUE);
	}
}
///////////////////////////////////////////////////////////////////////////////////////
void CRecordStarInterface::OnSendDataStarinterfacectrl(short nIndex, LPCTSTR SendString)
{
	if (GetTraceRS232DataAscii())
	{
		TraceData((LPVOID)SendString,wcslen(SendString),FALSE);
	}
}
