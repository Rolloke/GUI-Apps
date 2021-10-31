// LineListBox.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "LineListBox.h"
#include "resource.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLineListBox

CLineListBox::CLineListBox()
{
   CListBox();
}

CLineListBox::~CLineListBox()
{
}


BEGIN_MESSAGE_MAP(CLineListBox, CListBox)
	//{{AFX_MSG_MAP(CLineListBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CLineListBox 

void CLineListBox::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
   if (lpDIS->CtlType != ODT_LISTBOX) return;
   if (lpDIS->CtlID   != IDC_FRAME_LINESTYLE) return;

   switch(lpDIS->itemAction)
   {
      case ODA_DRAWENTIRE:
         if (lpDIS->itemState & ODS_SELECTED) DrawSelect(lpDIS);
         if (lpDIS->itemState & ODS_FOCUS   ) DrawFocus( lpDIS);
         DrawLine(lpDIS);
         break;
      case ODA_FOCUS:
         DrawFocus(lpDIS);
         break;
         
      case ODA_SELECT:
         DrawSelect(lpDIS);
         break;
   }
}

void CLineListBox::DrawLine(LPDRAWITEMSTRUCT lpDIS)
{
   int x, y;
   LOGPEN   lp = {0, {1,1}, 0};
   HPEN     hpen, oldpen;

   lp.lopnStyle = lpDIS->itemData;
   hpen = CreatePenIndirect(&lp);
   oldpen = (HPEN) SelectObject(lpDIS->hDC, hpen);
   x = lpDIS->rcItem.left+2;
   y = (lpDIS->rcItem.top + lpDIS->rcItem.bottom) / 2;
   MoveToEx(lpDIS->hDC, x, y, NULL);
   x = lpDIS->rcItem.right-2;
   LineTo(lpDIS->hDC, x, y);
   SelectObject(lpDIS->hDC, oldpen);
   DeleteObject(hpen);
}

void CLineListBox::DrawFocus(LPDRAWITEMSTRUCT lpDIS)
{
   if (lpDIS->itemState & ODS_FOCUS) SelectObject(lpDIS->hDC, GetStockObject(BLACK_PEN));
   else                              SelectObject(lpDIS->hDC, GetStockObject(WHITE_PEN));
 
   SelectObject(lpDIS->hDC, GetStockObject(HOLLOW_BRUSH));

   Rectangle(lpDIS->hDC, lpDIS->rcItem.left+1, lpDIS->rcItem.top+1, lpDIS->rcItem.right-1, lpDIS->rcItem.bottom-1);
}

void CLineListBox::DrawSelect(LPDRAWITEMSTRUCT lpDIS)
{
   if (lpDIS->itemState & ODS_SELECTED) SelectObject(lpDIS->hDC, GetStockObject(BLACK_PEN));
   else                                 SelectObject(lpDIS->hDC, GetStockObject(WHITE_PEN));

   SelectObject(lpDIS->hDC, GetStockObject(HOLLOW_BRUSH));

   Rectangle(lpDIS->hDC, lpDIS->rcItem.left, lpDIS->rcItem.top, lpDIS->rcItem.right, lpDIS->rcItem.bottom);
}


/*
typedef struct tagLOGBRUSH { // lb 
   UINT     lbStyle; 
   COLORREF lbColor; 
   LONG     lbHatch; 
} LOGBRUSH; 
*/
/*

    UINT     lopnStyle; 
    POINT    lopnWidth; 
    COLORREF lopnColor; 

typedef struct tagDRAWITEMSTRUCT {
    UINT   CtlType;
    UINT   CtlID;
    UINT   itemID;
    UINT   itemAction;
    UINT   itemState;
    HWND   hwndItem;
    HDC    hDC;
    RECT   rcItem;
    DWORD  itemData;
} DRAWITEMSTRUCT;*/


