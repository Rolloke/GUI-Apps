#if !defined(AFX_DLGEXTERNALFRAME_H__A2039B24_DEC6_4645_B6B0_766841CE3655__INCLUDED_)
#define AFX_DLGEXTERNALFRAME_H__A2039B24_DEC6_4645_B6B0_766841CE3655__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgExternalFrame.h : header file
//
#include "WndSmall.h"
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgExternalFrame dialog
class CViewIdipClient;

class CDlgExternalFrame : public CSkinDialog
{
	DECLARE_DYNAMIC(CDlgExternalFrame)
	friend class CViewIdipClient;
	friend class CMainFrame;
// Construction
public:
	CDlgExternalFrame();
	CDlgExternalFrame(CWnd* pParent, CViewIdipClient *, int);   // standard constructor

	void OnViewFullscreen(bool);
	bool RemoveWndSmall(CWndSmall*);
	bool AddWndSmall(CWndSmall*);
// Access
	int        GetMonitor()          const { return m_nMonitor;      };
	DWORD      GetMonitorFlag()      const { return m_dwMonitorFlag; };
	int        GetNoOfSmallWindows() const { return m_nSmallWindows; };
	static int GetSmallWindows(HWND, CWndSmallArray&swa);


// Dialog Data
	//{{AFX_DATA(CDlgExternalFrame)
	enum  { IDD = IDD_EXTERNAL_FRAME };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
public:


private:
	void Init();
	void SetWindowSize();
	CViewIdipClient*m_pViewIdipClient;
	static BOOL CALLBACK RemoveSmallWindows(HWND, LPARAM);
	static BOOL CALLBACK EnumSmallWindows(HWND, LPARAM);
//	static BOOL CALLBACK SendMessageToDescendants(HWND, LPARAM);

protected:
	int   m_iXWin;
	DWORD m_dwMonitorFlag;
	int   m_nMonitor;
	int	m_nSmallWindows;
	CBrush m_Background;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExternalFrame)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgExternalFrame)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual void OnOK();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSysCommand(UINT, LPARAM);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

typedef CDlgExternalFrame* CDlgExternalFramePtr;
WK_PTR_ARRAY(CDlgExternalFrameArray, CDlgExternalFramePtr);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXTERNALFRAME_H__A2039B24_DEC6_4645_B6B0_766841CE3655__INCLUDED_)
