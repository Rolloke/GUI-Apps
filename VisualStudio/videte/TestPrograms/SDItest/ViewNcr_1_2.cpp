/* GlobalReplace: CSDITestNcrNewView --> CViewNcr_1_2 */
/* GlobalReplace: CSDITestKebaView --> CViewKebaPc2000 */
/* GlobalReplace: CSDITestNcrNewDoc --> CDocNcr_1_2 */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ViewNcr_1_2.cpp $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/SDITest/ViewNcr_1_2.cpp $
// CHECKIN:		$Date: 22.04.99 9:17 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 22.04.99 9:14 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#include "stdafx.h"

#include "ViewNcr_1_2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CViewNcr_1_2, CSDITestView)
/////////////////////////////////////////////////////////////////////////////
CViewNcr_1_2::CViewNcr_1_2()
	: CSDITestView(CViewNcr_1_2::IDD)
{
	//{{AFX_DATA_INIT(CViewNcr_1_2)
	m_iCommand = 0;
	m_iVariant = 0;
	//}}AFX_DATA_INIT
	m_uTimerID = TIMER_NCR;
}
/////////////////////////////////////////////////////////////////////////////
CViewNcr_1_2::~CViewNcr_1_2()
{
}
/////////////////////////////////////////////////////////////////////////////
void CViewNcr_1_2::ShowHide()
{
	// Die Schnittstelle und Kamerafelder anzeigen,
	BOOL bEnable = !(GetDocument()->m_bStarted)
					&& !(GetDocument()->m_bRepeat
					&& (0 != GetDocument()->m_iRepeatHow));
	m_btnStartCommand.EnableWindow(bEnable);
	m_btnDiagnose.EnableWindow(bEnable);

	m_btnVariant1.EnableWindow(bEnable);
	m_btnVariant2.EnableWindow(bEnable);
	m_btnVariant3.EnableWindow(bEnable);

	// Die TAN-Buttons anzeigen
	bEnable = !(GetDocument()->m_bStarted);

	// Basisklasse aufrufen
	CSDITestView::ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CViewNcr_1_2::InitializeData()
{
	m_iCommand	= GetDocument()->m_iCommand;

	CSDITestView::InitializeData();
}
/////////////////////////////////////////////////////////////////////////////
void CViewNcr_1_2::DoDataExchange(CDataExchange* pDX)
{
	CSDITestView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewNcr_1_2)
	DDX_Control(pDX, IDC_RADIO_VARIANT_1, m_btnVariant1);
	DDX_Control(pDX, IDC_RADIO_VARIANT_2, m_btnVariant2);
	DDX_Control(pDX, IDC_RADIO_VARIANT_3, m_btnVariant3);
	DDX_Control(pDX, IDC_RADIO_START_COMMAND, m_btnStartCommand);
	DDX_Control(pDX, IDC_RADIO_DIAGNOSE, m_btnDiagnose);
	DDX_Radio(pDX, IDC_RADIO_START_COMMAND, m_iCommand);
	DDX_Radio(pDX, IDC_RADIO_VARIANT_1, m_iVariant);
	//}}AFX_DATA_MAP

	if (m_ToolTip.GetSafeHwnd()) {
		m_ToolTip.Activate(TRUE);
	}
}
/////////////////////////////////////////////////////////////////////////////
// CViewNcr_1_2 diagnostics
#ifdef _DEBUG
void CViewNcr_1_2::AssertValid() const
{
	CSDITestView::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CViewNcr_1_2::Dump(CDumpContext& dc) const
{
	CSDITestView::Dump(dc);
}
/////////////////////////////////////////////////////////////////////////////
CDocNcr_1_2* CViewNcr_1_2::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDocNcr_1_2)));
	return (CDocNcr_1_2*)m_pDocument;
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CViewNcr_1_2, CSDITestView)
	//{{AFX_MSG_MAP(CViewNcr_1_2)
	ON_BN_CLICKED(IDC_RADIO_DIAGNOSE, OnRadioCommandChanged)
	ON_BN_CLICKED(IDC_RADIO_START_COMMAND, OnRadioCommandChanged)
	ON_BN_CLICKED(IDC_RADIO_VARIANT_1, OnRadioVariantChanged)
	ON_BN_CLICKED(IDC_RADIO_VARIANT_2, OnRadioVariantChanged)
	ON_BN_CLICKED(IDC_RADIO_VARIANT_3, OnRadioVariantChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CViewNcr_1_2 message handlers
void CViewNcr_1_2::OnRadioCommandChanged() 
{
	UpdateData();
	ShowHide();
	GetDocument()->m_iCommand = m_iCommand;
}
/////////////////////////////////////////////////////////////////////////////
void CViewNcr_1_2::OnRadioVariantChanged() 
{
	UpdateData();
	ShowHide();
	GetDocument()->m_iVariant = m_iVariant;
}
