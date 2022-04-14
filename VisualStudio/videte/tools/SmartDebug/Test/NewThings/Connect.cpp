// Connect.cpp : Implementierung von CConnect.
#include "stdafx.h"
#include "AddIn.h"
#include "Connect.h"
#include "copycsvdlg.h"

extern CAddInModule _AtlModule;
#define WATCH_ARRAY_CONTENT L"WatchArrayContent"
CComPtr<EnvDTE::_DTE> g_pDTE;

// DebuggerEventsClass für die Aktualisierung im Debug modus

// CConnect
STDMETHODIMP CConnect::OnConnection(IDispatch *pApplication, ext_ConnectMode ConnectMode, IDispatch *pAddInInst, SAFEARRAY ** /*benutzerdefiniert*/ )
{
	HRESULT hr = S_OK;
	pApplication->QueryInterface(__uuidof(DTE2), (LPVOID*)&g_pDTE);
	pAddInInst->QueryInterface(__uuidof(AddIn), (LPVOID*)&m_pAddInInstance);
	if(    ConnectMode == 5
        || ConnectMode == ext_cm_Startup) //
	{
		HRESULT hr = S_OK;
		CComPtr<IDispatch> pDisp;
		CComQIPtr<Commands> pCommands;
		CComQIPtr<Commands2> pCommands2;
		CComQIPtr<_CommandBars> pCommandBars;
		CComPtr<CommandBarControl> pCommandBarControl;
		CComPtr<Command> pCreatedCommand;
		CComPtr<CommandBar> pMenuBarCommandBar;
		CComPtr<CommandBarControls> pMenuBarControls;
		CComPtr<CommandBarControl> pToolsCommandBarControl;
		CComQIPtr<CommandBar> pToolsCommandBar;
		CComQIPtr<CommandBarPopup> pToolsPopup;

		IfFailGoCheck(g_pDTE->get_Commands(&pCommands), pCommands);
		pCommands2 = pCommands;
		if(SUCCEEDED(pCommands2->AddNamedCommand2(m_pAddInInstance, 
            CComBSTR("SmartDebug.Connect.WatchArrayContent"), CComBSTR("SmartDebug.Connect.WatchArrayContent"), CComBSTR("Shows Array values in a list and graphically"), VARIANT_TRUE, CComVariant(59), NULL, vsCommandStatusSupported+vsCommandStatusEnabled, vsCommandStylePictAndText, vsCommandControlTypeButton, &pCreatedCommand)) && (pCreatedCommand))
		{
			//Fügen Sie der Tools-Menüleiste eine Schaltfläche hinzu.
			IfFailGoCheck(g_pDTE->get_CommandBars(&pDisp), pDisp);
			pCommandBars = pDisp;

			//Suchen Sie die MenuBar-Befehlsleiste, die oberste Befehlsleiste mit allen Hauptmenüelementen:
			IfFailGoCheck(pCommandBars->get_Item(CComVariant(L"Debuggen"), &pMenuBarCommandBar), pMenuBarCommandBar);
			IfFailGoCheck(pMenuBarCommandBar->get_Controls(&pMenuBarControls), pMenuBarControls);

			//Suchen Sie die Tools-Befehlsleiste in der MenuBar-Befehlsleiste:
			IfFailGoCheck(pMenuBarControls->get_Item(CComVariant(L"Debuggen"), &pToolsCommandBarControl), pToolsCommandBarControl);
			pToolsPopup = pToolsCommandBarControl;
			IfFailGoCheck(pToolsPopup->get_CommandBar(&pToolsCommandBar), pToolsCommandBar);

			//Fügen Sie das Steuerelement hinzu:
			pDisp = NULL;
			IfFailGoCheck(pCreatedCommand->AddControl(pToolsCommandBar, 1, &pDisp), pDisp);
		}
		return S_OK;
	}
Error:
	return hr;
}

STDMETHODIMP CConnect::OnDisconnection(ext_DisconnectMode /*RemoveMode*/, SAFEARRAY ** /*benutzerdefiniert*/ )
{
	g_pDTE = NULL;
	m_pAddInInstance = NULL;
	return S_OK;
}

STDMETHODIMP CConnect::OnAddInsUpdate (SAFEARRAY ** /*benutzerdefiniert*/ )
{
	return S_OK;
}

STDMETHODIMP CConnect::OnStartupComplete (SAFEARRAY ** /*benutzerdefiniert*/ )
{
	return S_OK;
}

STDMETHODIMP CConnect::OnBeginShutdown (SAFEARRAY ** /*benutzerdefiniert*/ )
{
	return S_OK;
}

STDMETHODIMP CConnect::QueryStatus(BSTR bstrCmdName, vsCommandStatusTextWanted NeededText, vsCommandStatus *pStatusOption, VARIANT *pvarCommandText)
{
    if(NeededText == vsCommandStatusTextWantedNone)
	{ 
	  if(!_wcsicmp(bstrCmdName, L"SmartDebug.Connect.SmartDebug"))
	  //if(!_wcsicmp(bstrCmdName, L"SmartDebug.Connect.WatchArrayContent"))
	  {
		  *pStatusOption = (vsCommandStatus)(vsCommandStatusEnabled+vsCommandStatusSupported);
	  }
  }
	return S_OK;
}

CCopyCSVDlg g_dlg;

STDMETHODIMP CConnect::Exec(BSTR bstrCmdName, vsCommandExecOption ExecuteOption, VARIANT * /*pvarVariantIn*/, VARIANT * /*pvarVariantOut*/, VARIANT_BOOL *pvbHandled)
{
	*pvbHandled = VARIANT_FALSE;
	if(ExecuteOption == vsCommandExecOptionDoDefault)
	{
        if(!_wcsicmp(bstrCmdName, L"SmartDebug.Connect.SmartDebug"))
		//if(!_wcsicmp(bstrCmdName, L"SmartDebug.Connect.WatchArrayContent"))
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