/////////////////////////////////////////////////////////////////////////////
// PROJECT:		LogView
// FILE:		$Workfile: LogViewView.h $
// ARCHIVE:		$Archive: /Project/Tools/LogView/LogViewView.h $
// CHECKIN:		$Date: 8.03.06 11:11 $
// VERSION:		$Revision: 16 $
// LAST CHANGE:	$Modtime: 8.03.06 10:47 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGVIEWVIEW_H__4DB08FEF_BC2E_11D2_A9C4_004005A11E32__INCLUDED_)
#define AFX_LOGVIEWVIEW_H__4DB08FEF_BC2E_11D2_A9C4_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLogViewDoc;
class CLogViewCntrItem;

class CLogViewView : public CRichEditView
{
    struct TP { enum eTimeParameter
    {
        MiliSecond,
        MicroSecond,
        Second,
        Minute,
        Hour,
        hour,
        DayOfMonth,
        DayOfYear,
        DayOfWeek,
        WeekOfYear,
        Month,
        AbrMonthName,
        MonthName,
        Year,
        year,
        PM,
        Count=10,
        StringBuffer=32
    };};

protected: // create from serialization only
	CLogViewView();
	DECLARE_DYNCREATE(CLogViewView)

// Attributes
public:
	CLogViewDoc* GetDocument();

// Operations
public:
	CString GetSelText(HWND hWnd) const;
#ifdef _DEBUG
	void OnEditFindReplace(BOOL bFindOnly);
#endif
	BOOL OnEditRepeat();
	void DeleteContentThreadSave();
	void EndSearchInCompressedThread();
	void Stream(CArchive& ar, BOOL bSelection);
	static DWORD CALLBACK ReadUnicodeStreamCallBack(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogViewView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnTextNotFound(LPCTSTR lpszFind);
	virtual void OnFindNext(LPCTSTR lpszFind, BOOL bNext, BOOL bCase, BOOL bWord);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
protected:
	BOOL		GetTimeOfLine(int nLine, long&nDay, long&nSec);
	CHARRANGE	GetTimePositionOfLine(long *pnLine=NULL);
	static int __cdecl SortCharrangePtr(const void *p1, const void *p2);
	int			BSearchTime(long nStart, long nEnd, long nSyncDay, long nSyncSec);
    void        DetermineTimeType();
    void        ParseFormat(const CString& aTimeFmt, CString& aFmt, std::vector<int>& aParameter, int &aTimeStringLength);

public:
	int			OnViewSyncTime(long&nSyncDay, long&nSyncSec);
	BOOL		IsTimeAvailable(long nSyncDay, long nSyncSec);
	virtual ~CLogViewView();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


// Generated message map functions
protected:
	//{{AFX_MSG(CLogViewView)
	afx_msg void OnDestroy();
	afx_msg void OnRefresh();
	afx_msg void OnEditSearchInCompressed();
	afx_msg void OnEditFind();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnUpdateSC_CLOSE(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewShowSyncpoint(CCmdUI *pCmdUI);
	afx_msg void OnViewShowSyncpoint();
	afx_msg void OnUpdateEditSearchTimeValue(CCmdUI *pCmdUI);
	afx_msg void OnEditSearchTimeValue();
	afx_msg void OnViewSetBookMark();
	afx_msg void OnUpdateViewSetBookMark(CCmdUI *pCmdUI);
	afx_msg void OnViewNextBookmark();
	afx_msg void OnUpdateViewNextBookmark(CCmdUI *pCmdUI);
	afx_msg void OnViewPrevBookmark();
	afx_msg void OnUpdateViewPrevBookmark(CCmdUI *pCmdUI);
	afx_msg void OnViewDeleteBookmarks();
	afx_msg void OnUpdateViewDeleteBookmarks(CCmdUI *pCmdUI);
	afx_msg LRESULT OnUser(WPARAM, LPARAM);
	afx_msg void OnUpdateEditFind(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	int	 m_nCurrentItem;
	BOOL m_bNext;
protected:
	CString m_sFindText;
	BOOL m_bSearchInCompressed;
	BOOL m_bFoundInCompressed;
	CHARRANGE m_crPostSelection;
	CPtrArray m_BookMarks;
	POSITION m_posBookmark;
	CString mTimeFormat;
    std::vector<int> mTimeParamter;
    int mP[TP::Count];
    CString mStringParam[TP::Count];
    void*mPtr[TP::Count];
    int mTimeCharacters;
};

#ifndef _DEBUG  // debug version in LogViewView.cpp
inline CLogViewDoc* CLogViewView::GetDocument()
   { return (CLogViewDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGVIEWVIEW_H__4DB08FEF_BC2E_11D2_A9C4_004005A11E32__INCLUDED_)
