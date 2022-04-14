/////////////////////////////////////////////////////////////////////////////
//	Project:		Common
//
//	File:			$Workfile: ModifyStyle.cpp $ : implementation file
//
//	Start:			01.06.95
//
//  Last change:	$Modtime: 9/01/05 9:08a $
//
//  by Author	:	$Author: Rolf.kary-ehlers $
//
//  Checked in:	$Date: 9/05/05 1:44p $
//
//  Version:		$Revision: 4 $
//
//
//$Nokeywords:$
/////////////////////////////////////////////////////////////////////////////
// ModifyStyle.cpp : Legt das Klassenverhalten für die Anwendung fest.
//

#include "stdafx.h"
#include "ModifyStyle.h"
#include "ModifyStyleDlg.h"
#include ".\modifystyle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModifyStyleApp

BEGIN_MESSAGE_MAP(CModifyStyleApp, CWinApp)
	//{{AFX_MSG_MAP(CModifyStyleApp)
		// HINWEIS - Hier werden Mapping-Makros vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModifyStyleApp Konstruktion

CModifyStyleApp::CModifyStyleApp()
{
	// ZU ERLEDIGEN: Hier Code zur Konstruktion einfügen
	// Alle wichtigen Initialisierungen in InitInstance platzieren
}

/////////////////////////////////////////////////////////////////////////////
// Das einzige CModifyStyleApp-Objekt

CModifyStyleApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CModifyStyleApp Initialisierung

BOOL CModifyStyleApp::InitInstance()
{
	// Standardinitialisierung
	// Wenn Sie diese Funktionen nicht nutzen und die Größe Ihrer fertigen 
	//  ausführbaren Datei reduzieren wollen, sollten Sie die nachfolgenden
	//  spezifischen Initialisierungsroutinen, die Sie nicht benötigen, entfernen.
#if _MFC_VER < 0x0710
#ifdef _AFXDLL
	Enable3dControls();			// Diese Funktion bei Verwendung von MFC in gemeinsam genutzten DLLs aufrufen
#else
	Enable3dControlsStatic();	// Diese Funktion bei statischen MFC-Anbindungen aufrufen
#endif // _AFXDLL
#endif // _MFC_VER >= 0x0710
    SetRegistryKey(_T("KESoft"));

	CModifyStyleDlg *pDlg = new CModifyStyleDlg;
	m_pMainWnd = pDlg;
	m_pActiveWnd = m_pMainWnd;
	pDlg->Create(CModifyStyleDlg::IDD);
	return TRUE;
}

int CModifyStyleApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}

BOOL CModifyStyleApp::OnIdle(LONG lCount)
{
	// TODO: Add your specialized code here and/or call the base class

	return CWinApp::OnIdle(lCount);
}
