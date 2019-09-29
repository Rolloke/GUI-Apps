#pragma once


// CMessageView-Ansicht

class CMessageView : public CListView
{
    DECLARE_DYNCREATE(CMessageView)

protected:
    CMessageView();           // Dynamische Erstellung verwendet geschützten Konstruktor
    virtual ~CMessageView();
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

public:
#ifdef _DEBUG
    virtual void AssertValid() const;
#ifndef _WIN32_WCE
    virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
    DECLARE_MESSAGE_MAP()
public:
};


