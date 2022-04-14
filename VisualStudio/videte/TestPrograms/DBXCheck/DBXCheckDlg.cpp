/* GlobalReplace: PictData6 --> COldPictData */
/* GlobalReplace: pReportBox --> g_pReportBox */
// DBXCheckDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DBXCheck.h"
#include "DBXCheckDlg.h"

#include "WK_File.h"
#include "DVRCore/OldPictData.h"
#include "Util.h"
#include "Direct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


int MY_SearchFiles(
			   CStringArray &result,
			   const CString sPathname,
			   const CString &sPattern,
			   BOOL bIncludeDirectories, /*=FALSE*/
			   BOOL bRecurse
			   )
{
	HANDLE hF;
	WIN32_FIND_DATA FindFileData;
	CString sSearch;
	BOOL bIsEmpty = TRUE;
	int iNumFound=0;

	sSearch = sPathname;
	if (sSearch.GetLength()==0) {
		sSearch += ".\\";
	} else if (sSearch[sSearch.GetLength()-1]!='\\') {
		sSearch += '\\';
	}
	sSearch += sPattern;

	hF = FindFirstFile((const char*)sSearch,&FindFileData);
	if (hF != INVALID_HANDLE_VALUE)
	{
		if ( (strcmp(FindFileData.cFileName,".")!=0) &&
			 (strcmp(FindFileData.cFileName,"..")!=0))
		{
			if (bIncludeDirectories
				|| (FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0
				) {
			result.Add(FindFileData.cFileName);
			iNumFound++;
			}
		} 
		while (FindNextFile(hF,&FindFileData))
		{
			if ( (strcmp(FindFileData.cFileName,".")!=0) &&
				 (strcmp(FindFileData.cFileName,"..")!=0))
			{
				if ((FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)) {
					if (bIncludeDirectories ) {
						result.Add(FindFileData.cFileName);
						iNumFound++;
					}
					if (bRecurse) {
						CString sSubDir;
						sSubDir = sPathname;
						if (sSubDir[sSubDir.GetLength()-1]!= '\\') {
							sSubDir += '\\';
						}
						sSubDir += FindFileData.cFileName;
						MY_SearchFiles(
							result,
							sSubDir,
							sPattern,
							bIncludeDirectories,
							bRecurse
						);
					}
				}	// end of directory
				else {
					// non directory
					CString sTmp(sPathname);
					sTmp += "\\";
					sTmp += FindFileData.cFileName;
					result.Add(sTmp);
					iNumFound++;
				}
			}
		}
		FindClose(hF);
	}
	else
	{
#if 0
		// NOT YET error or pattern not matched
		WK_TRACE("read directory failed %s, %s",
				  (const char*)sPathname,(const char*)GetLastErrorString());
#endif
	}

	return iNumFound;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

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

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDBXCheckDlg dialog

CDBXCheckDlg::CDBXCheckDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDBXCheckDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDBXCheckDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDBXCheckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDBXCheckDlg)
	DDX_Control(pDX, IDC_REPORT, m_ctlReport);
	DDX_Control(pDX, IDC_DRIVE_LIST, m_ctlDriveList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDBXCheckDlg, CDialog)
	//{{AFX_MSG_MAP(CDBXCheckDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_DO_TEST, OnDoTest)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDBXCheckDlg message handlers

BOOL CDBXCheckDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	g_pReportBox = & m_ctlReport;

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
	
	// TODO: Add extra initialization here

	char tmpDrives[1000];
	DWORD dwNumDrives = GetLogicalDriveStrings( 
		sizeof(tmpDrives),	// size of buffer 
		tmpDrives			// address of buffer for drive strings 
		); 

	char *szDrive = &tmpDrives[0];

	while (szDrive[0]) {
		TRACE("ROOT %s\n",szDrive);

		DWORD dwType = GetDriveType(szDrive);
		BOOL bAdd=TRUE;

		switch (dwType) {
			case DRIVE_UNKNOWN:
			case DRIVE_NO_ROOT_DIR : //  The root directory does not exist. 

 			case DRIVE_FIXED :	// The disk cannot be removed from the drive. 
				bAdd=TRUE;
				break;
 			
			case DRIVE_REMOTE : // The drive is a remote (network) drive. 
				bAdd = FALSE;
				break;

			case DRIVE_REMOVABLE :// The disk can be removed from the drive. 
			case DRIVE_CDROM:	// The drive is a CD-ROM drive. 
 			case DRIVE_RAMDISK: // The drive is a RAM disk. 
				bAdd=FALSE;
				break;
			default:
				bAdd=TRUE;
 
		}
		if (bAdd) {
			int ix = m_ctlDriveList.AddString(szDrive);
			m_ctlDriveList.SetCheck( ix, 1);
		}
		
		szDrive += strlen(szDrive)+1;
	}

	PostMessage(WM_COMMAND, IDC_DO_TEST);	// OOPS autorun
 
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDBXCheckDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CDBXCheckDlg::OnPaint() 
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
HCURSOR CDBXCheckDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDBXCheckDlg::OnDoTest() 
{
	CStringArray dbxFiles;

	MY_WK_TRACE("Running test...\n");

	int i;
	int iCount = m_ctlDriveList.GetCount();
	for (i=0;i<iCount;i++) {
		CString sDrive;
		if (m_ctlDriveList.GetCheck(i)==1) {	// is it activated/checked
			m_ctlDriveList.GetText(i,sDrive);
			MY_WK_TRACE("Checking drive %s\n",sDrive);

			CString sDir;
			sDir.Format("%sDatabase\\",sDrive);
			MY_SearchFiles(
			   dbxFiles,// adds files no clear!
			   sDir,
			   "*",
			   FALSE, // include directories
			   TRUE
			   );
		}
	}

	// drop all non-dbx files
	for (i=0;i<dbxFiles.GetSize();i++) {
		if (dbxFiles[i].Find("dbx") == -1
			&& dbxFiles[i].Find("DBX") == -1) {
			dbxFiles.RemoveAt(i);
			i--;	// adjust loop
		}
	}

	MY_WK_TRACE("Found %d dbx files\n",dbxFiles.GetSize());

	COldPictData pictData;
	int iNumOpenErrors=0;
	char *pData = new  char [ 500 * 1024];

	static DWORD dwCounter = 0;
    _mkdir("E:\\Jpeg1");
    _mkdir("E:\\Jpeg1\\Corrupt");
    _mkdir("E:\\Jpeg1\\Valid");

	for (i=0;i<dbxFiles.GetSize();i++) {
		BOOL bOpenOkay = pictData.MyOpen(dbxFiles[i]);	// readOnly
		if (bOpenOkay) {
			int iCount = pictData.GetPictCount();
			MY_WK_TRACE("'%s' contains %d pictures\n",dbxFiles[i],iCount);
			for (int p=1;p<=iCount;p++) {
				DWORD dwLen = pictData.GetPictData (p, pData);
				if (dwLen==0) {
					MY_WK_TRACE("ERROR:Could not read image data in %s, at pos %d",
						dbxFiles[i], p
						);
				}
				else
				{
#if (1) // Export aller Jpegfiles mit der Auflösung 0x0
					SIZE Size;
					Size.cx=0;
					Size.cy=0;
					CString sName;

					GetJpegSize(pData, dwLen,  Size);
					if ((Size.cx == 0) || (Size.cy == 0) || (*((WORD*)pData) != 0xd8ff))
						sName.Format("E:\\Jpeg1\\Corrupt\\%06lu.jpg", dwCounter++);
					else
						sName.Format("E:\\Jpeg1\\Valid\\%06lu.jpg", dwCounter++);

					CFile wFile(sName,	CFile::modeCreate | CFile::modeWrite  |
									CFile::typeBinary | CFile::shareDenyWrite);

					wFile.WriteHuge(pData, dwLen); 
				}
#endif
			}
			pictData.Close();
		} else {
			MY_WK_TRACE("ERROR:Could not open '%s'\n",dbxFiles[i]);
			iNumOpenErrors++;
		}

		Sleep(50);
	}

	WK_DELETE_ARRAY(pData);


	MY_WK_TRACE("Test done.\n");

	PostQuitMessage(123);	// OOPS autotun
}


void CDBXCheckDlg::AddLine(const CString &sLine) 
{
}

void CDBXCheckDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	g_pReportBox = NULL;	
}
