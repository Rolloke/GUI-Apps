// CompareItDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CompareIt.h"
#include "CompareItDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCompareItDlg dialog

CCompareItDlg::CCompareItDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCompareItDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCompareItDlg)
	m_sFile1 = _T("");
	m_sFile2 = _T("");
	m_sProperties1 = _T("");
	m_sProperties2 = _T("");
	m_sResult = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCompareItDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCompareItDlg)
	DDX_Control(pDX, IDC_LIST_DIFF, m_Diffs);
	DDX_Text(pDX, IDC_TXT_FILE1, m_sFile1);
	DDX_Text(pDX, IDC_TXT_FILE2, m_sFile2);
	DDX_Text(pDX, IDC_TXT_FILE1_PROPERTIES, m_sProperties1);
	DDX_Text(pDX, IDC_TXT_FILE2_PROPERTIES, m_sProperties2);
	DDX_Text(pDX, IDC_TXT_RESULT, m_sResult);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCompareItDlg, CDialog)
	//{{AFX_MSG_MAP(CCompareItDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_FILE1, OnFile1)
	ON_BN_CLICKED(IDC_FILE2, OnFile2)
	ON_BN_CLICKED(IDC_COMPARE, OnCompare)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCompareItDlg message handlers

BOOL CCompareItDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCompareItDlg::OnPaint() 
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
HCURSOR CCompareItDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CCompareItDlg::OnFile1() 
{
	CFileDialog dlg(TRUE,NULL,m_sFile1,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,NULL,this);

	if (IDOK==dlg.DoModal())
	{
		m_sFile1 = dlg.GetPathName();

		CFileStatus cfs;
		CFile::GetStatus(m_sFile1,cfs);
		m_sProperties1.Format("%d Bytes",cfs.m_size);
		UpdateData(FALSE);
	}
	
}

void CCompareItDlg::OnFile2() 
{
	CFileDialog dlg(TRUE,NULL,m_sFile2,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,NULL,this);

	if (IDOK==dlg.DoModal())
	{
		m_sFile2 = dlg.GetPathName();
		CFileStatus cfs;
		CFile::GetStatus(m_sFile2,cfs);
		m_sProperties2.Format("%d Bytes",cfs.m_size);
		UpdateData(FALSE);
		OnCompare();
	}
}

void CCompareItDlg::OnCompare() 
{
	CFileStatus cfs1,cfs2;
	CFile file1,file2;
	BYTE* pBuffer1 = NULL;
	BYTE* pBuffer2 = NULL;

	m_Diffs.ResetContent();

	if (file1.Open(m_sFile1,CFile::modeRead))
	{
		file1.GetStatus(cfs1);
		pBuffer1 = new BYTE[cfs1.m_size];
		file1.Read(pBuffer1,cfs1.m_size);
		file1.Close();
	}
	if (file2.Open(m_sFile2,CFile::modeRead))
	{
		file2.GetStatus(cfs2);
		pBuffer2 = new BYTE[cfs2.m_size];
		file2.Read(pBuffer2,cfs2.m_size);
		file2.Close();
	}
	
	if (cfs2.m_size == cfs1.m_size)
	{
		int i,c,cc;
		CStringArray diffs;
		CString diff;

		c = cfs1.m_size;
		cc = 0;
		for (i=0;i<c;i++)
		{
			if (pBuffer1[i] != pBuffer2[i])
			{
				cc++;
				diff.Format("OFF %08x %02x %02x",i,(int)pBuffer1[i],(int)pBuffer2[i]);
				diffs.Add(diff);
			}
		}
		
		if (cc==0)
		{
			m_sResult = "gleich";
		}
		else
		{
			m_sResult.Format("%d x unterschiedlich",cc);

			for (i=0;i<diffs.GetSize() && i < 64;i++)
			{
				m_Diffs.AddString(diffs[i]);
			}
		}
	}
	else
	{
		m_sResult = "unterschiedlich";
	}
	if (pBuffer1)
		delete [] pBuffer1;
	if (pBuffer2)
		delete [] pBuffer2;
	UpdateData(FALSE);
}
