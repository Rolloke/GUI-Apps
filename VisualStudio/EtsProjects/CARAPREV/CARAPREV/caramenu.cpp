// custmenu.cpp : custom menu
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1998 Microsoft Corporation
// All rights reserved.
//

#include "stdafx.h"
#include "caramenu.h"
#include "CaraToolbar.h"
#include "CaraWinApp.h"
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
#ifdef ETSDEBUG_REPORT
 #define  ETSDEBUG_INCLUDE 
#endif
#include "CEtsDebug.h"

COLORREF CCaraMenu::gm_cHiLight     = RGB(255,   0,   0);
COLORREF CCaraMenu::gm_cHiLightText = RGB(255, 255, 255);
COLORREF CCaraMenu::gm_cMenu        = RGB(192, 192, 192);
COLORREF CCaraMenu::gm_cMenuText    = RGB(  0,   0,   0);
COLORREF CCaraMenu::gm_cBtnHiLight  = RGB(212, 212, 212);
COLORREF CCaraMenu::gm_cBtnShadow   = RGB(128, 128, 128);
COLORREF CCaraMenu::gm_cGrayedText  = RGB(200, 200, 200);
HBITMAP  CCaraMenu::gm_hbmpBullet   = NULL;

CSize    CCaraMenu::gm_szBmp        = CSize(15, 16);

LOGFONT  CCaraMenu::gm_lfMenuFont;
int      CCaraMenu::gm_nOwnerdraw   = 100;

extern "C" HINSTANCE g_hCaraPreviewInstance;

CCaraMenu::CCaraMenu()
{
//	VERIFY(CreateMenu());
   m_nTabItemWidth = 0;
   m_pMenuPt = NULL;
   if (gm_hbmpBullet == NULL)
   {
      gm_hbmpBullet = (HBITMAP) ::LoadImage(g_hCaraPreviewInstance, MAKEINTRESOURCE(IDB_BULLET), IMAGE_BITMAP, 0,0, LR_DEFAULTCOLOR);
   }
}

CCaraMenu::~CCaraMenu()
{
   if (m_pMenuPt)
   {
      if (!m_pMenuPt->m_MenuBitmaps.IsEmpty())
      {
         CBitmap * pBit;
         while (m_pMenuPt->m_MenuBitmaps.GetHeadPosition())
         {
            pBit = (CBitmap*) m_pMenuPt->m_MenuBitmaps.RemoveHead();
            pBit->DeleteObject();
            delete pBit;
         }
      }
      if (!m_pMenuPt->m_Menus.IsEmpty())
      {
         CCaraMenu* pMenu;
         while (m_pMenuPt->m_Menus.GetHeadPosition())
         {
            pMenu = (CCaraMenu*) m_pMenuPt->m_Menus.RemoveHead();
            pMenu->Detach();
            delete pMenu;
         }
      }
      delete m_pMenuPt;
   }
	Detach();
   DeleteStrings();   
	ASSERT(m_hMenu == NULL);    // default CMenu::~CMenu will destroy
}

void CCaraMenu::SysColorChange()
{
   gm_cHiLight     = ::GetSysColor(COLOR_HIGHLIGHT);
   gm_cHiLightText = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
   gm_cMenu        = ::GetSysColor(COLOR_MENU);
   gm_cMenuText    = ::GetSysColor(COLOR_MENUTEXT);
   gm_cBtnShadow   = ::GetSysColor(COLOR_BTNSHADOW);
   gm_cBtnHiLight  = ::GetSysColor(COLOR_BTNHIGHLIGHT);
   gm_cGrayedText  = ::GetSysColor(COLOR_GRAYTEXT);
   NONCLIENTMETRICS ncm;
   ncm.cbSize = sizeof(NONCLIENTMETRICS);
   ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
   gm_lfMenuFont = ncm.lfMenuFont;
   HDC   hdc     = ::GetDC(NULL);
   HFONT hFont   = ::CreateFontIndirect(&gm_lfMenuFont);
   HFONT hOldF   = (HFONT) ::SelectObject(hdc, hFont);
   TEXTMETRIC tm;
   GetTextMetrics(hdc, &tm);
   ::SelectObject(hdc, hOldF);
   ::DeleteObject(hFont);
   ::ReleaseDC(NULL, hdc);
   gm_lfMenuFont.lfWidth = tm.tmAveCharWidth;
}

void CCaraMenu::AppendODMenuItem(UINT nID, const TCHAR *pszText, CBitmap *pBit)
{
   if (CCaraMenu::gm_nOwnerdraw)
   {
      if (AppendMenu(MF_ENABLED|MF_OWNERDRAW, nID, (LPCTSTR) GetStringCount()))
      {
         AddString(pszText);
         if (pBit)
         {
            SetMenuItemBitmaps(nID, MF_BYCOMMAND, pBit, NULL);
            if (AllocMenuPointers())
            {
               if (m_pMenuPt->m_MenuBitmaps.Find(pBit) == NULL)
               {
                  m_pMenuPt->m_MenuBitmaps.AddTail(pBit);
               }
            }
         }
      }
   }
   else
   {
      ::InsertMenu(m_hMenu, nID, MF_BYCOMMAND|MF_STRING, nID, pszText);
   }
}

void CCaraMenu::SetMenuBitmaps(CPtrList*pTB, CPtrList*pM, CPtrList* pMB, CMenu *pMenu, CSize szBmp)
{
   if (pMenu)
   {
      int i, count = pMenu->GetMenuItemCount();
      int k=0;
      for (i=0; i<count; i++)
      {
         CMenu * pSubMenu = pMenu->GetSubMenu(i);
         if (pSubMenu)
         {
            CCaraMenu *pCM = new CCaraMenu;
            pM->AddTail((void*)pCM);
            pCM->Attach(pSubMenu->Detach());
            SetMenuBitmaps(pTB, pM, pMB, pCM, szBmp);
         }
         else
         {
            UINT nItemID = pMenu->GetMenuItemID(i);
            if (nItemID == 0) continue;

            CString string;
            pMenu->GetMenuString(i, string, MF_BYPOSITION);
            pMenu->ModifyMenu(i, MF_BYPOSITION|MF_OWNERDRAW, nItemID, (LPCTSTR)k++);
            ((CCaraMenu*)pMenu)->AddString(string);
            if (pTB && pMB)
            {
               CBitmap * pBit;
               POSITION  pos = pTB->GetHeadPosition();
               while (pos)
               {
                  CCaraToolbar *pCTB = (CCaraToolbar*) pTB->GetNext(pos);
                  if (pCTB)
                  {
                     pBit = pCTB->GetButtonBitmap(nItemID, szBmp);
                     if (pBit)
                     {
                        pMB->AddTail((void*)pBit);
                        pMenu->SetMenuItemBitmaps(i, MF_BYPOSITION, pBit, NULL);
                     }
                  }
               }
            }
         }
      }      
   }   
}

void CCaraMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
   int nTabItemWidth = 0;
   int nItemWidth    = 0;
   TCHAR *pszText     = NULL;
   //POSITION pos      = m_Strings.FindIndex(lpMIS->itemData);
   //if (pos) pszText  = (TCHAR*) m_Strings.GetAt(pos);

   if (lpMIS->itemData < (DWORD)m_Strings.GetSize())
   {
      pszText = (LPTSTR)LPCTSTR(m_Strings.GetAt(lpMIS->itemData));
   }
   RECT rect = {0,0,0,0};
   if (pszText)
   {
      HDC   hdc   = ::GetDC(NULL);
      HFONT hFont = ::CreateFontIndirect(&gm_lfMenuFont);
      ::SaveDC(hdc);
      ::SelectObject(hdc, hFont);
      ::SetTextAlign(hdc, TA_TOP|TA_LEFT|TA_NOUPDATECP);
      int i, nLen       = 0;
      for (i=0; pszText[i] != 0; i++)
      {
         if (pszText[i] == '\t')
         {
            break;
         }
      }

      if (pszText[i] == '\t')
      {
         ::DrawText(hdc, &pszText[i+1], _tcsclen(&pszText[i+1]), &rect, DT_CALCRECT);
         nTabItemWidth = rect.right;
      }
      ::DrawText(hdc, pszText, i, &rect, DT_CALCRECT);
      nItemWidth = rect.right;

      if (abs(rect.bottom) > gm_szBmp.cy)
   	   lpMIS->itemHeight = abs(rect.bottom) + 4;
      else
	      lpMIS->itemHeight = gm_szBmp.cy + 4;

      ::RestoreDC(hdc, -1);
      ::ReleaseDC(NULL, hdc);
      ::DeleteObject(hFont);
   }

   m_nTabItemWidth = max(nTabItemWidth, m_nTabItemWidth);
   lpMIS->itemWidth = nItemWidth + nTabItemWidth + gm_szBmp.cx + gm_lfMenuFont.lfWidth * 4;
   if (gm_nOwnerdraw != 100) lpMIS->itemWidth = MulDiv(lpMIS->itemWidth, gm_nOwnerdraw, 100);
}

void CCaraMenu::AddString(const TCHAR *psz)
{
   if (psz)
   {
      m_Strings.Add(psz);
      //TCHAR * pszText = new TCHAR[_tcsclen(psz)+1];
      //strcpy(pszText, psz);
      //m_Strings.AddTail((void*)pszText);
   }
}

void CCaraMenu::DeleteStrings()
{
   m_Strings.RemoveAll();
   //if (!m_Strings.IsEmpty())
   //{
   //   while (m_Strings.GetHeadPosition())
   //   {
   //      TCHAR *pText = (TCHAR*) m_Strings.RemoveHead();
   //      if (pText) delete[] pText;
   //   }
   //}
}
void CCaraMenu::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
   TCHAR *pszText    = NULL;
   //POSITION pos     = m_Strings.FindIndex(lpDIS->itemData);
   //if (pos) pszText = (TCHAR*) m_Strings.GetAt(pos);
   if (lpDIS->itemData < (DWORD)m_Strings.GetSize())
   {
      pszText = (LPTSTR)LPCTSTR(m_Strings.GetAt(lpDIS->itemData));
   }
   if (!pszText) return;
   MENUITEMINFO mii = {sizeof(MENUITEMINFO), MIIM_DATA|MIIM_TYPE|MIIM_CHECKMARKS|MIIM_ID|MIIM_STATE|MIIM_SUBMENU, 0, 0,0,NULL, NULL, NULL, 0, NULL, 0};
   GetMenuItemInfo(lpDIS->itemID, &mii);
   bool bSelected = (lpDIS->itemState & ODS_SELECTED) ? true : false;
   bool bChecked  = (mii.fState & MFS_CHECKED ) ? true : false;
   bool bGrayed   = (mii.fState & MFS_GRAYED  ) ? true : false;
   bool bDisabled = (mii.fState & MFS_DISABLED) ? true : false;
   lpDIS->rcItem.left += gm_szBmp.cx  + 4;
   int nTextLeft = lpDIS->rcItem.left + 2;
   int nTextTop  = lpDIS->rcItem.top;
   if (gm_szBmp.cy > abs(gm_lfMenuFont.lfHeight)) nTextTop += ((gm_szBmp.cy - abs(gm_lfMenuFont.lfHeight)) >> 1);

   if ((mii.hbmpUnchecked == NULL) && !bChecked) lpDIS->rcItem.left = 0;

   if (lpDIS->itemAction & (ODA_DRAWENTIRE|ODA_SELECT))
	{
      COLORREF cMenu = (bSelected) ? gm_cHiLight : gm_cMenu;
      CBrush br(cMenu);
		pDC->FillRect(&lpDIS->rcItem, &br);
      pDC->SetBkColor(cMenu);
      COLORREF cText = gm_cMenuText;
      UINT nFlags = 0;
      if (bDisabled)
      {
         nFlags |= DSS_DISABLED;
      }
      else
      {
         nFlags |= DSS_NORMAL;
         if (bSelected) cText = gm_cHiLightText;
      }
      pDC->SetTextColor(cText);

      CRect rect(lpDIS->rcItem);
      rect.left = nTextLeft;
      rect.top  = nTextTop;
	  int i;
      for (i=0; pszText[i] != 0; i++)
      {
         if (pszText[i] == '\t') break;
      }
      if (pszText[i] == '\t')
      {
         pDC->DrawState(CPoint(rect.left, rect.top), rect.Size(), pszText, nFlags, true, i, (HBRUSH)NULL);
         rect.left = lpDIS->rcItem.right-m_nTabItemWidth-gm_lfMenuFont.lfWidth*2;
         pDC->DrawState(CPoint(rect.left, rect.top), rect.Size(), &pszText[i+1], nFlags, true, 0, (HBRUSH)NULL);
      }
      else
      {
         pDC->DrawState(CPoint(rect.left, rect.top), rect.Size(), pszText, nFlags, true, i, (HBRUSH)NULL);
      }
      rect.left   = 2;
      rect.top    = lpDIS->rcItem.top+1;
      rect.right  = gm_szBmp.cx+2;
      rect.bottom = lpDIS->rcItem.top+gm_szBmp.cy+1;
      if (mii.hbmpUnchecked) // ist eine Bitmap für diesen Menüeintrag vorhanden ?
      {
         DrawPatternedBkGnd(lpDIS->hDC, rect, !bGrayed && !bSelected && bChecked);
         DrawTransparentBmp(lpDIS->hDC, mii.hbmpUnchecked, rect.left, rect.top, gm_szBmp.cx, gm_szBmp.cy, bGrayed);
         if (!bGrayed)
         {
            InflateRect(&rect, 1, 1);
            HollowButton(lpDIS->hDC, rect, mii.fState);
         }
      }
      else if (bChecked)                                       // CheckMarke zeichnen
      {
         DrawPatternedBkGnd(lpDIS->hDC, rect, !bSelected);
//         DrawFrameControl(lpDIS->hDC, &rect, DFC_MENU, 0x0800|(mii.fType & MFT_RADIOCHECK)? DFCS_MENUBULLET:DFCS_MENUCHECK);
         
         HPEN hPen = ::CreatePen(PS_SOLID, gm_szBmp.cx>>3, gm_cMenuText);
         HPEN hOld = (HPEN)::SelectObject(lpDIS->hDC, hPen);
         if (mii.fType & MFT_RADIOCHECK)
         {
            ::SelectObject(lpDIS->hDC, GetSysColorBrush(COLOR_MENUTEXT));
            DrawTransparentBmp(lpDIS->hDC, gm_hbmpBullet, rect.left, rect.top, gm_szBmp.cx, gm_szBmp.cy, bGrayed);
/*
            Ellipse(lpDIS->hDC, MulDiv( 6, gm_szBmp.cx, 16),          // left
                                MulDiv( 5, gm_szBmp.cy, 16)+rect.top, // top
                                MulDiv(14, gm_szBmp.cx, 16),          // right
                                MulDiv(13, gm_szBmp.cy, 16)+rect.top);// bottom
*/
            HollowButton(lpDIS->hDC, rect, mii.fState);
         }
         else
         {
            MoveToEx(lpDIS->hDC, MulDiv( 6, gm_szBmp.cx, 16), rect.top+MulDiv( 8, gm_szBmp.cy, 16), NULL);
            LineTo(  lpDIS->hDC, MulDiv( 9, gm_szBmp.cx, 16), rect.top+MulDiv(11, gm_szBmp.cy, 16));
            LineTo(  lpDIS->hDC, MulDiv(14, gm_szBmp.cx, 16), rect.top+MulDiv( 6, gm_szBmp.cy, 16));
            HollowButton(lpDIS->hDC, rect, mii.fState);
         }
         SelectObject(lpDIS->hDC, hOld);
         DeleteObject(hPen);
      }
	}
}

void CCaraMenu::DrawPatternedBkGnd(HDC hDC, RECT &rect, bool bPatterned)
{
   HBITMAP hMask;
   unsigned char bits[32];
   for (int i=0; i<32; i++)
      bits[i] = (unsigned char) (i&0x2) ? 0xAA:0x55;
   BITMAP  bmp = {0, 16, 16, 2, 1, 1, bits};
   hMask = ::CreateBitmapIndirect(&bmp);
   LOGBRUSH lb     = {BS_PATTERN, 0, (long)hMask};
   HBRUSH   hBr    = ::CreateBrushIndirect(&lb);
   ::SetTextColor(hDC, gm_cMenu);
   ::SetBkColor(hDC, (bPatterned) ? gm_cBtnHiLight : gm_cMenu);
   HBRUSH hOldBr = (HBRUSH) ::SelectObject(hDC, hBr);
   ::PatBlt(hDC, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, PATCOPY);
   if (hOldBr) ::SelectObject(hDC, hOldBr);
   ::DeleteObject(hMask);
   ::DeleteObject(hBr);
}

void CCaraMenu::HollowButton(HDC hDC, RECT &rcButton, UINT nState)
{
   LOGPEN lp      = {PS_SOLID, {1,1}, 0};
   if      (nState & MFS_CHECKED) lp.lopnColor = gm_cBtnShadow;
   else if (nState & MFS_HILITE ) lp.lopnColor = gm_cBtnHiLight;
   else                           lp.lopnColor = gm_cMenu;

   HPEN hPen      = ::CreatePenIndirect(&lp);
   HPEN hOldPen   = (HPEN)::SelectObject(hDC, hPen);
   ::MoveToEx(hDC, rcButton.right-1, rcButton.top   , NULL);
   ::LineTo(  hDC, rcButton.left   , rcButton.top   );
   ::LineTo(  hDC, rcButton.left   , rcButton.bottom);
   ::SelectObject(hDC, hOldPen);
   ::DeleteObject(hPen);

   if      (nState & MFS_CHECKED) lp.lopnColor = gm_cBtnHiLight;
   else if (nState & MFS_HILITE ) lp.lopnColor = gm_cBtnShadow;

   hPen      = ::CreatePenIndirect(&lp);
   hOldPen   = (HPEN)::SelectObject(hDC, hPen);
   ::LineTo(  hDC, rcButton.right, rcButton.bottom);
   ::LineTo(  hDC, rcButton.right, rcButton.top);
   ::SelectObject(hDC, hOldPen);
   ::DeleteObject(hPen);
}

void CCaraMenu::DrawTransparentBmp(HDC hDC, HBITMAP hBit, int left, int top, int nWidth, int nHeight, bool bGrayed)
{
   HDC hMemDC = ::CreateCompatibleDC(hDC);
   HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC, hBit);
   int i, j;                     // Achtung das Pixel 0,0 muß dem Hintergrund entsprechen
   COLORREF color, colbtnface = ::GetPixel(hMemDC,0,0);//::GetSysColor(COLOR_BTNFACE);

   for (i=0; i<nWidth; i++)
   {
      for (j=0; j<nHeight; j++)
      {
         color = ::GetPixel(hMemDC, i, j);
         if (color != colbtnface)
         {
            if (bGrayed)
            {
               ::SetPixelV(hDC, left+i+1, top+j+1, gm_cBtnHiLight);
               ::SetPixelV(hDC, left+i, top+j, ((GetRValue(color) + GetGValue(color) + GetBValue(color)) > 400) ? gm_cBtnHiLight : gm_cBtnShadow);
            }
            else
            {
               ::SetPixelV(hDC, left+i, top+j, color);
            }
         }
         else 
            ::SetPixelV(hDC, left+i, top+j, gm_cMenu);
      }
   }

   ::SelectObject(hMemDC, hOldBmp);
   ::DeleteDC(hMemDC);
}

UINT CCaraMenu::ContextMenu(UINT nID, CPtrList *pTB, UINT nFlags, int x, int y, CWnd *pParent)
{
   UINT nReturn = 0;
   CCaraMenu    Menu;
   CMenu *pContextMenu;
   Menu.LoadMenu(nID);

   if (CCaraMenu::gm_nOwnerdraw)
   {
      if (Menu.AllocMenuPointers())
         CCaraMenu::SetMenuBitmaps(pTB, &Menu.m_pMenuPt->m_Menus, &Menu.m_pMenuPt->m_MenuBitmaps, &Menu, gm_szBmp);
   }
   pContextMenu = Menu.GetSubMenu(0);
   if (pContextMenu)
   {
      nReturn = pContextMenu->TrackPopupMenu(nFlags, x, y, pParent);
   }

   Menu.DestroyMenu();
   return nReturn;
}


void CCaraMenu::SetMenuBitmaps(CMenu *pM)
{
   if (AllocMenuPointers())
      CCaraMenu::SetMenuBitmaps(&m_pMenuPt->m_ToolBars, &m_pMenuPt->m_Menus, &m_pMenuPt->m_MenuBitmaps, pM, gm_szBmp);
}

void CCaraMenu::AddToolBar(CCaraToolbar *pTB)
{
   if (pTB && AllocMenuPointers())
   {
      m_pMenuPt->m_ToolBars.AddHead(pTB);
      CSize szTBBmp = pTB->GetImageSize();
      CSize szBmp;
      szBmp.cx = ::GetSystemMetrics(SM_CXMENUCHECK);
      szBmp.cy = ::GetSystemMetrics(SM_CYMENUCHECK);
      if ((szTBBmp.cx < szBmp.cx) || (szTBBmp.cy < szBmp.cy))
         CCaraMenu::gm_szBmp = szBmp;
      else
         CCaraMenu::gm_szBmp = szTBBmp;
   }
}

bool CCaraMenu::AllocMenuPointers()
{
   if (!m_pMenuPt)
   {
      m_pMenuPt = new MenuPointers;
   }
   return (m_pMenuPt != NULL) ? true : false;
}

CPtrList * CCaraMenu::GetToolBars()
{
   if (m_pMenuPt) return &m_pMenuPt->m_ToolBars;
   return NULL;
}
