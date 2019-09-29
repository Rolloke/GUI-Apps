// AnalyseMapFileView.h : Schnittstelle der Klasse CAnalyseMapFileView
//


#pragma once


class CAnalyseMapFileView : public CListView
{
protected: // Nur aus Serialisierung erstellen
	CAnalyseMapFileView();
	DECLARE_DYNCREATE(CAnalyseMapFileView)

// Attribute
public:
	CAnalyseMapFileDoc* GetDocument() const;

// Operationen
public:
	void OnEditFindAddress();
	void OnEditFindFunction();
	void OnEditFindNextFnc();
	void OnUpdateEditFindNextFnc(CCmdUI *pCmdUI);

// Überschreibungen
	public:
	virtual void OnDraw(CDC* pDC);  // Überladen, um diese Ansicht darzustellen
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
protected:
	virtual void OnInitialUpdate(); // Erster Aufruf nach Erstellung
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementierung
public:
	virtual ~CAnalyseMapFileView();

	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CString m_sSearchFunction;
	int m_nLastPos;
	CString m_sSearchAddress;

// Generierte Funktionen für die Meldungstabellen
protected:
	//{{AFX_MSG(CAnalyseMapFileView)
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	afx_msg void OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
};

#ifndef _DEBUG  // Debugversion in AnalyseMapFileView.cpp
inline CAnalyseMapFileDoc* CAnalyseMapFileView::GetDocument() const
   { return reinterpret_cast<CAnalyseMapFileDoc*>(m_pDocument); }
#endif

