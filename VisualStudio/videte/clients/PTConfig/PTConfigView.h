#if !defined(AFX_PTCONFIGVIEW_H__9297D98F_3517_11D1_B8C8_0060977A76F1__INCLUDED_)
#define AFX_PTCONFIGVIEW_H__9297D98F_3517_11D1_B8C8_0060977A76F1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PTConfigView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPTConfigView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CPTConfigDoc ;

class CPTConfigView : public CFormView
{
protected:
	CPTConfigView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPTConfigView)

// Form Data
public:
	//{{AFX_DATA(CPTConfigView)
	enum { IDD = IDD_PT_FORM };
	CButton	m_ctGroupBasics;
	CButton	m_ctVideoSignal3;
	CButton	m_ctVideoSignal2;
	CButton	m_ctVideoSignal1;
	CButton	m_ctGroupActions;
	CButton	m_ctGroupCameras;
	CButton	m_ctGroupAlarms;
	CButton	m_ctDoSave;
	CAnimateCtrl	m_ctAnimate2;
	CAnimateCtrl	m_ctAnimate1;
	CString	m_sAlarmName1;
	CString	m_sAlarmName2;
	CString	m_sAlarmName3;
	CString	m_sCamName1;
	CString	m_sCamName2;
	CString	m_sCamName3;
	BOOL	m_bHasVideoSignal1;
	BOOL	m_bHasVideoSignal2;
	BOOL	m_bHasVideoSignal3;
	BOOL	m_bColorCam1;
	BOOL	m_bColorCam2;
	BOOL	m_bColorCam3;
	CString	m_sBoxName;
	CString	m_sBoxNumber;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPTConfigView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CPTConfigView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CPTConfigView)
	afx_msg void OnResetData();
	afx_msg void OnDoSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void ResetAllData();
	inline CPTConfigDoc *GetDoc()
	{
		return (CPTConfigDoc *)GetDocument();	// OOPS
	}
private:
	BOOL m_bInSave;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PTCONFIGVIEW_H__9297D98F_3517_11D1_B8C8_0060977A76F1__INCLUDED_)
