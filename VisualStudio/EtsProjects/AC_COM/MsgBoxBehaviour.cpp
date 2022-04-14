// MsgBoxBehaviour.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "AC_COM.h"
#include "AC_COMDlg.h"
#include "MsgBoxBehaviour.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CMsgBoxBehaviour 


CMsgBoxBehaviour::CMsgBoxBehaviour(CWnd* pParent /*=NULL*/)
	: CDialog(CMsgBoxBehaviour::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMsgBoxBehaviour)
	m_nEvent   = 0;
	m_bAuto    = FALSE;
	m_bRequest = FALSE;
	//}}AFX_DATA_INIT
}
CMsgBoxBehaviour::~CMsgBoxBehaviour()
{
}

void CMsgBoxBehaviour::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMsgBoxBehaviour)
	DDX_CBIndex(pDX, IDC_MBB_COMBO, m_nEvent);
	DDX_Check(pDX, IDC_MBB_CK_AUTO, m_bAuto);
	DDX_Check(pDX, IDC_MBB_CK_REQUEST, m_bRequest);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMsgBoxBehaviour, CDialog)
	//{{AFX_MSG_MAP(CMsgBoxBehaviour)
   ON_MESSAGE(WM_EXITSIZEMOVE, OnExitSizeMove)
	ON_BN_CLICKED(IDC_MBB_CK_REQUEST, OnMbbCkRequest)
	ON_BN_CLICKED(IDC_MBB_CK_AUTO, OnMbbCkAuto)
	ON_CBN_SELCHANGE(IDC_MBB_COMBO, OnSelchangeMbbCombo)
	ON_BN_CLICKED(IDC_MBB_BTN_DEFAULT, OnMbbBtnDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CMsgBoxBehaviour 

BOOL CMsgBoxBehaviour::OnInitDialog() 
{
	CDialog::OnInitDialog();

   OnSelchangeMbbCombo();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CMsgBoxBehaviour::OnMbbCkRequest() 
{
	CDataExchange dx(this, true);
	DDX_Check(&dx, IDC_MBB_CK_REQUEST, m_bRequest);
   if (m_bRequest)
   {
      m_bAuto = 0;
   }
   SetFlags();
   dx.m_bSaveAndValidate = false;
	DDX_Check(&dx, IDC_MBB_CK_AUTO, m_bAuto);
}

void CMsgBoxBehaviour::OnMbbCkAuto() 
{
	CDataExchange dx(this, true);
	DDX_Check(&dx, IDC_MBB_CK_AUTO, m_bAuto);
   if (m_bAuto)
   {
      m_bRequest = 0;
   }
   SetFlags();
   dx.m_bSaveAndValidate = false;
	DDX_Check(&dx, IDC_MBB_CK_REQUEST, m_bRequest);
}

void CMsgBoxBehaviour::OnSelchangeMbbCombo() 
{
	CDataExchange dx(this, true);
	DDX_CBIndex(&dx, IDC_MBB_COMBO, m_nEvent);
   DWORD dwFlags = m_dwFlags;
   dwFlags >>= (m_nEvent * 4);
   m_bRequest = (dwFlags & 1) ? true : false;
   m_bAuto    = (dwFlags & 2) ? true : false;
   dx.m_bSaveAndValidate = false;
	DDX_Check(&dx, IDC_MBB_CK_AUTO, m_bAuto);
	DDX_Check(&dx, IDC_MBB_CK_REQUEST, m_bRequest);
   GetDlgItem(IDC_MBB_CK_AUTO)->EnableWindow((m_nEvent<4) ? true :  false);
}

void CMsgBoxBehaviour::SetFlags()
{
   DWORD dwFlags = 0;
   DWORD dwMask  = 0xf;

   if (m_bRequest) dwFlags |= 1;
   if (m_bAuto)    dwFlags |= 2;
   dwFlags <<= (m_nEvent * 4);
   dwMask  <<= (m_nEvent * 4);
   m_dwFlags &= ~dwMask;
   m_dwFlags |=  dwFlags;
}

void CMsgBoxBehaviour::OnMbbBtnDefault() 
{
   m_dwFlags = MBF_WARN_REQU_ALL;
   OnSelchangeMbbCombo();
}

LRESULT CMsgBoxBehaviour::OnExitSizeMove(WPARAM, LPARAM)
{
/*
   CRect rcDlg;
   CRect rcParent;
   CPoint ptDiff;
   CPoint ptDocked;
   int nWhere;
   HWND hwndParent = ::GetParent(m_hWnd);

   ::GetWindowRect(m_hWnd, &rcDlg);
   ::GetWindowRect(hwndParent, &rcParent);

   for (nWhere=0; nWhere<4; nWhere++)
   {
      switch (nWhere)
      {
         case 0:                                               // Unten links
         ptDocked = CPoint(rcParent.left, rcParent.bottom);
         ptDiff   = ptDocked - rcDlg.TopLeft();
         m_pDocked->ptOffset = CPoint(0,0);
         break;
         case 1:                                               // Oben links
         ptDocked = CPoint(rcParent.left, rcParent.top);
         ptDiff   = ptDocked - CPoint(rcDlg.left, rcDlg.bottom);
         m_pDocked->ptOffset.x = 0;
         m_pDocked->ptOffset.y = -(rcParent.Height() + rcDlg.Height());
         break;
         case 2:                                               // Seite oben links
         ptDocked = CPoint(rcParent.left, rcParent.top);
         ptDiff   = ptDocked - CPoint(rcDlg.right, rcDlg.top);
         m_pDocked->ptOffset.x = -rcDlg.Width();
         m_pDocked->ptOffset.y = -rcParent.Height();
         break;
         case 3:                                               // Seite oben rechts
         ptDocked = CPoint(rcParent.right, rcParent.top);
         ptDiff   = ptDocked - CPoint(rcDlg.left, rcDlg.top);
         m_pDocked->ptOffset.x = rcParent.Width();
         m_pDocked->ptOffset.y = -rcParent.Height();
         break;
      }
      if (((ptDiff.x * ptDiff.x) + (ptDiff.y * ptDiff.y)) < 500)
         break;
   }
   if (nWhere<4)
   {
      long lData = ::GetWindowLong(hwndParent, DWL_USER);
      if (lData == 0)
      {
         ::SetWindowLong(hwndParent, DWL_USER, (long)m_pDocked);
         m_pDocked->hwndChild = m_hWnd;
         ::SetWindowPos(m_hWnd, NULL, rcParent.left+m_pDocked->ptOffset.x, rcParent.bottom+m_pDocked->ptOffset.y, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_ASYNCWINDOWPOS);
//         ::SetWindowPos(m_hWnd, NULL, ptDocked.x, ptDocked.y, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_ASYNCWINDOWPOS);
      }
   }
   else
   {
      ::SetWindowLong(hwndParent, DWL_USER, (long)0);
   }
   */
   return 0;
}
