// Connect.cpp : Implementation of CConnect
#include "stdafx.h"
#include "AddIn.h"
#include "Connect.h"
#include "copycsvdlg.h"

extern CAddInModule _AtlModule;
#define WATCH_ARRAY_CONTENT L"WatchArrayContent"

CComPtr<EnvDTE::_DTE> g_pDTE;

// When run, the Add-in wizard prepared the registry for the Add-in.
// At a later time, if the Add-in becomes unavailable for reasons such as:
//   1) You moved this project to a computer other than which is was originally created on.
//   2) You chose 'Yes' when presented with a message asking if you wish to remove the Add-in.
//   3) Registry corruption.
// you will need to re-register the Add-in by building the MyAddin21Setup project 
// by right clicking the project in the Solution Explorer, then choosing install.


// CConnect
STDMETHODIMP CConnect::OnConnection(IDispatch *pApplication, AddInDesignerObjects::ext_ConnectMode ConnectMode, IDispatch *pAddInInst, SAFEARRAY ** /*custom*/ )
{
	HRESULT hr = S_OK;
	pApplication->QueryInterface(__uuidof(EnvDTE::_DTE), (LPVOID*)&g_pDTE);
	pAddInInst->QueryInterface(__uuidof(EnvDTE::AddIn), (LPVOID*)&m_pAddInInstance);
	if(   ConnectMode == AddInDesignerObjects::ext_cm_Startup
      || ConnectMode == AddInDesignerObjects::ext_cm_AfterStartup)
	{
		HRESULT hr = S_OK;
		CComPtr<EnvDTE::Commands> pCommands;
		CComPtr<Office::_CommandBars> pCommandBars;
		CComPtr<Office::CommandBarControl> pCommandBarControl;
		CComPtr<EnvDTE::Command> pCreatedCommand;
		CComPtr<Office::CommandBar> pMenuBarCommandBar;

        // When run, the Add-in wizard prepared the registry for the Add-in.
        // At a later time, the Add-in or its commands may become unavailable for reasons such as:
        //   1) You moved this project to a computer other than which is was originally created on.
        //   2) You chose 'Yes' when presented with a message asking if you wish to remove the Add-in.
        //   3) You add new commands or modify commands already defined.
        // You will need to re-register the Add-in by building the DebugHelpersSetup project,
        // right-clicking the project in the Solution Explorer, and then choosing install.
        // Alternatively, you could execute the ReCreateCommands.reg file the Add-in Wizard generated in 
        // the project directory, or run 'devenv /setup' from a command prompt.
		IfFailGoCheck(g_pDTE->get_Commands(&pCommands), pCommands);
        if(SUCCEEDED(pCommands->AddNamedCommand(m_pAddInInstance, WATCH_ARRAY_CONTENT, CComBSTR("Watch"),
            CComBSTR("Shows Array values in a list"), VARIANT_TRUE, IDB_WATCH_ARRAY,
            NULL, EnvDTE::vsCommandStatusSupported+EnvDTE::vsCommandStatusEnabled, &pCreatedCommand)) && (pCreatedCommand))
		{
			//Add a button to the tools menu bar.
			IfFailGoCheck(g_pDTE->get_CommandBars(&pCommandBars), pCommandBars);
			IfFailGoCheck(pCommandBars->get_Item(CComVariant(L"Debug"), &pMenuBarCommandBar), pMenuBarCommandBar);
			IfFailGoCheck(pCreatedCommand->AddControl(pMenuBarCommandBar, 1, &pCommandBarControl), pCommandBarControl);
		}
		return S_OK;
	}
Error:
	return hr;
}

STDMETHODIMP CConnect::OnDisconnection(AddInDesignerObjects::ext_DisconnectMode /*RemoveMode*/, SAFEARRAY ** /*custom*/ )
{
    g_pDTE = NULL;
	return S_OK;
}

STDMETHODIMP CConnect::OnAddInsUpdate (SAFEARRAY ** /*custom*/ )
{
	return S_OK;
}

STDMETHODIMP CConnect::OnStartupComplete (SAFEARRAY ** /*custom*/ )
{
	return S_OK;
}

STDMETHODIMP CConnect::OnBeginShutdown (SAFEARRAY ** /*custom*/ )
{
	return S_OK;
}

STDMETHODIMP CConnect::QueryStatus(BSTR bstrCmdName, EnvDTE::vsCommandStatusTextWanted NeededText, EnvDTE::vsCommandStatus *pStatusOption, VARIANT *pvarCommandText)
{
    if(NeededText == EnvDTE::vsCommandStatusTextWantedNone)
	{
	  if(!_wcsicmp(bstrCmdName, L"SmartDebug.Connect."WATCH_ARRAY_CONTENT))
	  {
		  *pStatusOption = (EnvDTE::vsCommandStatus)(EnvDTE::vsCommandStatusEnabled+EnvDTE::vsCommandStatusSupported);
	  }
    }
	return S_OK;
}

CCopyCSVDlg g_dlg;

STDMETHODIMP CConnect::Exec(BSTR bstrCmdName, EnvDTE::vsCommandExecOption ExecuteOption, VARIANT * /*pvarVariantIn*/, VARIANT * /*pvarVariantOut*/, VARIANT_BOOL *pvbHandled)
{
	*pvbHandled = VARIANT_FALSE;
	if(ExecuteOption == EnvDTE::vsCommandExecOptionDoDefault)
	{
		if(!_wcsicmp(bstrCmdName, L"SmartDebug.Connect."WATCH_ARRAY_CONTENT))
        {
            GUID IID_TextSelection = {0x1fa0e135,0x399a,0x4d2c,{0xa4,0xfe,0xd2,0x1e,0x24,0x80,0xf9,0x21}};
            HWND hwndParent = NULL;
            CComPtr<EnvDTE::Window> pMainWnd;
            CComPtr<EnvDTE::TextSelection> pTextSelection;
            IDispatch *pSelection;
            CComPtr<EnvDTE::Document> pDocument;
            HRESULT hr = 0;

            hr = g_pDTE->get_MainWindow(&pMainWnd);
            if (SUCCEEDED(hr))
            {
                hr = pMainWnd->get_HWnd((long*)&hwndParent);
            }
            hr = g_pDTE->get_ActiveDocument(&pDocument);
            hr = pDocument->get_Selection(&pSelection);
            hr = pSelection->QueryInterface(IID_TextSelection, (void**)&pTextSelection);
            if(CCopyCSVDlg::gm_nModal)
            {
                hr = pTextSelection->get_Text(&g_dlg.m_pSelected);
                pSelection->Release();
                g_dlg.DoModal(hwndParent);
            }
            else
		    {
                CCopyCSVDlg*pDlg = new CCopyCSVDlg;
                hr = pTextSelection->get_Text(&pDlg->m_pSelected);
                pSelection->Release();
                pDlg->Create(hwndParent);
			    *pvbHandled = VARIANT_TRUE;
			    return S_OK;
		    }
        }
	}
	return S_OK;
}

