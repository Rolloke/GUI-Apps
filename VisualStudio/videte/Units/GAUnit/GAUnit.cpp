// GAUnit.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"

#include "GAUnit.h"
#include "GAInput.h"
#include "GAWindow.h"
#include "GaRS232.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define GA_STX	1
#define GA_IFN	2
#define GA_DATE 3
#define GA_TIME 4
#define GA_TAN  5
#define GA_AN   6
#define GA_BLZ  7
#define GA_KTN  8
#define GA_BETR 9
#define GA_KAN  10
#define GA_ETX  11
#define GA_US	12
#define GA_OK	13
#define GA_CONF 14
#define GA_CSUM 15

/////////////////////////////////////////////////////////////////////////////
// CGAUnitApp

BEGIN_MESSAGE_MAP(CGAUnitApp, CWinApp)
	//{{AFX_MSG_MAP(CGAUnitApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGAUnitApp construction

CGAUnitApp::CGAUnitApp()
{
	m_pInput = NULL;
	m_pSerialCom = NULL;


	m_nStatus = GA_STX;
	m_nOffset = 0;
	m_nInterface = 0;	// 1..4
	m_nCamera = 0;		// 1..2
	m_dwAlarmMask = 0;
	m_byXORCheckSum = 0;

	m_iCOMNumber = 0;
	m_bTraceiSTData = TRUE;
	m_bTraceRS232Data = TRUE;
	m_bTraceRS232DataHex = FALSE;
	m_bTraceRS232DataAscii = TRUE;
	m_bTraceRS232Events = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CGAUnitApp object

CGAUnitApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CGAUnitApp initialization

BOOL CGAUnitApp::InitInstance()
{
	if (WK_ALONE(WK_APP_NAME_GAUUNIT)==FALSE) 
	{
		return FALSE;
	}

	CString sTitle;
	sTitle = COemGuiApi::GetApplicationName(WAI_GAUNIT);
	m_pszAppName = _tcsdup(sTitle);    // human readable title

	InitDebugger("GAUnit.log",WAI_GAUNIT);

	WK_STAT_LOG("Reset",1,"ServerActive");

	WK_PRINT_VERSION(12, 5, 15, 53, "12/05/99 , 15:53\0", // @AUTO_UPDATE
		0);	// DO NOT TOUCH THIS LINE

	// Dongle vorhanden?
	CWK_Dongle dongle(WAI_GAUNIT); 
	if (dongle.IsExeOkay()==FALSE)
	{
		return FALSE;
	}

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
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
	wndclass.lpszClassName = WK_APP_NAME_GAUUNIT;

	AfxRegisterClass(&wndclass);

	CGAWindow *pWindow = new CGAWindow();
	m_pMainWnd = (CWnd*)pWindow;

	LoadDebugConfiguration();

	// create Input object
	m_pInput = new CGAInput(SM_GAUNIT_INPUT);

	// serielle Schnittstelle
	CWK_Profile wkp;
	m_sCurrency = wkp.GetString("GAA","Currency","");
	m_iCOMNumber = wkp.GetInt("GAA","COM",2);
	m_pSerialCom = new CGaRS232(m_iCOMNumber,
								m_bTraceRS232DataAscii,
								m_bTraceRS232DataHex,
								m_bTraceRS232Events);

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CGAUnitApp::LoadDebugConfiguration()
{
	// Einlesen der cfg-Datei
	CWKDebugOptions debugOptions;
	// check for C:\SDIUnit.cfg first, then in current directory
	BOOL bFoundFile = debugOptions.ReadFromFile("c:\\GaaDebug.cfg");
	if (bFoundFile==FALSE) {
		bFoundFile = debugOptions.ReadFromFile("GaaDebug.cfg");
	}

	// Setzen der Variablen
	m_bTraceiSTData = debugOptions.GetValue("TraceiSTData",m_bTraceiSTData);
	m_bTraceRS232DataHex = debugOptions.GetValue("TraceRS232DataHex",m_bTraceRS232DataHex);
	m_bTraceRS232DataAscii = debugOptions.GetValue("TraceRS232DataAscii",m_bTraceRS232DataAscii);
	m_bTraceRS232Events = debugOptions.GetValue("TraceRS232Events",m_bTraceRS232Events);
	int iTraceMilliSeconds = debugOptions.GetValue("TraceMilliSeconds",0);
	if (0 > iTraceMilliSeconds) {
		SET_WK_STAT_TICK_COUNT(GetTickCount());
	}
	else {
		SET_WK_STAT_TICK_COUNT((DWORD)iTraceMilliSeconds);
	}

	// TraceData auf mehreren Moeglichkeiten
	m_bTraceRS232Data = m_bTraceRS232DataHex || m_bTraceRS232DataAscii;

	// Im Debug-Modus einige Werte standartmaessig TRUE setzen
#ifdef _DEBUG
	WK_TRACE("===> Debug-Version <===\n");
#endif

	if (0 < GET_WK_STAT_TICK_COUNT()) {
		WK_TRACE("WK_STAT_TICK_COUNT=%ld\n",GET_WK_STAT_TICK_COUNT());
	}
	WK_TRACE("TraceiSTData=%d\n",m_bTraceiSTData);
	WK_TRACE("TraceRS232DataHex=%d\n",m_bTraceRS232DataHex);
	WK_TRACE("TraceRS232DataAscii=%d\n",m_bTraceRS232DataAscii);
	WK_TRACE("TraceRS232Events=%d\n",m_bTraceRS232Events);

	Sleep(100);
}
/////////////////////////////////////////////////////////////////////////////
int CGAUnitApp::ExitInstance() 
{
	WK_DELETE(m_pSerialCom);
	WK_DELETE(m_pInput);
	WK_STAT_LOG("Reset",0,"ServerActive");
	CloseDebugger();
	return m_msgCur.wParam; // returns the value from PostQuitMessage
}
/////////////////////////////////////////////////////////////////////////////
int CGAUnitApp::OnConnect()
{
	if (m_pSerialCom)
	{
		if (m_pSerialCom->Open(CBR_9600, 8, NOPARITY, ONESTOPBIT))
		{
			return 0;
		}
	}
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
void CGAUnitApp::AlarmReset()
{
	m_dwAlarmMask = 0L;
	m_Data.Clear();
	m_pInput->IndicateAlarm(m_dwAlarmMask,NULL);
}
/////////////////////////////////////////////////////////////////////////////
void CGAUnitApp::OnDisconnect()
{
	if (m_pSerialCom)
	{
		m_pSerialCom->Close();
	}
}
/////////////////////////////////////////////////////////////////////////////
/*
int	CGAUnitApp::CheckAndStoreOneData(LPBYTE lpB, char* pB, int nMax)
{
	int j;
	int i = 0;

	if (m_nOffset == 0)
		if (lpB[i] == ASCII_US)
			i++;
		else
			{
			ErrorHandler(GA_US);
			return 0;
			}

	for (j=m_nOffset; (j<nMax) && (lpB[i]) ; j++, i++)
	{
		pB[j] = lpB[i];
		m_byXORCheckSum = (BYTE)(m_byXORCheckSum ^ lpB[i]);
	}

	if (j==nMax)
	{
		m_nStatus++;
		m_nOffset = 0;
	}
	else
	{
		m_nOffset = j;
	}

	return i;
}
*/
int	CGAUnitApp::CheckAndStoreOneData(LPBYTE lpB, char* pB, int nMax)
{
	int i = 0;

	if (m_nOffset == 0)
	{
		if (lpB[i] == ASCII_US)
		{
			m_byXORCheckSum = (BYTE)(m_byXORCheckSum ^ lpB[i]);
			i++;
			m_nOffset++;
		}
		else
		{
			ErrorHandler(GA_US);
			return 0;
		}
	}

	while ((m_nOffset<=nMax) && (lpB[i]))
	{
		pB[m_nOffset-1] = lpB[i];
		m_byXORCheckSum = (BYTE)(m_byXORCheckSum ^ lpB[i]);
		i++;
		m_nOffset++;
	}

	if (m_nOffset > nMax)
	{
		m_nStatus++;
		m_nOffset = 0;
	}

	return i;
}
/////////////////////////////////////////////////////////////////////////////
void CGAUnitApp::CheckAndStoreReadData(LPBYTE lpB, DWORD dwLen)
{
	DWORD i = 0;

	while (i<dwLen)
	{
		switch (m_nStatus)
		{
		// -------- STX Startsymbol	1 Byte --------------------------------
		case GA_STX:	
			if (lpB[i] == ASCII_STX)
			{
				i++;
				m_nStatus = GA_IFN;
				m_nOffset = 0;
				m_byXORCheckSum = 0;
			}
			else
				i++;
			break;

		// ----------------- Schnittstellennummer 1 Byte -------------------
		case GA_IFN:
			if ( (lpB[i]>=0x30) && (lpB[i]<=0x33) )
			{
				m_nInterface = lpB[i] - 0x2F;
				m_byXORCheckSum = (BYTE)(m_byXORCheckSum ^ lpB[i]);
				i++;
				m_nStatus = GA_DATE;
				m_nOffset = 0;
			}
			else if (lpB[i] == ASCII_SPACE)
			{
				// space -> update
				m_nInterface = 0;
				m_byXORCheckSum = (BYTE)(m_byXORCheckSum ^ lpB[i]);
				i++;
				m_nStatus = GA_DATE;
				m_nOffset = 0;
			}
			else
				ErrorHandler(GA_IFN);
			break;

		// ------------------ Datum 6 Byte ----------------------
		case GA_DATE:
			i += CheckAndStoreOneData(&lpB[i],&m_Data.m_Datum[0],6);
			break;

		// -------------------- Zeit 4 Byte -------------------
		case GA_TIME:
			i += CheckAndStoreOneData(&lpB[i],&m_Data.m_Zeit[0],4);
			break;

		// ----------------- Transaktionsnummer 4 Byte --------------------
		case GA_TAN:
			i += CheckAndStoreOneData(&lpB[i],&m_Data.m_Transaktion[0],4);
			break;

		// ------------ Automatennummer 6 Byte -------------------
		case GA_AN:
			i += CheckAndStoreOneData(&lpB[i],&m_Data.m_Automat[0],6);
			break;

		// ------------ Bankleitzahl 8 Byte -----------------------
		case GA_BLZ:
			i += CheckAndStoreOneData(&lpB[i],&m_Data.m_Bankleitzahl[0],8);
			break;

		// ----------- Kontonummer 10 Byte -----------------------
		case GA_KTN:
			i += CheckAndStoreOneData(&lpB[i],&m_Data.m_Kontonummer[0],10);
			break;

		// --------------- GeldBetrag 4 Byte -------------------------
		case GA_BETR:
			i += CheckAndStoreOneData(&lpB[i],&m_Data.m_Betrag[0],4);
			break;

		// -----------------Kameranummer 1 Byte ------------------------
		case GA_KAN:
			if (m_nOffset == 0)
			{
				if (lpB[i] == ASCII_US)
				{
					m_byXORCheckSum = (BYTE)(m_byXORCheckSum ^ lpB[i]);
					i++;
				}
				else
				{
					ErrorHandler(GA_US);
					break;
				}
			}

			if (lpB[i]=='\0')
			{
				m_nOffset = 1;
				break;
			}
			
			if ( (lpB[i]>=0x30) && (lpB[i]<=0x32) )
			{
				// 0 -> update
				// 1 -> camera nr. 1
				// 2 -> camera nr. 2
				m_nCamera = lpB[i] - 0x30;
				m_byXORCheckSum = (BYTE)(m_byXORCheckSum ^ lpB[i]);
				i++;
				m_nStatus = GA_ETX;
				m_nOffset = 0;
			}
			else if (lpB[i] == ASCII_SPACE)
			{
				// space -> update
				m_nCamera = 0;
				m_byXORCheckSum = (BYTE)(m_byXORCheckSum ^ lpB[i]);
				i++;
				m_nStatus = GA_ETX;
				m_nOffset = 0;
			}
			else
			{
				ErrorHandler(GA_KAN);
			}
			break;

		// ------------- <ETX> Symbol 1 Byte ---------
		case GA_ETX:
			if (lpB[i] == ASCII_ETX)
			{
				m_byXORCheckSum = (BYTE)(m_byXORCheckSum ^ lpB[i]);
				i++;
				m_nStatus = GA_CSUM;
			}
			else
			{
				ErrorHandler(GA_ETX);
			}
			break;

		case GA_CSUM:
			m_nStatus = GA_OK;
			m_nOffset = 0;

			TRACE("Prüfsumme Ist %02x Soll %02x\n", m_byXORCheckSum, lpB[i]);
			if (m_byXORCheckSum != lpB[i])
			{
				WK_TRACE("Prüfsumme fehlerhaft %02x != %02x\n", m_byXORCheckSum, lpB[i]);
			}
			if (lpB[i] == ASCII_STX)	// checksummme kann startsymbol sein
			{
				i++;
			}
			break;

		default:
			break;
		} // switch

		if (m_nStatus == GA_OK)
		{
			ConfirmCom();
			TransmitDataToSecurity();
			m_nStatus = GA_STX;			
		}
	} // while
}
/////////////////////////////////////////////////////////////////////////////
void CGAUnitApp::ConfirmCom(BOOL bSuccess /*= TRUE*/)
{
	char szConfirm[3];
	DWORD dwL = 3;

	szConfirm[0] = ASCII_STX;
	if (bSuccess) {
		szConfirm[1] = ASCII_ACK;
	}
	else {
		szConfirm[1] = ASCII_NAK;
	}
	szConfirm[2] = ASCII_ETX;
	
	DWORD dwWritten = m_pSerialCom->Write(szConfirm,dwL);
	if (dwL != dwWritten)
	{
		WK_TRACE_ERROR("cannot write ack to COM%\n",m_iCOMNumber);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CGAUnitApp::TransmitDataToSecurity()
{
	int	nAlarm;
	int i,max,off;  

	m_Data.Validate();
	m_Data.SetTime();

	if ((m_nCamera!=0) && (m_nInterface!=0))
	{
		// no update new data no browse
		nAlarm = (m_nInterface-1) * 2 + m_nCamera;
		m_dwAlarmMask = 1 << (nAlarm-1);
		m_Datas[nAlarm-1] = m_Data;
		WK_TRACE("Alarm DTPMelder%d, Schnittstelle %d, Kamera %d\n",nAlarm,m_nInterface,m_nCamera);
		m_Data.Trace();
		m_pInput->IndicateAlarm(m_dwAlarmMask, m_Data.Format());
		Sleep(10);
		AlarmReset();
	}
	else
	{
		if ((m_nCamera==0) && (m_nInterface!=0))
		{
			// camera update
			CTime t1,t2;

			i = (m_nInterface-1) * 2;
			t1 = m_Datas[i].GetTime();
			t2 = m_Datas[i + 1].GetTime();
			max = (t1>t2) ? i : i+1;
		}
		else if ((m_nCamera!=0) && (m_nInterface==0))
		{
			// interface update
			CTime t1,t2;

			off = m_nCamera - 1;

			t1 = m_Datas[off].GetTime();
			max = off;
			for (i=1;i<4;i++)
			{
				t2 = m_Datas[i*2+off].GetTime();
				if (t2>t1)
				{
					max = i*2 + off;
				}
			}
		}
		else
		{
			// both
			CTime t1,t2;

			t1 = m_Datas[0].GetTime();
			max = 0;
			for (i=1;i<8;i++)
			{
				t2 = m_Datas[i].GetTime();
				if (t2>t1)
				{
					max = i;
				}
			}
		}
		m_Datas[max] = m_Data;
		WK_TRACE("Alarm Update DTPMelder%d, Schnittstelle %d, Kamera %d\n",max+1,m_nInterface,m_nCamera);
		if (m_bTraceiSTData) {
			m_Data.Trace();
		}

		WK_ASSERT(max <= USHRT_MAX);
		m_pInput->UpdateAlarm((WORD)max, m_Data.Format());
	}
}
/////////////////////////////////////////////////////////////////////////////
void CGAUnitApp::ErrorHandler(int nErrorCode)
{
	CString sError;

	sError.LoadString(IDS_STX + nErrorCode - 1);
	WK_TRACE_ERROR("%s\n",(const char*)sError);
	
	ConfirmCom(FALSE);
	m_Data.Clear();
	m_nInterface = 0;
	m_nCamera = 0;
	m_nStatus = GA_STX;
}
