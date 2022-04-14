/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ViewKebaPasador.cpp $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/SDITest/ViewKebaPasador.cpp $
// CHECKIN:		$Date: 25.08.99 16:02 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 25.08.99 14:33 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#include "stdafx.h"
#include "ViewKebaPasador.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CViewKebaPasador, CSDITestView)
//////////////////////////////////////////////////////////////////////
CViewKebaPasador::CViewKebaPasador()
	: CSDITestView(IDD)
{
	//{{AFX_DATA_INIT(CViewKebaPasador)
	m_sFileName = _T("");
	//}}AFX_DATA_INIT
	m_uTimerID = TIMER_KEBA_PASADOR;
}

CViewKebaPasador::~CViewKebaPasador()
{

}
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
CDocKebaPasador* CViewKebaPasador::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDocKebaPasador)));
	return (CDocKebaPasador*)m_pDocument;
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
void CViewKebaPasador::ShowHide()
{
	// Die Schnittstelle und Kamerafelder anzeigen,

	// Basisklasse aufrufen
	CSDITestView::ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CViewKebaPasador::InitializeData()
{

	CSDITestView::InitializeData();
}
/////////////////////////////////////////////////////////////////////////////
void CViewKebaPasador::DoDataExchange(CDataExchange* pDX)
{
	CSDITestView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewKebaPasador)
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_sFileName);
	//}}AFX_DATA_MAP

	if (m_ToolTip.GetSafeHwnd()) {
		m_ToolTip.Activate(TRUE);
	}
}
/////////////////////////////////////////////////////////////////////////////
// CViewKebaPasador diagnostics
#ifdef _DEBUG
void CViewKebaPasador::AssertValid() const
{
	CSDITestView::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CViewKebaPasador::Dump(CDumpContext& dc) const
{
	CSDITestView::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CViewKebaPasador, CSDITestView)
	//{{AFX_MSG_MAP(CViewKebaPasador)
	ON_EN_CHANGE(IDC_EDIT_FILENAME, OnChangeEditFilename)
	ON_BN_CLICKED(IDC_BTN_BROWSE, OnBtnBrowse)
	ON_EN_KILLFOCUS(IDC_EDIT_FILENAME, OnKillfocusEditFilename)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CViewKebaPasador message handlers
/////////////////////////////////////////////////////////////////////////////
void CViewKebaPasador::OnChangeEditFilename() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CSDITestView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData();
	ShowHide();
	GetDocument()->m_sFileName = m_sFileName;
}
/////////////////////////////////////////////////////////////////////////////
void CViewKebaPasador::OnBtnBrowse() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
}

void CViewKebaPasador::OnKillfocusEditFilename() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
}
