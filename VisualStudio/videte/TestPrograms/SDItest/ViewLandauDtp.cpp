/* GlobalReplace: CSDITestDtpDoc --> CDocLandauDtp */
/* GlobalReplace: CSDITestDtpView --> CViewLandauDtp */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ViewLandauDtp.cpp $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/SDITest/ViewLandauDtp.cpp $
// CHECKIN:		$Date: 28.01.00 12:09 $
// VERSION:		$Revision: 16 $
// LAST CHANGE:	$Modtime: 28.01.00 11:45 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#include "stdafx.h"

#include "ViewLandauDtp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewLandauDtp
IMPLEMENT_DYNCREATE(CViewLandauDtp, CSDITestView)
/////////////////////////////////////////////////////////////////////////////
CViewLandauDtp::CViewLandauDtp()
	: CSDITestView(CViewLandauDtp::IDD)
{
	//{{AFX_DATA_INIT(CViewLandauDtp)
	m_iPortNr = 0;
	m_iCameraNr = 0;
	//}}AFX_DATA_INIT
	m_uTimerID = TIMER_DTP;
}
/////////////////////////////////////////////////////////////////////////////
CViewLandauDtp::~CViewLandauDtp()
{
}
/////////////////////////////////////////////////////////////////////////////
void CViewLandauDtp::ShowHide()
{
	// Die Schnittstelle und Kamerafelder anzeigen,
	BOOL bEnable = !(GetDocument()->m_bStarted) && !(GetDocument()->m_bRepeat && (0 != GetDocument()->m_iRepeatHow));
	m_btnPortNr1.EnableWindow(bEnable);
	m_btnPortNr2.EnableWindow(bEnable);
	m_btnPortNr3.EnableWindow(bEnable);
	m_btnPortNr4.EnableWindow(bEnable);
	m_btnPortUpdate.EnableWindow(bEnable);
	m_btnCameraNr1.EnableWindow(bEnable);
	m_btnCameraNr2.EnableWindow(bEnable);
	m_btnCameraUpdateNull.EnableWindow(bEnable);
	m_btnCameraUpdateSpace.EnableWindow(bEnable);

	// Basisklasse aufrufen
	CSDITestView::ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CViewLandauDtp::InitializeData()
{
	m_iPortNr	= GetDocument()->m_iPortNr;
	m_iCameraNr	= GetDocument()->m_iCameraNr;

	CSDITestView::InitializeData();
}
/////////////////////////////////////////////////////////////////////////////
void CViewLandauDtp::DoDataExchange(CDataExchange* pDX)
{
	CSDITestView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewLandauDtp)
	DDX_Control(pDX, IDC_RADIO_PORT_NR_1, m_btnPortNr1);
	DDX_Control(pDX, IDC_RADIO_PORT_NR_2, m_btnPortNr2);
	DDX_Control(pDX, IDC_RADIO_PORT_NR_3, m_btnPortNr3);
	DDX_Control(pDX, IDC_RADIO_PORT_NR_4, m_btnPortNr4);
	DDX_Control(pDX, IDC_RADIO_PORT_UPDATE, m_btnPortUpdate);
	DDX_Control(pDX, IDC_RADIO_CAMERA_NR_1, m_btnCameraNr1);
	DDX_Control(pDX, IDC_RADIO_CAMERA_NR_2, m_btnCameraNr2);
	DDX_Control(pDX, IDC_RADIO_CAMERA_UPDATE_NULL, m_btnCameraUpdateNull);
	DDX_Control(pDX, IDC_RADIO_CAMERA_UPDATE_SPACE, m_btnCameraUpdateSpace);
	DDX_Radio(pDX, IDC_RADIO_PORT_NR_1, m_iPortNr);
	DDX_Radio(pDX, IDC_RADIO_CAMERA_NR_1, m_iCameraNr);
	//}}AFX_DATA_MAP

	if (m_ToolTip.GetSafeHwnd()) {
		m_ToolTip.Activate(TRUE);
	}
}
/////////////////////////////////////////////////////////////////////////////
// CViewLandauDtp diagnostics
#ifdef _DEBUG
void CViewLandauDtp::AssertValid() const
{
	CSDITestView::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CViewLandauDtp::Dump(CDumpContext& dc) const
{
	CSDITestView::Dump(dc);
}
/////////////////////////////////////////////////////////////////////////////
CDocLandauDtp* CViewLandauDtp::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDocLandauDtp)));
	return (CDocLandauDtp*)m_pDocument;
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CViewLandauDtp, CSDITestView)
	//{{AFX_MSG_MAP(CViewLandauDtp)
	ON_BN_CLICKED(IDC_RADIO_PORT_NR_1, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_PORT_NR_2, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_PORT_NR_3, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_PORT_NR_4, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_PORT_UPDATE, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_CAMERA_NR_1, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_CAMERA_NR_2, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_CAMERA_UPDATE_NULL, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_CAMERA_UPDATE_SPACE, OnRadioChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CViewLandauDtp message handlers
/////////////////////////////////////////////////////////////////////////////
void CViewLandauDtp::OnRadioChanged() 
{
	UpdateData();
	ShowHide();
	GetDocument()->m_iPortNr = m_iPortNr;
	GetDocument()->m_iCameraNr = m_iCameraNr;
}
