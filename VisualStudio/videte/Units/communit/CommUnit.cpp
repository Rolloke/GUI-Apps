/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CommUnit.cpp $
// ARCHIVE:		$Archive: /Project/Units/CommUnit/CommUnit.cpp $
// CHECKIN:		$Date: 5.05.06 12:19 $
// VERSION:		$Revision: 76 $
// LAST CHANGE:	$Modtime: 5.05.06 12:19 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"

#include "CommUnit.h"
#include "CommInput.h"
#include "CommWindow.h"
#include "CRS232.h"
#include "CameraControl.h"
#include "ControlRecordBaxall.h"
#include "ControlRecordFastrax.h"
#include "ControlRecordTOA.h"
#include "ControlRecordUni1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCommUnitApp
CCommUnitApp theApp;
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CCommUnitApp, CWinApp)
	//{{AFX_MSG_MAP(CCommUnitApp)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CCommUnitApp construction
CCommUnitApp::CCommUnitApp()
{
	InitVideteLocaleLanguage();
	m_pInput = NULL;
	m_pRS232Transparent = NULL;
	m_bTraceCommand = FALSE;
	m_bTraceData = FALSE;
	m_bTraceRS232Events = FALSE;
	m_iTestResponse = -1;
}
/////////////////////////////////////////////////////////////////////////////
// CCommUnitApp initialization
BOOL CCommUnitApp::InitInstance()
{
	if (WK_ALONE(WK_APP_NAME_COMMUNIT)==FALSE) return FALSE;

	CString sTitle;
	sTitle = COemGuiApi::GetApplicationName(WAI_COMMUNIT);
	m_pszAppName = _tcsdup(sTitle);    // human readable title

	InitDebugger(_T("CommUnit.log"),WAI_COMMUNIT);

	// Dongle vorhanden?
	CWK_Dongle dongle(WAI_COMMUNIT); 
	if (dongle.IsExeOkay()==FALSE)
	{
		return FALSE;
	}

#if _MFC_VER >= 0x0710
	SetLanguageParameters(0, 0);
#else
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	WNDCLASS  wndclass ;

	// register window class
	wndclass.style =         0;
	wndclass.lpfnWndProc =   DefWindowProc;
	wndclass.cbClsExtra =    0;
	wndclass.cbWndExtra =    0;
	wndclass.hInstance =     m_hInstance;
	wndclass.hIcon =         LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
	wndclass.hCursor =       LoadCursor(IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1) ;
	wndclass.lpszMenuName =  0L;
	wndclass.lpszClassName = WK_APP_NAME_COMMUNIT;

	AfxRegisterClass(&wndclass);

	CCommWindow *pWindow = new CCommWindow();
	m_pMainWnd = (CWnd*)pWindow;

	// create Input object
	m_pInput = new CCommInput(SM_COMMUNIT_INPUT);

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
int CCommUnitApp::ExitInstance() 
{
	WK_DELETE(m_pInput);
	CControlRecord* pControlRecord = NULL;
	for (int i=0 ; i<m_ControlRecordArray.GetSize() ; i++)
	{
		pControlRecord = m_ControlRecordArray.GetAtFast(i);
		pControlRecord->Destroy();
	}
	m_ControlRecordArray.DeleteAll();
	CCameraControlRecord* pCameraControlRecord;
	for (i=0 ; i<m_CameraControlRecordArray.GetSize() ; i++)
	{
		pCameraControlRecord = m_CameraControlRecordArray.GetAtFast(i);
		pCameraControlRecord->Destroy();
	}
	m_CameraControlRecordArray.DeleteAll();
	CloseDebugger();
	return CWinApp::ExitInstance();
}
/////////////////////////////////////////////////////////////////////////////
CameraControlType CCommUnitApp::GetControlType(CSecID id, CControlRecord**ppCR, CCameraControlRecord**ppCCR)
{
	if (id == m_RelayPTZID)
	{
		return CCT_RELAY_PTZ;
	}

	CControlRecord* pControlRecord = NULL;
	for (int i=0 ; i<m_ControlRecordArray.GetSize() ; i++)
	{
		pControlRecord = m_ControlRecordArray.GetAtFast(i);
		if (pControlRecord->HasSecID(id))
		{
			if (ppCR) *ppCR = pControlRecord;
			return pControlRecord->GetType();
		}
	}

	CCameraControlRecord* pCameraControlRecord = NULL;
	for (i=0 ; i<m_CameraControlRecordArray.GetSize() ; i++)
	{
		pCameraControlRecord = m_CameraControlRecordArray.GetAtFast(i);
		if (pCameraControlRecord->HasSecID(id))
		{
			if (ppCR) *ppCCR = pCameraControlRecord;
			return pCameraControlRecord->GetType();
		}
	}
	return CCT_UNKNOWN;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CCommUnitApp::GetControlFunctions(CSecID id)
{
	CControlRecord*pCR = NULL;
	CCameraControlRecord *pCCR=NULL;
	CameraControlType cct = GetControlType(id, &pCR, &pCCR);
	DWORD dwCCF = GetCameraControlFunctions(cct);
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
		dwCCF = wkp.GetInt(sReg, REG_PTZF_CCFUNCTIONS, dwCCF);
	}
	return dwCCF;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CCommUnitApp::GetControlFunctionsEx(CSecID id)
{
	CControlRecord*pCR = NULL;
	CCameraControlRecord *pCCR=NULL;
	CameraControlType cct = GetControlType(id, &pCR, &pCCR);
	DWORD dwCCFEx = GetCameraControlFunctionsEx(cct);
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
		dwCCFEx = wkp.GetInt(sReg, REG_PTZF_CCFUNCTIONS_EX, dwCCFEx);
	}
	return dwCCFEx;
}
/////////////////////////////////////////////////////////////////////////////
void CCommUnitApp::LoadDebugOptions()
{
	CWKDebugOptions debugOptions;
	CWK_Profile wkp;

	// check for trace options in registry
	BOOL bTraceMS = (BOOL)wkp.GetInt(_T("CommUnit\\Debug"), _T("TraceMilliSeconds"), 0);
	m_bTraceCommand		= wkp.GetInt(_T("CommUnit\\Debug"), _T("TraceCommand"), m_bTraceCommand);
	m_bTraceData		= wkp.GetInt(_T("CommUnit\\Debug"), _T("TraceData"), m_bTraceData);
	m_bTraceRS232Events	= wkp.GetInt(_T("CommUnit\\Debug"), _T("TraceRS232Events"), m_bTraceRS232Events);
	m_iTestResponse		= wkp.GetInt(_T("CommUnit\\Debug"), _T("TestResponse"), m_iTestResponse);

	// write trace options at least once in registry
	wkp.WriteInt(_T("CommUnit\\Debug"), _T("TraceMilliSeconds"), bTraceMS);
	wkp.WriteInt(_T("CommUnit\\Debug"), _T("TraceCommand"), m_bTraceCommand);
	wkp.WriteInt(_T("CommUnit\\Debug"), _T("TraceData"), m_bTraceData);
	wkp.WriteInt(_T("CommUnit\\Debug"), _T("TraceRS232Events"), m_bTraceRS232Events);
	wkp.WriteInt(_T("CommUnit\\Debug"), _T("TestResponse"), m_iTestResponse);

	if (bTraceMS)
	{
		SET_WK_STAT_TICK_COUNT(GetTickCount());
	}

	// check for C:\CommUnit.cfg first, then in current directory
	// will add trace options only
	BOOL bFoundFile = debugOptions.ReadFromFile(_T("c:\\CommUnit.cfg"));
	if (bFoundFile==FALSE) 
	{
		bFoundFile = debugOptions.ReadFromFile(_T("CommUnit.cfg"));
	}
	if (bFoundFile)
	{
		m_bTraceCommand = debugOptions.GetValue(_T("TraceCommand"), FALSE);
		m_bTraceData    = debugOptions.GetValue(_T("TraceData"), FALSE);
		m_iTestResponse = debugOptions.GetValue(_T("TestResponse"), -1);
	}

	// Trace the trace options in log file
	WK_TRACE(_T("TraceCommand=%d\n"), m_bTraceCommand);
	WK_TRACE(_T("TraceData=%d\n"), m_bTraceData);
	WK_TRACE(_T("TraceRS232Events=%d\n"), m_bTraceRS232Events);
	WK_TRACE(_T("TestResponse=%d\n"), m_iTestResponse);
	Sleep(100); // to 
}
/////////////////////////////////////////////////////////////////////////////
void CCommUnitApp::LoadCameraControls()
{
	int iRS232Transparent;
	int c,j,d;
	CString sSection, sKey, sTemp;
	CSecID  secID;
	DWORD	camID;

	CameraControlType type;
	DWORD id;
	int t;
	CCameraControlRecord* pCameraControlRecord = NULL;
	CControlRecord* pControlRecord = NULL;

	// will be called on reset
	// so close our coms
	// and terminate the repeat threads in CCameraControlRecord::Destroy()
	for (int i=0 ; i<m_ControlRecordArray.GetSize() ; i++)
	{
		pControlRecord = m_ControlRecordArray.GetAtFast(i);
		if (pControlRecord)
		{
			pControlRecord->Destroy();
		}
	}

	// delete the CCameraControlRecord's
	m_ControlRecordArray.DeleteAll();

	c =	m_CameraControlRecordArray.GetSize();
	for (i=0 ; i<c ; i++)
	{
		pCameraControlRecord = (CCameraControlRecord*)m_CameraControlRecordArray.GetAtFast(i);
		if (pCameraControlRecord)
		{
			pCameraControlRecord->Destroy();
		}
	}

	// delete the CCameraControlRecord's
	m_CameraControlRecordArray.DeleteAll();

	// Relay PTZ is independent from array
	m_RelayPTZID = SECID_NO_ID;

	CWK_Profile wkp;
	iRS232Transparent = wkp.GetInt(WK_APP_NAME_COMMUNIT,_T("COM"),0);
	m_pRS232Transparent = NULL;
	m_sOpenComs.Empty();

	sTemp = wkp.GetString(_T("CommUnit\\RelayPTZ"),_T("CID"),_T(""));
	if (!sTemp.IsEmpty())
	{
		_stscanf(sTemp,_T("%08x"),&id);
		m_RelayPTZID = id;
	}

	DWORD dwComMask = GetCommPortInfo();
	int iCom = 1;
	for (DWORD dwBit = 1; dwBit; dwBit<<=1,iCom++)
	{
		if (dwComMask & dwBit)
		{
			sSection.Format(_T("CommUnit\\COM%d"), iCom);
			d = wkp.GetInt(sSection,_T("Num"),0);
			if (d>0)
			{
				// found a sn control at COMi
				WK_TRACE(_T("found %d sn control at COM%d\n"),d, iCom);
				pControlRecord = NULL;
				pCameraControlRecord = NULL;
				t = wkp.GetInt(sSection,_T("Typ"),0);
				type = (CameraControlType)t;
				switch (type)
				{
				case CCT_BAXALL:
					pControlRecord = new CControlRecordBaxall(iCom, m_bTraceData);
					break;
				case CCT_FASTRAX:
					pControlRecord = new CControlRecordFastrax(iCom, m_bTraceData);
					break;
				case CCT_TOA:
					pControlRecord = new CControlRecordTOA(iCom, m_bTraceData);
					break;
				case CCT_PELCO_D:
				case CCT_PELCO_P:
				case CCT_BBV_RS422:
				case CCT_SENSORMATIC:
				case CCT_CBC_ZCNAF27:
				case CCT_VIDEOTEC:
				case CCT_BEWATOR:
				case CCT_JVC_TK_C:
				case CCT_MERIDIAN:
					pControlRecord = new CControlRecordUni1(iCom, m_bTraceData, type);
					break;
				default:
					pCameraControlRecord = new CCameraControlRecord();
					break;
				}
				if (pControlRecord)
				{
					for (j=0;j<d;j++)
					{
						sKey.Format(_T("SID%d"),j);
						sTemp = wkp.GetString(sSection,sKey,_T(""));
						_stscanf(sTemp,_T("%08x"),&id);
						secID = id;
						sKey.Format(_T("CID%d"),j);
						camID = wkp.GetInt(sSection,sKey,0);
						if (!pControlRecord->Add(secID,camID))
						{
							CString sError, sSection, sCam;
							sSection.Format(_T("OutputGroups\\Group%04x"), secID.GetGroupID());
							sCam = wkp.GetString(sSection, secID.GetSubID()+1, NULL);
							sCam = wkp.GetStringFromString(sCam, INI_COMMENT, NULL);
							sError.Format(IDS_WRONG_CAMERA_ID, camID, sCam, NameOfEnumPTZRealName(type), iCom);
							CWK_RunTimeError e(WAI_COMMUNIT, REL_ERROR, RTE_CAMERA_ID, sError,0,0);
							e.Send();
						}
					}
					if (pControlRecord->Create())
					{
						m_ControlRecordArray.Add(pControlRecord);
						sTemp.Format(_T("COM%d"), iCom);
						m_sOpenComs += _T(' ');
						m_sOpenComs += sTemp;
					}
					else
					{
						WK_DELETE(pControlRecord);
					}
				}
				else if (pCameraControlRecord)
				{
					for (j=0;j<d;j++)
					{
						sKey.Format(_T("SID%d"),j);
						sTemp = wkp.GetString(sSection,sKey,_T(""));
						_stscanf(sTemp,_T("%08x"),&id);
						secID = id;
						sKey.Format(_T("CID%d"),j);
						camID = wkp.GetInt(sSection,sKey,0);
						if (!pCameraControlRecord->Add(type, secID,camID))
						{
							CString sError, sSection, sCam;
							sSection.Format(_T("OutputGroups\\Group%04x"), secID.GetGroupID());
							sCam = wkp.GetString(sSection, secID.GetSubID()+1, NULL);
							sCam = wkp.GetStringFromString(sCam, INI_COMMENT, NULL);
							sError.Format(IDS_WRONG_CAMERA_ID, camID, sCam, NameOfEnumPTZRealName(type), iCom);
							CWK_RunTimeError e(WAI_COMMUNIT, REL_ERROR, RTE_CAMERA_ID, sError,0,0);
							e.Send();
						}
					}

					if (pCameraControlRecord->Create(type, iCom))
					{
						m_CameraControlRecordArray.Add(pCameraControlRecord);
						if (iRS232Transparent==iCom)
						{
							m_pRS232Transparent = pCameraControlRecord->GetRS232();
							WK_TRACE(_T("CameraControl Typ=%s COM=%d\n"),NameOfEnum(CCT_RS232_TRANSPARENT),iRS232Transparent);
						}
						sTemp.Format(_T("COM%d"), iCom);
						m_sOpenComs += _T(' ');
						m_sOpenComs += sTemp;
					}
					else
					{
						WK_DELETE(pCameraControlRecord);
					}
				}
				else
				{ // should never be reached 
					WK_TRACE_ERROR(_T("CameraControl not created at Com:%i\n"), iCom);
				}
			}
		}
	}

	if ((m_pRS232Transparent==NULL) && (iRS232Transparent!=0))
	{
		pCameraControlRecord = new CCameraControlRecord();
		if (pCameraControlRecord->Create(CCT_RS232_TRANSPARENT,iRS232Transparent))
		{
			m_CameraControlRecordArray.Add(pCameraControlRecord);
			m_pRS232Transparent = pCameraControlRecord->GetRS232();
			sKey.Format(_T("COM%d"),iRS232Transparent);
			m_sOpenComs += _T(' ');
			m_sOpenComs += sKey;
		}
		else
		{
			WK_DELETE(pCameraControlRecord);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCommUnitApp::OnReset()
{
	WK_STAT_LOG(_T("Reset"),-1,_T("COM"));
	LoadDebugOptions();
	LoadCameraControls();
	CCommWindow* pWnd = (CCommWindow*)m_pMainWnd;
	if (pWnd)
	{
		pWnd->SetToolTip(m_sOpenComs);
	}
	if (m_pInput)
	{
		m_pInput->DoConfirmReset(m_pInput->GetGroupID());
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCommUnitApp::OnReadData(int iCom)
{
	BOOL bTransparent = FALSE;
	if (m_pInput && m_pRS232Transparent)
	{
		if (m_pRS232Transparent->GetCOMNumber()==iCom)
		{
			m_pInput->IndicateData(m_pRS232Transparent->GetReadBuffer(),
							m_pRS232Transparent->GetReadBufferLength());
			m_pRS232Transparent->DeleteReadBuffer();
			bTransparent = TRUE;
		}
	}
	if (!bTransparent)
	{
		BOOL bFound = FALSE;
		CCameraControlRecord* pCameraControlRecord = NULL;
		for (int i=0 ; i<m_CameraControlRecordArray.GetSize() ; i++)
		{
			pCameraControlRecord = m_CameraControlRecordArray.GetAtFast(i);
			if (pCameraControlRecord)
			{
				CRS232* pRS232 = pCameraControlRecord->GetRS232();
				if (pRS232 && pRS232->GetCOMNumber() == iCom)
				{
					bFound = TRUE;
					CameraControlType cct = pCameraControlRecord->GetType();
					if (cct == CCT_VCL_MAXCOM)
					{
						pCameraControlRecord->ReceivedDataVCLMaxCom(pRS232->GetReadBuffer(),
												 pRS232->GetReadBufferLength());
						pRS232->DeleteReadBuffer();
						bFound = TRUE;
						break;
					}
				}
			}
		}
		if (!bFound)
		{
			WK_TRACE_ERROR(_T("OnReadData, CameraControl not found at Com:%i\n"), iCom);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCommUnitApp::OnWriteData()
{
	if (m_pRS232Transparent)
	{
		m_pRS232Transparent->Write(m_pInput->GetWriteBuffer(),
								   m_pInput->GetWriteBufferLength());
		m_pInput->DeleteWriteBuffer();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCommUnitApp::OnCameraCommand(CCameraCommandRecord* pCCommandR)
{
	BOOL bFound = FALSE;
	CControlRecord* pControlRecord = NULL;
	for (int i=0 ; i<m_ControlRecordArray.GetSize() ; i++)
	{
		pControlRecord = m_ControlRecordArray.GetAtFast(i);
		if (pControlRecord->HasSecID(pCCommandR->GetCamID()))
		{
			pControlRecord->OnNewCommand(pCCommandR);
			bFound = TRUE;
			break;
		}
	}
	if (!bFound)
	{
		CCameraControlRecord* pCameraControlRecord = NULL;
		for (int i=0 ; i<m_CameraControlRecordArray.GetSize() ; i++)
		{
			pCameraControlRecord = m_CameraControlRecordArray.GetAtFast(i);
			if (pCameraControlRecord->HasSecID(pCCommandR->GetCamID()))
			{
				pCameraControlRecord->OnNewCommand(pCCommandR);
				bFound = TRUE;
				break;
			}
		}
	}
	if (!bFound)
	{
		WK_TRACE_ERROR(_T("CameraControl not found %08x, %08x\n"),
							pCCommandR->GetCommID().GetID(),
							pCCommandR->GetCamID().GetID()
							);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCommUnitApp::OnTest()
{

	CCameraControlRecord* pCameraControlRecord = NULL;
	CRS232* pCom;
	int c;

	c = m_CameraControlRecordArray.GetSize();
	if (c>0)
	{
		pCameraControlRecord = m_CameraControlRecordArray.GetAtFast(0);
	}
	pCom = pCameraControlRecord->GetRS232();

	if (pCom)
	{
		BYTE szBuf[7];

		szBuf[0] = _STX_;
		szBuf[1] = 'F';
		szBuf[2] = 'F';
		szBuf[3] = 'S';
		szBuf[4] = '0';
		szBuf[5] = '1';
		szBuf[6] = _ETX_;
		pCom->Write((LPVOID)&szBuf,7);
	}

}
/////////////////////////////////////////////////////////////////////////////
void CCommUnitApp::TraceData(int iCom, LPVOID pBuf, DWORD dwLen, BOOL bRead)
{
	CString sM;
	CString sTrace;
	CString sHex;
	DWORD j;
	BYTE* pBuffer;

	if (pBuf)
	{
		pBuffer = (BYTE*)pBuf;
		if (bRead)
		{
			sM.Format(_T("COM%d read %ld byte(s)"),iCom,dwLen);
		}
		else
		{
			sM.Format(_T("COM%d wrote %ld byte(s)"),iCom,dwLen);
		}

		if (!(m_bTraceData & 0x0008))	// Hexausgabe
		{
			sTrace += _T("HEX:");
			for (j=0;j<dwLen;j++)
			{
				sHex.Format(_T("%02x "),pBuffer[j]);
				sTrace += sHex;
			}
		}
		if (m_bTraceData & 0x0004)		// Asscii
		{
			sTrace += _T("Asccii:");
			for (j=0;j<dwLen;j++)
			{
				sTrace += pBuffer[j];
			}
		}
		WK_TRACE(_T("%s:%s\n"), sM, sTrace);
	}
	else
	{
		WK_TRACE(_T("buffer is empty"));
	}
}
/////////////////////////////////////////////////////////////////////////////////
#if _MFC_VER >= 0x0710
int CCommUnitApp::SetLanguageParameters(UINT uCodePage, DWORD dwCPbits)
{
	if (uCodePage == 0)
	{
		uCodePage = _ttoi(COemGuiApi::GetCodePage());
	}
	else
	{
		COemGuiApi::ChangeLanguageResource();
		InitVideteLocaleLanguage();
	}

//  Set the CodePage for MBCS conversion, if necessary
//	CWK_String::SetCodePage(uCodePage);
//	CWK_Profile wkp;
//	wkp.SetCodePage(CWK_String::GetCodePage());
	if (dwCPbits == 0)
	{
		dwCPbits = COemGuiApi::GetCodePageBits();
	}
	if (dwCPbits)
	{
		SetFontFaceNamesFromCodePageBits(dwCPbits);
	}
	else
	{
		CSkinDialog::DoUseGlobalFonts(FALSE);
	}
	return 0;
}
#endif