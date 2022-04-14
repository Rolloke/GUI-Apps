// DecodeAppDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DecodeApp.h"
#include "DecodeAppDlg.h"
#include "DecodeWindow.h"
#include "..\PTDecoder\PTDecoderClass.h"
#include "About.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "CDump.h"
extern CDump* pDebug;

/////////////////////////////////////////////////////////////////////////////
CDecodeAppDlg::CDecodeAppDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDecodeAppDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDecodeAppDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
	m_pDecoder		= NULL;
	m_pDecodeView	= NULL;
	m_pFile			= NULL;
	m_pBuffer		= NULL;
	m_dwLen			= 0;
	m_bShowDifferenz= FALSE;
	m_bOK			= FALSE;

	if (!CDialog::Create(IDD))
		return;

	// Decoder-Objekt anlegen
	m_pDecoder = new CPTDecoder(TRUE);
	if (!m_pDecoder)
		return;

	// View-Window anlegen
	m_pDecodeView = new CDecodeWindow(m_pDecoder);
	if (!m_pDecodeView)
		return;

	m_pFile = new CFile;
	if (!m_pFile)
		return;

	GetDlgItem(ID_PLAY)->EnableWindow(FALSE);

	m_bOK = TRUE;

	return;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDecodeAppDlg::IsValid()
{

	return m_bOK;
}

/////////////////////////////////////////////////////////////////////////////
CDecodeAppDlg::~CDecodeAppDlg()
{
	delete m_pDecodeView;
	m_pDecodeView	= NULL;

	delete m_pDecoder;
	m_pDecoder		= NULL;

	delete m_pFile;
	m_pFile			= NULL;

	delete m_pBuffer;
	m_pBuffer		= NULL;

	DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
void CDecodeAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDecodeAppDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDecodeAppDlg, CDialog)
	//{{AFX_MSG_MAP(CDecodeAppDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_LOAD, OnLoad)
	ON_BN_CLICKED(ID_PLAY, OnPlay)
	ON_BN_CLICKED(IDC_DIFFERNE, OnDifferne)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
BOOL CDecodeAppDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDecodeAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
/////////////////////////////////////////////////////////////////////////////
void CDecodeAppDlg::OnPaint() 
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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
/////////////////////////////////////////////////////////////////////////////
HCURSOR CDecodeAppDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/////////////////////////////////////////////////////////////////////////////
void CDecodeAppDlg::OnCancel() 
{
	delete this;
}

/////////////////////////////////////////////////////////////////////////////
void CDecodeAppDlg::OnOK() 
{
	delete this;
}

/////////////////////////////////////////////////////////////////////////////
void CDecodeAppDlg::OnLoad() 
{
	CFileException fileException;
	CString sType="ptd";

	CFileDialog FileDlg(TRUE, sType, "*." + sType, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"H"+ sType +" files (*."+ sType +") | All files (*.*) | *.* ||");

	if (FileDlg.DoModal() == IDCANCEL)
		return;

	GetDlgItem(ID_PLAY)->EnableWindow(FALSE);

	CString sFileName = FileDlg.GetPathName();

	if (!m_pFile)
		return;

	// Ist schon eine Datei offen?
	if (m_pBuffer)
	{
		delete m_pBuffer;
		m_pBuffer = NULL;
	}

	TRY
	{
		// Datei öffnen
		if (!m_pFile->Open(sFileName, CFile::modeRead | CFile::typeBinary, &fileException ))
		{
			TRACE( "Can't open file %s, error = %u\n","Test." + sType, fileException.m_cause );
			return;
		}

		// Speicher zum Laden der Datei anfordern
		m_dwLen = m_pFile->GetLength();
		m_pBuffer = new BYTE[m_dwLen];
		if (!m_pBuffer)
		{
			m_pFile->Close();
			return;
		}

		// Datei komplett laden
		m_dwLen = m_pFile->Read(m_pBuffer, m_dwLen);

	}
	CATCH( CFileException, e )
	{
		TRACE("Fehler beim Lesen der Datei\n");
	}
	END_CATCH

	m_pFile->Close();

	GetDlgItem(ID_PLAY)->EnableWindow(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
void CDecodeAppDlg::OnPlay() 
{

	if ((!m_pBuffer)|| (m_dwLen == 0))
		return;

	static DWORD dwStartIndex;
	static DWORD dwEndIndex;
	BOOL	bPictureType = FALSE;

	dwStartIndex	= 0;
	dwEndIndex		= 0;
	
	if (m_pDecoder)
	{
		do{
			// Bildbeginn suchen
			dwStartIndex = FindPSC(dwStartIndex, bPictureType);
			if (dwStartIndex == m_dwLen)
				break;

			// Bildende suchen
			dwEndIndex = FindPEC(dwStartIndex);
			if (dwEndIndex == 0)
				break;

			DWORD dwTC = GetTickCount();

			if (!m_pDecoder->Decode(&m_pBuffer[dwStartIndex], dwEndIndex - dwStartIndex, bPictureType, m_bShowDifferenz))
				TRACE("ErrorCode=%u\n", m_pDecoder->GetLastErrorCode());

			pDebug->Trace("Time=%lu\n", GetTickCount() - dwTC);

			// Bild neu zeichnen
			m_pDecodeView->Invalidate(FALSE);
			m_pDecodeView->UpdateWindow();

			// Bildstart weitersetzen
			dwStartIndex = dwEndIndex;

		}while (TRUE);
	}
}

/////////////////////////////////////////////////////////////////////////////
DWORD CDecodeAppDlg::FindPSC(DWORD dwIndex, BOOL &bPictureType)
{
	bPictureType = FALSE;

	for (DWORD dwI = dwIndex; dwI < m_dwLen; dwI++)
	{
		if ((m_pBuffer[dwI] == 0x20) && (m_pBuffer[dwI+1] == 0xfe))
		{
			bPictureType = TRUE;
			return dwI+1;
		}
		if ((m_pBuffer[dwI] == 0x60) && (m_pBuffer[dwI+1] == 0xfe))
		{
			bPictureType = FALSE;
			return dwI+1;
		}

	}
	return m_dwLen;
}

/////////////////////////////////////////////////////////////////////////////
DWORD CDecodeAppDlg::FindPEC(DWORD dwIndex)
{
	for (DWORD dwI = dwIndex; dwI < m_dwLen; dwI++)
	{
		if ((m_pBuffer[dwI] == 0xff) && (m_pBuffer[dwI+1] == 0xff))
			return dwI+2;
	}
	return 0;
}

void CDecodeAppDlg::OnDifferne() 
{
	m_bShowDifferenz = !m_bShowDifferenz;
}
