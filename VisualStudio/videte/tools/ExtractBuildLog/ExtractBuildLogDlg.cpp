// ExtractBuildLogDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ExtractBuildLog.h"
#include "ExtractBuildLogDlg.h"
#include "StringArrayOnce.h"
#include <psapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtractBuildLogDlg dialog
CExtractBuildLogDlg::CExtractBuildLogDlg(int iFileType,
										 CString sWorkspaceDir,
										 CString sWorkspaceName,
										 CWnd* pParent /*=NULL*/)
	: CDialog(CExtractBuildLogDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExtractBuildLogDlg)
	m_sOutputFile = _T(sWorkspaceDir.Left(3) + "ExtractBuildLogOutput.csv");
	m_sInputFile = sWorkspaceDir + '\\' + sWorkspaceName + ".plg";
	m_sStatus = _T("Waiting for action...");
	m_sLibrary = _T("");
	m_iFileType = iFileType;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_sWorkspaceName = sWorkspaceName;
	if (m_iFileType == 0)
	{
		m_sOutputFile = _T(sWorkspaceDir.Left(2) + '\\' + "ExtractDocomaticList.csv");
		m_sInputFile = sWorkspaceDir + '\\' + sWorkspaceName + ".txt";
		m_sLibrary = sWorkspaceName;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CExtractBuildLogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExtractBuildLogDlg)
	DDX_Control(pDX, IDC_COMBO_LIBRARY, m_cbLibrary);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Text(pDX, IDC_EDIT_OUTPUT_FILE, m_sOutputFile);
	DDX_Text(pDX, IDC_EDIT_INPUT_FILE, m_sInputFile);
	DDX_Text(pDX, IDC_TXT_STATUS_ACTUAL, m_sStatus);
	DDX_CBString(pDX, IDC_COMBO_LIBRARY, m_sLibrary);
	DDX_Radio(pDX, IDC_RADIO_FT_1, m_iFileType);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CExtractBuildLogDlg, CDialog)
	//{{AFX_MSG_MAP(CExtractBuildLogDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_EDIT_OUTPUT_FILE, OnChangeEditOutputFile)
	ON_EN_CHANGE(IDC_EDIT_INPUT_FILE, OnChangeEditInputFile)
	ON_CBN_SELCHANGE(IDC_COMBO_LIBRARY, OnSelchangeComboLibrary)
	ON_CBN_EDITCHANGE(IDC_COMBO_LIBRARY, OnEditchangeComboLibrary)
	ON_BN_CLICKED(IDC_RADIO_FT_1, OnRadioFileType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
void CExtractBuildLogDlg::UpdateDialog(BOOL bSaveAndValidate /*=TRUE*/)
{
	UpdateData(bSaveAndValidate);
	// Enable Controls
	BOOL bEnable =    !m_sInputFile.IsEmpty()
				   && !m_sLibrary.IsEmpty()
				   && !m_sOutputFile.IsEmpty();
	m_btnOK.EnableWindow(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CExtractBuildLogDlg::ExtractDocomaticList(CStdioFile& file)
{
	m_sStatus = _T("Extracting Doc-O-Matic list...");
	UpdateDialog(FALSE);
	CStringArrayOnce sLines;
	CString sLine, sOutput;
	while (file.ReadString(sLine))
	{
		if (!sLine.IsEmpty())
		{
			if (   sLine.Left(1) == "b"
				|| sLine.Left(2) == "cs"
				|| sLine.Left(2) == "dw"
				|| sLine.Left(2) == "g_"
				|| sLine.Left(1) == "i"
				|| sLine.Left(2) == "sz"
				|| sLine.Left(3) == "the"
				|| sLine.Left(1) == "_"
				|| sLine.Left(4) == "AFX_"
				|| sLine.Left(4) == "API_"
				|| sLine.Left(4) == "BIT_"

				|| sLine.Right(4) == ".cpp"
				|| sLine.Right(2) == ".c"
				|| sLine.Right(4) == ".dsp"
				|| sLine.Right(2) == ".h"
				|| sLine.Right(4) == ".inl"
				|| sLine.Right(2) == "_H"
				)
			{
				// ok, skip this line
			}
			else
			{
				// some extracting
				CString sSearch = "::";
				int iIndex = sLine.Find(sSearch);
				if (iIndex != -1)
				{
					sLine = sLine.Left(iIndex);
					sSearch = "@";
					iIndex = sLine.Find(sSearch);
					if (iIndex != -1)
					{
						sLine = sLine.Left(iIndex);
					}
				}
				else
				{
					sSearch = "@";
					iIndex = sLine.Find(sSearch);
					if (iIndex != -1)
					{
						sLine = sLine.Left(iIndex);
					}
				}
				sOutput.Format("%s,%s\n", m_sLibrary, sLine);
				if (!sLine.IsEmpty())
				{
					sLines.AddOnce(sOutput);
				}
			}
		}
	}
	CStdioFile fileOut;
	if (fileOut.Open(m_sOutputFile, CFile::modeWrite|CFile::modeCreate))
	{
		for (int i=0 ; i<sLines.GetSize() ; i++)
		{
			fileOut.WriteString(sLines.GetAt(i));
		}
		fileOut.Close();
		// Call Excel to open output
		ShellExecute(NULL, "open", m_sOutputFile,  NULL,  "", SW_SHOWNORMAL);
	}
	else
	{
		CString sMsg;
		sMsg.Format("Could not open output file\n%s", m_sOutputFile);
		AfxMessageBox(sMsg, MB_OK);
	}
	m_sStatus = _T("Ready!   Waiting for action...");
	UpdateDialog(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CExtractBuildLogDlg::ExtractLinkErrors(CStdioFile& file)
{
	m_sStatus = _T("Extracting LINK errors...");
	UpdateDialog(FALSE);
	CStringArrayOnce sLines;
	BOOL bStarted = FALSE;
	BOOL bContinue = TRUE;
	CString sLine, sOutput;
	while (   bContinue
		   && file.ReadString(sLine)
		   )
	{
		if (!sLine.IsEmpty())
		{
			if (bStarted)
			{
				if (sLine.Find(">Results<") != -1)
				{
					bContinue = FALSE;
					AfxMessageBox("No Errors found", MB_OK);
				}
				else if (sLine.Find(" Creating library ") != -1)
				{
					// ok, skip this line
				}
				else if (sLine.Find("LINK : warning LNK") != -1)
				{
					// ok, skip this line
				}
				else if (sLine.Find("fatal error LNK1120") != -1)
				{
					bContinue = FALSE;
					CStdioFile fileOut;
					if (fileOut.Open(m_sOutputFile, CFile::modeWrite|CFile::modeCreate))
					{
						for (int i=0 ; i<sLines.GetSize() ; i++)
						{
							fileOut.WriteString(sLines.GetAt(i));
						}
						fileOut.Close();
						// Call Excel to open output
						ShellExecute(NULL, "open", m_sOutputFile,  NULL,  "", SW_SHOWNORMAL);
					}
					else
					{
						CString sMsg;
						sMsg.Format("Could not open output file\n%s", m_sOutputFile);
						AfxMessageBox(sMsg, MB_OK);
					}
				}
				else
				{
					// some extracting
					CString sSearch = " __cdecl ";
					int iIndex = sLine.Find(sSearch);
					if (iIndex != -1)
					{
						sLine = sLine.Mid(iIndex + sSearch.GetLength());
						iIndex = sLine.Find(':');
						if (iIndex != -1)
						{
							sLine = sLine.Left(iIndex);
						}
						else
						{
							if (sLine.Left(10) == "NameOfEnum")
							{
								iIndex = sLine.Find(')');
								if (iIndex != -1)
								{
									sLine = sLine.Left(iIndex+1);
								}
							}
							else
							{
								iIndex = sLine.Find('(');
								if (iIndex != -1)
								{
									sLine = sLine.Left(iIndex);
								}
							}
						}
					}
					else
					{
						sSearch = "__thiscall ";
						iIndex = sLine.Find(sSearch);
						if (iIndex != -1)
						{
							sLine = sLine.Mid(iIndex + sSearch.GetLength());
							iIndex = sLine.Find(':');
							if (iIndex != -1)
							{
								sLine = sLine.Left(iIndex);
							}
						}
						else
						{
							sSearch = "unresolved external symbol";
							iIndex = sLine.Find(sSearch);
							if (iIndex != -1)
							{
								sSearch = "  ";
								iIndex = sLine.Find(sSearch, iIndex);
								if (iIndex != -1)
								{
									sLine = sLine.Mid(iIndex + sSearch.GetLength());
									iIndex = sLine.Find(':');
									if (iIndex != -1)
									{
										sLine = sLine.Left(iIndex);
									}
								}
								else
								{
									CString sMsg;
									sMsg.Format("OOPS, No matching search criteria\n'DOUBLE SPACE' at 'unresolved external symbol'!\n%s",
																						sLine);
									AfxMessageBox(sMsg, MB_OK);
//									sLine.Empty();
								}
							}
							else
							{
								CString sMsg;
								sMsg.Format("OOPS, No matching search criteria\n'unresolved external symbol'!\n%s",
																						sLine);
								AfxMessageBox(sMsg, MB_OK);
//								sLine.Empty();
							}
						}
					}
					sOutput.Format("%s,%s,%s\n", m_sWorkspaceName, m_sLibrary, sLine);
					if (!sLine.IsEmpty())
					{
						sLines.AddOnce(sOutput);
					}
				}
			}
			else if (   !bStarted
					 && (sLine.Find("Linking...") != -1)
					 )
			{
				bStarted = TRUE;
			}
		}
	}
	m_sStatus = _T("Ready!   Waiting for action...");
	UpdateDialog(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
// CExtractBuildLogDlg message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CExtractBuildLogDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	UpdateDialog();

	return TRUE;  // return TRUE  unless you set the focus to a control
}
/////////////////////////////////////////////////////////////////////////////
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CExtractBuildLogDlg::OnPaint() 
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
HCURSOR CExtractBuildLogDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
/////////////////////////////////////////////////////////////////////////////
void CExtractBuildLogDlg::OnRadioFileType() 
{
	UpdateDialog();
}
/////////////////////////////////////////////////////////////////////////////
void CExtractBuildLogDlg::OnChangeEditInputFile() 
{
	UpdateDialog();
	if (m_iFileType == 0)
	{
		int iIndex = m_sInputFile.Find('.');
		if (iIndex != -1)
		{
			CString sFileProject = m_sInputFile.Left(iIndex);
			sFileProject.MakeLower();
			if (sFileProject.Right(4) == "cipc")
			{
				m_sLibrary = "CIPC";
				m_cbLibrary.SelectString(0, m_sLibrary);
			}
			else if (sFileProject.Right(6) == "oemgui")
			{
				m_sLibrary = "OemGui";
				m_cbLibrary.SelectString(0, m_sLibrary);
			}
			else if (sFileProject.Right(9) == "wkclasses")
			{
				m_sLibrary = "WKClasses";
				m_cbLibrary.SelectString(0, m_sLibrary);
			}
		}
		UpdateDialog(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CExtractBuildLogDlg::OnSelchangeComboLibrary() 
{
	// Workaround, CComboBox DDX is buggy!
	// Get the selected item ourself and force to save it in member!
	int iSel = m_cbLibrary.GetCurSel();
	if (iSel != LB_ERR)
	{
		m_cbLibrary.GetLBText(iSel, m_sLibrary);
		if (m_iFileType == 0)
		{
			int iIndex = m_sInputFile.ReverseFind('\\');
			if (iIndex != -1)
			{
				CString sWorkspaceDir = m_sInputFile.Left(iIndex+1);
				m_sInputFile = sWorkspaceDir + m_sLibrary + ".txt";
			}
		}
	}
	UpdateDialog(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CExtractBuildLogDlg::OnEditchangeComboLibrary() 
{
	UpdateDialog();
}
/////////////////////////////////////////////////////////////////////////////
void CExtractBuildLogDlg::OnChangeEditOutputFile() 
{
	UpdateDialog();
}
/////////////////////////////////////////////////////////////////////////////
void CExtractBuildLogDlg::OnOK() 
{
	UpdateData();
	CStdioFile file;
	if (file.Open(m_sInputFile, CFile::modeRead))
	{
		if (m_iFileType == 0)
		{
			ExtractDocomaticList(file);
		}
		else
		{
			ExtractLinkErrors(file);
		}
		file.Close();
//		CDialog::OnOK();
	}
	else
	{
		CString sMsg;
		sMsg.Format("Could not open build log file\n%s", m_sInputFile);
		AfxMessageBox(sMsg, MB_OK);
	}

}
