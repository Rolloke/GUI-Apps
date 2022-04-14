#if !defined(AFX_EXTERNALFRAMEDLG_H__A2039B24_DEC6_4645_B6B0_766841CE3655__INCLUDED_)
#define AFX_EXTERNALFRAMEDLG_H__A2039B24_DEC6_4645_B6B0_766841CE3655__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExternalFrameDlg.h : header file
//
#include "SmallWindow.h"

/////////////////////////////////////////////////////////////////////////////
// CExternalFrameDlg dialog
class CVisionView;
class CExternalFrameDlg : public CDialog
{
	friend class CVisionView;
	friend class CMainFrame;
// Construction
public:
	CExternalFrameDlg();
	CExternalFrameDlg(CWnd* pParent, CVisionView *, int);   // standard constructor

	void OnViewFullscreen(bool);
	bool RemoveSmallWindow(CSmallWindow*);
	bool AddSmallWindow(CSmallWindow*);
// Access
	int        GetMonitor()          const { return m_nMonitor;      };
	DWORD      GetMonitorFlag()      const { return m_dwMonitorFlag; };
	int        GetNoOfSmallWindows() const { return m_nSmallWindows; };
	static int GetSmallWindows(HWND, CSmallWindows&swa);


// Dialog Data
	//{{AFX_DATA(CExternalFrameDlg)
	enum { IDD = IDD_EXTERNAL_FRAME };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
public:


private:
	void Init();
	void SetWindowSize();
	CVisionView*m_pVisionView;
	static BOOL CALLBACK RemoveSmallWindows(HWND, LPARAM);
	static BOOL CALLBACK EnumSmallWindows(HWND, LPARAM);

protected:
	int   m_iXWin;
	DWORD m_dwMonitorFlag;
	int   m_nMonitor;
	int	m_nSmallWindows;
	CBrush m_Backgroud;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExternalFrameDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExternalFrameDlg)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual void OnOK();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	afx_msg void OnSysCommand(UINT, LPARAM);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXTERNALFRAMEDLG_H__A2039B24_DEC6_4645_B6B0_766841CE3655__INCLUDED_)
