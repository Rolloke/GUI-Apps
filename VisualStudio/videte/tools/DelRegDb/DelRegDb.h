// DelRegDb.h : main header file for the DELREGDB application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CDelRegDbApp:
// See DelRegDb.cpp for the implementation of this class
//

class CDelRegDbApp : public CWinApp
{
public:
	CDelRegDbApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDelRegDbApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
private:
	void DeleteSettings();
	void DeleteDatabase();

	void DeleteWinFiles();
	void DeleteSystemFiles();
	void DeleteMaskAndRegionFiles();

	void Delete_PCI_NT();


	BOOL RunningAsAdministrator();
	BOOL DeleteAllSubKey(HKEY hSecurityKey, LPCTSTR lpszSection);
	HKEY GetCompanyKey();
	HKEY GetNewCompanyKey();
	HKEY GetSectionKey(HKEY hKey, const CString & sSection);
	CString GetString(HKEY hKey, LPCTSTR lpszSection, LPCTSTR lpszKey, LPCTSTR lpszDefault);
	DWORD GetInt(HKEY hKey, LPCTSTR lpszSection, LPCTSTR lpszKey, DWORD dwDefault);
	CString GetOemname();


	//{{AFX_MSG(CDelRegDbApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL	m_bDelDB;
	BOOL	m_bDelReg;
	
	BOOL	m_bIdipJacob;
	BOOL	m_bDtsJacob;
	
	CString m_sDatabaseDrivesToDelete;  //include all Database Drive (from Reg) to delete
};


/////////////////////////////////////////////////////////////////////////////
