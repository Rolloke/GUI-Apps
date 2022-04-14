// BrushListBox.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "BrushListBox.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBrushListBox

CBrushListBox::CBrushListBox()
{
}

CBrushListBox::~CBrushListBox()
{
}


BEGIN_MESSAGE_MAP(CBrushListBox, CListBox)
	//{{AFX_MSG_MAP(CBrushListBox)
		// HINWEIS - Der Klassen-Assistent fügt hier Zuordnungsmakros ein und entfernt diese.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CBrushListBox 

void CBrushListBox::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
   if (lpDIS->CtlType != ODT_LISTBOX) return;

   switch(lpDIS->itemAction)
   {
      case ODA_DRAWENTIRE:
         if (lpDIS->itemState & ODS_SELECTED) DrawSelect(lpDIS);
         if (lpDIS->itemState & ODS_FOCUS   ) DrawFocus( lpDIS);
         DrawBrush(lpDIS);
         break;
      case ODA_FOCUS:
         DrawFocus(lpDIS);
         break;
      case ODA_SELECT:
         DrawSelect(lpDIS);
         break;
   }
}

void CBrushListBox::DrawBrush(LPDRAWITEMSTRUCT lpDIS)
{
   LOGPEN   lp = {PS_SOLID, {1,1}, 0};
   HPEN     hpen, oldpen;
   LOGBRUSH lb = {BS_HOLLOW, 0, 0};
   HBRUSH   hbrush, oldbrush;

   if (lpDIS->CtlID == IDC_BRUSH_STYLE)
   {
      lb = *((LOGBRUSH*)lpDIS->itemData);
   }
   hpen = ::CreatePenIndirect(&lp);
   oldpen = (HPEN) ::SelectObject(lpDIS->hDC, hpen);
   hbrush   = ::CreateBrushIndirect(&lb);
   oldbrush = (HBRUSH) ::SelectObject(lpDIS->hDC, hbrush);
   ::Rectangle(lpDIS->hDC, lpDIS->rcItem.left+4, lpDIS->rcItem.top+4, lpDIS->rcItem.right-4, lpDIS->rcItem.bottom-4);
   ::SelectObject(lpDIS->hDC, oldbrush);
   ::DeleteObject(hbrush);
   ::SelectObject(lpDIS->hDC, oldpen);
   ::DeleteObject(hpen);
}

void CBrushListBox::DrawFocus(LPDRAWITEMSTRUCT lpDIS)
{
   if (lpDIS->itemState & ODS_FOCUS) SelectObject(lpDIS->hDC, GetStockObject(BLACK_PEN));
   else                              SelectObject(lpDIS->hDC, GetStockObject(WHITE_PEN));
 
   SelectObject(lpDIS->hDC, GetStockObject(HOLLOW_BRUSH));

   Rectangle(lpDIS->hDC, lpDIS->rcItem.left+1, lpDIS->rcItem.top+1, lpDIS->rcItem.right-1, lpDIS->rcItem.bottom-1);
}

void CBrushListBox::DrawSelect(LPDRAWITEMSTRUCT lpDIS)
{
   if (lpDIS->itemState & ODS_SELECTED) SelectObject(lpDIS->hDC, GetStockObject(BLACK_PEN));
   else                                 SelectObject(lpDIS->hDC, GetStockObject(WHITE_PEN));

   SelectObject(lpDIS->hDC, GetStockObject(HOLLOW_BRUSH));

   Rectangle(lpDIS->hDC, lpDIS->rcItem.left, lpDIS->rcItem.top, lpDIS->rcItem.right, lpDIS->rcItem.bottom);
}

/*
   UINT     lbStyle;
   COLORREF lbColor;
   LONG     lbHatch;
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

