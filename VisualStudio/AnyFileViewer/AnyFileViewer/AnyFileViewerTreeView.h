#pragma once

#include "XmlNodeWrapper.h"

// CAnyFileViewerTreeView-Ansicht
class CAnyFileViewerDoc;
class CItemStructureHint;
class CTreeCtrlEx;

class CAnyFileViewerTreeView : public CTreeView
{
    class Attr { 
    public:
        enum eAttribute { Unknown, type, repeat, name, count, bytes, in_line, rule, factor, Last}; 
        static const LPCTSTR getNameOfType(eAttribute aType);
        static eAttribute getTypeOfName(const CString& aName);
    };
    class Item { 
    public:
        enum eItem {Unknown, Structure, Data , Last}; 
        static const LPCTSTR getNameOfType(eItem aType);
        static eItem getTypeOfName(const CString& aName);
    };
    class ContentData
    {
    public:
        ContentData(size_t aPos, IDispatch*aNode) : mPos(aPos), mNode(aNode) {};
        size_t mPos;
        IDispatch*mNode;
        static int findpos(const ContentData* a1, const ContentData* a2);
    };
    DECLARE_DYNCREATE(CAnyFileViewerTreeView)

protected:
    CAnyFileViewerTreeView();           // Dynamische Erstellung verwendet geschützten Konstruktor
    virtual ~CAnyFileViewerTreeView();

    HTREEITEM RecurseXMLStructure(IDispatch *pNode, HTREEITEM hParent, HTREEITEM hInsertAfter=0);
    void UpdateXmlAttributes(const HTREEITEM aItem, CXmlNodeWrapper&aNode);
    void RecurseContent(IDispatch *aNode, HTREEITEM aParent, size_t& aPos, CString *aTxt=0);
    void FindDataPositionOfItem(HTREEITEM aItemToFind, size_t& aPos, HTREEITEM aParent=TVI_ROOT);
    HTREEITEM FindTreeItem(const void*aValue, bool (*aFunc)(const CTreeCtrl&aTree, HTREEITEM aItem, const void*aValue), HTREEITEM aParent);

    static CString GetXmlAttribute(CXmlNodeWrapper&aNode, Attr::eAttribute aAttr);


public:
    enum eViewType { showFileStructure, showFileContent };
    struct ip { enum eInsertPosition { before, after, child }; };
    CAnyFileViewerDoc* GetDocument();
    void    OnInsertDataType(CString &aName, int aDataType, int aLen, int aInsertPos);
    BOOL    isStructureView();
    bool    GetSelection(size_t& aStartPos, size_t& aStopPos);
    void    LoadFormatFile(LPCTSTR sFile);
    void    OnTreeviewFileStructure();
    void    OnTreeviewFileContent();

    eViewType getViewType() { return mViewType;  }

#ifdef _DEBUG
    virtual void AssertValid() const;
#ifndef _WIN32_WCE
    virtual void Dump(CDumpContext& dc) const;
#endif
#endif

private:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
    virtual void OnInitialUpdate();

    afx_msg void OnFormatSave();
    afx_msg void OnUpdateFormatSave(CCmdUI *pCmdUI);
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnTvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnUpdateEditClear(CCmdUI *pCmdUI);
    afx_msg void OnEditClear();
    afx_msg void OnTvnBeginrdrag(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnEditInsertRepeat();
    afx_msg void OnUpdateEditInsertRepeat(CCmdUI *pCmdUI);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnEditCopy(UINT aID);
    afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
    afx_msg BOOL OnEditPaste(UINT uID);
    afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
    afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnEditInsertInlineAttr();
    afx_msg void OnUpdateEditInsertInlineAttr(CCmdUI *pCmdUI);
    afx_msg void OnViewUpdate();
    afx_msg void OnTreeviewInlineStructure();
    afx_msg void OnUpdateTreeviewInlineStructure(CCmdUI *pCmdUI);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnKeyDown(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnEditInsertFactorAttr();
    afx_msg void OnUpdateEditInsertFactorAttr(CCmdUI *pCmdUI);
    DECLARE_MESSAGE_MAP()

private:
    CXmlDocumentWrapper m_xmlDoc;
    eViewType           mViewType;
    CImageList          mTreeImages;
    HTREEITEM           mDraggedItem;
    BOOL                mRightDragged;
    BOOL                m_bInOnUpdate;
    BOOL                m_bShowInline;
    std::vector<ContentData> mContentData;
    CMap<CString, LPCTSTR, size_t, size_t> mCounters;
    CItemStructureHint* mItemStructureHint;
    CTreeCtrlEx*        mTreeCtrl;

public:
protected:
public:
};


#ifndef _DEBUG  // Debugversion in FormatView.cpp
inline CAnyFileViewerDoc* CAnyFileViewerTreeView::GetDocument()
   { return reinterpret_cast<CAnyFileViewerDoc*>(m_pDocument); }
#endif
