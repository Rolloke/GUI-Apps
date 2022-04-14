// SystemVerwaltung.h : main header file for the SYSTEMVERWALTUNG application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#define ID_CONNECT_LOW	0x0E00
#define ID_CONNECT_HIGH	0x0EFF

int CheckDigits(CString& s);
class CWK_Dongle;
/////////////////////////////////////////////////////////////////////////////
// CSVApp:
// See SystemVerwaltung.cpp for the implementation of this class
//

class CSVApp : public CWinApp
{
public:
	CSVApp();

public:
	inline CImageList*		GetSmallImage();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL


// Implementation
public:
	BOOL Login();
	BOOL LoginAsSuperVisor();

private:
	void RegisterWindowClass();

	//{{AFX_MSG(CSVApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
private:
	CImageList		m_SmallImage;

public:
	CUser*		 m_pUser;
	CPermission* m_pPermission;
	CString		 m_sLocalWWW;
	BOOL		 m_bRunAnyLinkUnit;
	
	// Dongle
private:
	CWK_Dongle*		m_pDongle;
};
/////////////////////////////////////////////////////////////////////////////
extern CSVApp theApp;
/////////////////////////////////////////////////////////////////////////////
inline CImageList* CSVApp::GetSmallImage()
{
	return &m_SmallImage;
}
/////////////////////////////////////////////////////////////////////////////
