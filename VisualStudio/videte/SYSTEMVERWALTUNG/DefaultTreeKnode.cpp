// DefaultTreeKnode.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "DefaultTreeKnode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDefaultTreeKnode dialog


CDefaultTreeKnode::CDefaultTreeKnode(CSVView* pParent)
	: CSVPage(CDefaultTreeKnode::IDD)
{
	m_pParent			= pParent;
	//{{AFX_DATA_INIT(CDefaultTreeKnode)
	m_strDscr = _T("");
	//}}AFX_DATA_INIT
   m_strGroup = _T("");
}

void CDefaultTreeKnode::Create()
{
   CSVPage::Create(IDD, (CWnd*)m_pParent);
}

void CDefaultTreeKnode::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDefaultTreeKnode)
	//}}AFX_DATA_MAP
   if (!pDX->m_bSaveAndValidate)
   {
      CString str = _T("");
	  DDX_Text(pDX, IDC_DEF_TEXT2, m_strDscr);
      for (int i=0; i<MAX_CHILDITEMS; i++)
      {
         if (m_strItem[i].IsEmpty())
         {
				GetDlgItem(IDC_DEF_ITEM0+i)->EnableWindow(false);
				DDX_Text(pDX, IDC_DEF_ITEM0+i, str);
         }
         else
         {
            GetDlgItem(IDC_DEF_ITEM0+i)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_STATIC_PT1+i)->ShowWindow(SW_SHOW);
            DDX_Text(pDX, IDC_DEF_ITEM0+i, m_strItem[i]);
         }
      }
	   DDX_Text(pDX, IDC_DEF_GROUP, m_strGroup);
   }
}

IMPLEMENT_DYNCREATE(CDefaultTreeKnode, CSVPage)

BEGIN_MESSAGE_MAP(CDefaultTreeKnode, CSVPage)
	//{{AFX_MSG_MAP(CDefaultTreeKnode)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDefaultTreeKnode message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CDefaultTreeKnode::OnInitDialog() 
{
	CSVPage::OnInitDialog();
	CWnd *pWnd;
	for (int i=0; i<MAX_CHILDITEMS; i++)
	{
		pWnd = GetDlgItem(IDC_DEF_ITEM0+i);
		if (pWnd && pWnd->IsWindowEnabled())
		{
			::SetWindowLong(pWnd->m_hWnd, GWL_USERDATA, ::SetWindowLong(pWnd->m_hWnd, GWL_WNDPROC, (long)CDefaultTreeKnode::SubClassProc));
		}
	}
	CFont *pFont = GetFont();
	if (CSkinDialog::UseGlobalFonts())
	{
		pFont = CSkinDialog::GetDialogItemGlobalFont();
	}
	if (pFont)
	{
		LOGFONT lf;
		pFont->GetLogFont(&lf);
		lf.lfUnderline = 1;
		m_FontSelected.CreateFontIndirect(&lf);
	}
   return TRUE;
}
/*
BOOL CDefaultTreeKnode::CanNew()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDefaultTreeKnode::CanDelete()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CDefaultTreeKnode::SaveChanges()
{
}
/////////////////////////////////////////////////////////////////////////////
void CDefaultTreeKnode::CancelChanges()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDefaultTreeKnode::IsDataValid()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
*/
LRESULT CALLBACK CDefaultTreeKnode::SubClassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   long lValue  = ::GetWindowLong(hwnd, GWL_USERDATA);
//   long lIDctrl = ::GetWindowLong(hwnd, GWL_ID);
   ASSERT(lValue != 0);
   switch (uMsg)
   {
      case WM_MOUSEMOVE:
      {
         HWND hwndParent     = ::GetParent(hwnd);
         HWND hwndCurrentHit = (HWND) ::GetWindowLong(hwndParent, GWL_USERDATA);
         if (hwnd != hwndCurrentHit)
         {
            CWnd *pWnd = CWnd::FromHandle(hwndParent);
            ASSERT_KINDOF(CDefaultTreeKnode, pWnd);
            ::SendMessage(hwnd, WM_SETFONT, (WPARAM) HFONT(((CDefaultTreeKnode*)pWnd)->m_FontSelected), 1);
            TRACKMOUSEEVENT tme;
            tme.cbSize      = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags     = TME_LEAVE;
            tme.hwndTrack   = hwnd;
            tme.dwHoverTime = HOVER_DEFAULT;
            _TrackMouseEvent(&tme);
            ::SetWindowLong(hwndParent, GWL_USERDATA, (long)hwnd);
         }
      } break;
      case WM_SETFOCUS:
      {
         ::SetFocus(::GetParent(hwnd));
      }break;
      case WM_LBUTTONUP:
      {
         TCHAR szText[64];
         ::GetWindowText(hwnd, szText, 64);
         ::SendMessage(::GetParent(::GetParent(hwnd)), WM_SELECT_PAGE, (WPARAM)szText, SELECT_FROM_STRING);
         return 1;
      } break;
      case WM_LBUTTONDOWN:
      {
         return 1;
      }
      case WM_SETCURSOR:
      {
         HCURSOR hC  = AfxGetApp()->LoadStandardCursor(MAKEINTRESOURCE(32649));
         if (!hC) hC = AfxGetApp()->LoadStandardCursor(IDC_UPARROW);
         ::SetCursor(hC);
         return TRUE;
      }
      case WM_MOUSELEAVE:
      {
         HWND  hwndCurrentHit = ::GetParent(hwnd);
		 HFONT hFont = (HFONT)::SendMessage(hwndCurrentHit, WM_GETFONT, 0, 0);
		 if (CSkinDialog::UseGlobalFonts())
		 {
			 hFont = (HFONT)CSkinDialog::GetDialogItemGlobalFont()->GetSafeHandle();
		 }
         ::SetWindowLong(hwndCurrentHit, GWL_USERDATA, (long)hwndCurrentHit);
         ::SendMessage(hwnd, WM_SETFONT, (WPARAM) hFont, 1);
         TRACKMOUSEEVENT tme;
         tme.cbSize      = sizeof(TRACKMOUSEEVENT);
         tme.dwFlags     = TME_LEAVE;
         tme.hwndTrack   = hwndCurrentHit;
         tme.dwHoverTime = HOVER_DEFAULT;
         _TrackMouseEvent(&tme);
      } break;
      case WM_DESTROY:
         ::SetWindowLong(hwnd, GWL_WNDPROC, lValue);
         break;
   }
   return ::CallWindowProc((WNDPROC)lValue, hwnd, uMsg, wParam, lParam);
}

HBRUSH CDefaultTreeKnode::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CSVPage::OnCtlColor(pDC, pWnd, nCtlColor);
	if (pWnd->m_hWnd == (HWND)GetWindowLong(m_hWnd, GWL_USERDATA))
   {
      pDC->SetTextColor(RGB(255,0,0));
   }
	return hbr;
}
