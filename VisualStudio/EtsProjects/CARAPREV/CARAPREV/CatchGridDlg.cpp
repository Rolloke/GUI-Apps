// CatchGridDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "CARAPREV.h"
#include "CatchGridDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CCatchGridDlg 


CCatchGridDlg::CCatchGridDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCatchGridDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCatchGridDlg)
	m_fGridstepX = 0.0f;
	m_fGridstepY = 0.0f;
	m_fGridOrgX = 0.0f;
	m_fGridOrgY = 0.0f;
	//}}AFX_DATA_INIT
}


void CCatchGridDlg::DoDataExchange(CDataExchange* pDX)
{
   double dTemp;
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCatchGridDlg)
	//}}AFX_DATA_MAP

   dTemp = m_fGridstepX;
   DDX_TextVar(pDX, IDC_CATCHGRID_STEP_X, FLT_DIG+1, dTemp);
   m_fGridstepX = (float)dTemp;
	DDV_MinMaxFloat(pDX, m_fGridstepX, 0.f, 50.f);

   dTemp = m_fGridstepY;
   DDX_TextVar(pDX, IDC_CATCHGRID_STEP_Y, FLT_DIG+1, dTemp);
   m_fGridstepY = (float)dTemp;
	DDV_MinMaxFloat(pDX, m_fGridstepY, 0.f, 50.f);

   dTemp = m_fGridOrgX;
   DDX_TextVar(pDX, IDC_CATCHGRID_ORG_X, FLT_DIG+1, dTemp);
   m_fGridOrgX = (float)dTemp;
	DDV_MinMaxFloat(pDX, m_fGridOrgX, 0.f, 50.f);

   dTemp = m_fGridOrgY;
   DDX_TextVar(pDX, IDC_CATCHGRID_ORG_Y, FLT_DIG+1, dTemp);
   m_fGridOrgY = (float)dTemp;
	DDV_MinMaxFloat(pDX, m_fGridOrgY, 0.f, 50.f);
}


BEGIN_MESSAGE_MAP(CCatchGridDlg, CDialog)
	//{{AFX_MSG_MAP(CCatchGridDlg)
		// HINWEIS: Der Klassen-Assistent fügt hier Zuordnungsmakros für Nachrichten ein
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CCatchGridDlg 
