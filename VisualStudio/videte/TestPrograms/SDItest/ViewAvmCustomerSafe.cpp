// ViewAvmCustomerSafe.cpp : implementation file
//

#include "stdafx.h"
#include "ViewAvmCustomerSafe.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewAvmCustomerSafe
IMPLEMENT_DYNCREATE(CViewAvmCustomerSafe, CSDITestView)
/////////////////////////////////////////////////////////////////////////////
CViewAvmCustomerSafe::CViewAvmCustomerSafe()
	: CSDITestView(CViewAvmCustomerSafe::IDD)
{
	//{{AFX_DATA_INIT(CViewAvmCustomerSafe)
	m_iCommand = 0;
	//}}AFX_DATA_INIT
	m_uTimerID = TIMER_AVM;
}
/////////////////////////////////////////////////////////////////////////////
CViewAvmCustomerSafe::~CViewAvmCustomerSafe()
{
}
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
CDocAvmCustomerSafe* CViewAvmCustomerSafe::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDocAvmCustomerSafe)));
	return (CDocAvmCustomerSafe*)m_pDocument;
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
void CViewAvmCustomerSafe::ShowHide()
{
	// Die Schnittstelle und Kamerafelder anzeigen,
	BOOL bEnable = !(	GetDocument()->m_bStarted)
					 && !(GetDocument()->m_bRepeat
					 && (0 != GetDocument()->m_iRepeatHow)
					 );
	m_btnAccess.EnableWindow(bEnable);
	m_btnRelease.EnableWindow(bEnable);

	// Basisklasse aufrufen
	CSDITestView::ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CViewAvmCustomerSafe::InitializeData()
{
	m_iCommand	= GetDocument()->m_iCommand;

	CSDITestView::InitializeData();
}
/////////////////////////////////////////////////////////////////////////////
void CViewAvmCustomerSafe::DoDataExchange(CDataExchange* pDX)
{
	CSDITestView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewAvmCustomerSafe)
	DDX_Control(pDX, IDC_RADIO_CHECK_ACCESS, m_btnCheckAccess);
	DDX_Control(pDX, IDC_RADIO_ACCESS, m_btnAccess);
	DDX_Control(pDX, IDC_RADIO_RELEASE, m_btnRelease);
	DDX_Radio(pDX, IDC_RADIO_CHECK_ACCESS, m_iCommand);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CViewAvmCustomerSafe, CSDITestView)
	//{{AFX_MSG_MAP(CViewAvmCustomerSafe)
	ON_BN_CLICKED(IDC_RADIO_ACCESS, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_RELEASE, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_CHECK_ACCESS, OnRadioChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CViewAvmCustomerSafe diagnostics
#ifdef _DEBUG
void CViewAvmCustomerSafe::AssertValid() const
{
	CSDITestView::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CViewAvmCustomerSafe::Dump(CDumpContext& dc) const
{
	CSDITestView::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// CViewAvmCustomerSafe message handlers
/////////////////////////////////////////////////////////////////////////////
void CViewAvmCustomerSafe::OnRadioChanged() 
{
	UpdateData();
	ShowHide();
	GetDocument()->m_iCommand = m_iCommand;
}
