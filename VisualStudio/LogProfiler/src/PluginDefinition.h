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

#ifndef PLUGINDEFINITION_H
#define PLUGINDEFINITION_H

//
// All difinitions of plugin interface
// Help: https://npp-user-manual.org/docs/plugin-communication
//
// TODO:
// NPPM_MODELESSDIALOG
// NPPM_DESTROYSCINTILLAHANDLE
// NPPM_SETSTATUSBAR
#include "PluginInterface.h"
#include "TextInterface.h"
#include <string>
#include <vector>

#include <CommCtrl.h>

#define PREVIOUS TRUE
#define NEXT     FALSE


class ScintillaWnd: public ITextInterface
{
public:
    ScintillaWnd(HWND aWnd, BOOL aDirect=FALSE);
    LRESULT sendMessage(UINT aMsg, WPARAM wParam=0, LPARAM aParam=0) const;

    size_t getLines() const;
    /*!
    \brief retrieves a text line from given scintilla window
    \param curScintilla window handle
    \param aLine requested line
    \param aLineText retrieved text
    \return (TRUE, FALSE)
    */
    bool getLineText(uint32_t aLine, std::string& aLineText) const;
    /*!
    \brief selects a text line in given scintilla window
    \param curScintilla window handle
    \param aLine requested line
    \return (TRUE, FALSE)
    */
    BOOL selectLine(INT_PTR aLine);

    /*!
    \brief sets a maker for the text line in given scintilla window
    \param curScintilla window handle
    \param aMarkerNo Marker Number
    \param aSymbol (SC_MARK_CIRCLE, SC_MARK_ROUNDRECT, SC_MARK_BOOKMARK, SC_MARK_UNDERLINE, SC_MARK_ARROW,
    SC_MARK_SMALLRECT, SC_MARK_SHORTARROW, SC_MARK_EMPTY, SC_MARK_ARROWDOWN, SC_MARK_MINUS,
    SC_MARK_PLUS, SC_MARK_ARROWS, SC_MARK_DOTDOTDOT, SC_MARK_BACKGROUND, SC_MARK_LEFTRECT,
    SC_MARK_FULLRECT)
    \return (TRUE, FALSE)
    */
    BOOL defineMarker(INT_PTR aMarker, INT_PTR aSymbol = SC_MARK_CIRCLE, COLORREF aForeground = -1, COLORREF aBackground = -1);

    /*!
    \brief sets a maker for the text line in given scintilla window
    \param curScintilla window handle
    \param aMarker Marker number
    \param aSet (TRUE = set marker, FALSE = delete marker)
    \param aLine requested line
    \return (TRUE, FALSE)
    */
    BOOL setMarker(INT_PTR aMarker, BOOL aSet = TRUE, INT_PTR aLine = -1);

    /*!
    \brief sets a maker for the text line in given scintilla window
    \param curScintilla window handle
    \param aPrevious previous line or next line
    \param aMarker Marker number
    \return (TRUE, FALSE)
    */
    BOOL gotoMarker(INT_PTR aMarker, BOOL aPrevious);

    /*!
    \brief ensures visiblity of text line in given scintilla window
    \param curScintilla window handle
    \param aLine requested line
    \return (TRUE, FALSE)
    */
    BOOL gotoLine(INT_PTR aLine);

    void addText(const std::string& aText);
    void setReadOnly(BOOL bReadOnly);

private:

    SciFnDirect mDirectFnc;
    sptr_t      mDirectData;
    HWND        mHwnd;
};


//-------------------------------------//
//-- STEP 1. DEFINE YOUR PLUGIN NAME --//
//-------------------------------------//
// Here define your plugin name
//
const TCHAR NPP_PLUGIN_NAME[] = TEXT("LogProfiler");

//-----------------------------------------------//
//-- STEP 2. DEFINE YOUR PLUGIN COMMAND NUMBER --//
//-----------------------------------------------//
//
// Here define the number of your plugin commands
//

enum MemuItems 
{    
    DisplayLogProfiler, DisplayProfilerResults, Separator1, About, NoOfFunctions
};


/*!
    \brief retrieves the inifile path and filename
*/
TCHAR* getIniFilePath();


#ifndef UNICODE
void   convertToUnicode(const std::string& aSource, std::string& aDest);
void   convertToMBCS(const std::string& aSource, std::string& aDest);
#endif

/*!
    \brief conversion from MBCS toUnicode and vice versa
*/
void   convertToUnicode(const std::string& aSource, std::wstring& aDest);
void   convertToMBCS(const std::wstring& aSource, std::string& aDest);

//
// Initialization of your plugin data
// It will be called while plugin loading
//
void pluginInit(HANDLE hModule);

//
// Cleaning of your plugin
// It will be called while plugin unloading
//
void pluginCleanUp();

//
//Initialization of your plugin commands
//
void commandMenuInit();

//
//Clean up your plugin commands allocation (if any)
//
void commandMenuCleanUp();

//
// Function which sets your command 
//
bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk = NULL, bool check0nInit = false);

/*!
\brief retrieves the active scintilla window
*/
HWND  getScintillaWindowHandle();




/*!
\brief retrieves the number of all files
\param aFlag (ALL_OPEN_FILES, PRIMARY_VIEW, SECOND_VIEW)
*/
INT_PTR    countOpenFiles(INT_PTR aFlag = ALL_OPEN_FILES);

/*!
\brief retrieves the active scintilla window
\param aFiles result vector with filenames
\param aFilesU result vector with filenames in Unicode
\param aFlag (ALL_OPEN_FILES, PRIMARY_VIEW, SECOND_VIEW)
*/
INT_PTR    getOpenFiles(std::vector<std::string>* aFiles, std::vector<std::wstring>* aFilesU=NULL, INT_PTR aFlag = ALL_OPEN_FILES);

void   getCurrentPathFileName(std::string& aPathFileName);

void   openPathFileName(std::wstring& aPathFileName);

//
// Your plugin command functions
//
void AboutLogProfiler();

void displayLogProfiler();
void displayProfilerResults();


#endif //PLUGINDEFINITION_H
