// CaraFrameWnd.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "CaraFrameWnd.h"
#include "CaraDoc.h"
#include "DibSection.h"
#include "PictureLabel.h"
#include "LabelContainer.h"
#include "TextLabel.h"
#include "CurveLabel.h"
#include "Resource.h"
#include "CaraToolBar.h"
#include "CaraMenu.h"
#include "CARAPREV.h"
#include "ListDlg.h"
#include "ETS3DGLRegKeys.h"
#include "CaraPreview.h"
#include "Curve.h"
#include "CaraWinApp.h"
#include "CEtsHelp.h"

//#include <_CommCtrl.h>


#ifdef ETSDEBUG_REPORT
 #define  ETSDEBUG_INCLUDE 
#endif
#include "CEtsDebug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static TCHAR THIS_FILE[] = __FILE__;
#endif

#define COMPILE_MULTIMON_STUBS
#include "MultiMon.h"

/////////////////////////////////////////////////////////////////////////////
// CCaraFrameWnd

#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

#ifndef TTS_BALLOON
   #define TTS_BALLOON 0x40
#endif
#ifndef TTM_SETTITLE
   #define TTM_SETTITLE           (WM_USER + 32)  // wParam = TTI_*, lParam = TCHAR* szTitle
#endif

IMPLEMENT_DYNAMIC(CCaraStatusBar, CStatusBar)


LRESULT CCaraStatusBar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam )
{
//   TRACE("Msg:%x\n", message);
   if (message == WM_CONTEXTMENU)
   {
      CMenu Menu, *pContextMenu;
      Menu.LoadMenu(IDR_STATUSBAR_MENU);
      pContextMenu = Menu.GetSubMenu(0);
      if (pContextMenu)
      {
         int nReturn = pContextMenu->TrackPopupMenu(0, LOWORD(lParam), HIWORD(lParam), m_pFW);
      }
   }
   return CStatusBar::WindowProc(message, wParam, lParam);
}

int CCaraStatusBar::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
   CRect rc;
   GetClientRect(&rc);
   rc.right -= 20;   // Gripper Breite
   if (rc.PtInRect(point))
   {
      int i, nWidth;
      UINT nID, nStyle;
      for (i=m_pFW->m_nActualIndicatorSize-1; i>=0; i--)
      {
         GetPaneInfo(i, nID, nStyle, nWidth);
         if (nWidth == 0) break;
         if ((point.x > rc.right-nWidth)&&(point.x < rc.right))
            break;
         rc.right -= nWidth;
      }

      if (pTI)
      {
         DWORD dwID    = i<<16;
         pTI->uId      = (UINT)m_hWnd;
         pTI->uFlags  |= TTF_IDISHWND|TTF_FROMSTATUSBAR|dwID;
         pTI->hwnd     = m_pFW->m_hWnd;
         pTI->lpszText = LPSTR_TEXTCALLBACK;
      }
      else
      {
         i = IDS_STATUS_PANE0;
      }
      return i;
   }
   return CStatusBar::OnToolHitTest(point, pTI);
}

/*
#include <SHLWAPI.H>
#define PACKVERSION(major,minor) MAKELONG(minor,major)

DWORD GetDllVersion(LPCTSTR lpszDllName)
{

    HINSTANCE hinstDll;
    DWORD dwVersion = 0;

    hinstDll = LoadLibrary(lpszDllName);
   
    if(hinstDll)
    {
        DLLGETVERSIONPROC pDllGetVersion;

        pDllGetVersion = (DLLGETVERSIONPROC) GetProcAddress(hinstDll, "DllGetVersion");

// Because some DLLs may not implement this function, you
// must test for it explicitly. Depending on the particular 
// DLL, the lack of a DllGetVersion function may
// be a useful indicator of the version.

        if(pDllGetVersion)
        {
            DLLVERSIONINFO dvi;
            HRESULT hr;

            ZeroMemory(&dvi, sizeof(dvi));
            dvi.cbSize = sizeof(dvi);

            hr = (*pDllGetVersion)(&dvi);

            if(SUCCEEDED(hr))
            {
                dwVersion = PACKVERSION(dvi.dwMajorVersion, dvi.dwMinorVersion);
            }
        }
        
        FreeLibrary(hinstDll);
    }
    return dwVersion;
}
*/
IMPLEMENT_DYNCREATE(CCaraFrameWnd, CFrameWnd)

UINT CCaraFrameWnd::gm_nPreviewIndicators[NO_OF_PREVIEW_INDICATORS] =
{
   ID_SEPARATOR,           // Statusleistenanzeige
   ID_SEPARATOR,           // Preview-Zusatzinfoanzeige 1
   ID_SEPARATOR            // Preview-Zusatzinfoanzeige 2
};

CCaraFrameWnd::CCaraFrameWnd()
{
   BEGIN("CCaraFrameWnd");
   m_nCF_LABELOBJECT      = RegisterClipboardFormat("ETSVIEWCLabel");
   m_nCF_OLD_LABELOBJECT  = RegisterClipboardFormat("CARAVIEWCLabel");
   m_bFirstTime           = true;
   m_nActualIndicatorSize = 0;
   CCaraMenu::gm_nOwnerdraw = AfxGetApp()->GetProfileInt(REGKEY_SETTINGS, REGKEY_SETTINGS_MENU, CCaraWinApp::IsOnWin98() ? 80 : SETTINGS_MENU_DEFAULT);
   m_pMainMenu            = NULL;
   m_wndStatusBar.m_pFW   = this;
   m_bForeGround          = false;
   m_hwndToolTip          = NULL;
//   CCaraMenu::gm_nOwnerdraw = false;
}

CCaraFrameWnd::~CCaraFrameWnd()
{
   BEGIN("~CCaraFrameWnd");
   if (m_pMainMenu)
      delete m_pMainMenu;
}


BEGIN_MESSAGE_MAP(CCaraFrameWnd, CFrameWnd)
   //{{AFX_MSG_MAP(CCaraFrameWnd)
   ON_WM_DESTROY()
   ON_WM_SYSCOLORCHANGE()
   ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_SELECTIV, OnUpdateEditPasteSelectiv)
   ON_COMMAND(IDX_HEAPWATCH, OnHeapwatch)
   ON_WM_CLOSE()
   ON_COMMAND(ID_VIEW_FOREGROUND, OnViewForeground)
   ON_UPDATE_COMMAND_UI(ID_VIEW_FOREGROUND, OnUpdateViewForeground)
   ON_WM_TIMER()
   ON_WM_CREATE()
   ON_WM_MOUSEMOVE()
   ON_COMMAND(ID_VIEW_BALOON_TOOLTIPS, OnViewBaloonTooltips)
   ON_UPDATE_COMMAND_UI(ID_VIEW_BALOON_TOOLTIPS, OnUpdateViewBaloonTooltips)
   ON_WM_MENUSELECT()
   //}}AFX_MSG_MAP
   // Globale Hilfebefehle
   ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
   ON_COMMAND(ID_HELP_FINDER, CFrameWnd::OnHelpFinder)
   ON_COMMAND(ID_HELP, CFrameWnd::OnHelp)
   ON_COMMAND(ID_CONTEXT_HELP, CFrameWnd::OnContextHelp)
   ON_COMMAND(ID_DEFAULT_HELP, CFrameWnd::OnHelpFinder)
   ON_MESSAGE(WM_SETTEXT, OnSetText)
   ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
   ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CCaraFrameWnd 
/*
   ON_MESSAGE(WM_DRAWITEM, OnDrawItem)
   ON_MESSAGE(WM_MEASUREITEM, OnMeasureItem)

LRESULT CCaraFrameWnd::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
   if(wParam==NULL)                              // handelt es sich um ein Menu ?
   {
//      CToolBar::DrawMenuBitmaps((DRAWITEMSTRUCT *) lParam);
   }
   return 0;
}

LRESULT CCaraFrameWnd::OnMeasureItem(WPARAM wParam, LPARAM lParam)
{
   if(wParam==NULL)                              // handelt es sich um ein Menu ?
   {
//      CToolBar::MeasureItem((MEASUREITEMSTRUCT *) lParam);
   }
   Default();
}
*/
void CCaraFrameWnd::ActivateFrame(int nCmdShow)
{
   BEGIN("ActivateFrame");
   if (m_bFirstTime)
   {
      SFrameSettings    Frame;
      WINDOWPLACEMENT   wndpl;

      m_bFirstTime = false;
      BYTE * byteptr = NULL;
      UINT   size    = 0;
      AfxGetApp()->GetProfileBinary(REGKEY_DEFAULT, REGKEY_DEFAULT_FRAME, &byteptr, &size);
      if (byteptr)
      {
         if (size == sizeof(SFrameSettings))
         {
            memcpy(&Frame, byteptr, sizeof(SFrameSettings));
         }
         delete[] byteptr;
         wndpl.rcNormalPosition = Frame.rcFrame;
         if (Frame.bIconic)
         {
            wndpl.showCmd = SW_SHOWMINNOACTIVE;
            if (Frame.bMaximized) wndpl.flags = WPF_RESTORETOMAXIMIZED;
            else                  wndpl.flags = WPF_SETMINPOSITION;
         }
         else
         {
            if (Frame.bMaximized)
            {
               wndpl.showCmd = SW_NORMAL;
               wndpl.flags   = WPF_RESTORETOMAXIMIZED;
            }
            else
            {
               wndpl.showCmd = SW_NORMAL;
               wndpl.flags   = WPF_SETMINPOSITION;
            }
         }
         wndpl.length  = sizeof(WINDOWPLACEMENT);
         wndpl.ptMinPosition = CPoint(0,0);
         wndpl.ptMaxPosition = CPoint(-::GetSystemMetrics(SM_CXBORDER),
                                      -::GetSystemMetrics(SM_CYBORDER));
         SetWindowPlacement(&wndpl);
         if (Frame.bMaximized)
         {
            wndpl.showCmd = SW_SHOWMAXIMIZED;
            SetWindowPlacement(&wndpl);
         }
         nCmdShow = wndpl.showCmd;
      }
      CDockState state;
      state.LoadState(REGKEY_CONTROLBARS);
      for (int i = 0; i < state.m_arrBarInfo.GetSize(); i++)
      {
         CControlBarInfo* pInfo = (CControlBarInfo*)state.m_arrBarInfo[i];
         if (!GetControlBar(pInfo->m_nBarID))
         {
            ASSERT(FALSE);
            return;
         }
      }
      SetDockState(state);
   }
   CFrameWnd::ActivateFrame(nCmdShow);
}

void CCaraFrameWnd::OnDestroy() 
{
   BEGIN("OnDestroy");
   SFrameSettings    Frame;
   WINDOWPLACEMENT   wndpl;
   wndpl.length = sizeof(WINDOWPLACEMENT);
   GetWindowPlacement(&wndpl);
   Frame.rcFrame = wndpl.rcNormalPosition;
   if (wndpl.showCmd == SW_SHOWNORMAL)
   {
      Frame.bIconic    = false;
      Frame.bMaximized = false;
   }
   else if (wndpl.showCmd == SW_SHOWMAXIMIZED)
   {
      Frame.bIconic    = false;
      Frame.bMaximized = true;
   }
   else if (wndpl.showCmd == SW_SHOWMINIMIZED)
   {
      Frame.bIconic    = true;
      if (wndpl.flags) Frame.bMaximized = true;
      else             Frame.bMaximized = false;
   }

   AfxGetApp()->WriteProfileBinary(REGKEY_DEFAULT, REGKEY_DEFAULT_FRAME, (BYTE*)&Frame, sizeof(SFrameSettings));

   SaveBarState(REGKEY_CONTROLBARS);

   HMENU hMenu = ::GetMenu(m_hWnd);
   CFrameWnd::OnDestroy();
   ::DestroyMenu(hMenu);
}

void CCaraFrameWnd::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(::IsClipboardFormatAvailable(CF_ENHMETAFILE) ||
                  ::IsClipboardFormatAvailable(CF_DIB)         ||
                  ::IsClipboardFormatAvailable(CF_TEXT)        ||
                  ((m_nCF_LABELOBJECT     !=0) && ::IsClipboardFormatAvailable(m_nCF_LABELOBJECT)) ||
                  ((m_nCF_OLD_LABELOBJECT !=0) && ::IsClipboardFormatAvailable(m_nCF_OLD_LABELOBJECT)));
}

void CCaraFrameWnd::OnUpdateEditPasteSelectiv(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable(true);
}

/******************************************************************************
* Name       : GetFromClipboard                                               *
* Definition : CLabel* GetFromClipboard();                                    *
* Zweck      : Einfügen eines Labels aus dem Clipboard.                       *
* Aufruf     : GetFromClipboard();                                            *
* Parameter  :                                                                *
* Funktionswert : Zeiger auf ein Labelobjekt                                  *
******************************************************************************/
CLabel* CCaraFrameWnd::GetFromClipboard(UINT nSelect)
{
   CLabel *pl = NULL;
   if (((nSelect & CARA_LABEL)!=0) && 
       (m_nCF_LABELOBJECT     !=0) &&
       ::IsClipboardFormatAvailable(m_nCF_LABELOBJECT) &&
       ::OpenClipboard(m_hWnd))
   {
      HGLOBAL hMem = ::GetClipboardData(m_nCF_LABELOBJECT);
      if (hMem)
      {
         CCaraSharedFile sf;
         sf.SetHandle(hMem, false);
         if (sf.TestMemory())
         {
            CArchive ar(&sf, CArchive::load | CArchive::bNoFlushOnDelete, 1024);
            ar.m_pDocument = GetActiveDocument();
            ASSERT_KINDOF(CCARADoc, ar.m_pDocument);
            CCARADoc *pDoc = (CCARADoc*) ar.m_pDocument;
            ar.Read(&pDoc->m_FileHeader, sizeof(CFileHeader));
            int nFileVersion = pDoc->m_FileHeader.GetVersion();
            if ((nFileVersion <= g_nFileVersion) && pDoc->m_FileHeader.IsType(LABEL_OBJECT))
            {
               unsigned long  lChecksum = pDoc->m_FileHeader.GetChecksum();
               CLabel::SetFileVersion(nFileVersion);
               pDoc->m_FileHeader.CalcChecksum();
               CLabel::LoadGDIObjects(ar);
               CObject *po = ar.ReadObject(RUNTIME_CLASS(CLabel));
               CLabel::DeleteLOGArray();
               if (po != NULL)
               {
                  if (pDoc->m_FileHeader.IsValid(lChecksum) &&       // Checksumme prüfen
                      po->IsKindOf(RUNTIME_CLASS(CLabel)))           // Label ?
                  {
                     pl = (CLabel*) po;
                  }
                  else
                  {
                     delete po;
                  }
               }
            }
         }
         hMem = sf.Detach();
      }
      ::CloseClipboard();
   }
   else if (((nSelect & CARA_LABEL)!=0) && (m_nCF_OLD_LABELOBJECT !=0) &&
            ::IsClipboardFormatAvailable(m_nCF_OLD_LABELOBJECT) && ::OpenClipboard(m_hWnd))
   {
      CLabel::SetFileVersion(100);  // Version 1.0
      HGLOBAL hMem = ::GetClipboardData(m_nCF_OLD_LABELOBJECT);
      if (hMem)
      {
         CCaraSharedFile sf;
         sf.SetHandle(hMem, false);
         if (sf.TestMemory())
         {
            CArchive ar(&sf, CArchive::load | CArchive::bNoFlushOnDelete, 1024);
            CLabel::LoadGDIObjects(ar);
            CObject *po = ar.ReadObject(RUNTIME_CLASS(CLabel));
            CLabel::DeleteLOGArray();
            if (po != NULL)
            {
               if (po->IsKindOf(RUNTIME_CLASS(CLabel)))           // Label ?
                  pl = (CLabel*) po;
               else
                  delete po;
            }
         }         
         hMem = sf.Detach();
      }
      ::CloseClipboard();
   }
   else if (((nSelect & META_LABEL)!=0) &&
            ::IsClipboardFormatAvailable(CF_ENHMETAFILE) && ::OpenClipboard(m_hWnd))
   {
      HENHMETAFILE hMeta = (HENHMETAFILE) ::GetClipboardData(CF_ENHMETAFILE);
      if (hMeta)
      {
         HENHMETAFILE hMetaCopy = CopyEnhMetaFile(hMeta, NULL);
         CPictureLabel *ppl = new CPictureLabel;
         if (ppl)
         {
            ppl->AttachMetaFile(hMetaCopy, true);
            if (ppl->GetFormat() == META_FORMAT)
            {
               ppl->SetScale(SCALE_NONPROPORTIONAL, true);
               pl = ppl;
            }
            else
            {
               delete ppl;
            }
         }
      }
      ::CloseClipboard();
   }
   else if (((nSelect & DIB_LABEL)!=0) &&
            ::IsClipboardFormatAvailable(CF_DIB) && ::OpenClipboard(m_hWnd))
   {
      BITMAPINFO *pBmpInfo = (BITMAPINFO*)::GetClipboardData(CF_DIB);
      if (pBmpInfo)
      {
         CPictureLabel *ppl = new CPictureLabel;
         if (ppl)
         {
            CDibSection *pDib = new CDibSection;
            if (pDib)
            {
               if (pDib->CreateIndirect(pBmpInfo))
               {
                  ppl->AttachDIB(pDib, true);
                  ppl->SetScale(SCALE_PROPORTIONAL, true);
                  pl = ppl;
               }
               else
               {
                  delete pDib;
                  delete ppl;
               }
            }
            else delete ppl;
         }
      }
      ::CloseClipboard();
   }
   else if (((nSelect & TEXT_LABEL)!=0) &&
            ::IsClipboardFormatAvailable(CF_TEXT) && ::OpenClipboard(m_hWnd))
   {
      TCHAR * text= (TCHAR*)::GetClipboardData(CF_TEXT);
      if (text)
      {
         CCaraWinApp *pApp = (CCaraWinApp*) AfxGetApp();
         CRect rc = pApp->m_Margins.rcPageLoMetric;
         CTextLabel *ptxt = new CTextLabel(&rc, text);
         if (ptxt)
         {
            ptxt->SetTextAlign(TA_LEFT|TA_TOP);
            ptxt->InvalidateTextRect();
            ptxt->SetDrawPickPoints(true);
            pl = ptxt;
         }
      }
      ::CloseClipboard();
   }
   else if ((nSelect & BMP_FILE)!=0)
   {
      CString strFilter, strTitle;
      CFileDialog fd(true);
      fd.m_ofn.nFilterIndex   = 1;
      fd.m_ofn.nFileExtension = 1;
      fd.m_ofn.Flags         |= OFN_SHOWHELP;
      strTitle.LoadString(IDS_LOAD_BMP);
      fd.m_ofn.lpstrTitle     = LPCTSTR(strTitle);
      strFilter.LoadString(IDS_CARA_BITMAP);
      fd.m_ofn.lpstrFilter    = LPCTSTR(strFilter);
      for (int i=strFilter.GetLength()-1; i>=0; i--) if (fd.m_ofn.lpstrFilter[i] == '\n') ((TCHAR*)fd.m_ofn.lpstrFilter)[i] = 0;
      fd.m_ofn.nMaxCustFilter = 1;
      fd.m_ofn.lpstrDefExt    = &fd.m_ofn.lpstrFilter[_tcsclen(fd.m_ofn.lpstrFilter)];

      CFile file;
      if ((fd.DoModal() == IDOK) && (fd.m_ofn.lpstrFileTitle != NULL) &&
           file.Open(fd.m_ofn.lpstrFileTitle, CFile::modeRead))
      {
         CPictureLabel *ppl = new CPictureLabel;
         if (ppl)
         {
            CDibSection *pDib = new CDibSection;
            if (pDib)
            {
               if (pDib->LoadFromFile((HANDLE)file.m_hFile))
               {
                  ppl->AttachDIB(pDib, true);
                  ppl->SetScale(SCALE_PROPORTIONAL, true);
                  ppl->SetFileName(file.GetFilePath());
                  ppl->SetSaveInFile(true);
                  pl = ppl;
               }
               else
               {
                  delete pDib;
                  delete ppl;
               }
            }
            else delete ppl;
         }
      }
   }
   else if ((nSelect & SPECIAL_LABEL)!=0)
   {
      pl = PasteSpecial(NULL);
   }
   else if ((nSelect & TEXT_CSV)!=0
       && ::IsClipboardFormatAvailable(CF_TEXT)
       && ::OpenClipboard(m_hWnd))
   {
      TCHAR * text= (TCHAR*)::GetClipboardData(CF_TEXT);
      if (text)
      {
         CCaraWinApp *pApp = (CCaraWinApp*) AfxGetApp();
         CCurveLabel*pcrv = new CCurveLabel();
         if (pcrv)
         {
             pcrv->SetXUnit("n");
             pcrv->SetYUnit("y");
             pcrv->SetDescription("Clipboard");
             TCHAR*pstr = text;
             double fValue(0);
             TCHAR number[]  = "1234567890.e+-";
             CString fSep;
             int  i;
             for (i=0; text[i] != 0; ++i)
             {
                 if (strchr(number, text[i]) == 0) break;
             }
             for (; text[i] != 0; ++i)
             {
                if (strchr(number, text[i]) == 0)
                {
                    fSep += text[i];
                }
                else
                {
                    break;
                }
             }

             int fResult = sscanf(pstr, "%lf", &fValue);
             i = 1;
             while (fResult ==1)
             {
                 pcrv->SetSize(i);
                 pcrv->SetX_Value(i-1, (double)i);
                 pcrv->SetY_Value(i-1, fValue);
                 pstr = _tcsstr(pstr, fSep);
                 if (pstr)
                 {
                     pstr += fSep.GetLength();
                     fResult = sscanf(pstr, "%lf", &fValue);
                     ++i;
                 }
                 else
                 {
                     fResult = 0;
                 }
             }

             pl = pcrv;
         }
      }
      ::CloseClipboard();
   }
   if (pl) ASSERT_VALID(pl);
   return pl;
}

void CCaraFrameWnd::SetStatusPaneText(UINT nID, TCHAR *text)
{
   if (nID == 0)
   {
      if (text == NULL)
      {
         SendMessage(WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE, 0);
         return;
      }
   }
   if (/*(nID >= 0) &&*/ (nID < m_nActualIndicatorSize))
      m_wndStatusBar.SetPaneText(nID, text);
}

// Version 3
void CCaraFrameWnd::OnSysColorChange() 
{
   CFrameWnd::OnSysColorChange();
   if (!CCaraMenu::gm_nOwnerdraw) return;
   CMenu * pMenu  = GetMenu();
   if (pMenu && pMenu->DestroyMenu())
   {
      CMenu newMenu;
      VERIFY(newMenu.LoadMenu(m_nIDMenuResource));
      m_hMenuDefault = newMenu.Detach();
      VERIFY(SetMenu(CMenu::FromHandle(m_hMenuDefault)));
   }
   SetMenuBitmaps();
}

void CCaraFrameWnd::SetMenuBitmaps()
{
   if (!CCaraMenu::gm_nOwnerdraw) return;
   BEGIN("SetMenuBitmaps");
   CCaraMenu::SysColorChange();

   CMenu  Menu;
   HMENU hMenu = ::GetMenu(m_hWnd);
   if (!hMenu) return;
   Menu.Attach(hMenu);
   m_pMainMenu->SetMenuBitmaps(&Menu);
   Menu.Detach();
}

BOOL CCaraFrameWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
   BOOL bReturn = CFrameWnd::PreCreateWindow(cs);
   return bReturn;
}

bool CCaraFrameWnd::SetPreviewPaneInfo()
{
   bool bResult = false;
   if (::IsWindow(m_wndStatusBar.m_hWnd))
   {
      bResult = (m_wndStatusBar.SetIndicators(gm_nPreviewIndicators, NO_OF_PREVIEW_INDICATORS)!=0) ? true : false;
      if (bResult)
      {
         CString str;
         m_wndStatusBar.SetPaneInfo(0,0,SBPS_STRETCH|SBPS_NOBORDERS,0);
         if (str.LoadString(IDS_STATUS_PANE0)) m_wndStatusBar.GetStatusBarCtrl().SetTipText(0, str);
         m_wndStatusBar.SetPaneInfo(1,1,0, 140);
         if (str.LoadString(IDS_PREVSTATUS_PANE1)) m_wndStatusBar.GetStatusBarCtrl().SetTipText(1, str);
         m_wndStatusBar.SetPaneInfo(2,1,0, 140);
         if (str.LoadString(IDS_PREVSTATUS_PANE2)) m_wndStatusBar.GetStatusBarCtrl().SetTipText(2, str);
         m_nActualIndicatorSize = NO_OF_PREVIEW_INDICATORS;
      }
   }
   else ASSERT(false);
   return bResult;
}

void CCaraFrameWnd::OnHeapwatch() 
{
#ifndef _DEBUG
   CLabel::HeapWatch();
   CCurve::HeapWatch();
#endif
}

CLabel* CCaraFrameWnd::PasteSpecial(CPtrList* pList)
{
   CPtrList Lptr;                               // Einfügen von 
   CListDlg LDlg;

   CString  strDate;                            // Datum
   strDate.LoadString(IDS_PASTEDATE);
   Lptr.AddTail((void*)LPCTSTR(strDate));

   CString  strTime;                            // Uhrzeit
   strTime.LoadString(IDS_PASTETIME);
   Lptr.AddTail((void*)LPCTSTR(strTime));

   CString  strDateTime;                        // Datum & Uhrzeit
   strDateTime.LoadString(IDS_PASTEDATETIME);
   Lptr.AddTail((void*)LPCTSTR(strDateTime));

   if (pList)                                   // Programmabhängiges
   {
      Lptr.AddTail(pList);
      pList->RemoveAll();
   }

   LDlg.m_pCallBack   = ListSpecialPasteItems;
   LDlg.m_pParam      = &Lptr;
   LDlg.m_strSelected.LoadString(IDS_PASTE_SPECIAL);
   LDlg.m_szSize.cx   = 160;
   LDlg.m_szSize.cy   = 200;
   if (LDlg.DoModal() == IDOK)
   {
      switch (LDlg.m_nCursel)
      {
         case -1: break;
         case 0: case 1: case 2:
         {
            _tzset();
            time_t     Timer;
            time(&Timer);
            tm *pTm = localtime(&Timer);
            if (pTm)
            {
               int nYear  = pTm->tm_year + ((pTm->tm_year < 17) ? 2000 : 1900);
               CString text;
#ifdef AFX_TARG_DEU                             // Deutsches Datumsformat
               if (LDlg.m_nCursel == 0) text.Format(IDS_PASTEDATE_FMT, pTm->tm_mday, pTm->tm_mon+1, nYear);
#endif
#ifdef AFX_TARG_ENU                             // Englisches Datumsformat
               if (LDlg.m_nCursel == 0) text.Format(IDS_PASTEDATE_FMT, pTm->tm_mon+1, pTm->tm_mday, nYear);
#endif
               if (LDlg.m_nCursel == 1) text.Format(IDS_PASTETIME_FMT, pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
               if (LDlg.m_nCursel == 2) text.Format(IDS_PASTEDATETIME_FMT, pTm->tm_mday, pTm->tm_mon+1, nYear, pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
               if (!text.IsEmpty())
               {
                  CRect rect(0,0,400,-200);
                  CTextLabel *ptl = new CTextLabel(&rect, (TCHAR*)LPCTSTR(text));
                  if (ptl) ptl->SetDrawPickPoints(true);
                  return ptl;
               }
            } break;
         }
         default:
            if (pList) pList->AddTail((void*)(LDlg.m_nCursel-3));
      }
   }
   return NULL;
}

int CCaraFrameWnd::ListSpecialPasteItems(CListDlg *pl, int nReason)
{
   CPtrList *pLptr = (CPtrList*)pl->m_pParam;
   POSITION pos = pLptr->GetHeadPosition();
   if (nReason == ONINITDIALOG)
   {
      int i = 0;
      if (pos == NULL) return -1;
      CRect rc;
      pl->m_List.ModifyStyle(0, LVS_NOCOLUMNHEADER, 0);
      pl->m_List.GetClientRect(&rc);
      pl->m_List.InsertColumn(0, "", LVCFMT_LEFT, rc.right-2, 0);

      while (pos)
      {
         pl->m_List.InsertItem(i++, (const TCHAR*)pLptr->GetNext(pos));
      }
      return 1;
   }
   return 0;
}

LRESULT CCaraFrameWnd::OnSetText(WPARAM wParam, LPARAM lParam)
{
   //if (lParam)
   //{
   //   TCHAR *text = (TCHAR*) lParam;              // Fenstertext umdödeln auf:
   //   lstrcpy(text, m_strTitle);                // Programmname - [Dokument.doc]
   //   CDocument *pDoc = GetActiveDocument();
   //   ASSERT_KINDOF(CCARADoc, pDoc);
   //   if (pDoc)
   //   {
   //      CString pn;
   //      lstrcat(text, " - ");
   //      if (!((CCARADoc*)pDoc)->m_bIsTempFile)
   //         pn = pDoc->GetPathName();
   //      lstrcat(text, "[");
   //      if (!pn.IsEmpty()) lstrcat(text, pn);
   //      else               lstrcat(text, pDoc->GetTitle());
   //      lstrcat(text, "]");
   //   }
   //}
   return DefWindowProc(WM_SETTEXT, wParam, lParam);
}

void CCaraFrameWnd::WinHelp(DWORD dwData, UINT nCmd) 
{
   if (dwData == 0) nCmd = HELP_FINDER;

   switch (LOWORD(dwData))
   {
      case IDR_MAINFRAME:
      case AFX_IDW_DOCKBAR_TOP:
      case AFX_IDW_DOCKBAR_LEFT:
      case AFX_IDW_DOCKBAR_RIGHT:
      case AFX_IDW_DOCKBAR_BOTTOM:
      case AFX_IDW_DOCKBAR_FLOAT:
         dwData = 0;
         nCmd = HELP_FINDER;
         break;
   }

   CCaraWinApp *pApp = (CCaraWinApp*)AfxGetApp();
   const TCHAR * pszDefault = NULL;
   if (!pApp->m_strPreviewHelpPath.IsEmpty())   // Hilfepfad der gemeinsamen Hifedatei
   {
      CView *pView = GetActiveView();
      if ((pView!= NULL) && pView->IsKindOf(RUNTIME_CLASS(CPreviewView)))
      {
         pszDefault = pApp->m_pszHelpFilePath;
         pApp->m_pszHelpFilePath = LPCTSTR(pApp->m_strPreviewHelpPath);
      }
   }
   CFrameWnd::WinHelp(dwData, nCmd);
   if (pszDefault) pApp->m_pszHelpFilePath = pszDefault;
}

void CCaraFrameWnd::OnClose() 
{
   if (InSendMessage())                // Wenn der Befehl WM_CLOSE von einem anderen
   {                                   // Thread (anderes Programm) kommt,
      if (m_lpfnCloseProc != NULL)     // so wird erst der Preview und dann das
         (*m_lpfnCloseProc)(this);     // Hauptfenster geschlossen.
   }
   CCaraWinApp *pApp = (CCaraWinApp*)AfxGetApp();
   // Anderenfalls wird nur der Preview geschlossen!
   CFrameWnd::OnClose();
}

bool CCaraFrameWnd::GetMonitorInfo(MONITORINFOEX &sMonInfo, HWND hwnd, CRect*pRect, CPoint*pPoint, DWORD dwFlags)
{
   HMONITOR  hMon = NULL;
   if      (pPoint) hMon = MonitorFromPoint(*pPoint, dwFlags);
   else if (pRect ) hMon = MonitorFromRect(   pRect, dwFlags);
   else
   {
      if (!hwnd) dwFlags = MONITOR_DEFAULTTOPRIMARY;
      hMon = MonitorFromWindow(hwnd, dwFlags);
   }

   if (hMon)
   {
      sMonInfo.cbSize = sizeof(MONITORINFOEX);
      if (::GetMonitorInfo(hMon, &sMonInfo))
         return true;
   }
   return false;
}

void     CCaraFrameWnd::ShowShortCuts(HACCEL hAccelTable)
{
	if (hAccelTable)
	{
		int i, nAcc = CopyAcceleratorTable(hAccelTable, NULL, 0);
		if (nAcc)
		{
			struct MyAccel
			{
				WORD fVirt;
				WORD key;
				WORD cmd;
			};
			MyAccel *pAcc = new MyAccel[nAcc];
			CString s, sCmd, sTxt, sMsg;
			nAcc = CopyAcceleratorTable(hAccelTable, (ACCEL*)pAcc, nAcc);
			for (i=0; i<nAcc; i++)
			{
				UINT uID = pAcc[i].cmd;
				sCmd.Empty();
				if (pAcc[i].fVirt & FALT) sCmd += _T("ALT+");
				if (pAcc[i].fVirt & FCONTROL) sCmd += _T("Ctrl+");
				if (pAcc[i].fVirt & FSHIFT) sCmd += _T("Shift+");
				if (isalpha(pAcc[i].key)&& isupper(pAcc[i].key))
				{
					TCHAR sKey[2] = _T(" ");
					sKey[0] = (TCHAR)pAcc[i].key;
					sCmd += sKey;
				}
				else
				{
					CString sKey;
					switch(pAcc[i].key)
					{
						case VK_ESCAPE: sKey = _T("ESC"); break;
						case VK_RETURN: sKey = _T("Enter"); break;
						case VK_DELETE: sKey = _T("Del"); break;
						case VK_NEXT: sKey = _T("Next"); break;
						case VK_PRIOR: sKey = _T("Prior"); break;
						case VK_END: sKey = _T("End"); break;
						case VK_HOME: sKey = _T("Home"); break;
						case VK_RIGHT: sKey = _T("Right"); break;
						case VK_LEFT: sKey = _T("Left"); break;
						case VK_UP: sKey = _T("Up"); break;
                  case VK_DOWN: sKey = _T("Down"); break;
						case VK_PAUSE: sKey = _T("|-|"); break;
						case VK_SPACE: sKey = _T("Space"); break;
						default:
							if (pAcc[i].key >= VK_F1 && pAcc[i].key <=  VK_F24)
							{
								sKey.Format(_T("F%d"), pAcc[i].key - VK_F1 + 1);
							}
							else
							{
								sKey.Format(_T("VK (%d)"), pAcc[i].key);
							}
							break;
					}
					sCmd += sKey;
				}
				if (  s.LoadString(uID))
				{
					s.Replace(_T("\n"), _T(" / "));
					sTxt.Format(_T("%s:\t%s\n"), sCmd, s);
				}
				else
				{
					sTxt.Format(_T("%s:\t%d\n"), sCmd, uID);
//								continue;
				}
				TRACE(_T("%s"), sTxt);
				sMsg += sTxt;
			}
			AfxMessageBox(sMsg, MB_OK);
			delete pAcc;
		}
   }
}

BOOL CCaraFrameWnd::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
   ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

   // need to handle both ANSI and UNICODE versions of the message
   TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
   TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
   TCHAR szFullText[256];
   CString strTipText;
   UINT nID = pNMHDR->idFrom;
   UINT *puFlags;

   if (pNMHDR->code == TTN_NEEDTEXTA) puFlags = &pTTTA->uFlags;
   else                               puFlags = &pTTTW->uFlags;
     
   if (*puFlags & TTF_FROMVIEW)
   {
      *puFlags &= ~TTF_FROMVIEW;
      nID = 0;
   }
   else if (*puFlags & TTF_FROMSTATUSBAR)
   {
      *puFlags &= ~TTF_FROMSTATUSBAR;
      strTipText = m_wndStatusBar.GetStatusBarCtrl().GetTipText(HIWORD(*puFlags));
      nID = 0;
   }
   else if (*puFlags & TTF_IDISHWND)
   {
      nID = ::GetDlgCtrlID((HWND)nID);
   }
   if (nID != 0) // will be zero on a separator
   {
      int nString = 1;
      // don't handle the message if no string resource found
      if (AfxLoadString(nID, szFullText) == 0)
         return FALSE;
      CCaraMenu *pMainMenu = m_pMainMenu;
      CView     *pView     = GetActiveView();
      if (pView && pView->IsKindOf(RUNTIME_CLASS(CCARAPreview)))
         pMainMenu = ((CCARAPreview*)pView)->GetMainMenu();

      if (pMainMenu)
      {
         CPtrList *pL = pMainMenu->GetToolBars();
         if (pL)
         {
            POSITION  pos = pL->GetHeadPosition();
            while (pos)
            {
               CCaraToolbar *pCTB = (CCaraToolbar*) pL->GetNext(pos);
               if (pCTB)
               {
                  if (pCTB->IsButtonPressed(nID)) 
                     nString = 2;
               }
            }
         }
      }
      // this is the command id, not the button index
      if (!AfxExtractSubString(strTipText, szFullText, nString, '\n') && (nString == 2))
         AfxExtractSubString(strTipText, szFullText, 1, '\n');
   }

   if (!strTipText.IsEmpty())
   {
#ifndef _UNICODE
      if (pNMHDR->code == TTN_NEEDTEXTA)
         lstrcpyn(pTTTA->szText, strTipText, _countof(pTTTA->szText));
      else
         _mbstowcsz(pTTTW->szText, strTipText, _countof(pTTTW->szText));
#else
      if (pNMHDR->code == TTN_NEEDTEXTA)
         _wcstombsz(pTTTA->szText, strTipText, _countof(pTTTA->szText));
      else
         lstrcpyn(pTTTW->szText, strTipText, _countof(pTTTW->szText));
#endif
   }
   *pResult = 0;

   if (m_hwndToolTip == NULL)
   {
      m_hwndToolTip = pNMHDR->hwndFrom;
   }

   ::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
      SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);

   return TRUE;    // message was handled
}

void CCaraFrameWnd::OnViewBaloonTooltips() 
{
   if (m_hwndToolTip)
   {
      long lStyle = ::GetWindowLong(m_hwndToolTip, GWL_STYLE);
      if (lStyle & TTS_BALLOON) lStyle &= ~TTS_BALLOON;
      else                      lStyle |=  TTS_BALLOON;
      ::SendMessage(m_hwndToolTip, TTM_SETTITLE, 1, (LPARAM)"Info");
      ::SetWindowLong(m_hwndToolTip, GWL_STYLE, lStyle);
   }
   
}

void CCaraFrameWnd::OnUpdateViewBaloonTooltips(CCmdUI* pCmdUI) 
{
   if (m_hwndToolTip)
   {
      long lStyle = ::GetWindowLong(m_hwndToolTip, GWL_STYLE);
      pCmdUI->SetCheck((lStyle & TTS_BALLOON) ? 1 : 0);
      pCmdUI->Enable(true);
   }
   else pCmdUI->Enable(false);
}

BOOL CCaraFrameWnd::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
/*
   NMHDR *pn = (NMHDR*)lParam;
   if ((pn->code == TTN_SHOW) && (m_hwndToolTip != NULL))
   {
      CRect rc;
      ::GetWindowRect(m_hwndToolTip, &rc);
      CPoint pt = rc.TopLeft();
      rc.InflateRect(2, 2);
      ::SetWindowPos(m_hwndToolTip,
                 NULL,
                 0, 0,
                 rc.Width(), rc.Height(),
                 SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
   }
   if ((pn->code == NM_CUSTOMDRAW) && (m_hwndToolTip != NULL) && (pn->hwndFrom == m_hwndToolTip))
   {
      NMTTCUSTOMDRAW *pnc = (NMTTCUSTOMDRAW*) lParam;
      switch(pnc->nmcd.dwDrawStage) 
      {
         case CDDS_PREPAINT:
         {
            ::SetTextColor(pnc->nmcd.hdc, RGB(255, 0, 0));
         } break;
      }
   }
 
   // TODO: Speziellen Code hier einfügen und/oder Basisklasse aufrufen
*/   
   return CFrameWnd::OnNotify(wParam, lParam, pResult);
}

LRESULT CCaraFrameWnd::OnSetMessageString(WPARAM wParam, LPARAM lParam)
{
   if ((ID_FILE_MRU_FILE1 <= wParam) && (wParam <= ID_FILE_MRU_LAST))
      wParam = ID_FILE_MRU_FILE1;
   if (wParam == IDCANCEL) wParam = IDS_CANCEL;
   return CFrameWnd::OnSetMessageString(wParam, lParam);
}

void CCaraFrameWnd::OnViewForeground() 
{
   m_bForeGround = !m_bForeGround;
   ::SetWindowPos(m_hWnd, m_bForeGround ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOOWNERZORDER);
}

void CCaraFrameWnd::OnUpdateViewForeground(CCmdUI* pCmdUI) 
{
   pCmdUI->SetCheck(m_bForeGround);
}

void CCaraFrameWnd::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
{
   CFrameWnd::OnMenuSelect(nItemID, nFlags, hSysMenu);
   
}