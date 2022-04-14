/* GlobalReplace: CSDITestSniView --> CViewSniDos */
/* GlobalReplace: CSDITestSniDoc --> CDocSniDos */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ViewSniDos.cpp $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/SDITest/ViewSniDos.cpp $
// CHECKIN:		$Date: 22.04.99 9:17 $
// VERSION:		$Revision: 10 $
// LAST CHANGE:	$Modtime: 22.04.99 9:15 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$


#include "stdafx.h"
#include "ViewSniDos.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewSniDos
IMPLEMENT_DYNCREATE(CViewSniDos, CSDITestView)
/////////////////////////////////////////////////////////////////////////////
CViewSniDos::CViewSniDos()
	: CSDITestView(CViewSniDos::IDD)
{
	//{{AFX_DATA_INIT(CViewSniDos)
	m_iPointOfData = 0;
	//}}AFX_DATA_INIT
	m_uTimerID = TIMER_SNI;
}
/////////////////////////////////////////////////////////////////////////////
CViewSniDos::~CViewSniDos()
{
}
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
CDocSniDos* CViewSniDos::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDocSniDos)));
	return (CDocSniDos*)m_pDocument;
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
void CViewSniDos::ShowHide()
{
	// Die Schnittstelle und Kamerafelder anzeigen,
	BOOL bEnable = !(GetDocument()->m_bStarted)
					&& !(GetDocument()->m_bRepeat
					&& (0 != GetDocument()->m_iRepeatHow));
	m_btnPointOfData0.EnableWindow(bEnable);
	m_btnPointOfData1.EnableWindow(bEnable);
	m_btnPointOfData2.EnableWindow(bEnable);
	m_btnPointOfData3.EnableWindow(bEnable);
	m_btnPointOfData4.EnableWindow(bEnable);
	m_btnPointOfData5.EnableWindow(bEnable);
	m_btnPointOfData6.EnableWindow(bEnable);
	m_btnPointOfData7.EnableWindow(bEnable);
	m_btnPointOfData8.EnableWindow(bEnable);
	m_btnPointOfData9.EnableWindow(bEnable);

	// Die Betrag-Buttons anzeigen
	bEnable = !(GetDocument()->m_bStarted);

	// Basisklasse aufrufen
	CSDITestView::ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CViewSniDos::InitializeData()
{
	m_iPointOfData	= GetDocument()->m_iPointOfData;

	CSDITestView::InitializeData();
}
/////////////////////////////////////////////////////////////////////////////
void CViewSniDos::DoDataExchange(CDataExchange* pDX)
{
	CSDITestView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewSniDos)
	DDX_Control(pDX, IDC_RADIO_POINT_OF_DATA_0, m_btnPointOfData0);
	DDX_Control(pDX, IDC_RADIO_POINT_OF_DATA_1, m_btnPointOfData1);
	DDX_Control(pDX, IDC_RADIO_POINT_OF_DATA_2, m_btnPointOfData2);
	DDX_Control(pDX, IDC_RADIO_POINT_OF_DATA_3, m_btnPointOfData3);
	DDX_Control(pDX, IDC_RADIO_POINT_OF_DATA_4, m_btnPointOfData4);
	DDX_Control(pDX, IDC_RADIO_POINT_OF_DATA_5, m_btnPointOfData5);
	DDX_Control(pDX, IDC_RADIO_POINT_OF_DATA_6, m_btnPointOfData6);
	DDX_Control(pDX, IDC_RADIO_POINT_OF_DATA_7, m_btnPointOfData7);
	DDX_Control(pDX, IDC_RADIO_POINT_OF_DATA_8, m_btnPointOfData8);
	DDX_Control(pDX, IDC_RADIO_POINT_OF_DATA_9, m_btnPointOfData9);
	DDX_Radio(pDX, IDC_RADIO_POINT_OF_DATA_0, m_iPointOfData);
	//}}AFX_DATA_MAP

	if (m_ToolTip.GetSafeHwnd()) {
		m_ToolTip.Activate(TRUE);
	}
}
/////////////////////////////////////////////////////////////////////////////
// CViewSniDos diagnostics
#ifdef _DEBUG
void CViewSniDos::AssertValid() const
{
	CSDITestView::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CViewSniDos::Dump(CDumpContext& dc) const
{
	CSDITestView::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CViewSniDos, CSDITestView)
	//{{AFX_MSG_MAP(CViewSniDos)
	ON_BN_CLICKED(IDC_RADIO_POINT_OF_DATA_0, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_POINT_OF_DATA_1, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_POINT_OF_DATA_2, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_POINT_OF_DATA_3, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_POINT_OF_DATA_4, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_POINT_OF_DATA_5, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_POINT_OF_DATA_6, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_POINT_OF_DATA_7, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_POINT_OF_DATA_8, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_POINT_OF_DATA_9, OnRadioChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CViewSniDos message handlers
void CViewSniDos::OnRadioChanged() 
{
	UpdateData();
	ShowHide();
	GetDocument()->m_iPointOfData = m_iPointOfData;
}
