// ViewFile.cpp : implementation file
//

#include "stdafx.h"
#include "sditest.h"
#include "ViewFile.h"

#include "ComConfigurationDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewFile

IMPLEMENT_DYNCREATE(CViewFile, CSDITestView)

CViewFile::CViewFile()
	: CSDITestView(CViewFile::IDD)
{
	//{{AFX_DATA_INIT(CViewFile)
	m_sPathName = _T("");
	//}}AFX_DATA_INIT
}

CViewFile::~CViewFile()
{
}

void CViewFile::DoDataExchange(CDataExchange* pDX)
{
	CSDITestView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewFile)
	DDX_Text(pDX, IDC_FILE, m_sPathName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CViewFile, CSDITestView)
	//{{AFX_MSG_MAP(CViewFile)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_COM, OnCom)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewFile diagnostics

#ifdef _DEBUG
void CViewFile::AssertValid() const
{
	CSDITestView::AssertValid();
}

void CViewFile::Dump(CDumpContext& dc) const
{
	CSDITestView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CViewFile message handlers
void CViewFile::OnBrowse() 
{
	CString sFilter;
	sFilter.LoadString(IDS_DAT_FILTER);
	CFileDialog dlg(TRUE,_T("*.dat"),m_sPathName,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		sFilter,this);

	if (IDOK == dlg.DoModal())
	{
		m_sPathName = dlg.GetPathName();
		((CFileDoc*)GetDocument())->LoadFile(m_sPathName);
		UpdateData(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewFile::OnCom() 
{
	CComParameters com = ((CFileDoc*)GetDocument())->GetComParameters();
	CString s;

	s.Format(_T("COM%d"),GetDocument()->GetCOM());
	
	CComConfigurationDialog dlg(s,&com,this);
	if (IDOK==dlg.DoModal())
	{
		((CFileDoc*)GetDocument())->SetComParameters(com);
//		AfxMessageBox(IDS_REOPEN_COM_AFTER_COM_PARAMS_CHANGED);
		GetDocument()->ConfigCom();
	}
}
