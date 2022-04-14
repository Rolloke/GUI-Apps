/////////////////////////////////////////////////////////////////////////////
// PROJECT:		QUnit
// FILE:		$Workfile: CMDConfigDlg.h $
// ARCHIVE:		$Archive: /Project/Units/QUnit/CMDConfigDlg.h $
// CHECKIN:		$Date: 14.11.05 11:59 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 14.11.05 11:59 $
// BY AUTHOR:	$Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMDCONFIGDLG_H__D50FF37A_2EAF_11D3_8D55_004005A11E32__INCLUDED_)
#define AFX_CMDCONFIGDLG_H__D50FF37A_2EAF_11D3_8D55_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "CMDConfigDlgBase.h"

/////////////////////////////////////////////////////////////////////////////
// CMDConfigDlg dialog
class CToolTipCtrl;
class CMotionDetection;					   
class CQUnitDlg;
class CMDConfigDlg : public CMDConfigDlgBase
{
// Construction
public:
	CMDConfigDlg(CMotionDetection* pMD, CUDP* pUdp, CWnd* pParent = NULL);   // standard constructor
	~CMDConfigDlg();

	CWnd* GetInlayWnd();
	CWnd* GetDiffWnd();
	BOOL  ShowConfigDlg(HWND hWndParent);
	void  HideConfigDlg();
	void  EnableDlgCtrl();
	
	BOOL  UseAdaptiveMask();
// Dialog Data
	//{{AFX_DATA(CMDConfigDlg)
	enum { IDD = IDD_DLG_MD_CONFIG };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMDConfigDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMDConfigDlg)
	afx_msg void OnClearMask();
	afx_msg void OnInvertPermanentMask();
	afx_msg void OnInvertAdaptiveMask();
	afx_msg void OnSaveMask();
	afx_msg void OnSelectCamera();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLoadMask();
	afx_msg void OnHideMask();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnHistogramm();
	afx_msg void OnSelectMaskSensitivity();
	afx_msg void OnSelectAlarmSensitivity();
	afx_msg void OnFilter();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


//BOOL OnToolTipNotify(UINT id, NMHDR * pNMHDR, LRESULT * pResult);

protected:
	void InitToolTips();

private:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMDCONFIGDLG_H__D50FF37A_2EAF_11D3_8D55_004005A11E32__INCLUDED_)
