#if !defined(AFX_ALARMLISTWINDOW_H__5F483493_8007_11D5_9A22_004005A19028__INCLUDED_)
#define AFX_ALARMLISTWINDOW_H__5F483493_8007_11D5_9A22_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AlarmListWindow.h : header file
//

#include "SmallWindow.h"
#include "AlarmListDialog.h"
#include "QueryResult.h"
/////////////////////////////////////////////////////////////////////////////
// CAlarmListWindow window

class CAlarmListWindow : public CSmallWindow
{
// Construction
public:
	CAlarmListWindow(CServer* pServer, CIPCSequenceRecord* pSequence);

// Attributes
public:
	virtual	CSecID	GetID();
	virtual CString	GetName();
	virtual UINT GetToolBarID();
	virtual void OnSetWindowSize();
	virtual BOOL IsAlarmListWindow();
			BOOL IsInQuery();

// Operations
public:
	virtual BOOL Create(const RECT& rect, CRechercheView* pParentWnd);
	virtual void OnDraw(CDC* pDC);
	virtual	void PopupMenu(CPoint pt);
			void ConfirmRecords(CIPCFields fields,
								CIPCFields records);
			void ConfirmQuery(DWORD dwUserID,
						      WORD wArchivNr,
							  WORD wSequenceNr, 
							  DWORD dwRecordNr,
							  int iNumFields,
							  const CIPCField fields[]);
			void ConfirmQuery();
			void UpdateAlarmList();
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlarmListWindow)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAlarmListWindow();

	// Generated message map functions
protected:
	virtual CString GetTitleText(CDC* pDC);
	//{{AFX_MSG(CAlarmListWindow)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNavigateForward();
	afx_msg void OnUpdateNavigateForward(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnAlarmList();
	afx_msg void OnQueryResult();
	DECLARE_MESSAGE_MAP()

protected:
	CIPCSequenceRecord  m_SequenceRecord;
	CString				m_sName;
	CAlarmListDialog*   m_pAlarmListDialog;
	
	CIPCFields m_Records;
	int m_nTypeLen;
	int m_nDateLen;
	int m_nTimeLen;
	BOOL m_bQuery;
	int  m_nPrintIndex;
	int  m_nItemsPerPage;
	int	 m_nItemHeight;

	CQueryResult* m_pQueryResult;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALARMLISTWINDOW_H__5F483493_8007_11D5_9A22_004005A19028__INCLUDED_)
