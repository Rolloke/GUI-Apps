#pragma once

#define VIDEOSTATE_TIMER 12

/////////////////////////////////////////////////////////////////////////////
// CVideoStateCheckThread

class CVideoStateCheckThread : public CWK_WinThread
{
	DECLARE_DYNAMIC(CVideoStateCheckThread)

//protected:
	CVideoStateCheckThread(const CString&sName);           // protected constructor used by dynamic creation
public:
	virtual ~CVideoStateCheckThread();

	// Attribute
public:

	// Implementation
public:

protected:

	//{{AFX_VIRTUAL(CDirectShowThread)
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CDirectShowThread)
//	afx_msg void OnConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	//{{AFX_DATA(CDirectShowThread)
protected:
	//}}AFX_DATA
public:


protected:
	//CDsCamera* m_pIpCam;
	DWORD	m_dwCurrentVideoState[MAX_IP_CAMERAS];
};

