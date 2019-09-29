// FormatView.h : Schnittstelle der Klasse CFormatView
//


#pragma once

class CAnyFileViewerDoc;

class CFormatView : public CListView
{
protected: // Nur aus Serialisierung erstellen
    CFormatView();
    DECLARE_DYNCREATE(CFormatView)

// Attribute
public:
    CAnyFileViewerDoc* GetDocument();

// Operationen
public:
    void    OnInsertDataType(CString &sName, int nDataType, int nLen);
    CString GetItemDataType(int i);
    void    InsertItemDataType(CString sFmt);
    void    LoadFormatFile(LPCTSTR sFile);
    void    SetAllFormats();

// Überschreibungen
    protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnInitialUpdate(); // Erster Aufruf nach Erstellung
    virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);

// Implementierung
public:
    virtual ~CFormatView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generierte Funktionen für die Meldungstabellen
protected:
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnEditCopy();
    afx_msg void OnEditClear();
    afx_msg void OnEditPaste();
    afx_msg void OnEditCut();
    afx_msg void OnFormatSave();
    afx_msg void OnEditClearAll();
    afx_msg void OnFormatEdit();
    afx_msg void OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnUpdateEditClear(CCmdUI *pCmdUI);
    afx_msg void OnUpdateFormatSave(CCmdUI *pCmdUI);
    afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
    afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnHdnEndtrack(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnUpdateViewConcatenated(CCmdUI *pCmdUI);
    afx_msg void OnViewConcatenated();
    afx_msg void OnUpdateViewSameStartpos(CCmdUI *pCmdUI);
    afx_msg void OnViewSameStartpos();
    afx_msg void OnFormatAll();
    DECLARE_MESSAGE_MAP()
private:
    BOOL m_bInOnUpdate;
    int  m1stColWidth;
    int  m2ndColWidth;
    int  m_nOrder[3];
public:
     BOOL m_bConcatenated;
     afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};

#ifndef _DEBUG  // Debugversion in FormatView.cpp
inline CAnyFileViewerDoc* CFormatView::GetDocument()
   { return reinterpret_cast<CAnyFileViewerDoc*>(m_pDocument); }
#endif

