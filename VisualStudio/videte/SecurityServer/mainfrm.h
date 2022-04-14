/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: mainfrm.h $
// ARCHIVE:		$Archive: /Project/SecurityServer/mainfrm.h $
// CHECKIN:		$Date: 25.04.06 17:16 $
// VERSION:		$Revision: 27 $
// LAST CHANGE:	$Modtime: 25.04.06 16:48 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#ifndef __MAINFRAME_H__
#define __MAINFRAME_H__


/////////////////////////////////////////////////////////////////////////////
class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:
	void SetTimer(UINT nElapse);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();

	// control bar embedded members
	CStatusBar		m_wndStatusBar;
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnClose();
	afx_msg void OnTimeChange();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTrayClicked(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLanguageChanged(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	UINT m_uTimerID;
};

/////////////////////////////////////////////////////////////////////////////
#endif
