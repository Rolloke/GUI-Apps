/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDIConfigView.h $
// ARCHIVE:		$Archive: /Project/Clients/SDIConfig/SDIConfigView.h $
// CHECKIN:		$Date: 10.02.98 11:10 $
// VERSION:		$Revision: 10 $
// LAST CHANGE:	$Modtime: 4.02.98 11:12 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#if !defined(AFX_SDICONFIGVIEW_H__3716C0F2_5B7E_11D1_8143_EAF88A4ACC28__INCLUDED_)
#define AFX_SDICONFIGVIEW_H__3716C0F2_5B7E_11D1_8143_EAF88A4ACC28__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "SDIConfigDoc.h"

class CSDIConfigView : public CFormView
{
// Construction/Destruction
	DECLARE_DYNCREATE(CSDIConfigView)
public:
	CSDIConfigView(UINT uIDTemplate);
	virtual ~CSDIConfigView();
protected: // create from serialization only
	CSDIConfigView();

// Attributes
public:
	CSDIConfigDoc*	GetDocument();
	const	CSize&	GetOptimalSize() const;

// Overrides
	virtual	BOOL	ConfigureEnabled();
	virtual	void	Configure();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSDIConfigView)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void	ClearData();
	void	AddSDIGroup(CSecID id, CString& sComment);
	void	DeleteSDIGroup(CSecID id);
private:
	void	ConstructorInit(); // Funktion zum Initialisieren der Member-Variablen

// Generated message map functions
protected:
	//{{AFX_MSG(CSDIConfigView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Data
public:
	//{{AFX_DATA(CSDIConfigView)
	enum{ IDD = IDD_SDICONFIG_FORM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
protected:
	CSize			m_sizeOptimal;
	CToolTipCtrl	m_ToolTip;
	CStringArray	m_sTTArray;
private:
};

#ifndef _DEBUG  // debug version in SDIConfigView.cpp
inline CSDIConfigDoc* CSDIConfigView::GetDocument()
   { return (CSDIConfigDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDICONFIGVIEW_H__3716C0F2_5B7E_11D1_8143_EAF88A4ACC28__INCLUDED_)
