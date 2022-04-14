#if !defined(AFX_DlgSearchResult_H__605F3A03_25EA_11D2_B560_00C095EC9EFA__INCLUDED_)
#define AFX_DlgSearchResult_H__605F3A03_25EA_11D2_B560_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgSearchResult.h : header file
//

class CServer;
class CViewArchive;

#include "QueryResult.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgSearchResult dialog
class CDlgSearchResult : public CSkinDialog
{
// Construction
public:
	CDlgSearchResult(CViewArchive* pParent);   // standard constructor
	virtual ~CDlgSearchResult();

public:
	virtual BOOL StretchElements() {return TRUE;}

// Dialog Data
	//{{AFX_DATA(CDlgSearchResult)
	enum  { IDD = IDD_QUERY_RESULT };
	CSkinStatic	m_staticCopied;
	CSkinButton	m_btnDisconnect;
	CSkinListCtrlX	m_Results;
	CSkinStatic	m_staticFound;
	CAnimateCtrl	m_AnimateSearch;
	CString	m_sQuery;
	CString	m_sServers;
	CString	m_sElapsed;
	//}}AFX_DATA

public:
	void AddResult(DWORD dwUserID, WORD wServer,WORD wArchivNr,WORD wSequenceNr, DWORD dwRecordNr,
		           int iNumFields, const CIPCField fields[]);
	void AddResult(WORD wServer, const CIPCPictureRecord &pictData);
	void Disconnect(DWORD dwServerID);
	CString GetSearchName();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSearchResult)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSearchResult)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDblclkListResults(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void InitFields();
	void InsertQueryResult(CQueryResult* pQueryResult);
	void OpenQueryResult(CQueryResult* pQueryResult);
	void OnSearchResult();
	void OnSearchResultCopied();
	void OnSearchFinished();
	void TestForDisconnect();

private:
	CViewArchive*	m_pViewArchive;
	int				m_iFound;
	int				m_iCopied;
	CIPCFields		m_Fields;

	CQueryResultListCS   m_QueryResultsToShow;
	CQueryResultArrayCS  m_QueryResultsInserted;

	DWORD 			m_dwStart;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DlgSearchResult_H__605F3A03_25EA_11D2_B560_00C095EC9EFA__INCLUDED_)
