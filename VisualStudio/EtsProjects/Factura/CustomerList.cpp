// CustomerList.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Factura.h"
#include "CustomerList.h"
#include "FacturaDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustomerList

CCustomerList::CCustomerList()
{
   m_nValue = 0;
}

CCustomerList::~CCustomerList()
{
}


BEGIN_MESSAGE_MAP(CCustomerList, CListCtrl)
	//{{AFX_MSG_MAP(CCustomerList)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CCustomerList 
/*
void CCustomerList::DrawItem(LPDRAWITEMSTRUCT pDis)
{
   return;
}
*/
void CCustomerList::OnLButtonDown(UINT nFlags, CPoint point) 
{
/*
   LVHITTESTINFO lvHtI = {{point.x,point.y}, 0, 0, 0};
   SendMessage(LVM_SUBITEMHITTEST, 0, (LPARAM)&lvHtI);
   if (lvHtI.flags & LVHT_ONITEMLABEL)
   {
      SetFocus();
      UINT nBefore = GetItemState(lvHtI.iItem, LVIS_SELECTED);
      LVITEM lvItem = {LVIF_STATE|LVCF_SUBITEM, 0, 1, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED, NULL, 0, 0, NULL};
      lvItem.iItem = lvHtI.iItem;
      lvItem.iSubItem = lvHtI.iSubItem;
      SetItemState(lvItem.iItem, &lvItem);
      EnsureVisible(lvItem.iItem, false);
      if (nBefore & LVIS_SELECTED)
      {
         CWnd *pParent = GetParent();
         if (pParent)
         {
            LV_DISPINFO DispInfo;
            DispInfo.hdr.hwndFrom = m_hWnd;
            DispInfo.hdr.idFrom   = GetDlgCtrlID();
            DispInfo.hdr.code     = LVN_BEGINLABELEDIT; 
            DispInfo.item         = lvItem;
            pParent->SendMessage(WM_NOTIFY, 0, (LPARAM)&DispInfo);
         }
      }
   }
   else 
*/
   CListCtrl::OnLButtonDown(nFlags, point);
}


BOOL CCustomerList::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
   LPNMHDR  lpnmh = (LPNMHDR) lParam;
   switch(lpnmh->code)
   {
      case LVN_ODCACHEHINT:
      {
         LPNMLVCACHEHINT   lpCacheHint = (LPNMLVCACHEHINT)lParam;
         return 0;
      }
      case LVN_ODFINDITEM:
      {
         LPNMLVFINDITEM lpFindItem = (LPNMLVFINDITEM)lParam;
         return 0;
      }
   }
   return CListCtrl::OnNotify(wParam, lParam, pResult);
}

void CCustomerList::DoDataExchange(CDataExchange* pDX) 
{
   CEdit *pEdit = GetEditControl();
   if (pEdit)
   {
      DDX_Text(pDX, pEdit->GetDlgCtrlID(), m_nValue);
   }
   CListCtrl::DoDataExchange(pDX);
}
