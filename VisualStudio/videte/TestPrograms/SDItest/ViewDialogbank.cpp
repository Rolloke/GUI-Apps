// ViewDialogbank.cpp : implementation file
//

#include "stdafx.h"
#include "sditest.h"
#include "ViewDialogbank.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewDialogbank
IMPLEMENT_DYNCREATE(CViewDialogbank, CSDITestView)
/////////////////////////////////////////////////////////////////////////////
CViewDialogbank::CViewDialogbank()
	: CSDITestView(CViewDialogbank::IDD)
{
	//{{AFX_DATA_INIT(CViewDialogbank)
	m_iCommand = 0;
	//}}AFX_DATA_INIT
	m_uTimerID = TIMER_DIALOGBANK;
}
/////////////////////////////////////////////////////////////////////////////
CViewDialogbank::~CViewDialogbank()
{
}
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
CDocDialogbank* CViewDialogbank::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDocDialogbank)));
	return (CDocDialogbank*)m_pDocument;
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
void CViewDialogbank::ShowHide()
{
	// Die Schnittstelle und Kamerafelder anzeigen,
	BOOL bEnable = !(	GetDocument()->m_bStarted)
					 && !(GetDocument()->m_bRepeat
					 && (0 != GetDocument()->m_iRepeatHow)
					 );
	m_btnECCard.EnableWindow(bEnable);
	m_btnTANCard.EnableWindow(bEnable);

	// Basisklasse aufrufen
	CSDITestView::ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CViewDialogbank::InitializeData()
{
	m_iCommand	= GetDocument()->m_iCommand;

	CSDITestView::InitializeData();
}
/////////////////////////////////////////////////////////////////////////////
void CViewDialogbank::DoDataExchange(CDataExchange* pDX)
{
	CSDITestView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewDialogbank)
	DDX_Control(pDX, IDC_RADIO_EC_CARD, m_btnECCard);
	DDX_Control(pDX, IDC_RADIO_TAN_CARD, m_btnTANCard);
	DDX_Radio(pDX, IDC_RADIO_EC_CARD, m_iCommand);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CViewDialogbank, CSDITestView)
	//{{AFX_MSG_MAP(CViewDialogbank)
	ON_BN_CLICKED(IDC_RADIO_EC_CARD, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_TAN_CARD, OnRadioChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CViewDialogbank diagnostics
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CViewDialogbank::AssertValid() const
{
	CSDITestView::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CViewDialogbank::Dump(CDumpContext& dc) const
{
	CSDITestView::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// CViewDialogbank message handlers
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CViewDialogbank::OnRadioChanged() 
{
	UpdateData();
	ShowHide();
	GetDocument()->m_iCommand = m_iCommand;
}
