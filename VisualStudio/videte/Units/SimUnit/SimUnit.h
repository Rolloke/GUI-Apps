// SimUnit.h : main header file for the SIMUNIT application
// w+k Sample Application
// Author : Uwe Freiwald
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#define ID_CONNECTION_INPUT 0x7000
#define ID_HARDWARE_INPUT	0x7001
#define ID_EDGE_INPUT		0x7002
#define ID_ENABLE_INPUT		0x7003
#define ID_RESET_INPUT		0x7004
#define ID_DISCONNECT_INPUT	0x7005

#define ID_CONNECTION_OUTPUT	0x7700
#define ID_HARDWARE_OUTPUT		0x7701
#define ID_RESET_OUTPUT			0x7702
#define ID_DISCONNECT_OUTPUT	0x7703
#define ID_CLOSE_RELAY			0x7704
#define ID_OPEN_RELAY			0x7705


/////////////////////////////////////////////////////////////////////////////
inline void MY_POST(UINT dwCmd,WPARAM wParam) 
{
	CWnd * pWnd = AfxGetApp()->m_pMainWnd;
	if (WK_IS_WINDOW(pWnd)) {
		pWnd->PostMessage(WM_COMMAND,dwCmd,wParam);
	}
}

// CSimUnitApp:
// See SimUnit.cpp for the implementation of this class
//

class CIPCInputSimUnit;
class CIPCOutputSimUnit;
class CSimUnitDlg;

class CSimUnitApp : public CWinApp
{
public:
	CSimUnitApp();
// Attributes
	CIPCInputSimUnit*	GetInput () { return m_pCIPCInput; };
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimUnitApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
	void	Terminate();

	void	DoAlert(int nID);
	void	UndoAlert(int nID);
	
	// Output

	void	DoRelay(int iNr, BOOL bState);
#if _MFC_VER >= 0x0710
	int SetLanguageParameters(UINT uCodePage, DWORD dwCPbits);
#endif
	
	//{{AFX_MSG(CSimUnitApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	// main window
	CSimUnitDlg*		m_pDialog;
	
private:
	// connection to server
	CIPCInputSimUnit*	m_pCIPCInput;
	CIPCOutputSimUnit*	m_pCIPCOutput;
};
/////////////////////////////////////////////////////////////////////////////
