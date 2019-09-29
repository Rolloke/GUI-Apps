//this file is part of notepad++
//Copyright (C)2003 Don HO <donho@altern.org>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "PluginDefinition.h"
#include "menuCmdID.h"

//
// put the headers you need here
//
#include <stdlib.h>
#include <time.h>
#include <shlwapi.h>
#include "DockingFeature/SynchronizeTimeStamp.h"
#include "AboutDlg.h"

namespace
{
    const TCHAR sectionName[]    = TEXT("LogProfiler");
    const TCHAR keyName[]        = TEXT("doCloseTag");
    const TCHAR configFileName[] = TEXT("LogProfiler.ini");
}

SynchronizeTimeStamps gSynchronizeTimeStamps;

#ifdef UNICODE 
	#define generic_itoa _itow
#else
	#define generic_itoa itoa
#endif


FuncItem funcItem[NoOfFunctions];
HINSTANCE gInstance = 0;
//
// The data of Notepad++ that you can use in your plugin commands
//
NppData nppData;


TCHAR iniFilePath[MAX_PATH];


ScintillaWnd::ScintillaWnd(HWND aWnd, BOOL aDirect):
    mDirectFnc(0),
    mDirectData(0),
    mHwnd(0)
{
    if (aDirect)
    {
        mDirectFnc = (SciFnDirect)::SendMessage(aWnd, SCI_GETDIRECTFUNCTION, 0, 0);
        mDirectData = (sptr_t)::SendMessage(aWnd, SCI_GETDIRECTPOINTER, 0, 0);
    }
    else
    {
        mHwnd = aWnd;
    }
}

LRESULT ScintillaWnd::sendMessage(UINT aMsg, WPARAM wParam, LPARAM aParam) const
{
    if (mHwnd)
    {
        return ::SendMessage(mHwnd, aMsg, wParam, aParam);
    }
    else if (mDirectFnc)
    {
        return mDirectFnc(mDirectData, aMsg, wParam, aParam);
    }
    return 0;
}

//
// Initialize your plugin data here
// It will be called while plugin loading   
void pluginInit(HANDLE hModule)
{
	// Initialize dockable LogProfiler dialog
    gInstance = (HINSTANCE)hModule;
	gSynchronizeTimeStamps.init((HINSTANCE)hModule, NULL);
}

//
// Here you can do the clean up, save the parameters (if any) for the next session
//
void pluginCleanUp()
{
   
}

//
// Initialization of your plugin commands
// You should fill your plugins commands here
void commandMenuInit()
{
	//
	// Firstly we get the parameters from your plugin config file (if any)
	//

	// get path of plugin configuration
	::SendMessage(nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, (LPARAM)iniFilePath);

	// if config path doesn't exist, we create it
	if (PathFileExists(iniFilePath) == FALSE)
	{
		::CreateDirectory(iniFilePath, NULL);
	}

	// make your plugin config file full file path name
	PathAppend(iniFilePath, configFileName);

	// get the parameter value from plugin config
    //	doCloseTag = (::GetPrivateProfileInt(sectionName, keyName, 0, iniFilePath) != 0);

    //--------------------------------------------//
    //-- STEP 3. CUSTOMIZE YOUR PLUGIN COMMANDS --//
    //--------------------------------------------//
    // with function :
    // setCommand(int index,                      // zero based number to indicate the order of command
    //            TCHAR *commandName,             // the command name that you want to see in plugin menu
    //            PFUNCPLUGINCMD functionPointer, // the symbol of function (function pointer) associated with this command. The body should be defined below. See Step 4.
    //            ShortcutKey *shortcut,          // optional. Define a shortcut to trigger this command
    //            bool check0nInit                // optional. Make this menu item be checked visually
    //            );
    setCommand(About, TEXT("About"), AboutLogProfiler, NULL, false);
	// Here you insert a separator
	setCommand(Separator1, TEXT("---"), NULL, NULL, false);

	setCommand(DisplayLogProfiler, TEXT("Display Log Profiler"), displayLogProfiler, NULL, false);

	setCommand(DisplayProfilerResults, TEXT("Display Profiler Results"), displayProfilerResults, NULL, false);
}

void AboutLogProfiler()
{
    AboutDlg dlg;
    dlg.init(gInstance, getScintillaWindowHandle());
    dlg.doModal(IDD_ABOUTBOX);
}
//
// Here you can do the clean up (especially for the shortcut)
//
void commandMenuCleanUp()
{
	// Don't forget to deallocate your shortcut here

}

//----------------------------------------------//
//-- STEP 4. DEFINE YOUR ASSOCIATED FUNCTIONS --//
//----------------------------------------------//


//
// This function help you to initialize your plugin commands
//
bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool check0nInit) 
{
    if (index >= NoOfFunctions)
        return false;

    if (!pFunc)
        return false;

    lstrcpy(funcItem[index]._itemName, cmdName);
    funcItem[index]._pFunc = pFunc;
    funcItem[index]._init2Check = check0nInit;
    funcItem[index]._pShKey = sk;

    return true;
}

TCHAR* getIniFilePath()
{
    return iniFilePath;
}

void convertToUnicode(const std::string& aSource, std::wstring& aDest)
{
    aDest.resize(aSource.size());
    ::MultiByteToWideChar(CP_ACP, 0, aSource.c_str(), aSource.size(), &aDest[0], aDest.size());
    aDest.resize(wcslen(aDest.c_str()));
}

void convertToMBCS(const std::wstring& aSource, std::string& aDest)
{
    aDest.resize(aSource.size() * 2);
    ::WideCharToMultiByte(CP_ACP, 0, aSource.c_str(), aSource.size(), &aDest[0], aDest.size(), NULL, NULL);
    aDest.resize(strlen(aDest.c_str()));
}

HWND getScintillaWindowHandle()
{
    int which = -1;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
    if (which == -1)
        return NULL;

    return (which == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;
}

size_t ScintillaWnd::getLines() const
{
    return static_cast<size_t>(sendMessage(SCI_GETLINECOUNT));
}

bool ScintillaWnd::getLineText(uint32_t aLine, std::string& aLineText) const
{
    if (aLine >= 0)
    {
        Sci_TextRange fTextRange;

        fTextRange.chrg.cpMin = sendMessage(SCI_POSITIONFROMLINE, aLine);
        int fLineLength = sendMessage(SCI_LINELENGTH, aLine);
        fTextRange.chrg.cpMax = fTextRange.chrg.cpMin + fLineLength;
        aLineText.resize(fLineLength + 1);
        fTextRange.lpstrText = &aLineText[0];
        return sendMessage( SCI_GETTEXTRANGE, 0, (WPARAM)&fTextRange) > 0;
    }
    return false;
}

BOOL ScintillaWnd::selectLine(int aLine)
{
    if (aLine >= 0 )
    {
        int fPos = sendMessage(SCI_POSITIONFROMLINE, aLine);
        int fLineLength = sendMessage(SCI_LINELENGTH, aLine);
        return sendMessage(SCI_SETSEL, fPos, fPos + fLineLength - 1) >= 0;
    }
    return FALSE;
}

BOOL ScintillaWnd::defineMarker(int aMarker, int aSymbol, COLORREF aForeground, COLORREF aBackground)
{
    if (aForeground != -1)
    {
        sendMessage(SCI_MARKERSETFORE, aMarker, aForeground);
    }
    if (aBackground != -1)
    {
        sendMessage(SCI_MARKERSETBACK, aMarker, aBackground);
    }
    return sendMessage(SCI_MARKERDEFINE, aMarker, aSymbol) >= 0;
}

BOOL ScintillaWnd::setMarker(int aMarker, BOOL aSet, int aLine)
{
    if (aLine >= 0)
    {
        UINT fMessage = SCI_MARKERDELETE;
        if (aSet)
        {
            sendMessage(SCI_SETMARGINMASKN, 1, -1);
            fMessage = SCI_MARKERADD;
        }
        return sendMessage(fMessage, aLine, aMarker) >= 0;
    }
    else
    {
        return sendMessage(SCI_MARKERDELETEALL, aMarker) >= 0;
    }
}

BOOL ScintillaWnd::gotoMarker(int aMarker, BOOL aPrevious)
{
    int fLine = sendMessage(SCI_LINEFROMPOSITION, sendMessage(SCI_GETCURRENTPOS));
    if (aPrevious) fLine--;
    else           fLine++;
    uint32_t fMarkerMask = aMarker == -1 ? 0xffff : 1 << aMarker;
    fLine = sendMessage(aPrevious ? SCI_MARKERPREVIOUS : SCI_MARKERNEXT, fLine, fMarkerMask);
    if (fLine >= 0)
    {
        return gotoLine(fLine);
    }
    return FALSE;
}

BOOL ScintillaWnd::gotoLine(int aLine)
{
    if (aLine >= 0)
    {
        sendMessage(SCI_ENSUREVISIBLE, aLine);
        return sendMessage(SCI_GOTOLINE, aLine) >= 0;
    }
    return FALSE;
}

void  ScintillaWnd::addText(const std::string& aText)
{
    sendMessage(SCI_ADDTEXT, aText.size(), (LPARAM)&aText[0]);
}

void  ScintillaWnd::setReadOnly(BOOL bReadOnly)
{
    sendMessage(SCI_SETREADONLY, bReadOnly);
}


int countOpenFiles(int aFlag)
{
    return (int)::SendMessage(nppData._nppHandle, NPPM_GETNBOPENFILES, aFlag, 0);
}

int getOpenFiles(std::vector<std::string>* aFiles, std::vector<std::wstring>* aFilesU, int aFlag)
{
    int fFiles = countOpenFiles(aFlag);
    TCHAR **fileNames = (TCHAR **)new TCHAR*[fFiles];

    for (int i = 0; i < fFiles; ++i)
    {
        fileNames[i] = new TCHAR[MAX_PATH];
    }

    if (::SendMessage(nppData._nppHandle, NPPM_GETOPENFILENAMES, (WPARAM)fileNames, (LPARAM)fFiles))
    {
        for (int i = 0; i < fFiles; ++i)
        {
#ifdef UNICODE
			if (aFiles)
			{
				std::string fFileName;
				convertToMBCS(fileNames[i], fFileName);
				aFiles->push_back(fFileName);
			}
			if (aFilesU)
			{
				aFilesU->push_back(fileNames[i]);
			}
#else
			if (aFiles)
			{
				aFiles->push_back(fileNames[i]);
			}
			if (aFilesU)
			{
				std::wstring fFileName;
				convertToUnicode(fileNames[i], fFileName);
				aFilesU->push_back(fFileName);
			}
#endif 
        }
    }

    for (int i = 0; i < fFiles; ++i)
    {
        delete[] fileNames[i];
    }
    delete[] fileNames;

    return fFiles; 
}


void   openPathFileName(std::wstring& aPathFileName)
{
    ::SendMessage(nppData._nppHandle, NPPM_DOOPEN, 0, (LPARAM)&aPathFileName[0]);
}


void   getCurrentPathFileName(std::string& aPathFileName)
{
    TCHAR path[MAX_PATH];
    ::SendMessage(nppData._nppHandle, NPPM_GETFULLCURRENTPATH, MAX_PATH, (LPARAM)path);
	
#ifdef UNICODE
	convertToMBCS(path, aPathFileName);
#else
	aPathFileName = path;
#endif
}


// Dockable Dialog 
// 
// This demonstration shows you how to do a dockable dialog.
// You can create your own non dockable dialog - in this case you don't nedd this demonstration.
// You have to create your dialog by inherented DockingDlgInterface class in order to make your dialog dockable
// - please see DemoDlg.h and DemoDlg.cpp to have more informations.
void displayLogProfiler()
{
	gSynchronizeTimeStamps.setParent(nppData._nppHandle);
	tTbData	data = {0};

	if (!gSynchronizeTimeStamps.isCreated())
	{
		gSynchronizeTimeStamps.create(&data);

		// define the default docking behaviour
		data.uMask = DWS_DF_CONT_RIGHT;

		data.pszModuleName = gSynchronizeTimeStamps.getPluginFileName();

		// the dlgDlg should be the index of funcItem where the current function pointer is
		// in this case is DOCKABLE_DEMO_INDEX
		data.dlgID = DisplayLogProfiler;
		::SendMessage(nppData._nppHandle, NPPM_DMMREGASDCKDLG, 0, (LPARAM)&data);
	}
	gSynchronizeTimeStamps.display();
}


void displayProfilerResults()
{
	gSynchronizeTimeStamps.openTimeStampResults();
}