/////////////////////////////////////////////////////////////////////////////
// PROJECT:		QUnit
// FILE:		$Workfile: CMDConfigDlgBase.h $
// ARCHIVE:		$Archive: /Project/Units/QUnit/CMDConfigDlgBase.h $
// CHECKIN:		$Date: 1.12.05 16:09 $
// VERSION:		$Revision: 5 $
// LAST CHANGE:	$Modtime: 1.12.05 14:58 $
// BY AUTHOR:	$Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMDCONFIGDLGBASE_H__3813E1B3_5FDE_11D5_9013_004005A11E32__INCLUDED_)
#define AFX_CMDCONFIGDLGBASE_H__3813E1B3_5FDE_11D5_9013_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CMDConfigDlgBase.h : header file
//
#include <afxcmn.h>

/////////////////////////////////////////////////////////////////////////////
// CMDConfigDlgBase dialog

class CMotionDetection;					   
class CCodec;
class CMDConfigDlgBase : public CDialog
{
// Construction
public:
	CMDConfigDlgBase(CMotionDetection* pMD, CUDP* pUdp, CWnd* pParent, int nResourceID);   // standard constructor

	virtual CWnd* GetInlayWnd() = 0;
	virtual CWnd* GetDiffWnd() = 0;
	virtual int   GetSelectedCamera(){return m_nCamera;}
	virtual BOOL  ShowConfigDlg(HWND hWndParent) = 0;
	virtual	void  HideConfigDlg() = 0;
	virtual void  EnableDlgCtrl() = 0;

	BOOL  IsVisible()
	{
		return m_bVisible;
	}

// Dialog Data
	//{{AFX_DATA(CMDConfigDlgBase)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMDConfigDlgBase)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMDConfigDlgBase)
	afx_msg void OnCancelMode();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	BOOL				m_bVisible;
	int					m_nCamera;
	CMotionDetection*   m_pMD;
	CUDP*				m_pUDP;
	CToolTipCtrl		m_ToolTip;
	CStringArray		m_sTTArray;
	CWnd*				m_pMainWnd;
	int					m_nXInlayOffset;
	int					m_nYInlayOffset;
	int					m_nXMaskOffset;
	int					m_nYMaskOffset;
	CRect				m_rcDlg;
	CRect				m_rcInlay;
	CRect				m_rcMask;
	CRect				m_rcInlayScreen;
	CRect				m_rcMaskScreen;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMDCONFIGDLGBASE_H__3813E1B3_5FDE_11D5_9013_004005A11E32__INCLUDED_)
