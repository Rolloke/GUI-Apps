// RemoteCDDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RemoteCD.h"
#include "RemoteCDDlg.h"
#include "wk_msg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRemoteCDDlg dialog

//constructor
CRemoteCDDlg::CRemoteCDDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRemoteCDDlg::IDD, pParent),
	m_RemountThread(NULL, "", "&Remount"),
	m_FinalizeThread(NULL, "", "&Finalisiere...")
{
	//{{AFX_DATA_INIT(CRemoteCDDlg)
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_sEditText				= "";
	m_hWndPCD				= NULL;
	m_hChildWndPCD			= NULL;
	m_hFormatButton			= NULL;
	m_hDVClient				= NULL;
	m_hFinalizeStartDlg		= NULL;
	m_hFirstChild			= NULL;
	m_hFinalizeUDFDlg		= NULL;
	m_sDriveLetter			= "";
	m_sCmdLineFirstParam	= "";

	//get path to PacketCD folder
	m_sPathToPacketCD = GetPathToPacketCD();

// initialize all variables for working with PacketCD
	//actual version of PacketCD:		250.042
	//actual version of Formatierer:	2, 0, 0, 41
	//if the versions changes, check the new ID`s and the new buttontexts with spy++
	Init();


	CSystemTime sysTime;
	CString sDateTime;
	sysTime.GetLocalTime();
	sDateTime.Format(_T("%02d_%02d_%4d, %02d:%02d:%02d"),
			sysTime.GetDay(),
			sysTime.GetMonth(),
			sysTime.GetYear(),
			sysTime.GetHour(),
			sysTime.GetMinute(),
			sysTime.GetSecond()
			);
	m_sInitNewCDName	= sDateTime;

	m_RemoteCDStatus	= RS_UNDEFINED;
	m_FormatStatus		= FORMAT_UNDEFINED;
	m_FinalizeStatus	= FS_UNDEFINED;
	m_RemountStatus		= MOUNT_UNDEFINED;
	m_bFinalizeComplete	= FALSE;
	m_bFinalizeFailed	= FALSE;
	m_dwStart			= GetTickCount();
	m_dwWaitTimeout		= GetTickCount();
	m_dwStartFormat = 0;
	m_dwWaitFormat = 0;
	m_dwWaitAfterFormat = 0;
	m_dwWaitAfterFinalize = 0;

	//get CmdLine strings
	m_sCmdLine = GetCommandLine();

	int nIndex = m_sCmdLine.Find(_T(".exe"));
	int nLength = m_sCmdLine.GetLength();

	if (nIndex != -1)
	{
		m_sCmdLine = m_sCmdLine.Right(nLength-(nIndex+5));
		m_sCmdLine.TrimLeft();
	}

	WK_TRACE(_T("Constructor(): m_sCmdLine: %s\n"), m_sCmdLine);	


 	CheckCmdLine();

	Create(IDD);
}


//destructor
CRemoteCDDlg::~CRemoteCDDlg()
{

}

void CRemoteCDDlg::PostNcDestroy() 
{
	delete this;
}

void CRemoteCDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRemoteCDDlg)
	DDX_Control(pDX, IDC_EDIT2, m_Edit2);
	DDX_Control(pDX, IDC_EDIT1, m_Edit1);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRemoteCDDlg, CDialog)
	//{{AFX_MSG_MAP(CRemoteCDDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_CHECK_BACKUP_STATUS, OnReceiveStatus)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRemoteCDDlg message handlers

BOOL CRemoteCDDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
//	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	

	m_hDVClient = ::FindWindow(WK_APP_NAME_DVCLIENT,NULL);

	if (m_sCmdLineFirstParam == "")
	{
		// Call by PacketCD
		m_RemoteCDStatus = RS_REQUEST_STATUS;
	}
	else if(m_sCmdLineFirstParam == "-FinalizeCD" 
					&& m_sDriveLetter.GetLength() == 3)
	{
		m_RemoteCDStatus = RS_FINALIZE;
		m_FinalizeStatus = FS_BEGIN;
	}
	else if(m_sCmdLineFirstParam == "-RemountCDR")
	{
		m_RemoteCDStatus = RS_REMOUNT;
		m_RemountStatus = MOUNT_BEGIN;
	}
	else if(m_sCmdLineFirstParam == "-Remount__2")
	{
		//remount CD-R again, after formatting succeeds
		m_RemoteCDStatus = RS_REMOUNT_AGAIN;
		m_RemountStatus = MOUNT_BEGIN;
		RemountCD(FALSE);
	}
	else
	{
		WK_TRACE_ERROR(_T("unknown command line param %s\n"),m_sCmdLineFirstParam);
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRemoteCDDlg::OnPaint() 
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
HCURSOR CRemoteCDDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/////////////////////////////////////////////////////////////////////////////
void CRemoteCDDlg::OnIdle()
{
	switch(m_RemoteCDStatus)
	{
	case RS_REQUEST_STATUS:
		if (RequestStatusDVClient())
		{
			//DVClient is running
			m_dwWaitTimeout = GetTickCount();
			m_RemoteCDStatus = RS_WAIT;
		}
		else
		{
			//DVClient doesn`t run
			m_RemoteCDStatus = RS_QUIT;
		}
		break;

	case RS_WAIT:
		if(GetTimeSpan(m_dwWaitTimeout) > 10*1000)
		{
			m_RemoteCDStatus = RS_QUIT;
		}
		break;

	case RS_REMOUNT:
		RemountCD(TRUE); //TRUE = first call of remount includes a 
						// msg back to DVClient, if`remount was successfull
		PostMessage(WM_USER);
		break;

	case RS_REMOUNT_AGAIN:
		RemountCD(FALSE);	 //TRUE = first call of remount includes a 
							// msg back to DVClient, if remount was successfull
		PostMessage(WM_USER);
		break;

	case RS_FORMAT:
		OnFormat();
		PostMessage(WM_USER);
		break;

	case RS_FINALIZE:
		OnFinalize();
		PostMessage(WM_USER);
		break;

	case RS_QUIT:
		OnOK();
		break;

	default:
		WK_TRACE_ERROR(_T("RemoteCDStatus default: %s\n"), NameOfEnum(BS_NO_STATUS,
												m_RemoteCDStatus));
		OnOK();
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////
long CRemoteCDDlg::OnReceiveStatus(WPARAM wParam, LPARAM lParam)
{
	WK_TRACE(_T("OnReceiveStatus: Status: %s\n"), NameOfEnum((BackupStatus)wParam));

	switch (wParam)
	{
	case BS_NOT_IN_BACKUP:  //DVClient not in backup
		//close RemoteCD
		WK_TRACE(_T("OnReceiveStatus: DVClient not in backup\n"));
		m_RemoteCDStatus = RS_QUIT;
		break;

	case BS_IN_BACKUP: //DVClient in backup
		m_RemoteCDStatus = RS_FORMAT;
		m_FormatStatus = FORMAT_START;
		//get drive letter of CD-writer
		m_sDriveLetter = CString((char)lParam)+":\\";
		PostMessage(WM_USER);
		break;
	default:
		break;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CRemoteCDDlg::RequestStatusDVClient()
{
	BOOL bRet = FALSE;
	WK_TRACE(_T("RequestStatusDVClient(): Beginn\n"));

	m_hDVClient = ::FindWindow(WK_APP_NAME_DVCLIENT,NULL);
	if(IsWindow(m_hDVClient))
	{
		bRet = TRUE;
		BOOL bSendOK = ::PostMessage(m_hDVClient, WM_CHECK_BACKUP_STATUS, BS_REQUEST_STATUS, (LPARAM)this->m_hWnd);
		if(!bSendOK)
		{
			WK_TRACE(_T("CRequestDVClient: Could not post message to DVClient window\n"));
		}
		else
		{
			WK_TRACE(_T("RequestStatusDVClient: Message sent to DVClient\n"));
		}
	}
	else
	{
		WK_TRACE(_T("RemoteCD: DVClient not found on desktop\n"));
		m_RemoteCDStatus = RS_QUIT;
	}

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CRemoteCDDlg::ExecutePacketCD()
{
	WK_TRACE(_T("ExecutePacketCD(): Beginn\n"));
	BOOL bRet = FALSE;
	BOOL bProcess;

	STARTUPINFO			startUpInfo;
	PROCESS_INFORMATION prozessInformation;
	CString sPrgToStart;

	startUpInfo.cb				= sizeof(STARTUPINFO);
    startUpInfo.lpReserved      = NULL;
    startUpInfo.lpDesktop       = NULL;
    startUpInfo.lpTitle			= NULL;
    startUpInfo.dwX				= 0;
    startUpInfo.dwY				= 0;
    startUpInfo.dwXSize			= 0;
    startUpInfo.dwYSize			= 0;
    startUpInfo.dwXCountChars   = 0;
    startUpInfo.dwYCountChars   = 0;
    startUpInfo.dwFillAttribute = 0;
	startUpInfo.dwFlags			= STARTF_USESHOWWINDOW;
    startUpInfo.wShowWindow     = SW_HIDE;
    startUpInfo.cbReserved2     = 0;
    startUpInfo.lpReserved2     = NULL;

	CString sExe = m_sPathToPacketCD + "\\udforig.exe";
	sPrgToStart = '"' + sExe +'"' + " " + m_sCmdLine;
	WK_TRACE(_T("ExecutePacketCD(): sPrgToStart: %s\n"), sPrgToStart);

	bProcess = CreateProcess(
				(LPCTSTR)sExe,				// pointer to name of executable module 
				(LPTSTR)sPrgToStart.GetBuffer(0),		// pointer to command line string
				(LPSECURITY_ATTRIBUTES)NULL,// pointer to process security attributes 
				(LPSECURITY_ATTRIBUTES)NULL,// pointer to thread security attributes 
				FALSE,						// handle inheritance flag 
				NORMAL_PRIORITY_CLASS,		//|DEBUG_PROCESS|DEBUG_ONLY_THIS_PROCESS,	// creation flags 
				NULL,						// pointer to new environment block 
				NULL,						// pointer to current directory name 
				&startUpInfo,				// pointer to STARTUPINFO 
				&prozessInformation 		// pointer to PROCESS_INFORMATION  
							);
	sPrgToStart.ReleaseBuffer();	

	if (bProcess && prozessInformation.hProcess != NULL)
	{
		DWORD dwWFII;
		dwWFII = WaitForInputIdle(prozessInformation.hProcess,10*1000);
		if (dwWFII == WAIT_TIMEOUT)
		{
			WK_TRACE(_T("ExecutePacketCD(): WaitForInputIdle() time out\n")); 
		}

		WK_TRACE(_T("ExecutePacketCD(): Start waiting for PacketCD\n"));
		m_FormatStatus = FORMAT_WAITING;
		m_dwWaitFormat = GetTickCount();
		bRet = TRUE;
	}
	else
	{
		// TODO
		WK_TRACE(_T("ExecutePacketCD: Could not start %s\n Error: %s\n"), 
										sPrgToStart, GetLastErrorString());
	}

	return bRet;
}


/////////////////////////////////////////////////////////////////////////////

BOOL CRemoteCDDlg::MsgDVClientFormatStatus(BOOL bSuccess)
{
	BOOL bRet = FALSE;

	if(bSuccess) //formatting was successfull
	{
		if(IsWindow(m_hDVClient))
		{
			//send Msg to DVClient: Formatting successfull
			BOOL bSendOK = ::PostMessage(m_hDVClient, WM_CHECK_BACKUP_STATUS, 
										BS_FORMAT_SUCCESS, (LPARAM)this->m_hWnd);
			if(!bSendOK)
			{
				WK_TRACE(_T("MsgDVClientFormatStatus(): Could not post message %s to DVClient\n"),
										NameOfEnum(BS_FORMAT_SUCCESS));
			}
			else
			{
				bRet = TRUE;
				WK_TRACE(_T("MsgDVClientFormatStatus(): Message %s sendet to DVClient\n"),
										NameOfEnum(BS_FORMAT_SUCCESS));
			}
		}
		else
		{	
			WK_TRACE(_T("MsgDVClientFormatStatus(): DVClient not found, m_hDVClient= 0x%x\n"), m_hDVClient);
		}
	}
	else		//formatting was NOT successfull
	{
		if(IsWindow(m_hDVClient))
		{
			//send Msg to DVClient: Formatting NOT successfull
			BOOL bSendOK = ::PostMessage(m_hDVClient, WM_CHECK_BACKUP_STATUS, 
										BS_FORMAT_FAILED, (LPARAM)this->m_hWnd);
			if(!bSendOK)
			{
				WK_TRACE(_T("MsgDVClientFormatStatus: Could not post message %s to DVClient\n"),
							NameOfEnum(BS_FORMAT_FAILED));
			}
			else
			{
				bRet = TRUE;
				WK_TRACE(_T("MsgDVClientFormatStatus: Message sended to DVClient\n"),
							NameOfEnum(BS_FORMAT_FAILED));
			}
		}
	}

	return bRet;
}





/////////////////////////////////////////////////////////////////////////////
void CRemoteCDDlg::CheckCmdLine()
{

	if(m_sCmdLine.GetLength() > 11 && m_sCmdLine.GetLength() < 16)
	{
		//parameter: -FinalizeCD i: or -RemountCDR i:
		m_sCmdLineFirstParam = m_sCmdLine.Left(11);
		m_sDriveLetter = m_sCmdLine.Right((m_sCmdLine.GetLength()-12));
	}
}


////////////////////////////////////////////////////////////////////////////
void CRemoteCDDlg::RemountCD(BOOL bSendMsg)
{
	m_hDVClient = ::FindWindow(WK_APP_NAME_DVCLIENT,NULL);

	if (IsWindow(m_hDVClient))
	{
		if (!m_RemountThread.IsRunning() && (m_RemountStatus == MOUNT_BEGIN))
		{
			m_RemountThread.SetPath(m_sDriveLetter);
			CString sMenuItem = GetStringFromTable("c2udf.dll", 51);//51 = &Remount
			WK_TRACE(_T("RemountCD():  MenuItem on backup drive: %s\n"), sMenuItem);
			m_RemountThread.SetMenuItem(sMenuItem); 
			m_RemountThread.StartThread();
			WK_TRACE(_T("RemountCD(): Remount Thread started\n"));
			m_RemountStatus = MOUNT_IS_REMOUNTING;
		}
		else
		{
			switch(m_RemountStatus)
			{
			case MOUNT_IS_REMOUNTING:

				if(m_RemountThread.IsRunning())
				{
					m_RemountStatus = MOUNT_WAIT_FOR_END;
				}
				else
				{
					if(GetTimeSpan(m_dwStart) > 30*1000)
					{
						m_RemountStatus = MOUNT_FAILED;
					}

				}
				break;
			case MOUNT_WAIT_FOR_END:

				if(GetTimeSpan(m_dwStart) <= 30*1000)
				{
					if(!m_RemountThread.IsRunning())
					{
						//Remount successfull
						m_RemountStatus = MOUNT_SUCCESS;
						WK_TRACE(_T("RemountCD(): Remount success\n"));
					}
				}
				else
				{
					//Remount timeout after 30 sec.
					m_RemountStatus = MOUNT_FAILED;
					WK_TRACE(_T("RemountCD(): Remount failed after 30 sec.\n"));	
				}

				break;

			case MOUNT_SUCCESS:
				{
					//only send msg to DVClient, if it is the first call of
					//this function. First call is right after DVClient starts
					//with backup. This first remount checks if a cd-r is
					//already in the cd-writer
					if (bSendMsg)
					{
						BOOL bSendOK = ::PostMessage(m_hDVClient, 
											WM_CHECK_BACKUP_STATUS,
											BS_REMOUNT_SUCCESS, 0);
						if(!bSendOK)
						{
							WK_TRACE(_T("RemountCD(): could not post message BS_REMOUNT_SUCCESS to DVClient window\n"));
						}
						else
						{
							WK_TRACE(_T("RemountCD(): Message BS_REMOUNT_SUCCESS to DVClient sended\n"));
						}
					}
					else
					{
						//don`t send a msg to DVClient, because it`s the remount
						//after formatting. Just to update the CD-writer
						WK_TRACE(_T("RemountCD(): remount after formatting succeeds\n"));
					}
				}
				m_RemoteCDStatus = RS_QUIT;
				break;

			case MOUNT_FAILED:
				{
				BOOL bSendOK = ::PostMessage(m_hDVClient, 
									WM_CHECK_BACKUP_STATUS, 
									BS_REMOUNT_FAILED, 0);
				if(!bSendOK)
				{
					WK_TRACE(_T("RemountCD(): Could not post message BS_REMOUNT_FAILED to DVClient window\n"));
				}
				else
				{
					WK_TRACE(_T("RemountCD(): Message BS_REMOUNT_FAILED to DVClient sended\n"));
				}
				m_RemoteCDStatus = RS_QUIT;
				break;
				}
			default:
				WK_TRACE(_T("RemountCD(): RemountStatus: %s\n"), NameOfEnum(BS_NO_STATUS,
														RS_NO_STATUS,
														FS_NO_STATUS,
														m_RemountStatus));
				break;
			}
		}
	}
	else
	{
		// kein Client mehr da OOPS
		WK_TRACE_ERROR(_T("RemountCD(): no DVCLIENT any more\n"));
		m_RemoteCDStatus = RS_QUIT;
	}
}


////////////////////////////////////////////////////////////////////////////
void CRemoteCDDlg::OnCancel() 
{
	DestroyWindow();
}

////////////////////////////////////////////////////////////////////////////
void CRemoteCDDlg::OnOK() 
{
	DestroyWindow();
}

////////////////////////////////////////////////////////////////////////////
CString CRemoteCDDlg::NameOfEnum(BackupStatus bs,
								   RemoteCDStatus rs,
								   FinalizeStatus fs,
								   RemountStatus mount)
{
	if(bs != BS_NO_STATUS)
	{
		switch(bs)
		{
		case BS_NOT_IN_BACKUP:		return "BS_NOT_IN_BACKUP";
		case BS_IN_BACKUP:			return "BS_IN_BACKUP";
		case BS_FORMAT_SUCCESS:		return "BS_FORMAT_SUCCESS";
		case BS_FORMAT_FAILED:		return "BS_FORMAT_FAILED";
		case BS_FINALIZE_SUCCESS:	return "BS_FINALIZE_SUCCESS";
		case BS_FINALIZE_FAILED:	return "BS_FINALIZE_FAILED";
		case BS_REMOUNT_SUCCESS:	return "BS_REMOUNT_SUCCESS";
		case BS_REMOUNT_FAILED:		return "BS_REMOUNT_FAILED";
		case BS_REQUEST_STATUS:		return "BS_REQUEST_STATUS";

		default:					return "Unknown Status";
		}
	}

	else if(rs != RS_NO_STATUS)
	{
		switch(rs)
		{

		case RS_UNDEFINED:		return "RS_UNDEFINED";
		case RS_REQUEST_STATUS:	return "RS_REQUEST_STATUS";
		case RS_REMOUNT:		return "RS_REMOUNT";
		case RS_FORMAT:			return "RS_FORMAT";
		case RS_REMOUNT_AGAIN:	return "RS_REMOUNT_AGAIN";
		case RS_FINALIZE:		return "RS_FINALIZE";
		case RS_WAIT:			return "RS_WAIT";
		case RS_QUIT:			return "RS_QUIT";

		default:				return "Unknown Status";
		}
	}

	else if(fs != FS_NO_STATUS)
	{

		switch(fs)
		{

		case FS_UNDEFINED:					return "FS_UNDEFINED";
		case FS_BEGIN:						return "FS_BEGIN";
		case FS_WAIT_FOR_FIRST_DLG:			return "FS_WAIT_FOR_FIRST_DLG";
		case FS_CLICK_BUTTON:				return "FS_CLICK_BUTTON";
		case FS_WAIT_FOR_PROGRESS_DLG:		return "FS_WAIT_FOR_PROGRESS_DLG";
		case FS_WAIT_FOR_FINALIZE_COMPLETE:	return "FS_WAIT_FOR_FINALIZE_COMPLETE";
		case FS_SUCCESS:					return "FS_SUCCESS";
		case FS_FAILED:						return "FS_FAILED";

		default:				return "Unknown Status";
		}
	}
	
	else if(mount != MOUNT_NO_STATUS)
	{

		switch(mount)
		{

		case MOUNT_UNDEFINED:				return "MOUNT_UNDEFINED";
		case MOUNT_BEGIN:					return "MOUNT_BEGIN";
		case MOUNT_IS_REMOUNTING:			return "MOUNT_IS_REMOUNTING";
		case MOUNT_WAIT_FOR_END:			return "MOUNT_WAIT_FOR_END";
		case MOUNT_SUCCESS:					return "MOUNT_SUCCESS";
		case MOUNT_FAILED:					return "MOUNT_FAILED";

		default:				return "Unknown Status";
		}
	}
	else
	{
		return "Unknown NameOfEnum-call";
	}
}

////////////////////////////////////////////////////////////////////////////
CString CRemoteCDDlg::GetPathToPacketCD()
{
	CString sRet = "";

	CString cSubKey;
	DWORD dwType = REG_SZ;
	DWORD dwSize;

	HKEY hPacketCD;

	cSubKey = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
	
	WK_TRACE(_T("GetPathToPacketCD(): Start open Registry\n"));
	// open Registry 
	//get path of PacketCD from registry without using CIPC
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
					cSubKey,
					0,
					KEY_READ,
					&hPacketCD) == ERROR_SUCCESS)

	{
		CString sPacketCD = "PacketCD";
		RegQueryValueEx(hPacketCD,
					   sPacketCD,
					   NULL,
					   &dwType,
					   NULL,
					   &dwSize);

		char* psBuffer = new char[dwSize];
		if(RegQueryValueEx(hPacketCD,
					   sPacketCD,
					   NULL,
					   &dwType,
					   (PBYTE)psBuffer,
					   &dwSize) == ERROR_SUCCESS)
		{
			CString sPath(psBuffer, dwSize);
			sRet = sPath.Left(sPath.ReverseFind('\\'));
		}
		WK_DELETE(psBuffer);
	}
	RegCloseKey(hPacketCD);

	return sRet;
}

////////////////////////////////////////////////////////////////////////////
//get status of hook from registry. If TRUE, all PacketCD windows are invisible
BOOL CRemoteCDDlg::GetHookStatusFromReg()
{
	BOOL bHookStatus = FALSE;

	CString cSubKey;
	DWORD dwType = REG_DWORD;
	DWORD dwSize;

	HKEY hHookStatus;

	cSubKey = "SOFTWARE\\dvrt\\dv\\dvstarter";
	
	WK_TRACE(_T("GetHookStatusFromReg(): Start open Registry\n"));
	// open Registry 
	//get path of PacketCD from registry without using CIPC
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
					cSubKey,
					0,
					KEY_READ,
					&hHookStatus) == ERROR_SUCCESS)

	{
		CString sHook = "Hook";
		DWORD dwValue = 0;
		if(RegQueryValueEx(hHookStatus,
					   sHook,
					   NULL,
					   &dwType,
					   (PBYTE)&dwValue,
					   &dwSize) == ERROR_SUCCESS)
		{
			bHookStatus = dwValue;
		}
	}
	RegCloseKey(hHookStatus);

	return bHookStatus;
}
///////////////////////////////////////////////////////////////////////
void CRemoteCDDlg::OnFormat()
{
	switch (m_FormatStatus)
	{
	case FORMAT_UNDEFINED:
		// TODO
		break;
	case FORMAT_START:
		OnFormatStart();
		break;
	case FORMAT_WAITING:
		OnFormatWaiting();
		break;
	case FORMAT_CLICKING:
		OnFormatClicking();
		break;
	case FORMAT_FORMATTING:
		OnFormatFormatting();
		break;
	case FORMAT_FORMATTED:
		OnFormatFormatted();
		break;
	default:
		WK_TRACE(_T("OnFormat(): unknown format status\n"));
		break;
	}
}
///////////////////////////////////////////////////////////////////////
void CRemoteCDDlg::OnFormatStart()
{

	WK_TRACE(_T("OnFormatStart(): m_sPathToPacketCD: %s\n"), m_sPathToPacketCD);
	WK_TRACE(_T("OnFormatStart(): m_sDriveLetter: %s\n"), m_sDriveLetter);
	if(   m_sPathToPacketCD.IsEmpty() 
	   || m_sDriveLetter.IsEmpty())
	{
		WK_TRACE(_T("OnFormatStart(): path to PacketCD or BackupDrive not found\n"));
		m_RemoteCDStatus = RS_QUIT;
	}
	else
	{
		//start format CD-R
		if (ExecutePacketCD())
		{
			//CD-R was successfully formatted
			m_FormatStatus = FORMAT_WAITING;
		}
		else
		{
			m_hDVClient = ::FindWindow(WK_APP_NAME_DVCLIENT,NULL);
			//CD-R was not successfully formatted
			BOOL bSendOK = ::PostMessage(m_hDVClient, WM_CHECK_BACKUP_STATUS, BS_FORMAT_FAILED, 0);
			if(!bSendOK)
			{
				WK_TRACE(_T("OnFormatStart(): Could not post BS_FORMAT_FAILED to DVClient window\n"));	
			}
			else
			{
				WK_TRACE(_T("OnFormatStart(): Message BS_FORMAT_FAILED to DVClient sended\n"));
			}

			m_RemoteCDStatus = RS_QUIT;  //quit RemoteCD
		}
	}
}
///////////////////////////////////////////////////////////////////////
void CRemoteCDDlg::OnFormatWaiting()
{
	m_hWndPCD = ::FindWindow(NULL, m_sInitWndTxtPCD);

	if (m_hWndPCD && IsWindow(m_hWndPCD))
	{
		// Packet CD ist da
		WK_TRACE(_T("OnFormatWaiting(): found PacketCD dialog\n"));
		::ShowWindow(m_hWndPCD, SW_HIDE);
		// alle Button klicken
		m_FormatStatus = FORMAT_CLICKING;
	}
	else
	{
		// noch nicht da 
		if (GetTimeSpan(m_dwWaitFormat) > 120*1000)
		{
			MsgDVClientFormatStatus(FALSE);
			WK_TRACE(_T("OnFormatWaiting(): not found PacketCD dialog after 120s\n"));
			m_RemoteCDStatus = RS_QUIT;
		}
	}
}
///////////////////////////////////////////////////////////////////////
void CRemoteCDDlg::OnFormatClicking()
{
	if (StartFormatCD()) //starts formatting and clicks format-button
	{
		m_dwStartFormat = GetTickCount();
		m_FormatStatus = FORMAT_FORMATTING;
	}
	else
	{
		MsgDVClientFormatStatus(FALSE);
		m_RemoteCDStatus = RS_QUIT;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRemoteCDDlg::StartFormatCD()
{
	BOOL	bRet = FALSE;
	HWND	hFirstChild = NULL;
	HWND	hSecondChild = NULL;
	WORD	wIDEditField;
	TCHAR	szWndText[128];

	//
	//get every child window of packed_cd desktop window and
	//find that childwindow which holds the editfield for the new CDName
	//
	m_hFormatButton = NULL;

	hFirstChild = ::GetWindow(m_hWndPCD, GW_CHILD);
	::GetWindowText(hFirstChild, szWndText, sizeof(szWndText));
	if ((WORD)::GetDlgCtrlID(hFirstChild) == m_dwInitFormateButID)
	{
		m_hFormatButton = hFirstChild; 
	}

	//
	//check every child window
	//
	while (   hFirstChild
		   && (szWndText != m_sInitWndTxtPCD)
		  )
	{
		hFirstChild = ::GetWindow(hFirstChild, GW_HWNDNEXT);
		::GetWindowText(hFirstChild, szWndText, sizeof(szWndText));
		if ((WORD)::GetDlgCtrlID(hFirstChild) == m_dwInitFormateButID)
		{
			m_hFormatButton = hFirstChild; 
		}
	}

	//
	//save handle to child window that holds the editfield for the new CDName
	//
	m_hChildWndPCD = hFirstChild;

	if (m_hChildWndPCD)
	{
		//
		//get childs child window of PacketCD desktop window and search
		//for the editfield to insert the new cd-name 
		//
		hSecondChild = ::GetWindow(m_hChildWndPCD, GW_CHILD);
		wIDEditField = (WORD)::GetDlgCtrlID(hSecondChild);
			
		//
		//find correct ID of edit field
		//
		while(   hSecondChild 
			  && (wIDEditField != m_dwInitEditFieldID)
			  )
		{
			hSecondChild = ::GetWindow(hSecondChild, GW_HWNDNEXT);
			wIDEditField = (WORD)::GetDlgCtrlID(hSecondChild);
		}

		//
		// send the new cd-name to the editfield
		// we can use send here because it's an edit control 
		// and we are fast enough back
		if (hSecondChild)
		{
			::SendMessage(hSecondChild,WM_SETTEXT, 0, (LPARAM)(LPCTSTR)m_sInitNewCDName);
			WK_TRACE(_T("StartFormatCD(): New CD-Name %s sended\n"), m_sInitNewCDName);
		}
		else
		{
			WK_TRACE(_T("StartFormatCD(): found no edit field for new CD-R name\n"));
		}


		//
		//click button "&Formatiere" in packet-cd dialog
		//
		::PostMessage(m_hWndPCD,WM_COMMAND,
				(WPARAM)MAKELONG(m_dwInitFormateButID,BN_CLICKED),
				(LPARAM)m_hFormatButton);

		bRet = TRUE;

		WK_TRACE(_T("StartFormatCD(): Format button clicked\n"));
	}
	else
	{
		WK_TRACE_ERROR(_T("StartFormatCD(): no first child window\n"));
	}
	return bRet;
}
///////////////////////////////////////////////////////////////////////
void CRemoteCDDlg::OnFormatFormatting()
{
	if (IsFormatted())
	{
		WK_TRACE(_T("OnFormatFormatting(): CD-R formatted\n"));
		m_dwWaitAfterFormat = GetTickCount();
        m_FormatStatus = FORMAT_FORMATTED;
	}
	else
	{
		if (GetTimeSpan(m_dwStartFormat)>120*1000)
		{
			//msg to DVClient that formatting was NOT successfull
			MsgDVClientFormatStatus(FALSE);
			WK_TRACE(_T("OnFormatFormatting(): CD-R not formatted\n"));
			m_RemoteCDStatus = RS_QUIT;
		}
		else
		{
			// immer noch auf CD Formatieren warten
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
//Waits until the text of button 0x3024 "&Formatiere" has changed
//that indicates, that the format progress has finished
BOOL CRemoteCDDlg::IsFormatted()
{
	BOOL	bRet = FALSE;
	TCHAR	sWndText[100];

	::GetDlgItemText(m_hWndPCD, m_dwInitFormateButID, sWndText, sizeof(sWndText));

	// has buttontext "&Formatiere" changed ?
	if (sWndText == m_sInitFormateButTxt)
	{
		bRet = FALSE;
	}
	else
	{
		::PostMessage(m_hWndPCD,WM_COMMAND,
					(WPARAM)MAKELONG(m_dwInitFertigButID,BN_CLICKED),
					(LPARAM)m_hFormatButton);
		bRet = TRUE;
	}

	return bRet;
}
///////////////////////////////////////////////////////////////////////
void CRemoteCDDlg::OnFormatFormatted()
{
	if (!IsWindow(m_hWndPCD))
	{
		//msg to DVClient that formatting was successfull
		WK_TRACE(_T("OnFormatFormatted(): formatting successfull, end of PacketCD\n"));
		MsgDVClientFormatStatus(TRUE);
		m_RemoteCDStatus = RS_QUIT;
	}
	else
	{
		if (GetTimeSpan(m_dwWaitAfterFormat)>20*1000)
		{
			WK_TRACE(_T("OnFormatFormatted(): timeout wait for end of PacketCD after 20s\n"));
			MsgDVClientFormatStatus(TRUE);
			m_RemoteCDStatus = RS_QUIT;
		}
	}
}
///////////////////////////////////////////////////////////////////////
void CRemoteCDDlg::OnFinalize()
{
	m_hDVClient = ::FindWindow(WK_APP_NAME_DVCLIENT,NULL);
	if (IsWindow(m_hDVClient))
	{
		if (!m_FinalizeThread.IsRunning() && (m_FinalizeStatus == FS_BEGIN))
		{
			m_FinalizeThread.SetPath(m_sDriveLetter);
			CString sMenuItem = GetStringFromTable("c2udf.dll",2); //2 = Finalisieren
			WK_TRACE(_T("OnFinalize(): Menuitem: %s\n"), sMenuItem);
			m_FinalizeThread.SetMenuItem(sMenuItem);
			m_FinalizeThread.StartThread();
			WK_TRACE(_T("OnFinalize(): finalize thread started\n"));
			Sleep(20);
			m_FinalizeStatus = FS_WAIT_FOR_FIRST_DLG;
		}
		else
		{
			switch(m_FinalizeStatus)
			{
			case FS_WAIT_FOR_FIRST_DLG:
				//wait until first finalize dlg is on desktop
				m_hFinalizeStartDlg = ::FindWindow(NULL, _T("PacketCD"));
				::ShowWindow(m_hFinalizeStartDlg, SW_HIDE);
				if(!IsWindow(m_hFinalizeStartDlg))
				{
					Sleep(200);
					//on timeout send msg to DVClient that finalize failed					
					if(GetTimeSpan(m_dwStart) > 120*1000)
					{
						WK_TRACE(_T("OnFinalize(): finalize start dlg not found after timeout of 120s\n"));
						m_FinalizeStatus = FS_FAILED;	
					}
				}
				else
				{
					::ShowWindow(m_hFinalizeStartDlg, SW_HIDE);
					m_FinalizeStatus = FS_CLICK_BUTTON;
					WK_TRACE(_T("OnFinalize(): found first Finalize Dlg\n"));
				}
				break;
				
			case FS_CLICK_BUTTON:
				{
					//click button "ja" on first finalize dlg
					m_hFirstChild = ::GetWindow(m_hFinalizeStartDlg, GW_CHILD);
					BOOL bButtonClicked = ::PostMessage(m_hFinalizeStartDlg,WM_COMMAND,
						(WPARAM)MAKELONG(m_dwInitFinalizeJaID,BN_CLICKED),
						(LPARAM)m_hFirstChild);
					if(bButtonClicked)
					{
						WK_TRACE(_T("OnFinalize(): button on first Finalize Dlg clicked\n"));
						m_FinalizeStatus = FS_WAIT_FOR_PROGRESS_DLG;
					}	
					else
					{
						m_FinalizeStatus = FS_FAILED;
						WK_TRACE(_T("OnFinalize(): couln`d click -yes- button on first finalize dlg \n"));
					}
				}
				break;
				
			case FS_WAIT_FOR_PROGRESS_DLG:
				{
					//wait until finalize progress bar dlg is on desktop
					m_hFinalizeUDFDlg = ::FindWindow(NULL, m_sInitFinalizeDlg);
					if (!IsWindow(m_hFinalizeUDFDlg))
					{	
						//on timeout send msg to DVClient that finalize failed					
						if(GetTimeSpan(m_dwStart) > 120*1000)
						{
							WK_TRACE(_T("OnFinalize(): not found finalize progress bar dlg after timeout 120s\n"));
							m_FinalizeStatus = FS_FAILED;	
						}
					}
					else
					{
						::ShowWindow(m_hFinalizeUDFDlg, SW_HIDE);
						WK_TRACE(_T("OnFinalize(): found progress dlg on desktop\n"));
						m_FinalizeStatus = FS_WAIT_FOR_FINALIZE_COMPLETE;
					}
				}
				break;
				
			case FS_WAIT_FOR_FINALIZE_COMPLETE:
				{
					//wait until finalize progress bar dlg is not longer on desktop
					m_hFinalizeUDFDlg = ::FindWindow(NULL, m_sInitFinalizeDlg);
					if (IsWindow(m_hFinalizeUDFDlg))
					{
						::ShowWindow(m_hFinalizeUDFDlg, SW_HIDE);
						Sleep(20);
						//on timeout send msg to DVClient that finalize failed					
						if(GetTimeSpan(m_dwStart) > 5*60*1000)
						{
							WK_TRACE(_T("OnFinalize(): finalize progress dlg still on desktop after 5 min.\n"));
							m_FinalizeStatus = FS_FAILED;	
						}
					}
					else
					{
						WK_TRACE(_T("OnFinalize(): progress dlg not longer on desktop\n"));
						m_dwWaitAfterFinalize = GetTickCount();
						m_FinalizeStatus = FS_WAIT_FOR_FINALIZE_THREAD;
					}
				}
				break;

			case FS_WAIT_FOR_FINALIZE_THREAD:
				{
					if (!m_FinalizeThread.IsRunning())
					{
						WK_TRACE(_T("OnFinalize(): Finalize-Thread not running: 0x%x\n"), m_FinalizeThread);
						m_dwWaitAfterFinalize = 0;
						m_FinalizeStatus = FS_SUCCESS;
					}
					else
					{
						// time out
                        if (GetTimeSpan(m_dwWaitAfterFinalize)>20*1000)
						{
							WK_TRACE(_T("OnFinalize(): Finalize timeout: %i\n"), GetTimeSpan(m_dwWaitAfterFinalize));  
							// OOPS hat Finalize wirklich geklappt ?
							m_FinalizeStatus = FS_SUCCESS;
						}
					}
				}
				break;
			case FS_SUCCESS:
				{
					WK_TRACE(_T("OnFinalize(): Finalize success\n"));
					BOOL bSendMsg = ::PostMessage(m_hDVClient, 
						WM_CHECK_BACKUP_STATUS, 
						BS_FINALIZE_SUCCESS, 0);
					if(bSendMsg)
					{
						WK_TRACE(_T("OnFinalize(): Sended BS_FINALIZE_SUCCESS to DVClient\n"));
					}
					else
					{
						WK_TRACE(_T("OnFinalize(): Couldn`t send BS_FINALIZE_SUCCESS to DVClient\n"));
					}
					m_RemoteCDStatus = RS_QUIT;	
				}
				break;
				
			case FS_FAILED:
				{
					WK_TRACE(_T("OnFinalize(): Finalize failed\n"));
					BOOL bSendMsg = ::PostMessage(m_hDVClient, 
						WM_CHECK_BACKUP_STATUS, 
						BS_FINALIZE_FAILED, 0);
					if(bSendMsg)
					{
						WK_TRACE(_T("OnFinalize(): Sended BS_FINALIZE_FAILED to DVClient\n"));
					}
					else
					{
						WK_TRACE(_T("OnFinalize(): Couldn`t send BS_FINALIZE_FAILED to DVClient\n"));
					}
					m_RemoteCDStatus = RS_QUIT;	
				}
				break;
			default:
				WK_TRACE(_T("OnFinalize(): FinalizeStatus: %s\n"), NameOfEnum(BS_NO_STATUS,
					RS_NO_STATUS,
					m_FinalizeStatus));
				break;
			}
		}
	}
	else  
	{
		WK_TRACE(_T("OnFinalize(): DVClient not found\n"));
		m_RemoteCDStatus = RS_QUIT;	
	}
}

///////////////////////////////////////////////////////////////////////
//gets the string on the delivered position (iStringID) from the sFile
CString CRemoteCDDlg::GetStringFromTable(CString sFile, int iStringID)
{

	CString s("");
	CString sText("");

	HMODULE hFile = LoadLibraryEx(m_sPathToPacketCD + "\\" + sFile,NULL,0);

	if(hFile != NULL)
	{
		LoadString(hFile,iStringID,sText.GetBuffer(_MAX_PATH),_MAX_PATH);
		sText.ReleaseBuffer();
	}
	else
	{
		WK_TRACE_ERROR(_T("GetStringFromTable(): Error %u ; %s\n"),
							GetLastError(), GetLastErrorString());
	}
	FreeLibrary(hFile);
	return sText;
}

///////////////////////////////////////////////////////////////////////
//gets the string on the delivered position (iStringID) from the sFile
CString CRemoteCDDlg::GetStringFromDlg(CString sFile, CString sDlgID)
{
	CString sDlgTitle("");
	CString s("");

	HMODULE hFile = LoadLibraryEx(m_sPathToPacketCD + "\\" + sFile,NULL,0);
	HRSRC FindRes = FindResourceEx(hFile, RT_DIALOG, sDlgID, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL));
	if(FindRes != NULL)
	{
		HGLOBAL LoadRes = LoadResource(hFile, FindRes);
		if(LoadRes != NULL)
		{
			LPCDLGTEMPLATE pLockRes = (LPCDLGTEMPLATE)LockResource(LoadRes);
	
			CDialog dlg;
			dlg.CreateIndirect(pLockRes);
			dlg.ShowWindow(SW_HIDE);	
			dlg.GetWindowText(sDlgTitle);
			dlg.DestroyWindow();
			FreeResource(LoadRes);
		}
		else
		{

			s += "GetMenuItem(): Error during LoadResource\n";
			WK_TRACE_ERROR(_T("GetStringFromDlg(): Error %u ; %s\n"),
							GetLastError(), GetLastErrorString());
		}
	}
	else
	{
			s += "GetMenuItem(): Error during FindResourceEx\n";
			WK_TRACE_ERROR(_T("GetStringFromDlg(): Error %u ; %s\n"),
							GetLastError(), GetLastErrorString());
	}
	return sDlgTitle;
}
///////////////////////////////////////////////////////////////////////
void CRemoteCDDlg::Init()
{

	
	m_sInitFormateButTxt = GetStringFromTable("udforig.exe", 18);
	//if Hookstatus == TRUE, get DlgTitles direct from packetcd resources
	if(GetHookStatusFromReg())
	{
		m_sInitWndTxtPCD = GetStringFromDlg("udforig.exe", "#108");
		m_sInitFinalizeDlg = GetStringFromDlg("c2udf.dll", "#143");
	}
	else
	{
		if(m_sInitFormateButTxt == "&Formatiere")
		{
			//german version
			m_sInitWndTxtPCD = "PacketCD Formatierer";
			m_sInitFinalizeDlg = "Finalisiere UDF CD";
		}
		else if(m_sInitFormateButTxt == "&Format")
		{
			//english version
			m_sInitWndTxtPCD = "PacketCD Formatter";
			m_sInitFinalizeDlg = "Finalizing UDF disc";
		}
		else
		{
			WK_TRACE(_T("Init(): no english or german NT-version\n"));
			m_sInitWndTxtPCD = "PacketCD Formatierer";
		}
	}


	m_dwInitEditFieldID		= 0x0425;
	m_dwInitFormateButID	= 0x3024; //0x0009;
	m_dwInitFertigButID		= 0x3025;
	m_dwInitAbbrechenButID	= 0x0002;

	m_dwInitFinalizeJaID	= 0x0006; //0x0007 = nein;
}

