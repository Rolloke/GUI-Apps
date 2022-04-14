// ViewRondo.cpp : implementation file
//

#include "stdafx.h"
#include "sditest.h"
#include "ViewRondo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewRondo
IMPLEMENT_DYNCREATE(CViewRondo, CSDITestView)
/////////////////////////////////////////////////////////////////////////////
CViewRondo::CViewRondo()
	: CSDITestView(CViewRondo::IDD)
{
	//{{AFX_DATA_INIT(CViewRondo)
	m_iCommand = 0;
	//}}AFX_DATA_INIT
	m_uTimerID = TIMER_RONDO;
}
/////////////////////////////////////////////////////////////////////////////
CViewRondo::~CViewRondo()
{
}
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
CDocRondo* CViewRondo::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDocRondo)));
	return (CDocRondo*)m_pDocument;
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
void CViewRondo::ShowHide()
{
	// Die Schnittstelle und Kamerafelder anzeigen,
	BOOL bEnable = !(	GetDocument()->m_bStarted)
					 && !(GetDocument()->m_bRepeat
					 && (0 != GetDocument()->m_iRepeatHow)
					 );
	m_btnStatusRequest.EnableWindow(bEnable);
	m_btnQuiet.EnableWindow(bEnable);
	m_btnPortrait.EnableWindow(bEnable);
	m_btnMoney.EnableWindow(bEnable);

	// Basisklasse aufrufen
	CSDITestView::ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CViewRondo::InitializeData()
{
	m_iCommand	= GetDocument()->m_iCommand;

	CSDITestView::InitializeData();
}
/////////////////////////////////////////////////////////////////////////////
void CViewRondo::DoDataExchange(CDataExchange* pDX)
{
	CSDITestView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewRondo)
	DDX_Control(pDX, IDC_RADIO_STATUS_REQUEST, m_btnStatusRequest);
	DDX_Control(pDX, IDC_RADIO_QUIET, m_btnQuiet);
	DDX_Control(pDX, IDC_RADIO_PORTRAIT, m_btnPortrait);
	DDX_Control(pDX, IDC_RADIO_MONEY, m_btnMoney);
	DDX_Radio(pDX, IDC_RADIO_STATUS_REQUEST, m_iCommand);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CViewRondo, CSDITestView)
	//{{AFX_MSG_MAP(CViewRondo)
	ON_BN_CLICKED(IDC_RADIO_STATUS_REQUEST, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_QUIET, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_PORTRAIT, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_MONEY, OnRadioChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CViewRondo diagnostics
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CViewRondo::AssertValid() const
{
	CSDITestView::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CViewRondo::Dump(CDumpContext& dc) const
{
	CSDITestView::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// CViewRondo message handlers
/////////////////////////////////////////////////////////////////////////////
void CViewRondo::OnRadioChanged() 
{
	UpdateData();
	ShowHide();
	GetDocument()->m_iCommand = m_iCommand;
}
