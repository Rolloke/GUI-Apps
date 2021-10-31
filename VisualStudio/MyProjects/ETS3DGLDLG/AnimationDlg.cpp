// AnimationDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ETS3DGLDLG.h"
#include "AnimationDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CAnimationDlg 
#ifdef _DEBUG

CAnimationDlg::CAnimationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAnimationDlg::IDD, pParent)
{
   AfxEnableControlContainer();
   AfxVariantInit(&m_varRpt);
   AfxVariantInit(&m_varStart);
   AfxVariantInit(&m_varStop);
   m_varRpt.vt     = VT_I4;
   m_varRpt.lVal   = 1;
   m_varStart.vt   = VT_I4;
   m_varStart.lVal = 0;
   m_varStop.vt    = VT_I4;
   m_varStop.lVal  = -1;

	//{{AFX_DATA_INIT(CAnimationDlg)
	//}}AFX_DATA_INIT
}


void CAnimationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAnimationDlg)
	DDX_Control(pDX, IDC_ANIMATION, m_cAnimationCtrl);
	DDX_Control(pDX, IDC_MMCONTROL, m_cMMControl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAnimationDlg, CDialog)
	//{{AFX_MSG_MAP(CAnimationDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CAnimationDlg 

BOOL CAnimationDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

BEGIN_EVENTSINK_MAP(CAnimationDlg, CDialog)
    //{{AFX_EVENTSINK_MAP(CAnimationDlg)
	ON_EVENT(CAnimationDlg, IDC_MMCONTROL, 4 /* PlayClick */, OnPlayClickMmcontrol, VTS_PI2)
	ON_EVENT(CAnimationDlg, IDC_MMCONTROL, 7 /* StopClick */, OnStopClickMmcontrol, VTS_PI2)
	ON_EVENT(CAnimationDlg, IDC_MMCONTROL, 3 /* NextClick */, OnNextClickMmcontrol, VTS_PI2)
	ON_EVENT(CAnimationDlg, IDC_MMCONTROL, 2 /* PrevClick */, OnPrevClickMmcontrol, VTS_PI2)
	ON_EVENT(CAnimationDlg, IDC_MMCONTROL, 1 /* BackClick */, OnBackClickMmcontrol, VTS_PI2)
	ON_EVENT(CAnimationDlg, IDC_MMCONTROL, 6 /* StepClick */, OnStepClickMmcontrol, VTS_PI2)
	ON_EVENT(CAnimationDlg, IDC_MMCONTROL, 5 /* PauseClick */, OnPauseClickMmcontrol, VTS_PI2)
	ON_EVENT(CAnimationDlg, IDC_MMCONTROL, 9 /* EjectClick */, OnEjectClickMmcontrol, VTS_PI2)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CAnimationDlg::OnPlayClickMmcontrol(short FAR* Cancel) 
{
   m_varStop.lVal = -1;
	m_cAnimationCtrl.Play(m_varRpt, m_varStart, m_varStop);
}

void CAnimationDlg::OnStopClickMmcontrol(short FAR* Cancel) 
{
	m_cAnimationCtrl.Stop();
}

void CAnimationDlg::OnNextClickMmcontrol(short FAR* Cancel) 
{
   m_varStop.lVal = m_varStart.lVal+1;
	m_cAnimationCtrl.Play(m_varRpt, m_varStart, m_varStop);
   m_varStart.lVal++;
}

void CAnimationDlg::OnPrevClickMmcontrol(short FAR* Cancel) 
{
   if (m_varStart.lVal > 0)
   {
      m_varStop.lVal = m_varStart.lVal-1;
	   m_cAnimationCtrl.Play(m_varRpt, m_varStart, m_varStop);
      m_varStart.lVal--;
   }
}

void CAnimationDlg::OnBackClickMmcontrol(short FAR* Cancel) 
{
   m_varStop.lVal = m_varStart.lVal = 0;
	m_cAnimationCtrl.Play(m_varRpt, m_varStart, m_varStop);
}

void CAnimationDlg::OnStepClickMmcontrol(short FAR* Cancel) 
{
   m_varStop.lVal = -1;
	m_cAnimationCtrl.Play(m_varRpt, m_varStart, m_varStop);
}

void CAnimationDlg::OnPauseClickMmcontrol(short FAR* Cancel) 
{
   m_varStop.lVal = -1;
	m_cAnimationCtrl.Play(m_varRpt, m_varStart, m_varStop);
}

void CAnimationDlg::OnEjectClickMmcontrol(short FAR* Cancel) 
{
   CETS3DGLDLGApp *pApp = (CETS3DGLDLGApp*) AfxGetApp();
   CString str;
   if (pApp->GetFolderPath(str, NULL, FOLDERPATH_RETURN_FILES))
   {
      m_cAnimationCtrl.Open(str);
      m_cMMControl.SetCanPlay(true);
   }
}

#endif // _DEBUG
