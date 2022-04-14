// EmailUnitDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EmailUnit.h"
#include "EmailUnitDlg.h"
#include "Email.h"
#include "IPCOutputEmail.h"
#include "IPCServerControlEmail.h"
#include "DialUpNetwork.h"
#include "Profiler.h"
#include "skins\skins.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CEmailUnitApp theApp;
#define MPEG4_PICTURE_DECODED 0x0815

UINT CEmailUnitDlg::m_uRasDialEventMsg = 0;
/////////////////////////////////////////////////////////////////////////////
// CEmailUnitDlg dialog
CEmailUnitDlg::CEmailUnitDlg(CWnd* pParent /*=NULL*/)
: CDialog(CEmailUnitDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEmailUnitDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon=AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
	m_pCIPCServerControlEmail = new CIPCServerControlEmail(this);
	m_pCIPCOutputEmail = NULL;
	m_pEMail = NULL;
	m_pH263Decoder = NULL;
	m_pMp4Decoder	= NULL;
	m_pEmailClient = new CEmailClient(this);
	
	
	m_uRasDialEventMsg = RegisterWindowMessageA( RASDIALEVENT );
	
	if (m_uRasDialEventMsg == 0)
		m_uRasDialEventMsg = WM_RASDIALEVENT; 
	
}

CEmailUnitDlg::~CEmailUnitDlg()
{
	WK_DELETE(m_pCIPCServerControlEmail);
	WK_DELETE(m_pCIPCOutputEmail);
	WK_DELETE(m_pEmailClient);
	WK_DELETE(m_pH263Decoder);	
	WK_DELETE(m_pMp4Decoder);	
	if (m_pEMail)
	{
		m_pEMail->Release();
	}
}

//////////////////////////////////////////////////////////////////////////////////////

void CEmailUnitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEmailUnitDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

//////////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CEmailUnitDlg, CDialog)
	//{{AFX_MSG_MAP(CEmailUnitDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_WM_TIMER()
	ON_WM_SYSCOMMAND()
	ON_COMMAND(ID_ABOUT, OnAbout)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_EMAIL,OnNewEmail)
	ON_MESSAGE(WM_PICTURE,OnPicture)
	ON_MESSAGE(WM_COCO_DECODED,OnCocoDecoded)
	ON_MESSAGE(WM_MAILSEND,OnMailSend)
	ON_WM_QUERYOPEN()
	ON_REGISTERED_MESSAGE(m_uRasDialEventMsg,OnDialUp)
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_MESSAGE(WM_TRAYCLICKED, OnTrayClicked)	
	ON_MESSAGE(WM_LANGUAGE_CHANGED, OnLanguageChanged)	
	//}}AFX_MSG_MAP
	//	ON_MESSAGE(m_uRasDialEventMsg,OnDialUp)
	//	ON_MESSAGE(WM_RASDIALEVENT,OnDialUp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CEmailUnitDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// Nur erforderlich, wenn Icon in der Systemtaskleiste
	NOTIFYICONDATA tnd;

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;
	tnd.uFlags		= 0;

	Shell_NotifyIcon(NIM_DELETE, &tnd);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CEmailUnitDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	//EnableWindow(FALSE);

	
	// Icon ins Systemtray
	NOTIFYICONDATA tnd;

	CString sTip = COemGuiApi::GetApplicationName(WAI_EMAIL_UNIT);

	SetWindowText(sTip);

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;

	tnd.uFlags		= NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.uCallbackMessage = WM_TRAYCLICKED;
	tnd.hIcon		= theApp.LoadIcon(IDR_MAINFRAME);

	lstrcpyn(tnd.szTip, sTip, sTip.GetLength()+1);
	Shell_NotifyIcon(NIM_ADD, &tnd);

	ShowWindow(SW_HIDE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

//////////////////////////////////////////////////////////////////////////////////////

void CEmailUnitDlg::OnPaint() 
{
	ShowWindow(SW_HIDE);
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
		ShowWindow(SW_HIDE);
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CEmailUnitDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

//////////////////////////////////////////////////////////////////////////////////////

long CEmailUnitDlg::OnNewEmail(WPARAM wParam, LPARAM lParam)
{
	WK_TRACE(_T("[CEmailUnitDlg::OnNewEmail]\n"));	
	CProfiler profile;
	CConnectionRecord* pConRec = m_ConnectionRecords.SafeGetAndRemoveHead();

//	const CString &shmInputName  = pConRec->GetInputShm();
	const CString &shmOutputName = pConRec->GetOutputShm();
	
	CString sMessage;
	CString sCodePage;
	pConRec->GetFieldValue(NM_MESSAGE, sMessage);
	pConRec->GetFieldValue(NM_CODEPAGE, sCodePage);
	CStringArray sa;
	CString sCharset;
	UINT nCodePage = _ttoi(sCodePage);
	if (GetCodePageInfo(nCodePage, sa))
	{
		sCharset = sa[OEM_CP_WEBCHARSET];
	}
	else
	{
		sCharset.Format(_T("windows-%d"), nCodePage);
	}

	if (sMessage.IsEmpty())
	{
		CString sUrl;
		pConRec->GetFieldValue(NM_PARAMETER, sUrl);
		if (sUrl.IsEmpty() == FALSE)
		{
			if (DoesFileExist(sUrl))
			{
				CFile file;
				if (file.Open(sUrl, CFile::modeRead, NULL))
				{
					DWORD dwLen = (DWORD)file.GetLength();
					file.SeekToBegin();
					file.Read(sMessage.GetBufferSetLength(dwLen), dwLen);
					file.Close();
					sMessage.ReleaseBuffer();
				}
			}
		}
	}
	
	if (sMessage.IsEmpty())
	{
		WK_TRACE_WARNING(_T("[CEmailUnitDlg::OnNewEmail] NO message body\n"));	
	}

	CString sHost = pConRec->GetDestinationHost();
	m_sSourceHost = pConRec->GetSourceHost();

	sHost = sHost.Mid(sHost.Find(_T("mailto:"))+7);
	m_pEMail = m_pEmailClient->CreateMail();		//Email Objekt anlegen
	m_pEMail->AddRef();

	WK_TRACE(_T("[CEmailUnitDlg::OnNewEmail] create mail to %s\n"),sHost);	

	//Empfänger aus sHost holen und in Empfängerliste eintragen
	m_pEMail->SetReceiver(sHost);
	//Email Absender aus Registry holen
	m_pEMail->SetSender(_T("<")+profile.GetEmail()+_T(">"));
	//Emailtext eintragen	
	// geändert auf quoted-printable. Base64 verlangt eine Mindestgrösse des zu kodierenden Buffers,
	// ansonsten´wird der Rest mit "Müll" aufgefüllt
	if (!sCharset.IsEmpty())
	{
		m_pEMail->SetCharSet(sCharset);
	}

	m_pEMail->SetMessage(sMessage);

	if (pConRec->GetCamID() != SECID_NO_ID)
	{
		// ein Bild von Kamera holen
		// ein CIPCOutputClient Object anlegen
#ifdef _UNICODE
		CString sCodePageOutput;
		pConRec->GetFieldValue(CRF_CODEPAGE, sCodePageOutput);
		WORD wCodePageOutput = (WORD)_ttoi(sCodePageOutput);
		m_pCIPCOutputEmail = new CIPCOutputEmail(shmOutputName,this,pConRec->GetCamID(), wCodePageOutput);
#else
		m_pCIPCOutputEmail = new CIPCOutputEmail(shmOutputName,this,pConRec->GetCamID());
#endif
		
		if (WaitForSingleObject(m_ePictureHasArrived,GETPICTURE_TIMEOUT*1000)==WAIT_OBJECT_0)
		{
			m_pCIPCServerControlEmail->DoConfirmAlarmConnection(pConRec->GetInputShm(),
																pConRec->GetOutputShm(),
																_T(""),_T(""),0
#ifdef _UNICODE
																, MAKELONG(CODEPAGE_UNICODE, 0)
#endif
																);
			WK_TRACE(_T("[CEmailUniDlg::OnNewEmail] DoConfirmAlarmConnection()\n"));
		}
		else
		{
			WK_TRACE(_T("[CEmailUnitDlg::OnNewEmail]No Picture has arrived,send pure Text Email\n"));

			CString sSubject;
			sSubject.Format(_T("%s [%s]"), m_sSourceHost, GetLocalTime());
			m_pEMail->SetSubject(sSubject);
			m_pCIPCServerControlEmail->DoConfirmAlarmConnection(pConRec->GetInputShm(),
																pConRec->GetOutputShm(),
																_T(""),_T(""),0
#ifdef _UNICODE
																, MAKELONG(CODEPAGE_UNICODE, 0)
#endif
																);
			m_pEmailClient->SendMail();
		}
		
	}
	else
	{
		WK_TRACE(_T("[CEmailUnitDlg::OnNewEmail]Camera not identified,send pure Text Email\n"));

		CString sSubject;
		sSubject.Format(_T("%s [%s]"),m_sSourceHost,GetLocalTime());
		m_pEMail->SetSubject(sSubject);
		m_pCIPCServerControlEmail->DoConfirmAlarmConnection(pConRec->GetInputShm(),
															pConRec->GetOutputShm(),
															_T(""),_T(""),0
#ifdef _UNICODE
															, MAKELONG(CODEPAGE_UNICODE, 0)
#endif
															);
		m_pEmailClient->SendMail();
	}
	WK_DELETE (pConRec);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////

long CEmailUnitDlg::OnCocoDecoded(WPARAM wParam, LPARAM lPAram)
{
	if (m_pH263Decoder)
	{
		CJpeg jpegPict;
		DWORD dwJpegLenNew=100*1024;
		BYTE* pJpegDataNew=NULL;

		CDib* pDib = m_pH263Decoder->GetDib();

		if (pDib)
		{
			HANDLE hDIB = pDib->GetHDIB();		//in JPEG kodieren

			if (hDIB)
			{
				pJpegDataNew = new BYTE[dwJpegLenNew];
				
				if (jpegPict.EncodeJpegToMemory(hDIB, pJpegDataNew, dwJpegLenNew))
				{
					m_pEMail->AddContentToMail(pJpegDataNew,
											   dwJpegLenNew,
											   _T("image"),
											   _T("jpeg"),
											   _T("base64"),
											   m_sCameraName+_T(".jpg"));
					WK_DELETE(m_pCIPCOutputEmail);
					m_pEmailClient->SendMail();
					m_pEMail->Release();
					m_pEMail = NULL;
				}
				else
				{
					WK_TRACE(_T("cannot encode as jpeg\n"));
				}
				WK_DELETE(pJpegDataNew);

				GlobalFree(hDIB);
			}
			else
			{
				WK_TRACE_ERROR(_T("no coco HDIB\n"));
			}
		}
		else
		{
			WK_TRACE_ERROR(_T("no coco CDib\n"));
		}
		WK_DELETE(pDib);
	}
	else
	{
		WK_TRACE_ERROR(_T("OnCocoDecoded without Decoder\n"));
	}
	WK_DELETE(m_pH263Decoder);

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////

long CEmailUnitDlg::OnPicture(WPARAM wParam, LPARAM lPAram)
{
	WK_TRACE(_T("[CEmailUnitDlg::OnPicture]\n"));
	if (wParam == MPEG4_PICTURE_DECODED && m_pMp4Decoder && m_pEmailClient && m_pEMail)
	{
		CJpeg			jpeg;
		DWORD			dwJpegLenNew=100000;
		BYTE*			pJpegDataNew=NULL;
		CDib* pDib = m_pMp4Decoder->GetDib();
		if (pDib)
		{
			HGLOBAL hDIB = pDib->GetHDIB();
			if (hDIB)
			{
				pJpegDataNew = new BYTE[dwJpegLenNew];
				if(jpeg.EncodeJpegToMemory(hDIB, pJpegDataNew, dwJpegLenNew))
				{
					m_pEMail->AddContentToMail(pJpegDataNew,
												dwJpegLenNew,
												_T("image"),
												_T("jpeg"),
												_T("base64"),
												m_sCameraName+_T(".jpg"));
					m_pEmailClient->SendMail();
					m_pEMail->Release();
					m_pEMail = NULL;
				}
				else
				{
					WK_TRACE_ERROR(_T("Error EncodeJpegToMemory\n"));
				}
				WK_DELETE(pJpegDataNew);
				GlobalFree(hDIB);  
			}
			else
			{
				WK_TRACE_ERROR(_T("no MPEG4 hDib\n"));
			}
			delete pDib;
		}
		else
		{
			WK_TRACE_ERROR(_T("no MPEG4 CDib\n"));
		}
		WK_DELETE(m_pCIPCOutputEmail);
		WK_DELETE(m_pMp4Decoder);
	}

	if (m_pCIPCOutputEmail)
	{
        CIPCPictureRecord* pPicture = m_pCIPCOutputEmail->GetPicture();
		
		m_sCameraName = m_pCIPCOutputEmail->GetCamName();
		m_sJobTime =  m_pCIPCOutputEmail->GetJobTime();

		m_sCameraName.Format(_T("%s: %s [%s]"),m_sSourceHost,m_sCameraName,m_sJobTime);
		m_pEMail->SetSubject(m_sCameraName);					//Mail Betreff
		if (pPicture)
		{
			if (pPicture->GetCompressionType()==COMPRESSION_JPEG)
			{
				//////////// JPEG ////////////////
				WK_TRACE(_T("[CEmailUnitDlg::OnPicture] CameraIsJpeg\n"));
				CJpeg jpegPict;
				DWORD jpegLen,dwJpegLenNew=100000;
				BYTE* jpegData=NULL, *pJpegDataNew=NULL;
				
				jpegLen = pPicture->GetDataLength();
				jpegData= (BYTE*)pPicture->GetData();
				
				WK_TRACE(_T("picture len %d\n"),jpegLen);
				
				//JPEG dekodieren und encodieren zur Grössenkorrektur
				if(jpegPict.DecodeJpegFromMemory(jpegData,jpegLen))
				{
					WK_TRACE(_T("Jpeg dekodiert\n"));
					HANDLE hDIB = jpegPict.CreateDIB();
					if (hDIB)
					{
						pJpegDataNew = new BYTE[dwJpegLenNew];
						
						if(jpegPict.EncodeJpegToMemory(hDIB, pJpegDataNew, dwJpegLenNew))
						{
							m_pEMail->AddContentToMail(pJpegDataNew,
													   dwJpegLenNew,
													   _T("image"),
													   _T("jpeg"),
													   _T("base64"),
													   m_sCameraName+_T(".jpg"));
							WK_DELETE(m_pCIPCOutputEmail);
							m_pEmailClient->SendMail();
							m_pEMail->Release();
							m_pEMail = NULL;
						}
						else
						{
						}
						WK_DELETE(pJpegDataNew);
						GlobalFree(hDIB);  
					}
					else
					{
						WK_TRACE(_T("Error creating handle (OnPicture())\n"));
					}
				}
				else
				{
					WK_TRACE(_T("Error decoding jpeg (OnPicture())\n"));
					
				}
			}
			///////////////////////////////////////////////////////////////////////COCO////////////
			else if (pPicture->GetCompressionType()==COMPRESSION_H263)
			{
				//////////// H.263 ////////////////
				WK_TRACE(_T("[CEmailUnitDlg::OnPicture] CameraIsCoco\n"));

				m_pH263Decoder = new CH263();
				m_pH263Decoder->Init(this,WM_COCO_DECODED,0,0);
				m_pH263Decoder->Decode(*pPicture);

				// warten, bis das CoCo Bild dekodiert wurde
			}
			///////////////////////////////////////////////////////////////////////MPEG4////////////
			else if (pPicture->GetCompressionType()==COMPRESSION_MPEG4)
			{
				m_pMp4Decoder = new CMPEG4Decoder();
				m_pMp4Decoder->Init(this, WM_PICTURE, MPEG4_PICTURE_DECODED, 0);
				m_pMp4Decoder->Decode(*pPicture, 0, 0);
			}
			else
			{
				WK_TRACE(_T("unknown picture format\n"));
			}
		}
		else
		{
			WK_TRACE(_T("kein Bild ??? \n"));
		}
	}
	
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////
void CEmailUnitDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==NETWORK_TIMER)
	{
		m_pEmailClient->ParkEmail();
		KillTimer(NETWORK_TIMER);
		m_pEmailClient->DisconnectNetwork();
	}
	else if(nIDEvent==EMAIL_PARKING_TIMER)
	{
		m_pEmailClient->SetTimerIsSet(FALSE);
		WK_TRACE(_T("TimerIsSet FALSE\n"));
		KillTimer(EMAIL_PARKING_TIMER);
		m_pEmailClient->SendMail();		 //Erneuter Versuch zu senden
	}
	else if(nIDEvent==HANG_UP_TIMER)
	{
		KillTimer(HANG_UP_TIMER);
		m_pEmailClient->DisconnectNetwork();
		WK_TRACE(_T("[CEmailUnitDlg::OnTimer] HANG_UP_TIMER\n"));

	}
}
//////////////////////////////////////////////////////////////////////////////////////
long CEmailUnitDlg::OnDialUp(WPARAM wParam, LPARAM lParam)
{
	WK_TRACE(_T("OnDialUp Message\n"));
	UINT iConnectStatus;
	m_pDialUpNetwork=m_pEmailClient->GetPtrToDialUpNetwork();

	iConnectStatus=m_pDialUpNetwork->GetConnectStatus();
/*
	0: Dialing
	1: Connected
	2: Error when dialing
	3: Disconnected
	4: Error function call
 */
	if(iConnectStatus==0)//DFÜ Netzwerk aufgebaut wenn TRUE
	{
		ChangeIcon(YELLOW);
		
	}
//--------------------------------------------------------------------
	else if(iConnectStatus==1)
	{

		CProfiler profiler;
		BOOL bAuthenticateByPOP;
	
		bAuthenticateByPOP=profiler.GetPOPAuth();
		m_pEmailClient->SetConnectedToNetwork(TRUE);
		if(bAuthenticateByPOP)
		{
			if(!(m_pEmailClient->ConnectToPOPServer()))
			{
				WK_TRACE(_T("[CEmailClient::SendMail()] NoPOPConnection ParkEmail()\n"));
				m_pEmailClient->ParkEmail();
				TRACE(_T("4 ParkEmail\n"));
				m_pEmailClient->DisconnectNetwork();
			}
		}
		else if(!bAuthenticateByPOP)
		{
			if(!(m_pEmailClient->ConnectToSMTPServer()))//Verbindung zum MailServer herstellen
			{
				//Verbindungsaufbau erfolglos, Mail speichern, später erneut versuchen
				//(Fehler in Netzwerkschicht)
				WK_TRACE(_T("[CEmailUnitDlg::OnDialUp]\n"));
				WK_TRACE(_T("Socketverbindung zum SMTP Server nicht möglich\n"));
				TRACE(_T("5 ParkEmail\n"));
				m_pEmailClient->ParkEmail();

				m_pEmailClient->DisconnectNetwork();
			}
		}
		else
		{
			KillTimer(NETWORK_TIMER);
			ChangeIcon(GREEN);
		   	return 0;
		}
	}
//--------------------------------------------------------------------		
		
	else if(iConnectStatus==2)
	{
		WK_TRACE(_T("[CEmailUnitDlg::OnDialUp]\n"));
		WK_TRACE(_T("DFÜ verbindung nicht möglich\n"));
		m_pEmailClient->ParkEmail();
		m_pEmailClient->DisconnectNetwork();
		return 0;
	}

//--------------------------------------------------------------------	

	else if(iConnectStatus==3)
	{
		return 0;
	}

//--------------------------------------------------------------------	
	else if(iConnectStatus==4)
	{
		WK_TRACE(_T("[CEmailUnitDlg::OnDialUp]\n"));
		WK_TRACE(_T("Funktionsaufruf GetConnectStatus Fehler\n"));
		m_pEmailClient->ParkEmail();
		m_pEmailClient->DisconnectNetwork();
		return 0;
	}


	return 0;
}
///////////////////////////////////////////////////////////////////////////
//
//  wParam = 0 : LAN
//  wParam = 1 : DFÜ
//  lParam =FALSE Fehler in der Übertragung

long CEmailUnitDlg::OnMailSend(WPARAM wParam, LPARAM lParam)
{

	if((UINT)wParam==1) //Verbindung war DFÜ-Netzwerk
	{
		
	}
	
	if(!((BOOL)lParam))				//Fehler in der Übertragung
	{	   
		m_pEmailClient->ParkEmail(m_pEMail);
		WK_TRACE(_T("[CEmailUnitDlg::OnMailSend]\n"));
		WK_TRACE(_T("Fehler während der Mailübertragung\n"));
	}
	
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////

CDialUpNetwork* CEmailUnitDlg::GetDialUpNetwork()
{
	return m_pDialUpNetwork;
}

void CEmailUnitDlg::ChangeIcon(int icolor)
{

   // Icon ins Systemtray
   NOTIFYICONDATA tnd;
 
   CString sTip = COemGuiApi::GetApplicationName(WAI_SOCKET_UNIT);
 
   tnd.cbSize  = sizeof(NOTIFYICONDATA);
   tnd.hWnd  = m_hWnd;
   tnd.uID   = 1;
 
   tnd.uFlags  = NIF_MESSAGE|NIF_ICON|NIF_TIP;
   tnd.uCallbackMessage = WM_TRAYCLICKED;
 
 
 
	if(icolor==YELLOW)
	{
		tnd.hIcon=AfxGetApp()->LoadIcon(IDI_ICON_YELLOW);
		sTip.LoadString(IDS_STRING_CONNECTED_TO_NETWORK);

	}
	else if(icolor==GREY)
	{
		tnd.hIcon=AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	}
	else if(icolor==RED)
	{
		tnd.hIcon=AfxGetApp()->LoadIcon(IDI_ICON_RED);

	}
	else if(icolor==BLUE)
	{
		tnd.hIcon=AfxGetApp()->LoadIcon(IDI_ICON_BLUE);
		sTip.LoadString(IDS_STRING_PARK_EMAIL);
	}
	else if(icolor==GREEN)
	{
		tnd.hIcon=AfxGetApp()->LoadIcon(IDI_ICON_GREEN);
		sTip.LoadString(IDS_STRING_CONNECTED_TO_SERVER);

	}

	lstrcpyn(tnd.szTip, sTip, sTip.GetLength()+1);

	Shell_NotifyIcon(NIM_MODIFY, &tnd);

}

//////////////////////////////////////////////////////////////////////////////////////

BOOL CEmailUnitDlg::OnQueryOpen()
{
	return FALSE;	//Fenster kann nicht maximiert werden
}


//////////////////////////////////////////////////////////////////////////////////////

CString CEmailUnitDlg::GetLocalTime()
{
	CString sJobTime;
	SYSTEMTIME sTime;
	GetSystemTime(&sTime);


	sJobTime.Format(_T("%02d.%02d.%04d - %02d:%02d:%02d"),sTime.wDay,
													  sTime.wMonth,
													  sTime.wYear,
													  sTime.wHour,
													  sTime.wMinute,
													  sTime.wSecond);
	   
	return 	sJobTime;

}

////////////////////////////////////////////////////////////////////////////
//
long CEmailUnitDlg::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
		::PostMessage(hWnd, WM_SELFCHECK, WAI_EMAIL_UNIT, 0);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CEmailUnitDlg::OnTrayClicked(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
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

/////////////////////////////////////////////////////////////////////////////
void CEmailUnitDlg::OnSysCommand(UINT nID, LPARAM lParam) 
{
	CDialog::OnSysCommand(nID, lParam);

	if (nID == SC_MINIMIZE)
		ShowWindow(SW_HIDE);
}

/////////////////////////////////////////////////////////////////////////////
void CEmailUnitDlg::OnAbout() 
{
	COemGuiApi::AboutDialog(this);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CEmailUnitDlg::OnLanguageChanged(WPARAM wParam, LPARAM lParam)
{
#if _MFC_VER >= 0x0710
	return theApp.SetLanguageParameters((UINT)wParam, (DWORD)lParam);
#else
	return 0;
#endif

}
