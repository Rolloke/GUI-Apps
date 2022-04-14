// CApplication.h: interface for the CApplication class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAPPLICATION_H__11809DA8_A703_11D3_8DFB_004005A11E32__INCLUDED_)
#define AFX_CAPPLICATION_H__11809DA8_A703_11D3_8DFB_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CApplication  
{
public:
	PROCESS_INFORMATION GetProcessInformation(){return m_ProzessInformation;}
	CApplication();
	virtual ~CApplication();

	BOOL Create(const CString &sExePath, WK_ApplicationId id, int nMaxOutstandingConfirms);

	BOOL	IsRunning(BOOL bTrace=FALSE);
	BOOL	WasCreated();
	DWORD	GetExitCode() const;

	BOOL Load(DWORD dwStartFlags, DWORD dwTimeOut=5000);
	void Terminate();
	BOOL MakeSelfCheck();
	void ConfirmSelfcheckMessage();
	void SetAutoTerminate(BOOL bTerminate);
	void SetTemporaryCmdLine(LPCTSTR sCmdLine);
	void SetWaitUntilProcessEnd(BOOL bWait);
	BOOL SendMessageTimeOut(UINT nMsg, WPARAM wParam, LPARAM lParam, UINT uTimeOut, DWORD*pdwResult);
	LRESULT PostMessage(UINT nMsg, WPARAM wParam, LPARAM lParam);
// Muﬂ zum Test mal public sein
	BOOL Shutdown();

protected:
	static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);
	BOOL IsEmpty();
	void Clear();
	void FindOutWindowHandle();
	BOOL FindRunningProcess(BOOL bTrace);
	BOOL SendSelfCheckMessage();

	BOOL Exit();
	BOOL Close();
	BOOL Quit();
//	BOOL Shutdown();

private:
	int					m_nMaxOutstandingConfirms;
	int					m_nOutstandingConfirms;
	WK_ApplicationId	m_AppId;
	PROCESS_INFORMATION m_ProzessInformation;		  
	CString				m_sExeName;
	CString				m_sWkAppName;
	CString				m_sCommandLine; 
	HWND				m_hWnd;
	BOOL				m_bAutoTerminate;
	BOOL				m_bWaitUntilProcessEnd;
	DWORD				m_dwCloseTimeOut;
	DWORD				m_dwQuitTimeOut;
	DWORD				m_dwExitTimeOut;
	DWORD				m_dwExitCode;
	DWORD				m_dwLoadTimeout;
//	DWORD				m_dwSendTC;	// only for trace, not used
};

typedef CApplication* CApplicationPtr;
WK_PTR_ARRAY(CApplications,CApplicationPtr);

#endif // !defined(AFX_CAPPLICATION_H__11809DA8_A703_11D3_8DFB_004005A11E32__INCLUDED_)
