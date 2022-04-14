// CommentDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Recherche.h"
#include "CommentDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCommentDialog dialog


CCommentDialog::CCommentDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CCommentDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCommentDialog)
	m_sComment = _T("");
	//}}AFX_DATA_INIT
}


void CCommentDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCommentDialog)
	DDX_Text(pDX, IDC_COMMENT, m_sComment);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCommentDialog, CDialog)
	//{{AFX_MSG_MAP(CCommentDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCommentDialog message handlers
