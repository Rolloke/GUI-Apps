// PTConfigView.cpp : implementation file
//

#include "stdafx.h"
#include "PTConfig.h"
#include "PTConfigView.h"
#include "PTConfigDoc.h"

#include "PTInputClient.h"
#include "PTOutputClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPTConfigView

IMPLEMENT_DYNCREATE(CPTConfigView, CFormView)

CPTConfigView::CPTConfigView()
	: CFormView(CPTConfigView::IDD)
{
	//{{AFX_DATA_INIT(CPTConfigView)
	m_sAlarmName1 = _T("");
	m_sAlarmName2 = _T("");
	m_sAlarmName3 = _T("");
	m_sCamName1 = _T("");
	m_sCamName2 = _T("");
	m_sCamName3 = _T("");
	m_bHasVideoSignal1 = FALSE;
	m_bHasVideoSignal2 = FALSE;
	m_bHasVideoSignal3 = FALSE;
	m_bColorCam1 = FALSE;
	m_bColorCam2 = FALSE;
	m_bColorCam3 = FALSE;
	m_sBoxName = _T("");
	m_sBoxNumber = _T("");
	//}}AFX_DATA_INIT
	m_bInSave = FALSE;
	
}

CPTConfigView::~CPTConfigView()
{
}

void CPTConfigView::DoDataExchange(CDataExchange* pDX)
{
	// if (pDX==NULL) return;
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPTConfigView)
	DDX_Control(pDX, IDC_GROUP_BASICS, m_ctGroupBasics);
	DDX_Control(pDX, IDC_VIDEO_SIGNAL3, m_ctVideoSignal3);
	DDX_Control(pDX, IDC_VIDEO_SIGNAL2, m_ctVideoSignal2);
	DDX_Control(pDX, IDC_VIDEO_SIGNAL1, m_ctVideoSignal1);
	DDX_Control(pDX, ID_GROUP_ACTIONS, m_ctGroupActions);
	DDX_Control(pDX, ID_GROUP_CAMERAS, m_ctGroupCameras);
	DDX_Control(pDX, ID_GROUP_ALARMS, m_ctGroupAlarms);
	DDX_Control(pDX, IDC_DO_SAVE, m_ctDoSave);
	DDX_Control(pDX, IDC_ANIMATE2, m_ctAnimate2);
	DDX_Control(pDX, IDC_ANIMATE1, m_ctAnimate1);
	DDX_Text(pDX, IDC_ALARM_NAME1, m_sAlarmName1);
	DDV_MaxChars(pDX, m_sAlarmName1, 12);
	DDX_Text(pDX, IDC_ALARM_NAME2, m_sAlarmName2);
	DDV_MaxChars(pDX, m_sAlarmName2, 12);
	DDX_Text(pDX, IDC_ALARM_NAME3, m_sAlarmName3);
	DDV_MaxChars(pDX, m_sAlarmName3, 12);
	DDX_Text(pDX, IDC_KAMERA_NAME1, m_sCamName1);
	DDV_MaxChars(pDX, m_sCamName1, 20);
	DDX_Text(pDX, IDC_KAMERA_NAME2, m_sCamName2);
	DDX_Text(pDX, IDC_KAMERA_NAME3, m_sCamName3);
	DDX_Check(pDX, IDC_VIDEO_SIGNAL1, m_bHasVideoSignal1);
	DDX_Check(pDX, IDC_VIDEO_SIGNAL2, m_bHasVideoSignal2);
	DDX_Check(pDX, IDC_VIDEO_SIGNAL3, m_bHasVideoSignal3);
	DDX_Check(pDX, IDC_COLOR_CAM1, m_bColorCam1);
	DDX_Check(pDX, IDC_COLOR_CAM2, m_bColorCam2);
	DDX_Check(pDX, IDC_COLOR_CAM3, m_bColorCam3);
	DDX_Text(pDX, IDC_BOX_NAME, m_sBoxName);
	DDX_Text(pDX, IDC_BOX_NUMBER, m_sBoxNumber);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPTConfigView, CFormView)
	//{{AFX_MSG_MAP(CPTConfigView)
	ON_BN_CLICKED(IDC_RESET_DATA, OnResetData)
	ON_BN_CLICKED(IDC_DO_SAVE, OnDoSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPTConfigView diagnostics

#ifdef _DEBUG
void CPTConfigView::AssertValid() const
{
	CFormView::AssertValid();
}

void CPTConfigView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPTConfigView message handlers

void CPTConfigView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if (m_bInSave) {
		WK_TRACE("Still in save...\n");
		return;	// <<< EXIT >>>
	}

	if (GetDoc()->m_state != PCS_CALLING)  {
		m_ctAnimate2.Stop();
		m_ctAnimate2.Close();
		m_ctAnimate2.ShowWindow(SW_HIDE);
	}
	ResetAllData();
	// OOPS CFormView::OnUpdate(pSender,lHint,pHint);
}

void CPTConfigView::ResetAllData()
{
	if (GetDoc()->m_state==PCS_CONNECTED) {
		m_ctDoSave.EnableWindow(TRUE);
	} else {
		m_ctDoSave.EnableWindow(FALSE);
	}

	m_sAlarmName1 = "Unbekannt";
	m_sAlarmName2 = "Unbekannt";
	m_sAlarmName3 = "Unbekannt";

	m_sCamName1 = "Unbekannt";
	m_sBoxName = GetDoc()->m_sBoxName;
	m_sBoxNumber = GetDoc()->m_sBoxNumber;

	int i=0;
	int iNumRecords = 0;
	if (GetDoc()->m_pInput) {
		
		iNumRecords = GetDoc()->m_pInput->GetNumberOfInputs();
		for (i=0;i<iNumRecords;i++) {
			const CIPCInputRecord& oneRecord = 
				GetDoc()->m_pInput->GetInputRecordFromIndex(i);
			switch (i) {
			case 0:
				m_sAlarmName1 = oneRecord.GetName();
				break;
			case 1:
				m_sAlarmName2 = oneRecord.GetName();
				break;
			case 2:
				m_sAlarmName3 = oneRecord.GetName();
				break;
			default:
				WK_TRACE("Error unsupported inputRecord at %d\n",i);
			}
		}
		// NOT YET disable non-available windows
	}

	//////////////// outputs //////////////
	if (GetDoc()->m_pOutput) {
		iNumRecords = GetDoc()->m_pOutput->GetNumberOfOutputs();
		for (i=0;i<iNumRecords;i++) {
			const CIPCOutputRecord& oneRecord = 
				GetDoc()->m_pOutput->GetOutputRecordFromIndex(i);

			switch (i) {
			case 0:
				m_sCamName1 = oneRecord.GetName();
				m_bHasVideoSignal1 = (oneRecord.GetFlags() & OREC_IS_OKAY);
				break;
			case 1:
				m_sCamName2 = oneRecord.GetName();
				m_bHasVideoSignal2 = (oneRecord.GetFlags() & OREC_IS_OKAY);				
				break;
			case 2:
				m_sCamName3 = oneRecord.GetName();
				m_bHasVideoSignal3 = (oneRecord.GetFlags() & OREC_IS_OKAY);				
				break;
			default:
				WK_TRACE("Error unsupported outputRecord at %d\n",i);
			}
		}
	}

	UpdateData(FALSE);
}


void CPTConfigView::OnResetData() 
{
	ResetAllData();	
}

void CPTConfigView::OnDoSave() 
{
	m_ctAnimate1.ShowWindow(SW_NORMAL);
#if 0
	m_ctAnimate1.Open("filecopy.avi");
#else
	m_ctAnimate1.Open(IDR_AVI_COPY);
#endif
	m_ctAnimate1.Play(0,-1,-1);	// start, end , count

	UpdateData();	// fill the members

	m_bInSave = TRUE;
	// take the views m_s..... and set them via CIPC
	GetDoc()->m_pOutput->DoRequestSetValue( SECID_NO_ID,"BoxName",m_sBoxName);

	GetDoc()->m_pOutput->DoRequestSetValue( 
		CSecID(1,0),	// OOPS no groupid
		"CameraName",
		m_sCamName1
		);
	GetDoc()->m_pOutput->DoRequestSetValue( 
		CSecID(1,1),	// OOPS no groupid
		"CameraName",
		m_sCamName2
		);
	GetDoc()->m_pOutput->DoRequestSetValue( 
		CSecID(1,2),	// OOPS no groupid
		"CameraName",
		m_sCamName3
		);

	GetDoc()->m_pOutput->DoRequestSetValue( CSecID(1,0),"WriteEEP","egal" );
	// OOPS what about incoming changes ?
	m_bInSave=FALSE;

	Sleep(2000);	// OOPS

	GetDoc()->m_pOutput->DoRequestGetValue(SECID_NO_ID,"BoxName");
	GetDoc()->m_pOutput->DoRequestInfoOutputs(SECID_NO_GROUPID );
	m_ctAnimate1.Stop();
	m_ctAnimate1.Close();
	m_ctAnimate1.ShowWindow(SW_HIDE);

	// ResetAllData();	// OOPS also the still incoming confirms ?
}

void CPTConfigView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	m_ctVideoSignal1.EnableWindow(FALSE);
	m_ctVideoSignal2.EnableWindow(FALSE);
	m_ctVideoSignal3.EnableWindow(FALSE);

	// set the nifty icons, if not already done so
	if (m_ctGroupBasics.GetIcon()==NULL) {
		HICON hIcon = AfxGetApp()->LoadIcon( MAKEINTRESOURCE(IDI_BASICS) );
		m_ctGroupBasics.SetIcon(hIcon);
	}

	if (m_ctGroupAlarms.GetIcon()==NULL) {
		HICON hIcon = AfxGetApp()->LoadIcon( MAKEINTRESOURCE(IDI_ALARMS) );
		m_ctGroupAlarms.SetIcon(hIcon);
	}

	if (m_ctGroupCameras.GetIcon()==NULL) {
		HICON hIcon = AfxGetApp()->LoadIcon( MAKEINTRESOURCE(IDI_CAMERAS) );
		m_ctGroupCameras.SetIcon(hIcon);
	}

	if (m_ctGroupActions.GetIcon()==NULL) {
		HICON hIcon = AfxGetApp()->LoadIcon( MAKEINTRESOURCE(IDI_ACTIONS) );
		m_ctGroupActions.SetIcon(hIcon);
	}


	if (bActivate) {
		CRect rect;
		m_ctAnimate1.GetClientRect(rect);
		
		/* m_ctAnimate1.Create(
			dwStyle, 
			rect, 
			this, 1
			); */
#if 0
		m_ctAnimate2.Open("search.avi");
#else
		m_ctAnimate2.Open( IDR_AVI_SEARCH);
#endif
		m_ctAnimate2.Play(0,-1,-1);	// start, end , count
	}

	
	CFormView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}



