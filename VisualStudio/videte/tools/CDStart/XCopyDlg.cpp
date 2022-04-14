// XCopyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "cdstart.h"
#include "XCopyDlg.h"

#include "winbase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXCopyDlg dialog
CXCopyDlg::CXCopyDlg(CString sSource,
					 CString sDestination,
					 BOOL bRecursiv,
					 BOOL bEnableCancel,
					 CWnd* pParent /*=NULL*/)
	: CDialog(CXCopyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CXCopyDlg)
	m_sSourceFile = _T("");
	m_sDestinationFile = _T("");
	m_sTo = _T("");
	//}}AFX_DATA_INIT
	m_sSource = sSource;
	m_sDestination = sDestination;
	m_bRecursiv = bRecursiv;
	m_bEnableCancel = FALSE;	// bEnableCancel; not yet implemented
	m_bIsCanceled = FALSE;
	m_liTotalBytes.QuadPart = 0;
	m_liTotalBytesCopied.QuadPart = 0;
	m_dwFileBytes = 0;
	m_dwFileBytesCopied = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CXCopyDlg::Run()
{
	// Initializing
	Initializing();

	// now start copying
	CopyAllFiles();

	// Ready
	m_sSourceFile.LoadString(IDS_READY);
	m_sTo = _T("");
	m_sDestinationFile = _T("");
	m_progressFile.SetPos(0);
	m_progressFile.SetWindowText(_T(" "));
	UpdateData(FALSE);

	// close dlg when ready
	CDialog::OnCancel();
}
/////////////////////////////////////////////////////////////////////////////
void CXCopyDlg::Initializing()
{
	// Collect all files
	CString sPattern;
	WK_SplitPath(m_sDestination, m_sDestinationPath, sPattern);
	WK_SplitPath(m_sSource, m_sSourcePath, sPattern);
	if (sPattern.IsEmpty())
	{
		sPattern = _T("*.*");
	}
	int iReturn;
	if (m_bRecursiv)
	{
		// m_arrayFiles will contain all files WITH complete path
		iReturn = WK_SearchFilesRecursiv(m_arrayFiles, m_sSourcePath, sPattern);
	}
	else
	{
		// m_arrayFiles will contain all files WITHOUT path
		iReturn = WK_SearchFiles(m_arrayFiles, m_sSourcePath, sPattern);
		// but all files should be stored WITH complete path
		CString sFile;
		for (int i=0 ; i< m_arrayFiles.GetSize() ; i++)
		{
			sFile = m_arrayFiles.GetAt(i);
			sFile = m_sSourcePath + sFile;
			m_arrayFiles.SetAt(i, sFile);
		}
	}
	TRACE(_T("%i file(s) found\n"), iReturn);

	// Count total bytes to copy
	CountTotalBytesToCopy();

	m_sSourceFile = _T("");
	m_txtTo.ShowWindow(SW_SHOW);
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CXCopyDlg::CountTotalBytesToCopy()
{
	CString sFile;
	CFile file;
	CFileStatus fileStatus;
	for(int i=0 ; i<m_arrayFiles.GetSize() ; i++)
	{
		sFile = m_arrayFiles.GetAt(i);
		if (file.GetStatus(sFile, fileStatus))
		{
			m_liTotalBytes.QuadPart += fileStatus.m_size;
		}
		else
		{
			WK_TRACE_ERROR(_T("file not found %s\n"), sFile);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CXCopyDlg::CopyAllFiles()
{
	m_Animation.Open(IDR_AVI_FILECOPY);
	m_Animation.Play(0, (UINT)-1, (UINT)-1);
	m_sTo.LoadString(IDS_TO);

	CString sSource, sDest;
	for(int i=0 ; i<m_arrayFiles.GetSize() ; i++)
	{
		WK_STAT_PEAK(_T("CopyAllFiles"), 1, _T("Array"));
		sSource = m_arrayFiles.GetAt(i);
		WK_STAT_PEAK(_T("CopyAllFiles"), 1, _T("Find"));
		if (-1 != sSource.Find(m_sSourcePath))
		{
			WK_STAT_PEAK(_T("CopyAllFiles"), 1, _T("Destination"));
			sDest = m_sDestinationPath + sSource.Mid(m_sSourcePath.GetLength());
			WK_STAT_PEAK(_T("CopyAllFiles"), 1, _T("Copy start"));
			CopyFile(sSource, sDest);
			WK_STAT_PEAK(_T("CopyAllFiles"), 1, _T("Copy end"));
		}
		else
		{
			WK_TRACE_ERROR(_T("SourcePath not found, ignore file %s\n"), sSource);
		}

	}
	m_Animation.Stop();
}
/////////////////////////////////////////////////////////////////////////////
void CXCopyDlg::CopyFile(CString sSource, CString sDest)
{
	WK_STAT_PEAK(_T("CopyFile"), 1, _T("Dlg"));
	CWK_StopWatch watch;

	// set some dlg controls
	m_sSourceFile = sSource;
	m_sDestinationFile = sDest;
	m_progressFile.SetPos(0);
	UpdateData(FALSE);

	BOOL bCopied = FALSE;

	WK_STAT_PEAK(_T("CopyFile"), 1, _T("Dir"));
	// create dir , it could not exist
	CString sDestDir, sDestFile;
	WK_SplitPath(sDest, sDestDir, sDestFile);
	// remove last slash
	WK_CreateDirectory(sDestDir.Left(sDestDir.GetLength()-1));

	// open source file
	WK_STAT_PEAK(_T("CopyFile"), 1, _T("Start"));
	CFileException eFileError;
	CFile fileSource;
	if (fileSource.Open(sSource, CFile::modeRead|CFile::shareDenyWrite, &eFileError))
	{
		m_dwFileBytes = (DWORD)fileSource.GetLength();
		// destination: reset readonly attribute and open file
		CFile fileDest;
		CFileStatus fileStatus;
		if (CFile::GetStatus(sDest, fileStatus))
		{
			fileStatus.m_attribute &= ~CFile::readOnly;
			CFile::SetStatus(sDest, fileStatus);
		}
		if (fileDest.Open(sDest, CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive, &eFileError))
		{
			DWORD dwBufferSize = 100000;
			BYTE* pData = new BYTE[dwBufferSize];
			if (pData)
			{
				TRY
				{
					m_dwFileBytesCopied = 0;
					DWORD dwRead = 0;
					DWORD dwToRead = m_dwFileBytes;
					while (m_dwFileBytes != m_dwFileBytesCopied)
					{
						dwRead = fileSource.Read(pData, min(dwToRead, dwBufferSize));
						fileDest.Write(pData, dwRead);
						dwToRead -= dwRead;
						m_dwFileBytesCopied += dwRead;
						m_liTotalBytesCopied.QuadPart += dwRead;
						UpdateDlg();
					}
					bCopied = TRUE;
					CString sMsg;
					sMsg.Format(_T("copy file %u"), m_dwFileBytes);
					watch.StopWatch(sMsg);
				}
				CATCH(CFileException, e)
				{
					WK_TRACE_ERROR(_T("copy file failed: %s\n"), GetFileException(e->m_cause));
				}
				END_CATCH
			}
			else
			{
				WK_TRACE_ERROR(_T("memory alloc of %lu bytes failed\n"), dwBufferSize);
			}
			fileDest.Close();
			WK_DELETE(pData);
		}
		else
		{
			WK_TRACE_ERROR(_T("open dest file failed: %s\n"), GetFileException(eFileError.m_cause));
		}
		fileSource.Close();
	}
	else
	{
		WK_TRACE_ERROR(_T("open source file failed: %s\n"), GetFileException(eFileError.m_cause));
	}

	if (bCopied)
	{
//		TRACE(_T("%s copied\n"), sSource);
	}
	else
	{
		WK_TRACE_ERROR(_T("%s not copied\n"), sSource);
		CString sMsg;
		sMsg.Format(IDS_COPY_FILE_FAILED, sSource, sDest);
		AfxMessageBox(sMsg);
	}
	WK_STAT_PEAK(_T("CopyFile"), 1, _T("End"));
}
/////////////////////////////////////////////////////////////////////////////
void CXCopyDlg::UpdateDlg()
{
	int iPercentage = m_dwFileBytesCopied * 100 / m_dwFileBytes;
	m_progressFile.SetPos(iPercentage);
	iPercentage = (int)(m_liTotalBytesCopied.QuadPart * 100 / m_liTotalBytes.QuadPart);
	m_progressTotal.SetPos(iPercentage);
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
LPCTSTR CXCopyDlg::GetFileException(int iCause)
{
	switch (iCause)
	{
		case CFileException::none:				return _T("no error");				break;
		case CFileException::generic:			return _T("generic, unspecified");	break;
		case CFileException::fileNotFound:		return _T("fileNotFound");			break;
		case CFileException::badPath:			return _T("badPath");				break;
		case CFileException::tooManyOpenFiles:	return _T("tooManyOpenFiles");		break;
		case CFileException::accessDenied:		return _T("accessDenied");			break;
		case CFileException::invalidFile:		return _T("invalidFile");			break;
		case CFileException::removeCurrentDir:	return _T("removeCurrentDir");		break;
		case CFileException::directoryFull:		return _T("directoryFull");			break;
		case CFileException::badSeek:			return _T("badSeek");				break;
		case CFileException::hardIO:			return _T("hardIO");				break;
		case CFileException::sharingViolation:	return _T("sharingViolation");		break;
		case CFileException::lockViolation:		return _T("lockViolation");			break;
		case CFileException::diskFull:			return _T("diskFull");				break;
		case CFileException::endOfFile:			return _T("endOfFile");				break;
		default:								return _T("cause unknown");			break;
	}
}
/////////////////////////////////////////////////////////////////////////////
/*
DWORD CXCopyDlg::CopyProgress(LARGE_INTEGER liTotalFileSize,  // total file size, in bytes
							  LARGE_INTEGER liTotalBytesTransferred, // total number of bytes transferred
							  LARGE_INTEGER liStreamSize,  // total number of bytes for this stream
							  LARGE_INTEGER liStreamBytesTransferred, // total number of bytes transferred for this stream
							  DWORD dwStreamNumber,     // the current stream
							  DWORD dwCallbackReason,   // reason for callback
							  HANDLE hSourceFile,       // handle to the source file
							  HANDLE hDestinationFile,  // handle to the destination file
							  LPVOID lpData             // passed by CopyFileEx
							  )
{
	TRACE(_T("CallbackReason %lu\n"), dwCallbackReason);
	TRACE(_T("liTotalFileSize %lu   liTotalBytesTransferred %lu\n"),
						liTotalFileSize, liTotalBytesTransferred);
	CXCopyDlg* pDlg = (CXCopyDlg*)lpData;
	pDlg->UpdateDlg(liTotalFileSize, liTotalBytesTransferred);
	DWORD dwReturn = PROGRESS_CONTINUE;
	return dwReturn;
}
*/
/////////////////////////////////////////////////////////////////////////////
void CXCopyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXCopyDlg)
	DDX_Control(pDX, IDC_ANIMATE, m_Animation);
	DDX_Control(pDX, IDC_TXT_TO, m_txtTo);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_PROGRESS_FILE, m_progressFile);
	DDX_Control(pDX, IDC_PROGRESS_TOTAL, m_progressTotal);
	DDX_Control(pDX, IDC_TXT_TOTAL, m_txtTotal);
	DDX_Control(pDX, IDC_TXT_DESTINATION, m_txtDestinationFile);
	DDX_Control(pDX, IDC_TXT_SOURCE, m_txtSourceFile);
	DDX_Text(pDX, IDC_TXT_SOURCE, m_sSourceFile);
	DDX_Text(pDX, IDC_TXT_DESTINATION, m_sDestinationFile);
	DDX_Text(pDX, IDC_TXT_TO, m_sTo);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CXCopyDlg, CDialog)
	//{{AFX_MSG_MAP(CXCopyDlg)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WKM_INITDLG_READY, OnInitDlgReady)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CXCopyDlg message handlers
BOOL CXCopyDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	// is cancelling 
	if (FALSE == m_bEnableCancel)
	{
		m_btnCancel.EnableWindow(FALSE);
		m_btnCancel.ShowWindow(SW_HIDE);
	}

	// Clear some controls
	m_sSourceFile.LoadString(IDS_INITIALIZING);
	m_sTo = _T("");
	m_sDestinationFile = _T("");
	m_progressFile.SetShowText(TRUE);
	m_progressFile.SetBkColour(GetSysColor(COLOR_BTNFACE));
	m_progressTotal.SetShowText(TRUE);
	m_progressTotal.SetBkColour(GetSysColor(COLOR_BTNFACE));
	UpdateData(FALSE);

	PostMessage(WKM_INITDLG_READY, NULL, NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CXCopyDlg::OnInitDlgReady(WPARAM wParam, LPARAM lParam)
{
	Run();
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CXCopyDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	if (m_bEnableCancel)
	{
		CDialog::OnCancel();
	}
}
