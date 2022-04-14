/////////////////////////////////////////////////////////////////////////////
// PROJECT:		StressIt
// FILE:		$Workfile: StressItDlg.h $
// ARCHIVE:		$Archive: /Project/Tools/StressIt/StressItDlg.h $
// CHECKIN:		$Date: 5.08.98 9:56 $
// VERSION:		$Revision: 7 $
// LAST CHANGE:	$Modtime: 5.08.98 9:56 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRESSITDLG_H__AA88D717_ADBF_11D1_9C64_00A024D29804__INCLUDED_)
#define AFX_STRESSITDLG_H__AA88D717_ADBF_11D1_9C64_00A024D29804__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CStressItDlg dialog

class CStressItDlg : public CDialog
{
public:
	BOOL			m_bRun;
	int				m_nSleepSliderPos;
	int				m_nWriteSliderPos;	
// Construction
public:
	CStressItDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CStressItDlg)
	enum { IDD = IDD_STRESSIT_DIALOG };
	CSliderCtrl	m_WriteSlider;
	CSliderCtrl	m_SleepSlider;
	CString	m_sAvailPhysMemory;
	CString	m_sAvailPageMemory;
	CString	m_sUsedPhysMemory;
	CString	m_sUsedPageMemory;
	CString	m_sMemoryLoad;
	CString	m_sAllocMemory;
	CString	m_sAvailSumme;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStressItDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CStressItDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnAllocate01MB();
	afx_msg void OnAllocate100MB();
	afx_msg void OnAllocate10MB();
	afx_msg void OnAllocate1MB();
	afx_msg void OnAllocate50MB();
	afx_msg void OnReleaseAll();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSetThreadPriorityHigh();
	afx_msg void OnSetThreadPriorityMid();
	afx_msg void OnSetThreadPriorityLow();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	MEMORYSTATUS	m_Mem;
	DWORD			m_dwAllocMemory;
	CPtrList		m_List;
	CWinThread*		m_pStressSleepThread;
	CWinThread*		m_pStressWriteThread;

	void UpdateDialog();
	BOOL AllocateIt(DWORD dwDeltaSize);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STRESSITDLG_H__AA88D717_ADBF_11D1_9C64_00A024D29804__INCLUDED_)
