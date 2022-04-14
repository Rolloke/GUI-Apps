/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicUnit
// FILE:		$Workfile: SavicUnitApp.h $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicUnit/SavicUnitApp.h $
// CHECKIN:		$Date: 24.06.02 13:52 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 24.06.02 13:22 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////
	  
#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "CInstanceCounter.h"

/////////////////////////////////////////////////////////////////////////////
// CSaVicApp:
//
class CDebug;
class CSaVicException;
class CSaVicUnitDlg;
class CInstanceCounter;
class CSaVicApp : public CWinApp
{
public:
	WK_ApplicationId GetApplicationId();
	CSaVicApp();
	~CSaVicApp();

	BOOL CleanUp();
	BOOL CopyIni2Reg(CString &sStartDir);
	void InitPathes();

	CSaVicUnitDlg* GetSaVicUnitDlg(){return m_pSaVicUnitDlg;}

	int GetCardRef(){return m_pInstance->GetInstance();}

//	static	LONG TopLevelException(LPEXCEPTION_POINTERS pExc);
//	static	CString GetExcErrorString(LPEXCEPTION_POINTERS pExc);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaVicApp)
	public:
	virtual BOOL InitInstance();
	virtual int  ExitInstance();
	virtual void Dump( CDumpContext &dc ) const;
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSaVicApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CSaVicException		*m_pCSaVicException;
	CSaVicUnitDlg		*m_pSaVicUnitDlg;	
	CMutex				*m_pMutex;
	CString				m_sWorkingDirectory;
	CString				m_sWindowsDirectory;
	CString				m_sSystemDirectory;
	CString				m_sCurrentDirectory;
	CDebug				*m_pDebug;
	CInstanceCounter	*m_pInstance;
};


/////////////////////////////////////////////////////////////////////////////
