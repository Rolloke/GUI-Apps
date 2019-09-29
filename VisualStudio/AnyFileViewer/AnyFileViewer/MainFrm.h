// MainFrm.h : Schnittstelle der Klasse CMainFrame
//


#pragma once

#include "MainDlgBar.h"
#include "MoveFindDlgBar.h"
#include "FormatViewDlg.h"
#include "formatconvertdlg.h"

#define PANE_POSITION   2
#define PANE_ROW        4
#define PANE_COLUMN     6

enum ContexMenu
{
    StructureView,
    ContentView,
    ContentFormat
};


class CAnyFileViewerView;
class CFormatView;
class CAnyFileViewerTreeView;

class CMainFrame : public CFrameWnd
{
    
protected: // Nur aus Serialisierung erstellen
    CMainFrame();
    DECLARE_DYNCREATE(CMainFrame)

// Attribute
protected:
    CSplitterWnd m_wndSplitter;
public:

// Operationen
public:
   void    SetPaneInt(int nPane, int n);
   void    SetPaneInt(int nPane, size_t n);
   void    SetClipboardData(CString str);
   CString GetClipboardData();

// Überschreibungen
public:
    virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
    virtual BOOL DestroyWindow();

// Implementierung
public:
    virtual ~CMainFrame();
    CAnyFileViewerView*        GetMainPane();
    CFormatView*            GetFormatPane();
    CAnyFileViewerTreeView* GetTreeViewPane();
    CMainDlgBar&            GetMainDlgBar() {return m_wndDlgBar; };
    CFormatViewDlgBar&      GeFormatViewDlgBar() { return m_wndFmtDlgBar; };

    void UpdateFormatFiles(void);
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // Eingebundene Elemente der Steuerleiste
    CStatusBar  m_wndStatusBar;
    CToolBar    m_wndToolBar;
    CMainDlgBar  m_wndDlgBar;
    CFormatViewDlgBar m_wndFmtDlgBar;
    CMoveFindDlgBar m_wndMoveFindDlgBar;
    CFormatConvertDlg m_FormatConvertDlg;
    CSplitterWnd    m_wndSplitterLists;

// Generierte Funktionen für die Meldungstabellen
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnViewDlgToolBar();
    afx_msg void OnUpdateViewDlgToolBar(CCmdUI *pCmdUI);
    afx_msg void OnViewFormatToolBar();
    afx_msg void OnUpdateViewFormatToolBar(CCmdUI *pCmdUI);
    afx_msg void OnViewSearchToolBar();
    afx_msg void OnUpdateViewSearchToolBar(CCmdUI *pCmdUI);
    afx_msg void OnEditFind();
    afx_msg LRESULT OnFind(WPARAM wParam, LPARAM lParam);
    afx_msg void OnUpdateFormatDifferentEndianess(CCmdUI *pCmdUI);
    afx_msg void OnUpdateFormatSameEndianess(CCmdUI *pCmdUI);
    afx_msg void OnUpdateFormatConvertDlg(CCmdUI *pCmdUI);
    afx_msg void OnFormatDifferentEndianess();
    afx_msg void OnFormatSameEndianess();
    afx_msg void OnFormatConvertDlg();
    afx_msg void OnFormatLoad();
    afx_msg void OnUpdateFormatLoad(CCmdUI *pCmdUI);
    afx_msg void OnTreeviewFileContent();
    afx_msg void OnTreeviewFileStructure();
    afx_msg void OnUpdateTreeviewFileStructure(CCmdUI *pCmdUI);
    afx_msg void OnUpdateTreeviewFileContent(CCmdUI *pCmdUI);
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnEditChangeDataType();
    afx_msg void OnUpdateEditChangeDataType(CCmdUI *pCmdUI);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};


