/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: Application.h $
// ARCHIVE:		$Archive: /Project/SecurityLauncher/Application.h $
// CHECKIN:		$Date: 11.10.05 11:09 $
// VERSION:		$Revision: 24 $
// LAST CHANGE:	$Modtime: 11.10.05 10:58 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef _Application_H_
#define _Application_H_


class CApplication
{
public:
	CApplication();
	~CApplication();

// Attributes
public:
	inline CString			GetExePathName() const;
	inline CString			GetVersion() const;
	inline CString			GetWorkingDir() const;
	inline BOOL				KeepAlive() const;
	inline BOOL				IsDialogBased() const;
	inline CString			GetName() const;
	inline WK_ApplicationId GetId() const;
	inline DWORD			GetExitCode() const;

	BOOL IsRunning(BOOL bTrace = FALSE, DWORD dwTimeOut=0);
	BOOL IsEmpty();

// Operations
public:
	BOOL Create(WK_ApplicationId Id, BOOL bIsDialogBased,
				DWORD dwStartTimeOut,
				int iExitTry=40, int iCloseTry=40, BOOL bKeepRunning = TRUE);

	BOOL Create(const CString& sPathName, BOOL bIsDialogBased, LPCTSTR pszName=NULL);

	BOOL Create(const CString& sKey , BOOL bIsDialogBased,
				DWORD dwStartTimeOut,
				int iExitTry=40, int iCloseTry=40, BOOL bKeepRunning = TRUE);

	BOOL InitialStart();
	void SetExePathName(CString& appName);
	void SetTemporaryCmdLine(LPCTSTR sCmdLine) { m_sCommandLine = sCmdLine;};
	
	BOOL Execute();
	void Terminate();
	BOOL Close();
	BOOL PostMessage(UINT nMsg, WPARAM wParam, LPARAM lParam);
	BOOL SendMessageTimeOut(UINT nMsg, WPARAM wParam, LPARAM lParam, UINT uTimeOut, DWORD*pdwResult);
	int  SetCloseTry(int nTry);
	void SetWaitUntilProcessEnd(bool bWait, DWORD dwTimeOut=INFINITE);

	BOOL Actualize(BOOL bTrace = TRUE); // returns TRUE if app is still running or empty
	void BringToTop();

	void SetNrOfBChannels(int iNrOfBChannels);
	int  GetNrOfBChannels();
	
	void SetUserID(WORD wUserID);

	void MakeSelfCheck();
	void ConfirmSelfcheckMessage();

	static BOOL HandleCRTMessageBox();

	// Implementation
public:

private:
	BOOL SendSelfCheckMessage();

	void FindRunningProcess(BOOL bTrace = TRUE);
	static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);
	void Clear();
	void FindOutWindowHandle();
	BOOL Shutdown();
	BOOL Exit();
	BOOL Quit();
	BOOL DoesFileExist();
	BOOL HandleGPDialog();
	BOOL HandleGPDialog95();
	BOOL HandleGPDialogNT();
	CString ReadExePathName();
	void    ExtractGPDialogName();
	
private:
	CString	m_ExePathName;
	CString m_sVersion;
	CString m_WorkingDirectory;
	CString	m_GPDialogName;
	CString	m_Key;
	CString	m_WkAppName;
	CString m_sName;

	CStartupInfo		m_StartUpInfo;
	CProcessInformation m_ProzessInformation;
	HWND				m_hWnd;

	CString				m_sCommandLine;
	DWORD				m_StartTime;
	bool				m_bKeepRunning;
	bool				m_bDialogBased;
	bool				m_bLogged;
	bool				m_bWaitUntilProcessEnd;
	int					m_iExitTry;
	int					m_iCloseTry;
	DWORD				m_CreateStartTime;
	int					m_iNrOfBChannels;
	DWORD				m_dwExitCode;

	WK_ApplicationId	m_Id;

	int					m_nOutstandingConfirms;
	int					m_nMaxOutstandingConfirms;
	WORD				m_wUserID;
//	DWORD				m_dwSendTimeTC;	// only for trace, not used
};
inline CString CApplication::GetExePathName() const 	
{ 
	return m_ExePathName;	
}
inline CString CApplication::GetVersion() const
{
	return m_sVersion;
}
inline CString	CApplication::GetWorkingDir() const 
{ 
	return m_WorkingDirectory;
}
inline BOOL  CApplication::KeepAlive() const 
{
	return m_bKeepRunning ? TRUE : FALSE;
}
inline BOOL  CApplication::IsDialogBased() const 
{
	return m_bDialogBased ? TRUE : FALSE;
}
inline CString CApplication::GetName() const
{
	return m_sName;
}
inline WK_ApplicationId CApplication::GetId() const
{
	return m_Id;
}
inline DWORD CApplication::GetExitCode() const
{
	return m_dwExitCode;
}

/////////////////////////////////////////////////////////////////////////////
#endif