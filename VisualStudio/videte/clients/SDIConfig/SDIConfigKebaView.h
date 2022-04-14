/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDIConfigKebaView.h $
// ARCHIVE:		$Archive: /Project/Clients/SDIConfig/SDIConfigKebaView.h $
// CHECKIN:		$Date: 10.02.98 11:10 $
// VERSION:		$Revision: 11 $
// LAST CHANGE:	$Modtime: 4.02.98 11:11 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#if !defined(AFX_SDICONFIGKEBAVIEW_H__5A614CA4_5BFB_11D1_9F29_0000C036AC0D__INCLUDED_)
#define AFX_SDICONFIGKEBAVIEW_H__5A614CA4_5BFB_11D1_9F29_0000C036AC0D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "SDIConfigView.h"
#include "SDIConfigKebaDoc.h"
#include "SecID.h"
#include "SDIConfigRecord.h"

/////////////////////////////////////////////////////////////////////////////
// CSDIConfigKebaView form view

class CSDIConfigKebaView : public CSDIConfigView
{
// Constructio/Destruction
protected:
	DECLARE_DYNCREATE(CSDIConfigKebaView)
	CSDIConfigKebaView();           // protected constructor used by dynamic creation
	virtual ~CSDIConfigKebaView();

// Overrides
public:
	virtual	BOOL	ConfigureEnabled();
	virtual	void	Configure();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSDIConfigKebaView)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Implementation
public:
	CSDIConfigKebaDoc*	GetDocument();
protected:
	void				ClearData();
	void				AddSDIGroup(CSDIConfigRecord* pRecord);
	void				UpdateSDIGroup(CSDIConfigRecord* pRecord);
	void				DeleteSDIGroup(CSecID id);
	void				StartElapsedTimer();
	void				UpdateElapsedTimer();
	void				StopElapsedTimer();
	void				AskForDisconnect();

	// Generated message map functions
	//{{AFX_MSG(CSDIConfigKebaView)
	afx_msg void OnSelcancelLbControls();
	afx_msg void OnSelchangeLbControls();
	afx_msg void OnDblclkLbControls();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Form Data
public:
	//{{AFX_DATA(CSDIConfigKebaView)
	enum { IDD = IDD_KEBA_FORM };
	CStatic	m_txtTimeElapsed;
	CStatic	m_txtTime;
	CAnimateCtrl	m_animState;
	CListBox	m_lbControls;
	CEdit	m_editConfigState;
	CString	m_sConfigState;
	CString	m_sTime;
	//}}AFX_DATA
private:
//	CToolTipCtrl	m_ToolTip;
//	CStringArray	m_sTTArray;
	DWORD			m_dwTimeAnim;
	CTime			m_timeTimerStarted;
	UINT			m_uElapsedTimerID;
};

#ifndef _DEBUG  // debug version in CSDIConfigKebaView.cpp
inline CSDIConfigKebaDoc* CSDIConfigKebaView::GetDocument()
   { return (CSDIConfigKebaDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDICONFIGKEBAVIEW_H__5A614CA4_5BFB_11D1_9F29_0000C036AC0D__INCLUDED_)
