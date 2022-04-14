/////////////////////////////////////////////////////////////////////////////
// PROJECT:		JaCobDll
// FILE:		$Workfile: CMDConfigDlg.h $
// ARCHIVE:		$Archive: /Project/Units/GenericUnit/CMDConfigDlg.h $
// CHECKIN:		$Date: 23.11.00 10:41 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 1.09.00 8:54 $
// BY AUTHOR:	$Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMDCONFIGDLG_H__D50FF37A_2EAF_11D3_8D55_004005A11E32__INCLUDED_)
#define AFX_CMDCONFIGDLG_H__D50FF37A_2EAF_11D3_8D55_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxcmn.h>
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CMDConfigDlg dialog
class CToolTipCtrl;
class CMotionDetection;					   
class CCodec;
class CMDConfigDlg : public CDialog
{
// Construction
public:
	CMDConfigDlg(CMotionDetection* pMD, CCodec* pCodec, CWnd* pParent = NULL);   // standard constructor
	~CMDConfigDlg();

	CWnd* GetInlayWnd();
	CWnd* GetDiffWnd();
	WORD  GetSelectedCamera(){return m_wSubID;}
	void  ShowConfigDlg();
	void  HideConfigDlg();
	void  EnableDlgCtrl();
	void  ClientToScreen(int nID, int& nX, int& nY);
	
	BOOL  IsVisible()
	{
		return m_bVisible;
	}
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
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSelectCamera();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
	afx_msg void OnLoadMask();
	afx_msg void OnHideMask();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnHistogramm();
	afx_msg void OnSelectMaskSensitivity();
	afx_msg void OnSelectAlarmSensitivity();
	afx_msg void OnFilter();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

//BOOL OnToolTipNotify(UINT id, NMHDR * pNMHDR, LRESULT * pResult);

protected:
	void InitToolTips();

private:
	CMotionDetection*   m_pMD;
	CCodec*				m_pCodec;
	CToolTipCtrl		m_ToolTip;
	CStringArray		m_sTTArray;

	WORD				m_wSubID;
	int					m_nXInlayOffset;
	int					m_nYInlayOffset;
	int					m_nXMaskOffset;
	int					m_nYMaskOffset;
	CRect				m_rcDlg;
	CRect				m_rcInlay;
	CRect				m_rcMask;
	CRect				m_rcInlayScreen;
	CRect				m_rcMaskScreen;

	BOOL				m_bVisible;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMDCONFIGDLG_H__D50FF37A_2EAF_11D3_8D55_004005A11E32__INCLUDED_)
