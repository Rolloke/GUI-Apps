#if !defined(AFX_SDIPAGE_H__4329CCC2_189D_11D1_93E0_00C095ECA33E__INCLUDED_)
#define AFX_SDIPAGE_H__4329CCC2_189D_11D1_93E0_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SDIPage.h : header file
//

#include "SVPage.h"
#include "SDIDialog.h"

class CInputList;
/////////////////////////////////////////////////////////////////////////////
// CSDIPage dialog

class CSDIPage : public CSVPage
{
// Construction
public:
	CSDIPage(CSVView* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSDIPage)
	enum { IDD = IDD_SDI };
	CStatic	m_staticCom;
	CScrollBar	m_ScrollBar;
	int		m_iScroll;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSDIPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Overrides
protected:
	virtual BOOL StretchElements();
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();

	virtual void OnNew();
	virtual void OnDelete();
	
	virtual BOOL CanNew();
	virtual BOOL CanDelete();
// Implementation
protected:
	void 	CreateSDIDialogs();
	void	Resize();
	void	PostResize();

	// Generated message map functions
	//{{AFX_MSG(CSDIPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CInputList*	m_pInputList;	// Inputliste mit den Daten
	CIPCFields*	m_pDatabaseFields;
	CSDIDialogArray	m_SDIDialogs;
	CSDIDialogArray	m_Removed;

	friend class CSDIDialog;
public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDIPAGE_H__4329CCC2_189D_11D1_93E0_00C095ECA33E__INCLUDED_)
