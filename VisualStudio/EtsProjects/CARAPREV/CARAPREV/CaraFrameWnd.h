#ifndef AFX_CARAFRAMEWND_H__30FA9EDB_14B6_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_CARAFRAMEWND_H__30FA9EDB_14B6_11D2_9DB9_0000B458D891__INCLUDED_

// CaraFrameWnd.h : Header-Datei
//
#include "stdafx.h"
#include "MultiMon.h"

#define NO_OF_PREVIEW_INDICATORS 3

#define HEAD_ELEMENT true
#define TAIL_ELEMENT false

#define LABEL_OBJECT             "LBO"

#define REGKEY_CONTROLBARS      "Control-Bars"
#define REGKEY_DEFAULT_FRAME    "Frame"

#define CARA_LABEL              0x0001
#define META_LABEL              0x0002
#define DIB_LABEL               0x0004
#define TEXT_LABEL              0x0008
#define BMP_FILE                0x0010
#define SPECIAL_LABEL           0x0020
#define TEXT_CSV                0x0040

#define TTF_FROMSTATUSBAR  0x01000000
#define TTF_FROMVIEW       0x02000000

class CLabel;
class CCaraMenu;
class CListDlg;

struct SFrameSettings
{
   CRect rcFrame;
   BOOL  bIconic,
         bMaximized;
};

/////////////////////////////////////////////////////////////////////////////
// Rahmen CCaraFrameWnd 
class CCaraFrameWnd;

class CCaraStatusBar : public CStatusBar
{
    DECLARE_DYNAMIC(CCaraStatusBar)
public:
    CCaraStatusBar() {m_pFW = NULL;};
    virtual int OnToolHitTest(CPoint, TOOLINFO*) const;
    virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );
    CCaraFrameWnd *m_pFW;
private:
};

class AFX_EXT_CLASS CCaraFrameWnd : public CFrameWnd
{
   friend class CCaraStatusBar;
    DECLARE_DYNCREATE(CCaraFrameWnd)
protected:
    CCaraFrameWnd();           // Dynamische Erstellung verwendet geschützten Konstruktor

// Attribute
public:

// Operationen
public:
   
   CLabel*  GetFromClipboard(UINT nSelect = 0x0F);
   void     SetStatusPaneText(UINT nID, TCHAR *text);
   void     SetMenuBitmaps();
   bool     GetMonitorInfo(MONITORINFOEX &, HWND hwnd=NULL, CRect*pRect=NULL, CPoint*pPoint=NULL, DWORD dwFlags=MONITOR_DEFAULTTONEAREST);
   void     ShowShortCuts(HACCEL);

   LRESULT  OnSetText(WPARAM, LPARAM);
   LRESULT  OnDrawItem(WPARAM, LPARAM);
   LRESULT  OnMeasureItem(WPARAM, LPARAM);
   

// Überschreibungen
    // Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
    //{{AFX_VIRTUAL(CCaraFrameWnd)
    public:
    virtual void ActivateFrame(int nCmdShow = -1);
    virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
    protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    //}}AFX_VIRTUAL


// Implementierung
protected:
    virtual ~CCaraFrameWnd();
    virtual CLabel* PasteSpecial(CPtrList*);

    // Generierte Nachrichtenzuordnungsfunktionen
    //{{AFX_MSG(CCaraFrameWnd)
    afx_msg void OnDestroy();
    afx_msg void OnSysColorChange();
    afx_msg void OnUpdateEditPasteSelectiv(CCmdUI* pCmdUI);
    afx_msg void OnHeapwatch();
    afx_msg void OnClose();
    afx_msg void OnViewForeground();
    afx_msg void OnUpdateViewForeground(CCmdUI* pCmdUI);
    afx_msg void OnViewBaloonTooltips();
    afx_msg void OnUpdateViewBaloonTooltips(CCmdUI* pCmdUI);
    //}}AFX_MSG
    afx_msg LRESULT OnSetMessageString(WPARAM wParam, LPARAM lParam);
    afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnMenuSelect( UINT nItemID, UINT nFlags, HMENU hSysMenu);
//    afx_msg void OnTimer(UINT nIDEvent);
//    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
//    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()


public:
    afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
    bool SetPreviewPaneInfo();

public:
    static int ListSpecialPasteItems(CListDlg*, int);
   UINT  m_nCF_LABELOBJECT;
   UINT  m_nCF_OLD_LABELOBJECT;

protected:
   CCaraStatusBar m_wndStatusBar;
   CCaraMenu     *m_pMainMenu;
   UINT           m_nIDMenuResource;
   UINT           m_nActualIndicatorSize;

private:
   bool           m_bFirstTime;
   bool           m_bForeGround;
   HWND           m_hwndToolTip;
   static    UINT gm_nPreviewIndicators[NO_OF_PREVIEW_INDICATORS];
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_CARAFRAMEWND_H__30FA9EDB_14B6_11D2_9DB9_0000B458D891__INCLUDED_
