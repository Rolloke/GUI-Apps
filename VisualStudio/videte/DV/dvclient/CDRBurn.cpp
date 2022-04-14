// CDRBurn.cpp: implementation of the CCDRBurn class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CDRBurn.h"
#include "InitPacketCD.h"
#include "DVClient.h"
#include "MainFrame.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCDRBurn::CCDRBurn(CIPCDrive* pCD_Drive)
: m_RemountThread(theApp.GetMainFrame()->m_hWnd,
				  pCD_Drive->GetRootString(),
				  "&Remount"),
  m_FormatThread(theApp.GetMainFrame()->m_hWnd,
				  pCD_Drive->GetRootString(),
				  "&Formatiere neue UDF CD..."),
  m_FinalizeThread(theApp.GetMainFrame()->m_hWnd,
				  pCD_Drive->GetRootString(),
				  "&Finalisiere..."),
  m_EjectThread(theApp.GetMainFrame()->m_hWnd,
				  pCD_Drive->GetRootString(),
				  "&Auswerfen")
{
	WK_TRACE("CCDRBurn::CCDRBurn\n");
	m_pCD_Drive				= pCD_Drive;
	m_pInitPCD				= new CInitPacketCD;
	m_dwTickCountStart		= GetTickCount();
	m_dwTickCountEnd		= GetTickCount();
	m_bFertigButtonClicked	= FALSE;
	m_bFinalizeDlgOnDesktop = FALSE;
	m_bFinalizeInProgress	= FALSE;
	m_hFinalizeProgressDlg	= NULL;
}

CCDRBurn::~CCDRBurn()
{
	WK_DELETE(m_pInitPCD);
	WK_TRACE("CCDRBurn::~CCDRBurn\n");
}
		
//////////////////////////////////////////////////////////////////////
//Search for dialog window of PacketCD
BOOL CCDRBurn::IsPacketCDWindow()
{
	BOOL	bReturn = FALSE;
	HWND	hPacketCDMainWnd; 

	if((hPacketCDMainWnd = FindWindow(NULL, m_pInitPCD->GetStringWndTxtPacketCD())) == NULL)
	{
		SetTickCountEnd(GetTickCount());
	}
	else
	{
		(CWnd*)theApp.GetMainFrame()->SetForegroundWindow();
		WK_TRACE("IsPacketCDWindow(): Found PacketCD Dlg\n");
		m_pInitPCD->SetHandlePacketCDWnd(hPacketCDMainWnd);
		bReturn = TRUE;
	}

	return bReturn;
}

//////////////////////////////////////////////////////////////////////
//Waits until dialog window of PacketCD is erased from desktop
BOOL CCDRBurn::IsFormatted()
{
	BOOL	bRet = FALSE;
	char	sWndText[128];

	if (   m_pInitPCD->GetHandlePacketCDWnd()
		&& ::IsWindow(m_pInitPCD->GetHandlePacketCDWnd()))
	{
		//m_bFertigButtonClicked == TRUE means,
		//button "&Fertig stellen" not yet clicked
		WK_TRACE("IsFormated(): PacketCD Dlg still on desktop\n");
		if(!m_bFertigButtonClicked) 
		{
			GetDlgItemText(m_pInitPCD->GetHandlePacketCDWnd(), 
				m_pInitPCD->GetWordFormate_ButtonID(), sWndText, sizeof(sWndText));
			//
			//wait until the text of button "&Formatiere" in main PacketCD dialog
			//has changed to "&Weiter >"
			//
			if(sWndText != m_pInitPCD->GetStringFormate_ButtonTxt())
			{
				//send click message to button "&Fertig stellen"
				PostMessage(m_pInitPCD->GetHandlePacketCDWnd(),WM_COMMAND,
					(WPARAM)MAKELONG(m_pInitPCD->GetWordFertig_ButtonID(),BN_CLICKED),
					(LPARAM)m_pInitPCD->GetHandlePacketCDChildWnd());
				Sleep(50);
				WK_TRACE("IsFormated(): Fertigstellen button clicked\n");
				(CWnd*)theApp.GetMainFrame()->SetForegroundWindow();
				m_bFertigButtonClicked = TRUE;
			}
		}
	
		m_dwTickCountEnd = GetTickCount();
	}
	else
	{
		//formating finished
		bRet = TRUE;
	}

	return bRet;
}


//////////////////////////////////////////////////////////////////////
void CCDRBurn::SetTickCountStart(DWORD dwStart)
{
	m_dwTickCountStart = dwStart;
}

//////////////////////////////////////////////////////////////////////
void CCDRBurn::SetTickCountEnd(DWORD dwEnd)
{
	m_dwTickCountEnd = dwEnd;
}

//////////////////////////////////////////////////////////////////////
//Starts formatting the CD-R with clicking all nessesarry buttons in PacketCD dlg
BOOL CCDRBurn::StartFormatCD()
{

	BOOL	bRet = FALSE;
	DWORD	dwFirstTickCount, dwSecondTickCount, dwCount;
	HWND	hFirstChild = NULL, hSecondChild = NULL;
	WORD	wIDEditField;
	char	sWndText[128];
	dwFirstTickCount = dwSecondTickCount = GetTickCount();
	dwCount = 500;

	//
	//get every child window of packed_cd desktop window and
	//find that childwindow which holds the editfield for the new CDName
	//

	hFirstChild = GetWindow(m_pInitPCD->GetHandlePacketCDWnd(), GW_CHILD);
	GetWindowText(hFirstChild, sWndText, sizeof(sWndText));
	//
	//check every child window
	//
	while((sWndText != m_pInitPCD->GetStringWndTxtPacketCD())&&
					((dwSecondTickCount-dwFirstTickCount) <=dwCount))
		{
			WK_TRACE("StartFormatCD(): Searching PacketCD dlg\n");
			hFirstChild = GetWindow(hFirstChild, GW_HWNDNEXT);
			GetWindowText(hFirstChild, sWndText, sizeof(sWndText));
			dwSecondTickCount = GetTickCount();
		}
		
	if((dwSecondTickCount-dwFirstTickCount) > dwCount)
	{
		WK_TRACE("StartFormatCD(): PacketCD dlg not found after 500ms\n");
		return bRet;
	}

	//
	//save handle to child window that holds the editfield for the new CDName
	//
	m_pInitPCD->SetHandlePacketCDChildWnd(hFirstChild);
	(CWnd*)theApp.GetMainFrame()->SetForegroundWindow();
	

	//
	//get childs child window of PacketCD desktop window and search
	//for the editfield to insert the new cd-name 
	//
	hSecondChild = GetWindow(m_pInitPCD->GetHandlePacketCDChildWnd(), GW_CHILD);
	wIDEditField = (WORD)GetDlgCtrlID(hSecondChild);
		
	//
	//find correct ID of edit field
	//
	while((wIDEditField != m_pInitPCD->GetWordEdit_FieldID())&&
				((dwSecondTickCount-dwFirstTickCount) <=dwCount))
	{
		hSecondChild = GetWindow(hSecondChild, GW_HWNDNEXT);
		wIDEditField = (WORD)GetDlgCtrlID(hSecondChild);
		dwSecondTickCount = GetTickCount();
	}
		
	if((dwSecondTickCount-dwFirstTickCount) > dwCount)
	{
		WK_TRACE("StartFormatCD(): Searching for edit field tooks more than 500ms\n");
		return bRet;
	}

	//
	// send the new cd-name to the editfield
	// we can use send here because it's an edit control 
	// and we are fast enough back
	SendMessage(hSecondChild,WM_SETTEXT, 0, (LPARAM)(const char*)m_pInitPCD->GetStringNewCDname());
	(CWnd*)theApp.GetMainFrame()->SetForegroundWindow();
	WK_TRACE("StartFormatCD(): New CD-Name %s sended\n", m_pInitPCD->GetStringNewCDname());

	//
	//click button "&Formatiere" in packet-cd dialog
	//
	hFirstChild = GetWindow(m_pInitPCD->GetHandlePacketCDWnd(), GW_CHILD);
	// we post here
	// because it takes a long time an we can check
	// wether Packet CD is still running or not
	PostMessage(m_pInitPCD->GetHandlePacketCDWnd(),WM_COMMAND,
			(WPARAM)MAKELONG(m_pInitPCD->GetWordFormate_ButtonID(),BN_CLICKED),
			(LPARAM)hFirstChild);

	(CWnd*)theApp.GetMainFrame()->SetForegroundWindow();
	WK_TRACE("StartFormatCD(): Format button clicked\n");
	bRet = TRUE;

	return bRet;
}

//////////////////////////////////////////////////////////////////////
//Starts finalizing CD-R in own thread
BOOL CCDRBurn::StartExplMenuFinalizeCD()
{
	if (!m_FinalizeThread.IsRunning())
	{
		// first stop/delete previous start
		m_FinalizeThread.StopThread();
		m_FinalizeThread.StartThread();
		m_bFinalizeDlgOnDesktop = FALSE;
		m_bFinalizeInProgress = FALSE;
		(CWnd*)theApp.GetMainFrame()->SetForegroundWindow();

	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//Waits until dlg finalize is erased from desktop
BOOL CCDRBurn::IsFinalized()
{
	BOOL	bRet = FALSE;
	HWND	hFinalizeDlg, hFinalizeFirstChild;
	HWND	hProgress;

	//
	//wait until the first dialogbox of PacketCD for finalizing exists 
	//on desktop
	//
	if(!m_bFinalizeDlgOnDesktop) 
	{
		if((hFinalizeDlg = FindWindow(NULL, m_pInitPCD->GetStringTxtFirstFinalizeDlg())) != NULL)
		{
			ShowWindow(hFinalizeDlg, SW_HIDE);
			m_pInitPCD->SetHandleFirstFinalizeDlg(hFinalizeDlg);
			m_bFinalizeDlgOnDesktop = TRUE;
		}
		else
		{
			// time out
			if((m_dwTickCountEnd-m_dwTickCountStart) > 20*1000)
			{
				WK_TRACE("Starting finalize via explorer menu failed\n");
			}
		}
	}
	else
	{	
		//
		//is button "ja" in finalize dialog already clicked?
		//
		if(!m_bFinalizeInProgress)
		{
			//
			//search button "ja" in first dialog box for finalizing the CD
			//
			hFinalizeFirstChild = GetWindow(m_pInitPCD->GetHandleFirstFinalizeDlg(), GW_CHILD);

			PostMessage(m_pInitPCD->GetHandleFirstFinalizeDlg(),WM_COMMAND,
				(WPARAM)MAKELONG(m_pInitPCD->GetWordYesButtonID_FirstFinalizeDlg(),
					BN_CLICKED), (LPARAM)hFinalizeFirstChild);
			m_bFinalizeInProgress = TRUE;
			hProgress = FindWindow(NULL, m_pInitPCD->GetStringTxtSecondFinalizeDlg());
			if(IsWindow(hProgress))
			{
				ShowWindow(hProgress,SW_HIDE);
			}
			WK_TRACE("CCDRBurn::IsFinalized: yes button aktivated\n");
			(CWnd*)theApp.GetMainFrame()->SetForegroundWindow();
		}
		else
		{
			//
			//wait until the second dialog box is on desktop. This dlg box holds
			//the progress bar for finalizing process
			//
			hProgress = FindWindow(NULL, m_pInitPCD->GetStringTxtSecondFinalizeDlg());
			(CWnd*)theApp.GetMainFrame()->SetForegroundWindow();
			if (IsWindow(hProgress))
			{
				m_hFinalizeProgressDlg = hProgress;
				ShowWindow(hProgress,SW_HIDE);
				//keep on waiting until finalize progress
				//"Finalisiere UDF CD" is not longer been found
				WK_TRACE("CCDRBurn::IsFinalized(): found finalize progress dlg\n");
				bRet = TRUE;
				(CWnd*)theApp.GetMainFrame()->SetForegroundWindow();
			}
		}
	}

	m_dwTickCountEnd = GetTickCount();

	return bRet;
}

//////////////////////////////////////////////////////////////////////
BOOL CCDRBurn::StartExplMenuRemountCD()
{
	if (!m_RemountThread.IsRunning())
	{
		// first stop/delete previous start
		m_RemountThread.StopThread();
		m_RemountThread.StartThread();
		WK_TRACE("StartExplMenuRemountCD() ausgeführt\n");
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CCDRBurn::TerminateExplMenuRemountCD()
{
	if (m_RemountThread.IsRunning())
	{
		m_RemountThread.Terminate();
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CCDRBurn::IsCDRemounting()
{
	return m_RemountThread.IsRunning();
}

//////////////////////////////////////////////////////////////////////
CExplorerMenuThread* CCDRBurn::GetThreadRemount()
{
	return &m_RemountThread;
}

//////////////////////////////////////////////////////////////////////
BOOL CCDRBurn::StartExplMenuFormatCD()
{
	if (!m_FormatThread.IsRunning())
	{
		// first stop/delete previous start
		m_FormatThread.StopThread();
		m_FormatThread.StartThread();
		(CWnd*)theApp.GetMainFrame()->SetForegroundWindow();
	}
	return TRUE;
}


//////////////////////////////////////////////////////////////////////

BOOL CCDRBurn::IsCDFinalizing()
{
	if(IsWindow(m_hFinalizeProgressDlg))
	{
		ShowWindow(m_hFinalizeProgressDlg, SW_HIDE);
	}
	return m_FinalizeThread.IsRunning();
}
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
BOOL CCDRBurn::StartExplMenuEjectCD()
{
	if (!m_EjectThread.IsRunning())
	{
		// first stop/delete previous start
		m_EjectThread.StopThread();
		m_EjectThread.StartThread();
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////

