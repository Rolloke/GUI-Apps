/* GlobalReplace: CSDITestIbmDoc --> CDocIbm */
/* GlobalReplace: CSDITestIbmView --> CViewIbm */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ViewIbm.cpp $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/SDITest/ViewIbm.cpp $
// CHECKIN:		$Date: 10.05.99 11:56 $
// VERSION:		$Revision: 9 $
// LAST CHANGE:	$Modtime: 10.05.99 11:41 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#include "stdafx.h"
#include "sditest.h"
#include "ViewIbm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewIbm
IMPLEMENT_DYNCREATE(CViewIbm, CSDITestView)
/////////////////////////////////////////////////////////////////////////////
CViewIbm::CViewIbm()
	: CSDITestView(CViewIbm::IDD)
{
	//{{AFX_DATA_INIT(CViewIbm)
	m_iPointOfData = 0;
	m_sDataFormat = _T("");
	m_bStatusRequest = FALSE;
	m_bWithTerminalID = FALSE;
	//}}AFX_DATA_INIT
	m_uTimerID = TIMER_IBM;
}
/////////////////////////////////////////////////////////////////////////////
CViewIbm::~CViewIbm()
{
}
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
CDocIbm* CViewIbm::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDocIbm)));
	return (CDocIbm*)m_pDocument;
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
void CViewIbm::ShowHide()
{
	// Die Schnittstelle und Kamerafelder anzeigen,
	BOOL bEnable = !(GetDocument()->m_bStarted)
					&& !((GetDocument()->m_bRepeat)
						&& (0 != GetDocument()->m_iRepeatHow));
	m_btnPointOfData1.EnableWindow(bEnable);
	m_btnPointOfData2.EnableWindow(bEnable);
	m_btnPointOfData3.EnableWindow(bEnable);
	m_btnPointOfData4.EnableWindow(bEnable);
	m_btnPointOfData5.EnableWindow(bEnable);
	m_btnPointOfData6.EnableWindow(bEnable);
	m_btnPointOfData7.EnableWindow(bEnable);
	m_btnPointOfData8.EnableWindow(bEnable);
	m_btnPointOfData9.EnableWindow(bEnable);
	m_btnPointOfData10.EnableWindow(bEnable);
	m_btnPointOfData11.EnableWindow(bEnable);
	m_btnPointOfData12.EnableWindow(bEnable);
	m_btnPointOfData13.EnableWindow(bEnable);
	m_btnPointOfData14.EnableWindow(bEnable);
	m_btnPointOfData15.EnableWindow(bEnable);
	m_btnPointOfData16.EnableWindow(bEnable);

	bEnable = !(GetDocument()->m_bStarted)
				&& !(GetDocument()->m_bStatusRequest);
	m_editDataFormat.EnableWindow(bEnable);

	bEnable = (GetDocument()->m_bStatusRequest);
	m_btnWithTerminalID.EnableWindow(bEnable);

	// Basisklasse aufrufen
	CSDITestView::ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CViewIbm::InitializeData()
{
	m_iPointOfData	= GetDocument()->m_iPointOfData;
	m_sDataFormat	= GetDocument()->m_sDataFormat;

	CSDITestView::InitializeData();
}
/////////////////////////////////////////////////////////////////////////////
void CViewIbm::DoDataExchange(CDataExchange* pDX)
{
	CSDITestView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewIbm)
	DDX_Control(pDX, IDC_CHECK_WITH_TERMINAL_ID, m_btnWithTerminalID);
	DDX_Control(pDX, IDC_CHECK_STATUS_REQUEST, m_btnStatusRequest);
	DDX_Control(pDX, IDC_EDIT_DATA_FORMAT, m_editDataFormat);
	DDX_Control(pDX, IDC_RADIO_POINT_OF_DATA_1, m_btnPointOfData1);
	DDX_Control(pDX, IDC_RADIO_POINT_OF_DATA_2, m_btnPointOfData2);
	DDX_Control(pDX, IDC_RADIO_POINT_OF_DATA_3, m_btnPointOfData3);
	DDX_Control(pDX, IDC_RADIO_POINT_OF_DATA_4, m_btnPointOfData4);
	DDX_Control(pDX, IDC_RADIO_POINT_OF_DATA_5, m_btnPointOfData5);
	DDX_Control(pDX, IDC_RADIO_POINT_OF_DATA_6, m_btnPointOfData6);
	DDX_Control(pDX, IDC_RADIO_POINT_OF_DATA_7, m_btnPointOfData7);
	DDX_Control(pDX, IDC_RADIO_POINT_OF_DATA_8, m_btnPointOfData8);
	DDX_Control(pDX, IDC_RADIO_POINT_OF_DATA_9, m_btnPointOfData9);
	DDX_Control(pDX, IDC_RADIO_POINT_OF_DATA_10, m_btnPointOfData10);
	DDX_Control(pDX, IDC_RADIO_POINT_OF_DATA_11, m_btnPointOfData11);
	DDX_Control(pDX, IDC_RADIO_POINT_OF_DATA_12, m_btnPointOfData12);
	DDX_Control(pDX, IDC_RADIO_POINT_OF_DATA_13, m_btnPointOfData13);
	DDX_Control(pDX, IDC_RADIO_POINT_OF_DATA_14, m_btnPointOfData14);
	DDX_Control(pDX, IDC_RADIO_POINT_OF_DATA_15, m_btnPointOfData15);
	DDX_Control(pDX, IDC_RADIO_POINT_OF_DATA_16, m_btnPointOfData16);
	DDX_Radio(pDX, IDC_RADIO_POINT_OF_DATA_1, m_iPointOfData);
	DDX_Text(pDX, IDC_EDIT_DATA_FORMAT, m_sDataFormat);
	DDV_MaxChars(pDX, m_sDataFormat, 228);
	DDX_Check(pDX, IDC_CHECK_STATUS_REQUEST, m_bStatusRequest);
	DDX_Check(pDX, IDC_CHECK_WITH_TERMINAL_ID, m_bWithTerminalID);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
// CViewIbm diagnostics
#ifdef _DEBUG
void CViewIbm::AssertValid() const
{
	CSDITestView::AssertValid();
}

void CViewIbm::Dump(CDumpContext& dc) const
{
	CSDITestView::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CViewIbm, CSDITestView)
	//{{AFX_MSG_MAP(CViewIbm)
	ON_BN_CLICKED(IDC_RADIO_POINT_OF_DATA_1, OnRadioChanged)
	ON_EN_CHANGE(IDC_EDIT_DATA_FORMAT, OnChangeEditDataFormat)
	ON_BN_CLICKED(IDC_RADIO_POINT_OF_DATA_2, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_POINT_OF_DATA_3, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_POINT_OF_DATA_4, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_POINT_OF_DATA_5, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_POINT_OF_DATA_6, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_POINT_OF_DATA_7, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_POINT_OF_DATA_8, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_POINT_OF_DATA_9, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_POINT_OF_DATA_10, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_POINT_OF_DATA_11, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_POINT_OF_DATA_12, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_POINT_OF_DATA_13, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_POINT_OF_DATA_14, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_POINT_OF_DATA_15, OnRadioChanged)
	ON_BN_CLICKED(IDC_RADIO_POINT_OF_DATA_16, OnRadioChanged)
	ON_BN_CLICKED(IDC_CHECK_STATUS_REQUEST, OnCheckStatusRequest)
	ON_BN_CLICKED(IDC_CHECK_WITH_TERMINAL_ID, OnCheckWithTerminalID)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CViewIbm message handlers
void CViewIbm::OnRadioChanged() 
{
	UpdateData();
	ShowHide();
	GetDocument()->m_iPointOfData = m_iPointOfData;
}
/////////////////////////////////////////////////////////////////////////////
void CViewIbm::OnCheckStatusRequest() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	GetDocument()->m_bStatusRequest = m_bStatusRequest;
	if (m_bStatusRequest == FALSE) {
		m_bWithTerminalID = FALSE;
		GetDocument()->m_bWithTerminalID = m_bWithTerminalID;
		UpdateData(FALSE);
	}
	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CViewIbm::OnCheckWithTerminalID() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	GetDocument()->m_bWithTerminalID = m_bWithTerminalID;
}
/////////////////////////////////////////////////////////////////////////////
void CViewIbm::OnChangeEditDataFormat() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CSDITestView::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	UpdateData();
	GetDocument()->m_sDataFormat = m_sDataFormat;
}
