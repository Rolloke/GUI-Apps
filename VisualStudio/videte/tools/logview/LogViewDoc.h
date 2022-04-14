/////////////////////////////////////////////////////////////////////////////
// PROJECT:		LogView
// FILE:		$Workfile: LogViewDoc.h $
// ARCHIVE:		$Archive: /Project/Tools/LogView/LogViewDoc.h $
// CHECKIN:		$Date: 3/14/06 4:32p $
// VERSION:		$Revision: 17 $
// LAST CHANGE:	$Modtime: 3/14/06 4:03p $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGVIEWDOC_H__4DB08FED_BC2E_11D2_A9C4_004005A11E32__INCLUDED_)
#define AFX_LOGVIEWDOC_H__4DB08FED_BC2E_11D2_A9C4_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _THREAD_SEARCH 1

class CLogViewView;
class CLogViewDoc : public CRichEditDoc
{
protected: // create from serialization only
	CLogViewDoc();
	DECLARE_DYNCREATE(CLogViewDoc)

// Attributes
public:

// Operations
public:
	void StartSearchInCompressedThread(CLogViewView*pView);
	BOOL GetCompressedFiles(CString &sPath, CStringArray* par);
	BOOL SelectCompressedFileByTime(CLogViewView*pView, int nDay, int nSec);
	BOOL IsSearchThreadRunning();
	void StopSearchThread();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogViewDoc)
protected:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void DeleteContents();
	public:
	virtual CRichEditCntrItem* CreateClientItem(REOBJECT* preo) const;
	virtual BOOL IsModified();
	virtual CFile* GetFile(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLogViewDoc();
	void	Refresh();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	static int __cdecl CLogViewDoc::CompareCStringRef(const void *p1, const void *p2);
	void	QSortStrings(CStringArray &ar);
	static	UINT SearchThread(LPVOID pParam);
	void	LoadDocumentThreadSave(LPCTSTR lpszPathName);
	void	OnEditSearchInCompressed(CLogViewView*pView);

// Generated message map functions
protected:
	//{{AFX_MSG(CLogViewDoc)
	afx_msg void OnUpdateEditSearchInCompressed(CCmdUI *pCmdUI);
	afx_msg void OnViewPrevCompressed();
	afx_msg void OnUpdateViewPrevCompressed(CCmdUI *pCmdUI);
	afx_msg void OnViewNextCompressed();
	afx_msg void OnUpdateViewNextCompressed(CCmdUI *pCmdUI);
	afx_msg void OnEditStopSearchInCompressed();
	afx_msg void OnUpdateEditStopSearchInCompressed(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL DecompressLGZFile(const CString & sName, const CString &sNewName);
	BOOL ReadFile(const CString &sFileName, CString &sTempFileName, BOOL bAppend);
	BOOL LoadList(const CStringArray &FileNames);

	CString		m_sTempFileName;
	CString		m_sFileName;
	CString		m_sCurrent;
	BOOL		m_bDecompress;
	volatile CWinThread *m_pSearchThread;
	volatile BOOL		m_bRun;
public:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGVIEWDOC_H__4DB08FED_BC2E_11D2_A9C4_004005A11E32__INCLUDED_)
