// acdcDlg.cpp : implementation file
//

#include "stdafx.h"
#include "acdc.h"
#include "acdcDlg.h"
#include "NeroThread.h"
#include "NeroBurn.h"


#include "IPCServerControlACDC.h"
#include "IPCBurnDataCarrier.h"

#include <dbt.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CAcdcDlg dialog

CAcdcDlg::CAcdcDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAcdcDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAcdcDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
	Create(IDD);		//create not modal dialog
}

void CAcdcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAcdcDlg)
	DDX_Control(pDX, IDOK, m_CtrlOK);
	DDX_Control(pDX, IDC_MESSAGES, m_EdtMessage);
	DDX_Control(pDX, IDC_PROGRESS1, m_prgCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAcdcDlg, CDialog)
	//{{AFX_MSG_MAP(CAcdcDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ABORT, OnDummyAbort)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_ABOUT, OnAbout)
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_BURN_CDINFO, OnBurnCDInfo)
	ON_MESSAGE(WM_BURN_WRITEISO, OnBurnWriteIso)
	ON_MESSAGE(WM_BURN_NEW_MESSAGE, OnNewMessage)
	ON_MESSAGE(WM_BURN_ERROR, OnBurnError)
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_MESSAGE(WM_TRAYCLICKED, OnTrayClicked)	
	ON_MESSAGE(WM_BURN_NOTIFY_DRIVE, OnNotifyDrive)
	ON_WM_DEVICECHANGE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAcdcDlg message handlers

BOOL CAcdcDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND|MF_GRAYED);

	NOTIFYICONDATA tnd;

	CString sTip = COemGuiApi::GetApplicationName(WAI_AC_DC);

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;

	tnd.uFlags		= NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.uCallbackMessage = WM_TRAYCLICKED;
	tnd.hIcon		= theApp.LoadIcon(IDR_MAINFRAME);

	lstrcpyn(tnd.szTip, sTip, sTip.GetLength()+1);
	Shell_NotifyIcon(NIM_ADD, &tnd);
	//set position of windows
//	CRect lpRect;
//	GetClientRect(lpRect);
//	SetWindowPos(&wndTop,320,0,lpRect.right, lpRect.bottom, SWP_SHOWWINDOW);


	m_prgCtrl.SetRange(0,100);
	m_prgCtrl.SetPos(0);

	m_MainFunktion		= MF_START;

	m_dwCDRWErasingTime = 0;	//cdrw erasing time in seconds
	m_dwCDRWProgress	= 0;
	m_dwTick			= 0;
	m_bValidNeroVersion = FALSE;

	m_pProducer = new CProducer;
	m_BurnProducer = GetInstalledBurnSoftware();
	m_pNeroThread = NULL;

	if(m_BurnProducer != BP_NOPRODUCER)
	{
		m_pNeroThread = new CNeroThread();
		m_pNeroThread->StartThread();
		m_pNeroBurn = m_pNeroThread->GetNeroBurn();
	}

	m_sVolumeName = _T("");
	m_bIsInBackup = FALSE;
	
	SetMainFunction(MF_START);


//TODO tkr DeleteVolume (wie in CIPC schon vorgesehen) noch einbauen und confirmdelete zurückliefern

	return TRUE;  // return TRUE  unless you set the focus to a control
}
/////////////////////////////////////////////////////////////////////////////
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CAcdcDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

/////////////////////////////////////////////////////////////////////////////
// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAcdcDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


/*********************************************************************************************
 Class		   : CAcdcDlg
 Function      : SetProgress 
 Description   : This function sets the progress bar in programs window (gui).

 Parameters:  
  dwPercent:  (O):  how much percent of the current NeroAPI process has been completed (DWORD)

 Result type:  --

 Author: TKR
 created: July, 16 2002
 <TITLE SetProgress >
 <GROUP CAcdcDlg>
 <TOPICORDER 0>
 <KEYWORDS CAcdcDlg, SetProgress>
*********************************************************************************************/
void CAcdcDlg::SetProgress(DWORD dwPercent)
{
	if(IsWindow(m_prgCtrl))	
		m_prgCtrl.SetPos(dwPercent);

	CIPCServerControlACDC* pSCAcdc = theApp.GetServerControl();
	if(pSCAcdc)
	{
		CIPCBurnDataCarrier* pBDC = pSCAcdc->GetBurnDataCarrier();
		if(pBDC)
		{
			DWORD dwSessionID = pBDC->GetSessionID();
			pBDC->DoIndicateSessionProgress(dwSessionID, (int)dwPercent);
		}
	}
}

/*********************************************************************************************
 Class		   : CAcdcDlg
 Function      : OnOK 
 Description   : This function frees the memory, frees the connection to the NeroAPI
				 and quits the main program.

 Parameters:  

 Result type:  --

 Author: TKR
 created: July, 16 2002
 <TITLE OnOK >
 <GROUP CAcdcDlg>
 <TOPICORDER 0>
 <KEYWORDS CAcdcDlg, OnOK>
*********************************************************************************************/
void CAcdcDlg::OnOK() 
{
	StopNeroThread();
	WK_DELETE(m_pNeroThread);
	WK_DELETE(m_pProducer);
	DestroyWindow();
	WK_TRACE(_T("ACDC alle Threads gestoppt\n"));
}


/*********************************************************************************************
 Class		   : CAcdcDlg
 Function      : OnAbort 
 Description   : This function sets the BurnContexts variable m_bAborted = TRUE.
				 This variable is checked in each call of the BurnContext`s IdleCallback().
				 If it is TRUE, the current NeroAPI function will be stoped.

 Parameters: -- 

 Result type:  -- 

 Author: TKR
 created: July, 16 2002
 <TITLE OnAbort >
 <GROUP CAcdcDlg>
 <TOPICORDER 0>
 <KEYWORDS CAcdcDlg, OnAbort>
*********************************************************************************************/
void CAcdcDlg::OnDummyAbort() 
{
	if(m_pNeroBurn)
	{
		m_pNeroBurn->SetAbortFlag();
		WK_TRACE(_T("CAcdcDlg::OnDummyAbort() : setting aborted flag \n"));
	}
}


/*********************************************************************************************
 Class		   : CAcdcDlg
 Function      : AppendString 
 Description   : Appends a new message line in the edit field on gui.

 Parameters: 
  sMessage: (I): the new Mmessage 

 Result type:  --

 Author: TKR
 created: July, 16 2002
 <TITLE AppendString >
 <GROUP CAcdcDlg>
 <TOPICORDER 0>
 <KEYWORDS CAcdcDlg, AppendString>
*********************************************************************************************/
void CAcdcDlg::AppendMessage(CString sMessage)
{
	if(sMessage.Right(1) == _T("\n"))
	{
		sMessage = sMessage.Left(sMessage.GetLength()-1);
	}
	CString strBuffer;
	m_EdtMessage.GetWindowText (strBuffer);
	if (!strBuffer.IsEmpty())
	{
		strBuffer += _T("\r\n");
	}
	
	strBuffer += sMessage;
	m_EdtMessage.SetWindowText (strBuffer);
	m_EdtMessage.LineScroll (m_EdtMessage.GetLineCount(), 0);
}

/*********************************************************************************************
 Class		   : CAcdcDlg
 Function      : OnIdle 
 Description   : This function is called continously from the main application. It handles all
				 incomming commands from the CIPC.

 Parameters: --

 Result type:  --

 Author: TKR
 created: July, 23 2002
 <TITLE OnIdle >
 <GROUP CAcdcDlg>
 <TOPICORDER 0>
 <KEYWORDS CAcdcDlg, OnIdle>
*********************************************************************************************/
void CAcdcDlg::OnIdle()
{
	Sleep(20);
	switch(GetMainFunction())
	{

	case MF_START:
		m_dwCDRWErasingTime = 0;
		PostMessage(WM_USER);
		break;

	case MF_CD_DRIVES:
	case MF_CD_INFO:
	case MF_CD_WRITE_ISO:
	case MF_EJECT:

		if(GetMainFunction() != MF_EJECT)
		{
			m_CtrlOK.EnableWindow(FALSE);
		}
		//start specified function
		m_pNeroThread->SetMainFunktion(m_MainFunktion);
		SetMainFunction(MF_WAIT_IN_USE);
		PostMessage(WM_USER);
		break;

	case MF_WAIT_IN_USE:
		CDRWEraseProgress();
		PostMessage(WM_USER);
		break;

	//write CD in VideoCD format
	case MF_CD_WRITE_VCD:
		break;

	//write CD in SuperVideoCD format
	case MF_CD_WRITE_SVCD:
		break;

	//erase medium
	case MF_ERASE:
		break;

	//abort current process
	case MF_ABORT:
		break;

	case MF_ERRORS:
		WK_TRACE(_T("error \n"));
		PostMessage(WM_USER);
		break;

	default:
		break;

	}

}

/*********************************************************************************************
 Class		   : CAcdcDlg
 Function      : GetMainFunction 
 Description   : This function gets the MainFunction. This MainFunktion is set fromout the CIPC
				 and indicates what this program (Acdc.exe) has to do next.
				 The following MainFunctions are allowed:
				MF_CD_DRIVES		- get all available CD-RW drives
				MF_CD_INFO			- get information about loaded medium (CD-R/CD-RW/DVD)
				MF_ERASE			- erase CD-RW / DVD-RW
				MF_CD_WRITE_ISO		- write CD in ISO format
				MF_CD_WRITE_VCD		- write CD in VideoCD format
				MF_CD_WRITE_SVCD	- write CD in SuperVideoCD format
				MF_EJECT			- eject medium

 Parameters: --

 Result type:  must be return 0 (neccessary for threads) (enum MainFunction)

 Author: TKR
 created: July, 16 2002
 <TITLE GetMainFunction >
 <GROUP CAcdcDlg>
 <TOPICORDER 0>
 <KEYWORDS CAcdcDlg, GetMainFunction>
*********************************************************************************************/
MainFunction CAcdcDlg::GetMainFunction()
{
	return m_MainFunktion;
}

/*********************************************************************************************
 Class		   : CAcdcDlg
 Function      : SetMainFunction 
 Description   : This function sets the new MainFunktion.

 Parameters: 
  mf: (I): the new MainFunktion (enum MainFunction)

 Result type:  --

 Author: TKR
 created: July, 16 2002
 <TITLE SetMainFunction >
 <GROUP CAcdcDlg>
 <TOPICORDER 0>
 <KEYWORDS CAcdcDlg, SetMainFunction>
*********************************************************************************************/
void CAcdcDlg::SetMainFunction(MainFunction mf)
{
	m_MainFunktion = mf;
}
/*********************************************************************************************
 Class		   : CAcdcDlg
 Function      : OnBurnWriteIso
 Description   : This function is called on message WM_BURN_WRITEISO at the end of burn process

 Parameters: 
  wParam: (I): the burn result, end of burning process, new Mainfunction (enum MainFunction)
  lParam: (I): last Mainfunction (enum MainFunction)

 Result type:  afx_msg (long) 

 Author: TKR
 created: August, 02 2002
 <TITLE OnBurnWriteIso >
 <GROUP CAcdcDlg>
 <TOPICORDER 0>
 <KEYWORDS CAcdcDlg, OnBurnWriteIso>
*********************************************************************************************/
long CAcdcDlg::OnBurnWriteIso(WPARAM wParam, LPARAM lParam)
{
//	CIPCServerControlACDC* pSCAcdc = theApp.GetServerControl();
	CString sError;

	MainFunction mf = (MainFunction)wParam;
	EXITCODE code	= (EXITCODE)lParam;

	//if an error accours, send error 
	if((EXITCODE)lParam != EXITCODE_OK)
	{
		PostMessage(WM_BURN_ERROR, mf, code);
	}
	else
	{
		//confirm requested process
		ConfirmSession();
	}

	//backup wurde beendet (abgebrochen). Nun darf ACDC wieder USB laufwerke erkennen und
	//darauf reagieren (Neustart).
	SetIsInBackup(FALSE);

	//process finshed, set start state (rewaiting for next process)
	SetMainFunction(MF_START);
	
	return 0;
}

/*********************************************************************************************
 Class		   : CAcdcDlg
 Function      : OnBurnCDInfo 
 Description   : This function is called on message WM_BURN_CDINFO at the end of getting
				 information about in drive inserted medium.

 Parameters: 
  wParam: (I): wParam: the Mainfunction
  lParam: (I): lParam: the exitcode

 Result type:  afx_msg (long) 

 Author: TKR
 created: August, 05 2002
 <TITLE OnBurnCDInfo >
 <GROUP CAcdcDlg>
 <TOPICORDER 0>
 <KEYWORDS CAcdcDlg, OnBurnCDInfo>
*********************************************************************************************/
long CAcdcDlg::OnBurnCDInfo(WPARAM wParam, LPARAM lParam)
{
//	CIPCServerControlACDC* pSCAcdc = theApp.GetServerControl();
	CString sError;

	MainFunction mf = (MainFunction)wParam;
	EXITCODE code	= (EXITCODE)lParam;

	//if an error accours, send error 
	if((EXITCODE)lParam != EXITCODE_OK)
	{
		PostMessage(WM_BURN_ERROR, mf, code);
	}
	else
	{
		//confirm requested process
		ConfirmVolumeInfos();
	}
	
	//process finshed, set start state (rewaiting for next process)
	SetMainFunction(MF_START);

	return 0;
}
/*********************************************************************************************/
long CAcdcDlg::OnBurnError(WPARAM wParam, LPARAM lParam)
{
	//if(lParam != -1) then lParam = EXITCODE, else lParam = NoBurnSoftwareInstalled

	m_CtrlOK.EnableWindow(TRUE);
	CIPCServerControlACDC* pSCAcdc = theApp.GetServerControl();
	CString sError;
	DWORD dwCmd			= CIPC_DC_REQUEST_VOLUME_INFOS;
	int iUnitErrorCode	= DC_ERROR_UNKNOWN_ERROR;

	EXITCODE code	= (EXITCODE)lParam;
	MainFunction mf = (MainFunction)wParam;

	switch(mf)
	{
	case MF_CD_INFO:
		dwCmd = CIPC_DC_REQUEST_VOLUME_INFOS;
		break;

	case MF_CD_WRITE_ISO:
		dwCmd = CIPC_DC_REQUEST_SESSION;
		break;

	default:
		break;

	}

	if(pSCAcdc)
	{
		CIPCBurnDataCarrier* pBDC = pSCAcdc->GetBurnDataCarrier();
		if(pBDC)
		{
			DWORD dwSessionID = pBDC->GetSessionID();

			if(lParam == -1) //no burn software is installed
			{
				code = EXITCODE_NO_BURN_SOFTWARE;
			}

			//get NERO Exitcode 
			CString sFileNotFound;
			sError.Format(_T("Error: %s\n"), GetTextualExitCode(code));
			AppendMessage(sError);

			if(m_pNeroBurn)
			{
				iUnitErrorCode = m_pNeroBurn->NeroExitCodeToDCErrorCode(code);
			
				if(code == EXITCODE_FILE_NOT_FOUND)
				{
					sFileNotFound = m_pNeroBurn->GetFileNotFound();
					sError += sFileNotFound;
					AppendMessage(sFileNotFound);
				}
			}

			if(code == EXITCODE_USER_ABORTED)
			{
				pBDC->DoConfirmCancelSession(dwSessionID);
				return 0;
			}

			pBDC->DoIndicateError(dwCmd, dwSessionID, CIPC_ERROR_DATA_CARRIER, iUnitErrorCode, sError);

			//reset FileNotFound if accoured
			if(!sFileNotFound.IsEmpty())
			{
				sFileNotFound.Empty();
				if(m_pNeroBurn)
				{
					m_pNeroBurn->SetFileNotFound(sFileNotFound);
				}
			}
		}
	}
	return 0;
}
/*********************************************************************************************
 Class		   : CAcdcDlg
 Function      : StopNeroThread 
 Description   : This function stops the running Nero thread 

 Parameters: --

 Result type: --

 Author: TKR
 created: August, 02 2002
 <TITLE StopNeroThread >
 <GROUP CAcdcDlg>
 <TOPICORDER 0>
 <KEYWORDS CAcdcDlg, StopNeroThread>
*********************************************************************************************/
void CAcdcDlg::StopNeroThread()
{
	if(m_pNeroThread)
	{
		if(m_pNeroThread->IsRunning())
		{
			m_pNeroThread->StopThread();
		}
	}
}

/*********************************************************************************************
 Class		   : CAcdcDlg
 Function      : SetCDRWErasingTime 
 Description   : This function sets the time in seconds which is needed to erase the 
				 inserted CDRW. The seconds are calculated by the NeroAPI and are not
				 so close to the exactly needed time. So we add a quarter of that time.

 Parameters: 
  sSeconds: (I): the approx. time for erasing the inserted cdrw (DWORD)
				 if is -1 the erasing progress a´has finished, so we set the progress bar
				 to 100%.

 Result type: -- 

 Author: TKR
 created: August, 02 2002
 <TITLE SetCDRWErasingTime >
 <GROUP CAcdcDlg>
 <TOPICORDER 0>
 <KEYWORDS CAcdcDlg, SetCDRWErasingTime>
*********************************************************************************************/
void CAcdcDlg::SetCDRWErasingTime(DWORD sSeconds)
{
	if(sSeconds == -1)
	{
		//set erase progress bar to 100%
		m_dwCDRWProgress = m_dwCDRWErasingTime;
		SetProgress(100);
		
	}
	else
	{
		m_dwCDRWErasingTime = sSeconds+(m_dwCDRWErasingTime/4);
		m_dwCDRWProgress = 0;
	}
	
	m_dwTick = GetTickCount();
}

/*********************************************************************************************
 Class		   : CAcdcDlg
 Function      : CDRWEraseProgress 
 Description   : This function sets the progress bar in the main window while erasing the
				 inserted CDRW.

 Parameters: --

 Result type: -- 

 Author: TKR
 created: August, 02 2002
 <TITLE CDRWEraseProgress >
 <GROUP CAcdcDlg>
 <TOPICORDER 0>
 <KEYWORDS CAcdcDlg, CDRWEraseProgress>
*********************************************************************************************/
void CAcdcDlg::CDRWEraseProgress()
{
	if(m_dwCDRWProgress < m_dwCDRWErasingTime)
	{
		if(GetTimeSpan(m_dwTick) > 1 * 1000)
		{
			m_dwCDRWProgress++;
			DWORD dwProgress = (m_dwCDRWProgress * 100)/m_dwCDRWErasingTime;
			SetProgress(dwProgress);
			m_dwTick = GetTickCount();

			//send progess to client
			CIPCServerControlACDC* pSCAcdc = theApp.GetServerControl();
			if(pSCAcdc)
			{
				CIPCBurnDataCarrier* pBDC = pSCAcdc->GetBurnDataCarrier();
				if(pBDC)
				{
					DWORD dwSessionID = pBDC->GetSessionID();
					pBDC->DoIndicateSessionProgress(dwSessionID, (int)dwProgress);
					if(dwProgress == 100)
					{
						WK_TRACE(_T("EraseProgress == 100\n"));
					}
				}
			}
		}
	}
}

/*********************************************************************************************
 Class		   : CAcdcDlg
 Function      : GetFilesToBackup 
 Description   : This function copies the CStringArray of all file/directories to burn to
				 a new, transferred CStringArray.

 Parameters: 
  saFiles (I/O): holds all files/directories to burn (CStringArray&)

 Result type: -- 

 Author: TKR
 created: August, 07 2002
 <TITLE GetFilesToBackup >
 <GROUP CAcdcDlg>
 <TOPICORDER 0>
 <KEYWORDS CAcdcDlg, GetFilesToBackup>
*********************************************************************************************/
void CAcdcDlg::GetFilesToBackup(CStringArray& saFiles)
{
	for(int i=0; i < m_saFilesToBackup.GetSize(); i++)
	{
		saFiles.Add(m_saFilesToBackup.GetAt(i));
	}
}

/*********************************************************************************************
 Class		   : CAcdcDlg
 Function      : GetVolumeName 
 Description   : This function return the name of the CD/DVD to burn onto.

 Parameters: --

 Result type: returns the name of the CD/DVD to burn onto (CString)

 Author: TKR
 created: August, 07 2002
 <TITLE GetVolumeName >
 <GROUP CAcdcDlg>
 <TOPICORDER 0>
 <KEYWORDS CAcdcDlg, GetVolumeName>
*********************************************************************************************/
CString CAcdcDlg::GetVolumeName()
{
	return m_sVolumeName;
}



/*********************************************************************************************/
BurnProducer CAcdcDlg::GetBurnProducer()
{
	return m_BurnProducer;
}

/*********************************************************************************************/
EXITCODE CAcdcDlg::SetVolumeProperties(CBurnContext* pBurnContext)
{
	EXITCODE code = EXITCODE_OK;

	//get all nessessary data, create a CIPCDrive and init CIPCDrive
	const CString sRoot = pBurnContext->GetParams()->GetDriveName();
	CString sRootExtended;
	sRootExtended.Format(_T("%s:\\"),sRoot);
	theApp.m_pDrive = new CIPCDrive(sRootExtended, DVR_CDR_BACKUP_DRIVE);

	ULONGLONG ullMediumFreeCapacity = 0;
	ULONGLONG ullMediumUsedCapacity = 0;
	
	ullMediumFreeCapacity = pBurnContext->GetMediumFreeCapacityInBytes();
	ullMediumUsedCapacity = pBurnContext->GetMediumUsedCapacityInBytes();

	ULONGLONG ullMediumTotalCapacityInBytes = ullMediumFreeCapacity + ullMediumUsedCapacity;

	//bei DVD+-RW`s die freie und benutzte Kapazität nicht addieren sondern fest setzen, da sie
	//ja sowieso nur benutzt werden kann, wenn sie gelöscht wird
	if(pBurnContext->GetMediumInfo() == MI_DVDRWDELETE)
	{
		//da eine zu löschende DVD+-RW sich von einer leeren DVD+-RW nur darin unterscheidet,
		//dass die freie Kapazität kleiner bzw. ungleich als die gesamte Kapazität ist
		//hier die freie etwas verringern
		ULONGLONG ullOneMegaByte = 1024 * 1024; 
		ullMediumTotalCapacityInBytes = MAX_DVD_CAPACITY_IN_MB * ullOneMegaByte;
		ullMediumFreeCapacity = ullMediumTotalCapacityInBytes - ullOneMegaByte;
	}
	
	ULONGLONG ullDummyFree = ullMediumFreeCapacity/(1024*1024);
	ULONGLONG ullDummyUsed = ullMediumUsedCapacity/(1024*1024);
	WK_TRACE(_T("Capacities in MB: free: %d  used: %d  \n"), (DWORD)ullDummyFree, (DWORD)ullDummyUsed);


	ULARGE_INTEGER uliTotalBytes, uliFreeBytes;
	uliTotalBytes.QuadPart	= ullMediumTotalCapacityInBytes;
	uliFreeBytes.QuadPart	= ullMediumFreeCapacity;

	//standard is no DVD
	m_pNeroBurn->SetIsDVD(FALSE);

	//get all medium information and send it to main window	
	WORD wMediumInfo = (WORD)pBurnContext->GetMediumInfo();


	switch(wMediumInfo)
	{
	case MI_CDREADONLY:	//medium is read only, free MB = 0
		theApp.m_pDrive->SetType(DRIVE_CDROM);
		uliFreeBytes.QuadPart = 0;
		uliTotalBytes.QuadPart = 0;
		break;

	case MI_CDOK:		//medium is an empty CD and is writable
		theApp.m_pDrive->SetType(DRIVE_CD_R);
		break;

	case MI_CDRWOK:		//medium is an empty CD-RW and is writable
	case MI_CDRWDELETE:	//medium is a not empty CD-RW and has to be deleted first
		theApp.m_pDrive->SetType(DRIVE_CD_RW);
		break;

	case MI_DVDREADONLY://medium is read only, free MB = 0
		theApp.m_pDrive->SetType(DRIVE_DVD_ROM);
		m_pNeroBurn->SetIsDVD(TRUE);
		break;

	case MI_DVDOK:		//medium is an empty DVD and is writable
		theApp.m_pDrive->SetType(DRIVE_DVD_R);
		m_pNeroBurn->SetIsDVD(TRUE);
		break;

	case MI_DVDRWOK:	//medium is an empty DVD-RW and is writable
	case MI_DVDRWDELETE://medium is a not empty DVD and has to be deleted first
		theApp.m_pDrive->SetType(DRIVE_DVD_RW);
		m_pNeroBurn->SetIsDVD(TRUE);
		break;

	case MI_UNKNOWN_MEDIUM:
		{
		//cd drive failed to get correct medium capacity
		theApp.m_pDrive->SetType(DRIVE_UNKNOWN);
		code = EXITCODE_UNKNOWN_MEDIUM;
		}
		break;

	default:
		theApp.m_pDrive->SetType(DRIVE_CDROM);
		break;
	};
			
	theApp.m_pDrive->SetFreeBytesAvailableToCaller(uliFreeBytes);
	theApp.m_pDrive->SetTotalNumberOfBytes(uliTotalBytes);

	theApp.m_Drives.SafeAdd(theApp.m_pDrive);

	return code;
}
/*********************************************************************************************/
void CAcdcDlg::ConfirmVolumeInfos()
{
	m_CtrlOK.EnableWindow(TRUE);
	CIPCServerControlACDC* pSCAcdc = theApp.GetServerControl();
	if(pSCAcdc)
	{
		CIPCBurnDataCarrier* pBDC = pSCAcdc->GetBurnDataCarrier();
		if(pBDC)
		{
			theApp.m_Drives.Lock();
			pBDC->DoConfirmVolumeInfos(theApp.m_Drives);
			theApp.m_Drives.Unlock();
			theApp.m_Drives.SafeDeleteAll();
		}
	}
}

/*********************************************************************************************/
void CAcdcDlg::ConfirmSession()
{
	m_CtrlOK.EnableWindow(TRUE);
	CIPCServerControlACDC* pSCAcdc = theApp.GetServerControl();
	if(pSCAcdc)
	{
		CIPCBurnDataCarrier* pBDC = pSCAcdc->GetBurnDataCarrier();
		if(pBDC)
		{
			AppendMessage(_T("session complete\n"));
			pBDC->DoConfirmSession(pBDC->GetSessionID());
		}
	}
}

/*********************************************************************************************/
CNeroThread* CAcdcDlg::GetNeroThread()
{
	return m_pNeroThread;
}

/*********************************************************************************************/
long CAcdcDlg::OnNewMessage(WPARAM wParam, LPARAM lParam)
{
	//wParam = identifies from which thread this function was called

	CIPCServerControlACDC* pServerControl = theApp.GetServerControl();	

	if(wParam == (NewMessageFrom)NM_SERVER_CONTROL_THREAD) //new message is from CIPCServerControlACDC thread
	{
		//no lParam is used
		if(pServerControl)
		{
			AppendMessage(pServerControl->GetNewMessage());
		}
	}
	else if(wParam == (NewMessageFrom)NM_NERO_BURN_THREAD) //new message is from CNeroThread thread
	{
		//lParam = Nero WaitCD Message (NERO_WAITCD_TYPE) 
		//which indicates an error during burn process 
		if(m_pNeroThread)
		{
			CNeroBurn* pNeroBurn = m_pNeroThread->GetNeroBurn();
			if(pNeroBurn)
			{
				CString sMsg = pNeroBurn->GetNewMessage();
				AppendMessage(sMsg);

				if(    (NERO_WAITCD_TYPE)lParam == NERO_WAITCD_NOTENOUGHSPACE
					|| (NERO_WAITCD_TYPE)lParam == NERO_WAITCD_MEDIUM_UNSUPPORTED)
				{

					CIPCBurnDataCarrier* pBDC = pServerControl->GetBurnDataCarrier();
					if(pBDC)
					{

						pBDC->DoIndicateError(CIPC_DC_REQUEST_SESSION,
											  pBDC->GetSessionID(),
											  CIPC_ERROR_DATA_CARRIER, 
											  DC_ERROR_BURN_PROCESS_FAILED, 
											  sMsg);
					}
					
					m_CtrlOK.EnableWindow(TRUE);
				}
			}
		}
	}
	return 0;
}

/*********************************************************************************************/
BurnProducer CAcdcDlg::GetInstalledBurnSoftware()
{
	BurnProducer bp = BP_NOPRODUCER;
	if(m_pProducer)
	{
		bp = m_pProducer->FindInstalledBurnSoftware();
		CString sBurnProducer = _T("installed burn software is: ");
		BOOL bRuntimeError = FALSE;
		CString sRuntimeError = _T("send runtime error");

		switch (bp)
		{
		case BP_NERO:
			{

			CString sVersion;
			BOOL bVersion = m_pProducer->GetNeroVersion(sVersion);//installed Nero version
			sBurnProducer += _T("Nero ") + sVersion;

			//has to be at least version 5.5.8.2
	//		DWORD dwMinVersion = NERO_MINIMUM_VERSION;
			if(bVersion)
			{
				m_bValidNeroVersion = TRUE;
			}
			else
			{
				AppendMessage(sBurnProducer);
				sBurnProducer = _T("invalid Nero version, must be at least version 5.5.8.2.");
				bp = BP_NOPRODUCER;
				CWK_RunTimeError RTerr(WAI_AC_DC, REL_ERROR, RTE_CONFIGURATION, sBurnProducer, 0, 0);
				RTerr.Send();
				bRuntimeError = TRUE;
			}
			}
			break;

		case BP_DIRECT_CD:
			 sBurnProducer += _T("DirectCD");
			 break;
		case BP_XP:
			sBurnProducer += _T("WindowsXP burn program");
			break;
		case BP_NOPRODUCER:
		default:
			{
			sBurnProducer += _T("no burn software (Nero) installed");
			CWK_Dongle dongle(WAI_AC_DC);
			if(!dongle.IsReceiver())
			{
				CWK_RunTimeError RTerr(WAI_AC_DC, REL_ERROR, RTE_CONFIGURATION, sBurnProducer, 0, 0);
				RTerr.Send();
				bRuntimeError = TRUE;
			}
			}
			break;
		}
		//current process has finished, installed burn software detected, stop NeroThread
		WK_TRACE(_T("%s\n"),sBurnProducer);
		AppendMessage(sBurnProducer);

		if(bRuntimeError)
		{
			AppendMessage(sRuntimeError);
		}
	}


	return bp;
}

/*********************************************************************************************/
void CAcdcDlg::SetFilesToBackup(const CStringArray& sPathnames)
{
	m_saFilesToBackup.RemoveAll();
	for(int i = 0; i < sPathnames.GetSize(); i++)
	{
		//first check if all element not empty
		if(!sPathnames.GetAt(i).IsEmpty())
		{
			m_saFilesToBackup.Add(sPathnames.GetAt(i));
		}
	}

}

/*********************************************************************************************
 Class		   : CAcdcDlg
 Function      : SetVolumeName 
 Description   : This function return the name of the CD/DVD to burn onto.

 Parameters: 
  sName: (I):	the medium name (CString)

 Result type: --

 Author: TKR
 created: September, 18 2002
 <TITLE SetVolumeName >
 <GROUP CAcdcDlg>
 <TOPICORDER 0>
 <KEYWORDS CAcdcDlg, SetVolumeName>
*********************************************************************************************/
void CAcdcDlg::SetVolumeName(CString sName)
{
	m_sVolumeName = sName;
}

/*********************************************************************************************/
BOOL CAcdcDlg::IsValidNeroVersion()
{
	return m_bValidNeroVersion;
}

/*********************************************************************************************/
void CAcdcDlg::OnException()
{
	OnOK();
}
/*********************************************************************************************/
void CAcdcDlg::OnClose() 
{
	OnOK();
}
/*********************************************************************************************/
void CAcdcDlg::OnAppExit()
{
	OnOK();
}

/////////////////////////////////////////////////////////////////////////////
long CAcdcDlg::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
		::PostMessage(hWnd, WM_SELFCHECK, WAI_AC_DC, 0);
	return 0;
}

void CAcdcDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	NOTIFYICONDATA tnd;

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;
	tnd.uFlags		= 0;

	Shell_NotifyIcon(NIM_DELETE, &tnd);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CAcdcDlg::OnTrayClicked(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
		case WM_LBUTTONDBLCLK:
			ShowWindow(SW_RESTORE); //SW_SHOWNORMAL);
			break;
		case WM_RBUTTONDOWN:
		{
			CMenu menu;
			CMenu* pM;
			CPoint pt;

			GetCursorPos(&pt);
			menu.LoadMenu(IDR_MAINFRAME);
			pM = menu.GetSubMenu(0);

			SetForegroundWindow();		// Siehe ID: Q135788 
			pM->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
			PostMessage(WM_NULL, 0, 0); // Siehe ID: Q135788
			break;
		}
	}

	return 0;
}

void CAcdcDlg::OnAbout() 
{
	COemGuiApi::AboutDialog(this);
}

void CAcdcDlg::OnSysCommand(UINT nID, LPARAM lParam) 
{
	CDialog::OnSysCommand(nID, lParam);
	if (nID == SC_MINIMIZE)
		ShowWindow(SW_HIDE);
}

/////////////////////////////////////////////////////////////////////////////
//auf ein OnDeviceChange (durch USB) darf nur reagiert werden, wenn die NeroAPI.dll
//nicht gerade benutzt wird. Dies ist der Fall bei der m_MainFunktion != MF_START
//NeroAPI.dll braucht nur entladen werden, wenn sie geladen ist, 
//also CBurnContext::m_bNeroInitialized == FALSE ist
//ebenfalls FALSE liefern, wenn mehr als eine OnDeviceChange Msg kommt
//(sie kommt beim USB reinstecken stets 2-3 mal)
BOOL CAcdcDlg::CanChangeDevice()
{
	BOOL bRet = FALSE;

	//nur auf USB OnChangeDevice reagieren, wenn gerade im StartStatus
	if(GetMainFunction() == MF_START)
	{
		if(m_pNeroBurn)
		{
			bRet = m_pNeroBurn->IsNeroAPIInitialized();
			WK_TRACE(_T("*** CanChangeDevice() Is API init. : %d\n"), bRet);
		}	
		else
		{
			TRACE(_T("*** CanChangeDevice() m_pNeroBurn == NULL\n"));
		}
	}
	
	TRACE(_T("*** CanChangeDevice() MainFunktion != MF_START\n"));
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAcdcDlg::OnDeviceChange(UINT nEventType, DWORD dwData)
{
	DEV_BROADCAST_HDR* pHdr = (DEV_BROADCAST_HDR*)dwData;
	BOOL bRet = TRUE;

	if (pHdr)
	{
		if (pHdr->dbch_devicetype == DBT_DEVTYP_VOLUME)
		{
			DEV_BROADCAST_VOLUME *pDBV  = (DEV_BROADCAST_VOLUME*)pHdr;
			_TCHAR  szDrive[] = _T("A:");
			int   nDrive;
			DWORD dwUnitMask = pDBV->dbcv_unitmask;
			for (nDrive=-1; nDrive<32 && dwUnitMask!=0; nDrive++) 
			{
				dwUnitMask >>= 1;
			}
			szDrive[0] = (_TCHAR)(_T('A') + nDrive);
			UINT nDriveType = GetDriveType(szDrive);
			switch (nEventType)
			{
				case DBT_DEVICEARRIVAL:
				if(nDriveType == DRIVE_CDROM)
				{
					if (pDBV->dbcv_flags & DBTF_MEDIA)
					{
						// nur Medium ins Laufwerk eingelegt, kein neues Laufwerk erkannt
						//neu erkannte Laufwerke (CDROM) werde in OnNotifyDrive() erkannt
						WK_TRACE(_T("Device %s (%d) arrival\n"), szDrive, nDriveType);
					}
					else
					{			
						bool bPlugged = true;
						PostMessage(WM_BURN_NOTIFY_DRIVE, MAKELONG(nDriveType, bPlugged), pDBV->dbcv_unitmask);
					}
				}
				break;
				
				case DBT_DEVICEREMOVECOMPLETE:
				if((nDriveType == DRIVE_CDROM) && (pDBV->dbcv_flags & DBTF_MEDIA))
				{
					//WK_TRACE(_T("Device %s (%d) arrival\n"), szDrive, nDriveType);
				}
				else if(nDriveType == DRIVE_NO_ROOT_DIR)
				{
					bool bPlugged = false;
					PostMessage(WM_BURN_NOTIFY_DRIVE, MAKELONG(nDriveType, bPlugged), pDBV->dbcv_unitmask);
				}
				break;
				
				case DBT_DEVICEQUERYREMOVE:       WK_TRACE(_T("Drive %s query remove\n")  , szDrive); 
				break;

				case DBT_DEVICEREMOVEPENDING:     WK_TRACE(_T("Drive %s remove pending\n"), szDrive); 
				break;

				case DBT_DEVICEQUERYREMOVEFAILED: WK_TRACE(_T("Drive %s remove failed\n") , szDrive); 
				break;

				default: 
				break;
			} // End of switch block

		}

	}
	return bRet;
}

/*********************************************************************************************
 Class      : CAcdcDlg
 Function   : OnNotifyDrive
 Description: Notification for arrival or removal of hotplug drives.

 Parameters:   
  wParam   : (E): LOWORD: Drive type, HIWORD: Device plugged (true, false)  (WPARAM)
  lUnitMask: (E): Unit Bit Mask for the drive letter  (LPARAM)

 Result type: Return zero (long)
 Author: TKR
 created: October, 15 2002
 <TITLE OnNotifyDrive >
 <GROUP CAcdcDlg>
 <TOPICORDER 0>
 <KEYWORDS CAcdcDlg, OnNotifyDrive>
*********************************************************************************************/
long CAcdcDlg::OnNotifyDrive(WPARAM wParam, LPARAM lUnitMask)
{
	int  nDriveType = (int) LOWORD(wParam);
	BOOL bPlugged   = (BOOL)HIWORD(wParam);	
	CString sMsg;


	CString sDrive(_T("A"));
	int   nDrive;
	DWORD dwCurrentUnitMask = lUnitMask;
	for (nDrive=-1; nDrive<32 && lUnitMask!=0; nDrive++) 
	{
		lUnitMask >>= 1;
	}
	sDrive = (char)(_T('A') + nDrive);
	sDrive.MakeLower();

	if (nDriveType == DRIVE_CDROM)
	{
		if (m_dwUsedDriveLetter != dwCurrentUnitMask)
		{
			sMsg.Format(_T("Found new CD/DVD drive: %s \n"), sDrive);
			AppendMessage(sMsg);
			
			if(m_bIsInBackup)
			{
				AppendMessage(_T("not restart ACDC, backup is running"));
			}
			else
			{
				AppendMessage(_T("restart ACDC"));
				OnOK();
			}

		}	
	}

	if (   (nDriveType == DRIVE_NO_ROOT_DIR)					// on removal
		 || (!bPlugged))
	{
		if (m_dwUsedDriveLetter == dwCurrentUnitMask)
		{
			sMsg.Format(_T("CD/DVD drive: %s removed\n"), sDrive);
			AppendMessage(sMsg);

			if(    m_bIsInBackup 
				&& sDrive != m_sUsedDriveLetter)
			{
				//do not reset ACDC while backup is active to not currently removed drive
				AppendMessage(_T("not restart ACDC, backup is running"));
			}
			else
			{
				AppendMessage(_T("restart ACDC"));
				OnOK();
			}
		}
	}
	return 0;
}

/*********************************************************************************************
 Class      : CAcdcDlg
 Function   : SetCurrentDrive
 Description: Sets the current drive letter of the used drive to burn to.

 Parameters:   
  sDriveLetter   : (E): the current drive letter  (CString)
 

 Result type: --
 
 Author: TKR
 created: October, 15 2002
 <TITLE SetCurrentDrive >
 <GROUP CAcdcDlg>
 <TOPICORDER 0>
 <KEYWORDS CAcdcDlg, SetCurrentDrive>
*********************************************************************************************/
void CAcdcDlg::SetCurrentDrive(CString sDriveLetter)
{
	//save current drive as DWORD
	char cValue;
	CString sDrive;
	m_dwUsedDriveLetter = 1;
	m_sUsedDriveLetter.Empty();

	for(int i = 0; i <= 32; i++)
	{
		m_dwUsedDriveLetter <<= 1;
		cValue = (char)(i + 66);
		sDrive = cValue;
		sDrive.MakeLower();
		if(sDrive == sDriveLetter)
		{
			m_sUsedDriveLetter = sDrive;
			break;
		}
	}
}
/*********************************************************************************************
 Class      : CAcdcDlg
 Function   : GetUsedDriveLetter
 Description: Gets the current drive letter of the used drive to burn to.

 Parameters:   --
 
 Result type: 
  sUsedDrive: (E): the current drive letter  (CString)
 
 Author: TKR
 created: October, 16 2002
 <TITLE GetUsedDriveLetter >
 <GROUP CAcdcDlg>
 <TOPICORDER 0>
 <KEYWORDS CAcdcDlg, GetUsedDriveLetter>
*********************************************************************************************/
CString CAcdcDlg::GetUsedDriveLetter()
{
	return m_sUsedDriveLetter;
}

/*********************************************************************************************
 Class      : CAcdcDlg
 Function   : SetIsInBackup
 Description: Sets the information, that ACDC currently in in backup

 Parameters:   (E): if TRUE, backup is currently running  (BOOL)
 
 Result type: --
 
 
 Author: TKR
 created: November, 22 2002
 <TITLE SetIsInBackup >
 <GROUP CAcdcDlg>
 <TOPICORDER 0>
 <KEYWORDS CAcdcDlg, SetIsInBackup>
*********************************************************************************************/
void CAcdcDlg::SetIsInBackup(BOOL bIsInBackup)
{
	m_bIsInBackup = bIsInBackup;
}

/*********************************************************************************************/
CProducer* CAcdcDlg::GetProducer()
{
	return m_pProducer;
}