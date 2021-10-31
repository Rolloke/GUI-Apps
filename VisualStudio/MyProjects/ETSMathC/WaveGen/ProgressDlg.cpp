// ProgressDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "WaveGen.h"
#include "ProgressDlg.h"
#include "MainFrm.h"
#include "WaveGenDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CProgressDlg 


CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressDlg::IDD, pParent)
{
   ::InitializeCriticalSection(&m_csProgress);
	//{{AFX_DATA_INIT(CProgressDlg)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
   
}

CProgressDlg::~CProgressDlg()
{
   ::DeleteCriticalSection(&m_csProgress);
}


void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgressDlg)
	DDX_Control(pDX, IDC_PROGRESS1, m_cProgress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
	//{{AFX_MSG_MAP(CProgressDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CProgressDlg 

void CProgressDlg::OnCancel() 
{
//   ::EnterCriticalSection(&m_csProgress);
//   m_cProgress.m_hWnd = NULL;
   theApp.m_pMainWnd->PostMessage(WM_COMMAND, MAKELONG(ID_CALC_CANCEL, 1), NULL);
//   ::LeaveCriticalSection(&m_csProgress);
}

BOOL CProgressDlg::OnInitDialog() 
{
   ::EnterCriticalSection(&m_csProgress);
	CDialog::OnInitDialog();
   ::LeaveCriticalSection(&m_csProgress);
   if (!::IsWindow(m_cProgress.m_hWnd))
   {
      m_cProgress.m_hWnd = NULL;
      return 0;
   }

	m_cProgress.SetRange(0, 64);
   m_cProgress.SetPos(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CProgressDlg::SetRange(int nR)
{
   m_nStep = nR >> 6;
   if (m_nStep <= 0) m_nStep = 1;
   m_nPos       = 0;
   m_nStepCount = 0;
   if (m_cProgress.m_hWnd)
   {
      ::EnterCriticalSection(&m_csProgress);
      m_cProgress.SetPos(++m_nPos);
      ::LeaveCriticalSection(&m_csProgress);
   }
}

void CProgressDlg::Step()
{
   if (m_nStepCount++ > m_nStep)
   {
      m_nStepCount = 0;
      if (m_cProgress.m_hWnd)
      {
         ::EnterCriticalSection(&m_csProgress);
         ::SendMessage(m_cProgress.m_hWnd, PBM_SETPOS, ++m_nPos, 0L);
         ::LeaveCriticalSection(&m_csProgress);
      }
   }
}
