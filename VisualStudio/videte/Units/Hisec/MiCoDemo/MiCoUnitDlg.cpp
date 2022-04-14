// MiCoAdlg.cpp : implementation file

#include "stdafx.h"
#include <direct.h>
#include "mmsystem.h"
#include "CMiCo.h"
#include "MiCoUnitApp.h"
#include "MiCoUnitDlg.h"
#include "Settings.h"
					  
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
UINT PollProzessThread(LPVOID pData)
{
	CMiCoUnitDlg*	pAppDlg	 = (CMiCoUnitDlg*)pData;  // Pointer auf's App-Objekt

	// Warte bis MainProzeß initialisiert ist.
	
	while (!pAppDlg->m_bRun)
	{
		Sleep(10);
	}
	
	// Poll bis Mainprozeß terminiert werden soll.
	while (pAppDlg->m_bRun)
	{
		pAppDlg->PollProzess();	// Prozeßmanagement
	}
	WK_TRACE("ExitPollProzessManagementThread\n");
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
UINT PollEncoderThread(LPVOID pData)
{
	CMiCoUnitDlg*	pAppDlg	 = (CMiCoUnitDlg*)pData;  // Pointer auf's App-Objekt

	// Warte bis MainProzeß initialisiert ist.
	
	while (!pAppDlg->m_bRun)
	{
		Sleep(10);
	}
	
	// Poll bis Mainprozeß terminiert werden soll.
	while (pAppDlg->m_bRun)
	{
		pAppDlg->PollEncoder();	 // Daten vom Encoder holen und an den Server schicken.
	}
	WK_TRACE("ExitPollEncoderThread\n");
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
UINT SwitchThread(LPVOID pData)
{
	CMiCoUnitDlg*	pAppDlg	 = (CMiCoUnitDlg*)pData;  // Pointer auf's App-Objekt

	// Warte bis MainProzeß initialisiert ist.
	while (!pAppDlg->m_bRun)
	{
		Sleep(10);
	}
	
	// Poll bis Mainprozeß terminiert werden soll.
	while (pAppDlg->m_bRun)
	{
		pAppDlg->SwitchThread();
		Sleep(250);
	}
	WK_TRACE("ExitSwitchThread\n");
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CMiCoUnitDlg, CDialog)
	//{{AFX_MSG_MAP(CMiCoUnitDlg)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BE_NTSC_CCIR, OnBeNtscCCIR)
	ON_BN_CLICKED(IDC_BE_PAL_CCIR, OnBePalCCIR)
	ON_BN_CLICKED(IDC_BE_NTSC_SQUARE, OnBeNtscSquare)
	ON_BN_CLICKED(IDC_BE_PAL_SQUARE, OnBePalSquare)
	ON_BN_CLICKED(IDC_DLG_SETTINGS, OnDlgSettings)
	ON_BN_CLICKED(IDC_EXTCARD0, OnExtcard0)
	ON_BN_CLICKED(IDC_EXTCARD1, OnExtcard1)
	ON_BN_CLICKED(IDC_EXTCARD2, OnExtcard2)
	ON_BN_CLICKED(IDC_EXTCARD3, OnExtcard3)
	ON_BN_CLICKED(IDC_FE_FBAS, OnFeFbas)
	ON_BN_CLICKED(IDC_FE_NTSC_CCIR, OnFeNtscCCIR)
	ON_BN_CLICKED(IDC_FE_PAL_CCIR, OnFePalCCIR)
	ON_BN_CLICKED(IDC_FE_NTSC_SQUARE, OnFeNtscSquare)
	ON_BN_CLICKED(IDC_FE_PAL_SQUARE, OnFePalSquare)
	ON_BN_CLICKED(IDC_FE_SVHS, OnFeSvhs)
	ON_BN_CLICKED(IDC_SOURCE1, OnSource1)
	ON_BN_CLICKED(IDC_SOURCE2, OnSource2)
	ON_BN_CLICKED(IDC_SOURCE3, OnSource3)
	ON_BN_CLICKED(IDC_SOURCE4, OnSource4)
	ON_BN_CLICKED(IDC_SOURCE5, OnSource5)
	ON_BN_CLICKED(IDC_SOURCE6, OnSource6)
	ON_BN_CLICKED(IDC_SOURCE7, OnSource7)
	ON_BN_CLICKED(IDC_SOURCE8, OnSource8)
	ON_BN_CLICKED(IDC_BE_COMPOSITE, OnBeComposite)
	ON_BN_CLICKED(IDC_BE_RGB, OnBeRgb)
	ON_BN_CLICKED(IDC_FE_LNOTCH, OnFeLNotch)
	ON_BN_CLICKED(IDC_FE_LDEC, OnFeLDec)
	ON_BN_CLICKED(IDC_EDGE7, OnEdge7)
	ON_BN_CLICKED(IDC_EDGE6, OnEdge6)
	ON_BN_CLICKED(IDC_EDGE5, OnEdge5)
	ON_BN_CLICKED(IDC_EDGE4, OnEdge4)
	ON_BN_CLICKED(IDC_EDGE3, OnEdge3)
	ON_BN_CLICKED(IDC_EDGE2, OnEdge2)
	ON_BN_CLICKED(IDC_EDGE1, OnEdge1)
	ON_BN_CLICKED(IDC_EDGE0, OnEdge0)
	ON_BN_CLICKED(IDC_ACK0, OnAck0)
	ON_BN_CLICKED(IDC_ACK1, OnAck1)
	ON_BN_CLICKED(IDC_ACK2, OnAck2)
	ON_BN_CLICKED(IDC_ACK3, OnAck3)
	ON_BN_CLICKED(IDC_ACK4, OnAck4)
	ON_BN_CLICKED(IDC_ACK5, OnAck5)
	ON_BN_CLICKED(IDC_ACK6, OnAck6)
	ON_BN_CLICKED(IDC_ACK7, OnAck7)
	ON_BN_CLICKED(IDC_RELAIS0, OnRelais0)
	ON_BN_CLICKED(IDC_RELAIS1, OnRelais1)
	ON_BN_CLICKED(IDC_RELAIS2, OnRelais2)
	ON_BN_CLICKED(IDC_RELAIS3, OnRelais3)
	ON_BN_CLICKED(IDC_DEC_START, OnDecStart)
	ON_BN_CLICKED(IDC_DEC_STOP, OnDecStop)
	ON_BN_CLICKED(IDC_ENC_START, OnEncStart)
	ON_BN_CLICKED(IDC_ENC_STOP, OnEncStop)
	ON_BN_CLICKED(IDC_TEST, OnTest)
	ON_BN_CLICKED(IDC_COMPRESS_1, OnCompress1)
	ON_BN_CLICKED(IDC_COMPRESS_2, OnCompress2)
	ON_BN_CLICKED(IDC_COMPRESS_3, OnCompress3)
	ON_BN_CLICKED(IDC_COMPRESS_4, OnCompress4)
	ON_BN_CLICKED(IDC_COMPRESS_5, OnCompress5)
	ON_BN_CLICKED(IDC_HIGH_RES, OnHighResolution)
	ON_BN_CLICKED(IDC_LOW_RES, OnLowResolution)
	ON_WM_DESTROY()
	ON_WM_QUERYENDSESSION()
	ON_MESSAGE(WM_MICO_ALARM,					OnWmAlarm)
	ON_MESSAGE(WM_MICO_VXD_LOST_FIELD,			OnWmLostField)
	ON_MESSAGE(WM_MICO_VXD_UNVALID_FIELD,		OnWmUnvalidField)
	ON_MESSAGE(WM_MICO_VXD_UNVALID_BUFFER,		OnWmUnvalidBuffer)
	ON_MESSAGE(WM_MICO_VXD_UNVALID_FRAMELEN,	OnWmUnvalidFrameLen)
	ON_MESSAGE(WM_MICO_VXD_REKURSION,			OnWmIRQRekursion)
	ON_WM_ACTIVATEAPP( )	
	ON_WM_QUERYOPEN()
	ON_WM_SYSCOMMAND()
	ON_WM_ENDSESSION()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
	  
/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMiCoUnitDlg)
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::VideoDataService()
{
	BYTE *szBuffer = NULL;

	static char szText[16];
	DWORD  dwLen = 0;

	if (!IsValid()){
		return;
	}

	szBuffer = new BYTE[JPEG_BUFFER_SIZE];

	dwLen =	JPEG_BUFFER_SIZE;
	if (m_pMiCo && m_pMiCo->GetDecoderState() != MICO_DECODER_OFF)
	{ 
		CString s;
		CStringArray result;
		s.Format("%sJ_%06lu.jpg", m_sJpegPath, m_dwDecodedFrames);
		
		TRY
		{
			CFile wFile(s,	CFile::modeRead   |
							CFile::typeBinary |
							CFile::shareDenyWrite);
			dwLen = wFile.Read(szBuffer, JPEG_BUFFER_SIZE);

			m_pMiCo->WriteData((LPCSTR)&szBuffer[0], dwLen);
			wsprintf(szText, "%lu", m_dwDecodedFrames);
			SetDlgText(IDC_DECODED_FRAMES, szText);
			m_dwDecodedFrames++;
		}
		CATCH( CFileException, e )
		{
			OnDecStop();
		}
		END_CATCH
	}

	WK_DELETE(szBuffer);

	return;
}

/////////////////////////////////////////////////////////////////////////////
CMiCoUnitDlg::CMiCoUnitDlg()
	: CDialog(CMiCoUnitDlg::IDD)
{
	//{{AFX_DATA_INIT(CMiCoUnitDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32

	m_bOk				= FALSE;
	m_bRun				= FALSE;
	m_bOn				= FALSE;
	m_hIcon				= AfxGetApp()->LoadIcon(IDR_MICOUNIT);
	m_pMiCo				= NULL;
	m_pPollProzessThread= NULL;
	m_pPollEncoderThread= NULL;
	m_pSwitchThread		= NULL;
	m_dwEncodedFrames	= 0L;
	m_dwDecodedFrames	= 0L;
	m_wFormat			= MICO_ENCODER_HIGH;
	m_wFPS				= 10;
	m_dwBPF				= MICO_BPF_3;
	m_nEncoderState		= MICO_ENCODER_OFF;
	m_nDecoderState		= MICO_DECODER_OFF;

	m_bTraceFps			= FALSE;
	m_dwSourceSwitch	= 0;

	//	NEW HEDU
	m_hSyncEvent=NULL;

	// Pfad zum Speichern der Jpeg-dateien einlesen
	char szBuffer[255];
	GetPrivateProfileString("MICOISA", "JpegPath", "E:\\Jpeg\\", szBuffer,  255, MICOUNIT_INI); 
	m_sJpegPath = szBuffer;
	_mkdir(m_sJpegPath);
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::PostNcDestroy() 
{
	delete this;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMiCoUnitDlg::Create()
{
	if (!CDialog::Create(IDD))
	{
		return FALSE;
	}

	m_pPollProzessThread = AfxBeginThread(PollProzessThread, this);
	if (m_pPollProzessThread)
		m_pPollProzessThread->m_bAutoDelete = FALSE;

	m_pPollEncoderThread = AfxBeginThread(::PollEncoderThread, this);
	if (m_pPollEncoderThread)
		m_pPollEncoderThread->m_bAutoDelete = FALSE;

 	m_pSwitchThread = AfxBeginThread(::SwitchThread, this);
	if (m_pSwitchThread)
		m_pSwitchThread->m_bAutoDelete = FALSE;

	// MiCo-Objekt anlegen
	m_pMiCo = new CMiCo(m_hWnd, MICOUNIT_INI);

	if (!m_pMiCo)
	{
		WK_TRACE_ERROR("CMiCoUnitDlg::Create\tCMiCo-Objekt konnte nicht angelegt werden.\n");
		return FALSE;
	}		

	if (!m_pMiCo->IsValid())
	{
		WK_TRACE_ERROR("CMiCoUnitDlg::Create\tCMiCo-Objekt konnte nicht korrekt initialisiert werden.\n");
		WK_DELETE(m_pMiCo);
		return FALSE;
	}		

	DWORD	dwHardwareState = 0L;
	DWORD	dwBit			= 0L;

	m_bOk  = TRUE;
	m_bRun = TRUE;
	
	UpdateDlg();
	ShowWindow(SW_SHOW);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMiCoUnitDlg::IsValid()
{
	return m_bOk;
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::SwitchThread()
{
	static int   nSource = MICO_SOURCE0;
	char	szText[255];
	if (m_bOn == TRUE)
	{
		switch (nSource)
		{
			case MICO_SOURCE0:
				m_dwSourceSwitch++;
				OnSource1();
				nSource = MICO_SOURCE1;
				break;
			case MICO_SOURCE1:
				m_dwSourceSwitch++;
				OnSource2();
				nSource = MICO_SOURCE2;
				break;
			case MICO_SOURCE2:
				m_dwSourceSwitch++;
				OnSource3();
				nSource = MICO_SOURCE3;
				break;
			case MICO_SOURCE3:
				m_dwSourceSwitch++;
				OnSource4();
				nSource = MICO_SOURCE4;
				break;
			case MICO_SOURCE4:
				m_dwSourceSwitch++;
				OnSource5();
				nSource = MICO_SOURCE5;
				break;
			case MICO_SOURCE5:
				m_dwSourceSwitch++;
				OnSource6();
				nSource = MICO_SOURCE6;
				break;
			case MICO_SOURCE6:
				m_dwSourceSwitch++;
				OnSource7();
				nSource = MICO_SOURCE7;
				break;
			case MICO_SOURCE7:
				m_dwSourceSwitch++;
				OnSource8();
				nSource = MICO_SOURCE0;
				break;
		}
		wsprintf(szText, "%lu", m_dwSourceSwitch);
		SetDlgText(IDC_DECODED_FRAMES, szText);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::PollProzess()
{
	// Übernimmt den Transport zum Decoder
	VideoDataService();
	Sleep(20);
}
 
/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::PollEncoder()
{
	JPEG*	pJpeg		= NULL;
	LPCSTR	lpBuffer	= NULL;
	DWORD	dwLen		= 0;
	WORD	wSource		= 0;
	WORD	wExtCard	= 0;
	WORD	wCamSubID;
  	static  char szText[16];
	int		nErrorCode  = MICO_ERR_NO_ERRROR;

	// Parameter aus der JPEG-Struktur holen
	if (m_pMiCo)
		pJpeg = m_pMiCo->ReadData(nErrorCode);

	// Gültiges Bild?
	if (pJpeg == 0){
		return;
	}

	wSource		= pJpeg->wSource;		// Videoquelle (0...7)
	wExtCard	= pJpeg->wExtCard;		// Erweiterungsboard (0...3)
	lpBuffer	= pJpeg->lpBuffer;		// Pointer auf Jpegdaten
	dwLen		= pJpeg->dwLen;			// Länge der Jpegdaten
	wCamSubID   = wSource + (MICO_MAX_SOURCE + 1) * wExtCard;

	CString s;

	// Name des Jpegfiles mit Hilfe einer laufenden Nummer generieren
	s.Format("%sJ_%06lu.jpg", m_sJpegPath, m_dwEncodedFrames);

	CFile wFile(s,	CFile::modeCreate | CFile::modeWrite  |
					CFile::typeBinary | CFile::shareDenyWrite);

	wFile.WriteHuge(lpBuffer, dwLen); 

	wsprintf(szText, "%lu", m_dwEncodedFrames);
	SetDlgText(IDC_ENCODED_FRAMES, szText);
	m_dwEncodedFrames++;
	return;
}

/////////////////////////////////////////////////////////////////////////////
int CMiCoUnitDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
	{
		WK_TRACE_ERROR("CMiCoUnitDlg::OnCreate failed\n");
		return -1;
	}    

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnCancel() 
{
	DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnOK() 
{
	DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CMiCoUnitDlg::OnInitDialog
BOOL CMiCoUnitDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CenterWindow();

	// only for standalone
	UpdateDlg();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMiCoUnitDlg::OnQueryOpen()
{
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMiCoUnitDlg::OnQueryEndSession() 
{
	if (!CDialog::OnQueryEndSession())
		return FALSE;
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnEndSession(BOOL bEnding) 
{
	CDialog::OnEndSession(bEnding);

	if (bEnding)
	{
		WK_TRACE("System shutdown...\n");

		DestroyWindow();
	}	
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == ID_SETTINGS)
	{
		OnDlgSettings();
	}
	else
	{
		CWnd::OnSysCommand(nID, lParam);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnPaint() 
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
long CMiCoUnitDlg::OnWmAlarm(WPARAM wParam, LPARAM lParam)
{
	WORD wExtCard	= (WORD)wParam;
	BYTE byAlarm	= (BYTE)lParam;

	WK_TRACE("CMiCoUnitDlg::OnAlarm\tExtCard%u\tAlarm%u\n", wExtCard, byAlarm);

	return 0L;
}

/////////////////////////////////////////////////////////////////////////////
long CMiCoUnitDlg::OnWmLostField(WPARAM wParam, LPARAM lParam)
{
	WK_TRACE_WARNING("WmLostField\n");
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
long CMiCoUnitDlg::OnWmUnvalidField(WPARAM wParam, LPARAM lParam)
{
	WK_TRACE_WARNING("WmUnvalidField\n");
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
long CMiCoUnitDlg::OnWmUnvalidBuffer(WPARAM wParam, LPARAM lParam)
{
	WK_TRACE_WARNING("WmUnvalidBuffer\n");
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
long CMiCoUnitDlg::OnWmUnvalidFrameLen(WPARAM wParam, LPARAM lParam)
{
	WK_TRACE_WARNING("WmUnvalidFrameLen\n");
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
long CMiCoUnitDlg::OnWmIRQRekursion(WPARAM wParam, LPARAM lParam)
{
	WK_TRACE_WARNING("Rekursiver Aufruf der ISR!\n");
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoUnitDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// PollThread beenden.
	m_bRun = FALSE;
 	m_bOk  = FALSE;

	// Warte bis 'PollThread' beendet ist.
	if (m_pPollProzessThread){
		WaitForSingleObject(m_pPollProzessThread->m_hThread, 5000);
	}

	WK_DELETE(m_pPollProzessThread); //Autodelete = FALSE;

	// Warte bis 'PollEncoderThread' beendet ist.
	if (m_pPollEncoderThread){
		WaitForSingleObject(m_pPollEncoderThread->m_hThread, 5000);
	}

	WK_DELETE(m_pPollEncoderThread); //Autodelete = FALSE;

	// Warte bis 'SwitchThread' beendet ist.
	if (m_pSwitchThread){
		WaitForSingleObject(m_pSwitchThread->m_hThread, 5000);
	}

	WK_DELETE(m_pSwitchThread); //Autodelete = FALSE;

	WK_DELETE(m_pMiCo);
}

