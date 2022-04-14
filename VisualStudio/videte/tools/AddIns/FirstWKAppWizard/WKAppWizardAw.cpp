// WKAppWizardaw.cpp : implementation file
//

#include "stdafx.h"
#include "WKAppWizard.h"
#include "WKAppWizardaw.h"
#include "chooser.h"

#include <atlbase.h>


#ifdef _PSEUDO_DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// This is called immediately after the custom AppWizard is loaded.  Initialize
//  the state of the custom AppWizard here.
void CWKAppWizardAppWiz::InitCustomAppWiz()
{
	// Create a new dialog chooser; CDialogChooser's constructor initializes
	//  its internal array with pointers to the steps.
	m_pChooser = new CDialogChooser;

	// At first, we don't know the total number of steps, since there are two
	//  possible "tracks" (MDI/SDI app and dialog-based app).
	SetNumberOfSteps(-1);

	// Inform AppWizard of the languages we support
	SetSupportedLanguages(_T("Deutsch [Deutschland] (APPWZDEU.DLL);0x40704b0\nEnglisch [USA] (APPWZENU.DLL);0x40904b0\nSpanisch [Moderne Sortierung] (APPWZESP.DLL);0xc0a04b0\nFranzösisch [Frankreich] (APPWZFRA.DLL);0x40c04b0\nItalienisch [Italien] (APPWZITA.DLL);0x41004b0"));

	// TODO: Add any other custom AppWizard-wide initialization here.
}

// This is called just before the custom AppWizard is unloaded.
void CWKAppWizardAppWiz::ExitCustomAppWiz()
{
	// Deallocate memory used for the dialog chooser
	ASSERT(m_pChooser != NULL);
	delete m_pChooser;
	m_pChooser = NULL;

	// TODO: Add code here to deallocate resources used by the custom AppWizard
}

// This is called when the user clicks "Create..." on the New Project dialog
//  or "Next" on one of the custom AppWizard's steps.
CAppWizStepDlg* CWKAppWizardAppWiz::Next(CAppWizStepDlg* pDlg)
{
	// Delegate to the dialog chooser
	return m_pChooser->Next(pDlg);
}

// This is called when the user clicks "Back" on one of the custom
//  AppWizard's steps.
CAppWizStepDlg* CWKAppWizardAppWiz::Back(CAppWizStepDlg* pDlg)
{
	// Delegate to the dialog chooser
	return m_pChooser->Back(pDlg);
}

void CWKAppWizardAppWiz::CustomizeProject(IBuildProject* pProject)
{
	// This is called immediately after the default Debug and Release
	//  configurations have been created for each platform.  You may customize
	//  existing configurations on this project by using the methods
	//  of IBuildProject and IConfiguration such as AddToolSettings,
	//  RemoveToolSettings, and AddCustomBuildStep. These are documented in
	//  the Developer Studio object model documentation.

	IConfigurations *configurations;
	pProject->get_Configurations(&configurations);
	configurations->AddRef();

	long count;
	// TRACE ("Num cfgs %d\n",configurations->get_Count(&count));
	configurations->get_Count(&count);

	OLECHAR sDebugLibs[] = L"CipcDebug.lib WKClassesDebug.lib";
	OLECHAR sLibs[] = L"Cipc.lib WKClasses.lib";
	
	for (int i=1;i<=count && i<=2;i++) {
		IConfiguration *oneConfiguration;

		CComVariant ix(i);
		configurations->Item(ix, &oneConfiguration) ;
		oneConfiguration->AddRef();

		CComVariant Reserved;


		OLECHAR sCompiler[] = L"cl.exe";
		OLECHAR sLinker[] = L"link.exe";

		if (i==1) {
			// debug configuration
			oneConfiguration->AddToolSettings(
				SysAllocString(sLinker), 
				SysAllocString(sDebugLibs), 
				Reserved
				);
		} else {
			// release configuration
			oneConfiguration->AddToolSettings(
				SysAllocString(sLinker), 
				SysAllocString(sLibs), 
				Reserved
				);
		}

		OLECHAR sAlignment[] = L"/Zp1";
		oneConfiguration->AddToolSettings(
			SysAllocString(sCompiler), 
			SysAllocString(sAlignment), 
			Reserved
			);

		oneConfiguration->Release();
	} // end of loop over configurations

	configurations->Release();


	// WARNING!!  IBuildProject and all interfaces you can get from it are OLE
	//  COM interfaces.  You must be careful to release all new interfaces
	//  you acquire.  In accordance with the standard rules of COM, you must
	//  NOT release pProject, unless you explicitly AddRef it, since pProject
	//  is passed as an "in" parameter to this function.  See the documentation
	//  on CCustomAppWiz::CustomizeProject for more information.
}


// Here we define one instance of the CWKAppWizardAppWiz class.  You can access
//  m_Dictionary and any other public members of this class through the
//  global WKAppWizardaw.
CWKAppWizardAppWiz WKAppWizardaw;

