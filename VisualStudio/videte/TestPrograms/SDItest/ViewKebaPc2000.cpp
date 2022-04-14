/* GlobalReplace: CSDITestKebaView --> CViewKebaPc2000 */
/* GlobalReplace: CSDITestKebaDoc --> CDocKebaPc2000 */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ViewKebaPc2000.cpp $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/SDITest/ViewKebaPc2000.cpp $
// CHECKIN:		$Date: 22.04.99 9:17 $
// VERSION:		$Revision: 10 $
// LAST CHANGE:	$Modtime: 22.04.99 9:13 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#include "stdafx.h"

#include "ViewKebaPc2000.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewKebaPc2000
IMPLEMENT_DYNCREATE(CViewKebaPc2000, CSDITestView)
/////////////////////////////////////////////////////////////////////////////
CViewKebaPc2000::CViewKebaPc2000()
	: CSDITestView(CViewKebaPc2000::IDD)
{
	//{{AFX_DATA_INIT(CViewKebaPc2000)
	m_iCardReader = 0;
	//}}AFX_DATA_INIT
	m_uTimerID = TIMER_KEBA;
}
/////////////////////////////////////////////////////////////////////////////
CViewKebaPc2000::~CViewKebaPc2000()
{
}
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
CDocKebaPc2000* CViewKebaPc2000::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDocKebaPc2000)));
	return (CDocKebaPc2000*)m_pDocument;
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
void CViewKebaPc2000::ShowHide()
{
	// Die Schnittstelle und Kamerafelder anzeigen,
	BOOL bEnable = !(GetDocument()->m_bStarted) && !(GetDocument()->m_bRepeat && (0 != GetDocument()->m_iRepeatHow));
	m_btnCardReader1.EnableWindow(bEnable);
	m_btnCardReader2.EnableWindow(bEnable);

	// Basisklasse aufrufen
	CSDITestView::ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CViewKebaPc2000::InitializeData()
{
	m_iCardReader	= GetDocument()->m_iCardReader;

	CSDITestView::InitializeData();
}
/////////////////////////////////////////////////////////////////////////////
void CViewKebaPc2000::DoDataExchange(CDataExchange* pDX)
{
	CSDITestView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewKebaPc2000)
	DDX_Control(pDX, IDC_RADIO_CARD_READER_1, m_btnCardReader1);
	DDX_Control(pDX, IDC_RADIO_CARD_READER_2, m_btnCardReader2);
	DDX_Radio(pDX, IDC_RADIO_CARD_READER_1, m_iCardReader);
	//}}AFX_DATA_MAP

	if (m_ToolTip.GetSafeHwnd()) {
		m_ToolTip.Activate(TRUE);
	}
}
/////////////////////////////////////////////////////////////////////////////
// CViewKebaPc2000 diagnostics
#ifdef _DEBUG
void CViewKebaPc2000::AssertValid() const
{
	CSDITestView::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CViewKebaPc2000::Dump(CDumpContext& dc) const
{
	CSDITestView::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CViewKebaPc2000, CSDITestView)
	//{{AFX_MSG_MAP(CViewKebaPc2000)
	ON_BN_CLICKED(IDC_RADIO_CARD_READER_1, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_CARD_READER_2, OnRadioChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CViewKebaPc2000 message handlers

void CViewKebaPc2000::OnRadioChanged() 
{
	UpdateData();
	ShowHide();
	GetDocument()->m_iCardReader = m_iCardReader;
}
