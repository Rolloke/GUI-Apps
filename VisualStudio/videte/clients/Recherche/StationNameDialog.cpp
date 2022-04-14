// StationNameDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Recherche.h"
#include "StationNameDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStationNameDialog dialog


CStationNameDialog::CStationNameDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CStationNameDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStationNameDialog)
	m_sStationName = _T("");
	//}}AFX_DATA_INIT
}


void CStationNameDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStationNameDialog)
	DDX_Text(pDX, IDC_EDIT_NAME, m_sStationName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStationNameDialog, CDialog)
	//{{AFX_MSG_MAP(CStationNameDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStationNameDialog message handlers
