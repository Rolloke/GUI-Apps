// ControlRecord.cpp: implementation of the CControlRecord class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CommUnit.h"
#include "ControlRecord.h"
#include ".\controlrecord.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CControlRecord::CControlRecord(	CameraControlType type,
								int iCom,
								BOOL bTraceAscii,
								BOOL bTraceHex,
								BOOL bTraceEvents)
	: CWK_RS232(iCom, bTraceAscii, bTraceHex, bTraceEvents)
{
	m_pCurrentCommand = NULL;
	m_Type = type;
	SetTraceRS232Events(theApp.m_bTraceRS232Events);
}
//////////////////////////////////////////////////////////////////////
CControlRecord::~CControlRecord()
{
	WK_DELETE(m_pCurrentCommand);
	m_arrayCommand.SafeDeleteAll();
}
/////////////////////////////////////////////////////////////////////////////
void CControlRecord::Destroy()
{
	Close();
	while (IsShuttingDown())
	{
		Sleep(10);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CControlRecord::InitialiseResponses()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CControlRecord::Add(CSecID secID, DWORD dwCamID)
{
	m_secIDs.Add(secID.GetID());
	m_camIDs.Add(dwCamID);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CControlRecord::HasSecID(CSecID secID)
{
	BOOL bFound = FALSE;
	for (int i=0 ; i<m_secIDs.GetSize() ; i++)
	{
		if (secID == m_secIDs[i])
		{
			bFound = TRUE;
			break;
		}
	}
	return bFound;
}
//////////////////////////////////////////////////////////////////////////////////////
void CControlRecord::OnReceivedData(LPBYTE pBuffer,DWORD dwLen)
{
	// may be ignored, otherwise overwrite in derived class
}
/////////////////////////////////////////////////////////////////////////////
void CControlRecord::ClearAllData()
{
	m_byaReceived.RemoveAll();
}
/////////////////////////////////////////////////////////////////////////////
CString CControlRecord::GetRegPath()
{
	CString sReg;
	sReg.Format(_T("%s\\COM%03d\\%s"), WK_APP_NAME_COMMUNIT, GetCOMNumber(), NameOfEnum(GetType()));
	return sReg;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CControlRecord::ReadPortValues(DWORD& dwBaudRate, BYTE& byDataBits, BYTE& byParity, BYTE& byStopBits)
{
	DWORD dwReg;
	CWK_Profile wkp;
	CString sReg = GetRegPath();
	dwReg = wkp.GetInt(sReg, _T("BaudRate"), -1);
	if (dwReg == -1)
	{
		wkp.WriteInt(sReg, _T("BaudRate"), dwBaudRate);
	}
	else
	{
		dwBaudRate = CheckBaudRate(dwReg);
	}

	dwReg = wkp.GetInt(sReg, _T("DataBits"), -1);
	if (dwReg == -1)
	{
		wkp.WriteInt(sReg, _T("DataBits"), byDataBits);
	}
	else
	{
		if (IsBetween(dwReg, 1, 8))
		{
			byDataBits = (BYTE)dwReg;
		}
		else
		{
			WK_TRACE(_T("Wrong DataBits registry value, taking 8 bits\n"), NameOfEnum(GetType()), GetCOMNumber());
		}
	}
	
	dwReg = wkp.GetInt(sReg, _T("Parity"), -1);
	if (dwReg == -1)
	{
		wkp.WriteInt(sReg, _T("Parity"), byParity);
	}
	else
	{
		if (IsBetween(dwReg, NOPARITY, SPACEPARITY))
		{
			byParity = (BYTE)dwReg;
		}
		else
		{
			WK_TRACE(_T("Wrong Parity registry value, taking NOPARITY\n"), NameOfEnum(GetType()), GetCOMNumber());
		}
	}

	dwReg = wkp.GetInt(sReg, _T("StopBits"), -1);
	if (dwReg == -1)
	{
		wkp.WriteInt(sReg, _T("StopBits"), byStopBits);
	}
	else
	{
		if (IsBetween(dwReg, ONESTOPBIT, TWOSTOPBITS))
		{
			byStopBits = (BYTE)dwReg;
		}
		else
		{
			WK_TRACE(_T("Wrong StopBit registry value, taking ONESTOPBIT\n"), NameOfEnum(GetType()), GetCOMNumber());
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CControlRecord::CheckBaudRate(DWORD dwBaudRate)
{
	int nBaudRates[] = 
	{
		CBR_110,
		CBR_300,
		CBR_600,
		CBR_1200,
		CBR_2400,
		CBR_4800,
		CBR_9600,
		CBR_14400,
		CBR_19200,
		CBR_38400,
		CBR_56000,
		CBR_57600,
		CBR_115200,
		CBR_128000,
		CBR_256000
	};
	int i, nBest = 0, nRates = sizeof(nBaudRates) / sizeof(int);
	DWORD dwBest = CBR_256000*2, dwDiff;
	for (i=0; i<nRates; i++)
	{
		dwDiff = abs((int)(nBaudRates[i] - dwBaudRate));
		if (dwDiff < dwBest)
		{
			dwBest = dwDiff;
			nBest = i;
			if (dwDiff == 0)
			{
				break;
			}
		}
		else if (dwDiff > dwBest)
		{
			break;
		}
	}
	return nBaudRates[nBest];
}

BOOL CCommandRecord::IsTelemetryCommand(CameraControlCmd cmd)
{
	switch (cmd)
	{
		case CCC_PAN_LEFT:
		case CCC_PAN_RIGHT:
		case CCC_TILT_UP:
		case CCC_TILT_DOWN:
		case CCC_MOVE_LEFT_DOWN:
		case CCC_MOVE_LEFT_UP:
		case CCC_MOVE_RIGHT_DOWN:
		case CCC_MOVE_RIGHT_UP:
			return TRUE;
		case CCC_FOCUS_FAR:
		case CCC_FOCUS_NEAR:
		case CCC_ZOOM_IN:
		case CCC_ZOOM_OUT:
		case CCC_IRIS_OPEN:
		case CCC_IRIS_CLOSE:
			return 2;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCommandRecord::IsPositionCmd(CameraControlCmd cmd)
{
	switch (cmd)
	{
		case CCC_POS_1: return 1;
		case CCC_POS_2: return 2;
		case CCC_POS_3: return 3;
		case CCC_POS_4: return 4;
		case CCC_POS_5: return 5;
		case CCC_POS_6: return 6;
		case CCC_POS_7: return 7;
		case CCC_POS_8: return 8;
		case CCC_POS_9: return 9;
		case CCC_POS_HOME: return -1;
	}
	return FALSE;
}
