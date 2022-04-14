/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDIUnit.cpp $
// ARCHIVE:		$Archive: /Project/Units/SDIUnit/SDIUnit.cpp $
// CHECKIN:		$Date: 7/20/06 2:20p $
// VERSION:		$Revision: 161 $
// LAST CHANGE:	$Modtime: 7/20/06 9:35a $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "resource.h"

#include "SDIUnit.h"
#include "SDIWindow.h"

#include "SDIControlRecord.h"
#include "RecordConfigurable.h"
#include "RecordSniDos.h"
#include "RecordNcr_1_1.h"
#include "RecordEngelMultipass.h"
#include "RecordKebaPc2000.h"
#include "RecordKebaPasador.h"
#include "RecordGermanParcel.h"
#include "RecordScanner.h"
#include "RecordIBM.h"
#include "RecordNCR_1_2.h"
#include "RecordSniIbmEmulation.h"
#include "RecordSniDosCom.h"
#include "RecordAscom.h"
#include "RecordLandauDtp.h"
#include "RecordMicrolockSystem1.h"
#include "RecordACTAccessControl.h"
#include "RecordNCTDiva.h"
#include "RecordStarInterface.h"
#include "RecordPM100.h"
#include "RecordPM100XML.h"
#include "RecordSchneiderIntercom.h"
#include "RecordPOS.h"
#include "RecordIdipXML.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only CSDIUnitApp object
CSDIUnitApp theApp;

CString GetValue(const CString& sData, const CString& sParameter)
{
	CString sValue;
	int pos1 = sData.Find(sParameter);
	if (pos1 != -1) 
	{
		int pos2 = sData.Find(_T('&'),pos1);
		if (pos2 != -1)
		{
			pos1 += sParameter.GetLength()+1;
			sValue = sData.Mid(pos1,pos2-pos1);
		}
		else
		{
			pos1 += sParameter.GetLength()+1;
			sValue = sData.Mid(pos1);
		}
	}

	if (!sValue.IsEmpty())
	{
		if (-1!= sValue.Find(_T('%')))
		{
			CStringArray sa;
			SplitString(sValue,sa,_T('%'));
			sValue.Empty();
			for (int i=0;i<sa.GetSize();i++)
			{
				DWORD dw = 0;
				CString s(sa[i]);
				if (!s.IsEmpty())
				{
					_stscanf((LPCTSTR)s,_T("%02x"),&dw);
					sValue += (char)dw;
				}
			}
		}
	}

	return sValue;
}
//////////////////////////////////////////////////////////////////////
// TicTime=820842276
// Sekunden seit 01.01.1980 0:00 Uhr
CSystemTime GetSystemTimeFromDesignaTime(DWORD dwDesignaTime)
{
	CSystemTime st1980;
	st1980.wDay = 1;
	st1980.wMonth = 1;
	st1980.wYear = 1980;
	st1980.wHour = 0;
	st1980.wMinute = 0;
	st1980.wSecond = 0;
	st1980.IncrementTime(0,0,0,dwDesignaTime,0);
	return st1980;

}
/////////////////////////////////////////////////////////////////////////////
CSDIUnitApp::CSDIUnitApp()
{
	m_pDongle = NULL;
	m_bTraceiSTData = TRUE;
	m_bTraceRS232Data = TRUE;
	m_bTraceHex = FALSE;
	m_bTraceAscii = TRUE;
	m_bTraceEvents = FALSE;
	m_bIgnoreNonDigitsInValue = FALSE;
	m_dwLastReset = GetCurrentTime() - RESET_DELAY_TIME;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIUnitApp::InitInstance()
{
	/*
	CSystemTime t = GetSystemTimeFromDesignaTime(820842276);
	TRACE(_T("%s\n"),t.GetDateTime());*/

	if (!WK_ALONE(WK_APP_NAME_SDIUNIT)) 
	{
		return FALSE;
	}
	AfxEnableControlContainer();

	AfxSocketInit();

	CoInitialize(0);

	CString sTitle;
	sTitle = COemGuiApi::GetApplicationName(WAI_SDIUNIT);
	m_pszAppName = _tcsdup(sTitle);    // human readable title

	InitDebugger(_T("SDIUnit.log"),WAI_SDIUNIT);

	// Never touch this line manually
	WK_TRACE(_T("Version %s\n"),
				PLAIN_TEXT_VERSION(13, 12, 11, 22, _T("13/12/99 , 11:22\0"), // @AUTO_UPDATE
				0)
				);

	WK_STAT_PEAK(_T("Reset"), 2, _T("Active"));
	WK_STAT_LOG(_T("Reset"), 1, _T("Active"));

	// Dongle vorhanden?
	m_pDongle = new CWK_Dongle(WAI_SDIUNIT); 
	if ( !m_pDongle->RunSDIUnit() ) {
		WK_TRACE_WARNING(_T("No Dongle or not allowed\n"));
		return FALSE;
	}

#if _MFC_VER <0x700
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	// Kommandozeile auswerten
	CString m_sCommandLine = m_lpCmdLine;

	// register window class
	WNDCLASS  wndclass ;
	wndclass.style =         0;
	wndclass.lpfnWndProc =   DefWindowProc;
	wndclass.cbClsExtra =    0;
	wndclass.cbWndExtra =    0;
	wndclass.hInstance =     m_hInstance;
	wndclass.hIcon =         LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
	wndclass.hCursor =       LoadCursor(IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1) ;
	wndclass.lpszMenuName =  0L;
	wndclass.lpszClassName = WK_APP_NAME_SDIUNIT;

	AfxRegisterClass(&wndclass);

	CSDIWindow *pWindow = new CSDIWindow();
	m_pMainWnd = (CWnd*)pWindow;

	// Debug Konfiguration laden
	LoadDebugConfiguration();

	// Erst mal alle ControlRecords anlegen
	LoadSDIControlRecords();

	// Im ToolTip des Icons die geoeffneten Schnittstellen ausgeben
	CSDIWindow* pWnd = (CSDIWindow*)m_pMainWnd;
	if (pWnd) 
	{
		pWnd->SetToolTip(m_sOpenComs);
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
int CSDIUnitApp::ExitInstance() 
{
	m_CSDICRArray.DeleteAll();
	WK_DELETE(m_pDongle);
	WK_STAT_LOG(_T("Reset"), 0, _T("Active"));
	CloseDebugger();
	CoUninitialize();
	return CWinApp::ExitInstance();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIUnitApp::LoadDebugConfiguration() 
{
	// Einlesen der cfg-Datei
	CWKDebugOptions debugOptions;
	// check for C:\SDIUnit.cfg first, then in current directory
	BOOL bFoundFile = debugOptions.ReadFromFile(_T("c:\\SDIUnit.cfg"));
	if (bFoundFile==FALSE) {
		bFoundFile = debugOptions.ReadFromFile(_T("SDIUnit.cfg"));
	}
	// Setzen der Variablen
	int iTraceMilliSeconds = 0;
	if (bFoundFile)
	{
		m_bTraceiSTData = debugOptions.GetValue(_T("TraceiSTData"),m_bTraceiSTData);
		m_bTraceHex = debugOptions.GetValue(_T("TraceRS232DataHex"),m_bTraceHex);
		m_bTraceAscii = debugOptions.GetValue(_T("TraceRS232DataAscii"),m_bTraceAscii);
		m_bTraceEvents = debugOptions.GetValue(_T("TraceRS232Events"),m_bTraceEvents);
		m_bIgnoreNonDigitsInValue = debugOptions.GetValue(_T("IgnoreNonDigitsInValue"),m_bIgnoreNonDigitsInValue);
		iTraceMilliSeconds = debugOptions.GetValue(_T("TraceMilliSeconds"),0);
	}
	else
	{
		CWK_Profile wkp;
		CString sSection = _T("SDIUnit\\Debug");
		m_bTraceiSTData = wkp.GetInt(sSection, _T("TraceiSTData"), m_bTraceiSTData);
		m_bTraceHex = wkp.GetInt(sSection, _T("TraceRS232DataHex"), m_bTraceHex);
		m_bTraceAscii = wkp.GetInt(sSection, _T("TraceRS232DataAscii"), m_bTraceAscii);
		m_bTraceEvents = wkp.GetInt(sSection, _T("TraceRS232Events"), m_bTraceEvents);
		m_bIgnoreNonDigitsInValue = wkp.GetInt(sSection, _T("IgnoreNonDigitsInValue"), m_bIgnoreNonDigitsInValue);
		iTraceMilliSeconds = wkp.GetInt(sSection, _T("TraceMilliSeconds"), 0);

		// Mindestens einmal die Schlüssel in die Registry schreiben
		wkp.WriteInt(sSection, _T("TraceiSTData"), m_bTraceiSTData);
		wkp.WriteInt(sSection, _T("TraceRS232DataHex"), m_bTraceHex);
		wkp.WriteInt(sSection, _T("TraceRS232DataAscii"), m_bTraceAscii);
		wkp.WriteInt(sSection, _T("TraceRS232Events"), m_bTraceEvents);
		wkp.WriteInt(sSection, _T("IgnoreNonDigitsInValue"), m_bIgnoreNonDigitsInValue);
		wkp.WriteInt(sSection, _T("TraceMilliSeconds"), iTraceMilliSeconds);
	}

	if (0 > iTraceMilliSeconds)
	{
		SET_WK_STAT_TICK_COUNT(GetTickCount());
	}
	else
	{
		SET_WK_STAT_TICK_COUNT((DWORD)iTraceMilliSeconds);
	}

	// TraceData auf mehreren Moeglichkeiten
	m_bTraceRS232Data = m_bTraceHex || m_bTraceAscii;

	// Im Debug-Modus einige Werte standartmaessig TRUE setzen
#ifdef _DEBUG
	WK_TRACE(_T("===> Debug-Version <===\n"));
#endif

	if (0 < GET_WK_STAT_TICK_COUNT()) {
		WK_TRACE(_T("WK_STAT_TICK_COUNT=%ld\n"),GET_WK_STAT_TICK_COUNT());
	}
	WK_TRACE(_T("TraceiSTData=%d\n"),m_bTraceiSTData);
	WK_TRACE(_T("TraceRS232DataHex=%d\n"),m_bTraceHex);
	WK_TRACE(_T("TraceRS232DataAscii=%d\n"),m_bTraceAscii);
	WK_TRACE(_T("TraceRS232Events=%d\n"),m_bTraceEvents);
	WK_TRACE(_T("IgnoreNonDigitsInValue=%d\n"),m_bIgnoreNonDigitsInValue);

	Sleep(100);
}
/////////////////////////////////////////////////////////////////////////////
void CSDIUnitApp::LoadSDIControlRecords()
{
	// will be called on reset
	// so close our coms
	// terminate the repeat threads in CSDIControlRecord::~CSDIControlRecord()
	m_CSDICRArray.DeleteAll();
	WK_TRACE(_T("All Controls deleted\n"));
	m_sOpenComs.Empty();

	// check for available COM-Ports
	DWORD dwComPortMask = GetCommPortInfo();
	CString sComPorts, sCom;
	DWORD dw=1;
	for (int i=1 ; dw ; dw<<=1,i++) {
		if (dw & dwComPortMask) {
			sCom.Format(_T(" %i"), i);
			sComPorts += sCom;
		}
	}
	WK_TRACE( _T("COMs available%s\n"),sComPorts );

	CWK_Profile wkp;
	CString sSection, sComment;
	SDIControlType type = SDICT_UNKNOWN;
	int iType = SDICT_UNKNOWN;
	CSDIControlRecord* pSDIControlRecord = NULL;

	// check for COM Registry entries
	for (int iCom=1, dw=1; dw ; dw<<=1,i++,iCom++) 
	{
		if (dw & dwComPortMask) 
		{
			sSection.Format(szSectionFormat, iCom);
			iType = wkp.GetInt(sSection, szEntryType, SDICT_UNKNOWN);
			if (iType!=SDICT_UNKNOWN) 
			{
				// found a SDIcontrol at COMi
				type = (SDIControlType)iType;
				sComment = wkp.GetString(sSection, szEntryComment, _T(""));
				WK_TRACE(_T("Found SDIControl %s at COM%d, %s\n"),CSDIControl::NameOfEnum(type), iCom, (LPCTSTR)sComment);
				pSDIControlRecord = CreateControlRecord(wkp,type,iCom);
				if (pSDIControlRecord)
				{
					AppendControlRecord(wkp,sSection,pSDIControlRecord);
				}
			}
		}
	}

	CString sAddress;
	int iPort = 0;
	// check for IP Registry entries
	for (int iIP=1 ; ;iIP++) 
	{
		sSection.Format(szSectionUDP, iIP);
		iType = wkp.GetInt(sSection, szEntryType, SDICT_UNKNOWN);
		if (iType==SDICT_UNKNOWN) 
		{
			break;
		}
		else
		{
			// found a SDIcontrol at COMi
			type = (SDIControlType)iType;
			sComment = wkp.GetString(sSection, szEntryComment, _T(""));
			sAddress = wkp.GetString(sSection, szEntryIPaddress, _T(""));
			iPort = wkp.GetInt(sSection,szEntryPort,0);
			WK_TRACE(_T("Found SDIControl %s at IP%d, %s\n"),CSDIControl::NameOfEnum(type), iIP, (LPCTSTR)sComment);
			pSDIControlRecord = CreateControlRecord(wkp,type,iPort,sAddress);
			if (pSDIControlRecord)
			{
				AppendControlRecord(wkp,sSection, pSDIControlRecord);
			}
		}

	}
}
/////////////////////////////////////////////////////////////////////////////
CSDIControlRecord* CSDIUnitApp::CreateControlRecord(CWK_Profile& wkp, SDIControlType type, int iCom)
{
	CSDIControlRecord* pSDIControlRecord = NULL;

	if (	m_pDongle->AllowSDICashSystems()
		||	m_pDongle->AllowSDIAccessControl()
		)
	{
		if (SDICT_LANDAU_DTP == type ) 
		{
			pSDIControlRecord = new CRecordLandauDtp(type,iCom,m_bTraceAscii,m_bTraceHex,m_bTraceEvents);
		}
	}

	if ( m_pDongle->AllowSDICashSystems() ) 
	{
		switch (type)
		{
		case SDICT_SNI_DOS_CHASE:
			pSDIControlRecord = new CRecordSniDosChase(type,iCom,m_bTraceAscii,m_bTraceHex,m_bTraceEvents);
			break;
		case SDICT_NCR_VER_1_1:
			pSDIControlRecord = new CRecordNcr_1_1(type,iCom,m_bTraceAscii,m_bTraceHex,m_bTraceEvents);
			break;
		case SDICT_IBM:
			pSDIControlRecord = new CRecordIbm(type,iCom,m_bTraceAscii,m_bTraceHex,m_bTraceEvents);
			break;
		case SDICT_NCR_VER_1_2:
			pSDIControlRecord = new CRecordNcr_1_2(type,iCom,m_bTraceAscii,m_bTraceHex,m_bTraceEvents);
			break;
		case SDICT_SNI_IBM_EMU:
			pSDIControlRecord = new CRecordSniIbmEmulation(type,iCom,m_bTraceAscii,m_bTraceHex,m_bTraceEvents);
			break;
		case SDICT_SNI_DOS_COM:
				pSDIControlRecord = new CRecordSniDosCom(type,iCom,m_bTraceAscii,m_bTraceHex,m_bTraceEvents);
				break;
		case SDICT_ASCOM:
			pSDIControlRecord = new CRecordAscom(type,iCom,m_bTraceAscii,m_bTraceHex,m_bTraceEvents);
			break;
		}
	}
	if ( m_pDongle->AllowSDIAccessControl() )
	{
		switch (type)
		{
		case SDICT_ENGEL_MULTIPASS:
			pSDIControlRecord = new CRecordEngelMultipass(type,iCom,m_bTraceAscii,m_bTraceHex,m_bTraceEvents);
			break;
		case SDICT_KEBA_PC400:
			pSDIControlRecord = new CRecordKebaPc400(type,iCom,m_bTraceAscii,m_bTraceHex,m_bTraceEvents);
			break;
		case SDICT_KEBA_PC2000:
			pSDIControlRecord = new CRecordKebaPc2000(type,iCom,m_bTraceAscii,m_bTraceHex,m_bTraceEvents);
			break;
		case SDICT_KEBA_PASADOR:
			pSDIControlRecord = new CRecordKebaPasador(type,iCom,m_bTraceAscii,m_bTraceHex,m_bTraceEvents);
			break;
		case SDICT_ACCESS_MICROLOCK:
			pSDIControlRecord = new CRecordMicrolockSystem1(type,iCom,m_bTraceAscii,m_bTraceHex,m_bTraceEvents);
			break;
		case SDICT_ACCESS_ACT:
			pSDIControlRecord = new CRecordACTAccessControl(type,iCom,m_bTraceAscii,m_bTraceHex,m_bTraceEvents);
			break;
		case SDICT_STAR_INTERFACE:
			pSDIControlRecord = new CRecordStarInterface(type,iCom,m_bTraceAscii,m_bTraceHex,m_bTraceEvents);
			break;
		}
	}
	if ( m_pDongle->AllowSDIIndustrialEquipment() ) 
	{
		switch (type)
		{
		case SDICT_GERMAN_PARCEL:
			pSDIControlRecord = new CRecordGermanParcel(type,iCom,m_bTraceAscii,m_bTraceHex,m_bTraceEvents);
			break;
		case SDICT_SCANNER:
			pSDIControlRecord = new CRecordScanner(type,iCom,m_bTraceAscii,m_bTraceHex,m_bTraceEvents);
			break;
		case SDICT_NCT_DIVA:
			pSDIControlRecord = new CRecordNCTDiva(type,iCom,m_bTraceAscii,m_bTraceHex,m_bTraceEvents);
			break;
		case SDICT_POS:
			pSDIControlRecord = new CRecordPOS(type,iCom,m_bTraceAscii,m_bTraceHex,m_bTraceEvents);
			break;
		case SDICT_IDIP_XML:
			pSDIControlRecord = new CRecordIdipXML(type,iCom,0,NULL,m_bTraceAscii,m_bTraceHex);
			break;
		}
	}

	if (m_pDongle->AllowSDIParkmanagment())
	{
		switch (type) 
		{
		case SDICT_SCHNEIDER_INTERCOM:
			pSDIControlRecord = new CRecordSchneiderIntercom(type,iCom,m_bTraceAscii,m_bTraceHex,m_bTraceEvents);
			break;
		}
	}

	return pSDIControlRecord;

}
/////////////////////////////////////////////////////////////////////////////
CSDIControlRecord* CSDIUnitApp::CreateControlRecord(CWK_Profile& wkp, SDIControlType type, int iPort, LPCTSTR szAddress)
{
	CSDIControlRecord* pSDIControlRecord = NULL;

	if (m_pDongle->AllowSDIParkmanagment())
	{
		switch (type)
		{
		case SDICT_DESIGNA_PM_100_ALARM:
			pSDIControlRecord = new CRecordPM100Alarm(type,iPort,szAddress,m_bTraceAscii,m_bTraceHex);
			break;
		case SDICT_DESIGNA_PM_100_XML:
			pSDIControlRecord = new CRecordPM100XML(type,iPort,szAddress,m_bTraceAscii,m_bTraceHex);
			break;
		}
	}
	if (m_pDongle->AllowSDIIndustrialEquipment())
	{
		switch (type) 
		{
		case SDICT_IDIP_XML:
			pSDIControlRecord = new CRecordIdipXML(type,0,iPort,szAddress,m_bTraceAscii,m_bTraceHex);
			break;
		default:
			break;
		}
	}
	return pSDIControlRecord;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIUnitApp::AppendControlRecord(CWK_Profile& wkp, const CString& sSection, CSDIControlRecord* pSDIControlRecord)
{
	int iCom = pSDIControlRecord->GetCOMNumber();
	SDIControlType type = pSDIControlRecord->GetType();
	if (pSDIControlRecord) 
	{
		if (pSDIControlRecord->Create(wkp,sSection)) 
		{
			m_CSDICRArray.Add(pSDIControlRecord);
			if ( pSDIControlRecord->IsValid() ) 
			{
				CString sTemp;
				sTemp.Format(_T(" COM%i"),iCom);
				m_sOpenComs += sTemp;
			}
		}
		else 
		{
			CString sText;
			sText.Format(IDS_SDI_CONTROL_CREATE_FAILED,(int)type, CSDIControl::NameOfEnum(type),iCom);
			CWK_RunTimeError rtError(WAI_SDIUNIT, REL_ERROR, RTE_CONFIGURATION, sText);
			rtError.Send();
			WK_DELETE(pSDIControlRecord);
		}
	}
	else 
	{
		CString sText;
		sText.Format(IDS_SDICONTROL_WRONG_TYPE,
			(int)type, CSDIControl::NameOfEnum(type), iCom);
		CWK_RunTimeError rtError(WAI_SDIUNIT, REL_ERROR, RTE_CONFIGURATION, sText);
		rtError.Send();
	}
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSDIUnitApp, CWinApp)
	//{{AFX_MSG_MAP(CSDIUnitApp)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
void CSDIUnitApp::OnReset()
{
	DWORD dwTime = GetCurrentTime();
	if ( dwTime > m_dwLastReset + RESET_DELAY_TIME ) {
		WK_STAT_LOG(_T("Reset"),-1,_T("Active"));
		LoadSDIControlRecords();
		CSDIWindow* pWnd = (CSDIWindow*)m_pMainWnd;
		if (pWnd) {
			pWnd->SetToolTip(m_sOpenComs);
		}
		m_dwLastReset = GetCurrentTime();
	}
}
