// RemoteControlDlg.h : header file
//

#if !defined(AFX_REMOTECONTROLDLG_H__57580DB7_8B70_11D2_8CA3_004005A11E32__INCLUDED_)
#define AFX_REMOTECONTROLDLG_H__57580DB7_8B70_11D2_8CA3_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CHHLA;
class CClientControl;
class CRemoteControlDlg : public CDialog
{
// Construction
public:
	BOOL GetRunStatus(){return m_bRun;}
	CRemoteControlDlg(CWnd* pParent = NULL);	// standard constructor
	~CRemoteControlDlg();
	
	CClientControl* GetClientControl(WORD wI);
	BOOL DoConnection();
	
	void SetStatusText(const CString &sText, BOOL bMiniMize = FALSE);
	void GetStatusText(CString &sText);

	// Dialog Data
	//{{AFX_DATA(CRemoteControlDlg)
	enum { IDD = IDD_REMOTECONTROL_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRemoteControlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CRemoteControlDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg long OnConfirmGetCameraParameter(WPARAM wParam, LPARAM lParam);
	afx_msg long OnSocketClosed(WPARAM wParam, LPARAM lParam);
	afx_msg long OnCameraMissing(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnQueryOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL Init();
	HANDLE Load(const CString &sExePathName, DWORD dwStartFlags);
	BOOL   IsRunning(HANDLE hProzess);
	void   StartErrorThread();
	void   StopErrorThread();

	CClientControl  *m_pClientCtrl[MAX_CLIENTS+1];
	CHHLA			*m_pCHHLA;
	int				m_nServerCount;	// Anzahl der Anmeldeserver
	int				m_nServerIPCnt;	// Servernummer
	BOOL			m_bIsInit;
	HANDLE			m_hServer;		// Prozesshandle des Servers
	HANDLE			m_hMiCoUnit;	// Prozesshandle der MiCoUnit
	BOOL			m_bRun;
	CWinThread*		m_pErrorThread;	
	BOOL			m_bMiniMize;	// TRUE -> Fenster bleibt minimiert.
	int				m_nMaxClients;		// Anzahl der Clients
	BOOL			m_bForceMinimizeDialog; // TRUE -> Der Dialog ist nicht zu sehen. Immer minimiert.
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif // !defined(AFX_REMOTECONTROLDLG_H__57580DB7_8B70_11D2_8CA3_004005A11E32__INCLUDED_)
