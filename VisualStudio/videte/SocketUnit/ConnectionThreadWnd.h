#pragma once

class CConnectionThread;
// CConnectionThreadWnd

#define WPARAM_SEND		0x01
#define WPARAM_DISCONNECT	0x02

class CConnectionThreadWnd : public CWnd
{
	DECLARE_DYNAMIC(CConnectionThreadWnd)

public:
	CConnectionThreadWnd(CConnectionThread* pConnectionThread);
	virtual ~CConnectionThreadWnd();

	// Generated message map functions
	//{{AFX_MSG(CConnectionThreadWnd)
protected:
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CConnectionThread* m_pConnectionThread;
protected:
	virtual void PostNcDestroy();
};


