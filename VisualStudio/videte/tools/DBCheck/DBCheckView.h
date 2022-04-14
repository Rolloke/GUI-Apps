// DBCheckView.h : interface of the CDBCheckView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DBCHECKVIEW_H__CDE92FF8_1B41_4367_AB41_1C36C5BF1902__INCLUDED_)
#define AFX_DBCHECKVIEW_H__CDE92FF8_1B41_4367_AB41_1C36C5BF1902__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CDBCheckView : public CListView
{
protected: // create from serialization only
	CDBCheckView();
	DECLARE_DYNCREATE(CDBCheckView)

// Attributes
public:
	CDBCheckDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDBCheckView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	void InsertRecord(WORD wArchivNr,WORD wSequenceNr, DWORD dwRecordNr, CIPCFields fields, int *pnPositions, CIPCFields records);
	virtual ~CDBCheckView();
	DWORD GetFirstRequestedNr();
	void SetFirstRequestedNr(DWORD);
	void InsertMaxArchiveTime();
	void InsertRecordNr(WORD wArchivNr,WORD wSequenceNr,DWORD dwRecordNr,DWORD dwNrOfRecords,const CIPCPictureRecord &pict,int iNumFields,const CIPCField fields[]);
	bool ContainsSequence(WORD, WORD, DWORD);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDBCheckView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	DECLARE_MESSAGE_MAP()

private:
	CSystemTime m_MaxArchiveTime;
	CSystemTime m_MaxSequenceTime;
   int         m_nMaxSequenceTimePos;
   int         m_nMaxArchiveTimePos;
   DWORD       m_dwFirstRequestedNr;
};

#ifndef _DEBUG  // debug version in DBCheckView.cpp
inline CDBCheckDoc* CDBCheckView::GetDocument()
   { return (CDBCheckDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBCHECKVIEW_H__CDE92FF8_1B41_4367_AB41_1C36C5BF1902__INCLUDED_)
