// AddInMod.cpp : implementation file
//

#include "stdafx.h"
#include "ReplAll.h"
#include "DSAddIn.h"
#include "Commands.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// CVideteStartupDlg g_StartUpDlg;

// This is called when the user first loads the add-in, and on start-up
//  of each subsequent Developer Studio session
/****************************************************************************
 Klasse:    CDSAddIn
 Funktion: OnConnection
 Zweck:     Funktion zum Anbinden der Dll
 Parameter: 4
 pApp:        (E): Anwendung an die die Dll gebunden wird.  (IApplication*)
 bFirstTime:  (E): Erste Bindung zur Laufzeit der Anwendung  (VARIANT_BOOL)
 dwCookie:    (E): Identifikation  (long)
 OnConnection:(E): weis nicht  (VARIANT_BOOL*)
 Author: Rolf Kary-Ehlers
 Rückgabewert: S_OK bei Anbindung  (STDMETHODIMP)
****************************************************************************/
STDMETHODIMP CDSAddIn::OnConnection(IApplication* pApp, VARIANT_BOOL bFirstTime,
		long dwCookie, VARIANT_BOOL* OnConnection)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	// Store info passed to us
	IApplication* pApplication = NULL;
	if (FAILED(pApp->QueryInterface(IID_IApplication, (void**) &pApplication))
		|| pApplication == NULL)
	{
		*OnConnection = VARIANT_FALSE;
		return S_OK;
	}

	m_dwCookie = dwCookie;

	// Create command dispatch, send info back to DevStudio
	CCommandsObj::CreateInstance(&m_pCommands);
	m_pCommands->AddRef();

	// The QueryInterface above AddRef'd the Application object.  It will
	//  be Release'd in CCommand's destructor.
	m_pCommands->SetApplicationObject(pApplication);

	// (see stdafx.h for the definition of VERIFY_OK)

	VERIFY_OK(pApplication->SetAddInInfo((long) AfxGetInstanceHandle(),
		(LPDISPATCH) m_pCommands, IDR_TOOLBAR_MEDIUM, IDR_TOOLBAR_LARGE, m_dwCookie));

	// Inform DevStudio of the commands we implement

	// The command name should not be localized to other languages.  The 
	//  tooltip, command description, and other strings related to this
	//  command are stored in the string table (IDS_CMD_STRING) and should
	//  be localized.
	LPCTSTR szCommand = _T("ReplAllCommand");
	VARIANT_BOOL bRet;
	CString strCmdString;
	strCmdString.LoadString(IDS_CMD_STRING);
	strCmdString = szCommand + strCmdString;
	CComBSTR bszCmdString(strCmdString);
	CComBSTR bszMethod(_T("ReplAllCommandMethod"));
	CComBSTR bszCmdName(szCommand);
	VERIFY_OK(pApplication->AddCommand(bszCmdString, bszMethod, 0, m_dwCookie, &bRet));

	if (bRet == VARIANT_FALSE)
	{
		*OnConnection = VARIANT_FALSE;
		return S_OK;
	}
	if (bFirstTime == VARIANT_TRUE)
	{
		VERIFY_OK(pApplication->AddCommandBarButton(dsGlyph, bszCmdName, m_dwCookie));
	}

	szCommand = _T("InsertCommentHeader");
	strCmdString.LoadString(IDS_CMD_STRING2);
	strCmdString = szCommand + strCmdString;
	bszCmdString = strCmdString;
	bszMethod    = _T("InsertCommentHeader");
	bszCmdName   = szCommand;
   VERIFY_OK(pApplication->AddCommand(bszCmdString, bszMethod, 1, m_dwCookie, &bRet));
	if (bRet == VARIANT_FALSE)
	{
		*OnConnection = VARIANT_FALSE;
		return S_OK;
	}
	if (bFirstTime == VARIANT_TRUE)
	{
		VERIFY_OK(pApplication->AddCommandBarButton(dsGlyph, bszCmdName, m_dwCookie));
	}

   szCommand = _T("PrintSelectedText");
	strCmdString.LoadString(IDS_CMD_STRING3);
	strCmdString = szCommand + strCmdString;
	bszCmdString = strCmdString;
	bszMethod    = _T("PrintSelectedText");
	bszCmdName   = szCommand;
   VERIFY_OK(pApplication->AddCommand(bszCmdString, bszMethod, 2, m_dwCookie, &bRet));
	if (bRet == VARIANT_FALSE)
	{
		*OnConnection = VARIANT_FALSE;
		return S_OK;
	}
	if (bFirstTime == VARIANT_TRUE)
	{
		VERIFY_OK(pApplication->AddCommandBarButton(dsGlyph, bszCmdName, m_dwCookie));
	}

	*OnConnection = VARIANT_TRUE;
	return S_OK;
}

// This is called on shut-down, and also when the user unloads the add-in
STDMETHODIMP CDSAddIn::OnDisconnection(VARIANT_BOOL bLastTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pCommands->Release();
	m_pCommands = NULL;

	return S_OK;
}
