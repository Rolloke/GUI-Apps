// LeftView.h : interface of the CLeftView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_LEFTVIEW_H__804F4122_B354_4CFE_8511_72911C39CE9F__INCLUDED_)
#define AFX_LEFTVIEW_H__804F4122_B354_4CFE_8511_72911C39CE9F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDBCheckDoc;

#define TREE_CONTROL_CHECK_OFF	0
#define TREE_CONTROL_UNCHECKED	2
#define TREE_CONTROL_CHECKED	1

class CLeftView : public CTreeView
{
protected: // create from serialization only
	CLeftView();
	DECLARE_DYNCREATE(CLeftView)

// Attributes
public:
	CDBCheckDoc* GetDocument();

// Operations
public:
	void Redraw();
	bool IterateSequences();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLeftView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
//}}AFX_VIRTUAL

// Implementation
public:
	bool CanIterateSequences();
	virtual ~CLeftView();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void		InsertDatabase(CIPCDatabaseDBCheck* pDatabase);
	HTREEITEM	InsertArchiv(CTreeCtrl& ctrl, HTREEITEM hParent, CIPCArchivRecord* pRecord);
	void		RemoveArchiv(WORD wServerID, WORD wArchivNr);

	HTREEITEM	InsertServer(const CServer* pServer);
	BOOL		DeleteServer(WORD wID);
	
	void		InsertArchivFile(WORD wServerID, CIPCArchivFileRecord* pFile);
	void		InsertSequence(WORD wServerID, CIPCSequenceRecord* pSequence);
	void		RemoveSequence(WORD wServerID, WORD wArchivNr, WORD wSequenceNr);
	
	BOOL		IsServerItem(HTREEITEM hItem);
	BOOL		IsArchivItem(HTREEITEM hItem);
	BOOL		IsSequenceItem(HTREEITEM hItem);
	HTREEITEM	GetServerItem(HTREEITEM hItem);
	HTREEITEM	GetServerItem(WORD wServer);
	HTREEITEM	GetArchivItem(WORD wServer, WORD wArchivNr);

 	CIPCSequenceRecord* GetSequenceRecord(HTREEITEM hSequence);
	CIPCArchivRecord*   GetArchiveRecord(HTREEITEM hArchiv);
	CServer*			GetServer(HTREEITEM hServer);
// Generated message map functions
protected:
	//{{AFX_MSG(CLeftView)
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL				m_bBackupMode;
	CToolTipCtrl		m_ToolTip;
	CIPCArchivRecord*	m_pArchivToolTip;
	CImageList			m_Images;
	CImageList			m_States;
	HTREEITEM			m_hItemFirstSel;
	HTREEITEM			m_hItemIterateSequence;
	HTREEITEM			m_hItemIterateDate;

//	CQueryDialog*			m_pQueryDialog;
//	CSearchResultDialog*	m_pSearchResultDialog;
	DWORD m_dwLastRedraw;
	BOOL  m_bNeedsRedraw;

};

#ifndef _DEBUG  // debug version in LeftView.cpp
inline CDBCheckDoc* CLeftView::GetDocument()
   { return (CDBCheckDoc*)m_pDocument; }
#endif

BOOL GetCheck(CTreeCtrl& ctrl, HTREEITEM hItem);
BOOL SetCheck(CTreeCtrl& ctrl, HTREEITEM hItem,BOOL bFlag,BOOL bDisable = FALSE);
void SetCheckRecurse(CTreeCtrl& ctrl, HTREEITEM hParent, int iState, BOOL bRecurse=TRUE);

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LEFTVIEW_H__804F4122_B354_4CFE_8511_72911C39CE9F__INCLUDED_)
