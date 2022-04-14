// AviPlayerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CreateAvi.h"
#include "AviPlayerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAviPlayerDlg dialog
CAviPlayerDlg::CAviPlayerDlg(CString sFilename, WORD wMaxFrames, CWnd* pParent /*=NULL*/)
	: CDialog(CAviPlayerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAviPlayerDlg)
	m_sAviFile = sFilename;
	m_sCurFrame = _T("");
	m_sMaxFrames = _T("");
	//}}AFX_DATA_INIT
	m_hStop			= LoadBitmap(AfxGetResourceHandle(),MAKEINTRESOURCE(IDB_STOP));
	m_hPause		= LoadBitmap(AfxGetResourceHandle(),MAKEINTRESOURCE(IDB_PAUSE));
	m_hPlayOnce		= LoadBitmap(AfxGetResourceHandle(),MAKEINTRESOURCE(IDB_PLAY_ONCE));
	m_hPlayLoop		= LoadBitmap(AfxGetResourceHandle(),MAKEINTRESOURCE(IDB_PLAY_LOOP));
	m_hStepBack		= LoadBitmap(AfxGetResourceHandle(),MAKEINTRESOURCE(IDB_STEP_BACK));
	m_hStepFor		= LoadBitmap(AfxGetResourceHandle(),MAKEINTRESOURCE(IDB_STEP_FOR));
	m_wCurFrame		= 0;
	m_wMaxFrames	= wMaxFrames;
	m_bPlaying		= FALSE;
}
/////////////////////////////////////////////////////////////////////////////
CAviPlayerDlg::~CAviPlayerDlg()
{
	DeleteObject(m_hStop);
	DeleteObject(m_hPause);
	DeleteObject(m_hPlayOnce);
	DeleteObject(m_hPlayLoop);
	DeleteObject(m_hStepBack);
	DeleteObject(m_hStepFor);
}
/////////////////////////////////////////////////////////////////////////////
void CAviPlayerDlg::EnableControls()
{
	UpdateData();

	BOOL bEnable = FALSE;
	bEnable = m_bPlaying;
	m_btnStop.EnableWindow(bEnable);
	bEnable = !m_bPlaying;
	m_btnPlayOnce.EnableWindow(bEnable);
	bEnable = !m_bPlaying;
	m_btnPlayLoop.EnableWindow(bEnable);
	bEnable =    !m_bPlaying
			  && (m_wCurFrame > 0);
	m_btnStepBack.EnableWindow(bEnable);
	bEnable =    !m_bPlaying
			  && (m_wCurFrame < m_wMaxFrames);
	m_btnStepFor.EnableWindow(bEnable);

	if (m_bPlaying)
	{
		m_sCurFrame = "";
	}
	else
	{
		m_sCurFrame.Format("%u", m_wCurFrame);
	}
	if (0 < m_wMaxFrames)
	{
		m_sMaxFrames.Format("%u", m_wMaxFrames);
	}
	else
	{
		m_sMaxFrames = "";
	}
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CAviPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAviPlayerDlg)
	DDX_Control(pDX, IDC_BTN_PAUSE, m_btnPause);
	DDX_Control(pDX, IDC_BTN_STOP, m_btnStop);
	DDX_Control(pDX, IDC_BTN_STEP_FOR, m_btnStepFor);
	DDX_Control(pDX, IDC_BTN_STEP_BACK, m_btnStepBack);
	DDX_Control(pDX, IDC_BTN_PLAY_LOOP, m_btnPlayLoop);
	DDX_Control(pDX, IDC_BTN_PLAY_ONCE, m_btnPlayOnce);
	DDX_Control(pDX, IDC_ANIMATE, m_Animation);
	DDX_Text(pDX, IDC_TXT_AVIFILE, m_sAviFile);
	DDX_Text(pDX, IDC_TXT_CUR_FRAME, m_sCurFrame);
	DDX_Text(pDX, IDC_TXT_MAX_FRAMES, m_sMaxFrames);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CAviPlayerDlg, CDialog)
	//{{AFX_MSG_MAP(CAviPlayerDlg)
	ON_BN_CLICKED(IDC_BTN_STOP, OnBtnStop)
	ON_BN_CLICKED(IDC_BTN_PLAY_ONCE, OnBtnPlayOnce)
	ON_BN_CLICKED(IDC_BTN_PLAY_LOOP, OnBtnPlayLoop)
	ON_BN_CLICKED(IDC_BTN_STEP_BACK, OnBtnStepBack)
	ON_BN_CLICKED(IDC_BTN_STEP_FOR, OnBtnStepFor)
	ON_BN_CLICKED(IDC_BTN_PAUSE, OnBtnPause)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CAviPlayerDlg message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CAviPlayerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_btnStop.SetBitmap(m_hStop);
	m_btnPause.SetBitmap(m_hPause);
	m_btnPlayOnce.SetBitmap(m_hPlayOnce);
	m_btnPlayLoop.SetBitmap(m_hPlayLoop);
	m_btnStepBack.SetBitmap(m_hStepBack);
	m_btnStepFor.SetBitmap(m_hStepFor);

	if (m_Animation.Open(m_sAviFile))
	{
		if (m_Animation.Seek(0))
		{
			m_wCurFrame = 0;
		}
	}
	
	EnableControls();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CAviPlayerDlg::OnBtnPause() 
{
	// TODO: Add your control notification handler code here
	
}
/////////////////////////////////////////////////////////////////////////////
void CAviPlayerDlg::OnBtnStop() 
{
	m_Animation.Stop();
	m_Animation.Seek(0);
	m_wCurFrame = 0;
	m_bPlaying	= FALSE;
	EnableControls();
}
/////////////////////////////////////////////////////////////////////////////
void CAviPlayerDlg::OnBtnPlayOnce() 
{
	m_Animation.Play(0, -1, 1);
	m_bPlaying	= TRUE;
	EnableControls();
}
/////////////////////////////////////////////////////////////////////////////
void CAviPlayerDlg::OnBtnPlayLoop() 
{
	m_Animation.Play(0, -1, -1);
	m_bPlaying	= TRUE;
	EnableControls();
}
/////////////////////////////////////////////////////////////////////////////
void CAviPlayerDlg::OnBtnStepBack() 
{
	if (m_Animation.Seek(m_wCurFrame-1))
	{
		m_wCurFrame--;
	}
	EnableControls();
}
/////////////////////////////////////////////////////////////////////////////
void CAviPlayerDlg::OnBtnStepFor() 
{
	if (m_Animation.Seek(m_wCurFrame+1))
	{
		m_wCurFrame++;
		if (m_wCurFrame > m_wMaxFrames)
		{
			m_wMaxFrames++;
		}
	}
	EnableControls();
}
