// LogExtractDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LogExtract.h"
#include "LogExtractDlg.h"

#include "wkclasses\Wk_ascii.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Einige Standardstringsdefinieren
static TCHAR BASED_CODE szTemp[]	= _T("Temp");

/////////////////////////////////////////////////////////////////////////////
// CLogExtractDlg dialog
CLogExtractDlg::CLogExtractDlg(CWnd* pParent /*=NULL*/)
	: CWK_Dialog(CLogExtractDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLogExtractDlg)
	m_sOutputFile = _T("Extract");
	m_sSearchFile = _T("");
	m_sCom = _T("");
	m_bOnlyCom = FALSE;
	m_bLineBreakAfter = FALSE;
	m_sLineBreakAfter = _T("");
	m_sSearchText = _T("");
	m_iSearchWhat = -1;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bSearching = FALSE;
	m_iCurrentIndex = 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLogExtractDlg::StretchElements() 
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CLogExtractDlg::ShowHide() 
{
	BOOL bEnable = FALSE;
	BOOL bExtraEnable = FALSE;

	bEnable = !m_bSearching;
	m_editOutputFile.EnableWindow(bEnable);
	m_btnOnlyCom.EnableWindow(bEnable);
	bExtraEnable = bEnable && m_bOnlyCom;
	m_editCom.EnableWindow(bExtraEnable);
	m_btnLineBreakAfter.EnableWindow(bEnable);
	bExtraEnable = bEnable && m_bLineBreakAfter;
	m_editLineBreakAfter.EnableWindow(bExtraEnable);
	m_btnSearchFile.EnableWindow(bEnable);
	CString sNoSearchFile;
	sNoSearchFile.LoadString(IDS_NO_SEARCH_FILE);
	bExtraEnable = bEnable && (m_sSearchFile!=sNoSearchFile);
	m_btnOK.EnableWindow(bExtraEnable);
	m_btnCancel.EnableWindow(bEnable);
	UpdateWindow();
}
//////////////////////////////////////////////////////////////////////
BOOL CLogExtractDlg::PrepareSearchFile()
{
	BOOL bReturn = FALSE;
	CString sPathFilenameExt = m_sSearchFile;
	CString sTempPathFileName;
	CString sFileReadFrom;
	// Extension extrahieren
	CString sPathFileName;
	CString sExtension;
	WK_SplitExtension(sPathFilenameExt, sPathFileName, sExtension);
	// ggf. komprimierte Datei dekomprimieren
	if (sExtension.CompareNoCase(".lgz") == 0) {
		CString sPath;
		CString sFileName;
		WK_SplitPath(sPathFileName, sPath, sFileName);
		// Temporären Dateinamen für das zu durchsuchende File erzeugen
		char* pszTempName = _tempnam(sFileName, szTemp);
		sTempPathFileName = pszTempName;
		free(pszTempName);
		// Datei dekomprimieren
		if ( DecompressLGZFile(sPathFilenameExt, sTempPathFileName) ) {
			TRACE( "File %s decompressed to %s\n",
						(LPCSTR)sPathFilenameExt, (LPCSTR)sTempPathFileName );
			sFileReadFrom = sTempPathFileName;
		}
		else {
			WK_TRACE_ERROR( "File decompress failed %s\n", (LPCSTR)sPathFilenameExt );
			sTempPathFileName.Empty();
		}
	}
	else {
		sFileReadFrom = sPathFilenameExt;
		sTempPathFileName.Empty();
	}
	// Datei oeffnen
	if (sFileReadFrom.IsEmpty() == FALSE) {
		CFile file;
		if ( file.Open(sFileReadFrom, CFile::modeRead) ) {
			m_sLogFile.Empty();
			m_iCurrentIndex = 0;
			DWORD dwLen = file.GetLength();
			// Assume that logfiles smaller than 2GB
			char* pData = m_sLogFile.GetBuffer(dwLen);
			// File laden
			DWORD dwRead = file.ReadHuge(pData, dwLen);
			if (dwRead == dwLen) {
				// Decodieren
				Decode((BYTE*)pData, dwLen);
				m_sLogFile.ReleaseBuffer();
				WK_TRACE_USER( "Searching File %s\n", (LPCSTR)sPathFilenameExt);
			}
			else {
				WK_TRACE_ERROR( "File read failed %s read %lu of %lu\n",
									(LPCSTR)sFileReadFrom, dwRead, dwLen );
			}
			file.Close();
			bReturn = TRUE;
		}
		else {
			WK_TRACE_ERROR( "File open failed %s\n", (LPCSTR)sFileReadFrom );
		}
		DeleteTempFile(sTempPathFileName);
	}
	else {
		// Kein Filename, keine Arbeit
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLogExtractDlg::ReadLine(CString& sLine)
{
	BOOL bReturn = FALSE;
	if (m_iCurrentIndex < m_sLogFile.GetLength() ) {
		int iFound = m_sLogFile.Find('\n', m_iCurrentIndex);
		if (iFound == -1){
			iFound = m_sLogFile.GetLength();
		}
		sLine = m_sLogFile.Mid(m_iCurrentIndex, iFound-m_iCurrentIndex);
		m_iCurrentIndex = iFound+1;
		bReturn = TRUE;
	}
	else {
		m_iCurrentIndex = 0;
		m_sLogFile.Empty();
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CLogExtractDlg::SearchAndCopy(const CString& sLine)
{
	int iIndex = sLine.Find(m_sSearchFor);
	if (-1 != iIndex)
	{
		CString sData;
		if (m_iSearchWhat == 0)
		{
			// we need only read characters
			BOOL bFound = FALSE;
			CString sPrefix;
			if ( -1 != sLine.Find(" read ") )
			{
				bFound = TRUE;
//				sPrefix = " byte(s) Ascii:";
				sPrefix = " byte(s) :";
			}
			else if ( -1 != sLine.Find(" R ") )
			{
				bFound = TRUE;
				sPrefix = " Ascii:";
			}
			if (bFound)
			{
				// extract the prefix
				iIndex = sLine.Find(sPrefix);
				if ( -1 != iIndex ) {
					iIndex += sPrefix.GetLength();
					sData = sLine.Mid(iIndex);
					// extract the suffix, if present
					iIndex = sData.Find("<ENDE>");
					if ( -1 != iIndex ) {
						sData = sData.Left(iIndex);
					}
				}
			}
		}
		else
		{
			sData = sLine;
		}
		if (sData.IsEmpty() == FALSE)
		{
			// insert line break if wanted
			if (m_bLineBreakAfter && !m_sLineBreakAfter.IsEmpty()) {
				iIndex = sData.Find(m_sLineBreakAfter);
				if ( -1 != iIndex ) {
					sData = sData.Left(iIndex)
							+ m_sLineBreakAfter
							+ "\r\n"
							+ sData.Mid(iIndex + m_sLineBreakAfter.GetLength());
				}
			}
			m_fileOutput.Write(sData, sData.GetLength());
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLogExtractDlg::DecompressLGZFile(const CString & sName, const CString &sNew)
{
	BOOL bReturn = FALSE;
	CString sNewNameLog = sNew+".log";
	CString sNewNameLgz = sNew+".lgz";
	
	if (CopyFile(sName,sNewNameLgz,TRUE)) {
		if (WK_DecompressLogFile(sNewNameLgz)) {
			TRY {
				CFile::Rename(sNewNameLog, sNew);
				bReturn = TRUE;
			}
			CATCH( CFileException, e ) {
				WK_TRACE_ERROR( "Could not rename tmp file %s to %s\n",
									(LPCSTR)sNewNameLog, (LPCSTR)sNew);
			}
			END_CATCH
		}
		else
		{
			WK_TRACE_ERROR( "Could not decompress tmp file %s\n", (LPCSTR)sNewNameLgz);
		}
	}
	else
	{
		WK_TRACE_ERROR( "Could not copy tmp file %s to %s\n (%s)\n",
							(LPCSTR)sName, (LPCSTR)sNewNameLgz, GetLastErrorString() );
	}
	return bReturn;
}
//////////////////////////////////////////////////////////////////////
BOOL CLogExtractDlg::DeleteTempFile(const CString& sTempFileName)
{
	BOOL bReturn = FALSE;
	if (FALSE == sTempFileName.IsEmpty() ) {
		TRY {
			CFile::Remove(sTempFileName);
			bReturn = TRUE;
		}
		CATCH( CFileException, e ) {
			WK_TRACE_ERROR("Could not remove tmp file %s (%i)\n",
									(LPCSTR)sTempFileName, e->m_cause);
			bReturn = FALSE;
		}
		END_CATCH
	}
	else {
	}
	return bReturn;
}

/////////////////////////////////////////////////////////////////////////////
void CLogExtractDlg::DoDataExchange(CDataExchange* pDX)
{
	CWK_Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogExtractDlg)
	DDX_Control(pDX, IDC_ASCII_BIN, m_btnAsciiToBin);
	DDX_Control(pDX, IDC_EDIT_LINE_BREAK_AFTER, m_editLineBreakAfter);
	DDX_Control(pDX, IDC_CHECK_LINE_BREAK_AFTER, m_btnLineBreakAfter);
	DDX_Control(pDX, IDC_CHECK_ONLY_COM, m_btnOnlyCom);
	DDX_Control(pDX, IDC_EDIT_COM, m_editCom);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_EDIT_OUTPUT_FILE, m_editOutputFile);
	DDX_Control(pDX, IDC_BTN_SEARCH_FILE, m_btnSearchFile);
	DDX_Text(pDX, IDC_EDIT_OUTPUT_FILE, m_sOutputFile);
	DDX_Text(pDX, IDC_TXT_SEARCH_FILE, m_sSearchFile);
	DDX_Text(pDX, IDC_EDIT_COM, m_sCom);
	DDX_Check(pDX, IDC_CHECK_ONLY_COM, m_bOnlyCom);
	DDX_Check(pDX, IDC_CHECK_LINE_BREAK_AFTER, m_bLineBreakAfter);
	DDX_Text(pDX, IDC_EDIT_LINE_BREAK_AFTER, m_sLineBreakAfter);
	DDX_Text(pDX, IDC_EDIT_SEARCH_TEXT, m_sSearchText);
	DDX_Radio(pDX, IDC_RADIO_RS232_ASCII_DATA, m_iSearchWhat);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CLogExtractDlg, CWK_Dialog)
	//{{AFX_MSG_MAP(CLogExtractDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_SEARCH_FILE, OnBtnSearchFile)
	ON_EN_CHANGE(IDC_EDIT_OUTPUT_FILE, OnChangeEditOutputFile)
	ON_BN_CLICKED(IDC_CHECK_ONLY_COM, OnCheckOnlyCom)
	ON_EN_CHANGE(IDC_EDIT_COM, OnChangeEditCom)
	ON_BN_CLICKED(IDC_CHECK_LINE_BREAK_AFTER, OnCheckLineBreakAfter)
	ON_EN_CHANGE(IDC_EDIT_LINE_BREAK_AFTER, OnChangeEditLineBreakAfter)
	ON_BN_CLICKED(IDC_ASCII_BIN, OnAsciiBin)
	ON_EN_CHANGE(IDC_EDIT_SEARCH_TEXT, OnChangeEditSearchText)
	ON_BN_CLICKED(IDC_RADIO_RS232_ASCII_DATA, OnRadioSearchWhat)
	ON_BN_CLICKED(IDC_RADIO_FREESTYLE, OnRadioSearchWhat)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CLogExtractDlg message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CLogExtractDlg::OnInitDialog()
{
	CWK_Dialog::OnInitDialog();

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
	m_sSearchFile.LoadString(IDS_NO_SEARCH_FILE);
	UpdateData(FALSE);
	ShowHide();

	return TRUE;  // return TRUE  unless you set the focus to a control
}
/////////////////////////////////////////////////////////////////////////////
void CLogExtractDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
//		CAboutDlg dlgAbout;
//		dlgAbout.DoModal();
	}
	else
	{
		CWK_Dialog::OnSysCommand(nID, lParam);
	}
}
/////////////////////////////////////////////////////////////////////////////
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CLogExtractDlg::OnPaint() 
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
		CWK_Dialog::OnPaint();
	}
}
/////////////////////////////////////////////////////////////////////////////
// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLogExtractDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
/////////////////////////////////////////////////////////////////////////////
void CLogExtractDlg::OnBtnSearchFile() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE);
	if (dlg.DoModal() == IDOK) {
		m_sSearchFile = dlg.GetPathName();
		UpdateData(FALSE);
		ShowHide();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CLogExtractDlg::OnChangeEditOutputFile() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CWK_Dialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData();
}
/////////////////////////////////////////////////////////////////////////////
void CLogExtractDlg::OnCheckOnlyCom() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if (m_bOnlyCom) {
		ShowHide();
		m_editCom.SetFocus();
	}
	else {
		m_sCom.Empty();
		UpdateData(FALSE);
		ShowHide();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CLogExtractDlg::OnChangeEditCom() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CWK_Dialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData();
}
/////////////////////////////////////////////////////////////////////////////
void CLogExtractDlg::OnCheckLineBreakAfter() 
{
	UpdateData();
	if (m_bLineBreakAfter) {
		ShowHide();
		m_editLineBreakAfter.SetFocus();
	}
	else {
		m_sLineBreakAfter.Empty();
		UpdateData(FALSE);
		ShowHide();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CLogExtractDlg::OnChangeEditLineBreakAfter() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CWK_Dialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData();
}
/////////////////////////////////////////////////////////////////////////////
void CLogExtractDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	if ((m_sSearchFile.IsEmpty() == FALSE) && (m_sOutputFile.IsEmpty() == FALSE)) {
		m_bSearching = TRUE;
		ShowHide();
		if (PrepareSearchFile()) {
			// output file path
			CString sPath;
			CString sFileName;
			WK_SplitPath(m_sSearchFile, sPath, sFileName);
			if (m_fileOutput.Open(sPath + m_sOutputFile + ".txt",
									CFile::modeWrite|CFile::modeCreate|CFile::shareExclusive))
			{
				if (m_iSearchWhat == 0)
				{
					m_sSearchFor = ":COM";
					if (m_bOnlyCom) {
						m_sSearchFor += m_sCom;
					}
				}
				CString sCurrentLine;
				while (ReadLine(sCurrentLine)) {
					SearchAndCopy(sCurrentLine);
				}
/*				CString sLineFeed = "\n";
				m_fileOutput.Write(sLineFeed, sLineFeed.GetLength());
*/				m_fileOutput.Close();
			}
		}
		m_bSearching = FALSE;
		ShowHide();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CLogExtractDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CWK_Dialog::OnCancel();
}
/////////////////////////////////////////////////////////////////////////////
void CLogExtractDlg::OnAsciiBin() 
{
	CFile source;
	CFile dest;
	CString sd,ss;
	char c;
	CString sPath;
	CString sFileName;
	WK_SplitPath(m_sSearchFile, sPath, sFileName);
	sd = sPath + m_sOutputFile + ".dat";
	ss = sPath + m_sOutputFile + ".txt";

	if (source.Open(ss,CFile::modeRead))
	{
		if (dest.Open(sd,CFile::modeCreate|CFile::modeWrite))
		{

			while (1==source.Read(&c,1))
			{
				if (c == '<')
				{
					CString esc;

					while (1==source.Read(&c,1))
					{
						if (c == '>')
						{
							char d = 'a';
							if (0 == esc.CompareNoCase("cr"))
							{
								d = ASCII_CR;
							}
							else if (0 == esc.CompareNoCase("ff"))
							{
								d = ASCII_FF;
							}
							else if (0 == esc.CompareNoCase("lf"))
							{
								d = ASCII_LF;
							}
							else if (0 == esc.CompareNoCase("stx"))
							{
								d = ASCII_STX;
							}
							else if (0 == esc.CompareNoCase("etx"))
							{
								d = ASCII_ETX;
							}
							else if (0 == esc.CompareNoCase("ack"))
							{
								d = ASCII_ACK;
							}
							else if (0 == esc.CompareNoCase("nak"))
							{
								d = ASCII_NAK;
							}
							else if (0 == esc.CompareNoCase("us"))
							{
								d = ASCII_US;
							}

							if (d!='a')
							{
								dest.Write(&d,1);
							}
							break;
						}
						else
						{
							esc += c;
						}
					}
				}
				else
				{
					dest.Write(&c,1);
				}
			}

			dest.Close();
		}

		source.Close();
	}
}

void CLogExtractDlg::OnChangeEditSearchText() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CWK_Dialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void CLogExtractDlg::OnRadioSearchWhat() 
{
	// TODO: Add your control notification handler code here
	
}
