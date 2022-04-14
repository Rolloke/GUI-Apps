/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDITestView.h $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/SDITest/SDITestView.h $
// CHECKIN:		$Date: 13.12.99 11:43 $
// VERSION:		$Revision: 23 $
// LAST CHANGE:	$Modtime: 13.12.99 11:40 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#if !defined(AFX_SDITESTVIEW_H__2D22C523_A2C7_11D1_9F29_0000C036AC0D__INCLUDED_)
#define AFX_SDITESTVIEW_H__2D22C523_A2C7_11D1_9F29_0000C036AC0D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "SDITestDoc.h"

/////////////////////////////////////////////////////////////////////////////
class CStandardEntriesDlg;

/////////////////////////////////////////////////////////////////////////////
// Timer_IDs fuer die einzelnen Views
#define TIMER_UNKNOWN		0
#define TIMER_DTP			1
#define TIMER_KEBA			2
#define TIMER_NCR			3
#define TIMER_ENGEL			4
#define TIMER_SNI			5
#define TIMER_IBM			6
#define TIMER_AVM			7
#define TIMER_SNI_COM		8
#define TIMER_KEBA_PASADOR	9
#define TIMER_ASCOM			10
#define TIMER_RONDO			11
#define TIMER_DIALOGBANK	12
#define TIMER_DEPROS		13

/////////////////////////////////////////////////////////////////////////////
class CSDITestView : public CFormView
{
// Construction/Destruction
	DECLARE_DYNCREATE(CSDITestView)
public:
	CSDITestView(UINT uIDTemplate);
protected: // create from serialization only
	CSDITestView();

// Attributes
public:
	inline CSDITestDoc* GetDocument();
	const	CSize&	GetOptimalSize() const;

// Operations
public:
protected:

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSDITestView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL
public:
	virtual	void	ShowHide();
protected:
	virtual	void	EnlargeDlg();
	virtual	void	InitializeData();

// Implementation
public:
	virtual ~CSDITestView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void	ConstructorInit(); // Funktion zum Initialisieren der Member-Variablen

// Generated message map functions
protected:
	//{{AFX_MSG(CSDITestView)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	//{{AFX_DATA(CSDITestView)
	enum { IDD = IDD_SDITEST_FORM };
	//}}AFX_DATA
protected:
	CSize			m_sizeOptimal;
	CToolTipCtrl	m_ToolTip;
	CStringArray	m_sTTArray;

	CStandardEntriesDlg*	m_pStandardEntriesDlg;
	UINT					m_uTimerID;
	UINT					m_uCounter;
private:
};
/////////////////////////////////////////////////////////////////////////////
#ifndef _DEBUG  // debug version in SDITestView.cpp
inline CSDITestDoc* CSDITestView::GetDocument()
   { return (CSDITestDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDITESTVIEW_H__2D22C523_A2C7_11D1_9F29_0000C036AC0D__INCLUDED_)
