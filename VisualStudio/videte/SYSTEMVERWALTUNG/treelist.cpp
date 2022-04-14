/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: treelist.cpp $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/treelist.cpp $
// CHECKIN:		$Date: 31.07.97 13:19 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 29.07.97 14:13 $
// BY AUTHOR:	$Author: Uwe $
// $Nokeywords:$

#include "stdafx.h"
#include "treelist.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeList

CTreeList::CTreeList()
{
}

CTreeList::~CTreeList()
{
}


BEGIN_MESSAGE_MAP(CTreeList, CTreeCtrl)
	//{{AFX_MSG_MAP(CTreeList)
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTreeList message handlers

void CTreeList::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	HWND hWndScroll = 0;
	if (pScrollBar)
		hWndScroll = pScrollBar->m_hWnd;
	::PostMessage(GetParent()->m_hWnd, WM_VSCROLL, MAKELONG(nSBCode, nPos), (LPARAM)hWndScroll);	
	CTreeCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}
