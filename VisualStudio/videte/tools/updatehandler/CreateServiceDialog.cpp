// CreateServiceDialog.cpp : implementation file
//

#include "stdafx.h"
#include "updatehandler.h"
#include "CreateServiceDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCreateServiceDialog dialog


CCreateServiceDialog::CCreateServiceDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateServiceDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCreateServiceDialog)
	m_sTitel = _T("");
	m_iKind = 0;
	m_sVersion = _T("");
	//}}AFX_DATA_INIT
}
/////////////////////////////////////////////////////////////////////////////
void CCreateServiceDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreateServiceDialog)
	DDX_Control(pDX, IDC_FILES, m_lbFiles);
	DDX_Text(pDX, IDC_EDIT_SERVICE, m_sTitel);
	DDX_Radio(pDX, IDC_RADIO_UPDATE, m_iKind);
	DDX_Text(pDX, IDC_EDIT_VERSION, m_sVersion);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCreateServiceDialog, CDialog)
	//{{AFX_MSG_MAP(CCreateServiceDialog)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreateServiceDialog message handlers

void CCreateServiceDialog::OnOK() 
{
	// TODO: Add extra validation here
	int i,c;
	CString sOne;

	c = m_lbFiles.GetCount();

	for (i=0;i<c;i++)
	{
		m_lbFiles.GetText(i,sOne);
		m_Files.Add(sOne);
	}
	
	CDialog::OnOK();
}
/////////////////////////////////////////////////////////////////////////////
static TCHAR BASED_CODE szFilter[] = _T("All Files (*.*)|*.*||");
/////////////////////////////////////////////////////////////////////////////
void CCreateServiceDialog::OnAdd() 
{
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,szFilter);
	dlg.m_ofn.lpstrInitialDir = m_InitialDir.GetBuffer(0);
	int res = dlg.DoModal();
	if (res==IDOK)
	{
		CString sTemp = dlg.GetPathName();
		m_lbFiles.AddString(sTemp);
	}
	m_InitialDir.ReleaseBuffer();
}
/////////////////////////////////////////////////////////////////////////////
void CCreateServiceDialog::OnDelete() 
{
	int i = m_lbFiles.GetCurSel();

	if (i!=LB_ERR)
	{
		m_lbFiles.DeleteString(i);
	}
	
}
/////////////////////////////////////////////////////////////////////////////
//DEL HBRUSH CCreateServiceDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
//DEL {
//DEL 	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
//DEL 	
//DEL 	// TODO: Change any attributes of the DC here
//DEL 	pDC->SetBkMode(TRANSPARENT);
//DEL 	pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
//DEL 	
//DEL 	// TODO: Return a different brush if the default is not desired
//DEL //	return hbr;
//DEL 	return m_EmptyBrush;
//DEL }
/////////////////////////////////////////////////////////////////////////////
BOOL CCreateServiceDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_EmptyBrush.CreateStockObject(NULL_BRUSH);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
//DEL BOOL CCreateServiceDialog::OnEraseBkgnd(CDC* pDC) 
//DEL {
//DEL #if 1
//DEL 	// TODO: Add your message handler code here and/or call default
//DEL 	CDC memdc;
//DEL 	memdc.CreateCompatibleDC(pDC);
//DEL 	CBitmap* pOldBitmap = memdc.SelectObject(&m_EyeBitmap);
//DEL 	CRect rc;
//DEL 	GetClientRect(&rc);
//DEL 	CSize sz;
//DEL 	sz.cx = 133;
//DEL 	sz.cy = 102;
//DEL 	for (int y=0; y < rc.Height(); y += sz.cy) { // for each row:
//DEL 		for (int x=0; x < rc.Width(); x += sz.cx) { // for each column:
//DEL 			pDC->BitBlt(x, y, sz.cx, sz.cy, &memdc, 0, 0, SRCCOPY); // copy
//DEL 		}
//DEL 	}
//DEL 	memdc.SelectObject(pOldBitmap);
//DEL 
//DEL 	return TRUE;
//DEL #else	
//DEL 	return CDialog::OnEraseBkgnd(pDC);
//DEL #endif
//DEL }
