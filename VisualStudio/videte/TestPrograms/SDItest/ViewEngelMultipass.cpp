/* GlobalReplace: CSDITestEngelView --> CViewEngelMultipass */
/* GlobalReplace: CSDITestEngelDoc --> CDocEngelMultipass */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ViewEngelMultipass.cpp $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/SDITest/ViewEngelMultipass.cpp $
// CHECKIN:		$Date: 22.04.99 9:17 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 22.04.99 9:06 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#include "stdafx.h"
#include "sditest.h"
#include "ViewEngelMultipass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewEngelMultipass
IMPLEMENT_DYNCREATE(CViewEngelMultipass, CSDITestView)
/////////////////////////////////////////////////////////////////////////////
CViewEngelMultipass::CViewEngelMultipass()
	: CSDITestView(CViewEngelMultipass::IDD)
{
	//{{AFX_DATA_INIT(CViewEngelMultipass)
	m_iCommand = -1;
	//}}AFX_DATA_INIT
	m_uTimerID = TIMER_ENGEL;
}
/////////////////////////////////////////////////////////////////////////////
CViewEngelMultipass::~CViewEngelMultipass()
{
}
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
CDocEngelMultipass* CViewEngelMultipass::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDocEngelMultipass)));
	return (CDocEngelMultipass*)m_pDocument;
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
void CViewEngelMultipass::ShowHide()
{
	// Die Schnittstelle und Kamerafelder anzeigen,
	BOOL bEnable = !(GetDocument()->m_bStarted) && !(GetDocument()->m_bRepeat && (0 != GetDocument()->m_iRepeatHow));
	m_btnCommandL.EnableWindow(bEnable);
	m_btnCommandE.EnableWindow(bEnable);

	// Basisklasse aufrufen
	CSDITestView::ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CViewEngelMultipass::InitializeData()
{
	m_iCommand	= GetDocument()->m_iCommand;

	CSDITestView::InitializeData();
}
/////////////////////////////////////////////////////////////////////////////
void CViewEngelMultipass::DoDataExchange(CDataExchange* pDX)
{
	CSDITestView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewEngelMultipass)
	DDX_Control(pDX, IDC_RADIO_L_COMMAND, m_btnCommandL);
	DDX_Control(pDX, IDC_RADIO_E_COMMAND, m_btnCommandE);
	DDX_Radio(pDX, IDC_RADIO_L_COMMAND, m_iCommand);
	//}}AFX_DATA_MAP

	if (m_ToolTip.GetSafeHwnd()) {
		m_ToolTip.Activate(TRUE);
	}
}
/////////////////////////////////////////////////////////////////////////////
// CViewEngelMultipass diagnostics
#ifdef _DEBUG
void CViewEngelMultipass::AssertValid() const
{
	CSDITestView::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CViewEngelMultipass::Dump(CDumpContext& dc) const
{
	CSDITestView::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CViewEngelMultipass, CSDITestView)
	//{{AFX_MSG_MAP(CViewEngelMultipass)
	ON_BN_CLICKED(IDC_RADIO_E_COMMAND, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_L_COMMAND, OnRadioChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CViewEngelMultipass message handlers
/////////////////////////////////////////////////////////////////////////////
void CViewEngelMultipass::OnRadioChanged() 
{
	UpdateData();
	ShowHide();
	GetDocument()->m_iCommand = m_iCommand;
}
