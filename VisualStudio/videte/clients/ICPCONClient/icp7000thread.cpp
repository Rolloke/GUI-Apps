// ICP7000Thread.cpp : implementation file
//

#include "stdafx.h"
#include "ICPCONClient.h"
#include "ICP7000Thread.h"
#include "SwitchPanel.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CICPI7000Module class
CICPI7000Module::CICPI7000Module()
{
	ZeroMemory(m_w7000, sizeof(WORD)*ARRAY_SIZE);
	m_w7000[4]   = 100; // timeout value
	m_wDImask    = 0;
	m_wDIstate   = 0;
	m_wDIoffset  = 0;
	m_wDOmask    = 0;
	m_wDOoffset  = 0;
	m_wDOstate   = 0;
	m_wDOlevel   = 0;
	m_dwBaudrate = 9600;
	m_nReference = 0;
	m_nStartID   = 0;
	m_wFlags     = 0;
	m_wFailSeconds= 0;
}
/////////////////////////////////////////////////////////////////////////////
CICPI7000Module::~CICPI7000Module()
{
	ASSERT(m_nReference == 0);
	if (IsOpened())
	{
		Close_Com(GetComPort());
	}
}
/////////////////////////////////////////////////////////////////////////////
int CICPI7000Module::Release()
{
	int nReference = (--m_nReference);

	if (nReference == 0)
	{
		delete this;
	}
	return nReference;
}
/////////////////////////////////////////////////////////////////////////////
bool CICPI7000Module::SetModuleID(const char *pszModuleID)
{
	char *stopChar;
	char *pstr = strstr(pszModuleID, "7");
	if (pstr)
	{
		char c  = pstr[4];
		m_sItemID = _T(pstr);
		pstr[4] = 0;
		m_w7000[2] = (WORD)strtoul(pstr, &stopChar, 16);
		pstr[4] = c;
		switch (m_w7000[2])
		{
			case 0x7041:
				m_sDIOmode = _T("DI(8-13), DI(0-7)");
				m_wDImask  = 0x3fff;
				break;
			case 0x7042:
				m_sDIOmode = _T("DO(8-12), DO(0-7)");
				m_wDOmask  = 0x1fff;
				break;
			case 0x7043:
				m_sDIOmode = _T("DO(8-15), DO(0-7)");
				m_wDOmask  = 0xffff;
				break;
			case 0x7044:
				m_sDIOmode = _T("DO(1-8), DI(1-4)"); //  prüfen
				m_wDImask  = 0x000f;
				m_wDIoffset= 1;
				m_wDOmask  = 0x00ff;
				m_wDOoffset= 1;
				break;
			case 0x7050:
				m_sDIOmode = _T("DO(0-7), DI(0-6)");	// prüfen
				m_wDImask  = 0x003f;
				m_wDOmask  = 0x00ff;
				break;
			case 0x7052:
				m_sDIOmode = _T("DI(0-7)");
				m_wDImask  = 0x00ff;
				break;
			case 0x7053:
				m_sDIOmode = _T("DI(8-15), DI(0-7)");
				m_wDImask  = 0xffff;
				break;
			case 0x7060:
				m_sDIOmode = _T("DO(1-4), DI(1-4)");	// prüfen
				m_wDImask  = 0x000f;
				m_wDIoffset= 1;
				m_wDOmask  = 0x000f;
				m_wDOoffset= 1;
				break;
			case 0x7063:
				m_sDIOmode = _T("DO(1-3), DI(1-8)");	// prüfen
				m_wDImask  = 0x00ff;
				m_wDIoffset= 1;
				m_wDOmask  = 0x0007;
				m_wDOoffset= 1;
				break;
			case 0x7065:
				m_sDIOmode = _T("DO(1-5), DI(1-4)");	// prüfen
				m_wDImask  = 0x000f;
				m_wDIoffset= 1;
				m_wDOmask  = 0x001f;
				m_wDOoffset= 1;
				break;
			case 0x7066:
				m_sDIOmode = _T("DO(1-7)");
				m_wDOmask  = 0x007f;
				m_wDOoffset= 1;
				break;
			case 0x7067:
				m_sDIOmode = _T("DO(1-7)");
				m_wDOmask  = 0x007f;
				m_wDOoffset= 1;
				break;
		}

		return true;
	}
	return false;
}
/////////////////////////////////////////////////////////////////////////////
bool CICPI7000Module::SetConfig(char *szConfig)
{
	if (szConfig[3] == '4' && szConfig[4] == '0')
	{
		char *stopChar;
		if (szConfig[5] == '0')
		{
			switch(szConfig[6])
			{
				case '3': m_dwBaudrate =   1200; break;
				case '4': m_dwBaudrate =   2400; break;
				case '5': m_dwBaudrate =   4800; break;
				case '6': m_dwBaudrate =   9600; break;
				case '7': m_dwBaudrate =  19200; break;
				case '8': m_dwBaudrate =  38400; break;
				case '9': m_dwBaudrate =  57600; break;
				case 'A': m_dwBaudrate = 115200; break;
			}
		}
		char cOld = szConfig[9];
		szConfig[9]  = 0;
		WORD wConfig = strtoul(&szConfig[7], &stopChar, 16);
		szConfig[9]  = cOld;
		SetChecksum((wConfig & 0x40) ? 1 : 0);
		return true;
	}
	return false;
}
/////////////////////////////////////////////////////////////////////////////
bool CICPI7000Module::OpenPort()
{
	if (!IsOpened())
	{
		CTime t1 = CTime::GetCurrentTime();
		WORD wRetValue = Open_Com(GetComPort(), m_dwBaudrate, 8, 0, 0);
		CTime t2 = CTime::GetCurrentTime();

		if (wRetValue == NoError)
		{
			m_wFailSeconds = 0;
			return true;
		}
		else
		{
			CTimeSpan ts = t2 - t1;
			if (ts.GetSeconds() > 1)
			{
				m_wFailSeconds = ts.GetSeconds();
			}
#ifdef _DEBUG
			WK_TRACE(_T("Error Open_Com(%d): %d, %s sec\n"), GetComPort(), (short)wRetValue, ts.Format(_T("%S")));
#endif			
			return false;
		}
	}
	return true;
}
/////////////////////////////////////////////////////////////////////////////
void CICPI7000Module::ClosePort()
{
	if (IsOpened())
	{
		Close_Com(GetComPort());
	}
}
/////////////////////////////////////////////////////////////////////////////
bool CICPI7000Module::IsOpened()
{
	WORD wStatus = ComPortStatus(GetComPort());
	if (wStatus == ComportOpened)
	{
		return true;
	}
   return false;
}
/////////////////////////////////////////////////////////////////////////////
WORD CICPI7000Module::DigitalOut()
{
	float f7000[10];
	char  szSendTo7000[80];
	char  szReceiveFrom7000[80];
	m_w7000[5] = m_wDOstate;
	return ::DigitalOut(m_w7000, f7000, szSendTo7000, szReceiveFrom7000);
}
/////////////////////////////////////////////////////////////////////////////
WORD CICPI7000Module::DigitalIn()
{
	float f7000[10];
	char  szSendTo7000[80];
	char  szReceiveFrom7000[80];
	WORD wRet = ::DigitalIn(m_w7000, f7000, szSendTo7000, szReceiveFrom7000);
	if (wRet==NoError)
	{
		m_wDIstate = m_w7000[5];
	}
	return wRet;
}
/////////////////////////////////////////////////////////////////////////////
WORD CICPI7000Module::DigitalOutReadBack()
{
	float f7000[10];
	char  szSendTo7000[80];
	char  szReceiveFrom7000[80];
	WORD wRet = ::DigitalOutReadBack(m_w7000, f7000, szSendTo7000, szReceiveFrom7000);
	if (wRet==NoError)
	{
		m_wDOstate = m_w7000[5];
	}
	return wRet;
}
/////////////////////////////////////////////////////////////////////////////
void CICPI7000Module::SetDObit(int nBit, BOOL bSet)
{
	if (bSet)
	{
		m_wDOstate |= (1 << nBit);
	}
	else
	{
		m_wDOstate &= ~(1 << nBit);
	}
}
/////////////////////////////////////////////////////////////////////////////
bool CICPI7000Module::GetDObit(int nBit)
{
	return (m_wDOstate & (1 << nBit)) ? true : false;
}
/////////////////////////////////////////////////////////////////////////////
void CICPI7000Module::SetDOlevel(int nBit, BOOL bSet)
{
	if (bSet)
	{
		m_wDOlevel |= (1 << nBit);
	}
	else
	{
		m_wDOlevel &= ~(1 << nBit);
	}
}
/////////////////////////////////////////////////////////////////////////////
bool CICPI7000Module::GetDOlevel(int nBit)
{
	return (m_wDOlevel & (1 << nBit)) ? true : false;
}
/////////////////////////////////////////////////////////////////////////////
void CICPI7000Module::SetDIbit(int nBit, BOOL bSet)
{
	if (bSet)
	{
		m_wDIstate |= (1 << nBit);
	}
	else
	{
		m_wDIstate &= ~(1 << nBit);
	}
}
/////////////////////////////////////////////////////////////////////////////
bool CICPI7000Module::GetDIbit(int nBit)
{
	return (m_wDIstate & (1 << nBit)) ? true : false;
}
/////////////////////////////////////////////////////////////////////////////
int CICPI7000Module::GetOutputs()
{
	int i, n;
	for (i=0, n=1; i<16; i++, n <<= 1)
	{
		if (!(m_wDOmask & n))
			break;
	}
	return i;
}
/////////////////////////////////////////////////////////////////////////////
int CICPI7000Module::GetInputs()
{
	int i, n;
	for (i=0, n=1; i<16; i++, n <<= 1)
	{
		if (!(m_wDImask & n))
			break;
	}
	return i;
}
/////////////////////////////////////////////////////////////////////////////
bool CICPI7000Module::GetOutput(int nItem)
{
	return (m_wDOmask & (1 << nItem)) ? true : false;
}
/////////////////////////////////////////////////////////////////////////////
bool CICPI7000Module::GetInput(int nItem)
{
	return (m_wDImask & (1 << nItem)) ? true : false;
}
/////////////////////////////////////////////////////////////////////////////
void CICPI7000Module::SetPollDO(bool bPoll)
{
	if (bPoll) m_wFlags |=  0x0001;
	else       m_wFlags &= ~0x0001;
}
/////////////////////////////////////////////////////////////////////////////
void CICPI7000Module::SetPollDI(bool bPoll)
{
	if (bPoll) m_wFlags |=  0x0002;
	else       m_wFlags &= ~0x0002;
}
/////////////////////////////////////////////////////////////////////////////
void CICPI7000Module::SetDOstateChanged(bool bChanged)
{
	if (bChanged) m_wFlags |=  0x0004;
	else          m_wFlags &= ~0x0004;
}
/////////////////////////////////////////////////////////////////////////////
bool CICPI7000Module::DoPollDO()
{
	return (m_wFlags & 0x0001) ? true : false;
}
/////////////////////////////////////////////////////////////////////////////
bool CICPI7000Module::DoPollDI()
{
	return (m_wFlags & 0x0002) ? true : false;
}
/////////////////////////////////////////////////////////////////////////////
bool CICPI7000Module::IsDOstateChanged()
{
	return (m_wFlags & 0x0004) ? true : false;
}

/////////////////////////////////////////////////////////////////////////////
// CICPI7000Thread

IMPLEMENT_DYNCREATE(CICPI7000Thread, CWinThread)

CWK_PerfMon *CICPI7000Thread::gm_pPerformanceMonitor = NULL;

CICPI7000Thread::CICPI7000Thread()
{
	m_nPort            = 0;
	m_pSwitchPanel     = NULL;
	m_bAutoDelete      = FALSE;
	m_bPolling         = false;
	m_bMultipleAccess  = false;
	m_nFailcount       = 0;
	m_bDoPoll          = false;
}

 CICPI7000Thread::CICPI7000Thread(int nPort, void *pModule, CSwitchPanel *pPanel, bool bMultipleAccess)
{
	m_bAutoDelete      = FALSE;
	m_bPolling         = false;
	m_nFailcount       = 0;
	m_bDoPoll          = false;
	m_nPort            = nPort;
	m_pSwitchPanel     = pPanel;
	m_bMultipleAccess  = bMultipleAccess;
	AddModule(pModule);
}

CICPI7000Thread::~CICPI7000Thread()
{
}

BOOL CICPI7000Thread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int CICPI7000Thread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CICPI7000Thread, CWinThread)
	//{{AFX_MSG_MAP(CICPI7000Thread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CICPI7000Thread message handlers

int CICPI7000Thread::Run() 
{
	MSG msg;
	if (m_pSwitchPanel == NULL)
		return 0;

   while(GetMessage(&msg, NULL, 0, 0)) 
	{
		switch(msg.message)
		{
			case WM_TIMER:
				OnTimer((UINT)msg.wParam, msg.lParam);
				break;
			case WM_COMMAND:
				OnCommand(LOWORD(msg.wParam), HIWORD(msg.wParam), (HWND)msg.lParam);
				break;
			case WM_CHECK_SKINBUTTON:
				OnCheckSkinButton(msg.wParam, (CSkinButton*)msg.lParam);
				break;
		}
	};
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CICPI7000Thread::OnTimer(UINT nIDEvent, UINT nCount)
{
#ifdef _DEBUG
	CString str;
	str.Format(_T("OnTimer(COM%d)"), m_nPort);
	CThreadDebug td(str);
#endif
	if (nIDEvent == m_pSwitchPanel->m_nPollTimer)
	{
		if (gm_pPerformanceMonitor)
		{
			gm_pPerformanceMonitor->Start();
		}
		SetPollState(TRUE);
		int  nID;
		HWND hwnd;
		long lUserData;
		CICPI7000Module *pFirstModule = NULL;
		CICPI7000Module *pModule      = NULL;
		WORD  wRet;

		for (nID=IDC_RELAIS; nID<=IDC_RELAIS_LAST; nID++)
		{
			hwnd = ::GetDlgItem(m_pSwitchPanel->m_hWnd, nID);
			if (hwnd)
			{
				lUserData = ::GetWindowLong(hwnd, GWL_USERDATA);
				if (!lUserData) continue;
				if (lUserData != (long)pModule)
				{
					int   i = 0;
					wRet    = NoError;
					pModule = (CICPI7000Module*)lUserData;
					if (m_nPort != (int)pModule->GetComPort())		// Ein Thread gehört zu einem Port
					{
						pModule = NULL;
						continue;
					}
					
					if (pFirstModule == NULL)								// Das erste Modul
					{
						pFirstModule = pModule;
						while (!pModule->OpenPort())						// öffnet den Port
						{
							if (i++ > 10)										// nach 10 Fehlschlägen
							{
								if (DoPoll())									// wenn der Thread gepollt wird
								{
									SetFailCount(5);							// 5 Timerzyklen aussetzen
								}
								wRet = PortError;
								break;
							}
							if (pModule->GetFailSeconds())				// ist der Port nicht erreichbar (Timeout)
							{
								if (DoPoll())									//  wenn der Thread gepollt wird
								{
									int nTime = MulDiv(pModule->GetFailSeconds(), 1000, m_pSwitchPanel->m_nPollTime);
									if (nTime <= 0) nTime = 1;
									SetFailCount(nTime*2);					// zwei Timeoutperioden aussetzen
								}
								wRet = PortError;
								break;
							}
							Sleep(m_pSwitchPanel->m_nTimeout + 20);	// Wartezeit nach Fehlschlag
						}
					}
					
					if (wRet == NoError)										// Modulzustand abfragen
					{
						m_pSwitchPanel->Lock();
						if (pModule->HasInputs())
						{
							wRet = pModule->DigitalIn();
						}
						if (pModule->IsDOstateChanged())
						{
							pModule->DigitalOut();
						}
						else if (pModule->HasOutputs())
						{
							wRet = pModule->DigitalOutReadBack();
						}

						if (wRet != NoError)									// Bei Fehler
						{
							if (wRet == TimeOut)								// Timeout !
							{
								if (DoPoll())									// wenn der Thread gepollt wird
								{
									SetFailCount(5);							// 5 Timerzyklen aussetzen
								}
							}
							WK_TRACE(_T("Error: %d, DigitalIn/DigitalOutReadBack\n"), wRet);
						}
						m_pSwitchPanel->Unlock();
					}
				}
				
				if (wRet == NoError)											// Buttons benachrichtigen
				{
					BOOL bCheck = FALSE;
					if      (pModule->HasOutputs()) bCheck = pModule->GetDObit(nID - pModule->m_nStartID) ? TRUE : FALSE;
					else if (pModule->HasInputs())  bCheck = pModule->GetDIbit(nID - pModule->m_nStartID) ? TRUE : FALSE;
					m_pSwitchPanel->PostMessage(WM_CHECK_SKINBUTTON, nID, bCheck);
				}
				else
				{
					m_pSwitchPanel->PostMessage(WM_CHECK_SKINBUTTON, nID, -1);
				}
			}

			if (m_bMultipleAccess && pFirstModule)						// Das erste Modul schließt den Port wieder
			{
				pFirstModule->ClosePort();
			}
		}
		SetPollState(FALSE);
		if (gm_pPerformanceMonitor)
		{
			gm_pPerformanceMonitor->Stop();
			gm_pPerformanceMonitor->ShowResults();
		}

		//		Sleep(500); test für Haupt Timer
		BOOL bForce = HIWORD(nCount);										// nächsten Thread (Port)
		nCount = LOWORD(nCount);										   // benachrichtigen
		nCount++;
		if (nCount >= (UINT)m_pSwitchPanel->m_ThreadList.GetCount())// letzter Thread
		{																			// Fertig an das Steuerfenster senden
			m_pSwitchPanel->PostMessage(WM_USER, USER_MSG_POLL_THREADS_READY, nCount);
		}
		POSITION pos = m_pSwitchPanel->m_ThreadList.FindIndex(nCount);
		while (pos)
		{
			CICPI7000Thread *pThread = (CICPI7000Thread*)m_pSwitchPanel->m_ThreadList.GetAt(pos);
			if (pThread)
			{
				if (   (!pThread->IsPolling()								// Thread pollt nicht gerade oder
					 ||  !pThread->HasFailed())							// muß nicht aufgrund von Fehlern warten und
					 && (pThread->DoPoll()									// dieser Thread soll gepollt werden
					 || bForce))												// oder alle sollen gepollt werden
				{
					pThread->PostThreadMessage(WM_TIMER, nIDEvent, nCount);
					break;
				}
				else																// sonst den nächsten nehmen
				{
					pos = m_pSwitchPanel->m_ThreadList.FindIndex(++nCount);
				}
				if (pos == NULL)												// kein weiterer vorhanden
				{																	// Fertig an das Steuerfenster senden
					m_pSwitchPanel->PostMessage(WM_USER, USER_MSG_POLL_THREADS_READY, nCount);
				}
			}
		}
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CICPI7000Thread::OnCommand(WORD wID, WORD wNotify, HWND hwndFrom)
{
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CICPI7000Thread::StopThread()
{
	PostThreadMessage(WM_QUIT, 0, 0);
	WaitForSingleObject(m_hThread, INFINITE);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CICPI7000Thread::OnCheckSkinButton(UINT nID, CSkinButton *pSkinBtn)
{
#ifdef _DEBUG
	CThreadDebug td(_T("OnCheckSkinButton"));
#endif
	CICPI7000Module *pModule = (CICPI7000Module*) ::GetWindowLong(pSkinBtn->m_hWnd, GWL_USERDATA);
	if (pModule)
	{
		int   i=0, nItem = nID - pModule->m_nStartID;

		WORD wOldState = pModule->m_wDOstate;

		pModule->SetDObit(nItem, pSkinBtn->GetCheck());

		while (IsPolling())
		{
			Sleep(100);
		}
		SetPollState(TRUE);

		WORD wRet = NoError;
		while (!pModule->OpenPort())
		{
			if (i++ > 10)
			{
				wRet = PortError;
				break;
			}
			Sleep(180);
		}
		
		if (wRet == NoError)
		{
			if (pModule->HasOutputs())
			{
				wRet = pModule->DigitalOut();
			}
			else
			{
				WK_TRACE(_T("Pressed Input indicator\n"), wRet);
			}

			if (wRet != NoError)									// bei Fehler
			{
				WK_TRACE(_T("Error: %d, DigitalOut\n"), wRet);
				pModule->m_wDOstate = wOldState;				// alten Zustand setzen
				m_pSwitchPanel->PostMessage(WM_CHECK_SKINBUTTON, nID, pModule->GetDObit(nItem));
			}
			else
			{
				if (pModule->HasOutputs() && !pModule->GetDOlevel(nItem) && pModule->GetDObit(nItem))
				{
					Sleep(m_pSwitchPanel->m_nTimeoutEdge);
					pModule->SetDObit(nItem, FALSE);
					wRet = pModule->DigitalOut();
					if (wRet == NoError)
					{
						m_pSwitchPanel->PostMessage(WM_CHECK_SKINBUTTON, nID, pModule->GetDObit(nItem));
					}
				}
			}

		}
		if (m_bMultipleAccess)
		{
			pModule->ClosePort();
		}
		SetPollState(FALSE);
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CICPI7000Thread::AddModule(void *pModule)
{
	m_Modules.AddHead(pModule);
	if (((CICPI7000Module*)pModule)->DoPollDO() || ((CICPI7000Module*)pModule)->DoPollDI())
	{
		m_bDoPoll = true;
	}
}
/////////////////////////////////////////////////////////////////////////////
bool CICPI7000Thread::ContainsModule(void *pModule)
{
	return (m_Modules.Find(pModule) != NULL) ? true : false;
}
/////////////////////////////////////////////////////////////////////////////
void CICPI7000Thread::SetPollState(BOOL bPolling)
{
	m_pSwitchPanel->Lock();
	m_bPolling = bPolling;
	m_pSwitchPanel->Unlock();

}
/////////////////////////////////////////////////////////////////////////////
BOOL CICPI7000Thread::IncrementPollState()
{
	BOOL bPolling = FALSE;
	m_pSwitchPanel->Lock();
	if (m_bPolling)
	{
		m_bPolling++;
		bPolling = m_bPolling;
	}
	m_pSwitchPanel->Unlock();
	return bPolling;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CICPI7000Thread::IsPolling()
{ 
	BOOL bPolling;
	m_pSwitchPanel->Lock();
	bPolling = m_bPolling;
	m_pSwitchPanel->Unlock();
	return bPolling;
};
/////////////////////////////////////////////////////////////////////////////
bool CICPI7000Thread::HasFailed()
{
	if (m_nFailcount)
	{
		TRACE(_T("%d\n"), m_nFailcount);
		m_nFailcount--;
		return true;
	}
	return false;
}
/////////////////////////////////////////////////////////////////////////////
void CICPI7000Thread::DeletePerformanceMonitor()
{
	WK_DELETE(gm_pPerformanceMonitor);
}
/////////////////////////////////////////////////////////////////////////////
void CICPI7000Thread::CreatePerformanceMonitor()
{
	ASSERT(gm_pPerformanceMonitor == NULL);
	gm_pPerformanceMonitor = new CWK_PerfMon(_T("Polling Time"));
}
/////////////////////////////////////////////////////////////////////////////
