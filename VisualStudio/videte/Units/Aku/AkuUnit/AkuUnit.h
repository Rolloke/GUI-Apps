/////////////////////////////////////////////////////////////////////////////
// PROJECT:		AkuUnit
// FILE:		$Workfile: AkuUnit.h $
// ARCHIVE:		$Archive: /Project/Units/Aku/AkuUnit/AkuUnit.h $
// CHECKIN:		$Date: 5.08.98 9:52 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 5.08.98 9:52 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAkuUnitApp:
// See AkuUnit.cpp for the implementation of this class
//

class CAkuUnitApp : public CWinApp
{
public:
	CAkuUnitApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAkuUnitApp)
	public:
	virtual BOOL InitInstance();
	virtual int  ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAkuUnitApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL CopyIni2Reg(CString &sPath);
	void InitPathes();

	CString			m_sWorkingDirectory;
	CString			m_sWindowsDirectory;
	CString			m_sSystemDirectory;
	CString			m_sCurrentDirectory;


};


/////////////////////////////////////////////////////////////////////////////
