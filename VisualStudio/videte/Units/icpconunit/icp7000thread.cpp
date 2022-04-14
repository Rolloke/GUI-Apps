/////////////////////////////////////////////////////////////////////////////
// PROJECT:		ICPCONUnit
// FILE:		$Workfile: icp7000thread.cpp $
// ARCHIVE:		$Archive: /Project/Units/ICPCONUnit/icp7000thread.cpp $
// CHECKIN:		$Date: 24.05.05 16:16 $
// VERSION:		$Revision: 34 $
// LAST CHANGE:	$Modtime: 24.05.05 15:07 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ICPCONUnit.h"
#include "ICPCONUnitDlg.h"
#include "ICP7000Thread.h"
#include "i7000.h"
#include "ICPCONDll\ICP7000Module.h"

#include "wkclasses\wk_utilities.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CICPCONUnitApp theApp;

#if (_TEST_TRACES == 1)
   #define TEST_TRACE WK_TRACE
#endif

#if (_TEST_TRACES == 2)
   #define TEST_TRACE theApp.OutputDebug
#endif

#ifndef _TEST_TRACES
	#define TEST_TRACE //
#endif

/////////////////////////////////////////////////////////////////////////////
// CICPI7000Thread

IMPLEMENT_DYNCREATE(CICPI7000Thread, CWinThread)

CWK_PerfMon *CICPI7000Thread::gm_pPerformanceMonitor = NULL;

CICPI7000Thread::CICPI7000Thread()
{
	m_nPort            = 0;
	m_pDialog			 = NULL;
	m_bAutoDelete      = FALSE;
	m_bPolling         = false;
	m_bMultipleAccess  = false;
	m_nFailcount       = 0;
	m_bDoPoll          = false;
	m_dwSuspendCount   = 0;
}

CICPI7000Thread::CICPI7000Thread(int nPort, void *pModule, CICPCONUnitDlg *pPanel, bool bMultipleAccess)
{
	m_bAutoDelete      = FALSE;
	m_bPolling         = false;
	m_nFailcount       = 0;
	m_bDoPoll          = false;
	m_nPort            = nPort;
	m_pDialog		   = pPanel;
	m_bMultipleAccess  = bMultipleAccess;
	m_dwSuspendCount   = 0;
	AddModule(pModule);
	WK_TRACE(_T("CICPI7000Thread(%d): %d\n"), nPort, bMultipleAccess);
}

CICPI7000Thread::~CICPI7000Thread()
{
	while (m_Modules.GetCount())
	{
		CICPI7000Module*pModule = (CICPI7000Module*) m_Modules.RemoveHead();
		pModule->Release();
	}
}

BOOL CICPI7000Thread::InitInstance()
{
	CString str;
	str.Format(_T("CICPI7000Thread(%d)"), m_nPort);
	XTIB::SetThreadName(_tcsdup(str));
	
	return TRUE;
}

int CICPI7000Thread::ExitInstance()
{
	XTIB*pTib = XTIB::GetTIB();
	if (pTib->pvArbitrary)
	{
		free(pTib->pvArbitrary);
	}

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
	if (m_pDialog == NULL)
		return 0;

	WK_TRACE(_T("ICPCON Thread %x started\n"), m_nThreadID);
		
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
		}
	};
	
	WK_TRACE(_T("ICPCON Thread %x stopped\n"), m_nThreadID);
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CICPI7000Thread::OnTimer(UINT nIDEvent, UINT nCount)
{

#ifdef _DEBUG
//	CString str;
//	str.Format(_T("OnTimer(COM%d)"), m_nPort);
//	CThreadDebug td(str);
#endif

	if (nIDEvent == m_pDialog->m_nPollTimer)
	{
		if (gm_pPerformanceMonitor)
		{
			gm_pPerformanceMonitor->Start();
		}

		SetPollState(TRUE);
		int  i;
		CICPI7000Module *pModule      = NULL;
		WORD wRet;
		BOOL bVisible       = m_pDialog->IsWindowVisible(),
			  bPortIsOpen    = FALSE,
			  bErrorOccurred = FALSE,
			  bUpdate;

		POSITION pos;
		pos = m_Modules.GetHeadPosition();

		while (pos)
		{
			pModule = (CICPI7000Module*)m_Modules.GetNext(pos);
			wRet    = NoError;
			bUpdate = FALSE;
			if (pModule->HasFailed())
			{
				continue;
			}
			i = 0;
			if (!bPortIsOpen)
			{
				while (!pModule->OpenPort())							// öffnet den Port
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
					if (pModule->GetFailSeconds())						// ist der Port nicht erreichbar (Timeout)
					{
						if (DoPoll())									//  wenn der Thread gepollt wird
						{
							int nTime = MulDiv(pModule->GetFailSeconds(), 1000, m_pDialog->m_nPollTime);
							if (nTime <= 0) nTime = 1;
							SetFailCount(nTime*2);						// zwei Timeoutperioden aussetzen
						}
						wRet = PortError;
						pModule->SetFailCount(0);
						break;
					}
					Sleep(m_pDialog->m_nTimeout);						// Wartezeit nach Fehlschlag
				}
				bPortIsOpen = (wRet == NoError) ? TRUE : FALSE;
			}
			
			if (wRet == NoError)										// Modulzustand setzen
			{
				if (pModule->IsDOstateChanged())						// Zustand geändert ?
				{
//					TRACE(_T("TIMER: DigitalOut\n"));
					bUpdate = TRUE;
					wRet = pModule->DigitalOut();						// Zustand setzen
					if (wRet == NoError)								// Kein Fehler
					{
						pModule->ConfirmSetRelay(true);					// bestätigen
						pModule->SetPollDOonce(false);					// Zustand ist bekannt, erneute Abfrage (s.u.) ist unnötig
					}
					else												// Fehler
					{
						pModule->ConfirmSetRelay(false);				// nicht bestätigen
						CString str1, str2;
						str1.Format(_T("Error%x%x"), pModule->GetComPort(), pModule->GetModuleAddress());
						str2.Format(_T("%d, DigitalOut: %s"), wRet, pModule->GetModuleIDstring());
						WK_STAT_PEAK(str1, 1, str2);
					}
					pModule->SetDOstateChanged(false);
				}
			}

			if (wRet == NoError)										// Modulzustand abfragen
			{
				if (pModule->HasOutputs() && pModule->DoPollDO())
				{
//					TRACE(_T("TIMER: DigitalOutReadBack\n"));
					bUpdate = TRUE;
					wRet = pModule->DigitalOutReadBack();				// Zustand des Outputs abfragen
					if (wRet == NoError)								// kein Fehler
					{
						if (   pModule->DoConfirmCurrentState()			// wenn bestätigt werden soll
							 || pModule->DoPollDO())					// oder sowieso gepollt wird
						{
//							pModule->ConfirmSetRelay(true);				// bestätigen
							pModule->ConfirmCurrentState(OUTPUT_PINS);	// bestätigen
						}
					}
					else												// Fehler
					{
						if (!pModule->DoPollDO())						// wenn nicht gepollt wird
						{
							pModule->SetPollDOonce(true);				// erneut abfragen
						}
						CString str1, str2;
						str1.Format(_T("Error%x%x"), pModule->GetComPort(), pModule->GetModuleAddress());
						str2.Format(_T("%d, DigitalOutReadBack: %s"), wRet, pModule->GetModuleIDstring());
						WK_STAT_PEAK(str1, 1, str2);
					}
				}
			}
			if (wRet == NoError)										// Modulzustand abfragen
			{
				if (pModule->HasInputs() && pModule->DoPollDI())
				{
//					TRACE(_T("TIMER: DigitalIn\n"));
					WORD wOld = pModule->m_wDIstate;					// alten Zustand merken
					int nCount = 3;										// 3 Versuche
					bUpdate = TRUE;
					while (--nCount)
					{
						wRet = pModule->DigitalIn();					// Abfragen
						if (wRet == NoError)							// kein Fehler
						{
							if (pModule->DoConfirmCurrentState())		// wenn bestätigt werden soll
							{
								pModule->ConfirmCurrentState(INPUT_PINS);// bestätigen
							}
							else													
							{
								pModule->SetAlarmIfDifferent(wOld);		// Alarm indizieren
							}
							break;										// nicht nochmal abfragen
						}
						else											// Fehler
						{
							CString str1, str2;
							str1.Format(_T("Error%x%x"), pModule->GetComPort(), pModule->GetModuleAddress());
							str2.Format(_T("%d, DigitalIn: %s"), wRet, pModule->GetModuleIDstring());
							WK_STAT_PEAK(str1, 1, str2);
//							if (wRet == ResultStrCheckError)
							{
								Sleep(m_pDialog->m_nTimeout);
								continue;
							}
							break;
						}
					}
				}
			}
			if (wRet == NoError)										// Kein Fehler
			{
				pModule->SetConfirmCurrentState(false);					// Zustand wird nur einmal bestätigt
				pModule->m_wFails = 0;
			}
			else														// Fehler
			{
				if (wRet == TimeOut)									// Timeoutfehler für das Modul !
				{
					if (DoPoll())										// wenn der Thread gepollt wird
					{													// Timer versetzen
						m_pDialog->PostMessage(WM_USER, USER_MSG_RESTART_TIMER, 0);
					}
				}
				pModule->m_wFails++;
				if (pModule->m_wFails == 3)								// nach 3 Fehlern
				{
					if (DoPoll())										// wenn gepollt wird
					{
						pModule->SetFailCount(10);						// 10 Timerzyklen aussetzen
					}
					m_pDialog->PostMessage(WM_USER, USER_MSG_MODULE_ERROR+wRet, (LPARAM)pModule);
				}
				WK_TRACE(_T("Error(%d): %s COM%d Adr:%d\n"), wRet, pModule->GetModuleIDstring(), pModule->GetComPort(), pModule->GetModuleAddress());
				bErrorOccurred = TRUE;
			}
			if (bVisible && bUpdate && pModule->m_wFails <= 3)			// Dialog sichtbar, Änderungen, 
			{															// Dialogfenster updaten
				m_pDialog->PostMessage(WM_USER, USER_MSG_ALARM, (LPARAM)pModule);
			}
		}// while (pos)

		if (m_bMultipleAccess)											// bei Zugriff von mehreren Modulen
		{
			Sleep(50);
			pModule->ClosePort();										// Port schließen
		}

		SetPollState(FALSE);											// Thread pollt nicht mehr
		if (gm_pPerformanceMonitor)
		{
			gm_pPerformanceMonitor->Stop();
			gm_pPerformanceMonitor->ShowResults();
		}

		if (DoPoll())													// wenn dieser Thread gepollt wird
		{
			//		Sleep(500); test für Haupt Timer
			BOOL bForce = HIWORD(nCount);								// nächsten Thread (Port)
			nCount = LOWORD(nCount);									// benachrichtigen
			nCount++;
			if (nCount >= (UINT)m_pDialog->m_ThreadList.GetCount())		// letzter Thread
			{															// Fertig an das Steuerfenster senden
				m_pDialog->PostMessage(WM_USER, USER_MSG_POLL_THREADS_READY, nCount);
			}
			pos = m_pDialog->m_ThreadList.FindIndex(nCount);
			while (pos)
			{
				CICPI7000Thread *pThread = (CICPI7000Thread*)m_pDialog->m_ThreadList.GetAt(pos);
				if (pThread)
				{
					if (  (!pThread->IsPolling()						// Thread pollt nicht gerade oder
						 || !pThread->HasFailed())						// muß nicht aufgrund von Fehlern warten und
						 && (pThread->DoPoll()							// dieser Thread soll gepollt werden
						 || bForce))									// oder alle sollen gepollt werden
					{
						pThread->PostThreadMessage(WM_TIMER, nIDEvent, nCount);
						break;
					}
					else												// sonst den nächsten nehmen
					{
						pos = m_pDialog->m_ThreadList.FindIndex(++nCount);
					}
					if (pos == NULL)									// kein weiterer vorhanden
					{													// Fertig an das Steuerfenster senden
						m_pDialog->PostMessage(WM_USER, USER_MSG_POLL_THREADS_READY, nCount);
					}
				}
			}
		}
		else if (bErrorOccurred)										// Thread wird nicht gepollt
		{																// aber ein Fehler ist aufgetreten
			Sleep(m_pDialog->m_nPollTime);
			m_pDialog->PostMessage(WM_USER, EVENT_REQUEST_STATE, (LPARAM)pModule);
		}
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CICPI7000Thread::OnCommand(WORD wID, WORD wNotify, HWND hwndFrom)
{
	return 0;
}
/*********************************************************************************************
 Class      : CICPI7000Thread
 Function   : StopThread
 Description: Stops the thread

 Parameters:   
  bImmediate: (E): immediate means the thread is terminated after 500 ms otherwise
                   it waits infinite (true, false)  (bool)

 Result type:  (void)
 created: April, 24 2003
 <TITLE StopThread>
*********************************************************************************************/
void CICPI7000Thread::StopThread(bool bImmediate)
{
	int nTimeout = bImmediate ? 500 : INFINITE;
	if (m_dwSuspendCount)
	{
		ResumeThread();
	}
	PostThreadMessage(WM_QUIT, 0, 0);
	DWORD dwResult = WaitForSingleObject(m_hThread, nTimeout);
	if (WAIT_TIMEOUT == dwResult)
	{
		TerminateThread(m_hThread, 0xffffffff);
		dwResult = WaitForSingleObject(m_hThread, INFINITE);
	}
}
/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CICPI7000Thread
 Function   : AddModule
 Description: Adds a new module to be controlled by the thread.

 Parameters:   
  pModule: (E): pointer of the ICPCON module  (void*)

 Result type:  (void)
 created: April, 24 2003
 <TITLE AddModule>
*********************************************************************************************/
void CICPI7000Thread::AddModule(void *pModule)
{
	if (!ContainsModule(pModule))
	{
		if (m_Modules.GetCount() == 0)
		{
			m_nPort = ((CICPI7000Module*)pModule)->GetComPort();
		}
		ASSERT(m_nPort == (int)((CICPI7000Module*)pModule)->GetComPort());
		m_Modules.AddHead(pModule);
	}
	if (((CICPI7000Module*)pModule)->DoPollDO() || ((CICPI7000Module*)pModule)->DoPollDI())
	{
		m_bDoPoll = true;
	}
}
/*********************************************************************************************
 Class      : CICPI7000Thread
 Function   : ContainsModule
 Description: Determines whether a module is attached to this thread

 Parameters:   
  pModule: (E): pointer to the ICPCON module  (void*)

 Result type:  (bool)
 created: April, 24 2003
 <TITLE ContainsModule>
*********************************************************************************************/
bool CICPI7000Thread::ContainsModule(void *pModule)
{
	return (m_Modules.Find(pModule) != NULL) ? true : false;
}
/*********************************************************************************************
 Class      : CICPI7000Thread
 Function   : SetPollState
 Description: Sets the polling state of the thread.

 Parameters:   
  bPolling: (E): Thread is just polling (true, false)  (BOOL)

 Result type:  (void)
 created: April, 24 2003
 <TITLE SetPollState>
*********************************************************************************************/
void CICPI7000Thread::SetPollState(BOOL bPolling)
{
	m_pDialog->Lock();
	m_bPolling = bPolling;
	m_pDialog->Unlock();

}
/*********************************************************************************************
 Class      : CICPI7000Thread
 Function   : IncrementPollState
 Description: Increments the polling state if the state (just polling) is true.

 Parameters: None 

 Result type: Increment count (BOOL)
 created: April, 24 2003
 <TITLE IncrementPollState>
*********************************************************************************************/
BOOL CICPI7000Thread::IncrementPollState()
{
	BOOL bPolling = FALSE;
	m_pDialog->Lock();
	if (m_bPolling)
	{
		m_bPolling++;
		bPolling = m_bPolling;
	}
	m_pDialog->Unlock();
	return bPolling;
}
/*********************************************************************************************
 Class      : CICPI7000Thread
 Function   : IsPolling
 Description: Determines whether the thread is just polling

 Parameters: None 

 Result type: (TRUE, FALSE) (BOOL)
 created: April, 24 2003
 <TITLE IsPolling>
*********************************************************************************************/
BOOL CICPI7000Thread::IsPolling()
{ 
	BOOL bPolling;
	m_pDialog->Lock();
	bPolling = m_bPolling;
	m_pDialog->Unlock();
	return bPolling;
};
/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CICPI7000Thread
 Function   : HasFailed
 Description: Decrements a fail counter for this thread

 Parameters: None 

 Result type: Thread has failed to open its module port (true, false) (bool)
 created: April, 24 2003
 <TITLE HasFailed>
*********************************************************************************************/
bool CICPI7000Thread::HasFailed()
{
	if (m_nFailcount)
	{
		m_nFailcount--;
		return true;
	}
	return false;
}
/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CICPI7000Thread
 Function   : DeletePerformanceMonitor
 Description: Deletes the performance monitor

 Parameters: None 

 Result type:  (void)
 created: April, 24 2003
 <TITLE DeletePerformanceMonitor>
*********************************************************************************************/
void CICPI7000Thread::DeletePerformanceMonitor()
{
	WK_DELETE(gm_pPerformanceMonitor);
}
/*********************************************************************************************
 Class      : CICPI7000Thread
 Function   : CreatePerformanceMonitor
 Description: Creates a performance monitor for all threads

 Parameters: None 

 Result type:  (void)
 created: April, 24 2003
 <TITLE CreatePerformanceMonitor>
*********************************************************************************************/
void CICPI7000Thread::CreatePerformanceMonitor()
{
	ASSERT(gm_pPerformanceMonitor == NULL);
	gm_pPerformanceMonitor = new CWK_PerfMon(_T("Polling Time"));
}

void CICPI7000Thread::ResumeThread()
{
	if (m_dwSuspendCount)
	{
		m_bDoPoll = true;
		m_dwSuspendCount = CWinThread::ResumeThread();
	}
}

void CICPI7000Thread::SuspendThread()
{
	if (m_dwSuspendCount == 0)
	{
		m_bDoPoll = false;
		CWinThread::SuspendThread();
		m_dwSuspendCount++;
	}
}
