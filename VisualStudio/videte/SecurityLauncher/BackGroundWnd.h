// BackGroundWnd.h : header file
//

#ifndef __BackGroundWnd_H__
#define __BackGroundWnd_H__
/////////////////////////////////////////////////////////////////////////////
// CBackGroundWnd window

#define BACKGROUND_WND_CLASS _T("SecurityLauncherBkGndWnd")

class CBackGroundWnd : public CWnd
{
// Construction
public:
	CBackGroundWnd(int nMon);

// Attributes
public:

// Operations
public:
	void SetStatus(int iStatus);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBackGroundWnd)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBackGroundWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBackGroundWnd)
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnWaiClicked(UINT nID);
	afx_msg void OnDestroy();
	afx_msg void OnAppExit();
	afx_msg void OnDatetime();
	afx_msg void OnError();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void DrawLOGO(CDC*pDC); 

public:
	void	SetMonitorRect();
	void	AddResourceButton(UINT nID);
	void	AddApplicationButton(CApplication*pApp);
	void	DeleteApplicationButtons();

public:
	int	m_nOffset;
private:
	CFont	   m_BigFont;
	CFont	   m_SmallFont;
	CFont	   m_MiniFont;
	int		m_iStatus;
	int		m_nMonitor;
public:
};
/////////////////////////////////////////////////////////////////////////////
#endif
