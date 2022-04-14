/* GlobalReplace: CSDITestNcrView --> CViewNcr_1_1 */
/* GlobalReplace: CSDITestKebaView --> CViewKebaPc2000 */
/* GlobalReplace: CSDITestNcrDoc --> CDocNcr_1_1 */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ViewNcr_1_1.cpp $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/SDITest/ViewNcr_1_1.cpp $
// CHECKIN:		$Date: 22.04.99 9:17 $
// VERSION:		$Revision: 11 $
// LAST CHANGE:	$Modtime: 22.04.99 9:14 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#include "stdafx.h"

#include "ViewNcr_1_1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CViewNcr_1_1, CSDITestView)
/////////////////////////////////////////////////////////////////////////////
CViewNcr_1_1::CViewNcr_1_1()
	: CSDITestView(CViewNcr_1_1::IDD)
{
	//{{AFX_DATA_INIT(CViewNcr_1_1)
	m_iCommand = 0;
	//}}AFX_DATA_INIT
	m_uTimerID = TIMER_NCR;
}
/////////////////////////////////////////////////////////////////////////////
CViewNcr_1_1::~CViewNcr_1_1()
{
}
/////////////////////////////////////////////////////////////////////////////
void CViewNcr_1_1::ShowHide()
{
	// Die Schnittstelle und Kamerafelder anzeigen,
	BOOL bEnable = !(GetDocument()->m_bStarted)
					&& !(GetDocument()->m_bRepeat
					&& (0 != GetDocument()->m_iRepeatHow));
	m_btnStartCommand.EnableWindow(bEnable);
	m_btnDiagnose.EnableWindow(bEnable);

	// Die TAN-Buttons anzeigen
	bEnable = !(GetDocument()->m_bStarted);

	// Basisklasse aufrufen
	CSDITestView::ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CViewNcr_1_1::InitializeData()
{
	m_iCommand	= GetDocument()->m_iCommand;

	CSDITestView::InitializeData();
}
/////////////////////////////////////////////////////////////////////////////
void CViewNcr_1_1::DoDataExchange(CDataExchange* pDX)
{
	CSDITestView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewNcr_1_1)
	DDX_Control(pDX, IDC_RADIO_START_COMMAND, m_btnStartCommand);
	DDX_Control(pDX, IDC_RADIO_DIAGNOSE, m_btnDiagnose);
	DDX_Radio(pDX, IDC_RADIO_START_COMMAND, m_iCommand);
	//}}AFX_DATA_MAP

	if (m_ToolTip.GetSafeHwnd()) {
		m_ToolTip.Activate(TRUE);
	}
}
/////////////////////////////////////////////////////////////////////////////
// CViewNcr_1_1 diagnostics
#ifdef _DEBUG
void CViewNcr_1_1::AssertValid() const
{
	CSDITestView::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CViewNcr_1_1::Dump(CDumpContext& dc) const
{
	CSDITestView::Dump(dc);
}
/////////////////////////////////////////////////////////////////////////////
CDocNcr_1_1* CViewNcr_1_1::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDocNcr_1_1)));
	return (CDocNcr_1_1*)m_pDocument;
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CViewNcr_1_1, CSDITestView)
	//{{AFX_MSG_MAP(CViewNcr_1_1)
	ON_BN_CLICKED(IDC_RADIO_DIAGNOSE, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_START_COMMAND, OnRadioChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CViewNcr_1_1 message handlers
void CViewNcr_1_1::OnRadioChanged() 
{
	UpdateData();
	ShowHide();
	GetDocument()->m_iCommand = m_iCommand;
}
