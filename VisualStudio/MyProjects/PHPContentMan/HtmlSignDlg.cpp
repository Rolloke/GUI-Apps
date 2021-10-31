// HtmlSignDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "PHPContentMan.h"
#include "HtmlSignDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CHtmlSignDlg 
HWND CHtmlSignDlg::gm_hwndMouseLeave = NULL;

CHtmlSignDlg::CHtmlSignDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHtmlSignDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHtmlSignDlg)
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
   m_hWndPasteTo = NULL;
   m_nCommandID  = 0;
   m_bCloseFrame = false;
   m_bTopMost    = false;
}

CHtmlSignDlg::~CHtmlSignDlg()   // StandardDestruktor
{
   if (m_System.m_hObject) m_System.DeleteObject();
}
void CHtmlSignDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHtmlSignDlg)
	DDX_Control(pDX, IDC_HTML_PASTE_COMMAND, m_cCommandList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHtmlSignDlg, CDialog)
	//{{AFX_MSG_MAP(CHtmlSignDlg)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_COPY_HTML_CODE, OnBtnCopyHtmlCode)
	ON_WM_PAINT()
	ON_WM_SYSCOMMAND()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_HTML_PASTE_COMMAND, OnSelchangeHtmlPasteCommand)
	ON_WM_INITMENUPOPUP()
	//}}AFX_MSG_MAP
   ON_MESSAGE(WM_APP+1, OnAppMessage1)
   ON_MESSAGE(WM_APP+2, OnAppMessage2)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CHtmlSignDlg 

BOOL CHtmlSignDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{	
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
   CWinApp *pApp = AfxGetApp();
   CString str, strFormat;
   int     nID, i;
   for (i=1; ; i++)
   {
      strFormat.Format("%d", i);
      str = pApp->GetProfileString("Anwendungsname", strFormat, NULL); 
      if (str.IsEmpty()) break;
      nID = pApp->GetProfileInt("PasteCmd", strFormat, -1);
      if (nID == -1) break;
      m_cCommandList.AddString(str);
      m_cCommandList.SetItemData(i, nID);
   }
   m_cCommandList.SetCurSel(0);
	// Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden
	
   CWnd *pWnd = GetDlgItem(IDC_CODE);
   if (pWnd)
   {
      CRect rcWnd, rcNew;
      CSize szWnd;
      CButton *pwndBtn;
      CFont *pFont   = GetDlgItem(IDC_HTML_CODE)->GetFont();
      LOGFONT lf;
      char szSign[4] = " ";
      int i, j, nID;
      DWORD dwStyle  = ::GetWindowLong(pWnd->m_hWnd, GWL_STYLE);
      long lData = ::SetWindowLong(pWnd->m_hWnd, GWL_USERDATA, ::SetWindowLong(pWnd->m_hWnd, GWL_WNDPROC, (LPARAM)BtnWndProc));

      if (pFont)
      {
         pFont->GetLogFont(&lf);
         lf.lfHeight -= 5;
         strcpy(lf.lfFaceName, "system");
         m_System.CreateFontIndirect(&lf);
         strcpy(lf.lfFaceName, "symbol");
         m_Symbol.CreateFontIndirect(&lf);
      }

      CPHPContentManApp::SetDirection(TEXT2HTML);
      nID = pWnd->GetDlgCtrlID();
      pWnd->GetWindowRect(&rcWnd);
      ScreenToClient(&rcWnd);
      szWnd = rcWnd.Size();
      szSign[0] = (char)228;
      pWnd->SetFont(&m_Symbol);
      pWnd->SetWindowText(szSign);

      for (i=0; i<11; i++)
      {
         for (j= (i==0)?1:0; j<10; j++)
         {
            rcNew = rcWnd;
            rcNew.OffsetRect((szWnd.cx+1)*j, (szWnd.cy+1)*i);
            nID++;
            szSign[0] = CPHPContentManApp::GetCharCode(nID-IDC_CODE-1);
            if (szSign[0] == 0) break;
            szSign[1] = 0;
            pFont = &m_System;
            switch(szSign[0])
            {
            case (char)128:
                  szSign[0] = 'e';
                  szSign[1] = 'u';
                  szSign[2] = 0;
                  break;
               case '&':
                  szSign[1] = '&';
                  szSign[2] = 0;
                  break;
               case 22:
                  szSign[0] = '"';
                  break;
               case 'ÿ':
                  szSign[0] = 'y';
                  szSign[1] = (char)168;
                  break;
               case 215:
                  pFont = &m_Symbol;
                  break;
               default : 
                  break;
            }
            pwndBtn = new CButton;
            pwndBtn->Create(szSign, dwStyle, rcNew, this, nID);
            pwndBtn->SetFont(pFont);
            long lData = ::SetWindowLong(pwndBtn->m_hWnd, GWL_USERDATA, ::SetWindowLong(pwndBtn->m_hWnd, GWL_WNDPROC, (LPARAM)BtnWndProc));
            ASSERT(lData==0);
         }
      }

   }
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

BOOL CALLBACK CHtmlSignDlg::DeleteChildren(HWND hwnd, LPARAM lParam)
{
   HWND hwndParent = (HWND) lParam;
   int nID = GetWindowLong(hwnd, GWL_ID);
   if ((nID > IDC_CODE) && (nID <= IDC_CODE + LISTHTMLSIZE))
   {
      CWnd *pWnd = CWnd::FromHandlePermanent(hwnd);
      ::SetWindowLong(hwnd, GWL_WNDPROC, ::GetWindowLong(hwnd, GWL_USERDATA));
      if (pWnd) delete pWnd;
   }
   return 1;
}

void CHtmlSignDlg::OnDestroy()
{
	CDialog::OnDestroy();
   ::EnumChildWindows(m_hWnd, DeleteChildren, (LPARAM)m_hWnd);
}

BOOL CHtmlSignDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int nID = LOWORD(wParam) - IDC_CODE-1;
	if ((nID >= -1) && (nID < LISTHTMLSIZE))
   {
      char szSign[2] = " ";
      const char * pszCode = NULL;
      if (nID == -1) pszCode = "&trade;";
      else           pszCode = CPHPContentManApp::GetHTMLCode(nID);
      GetDlgItem(IDC_HTML_CODE)->SetWindowText(pszCode);
      if      (nID ==  0) szSign[0] = (char)228;
      else if (nID == 96) szSign[0] = '"';
      else                szSign[0] = CPHPContentManApp::GetCharCode(nID);

      if (HIWORD(wParam) == BN_CLICKED)
      {
         OnBtnCopyHtmlCode();
      }
   }
	return CDialog::OnCommand(wParam, lParam);
}

void CHtmlSignDlg::OnBtnCopyHtmlCode()
{
   CEdit *pEdit = (CEdit*)GetDlgItem(IDC_HTML_CODE);
   if (pEdit)
   {
      pEdit->SetSel(0,-1);
      pEdit->Copy();
      if ((m_nCommandID != 0) && ::IsWindow(m_hWndPasteTo))
      {
         ::PostMessage(m_hWndPasteTo, WM_COMMAND, m_nCommandID, 0);
         ::SetForegroundWindow(m_hWndPasteTo);
      }
   }
}


HCURSOR CHtmlSignDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CHtmlSignDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Gerätekontext für Zeichnen

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Symbol in Client-Rechteck zentrieren
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Symbol zeichnen
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

void CHtmlSignDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
      ((CPHPContentManApp*)AfxGetApp())->OnAppAbout();
	}
	else
	{
/*
      if (nID == SC_RESTORE)
      {
         OnAppMessage1((WPARAM)::GetForegroundWindow(), 0);
      }
*/
      if (nID == SC_MINIMIZE)
      {
         CRect rc;
         ::GetWindowRect(m_hWnd, &rc);
         CSize sz = rc.Size();
         if (sz.cy != 20)
         {
            m_nOldHeight = sz.cy;
            ::SetWindowPos(m_hWnd, NULL, 0, 0, sz.cx, 20, SWP_NOMOVE|SWP_NOZORDER|SWP_NOOWNERZORDER);
         }
         else
         {
            sz.cy = m_nOldHeight;
            ::SetWindowPos(m_hWnd, NULL, 0, 0, sz.cx, sz.cy, SWP_NOMOVE|SWP_NOZORDER|SWP_NOOWNERZORDER);
         }
         return;
      }

		CDialog::OnSysCommand(nID, lParam);
	}
}

LRESULT CHtmlSignDlg::OnAppMessage1(WPARAM wParam, LPARAM lParam)
{
   HWND hwndTop = ::GetParent(m_hWnd);
   if (hwndTop == NULL) hwndTop = m_hWnd;
   HWND hwndPasteTo = (HWND)wParam;
   while (hwndPasteTo)
   {
      HWND hwndParent = ::GetParent(hwndPasteTo);
      if (hwndParent == NULL)
      {
         if (hwndTop != hwndPasteTo)
         {
            m_hWndPasteTo = hwndPasteTo;
            char szText[64];
            ::GetWindowText(m_hWndPasteTo, szText, 64);
            GetDlgItem(IDC_PASTE_WND)->SetWindowText(szText);
         }
      }
      hwndPasteTo = hwndParent;
   }
   return 0;
}


LRESULT CHtmlSignDlg::OnAppMessage2(WPARAM wParam, LPARAM lParam)
{
   OnCommand(wParam, lParam);
   return 0;
}

LRESULT CALLBACK CHtmlSignDlg::BtnWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   switch (uMsg)
   {
      case WM_MOUSEMOVE:
      {
         if (hwnd != gm_hwndMouseLeave)
         {
            HWND hwndDlg = ::GetParent(hwnd);
            ::SendMessage(hwndDlg, WM_APP+1, (WPARAM)::GetForegroundWindow(), 0);
            ::SendMessage(hwndDlg, WM_APP+2, (WPARAM)MAKELONG(::GetWindowLong(hwnd, GWL_ID), BN_HILITE), 0);

            TRACKMOUSEEVENT tme;
            tme.cbSize      = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags     = TME_LEAVE;
            tme.hwndTrack   = hwnd;
            tme.dwHoverTime = HOVER_DEFAULT;
            _TrackMouseEvent(&tme);
            gm_hwndMouseLeave = hwnd;

            long lStyle = ::GetWindowLong(hwnd, GWL_STYLE);
            lStyle &= ~BS_FLAT;
            SetWindowLong(hwnd, GWL_STYLE, lStyle);
            ::UpdateWindow(hwnd);
            ::InvalidateRect(hwnd, NULL, true);
         }
      } break;
      case WM_MOUSELEAVE:
      {
         gm_hwndMouseLeave = ::GetParent(hwnd);
         TRACKMOUSEEVENT tme;
         tme.cbSize      = sizeof(TRACKMOUSEEVENT);
         tme.dwFlags     = TME_LEAVE;
         tme.hwndTrack   = gm_hwndMouseLeave;
         tme.dwHoverTime = HOVER_DEFAULT;
         _TrackMouseEvent(&tme);

         long lStyle = ::GetWindowLong(hwnd, GWL_STYLE);
         lStyle |= BS_FLAT;
         SetWindowLong(hwnd, GWL_STYLE, lStyle);
         ::UpdateWindow(hwnd);
         ::InvalidateRect(hwnd, NULL, true);
      } break;
   }
   return ::CallWindowProc((WNDPROC)::GetWindowLong(hwnd, GWL_USERDATA), hwnd, uMsg, wParam, lParam);
}


void CHtmlSignDlg::OnSelchangeHtmlPasteCommand() 
{
   int nSel = m_cCommandList.GetCurSel();
   if (nSel != CB_ERR)
   {
      m_nCommandID = m_cCommandList.GetItemData(nSel);
   }	
}

void CHtmlSignDlg::OnCancel() 
{
   if (m_bCloseFrame)
   {
      CWnd *pWnd = AfxGetMainWnd();
      if (pWnd) pWnd->PostMessage(WM_CLOSE);
   }
	CDialog::OnCancel();
}

LRESULT CHtmlSignDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
/*
   if	(message == WM_ACTIVATE)
   {
      OnAppMessage1((WPARAM)::GetForegroundWindow(), 0);
   }
*/
	return CDialog::WindowProc(message, wParam, lParam);
}

void CHtmlSignDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
	CDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
}
