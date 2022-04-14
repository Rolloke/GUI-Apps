#if !defined(AFX_SEARCHRESULTDIALOG_H__605F3A03_25EA_11D2_B560_00C095EC9EFA__INCLUDED_)
#define AFX_SEARCHRESULTDIALOG_H__605F3A03_25EA_11D2_B560_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SearchResultDialog.h : header file
//

class CServer;
class CObjectView;

#include "QueryResult.h"
/////////////////////////////////////////////////////////////////////////////
// CSearchResultDialog dialog

class CSearchResultDialog : public CWK_Dialog
{
// Construction
public:
	CSearchResultDialog(CObjectView* pParent);   // standard constructor
	virtual ~CSearchResultDialog();

public:
	virtual BOOL StretchElements() {return TRUE;}

// Dialog Data
	//{{AFX_DATA(CSearchResultDialog)
	enum { IDD = IDD_QUERY_RESULT };
	CStatic	m_staticCopied;
	CButton	m_btnDisconnect;
	CListCtrl	m_Results;
	CStatic	m_staticFound;
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
	//{{AFX_VIRTUAL(CSearchResultDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSearchResultDialog)
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
	CObjectView*	m_pObjectView;
	int				m_iFound;
	int				m_iCopied;
	CIPCFields		m_Fields;

	CQueryResultListCS   m_QueryResultsToShow;
	CQueryResultArrayCS  m_QueryResultsInserted;

	DWORD 			m_dwStart;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEARCHRESULTDIALOG_H__605F3A03_25EA_11D2_B560_00C095EC9EFA__INCLUDED_)
