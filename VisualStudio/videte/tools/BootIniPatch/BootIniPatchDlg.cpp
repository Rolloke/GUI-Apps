// BootIniPatchDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BootIniPatch.h"
#include "BootIniPatchDlg.h"
#include "AboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBootIniPatchDlg dialog
CBootIniPatchDlg::CBootIniPatchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBootIniPatchDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBootIniPatchDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_uErrors = 0;
	m_uWarnings = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CBootIniPatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBootIniPatchDlg)
	DDX_Control(pDX, IDC_LIST_OUTPUT, m_lbTextOutput);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CBootIniPatchDlg, CDialog)
	//{{AFX_MSG_MAP(CBootIniPatchDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER, OnUser)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
BOOL CBootIniPatchDlg::OnInitDialog()
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
	
	// TODO: Add extra initialization here
	CenterWindow();
	ShowWindow(SW_SHOW);
//	PostMessage(WM_USER);
		
	return TRUE;  // return TRUE  unless you set the focus to a control
}
/////////////////////////////////////////////////////////////////////////////
void CBootIniPatchDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
void CBootIniPatchDlg::OnPaint() 
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
HCURSOR CBootIniPatchDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CBootIniPatchDlg::OnUser(WPARAM, LPARAM)
{
	ClearData();
	DWORD dwCount = SearchBootIniFiles();
	if (dwCount)
	{
		CString sFile;
		for (int i=0 ; i<m_saBootIniFiles.GetSize() ; i++)
		{
			sFile = m_saBootIniFiles.GetAt(i);
			PatchBootIniFile(sFile);
			CopySystemFiles(sFile.Left(2));
		}
		CString sMsg;
		PrintOutput(sMsg);
		sMsg.Format(_T("%u Fehler, %u Warnungen\n"), m_uErrors, m_uWarnings);
		PrintOutput(sMsg);
		UINT uIcon = ((m_uErrors||m_uWarnings) ? MB_ICONERROR : MB_ICONINFORMATION);
		AfxMessageBox(sMsg, MB_OK|uIcon);
	}
	else
	{
		AfxMessageBox(_T("Es wurden keine weiteren boot.ini Dateien gefunden"));
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CBootIniPatchDlg::SearchBootIniFiles() 
{
	DWORD dwBootIniCount = 0;
	DWORD dwLD = GetLogicalDrives();
	DWORD dwBit = (1 << 3);
	char cDrive = 'D';
	CString sRoot;
	CString sFile;
	CString sMsg;
	UINT uDriveType = DRIVE_UNKNOWN;

	while (dwBit!=0 && cDrive<='Z')
	{
		if (dwLD & dwBit)
		{
			sRoot.Format(_T("%c:\\"), cDrive);
			uDriveType = GetDriveType(sRoot);
			if (   (uDriveType != DRIVE_UNKNOWN)
				&& (uDriveType != DRIVE_NO_ROOT_DIR)
				&& (uDriveType != DRIVE_REMOTE)
				&& (uDriveType != DRIVE_CDROM) 
				)
			{
				sFile.Format(_T("%sboot.ini"), sRoot);
				if (DoesFileExist(sFile))
				{
					m_saBootIniFiles.Add(sFile);
					dwBootIniCount++;
					sMsg.Format(_T("%s gefunden"), sFile);
					PrintOutput(sMsg);
				}
			}
		}
		dwBit <<= 1;
		cDrive++;
	}
	sMsg.Format(_T("%u Dateien gefunden"), dwBootIniCount);
	PrintOutput(sMsg);
	return dwBootIniCount;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBootIniPatchDlg::DoesFileExist(LPCTSTR szFile)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE h = FindFirstFile(szFile, &FindFileData);
	if (h == INVALID_HANDLE_VALUE){
		return FALSE;
	}
	FindClose(h);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CBootIniPatchDlg::PatchBootIniFile(CString sFile)
{
	DWORD dwPatches = 0;
	CString sMsg;
	PrintOutput(sMsg);
	sMsg.Format(_T("Bearbeite Datei %s\n"), sFile);
	PrintOutput(sMsg);

	CString sFileBak = sFile +_T(".bak");

	CFileStatus statusFileOrg, statusFileWork;
	if (CFile::GetStatus(sFile, statusFileOrg))
	{
		memset(&statusFileWork, 0, sizeof(statusFileWork));
		statusFileWork.m_attribute = CFile::normal;
		TRY
		{
			CFile::SetStatus(sFile, statusFileWork);
			if (_trename(sFile, sFileBak) == 0)
			{
				BOOL bNewWritten = FALSE;
				CStdioFile fileIn;
				if (fileIn.Open(sFileBak, CFile::modeRead|CFile::shareExclusive))
				{
					CStdioFile fileOut;
					if (fileOut.Open(sFile, CFile::modeWrite|CFile::modeCreate|CFile::shareExclusive))
					{
						CString sLine;
						while (fileIn.ReadString(sLine))
						{
							if (sLine.Find(_T("rdisk(")) != -1)
							{
								sMsg = _T("Gefunden");
								PrintOutput(sMsg);
								PrintOutput(sLine);
								if (sLine.Find(_T("rdisk(0)")) != -1)
								{
									sMsg.Format(_T("OK"));
									PrintOutput(sMsg);
								}
								else if (sLine.Find(_T("rdisk(1)")) != -1)
								{
									if (sLine.Replace(_T("rdisk(1)"), _T("rdisk(0)")) != 0)
									{
										dwPatches++;
										sMsg = _T("Ersetzt durch");
										PrintOutput(sMsg);
										PrintOutput(sLine);
									}
									else
									{
										sMsg.Format(_T("FEHLER: Nicht ersetzt"));
										OnError(sMsg);
									}
								}
								else
								{
									sMsg.Format(_T("WARNUNG: Nicht ersetzt"));
									OnWarning(sMsg);
								}
							}
							fileOut.WriteString(sLine+_T("\n"));
						}
						fileOut.Close();
						bNewWritten = TRUE;
						sMsg.Format(_T("%u Einträge ersetzt"), dwPatches);
						PrintOutput(sMsg);
					}
					else
					{
						sMsg.Format(_T("FEHLER: %s konnte nicht geöffnet werden!"), sFileBak);
						OnError(sMsg);
						if (_trename(sFileBak, sFile) != 0)
						{
							sMsg.Format(_T("FEHLER: %s konnte nicht umbenannt werden!"), sFileBak);
							OnError(sMsg);
						}
					}
					fileIn.Close();
				}
				else
				{
					sMsg.Format(_T("FEHLER: %s konnte nicht geöffnet werden!"), sFileBak);
					OnError(sMsg);
				}
				if (bNewWritten)
				{
					TRY
					{
						CFile::Remove(sFileBak);
					}
					CATCH (CFileException, e)
					{
						sMsg.Format(_T("FEHLER: %s konnte nicht gelöscht werden!"), sFileBak);
						OnError(sMsg);
					}
					END_CATCH
				}
				else
				{
					if (_trename(sFileBak, sFile) != 0)
					{
						sMsg.Format(_T("FEHLER: %s konnte nicht umbenannt werden!"), sFileBak);
						OnError(sMsg);
					}
				}
			}
			else
			{
				sMsg.Format(_T("FEHLER: %s konnte nicht umbenannt werden!"), sFile);
				OnError(sMsg);
			}
			statusFileWork.m_attribute = statusFileOrg.m_attribute;
			CFile::SetStatus(sFile, statusFileWork);
		}
		CATCH (CFileException, e)
		{
			e->ReportError(MB_OK|MB_ICONERROR);
			sMsg.Format(_T("FEHLER: beim Datei-Zugriff: %s , Nr. %li!"), e->m_strFileName, e->m_lOsError);
			OnError(sMsg);
		}
		END_CATCH
	}
	else
	{
		sMsg.Format(_T("FEHLER: %s konnte nicht gefunden werden!"), sFile);
		OnError(sMsg);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CBootIniPatchDlg::CopySystemFiles(CString sDrive)
{
	CString sSource, sDest;
	sSource = sDrive + _T("\\image\\system");
	sDest = sDrive + _T("\\windows\\system32\\config\\system");

	CString sMsg;
	PrintOutput(sMsg);
	sMsg.Format(_T("Bearbeite System-Datei %s\n"), sDest);
	PrintOutput(sMsg);

	CFileStatus statusFileSource;
	if (CFile::GetStatus(sSource, statusFileSource))
	{
		CFileStatus statusFileDest;
		if (CFile::GetStatus(sDest, statusFileDest))
		{
			CFileStatus statusFileWork;
			memset(&statusFileWork, 0, sizeof(statusFileWork));
			statusFileWork.m_attribute = CFile::normal;
			TRY
			{
				CFile::SetStatus(sDest, statusFileWork);
				CFile fileIn;
				if (fileIn.Open(sSource, CFile::modeRead|CFile::shareExclusive))
				{
					CFile fileOut;
					if (fileOut.Open(sDest, CFile::modeWrite|CFile::modeCreate|CFile::shareExclusive))
					{
						CByteArray buffer;
						buffer.SetSize(statusFileSource.m_size);
						LONG lReadTotal = 0;
						LONG lRead = 0;
						while (lReadTotal < statusFileSource.m_size)
						{
							lRead = fileIn.Read(buffer.GetData(), buffer.GetSize());
							fileOut.Write(buffer.GetData(), lRead);
							lReadTotal += lRead;
						}
						fileOut.Close();
						sMsg.Format(_T("Datei kopiert %s"), sDest);
						PrintOutput(sMsg);
						statusFileWork.m_attribute = statusFileDest.m_attribute;
						CFile::SetStatus(sDest, statusFileWork);
					}
					else
					{
						sMsg.Format(_T("FEHLER: %s konnte nicht geöffnet werden!"), sDest);
						OnError(sMsg);
					}
					fileIn.Close();
				}
				else
				{
					sMsg.Format(_T("FEHLER: %s konnte nicht geöffnet werden!"), sSource);
					OnError(sMsg);
				}
			}
			CATCH (CFileException, e)
			{
				e->ReportError(MB_OK|MB_ICONERROR);
				sMsg.Format(_T("FEHLER: beim Datei-Zugriff: %s , Nr. %li!"), e->m_strFileName, e->m_lOsError);
				OnError(sMsg);
			}
			END_CATCH
		}
		else
		{
			sMsg.Format(_T("FEHLER: %s konnte nicht gefunden werden!"), sDest);
			OnError(sMsg);
		}
	}
	else
	{
		sMsg.Format(_T("FEHLER: %s konnte nicht gefunden werden!"), sSource);
		OnError(sMsg);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CBootIniPatchDlg::OnWarning(CString& sText)
{
	m_uWarnings++;
	PrintOutput(sText);
}
/////////////////////////////////////////////////////////////////////////////
void CBootIniPatchDlg::OnError(CString& sText)
{
	m_uErrors++;
	PrintOutput(sText);
	AfxMessageBox(sText, MB_OK|MB_ICONERROR);
}
/////////////////////////////////////////////////////////////////////////////
void CBootIniPatchDlg::PrintOutput(CString& sText)
{
	int iIndex = m_lbTextOutput.AddString(sText);
	m_lbTextOutput.SetTopIndex(iIndex);
	m_lbTextOutput.UpdateWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CBootIniPatchDlg::ClearData()
{
	m_uErrors = 0;
	m_uWarnings = 0;
	m_lbTextOutput.ResetContent();
	m_lbTextOutput.UpdateWindow();
	m_saBootIniFiles.RemoveAll();
}
