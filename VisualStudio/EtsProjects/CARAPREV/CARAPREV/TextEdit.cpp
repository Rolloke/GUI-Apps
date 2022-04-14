// TextEdit.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "TextEdit.h"
#include "TextLabel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTextEdit 


CTextEdit::CTextEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CTextEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTextEdit)
	//}}AFX_DATA_INIT
	m_bCalcRect = FALSE;
   m_pTextLabel = NULL;
   m_fRatio = 1.0;
   m_szMin.cx = m_szMin.cy = 0;
   m_bAutoDelete = false;
}


void CTextEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextEdit)
	DDX_Control(pDX, IDC_TEXTEDIT, m_cTextEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTextEdit, CDialog)
	//{{AFX_MSG_MAP(CTextEdit)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_TEXT_CALCRECT, OnTextCalcrect)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_GETMINMAXINFO, OnGetminMaxInfo)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CTextEdit 


BOOL CTextEdit::OnInitDialog() 
{
   CDialog::OnInitDialog();
   HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
   SetIcon(hIcon, true);
   if (m_pTextLabel)
   {
      m_Font.DeleteObject();
      LOGFONT lf = m_pTextLabel->GetLogFont();
      lf.lfHeight = (int)(0.5 + m_fRatio * lf.lfHeight);
      if (lf.lfHeight > -12) lf.lfHeight = -12;
      lf.lfWidth  = 0;
      m_Font.CreateFontIndirect(&lf);
      m_cTextEdit.SetFont(&m_Font, false);
      m_cTextEdit.SetWindowText(m_pTextLabel->GetText());
   }

   CRect rcDlg, rcItem;

   GetWindowRect(&rcDlg);
   m_szMin.cx = rcDlg.right-rcDlg.left;
   m_szMin.cy = rcDlg.bottom-rcDlg.top;

   MapWindowPoints(NULL, (LPPOINT) &rcDlg, 2);

   GetDlgItem(IDC_TEXTEDIT)->GetWindowRect(&rcItem);
   MapWindowPoints(NULL, (LPPOINT) &rcItem, 2);
   m_nTextY = rcDlg.bottom - rcItem.bottom;

   GetDlgItem(IDOK)->GetWindowRect(&rcItem);
   m_szButton = rcItem.Size();
   MapWindowPoints(NULL, (LPPOINT) &rcItem, 2);
   m_ptOk.x = rcItem.left  - rcDlg.left;
   m_ptOk.y = rcDlg.bottom - rcItem.top;

   GetDlgItem(IDC_TEXT_CALCRECT)->GetWindowRect(&rcItem);
   MapWindowPoints(NULL, (LPPOINT) &rcItem, 2);
   m_ptCalcRect.x = m_szButton.cx + rcDlg.right  - rcItem.right;
   m_ptCalcRect.y = rcDlg.bottom - rcItem.top;

//   ::SetWindowPos(m_hWnd, NULL, 0,0, 200, 300, SWP_NOMOVE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CTextEdit::OnTextCalcrect() 
{
   m_bCalcRect = true;
   OnOK();
}
void CTextEdit::OnCancel() 
{
   CDialog::OnCancel();
}
void CTextEdit::OnOK() 
{
   if (m_pTextLabel)
   {
      CString str;
      m_cTextEdit.GetWindowText(str);
      m_pTextLabel->SetText((TCHAR*)LPCTSTR(str));
   }
   CDialog::OnOK();
}

void CTextEdit::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

   if (IsWindow(m_cTextEdit.m_hWnd))
   {
      m_cTextEdit.MoveWindow(0, 0, cx, cy-m_nTextY, true);
      GetDlgItem(IDOK             )->MoveWindow(m_ptOk.x           , cy - m_ptOk.y      , m_szButton.cx, m_szButton.cy, true);
      GetDlgItem(IDC_TEXT_CALCRECT)->MoveWindow(cx - m_ptCalcRect.x, cy - m_ptCalcRect.y, m_szButton.cx, m_szButton.cy, true);
   }
}

void CTextEdit::SetTextLabel(CTextLabel *ptl)
{
   m_pTextLabel = ptl;
}

HBRUSH CTextEdit::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
   if (CTLCOLOR_EDIT == nCtlColor)
   {
      if (m_pTextLabel)
      {
         pDC->SetTextColor(m_pTextLabel->GetTextColor());
      }
   }
	return hbr;
}

LRESULT CTextEdit::OnGetminMaxInfo(WPARAM wParam, LPARAM lParam)
{
   if (m_szMin.cx && m_szMin.cy)
   {
      MINMAXINFO *pMMI = (MINMAXINFO*)lParam;
      pMMI->ptMinTrackSize.x = m_szMin.cx;
      pMMI->ptMinTrackSize.y = m_szMin.cy;
   }
   return 0;
}

void CTextEdit::OnDestroy() 
{
   HICON hIcon = GetIcon(true);
   if (hIcon) DestroyIcon(hIcon);
	CDialog::OnDestroy();
   if (m_bAutoDelete)
      delete this;
}
