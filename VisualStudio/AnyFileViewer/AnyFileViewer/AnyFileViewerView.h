// AnyFileViewerView.h : Schnittstelle der Klasse CAnyFileViewerView
//


#pragma once

#include "DisplayType.h"

#include "AnyFileViewerDoc.h"

//class CAnyFileViewerDoc;

class CFindReplaceDialogEx : public CFindReplaceDialog
{
public:
    CFindReplaceDialogEx();
protected:
    virtual void PostNcDestroy()
    {
        // do nothing
    }
    static UINT_PTR CALLBACK FRHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);
public:
    size_t            m_nCurrentFindPos;
    CStringA          m_sSearchAnsi;
};

class CAnyFileViewerView : public CListView
{
protected: // Nur aus Serialisierung erstellen
    CAnyFileViewerView();
    DECLARE_DYNCREATE(CAnyFileViewerView)

// Attribute
public:
    CAnyFileViewerDoc* GetDocument() const;
    int   GetDataType() { return m_nType;};
    int   GetColumns() { return m_nColumns;};
    int   GetOffset() { return m_nOffset;};
    size_t GetDataPosition(int nItem=-1);

// Operationen
public:
    void OnSelectChangeDataType(int nType);
    int  OnEnChangeEdtColumns(int nColumns);
    int  OnEnChangeEdtOffset(int nColumns);
    void OnEditFind();
    void OnFind(LPFINDREPLACE lpfr);
    bool GetSelection(size_t& aStartPos, size_t& aStopPos);

// Überschreibungen
    public:
protected:
    virtual void OnInitialUpdate(); // Erster Aufruf nach Erstellung
    virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementierung
public:
    virtual ~CAnyFileViewerView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generierte Funktionen für die Meldungstabellen
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
    afx_msg void OnEditCopy();
    afx_msg void OnUpdateEditFind(CCmdUI *pCmdUI);
    afx_msg void OnHdnEndtrack(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnAccelLeft();
    afx_msg void OnUpdateAccelLeft(CCmdUI *pCmdUI);
    afx_msg void OnAccelRight();
    afx_msg void OnUpdateAccelRight(CCmdUI *pCmdUI);
    afx_msg void OnViewUpdate();
    DECLARE_MESSAGE_MAP()

private:
    int      m_nColumns;
    int      m_nColPosition;
    int      m_nCurrentLine;
    CDisplayType::eType    m_nType;
    int      m_nOffset;
    BOOL     m_bInOnUpdate;
    CString  m_sDisplay;
    CFont    m_FixedFont;

    CFindReplaceDialogEx m_dlgFR;
    int               m1stColWidthOfView;
    std::vector<CStructureItem> mItems;
public:
private:
    // convert dialog
public:
};

#ifndef _DEBUG  // Debugversion in AnyFileViewerView.cpp
inline CAnyFileViewerDoc* CAnyFileViewerView::GetDocument() const
   { return reinterpret_cast<CAnyFileViewerDoc*>(m_pDocument); }
#endif

