#include "stdafx.h"
#include "IdipClient.h"
#include "ButtonUpDown.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CButtonUpDown
CButtonUpDown::CButtonUpDown()
{
}
/////////////////////////////////////////////////////////////////////////////
CButtonUpDown::~CButtonUpDown()
{
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CButtonUpDown, CSkinButton)
	//{{AFX_MSG_MAP(CButtonUpDown)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CButtonUpDown message handlers
void CButtonUpDown::OnLButtonDown(UINT nFlags, CPoint point) 
{
	WPARAM wParam = MAKELONG((WORD)GetDlgCtrlID(),(WORD)BN_DOWN); // low, high
	CSkinButton::OnLButtonDown(nFlags, point);
	GetParent()->PostMessage(WM_COMMAND,wParam,(LPARAM)m_hWnd);
}
/////////////////////////////////////////////////////////////////////////////
void CButtonUpDown::OnLButtonUp(UINT nFlags, CPoint point) 
{
	WPARAM wParam = MAKELONG((WORD)GetDlgCtrlID(),(WORD)BN_UP); // low, high
	CSkinButton::OnLButtonUp(nFlags, point);
	GetParent()->PostMessage(WM_COMMAND,wParam,(LPARAM)m_hWnd);
}
