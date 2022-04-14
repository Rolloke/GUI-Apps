#pragma once


// CViewPrintPreview view

class CViewPrintPreview : public CPreviewView
{
	DECLARE_DYNCREATE(CViewPrintPreview)

protected:
	CViewPrintPreview();           // protected constructor used by dynamic creation
	virtual ~CViewPrintPreview();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	afx_msg void OnPreviewPrint();
	DECLARE_MESSAGE_MAP()
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
};


