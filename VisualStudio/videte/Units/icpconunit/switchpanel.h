#if !defined(AFX_SWITCHPANEL_H__8CFB3CCE_55AC_4A33_8789_032D0EBE7DA9__INCLUDED_)
#define AFX_SWITCHPANEL_H__8CFB3CCE_55AC_4A33_8789_032D0EBE7DA9__INCLUDED_

#include "ICP7000Thread.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SwitchPanel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSwitchPanel dialog
class CMinimizedDlg;
class CIPCInputICPCONUnit;

class CSwitchPanel : public CSkinDialog
{
	friend class CICPI7000Thread;
// Construction
public:
	CSwitchPanel(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSwitchPanel();
// Dialog Data
	//{{AFX_DATA(CSwitchPanel)
	enum eID { IDD = IDD_SWITCH_PANEL };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSwitchPanel)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void CreateAllButtons();
	void DeleteAllButtons();

	// Generated message map functions
	//{{AFX_MSG(CSwitchPanel)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	afx_msg LRESULT OnUserMessage(WPARAM, LPARAM);
	afx_msg LRESULT OnCheckSkinBtn(WPARAM, LPARAM);
	afx_msg void OnCheck(UINT);
	DECLARE_MESSAGE_MAP()

private:
	CSkinButton	m_cFirstButton;

	int    m_nEdgeX;
	int    m_nEdgeY;
	CRect  m_rcDlg;
	
	UINT   m_nMinimizeTimer;
	int    m_nMinimizeCounter;
	int    m_nMinimizeTime;

	int    m_nButtonShape;
	int    m_nSkinColor;

	DWORD  m_dwPushButton;
	DWORD  m_dwPushButtonState;
	UINT   m_nPushButtonTimer;

	CImageList     m_ilRelaisBtn;
	CPtrList       m_ThreadList;
	CMinimizedDlg *m_pMiniDlg;
	CIPCInputICPCONUnit *m_pInput;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SWITCHPANEL_H__8CFB3CCE_55AC_4A33_8789_032D0EBE7DA9__INCLUDED_)
