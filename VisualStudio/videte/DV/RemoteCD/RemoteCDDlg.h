// RemoteCDDlg.h : header file
//

#if !defined(AFX_REMOTECDDLG_H__56B7455A_4CC6_11D4_AD95_004005A1D890__INCLUDED_)
#define AFX_REMOTECDDLG_H__56B7455A_4CC6_11D4_AD95_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

extern CRemoteCDApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CRemoteCDDlg dialog

class CRemoteCDDlg : public CDialog
{
// Construction
public:
	CRemoteCDDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CRemoteCDDlg();


public:
	void OnIdle();

protected:
	CString GetPathToPacketCD();
	BOOL GetHookStatusFromReg();
	void RemountCD(BOOL bSendMsg);
	CString GetStringFromTable(CString sFile, int iStringID);
	CString GetStringFromDlg(CString sFile, CString sDlgID);
	void OnFormat();
	void OnFormatStart();
	void OnFormatWaiting();
	void OnFormatClicking();
	void OnFormatFormatting();
	void OnFormatFormatted();
	
	void OnFinalize();
	void CheckCmdLine();

	enum RemoteCDStatus
	{
		RS_UNDEFINED			= 1000,
		RS_REQUEST_STATUS		= 1001,
		RS_REMOUNT				= 1002,
		RS_REMOUNT_AGAIN		= 1003,
		RS_FORMAT				= 1004,
		RS_FINALIZE				= 1005,
		RS_QUIT					= 1006,

		
		RS_WAIT					= 1099,

		RS_NO_STATUS			= 9999
	};

	enum BackupStatus
	{
		BS_NOT_IN_BACKUP		= 0,
		BS_IN_BACKUP			= 1,
		BS_FORMAT_SUCCESS		= 2,
		BS_FORMAT_FAILED		= 3,
		BS_FINALIZE_SUCCESS		= 4,
		BS_FINALIZE_FAILED		= 5,

		BS_REMOUNT_SUCCESS		= 10,
		BS_REMOUNT_FAILED		= 11,

		BS_REQUEST_STATUS		=100,

		BS_NO_STATUS			=9999
	};

	enum RemountStatus
	{
		MOUNT_UNDEFINED		= 3000,
		MOUNT_BEGIN			= 3001,
		MOUNT_IS_REMOUNTING	= 3002,
		MOUNT_WAIT_FOR_END	= 3003,
		MOUNT_SUCCESS		= 3004,
		MOUNT_FAILED		= 3005,

		MOUNT_NO_STATUS		= 9999
	};

	enum FormatStatus
	{
		// TODO NameOfEnum 
		FORMAT_UNDEFINED	= 4000,
		FORMAT_START		= 4001,
		FORMAT_WAITING		= 4002,
		FORMAT_CLICKING		= 4003,
		FORMAT_FORMATTING	= 4004,
		FORMAT_FORMATTED	= 4005
	};

	enum FinalizeStatus
	{
		FS_UNDEFINED					= 2000,
		FS_BEGIN						= 2001,
		FS_WAIT_FOR_FIRST_DLG			= 2002,
		FS_CLICK_BUTTON					= 2003,
		FS_WAIT_FOR_PROGRESS_DLG		= 2004,
		FS_WAIT_FOR_FINALIZE_COMPLETE	= 2005,
		FS_WAIT_FOR_FINALIZE_THREAD		= 2006,
		FS_SUCCESS						= 2007,
		FS_FAILED						= 2008,

		FS_NO_STATUS					= 9999
	};



	BOOL MsgDVClientFormatStatus(BOOL bSuccess);
	BOOL IsFormatted();
	BOOL StartFormatCD();
	BOOL ExecutePacketCD();
	BOOL RequestStatusDVClient();

// Dialog Data
	//{{AFX_DATA(CRemoteCDDlg)
	enum { IDD = IDD_REMOTECD_DIALOG };
	CEdit	m_Edit2;
	CEdit	m_Edit1;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRemoteCDDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CRemoteCDDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	afx_msg long OnReceiveStatus(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()



// Members
private:
	void Init();
	CString NameOfEnum(BackupStatus bs = BS_NO_STATUS, 
						RemoteCDStatus rs = RS_NO_STATUS,
						FinalizeStatus fs = FS_NO_STATUS,
						RemountStatus mount = MOUNT_NO_STATUS);
	DWORD m_dwStart;
	CString m_sBackupDrive;
	CString m_sCmdLine;
	CString m_sDriveLetter;
	CString		m_sInitWndTxtPCD;
	CString		m_sInitFormateButTxt;
	DWORD		m_dwInitEditFieldID;
	HWND		m_hWndPCD;
	HWND		m_hChildWndPCD;
	HWND		m_hFormatButton;
	HWND		m_hDVClient;
	HWND		m_hFinalizeStartDlg;
	HWND		m_hFirstChild;
	HWND		m_hFinalizeUDFDlg;
	CString		m_sEditText;
	CString		m_sInitNewCDName;
	DWORD		m_dwInitFormateButID;
	DWORD		m_dwInitFertigButID;
	DWORD		m_dwInitAbbrechenButID;
	DWORD		m_dwInitFinalizeJaID;
	CString		m_sInitFinalizeDlg;
	CString		m_sCmdLineFirstParam;

	RemoteCDStatus	m_RemoteCDStatus;
	RemountStatus	m_RemountStatus;
	FormatStatus	m_FormatStatus;
	FinalizeStatus	m_FinalizeStatus;
	BOOL			m_bFinalizeComplete;
	BOOL			m_bFinalizeFailed;
	DWORD			m_dwWaitTimeout;

	CExplorerMenuThread m_RemountThread;
	CExplorerMenuThread m_FinalizeThread;
	CString				m_sPathToPacketCD;


	DWORD m_dwWaitFormat;
	DWORD m_dwStartFormat;
	DWORD m_dwWaitAfterFormat;
	DWORD m_dwWaitAfterFinalize;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REMOTECDDLG_H__56B7455A_4CC6_11D4_AD95_004005A1D890__INCLUDED_)
