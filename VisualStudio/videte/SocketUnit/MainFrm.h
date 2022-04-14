// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////
#ifdef _DTS
class CIPBook;
#endif

#ifdef _BETA
class CIPNetBook;
#endif

#include "DlgStatistic.h"

class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	void GetLocalIPList(int &iNrOfIP);
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members

// Generated message map functions
protected:

	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnAskCbFormatName(UINT nMaxCount, LPTSTR lpszString);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTrayClicked(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLanguageChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnViewBitrate();
	afx_msg void OnUpdateViewBitrate(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL m_bIsServer;
	unsigned long m_lLocalIp[10];
	int m_iNrOfIP;
	CStatusBar  m_wndStatusBar;
	CDlgStatistic m_DlgStatistic;
	UINT_PTR	m_uStatisticTimer;

#ifdef _DTS
	UINT_PTR		m_uIpBookTimer;
	CIPBook*		m_pIpBook[10] ;
#endif

#ifdef _BETA
	CIPNetBook*		m_pIpBook;
#endif
public:
};

/////////////////////////////////////////////////////////////////////////////
