#if !defined(AFX_WndAlarmList_H__5F483493_8007_11D5_9A22_004005A19028__INCLUDED_)
#define AFX_WndAlarmList_H__5F483493_8007_11D5_9A22_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WndAlarmList.h : header file
//

#include "WndSmall.h"
#include "DlgAlarmList.h"
#include "QueryResult.h"
/////////////////////////////////////////////////////////////////////////////
// CWndAlarmList window

#define OPTIMIZE_BY_HEADER		0	
#define OPTIMIZE_BY_CONTENT		1
#define OPTIMIZE_TO_WND_WIDTH	0x0001

class CWndAlarmList : public CWndSmall
{
	friend class CDlgAlarmList;
	DECLARE_DYNAMIC(CWndAlarmList)		
// Construction
public:
	CWndAlarmList(CServer* pServer, CIPCSequenceRecord* pSequence);

// Attributes
public:
	CString	GetSection();
	WORD	GetArchivNr();

// Operations
public:

			void ConfirmRecords(const CIPCFields& fields,
								const CIPCFields& records);
			void ConfirmQuery(DWORD dwUserID,
						      WORD wArchivNr,
							  WORD wSequenceNr, 
							  DWORD dwRecordNr,
							  int iNumFields,
							  const CIPCField fields[]);
			void ConfirmQuery();

			BOOL GetAlarm(int i, CString& sType, CString& sDate, CString& sTime, CString& sMS,
						  CSecID& idInput, CSecID& idOutput,CSecID& idArchive, CStringArray& sa);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndAlarmList)
	virtual BOOL Create(const RECT& rect, CViewIdipClient* pParentWnd);
	virtual void OnDraw(CDC* pDC);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual	void PopupMenu(CPoint pt);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual	CSecID	GetID();
	virtual CString	GetName();
	virtual UINT GetToolBarID();
	virtual void OnSetWindowSize();
			BOOL IsInQuery();
	virtual	BOOL CanPrint();
	//}}AFX_VIRTUAL

// Implementation
protected:
public:
	virtual ~CWndAlarmList();

	// Generated message map functions
protected:
	virtual CString GetTitleText(CDC* pDC);
	//{{AFX_MSG(CWndAlarmList)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNavigateForward();
	afx_msg void OnUpdateNavigateForward(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSmallContext(CCmdUI* pCmdUI);
	afx_msg void OnQueryResult();
	afx_msg void OnViewOptimizeColumnWidth();
	afx_msg void OnUpdateViewOptimizeColumnWidth(CCmdUI *pCmdUI);
	afx_msg void OnViewOptimizeByContent();
	afx_msg void OnUpdateViewOptimizeByContent(CCmdUI *pCmdUI);
	afx_msg void OnViewOptimizeByHeader();
	afx_msg void OnUpdateViewOptimizeByHeader(CCmdUI *pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnViewOptimizeToWndWidth();
	afx_msg void OnUpdateViewOptimizeToWndWidth(CCmdUI *pCmdUI);
	afx_msg void OnViewRefresh();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CIPCSequenceRecord  m_SequenceRecord;
	CString				m_sName;
	CDlgAlarmList*		m_pDlgAlarmList;
	CIPCFields m_Fields;
	CIPCFields m_Records;
	BOOL m_bQuery;
	int  m_nItemsPerPage;
	int	 m_nItemHeight;
	int	 m_nPageColumns;
	CDWordArray m_dwaColumnWidth;
	union
	{
		struct
		{
			WORD wLow, wHigh;
		}lh;
		DWORD dw;
	} m_OptimizeColumnWidth;

	CQueryResult* m_pQueryResult;
public:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WndAlarmList_H__5F483493_8007_11D5_9A22_004005A19028__INCLUDED_)
