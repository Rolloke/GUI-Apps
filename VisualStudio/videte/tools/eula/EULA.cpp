// EULA.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "EULA.h"
#include "EULADlg.h"
#include "EULALicenseDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEULAApp

BEGIN_MESSAGE_MAP(CEULAApp, CWinApp)
	//{{AFX_MSG_MAP(CEULAApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEULAApp construction

CEULAApp::CEULAApp()
{
	m_bIsDTS = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CEULAApp object

CEULAApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CEULAApp initialization
static void MySplitCommandLine(const CString &sCommandLine, CStringArray& saParameters)
{
	CString sCL = sCommandLine;
	CString sOne;
	int p;

	sCL.TrimLeft();
	p = sCL.Find(' ');
	while (p!=-1)
	{
		sOne = sCL.Left(p);
		sCL = sCL.Mid(p+1);
		sCL.TrimLeft();
		saParameters.Add(sOne);
		p = sCL.Find(' ');
	}
	if (!sCL.IsEmpty())
	{
		saParameters.Add(sCL);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CEULAApp::InitInstance()
{
	AfxEnableControlContainer();


	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#if _MFC_VER < 0x700
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	AfxInitRichEdit();
	CStringArray saParameters;
	MySplitCommandLine(m_lpCmdLine, saParameters);

	BOOL bShowEula = TRUE; 
	for (int i=0;i<saParameters.GetSize();i++)
	{
		if (saParameters[i] == _T("-DTS"))
		{
			m_bIsDTS = TRUE;
		}
		else if (saParameters[i] == _T("-NOEULA"))
		{
			bShowEula = FALSE;
		}
		else if (-1!=saParameters[i].Find(_T(".rtf")))
		{
			m_sEulaRTF = saParameters[i];
		}
	}


	CEULALicenseDlg dlgLicense;
	CString sSerial		= dlgLicense.GetSerialFromReg();
	CString sExpansion	= dlgLicense.GetExpansionFromReg();

	if(sSerial.GetLength() == 9)
	{
		//check serial
		if(dlgLicense.IsSerialCorrect(sSerial))
		{
			TRACE(_T("Serial korrekt\n"));
			CString sError;
			sError.Format(_T("serial OK, check expansion\n"));
//			AfxMessageBox(sError);
			if(!m_bIsDTS)
			{
				//check expansion code only on idip receivers
				if(sExpansion.GetLength())
				{
					//serial correct, check expansion code
					DWORD dwDongleDecode = 0;
					if(dlgLicense.IsExpansionCorrect(dwDongleDecode, sSerial, sExpansion))
					{
						CString sError;
						sError.Format(_T("serial & expansion OK\n"));
	//					AfxMessageBox(sError);
						return FALSE; //alles OK, starte Receiver Software
					}
					else
					{
						CString sError;
						sError.Format(_T("serial OK, expansion not OK, show eula\n"));
	//					AfxMessageBox(sError);
					}
				}
				else
				{
					CString sError;
					sError.Format(_T("serial OK, expansion not available from reg, show eula\n"));
	//				AfxMessageBox(sError);
				}
			}
			else
			{
				return FALSE; //alles OK, starte DTS Receiver Software
			}
		}
		else
		{
			CString sError;
			sError.Format(_T("serial not OK, show Eula\n"));
//			AfxMessageBox(sError);
		}
	}

	if (bShowEula)
	{
		CEULADlg dlg;
		m_pMainWnd = &dlg;
		dlg.DoModal();
	}
	else 
	{
		CEULALicenseDlg dlg;
		m_pMainWnd = &dlg;
		dlg.DoModal();
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CEULAApp::IsDTS()
{
	return m_bIsDTS;
}
/////////////////////////////////////////////////////////////////////////////
CString CEULAApp::GetEulaRTF()
{
	return m_sEulaRTF;
}
