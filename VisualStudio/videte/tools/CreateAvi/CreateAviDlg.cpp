// CreateAviDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CreateAvi.h"
#include "CreateAviDlg.h"
#include "WriteAvi.h"
#include "AviPlayerDlg.h"
#include "DirDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
/////////////////////////////////////////////////////////////////////////////
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}
/////////////////////////////////////////////////////////////////////////////
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// **************************************************************************
/////////////////////////////////////////////////////////////////////////////
// CCreateAviDlg dialog
/////////////////////////////////////////////////////////////////////////////
CCreateAviDlg::CCreateAviDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateAviDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCreateAviDlg)
	m_sFilename = _T("");
	m_iIndex = -1;
	m_iFps = 15;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hUp = LoadBitmap(AfxGetResourceHandle(),MAKEINTRESOURCE(IDB_UP));
	m_hDown = LoadBitmap(AfxGetResourceHandle(),MAKEINTRESOURCE(IDB_DOWN));
}
/////////////////////////////////////////////////////////////////////////////
CCreateAviDlg::~CCreateAviDlg()
{
	DeleteObject(m_hUp);
	DeleteObject(m_hDown);
}
/////////////////////////////////////////////////////////////////////////////
void CCreateAviDlg::EnableControls()
{
	UpdateData();
	if (m_iIndex != LB_ERR)
	{
		m_listBitmaps.GetText(m_iIndex, m_sCurrentBitmap);
		m_listBitmaps.SetTopIndex(m_iIndex);
	}
	BOOL bEnable = FALSE;
	bEnable = !m_sCurrentBitmap.IsEmpty();
	m_btnDelete.EnableWindow(bEnable);
	bEnable = (m_listBitmaps.GetCount() > 2);
	m_btnSort.EnableWindow(bEnable);
	bEnable = (m_listBitmaps.GetCount() > 1);
	m_btnClear.EnableWindow(bEnable);
	bEnable = (   (m_listBitmaps.GetCount() > 1)
			   && !m_sFilename.IsEmpty());
	m_btnSave.EnableWindow(bEnable);
	bEnable = !m_sAviFilePath.IsEmpty();
	m_btnPlay.EnableWindow(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CCreateAviDlg::MoveBitmap(BOOL bUp)
{
	// move object listbox
	if (   (m_iIndex != LB_ERR)
		&& !m_sCurrentBitmap.IsEmpty()
		)
	{
		CString sBitmap;
		int iNewIndex = m_iIndex;
		if (bUp) {
			iNewIndex = m_iIndex - 1;
		}
		else {
			iNewIndex = m_iIndex + 1;
		}
		m_listBitmaps.DeleteString(m_iIndex);
		m_listBitmaps.InsertString(iNewIndex, m_sCurrentBitmap);
		m_listBitmaps.SetCurSel(iNewIndex);
	}
	UpdateData();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCreateAviDlg::SortList()
{
	if (m_listBitmaps.GetCount() < 3)
	{
		return FALSE;
	}

	CString s1, s2, sTemp;
	for (int i = 2; i < m_listBitmaps.GetCount(); i++)
	{
		for (int j = 0; j < m_listBitmaps.GetCount()-i; j++)
		{
			m_listBitmaps.GetText(j, s1);
			m_listBitmaps.GetText(j+1, s2);
			if (s1 > s2)
			{
				m_listBitmaps.DeleteString(j+1);
				m_listBitmaps.InsertString(j, s2);
//				m_listBitmaps.UpdateWindow();
			}
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CCreateAviDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreateAviDlg)
	DDX_Control(pDX, IDC_BTN_PLAY, m_btnPlay);
	DDX_Control(pDX, IDC_BTN_CLEAR, m_btnClear);
	DDX_Control(pDX, IDC_LIST_BITMAPS, m_listBitmaps);
	DDX_Control(pDX, IDC_BTN_UP, m_btnUp);
	DDX_Control(pDX, IDC_BTN_SORT, m_btnSort);
	DDX_Control(pDX, IDC_BTN_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_BTN_DOWN, m_btnDown);
	DDX_Control(pDX, IDC_BTN_DELETE, m_btnDelete);
	DDX_Control(pDX, IDC_BTN_ADD, m_btnAdd);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_sFilename);
	DDX_LBIndex(pDX, IDC_LIST_BITMAPS, m_iIndex);
	DDX_Text(pDX, IDC_EDIT_FPS, m_iFps);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CCreateAviDlg, CDialog)
	//{{AFX_MSG_MAP(CCreateAviDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_ADD, OnBtnAdd)
	ON_BN_CLICKED(IDC_BTN_DELETE, OnBtnDelete)
	ON_BN_CLICKED(IDC_BTN_DOWN, OnBtnDown)
	ON_BN_CLICKED(IDC_BTN_SAVE, OnBtnSave)
	ON_BN_CLICKED(IDC_BTN_SORT, OnBtnSort)
	ON_BN_CLICKED(IDC_BTN_UP, OnBtnUp)
	ON_EN_CHANGE(IDC_EDIT_FILENAME, OnChangeEditFilename)
	ON_LBN_SELCHANGE(IDC_LIST_BITMAPS, OnSelchangeListBitmaps)
	ON_BN_CLICKED(IDC_BTN_CLEAR, OnBtnClear)
	ON_EN_CHANGE(IDC_EDIT_FPS, OnChangeEditFps)
	ON_BN_CLICKED(IDC_BTN_PLAY, OnBtnPlay)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CCreateAviDlg message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CCreateAviDlg::OnInitDialog()
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
	SetIcon(m_hIcon, TRUE);			// Set big icone
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_btnUp.SetBitmap(m_hUp);
	m_btnDown.SetBitmap(m_hDown);
	OnBtnClear();
	EnableControls();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}
/////////////////////////////////////////////////////////////////////////////
void CCreateAviDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
/////////////////////////////////////////////////////////////////////////////
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CCreateAviDlg::OnPaint() 
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
HCURSOR CCreateAviDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
/////////////////////////////////////////////////////////////////////////////
void CCreateAviDlg::OnChangeEditFilename() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	m_sAviFilePath = "";
	EnableControls();
}
/////////////////////////////////////////////////////////////////////////////
void CCreateAviDlg::OnChangeEditFps() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	m_sAviFilePath = "";
	EnableControls();
}
/////////////////////////////////////////////////////////////////////////////
void CCreateAviDlg::OnSelchangeListBitmaps() 
{
	EnableControls();
}
/////////////////////////////////////////////////////////////////////////////
void CCreateAviDlg::OnBtnUp() 
{
	if (   (0 < m_iIndex)
		&& !m_sCurrentBitmap.IsEmpty()
		)
	{
		MoveBitmap(TRUE);
		m_sAviFilePath = "";
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCreateAviDlg::OnBtnDown() 
{
	if (   (m_iIndex != -1)
		&& (m_iIndex != m_listBitmaps.GetCount())
		&& !m_sCurrentBitmap.IsEmpty()
		)
	{
		MoveBitmap(FALSE);
		m_sAviFilePath = "";
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCreateAviDlg::OnBtnAdd() 
{
	char buffer[10000];
	ZeroMemory(buffer, sizeof(buffer));
	char* szFilter = "Bitmap Files (*.bmp)|*.bmp||";
	CFileDialog dlg(TRUE,					//BOOL bOpenFileDialog,
					"bmp",					//LPCTSTR lpszDefExt = NULL,
					NULL,					//LPCTSTR lpszFileName = NULL,
					OFN_ALLOWMULTISELECT,	//DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					szFilter,				//LPCTSTR lpszFilter = NULL,
					this					//CWnd* pParentWnd = NULL);
					);
#ifdef _DEBUG
	dlg.m_ofn.lpstrInitialDir= "D:\\Animation\\Akubis_Info";
#endif
	dlg.m_ofn.lpstrTitle= "Bitmaps hinzufügen";
	dlg.m_ofn.lpstrFile	= buffer;
	dlg.m_ofn.nMaxFile	= sizeof(buffer);
	int iRet = dlg.DoModal();
	if (iRet == IDOK)
	{
		POSITION pos = dlg.GetStartPosition();
		CString sFilename;
		while (pos)
		{
			sFilename = dlg.GetNextPathName(pos);
			if (!sFilename.IsEmpty())
			{
				m_iIndex = m_listBitmaps.InsertString(m_iIndex, sFilename);
				m_iIndex++;
				m_listBitmaps.SetCurSel(m_iIndex);
				m_sAviFilePath = "";
			}
		}
		EnableControls();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCreateAviDlg::OnBtnDelete() 
{
	if (   (m_iIndex != LB_ERR)
		&& !m_sCurrentBitmap.IsEmpty()
		)
	{
		m_listBitmaps.DeleteString(m_iIndex);
		m_listBitmaps.SetCurSel(m_iIndex-1);
		m_sAviFilePath = "";
		EnableControls();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCreateAviDlg::OnBtnSort() 
{
	SortList();	
	m_sAviFilePath = "";
	EnableControls();
}
/////////////////////////////////////////////////////////////////////////////
void CCreateAviDlg::OnBtnClear() 
{
	m_listBitmaps.ResetContent();
	m_listBitmaps.AddString("");
	m_listBitmaps.SetCurSel(0);
	m_sAviFilePath = "";
	EnableControls();
}
/////////////////////////////////////////////////////////////////////////////
void CCreateAviDlg::OnBtnSave() 
{
	BOOL bOK = TRUE;
	if (m_sFilename.Right(4) != ".avi")
	{
		m_sFilename += ".avi";
	}

	char buffer[MAX_PATH];
	if (GetCurrentDirectory(MAX_PATH, buffer))
	{
		m_sAviFilePath = CString(buffer) + "\\" + m_sFilename;
	}
	char* szFilter = "No Files (*.abcdxyz)|*.abcdxyz||";
	CDirDialog dlg(TRUE,					//BOOL bOpenFileDialog,
					"avi",					//LPCTSTR lpszDefExt = NULL,
					m_sAviFilePath,			//LPCTSTR lpszFileName = NULL,
					OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,	//DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					szFilter,				//LPCTSTR lpszFilter = NULL,
					this					//CWnd* pParentWnd = NULL);
					);
	dlg.m_ofn.lpstrInitialDir = buffer;
	dlg.m_ofn.lpstrTitle= "Verzeichnis wählen";
	int iRet = dlg.DoModal();
	if (iRet == IDOK)
	{
		m_sAviFilePath = dlg.GetPathName();
		CAVIFile avi(m_sAviFilePath, m_iFps);
		if (avi.IsOK())
		{
			CString sBitmap;
			HBITMAP hBitmap;
			CBitmap* pBitmap;
			for (int i=0; i<m_listBitmaps.GetCount()-1 ; i++)
			{
				m_listBitmaps.GetText(i, sBitmap);
				if (!sBitmap.IsEmpty())
				{
					hBitmap = (HBITMAP)LoadImage(NULL,
												sBitmap,
												IMAGE_BITMAP,
												0,
												0,
												LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE
												);
					if (hBitmap)
					{
						pBitmap = CBitmap::FromHandle(hBitmap);
						if (pBitmap)
						{
							bOK = avi.AddFrame(*pBitmap);
							if (!bOK)
							{
								break;
							}
						}
					}
				}
			}
			if (bOK)
			{
				avi.Compress();
				EnableControls();
			}
		}
	}
	if (!bOK)
	{
		m_sAviFilePath = "";
		TRY
		{
			CFile::Remove(m_sFilename);
		}
		CATCH( CFileException, e )
		{
			#ifdef _DEBUG
				afxDump << "File " << m_sFilename << " cannot be removed\n";
			#endif
		}
		END_CATCH
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCreateAviDlg::OnBtnPlay() 
{
	CAviPlayerDlg dlg(m_sAviFilePath, m_listBitmaps.GetCount()-2);
	dlg.DoModal();
}
