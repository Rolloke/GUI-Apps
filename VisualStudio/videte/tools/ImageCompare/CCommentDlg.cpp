// CCommentDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ImageCompare.h"
#include "CCommentDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComment dialog
CComment::CComment(CWnd* pParent /*=NULL*/)
	: CDialog(CComment::IDD, pParent)
{
	//{{AFX_DATA_INIT(CComment)
	m_sComment = _T("");
	//}}AFX_DATA_INIT
}

/////////////////////////////////////////////////////////////////////////////
void CComment::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CComment)
	DDX_Control(pDX, IDC_EDIT_COMMENT, m_ctrlComment);
	DDX_Text(pDX, IDC_EDIT_COMMENT, m_sComment);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CComment, CDialog)
	//{{AFX_MSG_MAP(CComment)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComment message handlers
CString CComment::GetComment()
{
	return m_sComment;
}

/////////////////////////////////////////////////////////////////////////////
void CComment::OnOK() 
{
	UpdateData(TRUE);
	
	CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CComment::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_ctrlComment.SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
