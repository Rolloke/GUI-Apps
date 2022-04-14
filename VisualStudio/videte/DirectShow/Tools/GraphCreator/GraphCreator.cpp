// GraphCreator.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "GraphCreator.h"

#include "MainFrm.h"
#include "GraphCreatorDoc.h"
#include "GraphCreatorView.h"
#include "..\FindFilter\FindFilterDlg.h"
#include "graphcreatorpropertypage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SETTINGS_SECTION		_T("Settings")
#define MAX_RECENT_FILES		_T("MaxRecentFiles")
#define AUTO_REARRANGE_FILTERS	_T("AutoRearrangeFilters")
/////////////////////////////////////////////////////////////////////////////
// CGraphCreatorApp

BEGIN_MESSAGE_MAP(CGraphCreatorApp, CWinApp)
	//{{AFX_MSG_MAP(CGraphCreatorApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_FILE_RENDER_MEDIA, OnFileRenderMedia)
	ON_UPDATE_COMMAND_UI(ID_VIEW_AUTO_REARRANGE_FILTERS, OnUpdateViewAutoRearrangeFilters)
	ON_COMMAND(ID_VIEW_AUTO_REARRANGE_FILTERS, OnViewAutoRearrangeFilters)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// The one and only CGraphCreatorApp object

CGraphCreatorApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CGraphCreatorApp construction

CGraphCreatorApp::CGraphCreatorApp()
{
	m_bRenderMediaFile = FALSE;
	m_bAutoRearrangeFilters = TRUE;
	m_nMaxRecentFiles = 8;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphCreatorApp initialization

BOOL CGraphCreatorApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#if _MFC_VER >= 0x0710
#else
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif // _AFXDLL
#endif // _MFC_VER

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Videte"));

	// Load standard INI file options (including MRU)
	m_nMaxRecentFiles = GetProfileInt(SETTINGS_SECTION, MAX_RECENT_FILES, m_nMaxRecentFiles);
	m_bAutoRearrangeFilters = GetProfileInt(SETTINGS_SECTION, AUTO_REARRANGE_FILTERS, m_bAutoRearrangeFilters);
	LoadStdProfileSettings(m_nMaxRecentFiles);

	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		AfxMessageBox(_T("CoInitialize failed"));
		return FALSE;
	}

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CGraphCreatorDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CGraphCreatorView));
	AddDocTemplate(pDocTemplate);

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	return TRUE;
}


// App command to run the dialog
void CGraphCreatorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CGraphCreatorApp message handlers

/////////////////////////////////////////////////////////////////////////////
int CGraphCreatorApp::ExitInstance()
{
	WriteProfileInt(SETTINGS_SECTION, AUTO_REARRANGE_FILTERS, m_bAutoRearrangeFilters);
	WriteProfileInt(SETTINGS_SECTION, MAX_RECENT_FILES, m_nMaxRecentFiles);
	CoUninitialize();
	return CWinApp::ExitInstance();
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorApp::OnFileRenderMedia()
{
	m_bRenderMediaFile = TRUE;
	OnFileOpen();
	m_bRenderMediaFile = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
CView *CGraphCreatorApp::GetActiveView()
{
	if (m_pMainWnd)
	{
		return ((CMainFrame*)m_pMainWnd)->GetActiveView();
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CGraphCreatorApp::GetInterfaces(IUnknown*pUnk, CStringArray &sa)
{
	// RKE: does not show own interfaces, if the are not registered at the
	// default registry location.
	CRegKey reg;
	if (    pUnk != NULL 
		&& (ERROR_SUCCESS == reg.Open( HKEY_CLASSES_ROOT, _T("Interface"))))
	{
		TCHAR szKeyName[1024] = _T("");
		for ( int i = 0; ERROR_SUCCESS == RegEnumKey( reg.m_hKey, i, szKeyName, 1024 ); i++)
		{
			USES_CONVERSION;
			CLSID clsID = CLSID_NULL;
			if ( SUCCEEDED( CLSIDFromString( T2OLE(szKeyName), &clsID ) ) )
			{
				IUnknown * spObject = NULL;
				if ( SUCCEEDED( pUnk->QueryInterface( clsID, (void**)&spObject ) ) )
				{
					TCHAR szValue[1024] = _T(""); 
					LONG ncbValue = 1024;
					if ( ERROR_SUCCESS == RegQueryValue( reg.m_hKey, szKeyName, szValue, &ncbValue ) )
					{
						CString sVal(szValue);
						sa.Add(sVal);
					}
				}
				RELEASE_OBJECT(spObject);
			}
		}
		reg.Close();
		return sa.GetCount();
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorApp::DoUpdateMenuPopup(CMenu*pMenu, CWnd*pWnd)
{
	CCmdUI state;
	CMenu *pSub;
	state.m_pMenu = pMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);
	state.m_nIndexMax = pMenu->GetMenuItemCount();

	for (state.m_nIndex=0; state.m_nIndex<state.m_nIndexMax; state.m_nIndex++)
	{
		pSub = pMenu->GetSubMenu(state.m_nIndex);
		if (pSub)
		{
			DoUpdateMenuPopup(pSub, pWnd);
			continue;
		}
		state.m_nID = pMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
		{
			continue;
		}

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1)
		{
			state.DoUpdate(pWnd, FALSE);    // popups are never auto disabled
		}
		else
		{
			state.DoUpdate(pWnd, TRUE);    // popups are never auto disabled
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
HINSTANCE CGraphCreatorApp::LoadAppLangResourceDLL()
{
	return CWinApp::LoadAppLangResourceDLL();
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorApp::OnViewAutoRearrangeFilters()
{
	m_bAutoRearrangeFilters = !m_bAutoRearrangeFilters;
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorApp::OnUpdateViewAutoRearrangeFilters(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(m_bAutoRearrangeFilters);
}

#if GRAPH_CREATOR_PROPERTY_TEST
HRESULT CGraphCreatorApp::ShowFilterPropertyPage(IBaseFilter *pFilter, HWND hwndParent)
{
    HRESULT hr;
    ISpecifyPropertyPages *pSpecify=0;

    if (!pFilter)
        return E_NOINTERFACE;

    // Discover if this filter contains a property page
    hr = pFilter->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpecify);
    if (SUCCEEDED(hr)) 
    {
        do 
        {
            FILTER_INFO FilterInfo;
            hr = pFilter->QueryFilterInfo(&FilterInfo);
            if (FAILED(hr))
                break;

            CAUUID caGUID;
            hr = pSpecify->GetPages(&caGUID);
            if (FAILED(hr))
                break;

            pSpecify->Release();
        
            caGUID.cElems++;
            caGUID.pElems = (GUID*)CoTaskMemRealloc(caGUID.pElems, caGUID.cElems * sizeof(GUID));
            caGUID.pElems[caGUID.cElems-1] = CLSID_GraphCreatorPropertyPage;

            GUID classID;
            pFilter->GetClassID(&classID);
            g_Templates[0].m_ClsID = &classID;
            hr = AMovieDllRegisterServer( );
            // Display the filter's property page
            OleCreatePropertyFrame(
                hwndParent,             // Parent window
                0,                      // x (Reserved)
                0,                      // y (Reserved)
                FilterInfo.achName,     // Caption for the dialog box
                1,                      // Number of filters
                (IUnknown **)&pFilter,  // Pointer to the filter 
                caGUID.cElems,          // Number of property pages
                caGUID.pElems,          // Pointer to property page CLSIDs
                0,                      // Locale identifier
                0,                      // Reserved
                NULL                    // Reserved
            );
            CoTaskMemFree(caGUID.pElems);
            FilterInfo.pGraph->Release(); 

        } while(0);
    }

//    pFilter->Release();
    return hr;
}
#endif