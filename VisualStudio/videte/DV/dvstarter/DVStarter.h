// DVStarter.h : main header file for the DVSTARTER application
//

#if !defined(AFX_DVSTARTER_H__2E0EDA35_A6F8_11D3_8DFA_004005A11E32__INCLUDED_)
#define AFX_DVSTARTER_H__2E0EDA35_A6F8_11D3_8DFA_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "CApplication.h"

/////////////////////////////////////////////////////////////////////////////
// CDVStarterApp:
// See DVStarter.cpp for the implementation of this class
//

// Registry keys we access
#define KEYHOOK_DRIVER_KEY		_T("System\\CurrentControlSet\\Services\\KeyHook")
#define KEYHOOK_FILTER_KEY		_T("System\\CurrentControlSet\\Control\\Class\\{4D36E96B-E325-11CE-BFC1-08002BE10318}")
#define KEYHOOK					_T("KeyHook")

#define DV_DVSTARTER				  _T("DV\\DVStarter")
#define DV_DVSTARTER_DEBUG      _T("DV\\DVStarter\\Debug")
#define DV_DVSTARTER_AUTO_LOGIN _T("DV\\DVStarter\\AutoLogin")

#define VCD_RUN_MODULE _T("RunVideteCheckDisk")

class CWK_Dongle;
class CInfoDlg;
/////////////////////////////////////////////////////////////////////////////
class CDVStarterApp : public CWinApp,
	public COsVersionInfo
{
	// construction / destruction
public:
	CDVStarterApp();

	// attributes
public:
	BOOL IsReceiver();
	BOOL IsTransmitter();
	BOOL UsePiezo();
	BOOL AmIShell();
	inline const CApplications& GetApplications() const;

	// operations
public:
	void SetInstallationSourcePaths();
	void HideInfoText();
	void ShowInfoText(const CString &sText);
	const WSADATA& GetWSData(){return m_wsaData;}
	const CString& GetHomeDir(){return m_sHomePath;}
	CString GetNextAvailableFixedDrive();//e.g.: "E:"

	void OnPiezoUseChanged();
	void Rdisk(BOOL bWait);
	void Reboot();
	void AddApplication(CApplication*pApp);
	void RemoveApplication(CApplication*pApp);
#if _MFC_VER >= 0x0710
	int	 SetLanguageParameters(UINT uCodePage, DWORD dwCPbits);
#endif

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDVStarterApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDVStarterApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

		// implementation
private:
	CString GetOEMString();
	BOOL	CopyFile(const CString & sFrom, const CString &sTo);
	BOOL	MoveFile(const CString & sFrom, const CString &sTo);
	CString GetOriginalFileName(const CString &sFileName);
	_TCHAR* SearchMultiSz(_TCHAR *Value, DWORD ValueLength, _TCHAR *String);

	void	EnableCtrlAltDel();
	void	DisableCtrlAltDel();
	void	MakeMeShell();
	void	MakeExplorerShell();
	CString	GetWindowsDirectory();
	CString	GetSystemDirectory();
	CString GetTempDirectory();
	BOOL	AdjustPrivileges(LPCTSTR privilege,	DWORD dwAccess = SE_PRIVILEGE_ENABLED);
	BOOL	RebootNotAsShell();
	BOOL	RebootAsShell();
	BOOL    InstallKeyboardDriver();
	BOOL	UnInstallKeyboardDriver();
	void	EnableAutologin();
	void	DisableAutologin();
	void	ScanChk();
	BOOL	DoUpdate();

	BOOL	InitializeDrive(int nDrive);
	void	FormatAllUnformatedFixedVolumes();

	// data member
private:
	CString				m_sHomePath;
	WK_ApplicationId	m_AppID;	
	CWK_Dongle*			m_pDongle;
	WSADATA				m_wsaData; 	
	BOOL				m_bRebootAtExit;
	BOOL				m_bSystemParametersInfo;
	CInfoDlg*			m_pInfoDlg;
	BOOL				m_bUsePiezo;
	CApplications		m_Applications;
};


inline const CApplications& CDVStarterApp::GetApplications() const
{
	return m_Applications;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DVSTARTER_H__2E0EDA35_A6F8_11D3_8DFA_004005A11E32__INCLUDED_)
