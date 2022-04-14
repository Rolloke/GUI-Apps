// CommentDialog.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "DlgComment.h"
#include ".\dlgcomment.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgComment dialog


CDlgComment::CDlgComment(CWnd* pParent /*=NULL*/)
	: CSkinDialog(CDlgComment::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgComment)
	m_sComment = _T("");
	//}}AFX_DATA_INIT
	m_nInitToolTips = FALSE;
}


void CDlgComment::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgComment)
	DDX_Text(pDX, IDC_COMMENT, m_sComment);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgComment, CSkinDialog)
	//{{AFX_MSG_MAP(CDlgComment)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgComment message handlers

BOOL CDlgComment::OnInitDialog()
{
	CSkinDialog::OnInitDialog();
	AutoCreateSkinButtons();
	AutoCreateSkinEdit();

	return TRUE;
}
