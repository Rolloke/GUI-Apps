// ClientControl.h: interface for the CClientControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIENTCONTROL_H__62F57811_8DA4_11D2_8CA4_004005A11E32__INCLUDED_)
#define AFX_CLIENTCONTROL_H__62F57811_8DA4_11D2_8CA4_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CClientControl  
{
public:
	CClientControl(const CString &sExePathName, int nClientNr, HWND hWndRemoteControl);
	virtual ~CClientControl();

	void DoRequestSetWindowPosition(int nX, int nY, int nW, int nH);
	void DoRequestSetInputSource(int nViewType, BOOL bRefrsh);
	void DoRequestSavePicture(const CString &sFileName);
	void DoRequestSetCameraParameter(WORD wBrightness, WORD wContrast, WORD wSaturation);
	void DoRequestCameraParameter();
	void DoRequestCheckCamera();
	BOOL IsValid(){return m_bOk;}
	BOOL IsRunning();

private:
	BOOL LoadClient();
	HWND FindWindowHandle(DWORD dwProzessID);
	CWnd* ClimbWindow(CWnd* pWnd, DWORD pProzessID);
	
	CString m_sCmdLine;
	CString m_sWorkingDirectory;
	CString m_sExePathName;
	BOOL	m_bWait;
	DWORD	m_dwStartFlags;
	DWORD	m_dwStartTime;
	HWND	m_hWndClient;
	HWND	m_hWndRemoteControl;
	int		m_nClientNr;
	BOOL	m_bOk;
	DWORD	m_dwPID;
};

#endif // !defined(AFX_CLIENTCONTROL_H__62F57811_8DA4_11D2_8CA4_004005A11E32__INCLUDED_)
