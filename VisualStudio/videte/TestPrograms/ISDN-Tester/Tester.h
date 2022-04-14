///////////////////////////////////////////////////////////////
// FILE   : $Workfile: Tester.h $ 
// ARCHIVE: $Archive: /Project/Tools/Tester/Tester.h $
// DATE   :	$Date: 7.04.97 18:03 $ 
// VERSION: $Revision: 8 $
// LAST   : $Modtime: 7.04.97 17:21 $ 
// AUTHOR : $Author: Hajo $
//        : $Nokeywords:$

/**headerref **************************************************
 * name    : Tester.h
 * class   : CTesterApp
 * derived : CWinApp
 *         : 
 * remark  : See Tester.cpp for the implementation and  
 *         : documentation of this class either
 * author  : Hajo Fierke / Hedu 1997 w+k
 * history : 19.03.1997
 *		   : 01.04.97 hajo 
 **************************************************************/
#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

class CIPCInputTester;

class CTesterApp : public CWinApp
{
public:
	CTesterApp();
	// access:
	inline BOOL		DoVerify() const;
	inline CString	GetTestFileName() const;
	inline int		GetDestination() const;
	inline int		GetSizeofpData() const;
	inline int		GetLoop() const;
	inline int		GetDoDelete() const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTesterApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTesterApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int		DoTest(const CString &sHost);
	void	SplitArgs(const CString &sCmdLine);
	CString	ParseOptions(CStringArray &lpCmdLineArray);
	CString	FindHostNumberByName(const CString &sHostsname)	const;

	// data
	CString			 m_szHost;//( "83" ); // Micotest
	CString			 m_szUserDummy;
	CString			 m_sTestFileName;	// "C:\\Log\\foo.dat"
	int				 m_iDestination;	// use full pathname
	int				 m_iSizeofpData;	// (1024*64*3)/8;
	int				 m_iLoop;			//			= 10;
	CIPCInputTester	*m_pInput;
	CStringArray	 m_lpCmdLineArray;
	BOOL			 m_bDoVerify;
	BOOL			 m_bDoDelete;
};

inline BOOL CTesterApp::DoVerify() const
{
	return m_bDoVerify;
}

inline BOOL CTesterApp::GetDoDelete() const
{
	return m_bDoDelete;
}

inline CTesterApp *MyGetApp()
{
	return (CTesterApp*)AfxGetApp();
}

inline CString	CTesterApp::GetTestFileName() const
{
	return m_sTestFileName;
}

inline int		CTesterApp::GetDestination() const
{
	return m_iDestination;
}

inline int		CTesterApp::GetSizeofpData()const
{
	return m_iSizeofpData;
}

inline int		CTesterApp::GetLoop() const
{
	return m_iLoop;
}



///////////////////////////////////////////////////////////////////
