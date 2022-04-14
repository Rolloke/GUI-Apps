// EndcontrolCheck.h: interface for the CEndcontrolCheck class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENDCONTROLCHECK_H__D2061F27_44F1_11D3_8CBC_004005A1D890__INCLUDED_)
#define AFX_ENDCONTROLCHECK_H__D2061F27_44F1_11D3_8CBC_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EndcontrolRegKeySave.h" // forward declaration
#include <afxtempl.h>

#define DRIVES	1
#define VERSION	2

class CEndcontrolDlg;

class CEndcontrolCheck  
{
public:
	CString GetDrivesChanged();
	BOOL GetSecurityLauncherOK();
	BOOL CheckFolders();
	BOOL DelArchive(CString sAllHarddrives);
	BOOL LoadRegKey(CString sRegPathToLoad, int flag);
	BOOL SaveRegKey(CString sRegPathToSave, int flag);
	BOOL DeleteCriticalRegPath(CString sRegPathToDelete, CString sRegSubKeyToDelete);
	BOOL ImportEndcontrolRegistry();
	void ClimbWindow(CWnd* pWnd);
	void GetLogin();
	BOOL CheckHarddiskDrives(CString cDriveLetter);
	CString RenameHarddiskDrives();
	BOOL CheckSecurityLauncher();
	CEndcontrolCheck(CEndcontrolDlg* edlg);
	virtual ~CEndcontrolCheck();
	CList<CEndcontrolRegKeySave,CEndcontrolRegKeySave> m_ListVersion;
	CList<CEndcontrolRegKeySave,CEndcontrolRegKeySave> m_ListDrives;
	CEndcontrolRegKeySave m_ListSave;
private:

	CEndcontrolDlg* m_pClassDlg;
	BOOL m_bSecurityLauncherOK;

	CString m_sLoginPassword;
	CString m_sLoginName;
	CString m_sDrivesChanged;
};

#endif // !defined(AFX_ENDCONTROLCHECK_H__D2061F27_44F1_11D3_8CBC_004005A1D890__INCLUDED_)
