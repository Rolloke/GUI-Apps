#pragma once
// FileView.h : header file
//
class CDirContent;
/////////////////////////////////////////////////////////////////////////////
// CFileView view

class CFileView : public CListView
{
protected:
	CFileView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CFileView)

// Attributes
public:

// Operations
public:
	void	AddFile(const CString& sDir, LPWIN32_FIND_DATA pFFD);
	void    DeleteFile(const CString& sFile);
	void    Clear();
	
	void    SetDirInfo(const CDirContent* pDirContent);
	void	InsertRegValue(const CString& sReginfo);
protected:
	void	SetColumnText(int nColumn, UINT nID);
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CFileView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CFileView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFileDelete();
	afx_msg void OnFileExecute();
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFileReceive();
	afx_msg void OnUpdateFileDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileExecute(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileReceive(CCmdUI* pCmdUI);
	afx_msg void OnRegEdit();
	afx_msg void OnUpdateRegEdit(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CString m_Directory;
	BOOL m_bResizing;
	BOOL m_bRegistryValues;
public:
};

/////////////////////////////////////////////////////////////////////////////
