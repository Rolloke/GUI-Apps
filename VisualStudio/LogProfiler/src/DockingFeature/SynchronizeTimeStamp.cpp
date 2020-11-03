//this file is part of notepad++
//Copyright (C)2003 Don HO ( donho@altern.org )
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


#include "SynchronizeTimeStamp.h"
#include "SynchronizeTimeResults.h"

#include "..\TextFile.h"
#include "..\SearchLine.h"
#include "..\PluginDefinition.h"
#include "..\EnumFiles.h"
#include "..\logger.h"

#pragma warning( push )  
#pragma warning(disable: 4091)
#include <Shlobj.h>
#pragma warning( pop ) 

extern NppData nppData;

namespace
{
    const TCHAR sectionName[]     = TEXT("SynchronizeTimeStamps");
    const TCHAR keyFormats[]      = TEXT("NoOfTimeFormats");
    const TCHAR keyFormat[]       = TEXT("TimeFormat%02d");
    const TCHAR keySelectLine[]   = TEXT("SelectLine");
    const TCHAR keyBookmarkLine[] = TEXT("BookmarkLine");
    const TCHAR keyMarker[]       = TEXT("Marker");
    const TCHAR keyUseThreads[]   = TEXT("UseThread");
    const TCHAR keyLineLimit[]    = TEXT("LineLimit");
    const TCHAR keySearchInSubFolders[] = TEXT("SearchInSubFolders");
    const TCHAR keyMaxSearchDifference[] = TEXT("MaxSearchDifference");
}


SynchronizeTimeStamps::SynchronizeTimeStamps() 
    : DockingDlgInterface(IDD_SYNCHRONIZE_TIMESTAMPS) 
    , mTestParser(0)
    , mSelectFoundLine(FALSE)
    , mBookmarkFoundLine(TRUE)
    , mMarker(0)
    , mUseThreads(TRUE)
    , mLineLimit(128)
    , mSearchInSubFolders(TRUE)
    , mMaxSearchDifference_us(3600000000)
    , mActiveFile(NULL)
    , mTimeStampResults(NULL)
{
    mLastFoundParser = mParserToFormatMap.end();
}

SynchronizeTimeStamps::~SynchronizeTimeStamps()
{
}

INT_PTR CALLBACK SynchronizeTimeStamps::run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
    case WM_INITDIALOG: return OnInitDialog();
	case WM_COMMAND : 
	{
		switch (LOWORD(wParam))
		{
        case IDC_BTN_GET_TIME:           return StartThread(IDC_BTN_GET_TIME); //return OnGetTime();
        case IDC_BTN_FIND_TIME:          return StartThread(IDC_BTN_FIND_TIME); //return OnFindTime();
		case IDC_BTN_DIFF_TIME:          
			openTimeStampResults();
			return StartThread(IDC_BTN_DIFF_TIME); //return OnDiffTime();
		case IDC_BTN_SYNCHRONIZE:
			openTimeStampResults();
			return StartThread(IDC_BTN_SYNCHRONIZE);        //return OnSyncronizeOpenFiles();
        case IDC_BTN_SYNCHRONIZE_FOLDER:
			openTimeStampResults();
			return StartThread(IDC_BTN_SYNCHRONIZE_FOLDER); // return OnSyncronizeFolder();
        case IDC_BTN_INSERT: return OnInsertFormat();
        case IDC_BTN_DELETE: return OnDeleteFormat();
        case IDC_CK_SELECT_LINE: return OnCkSelectLine();
        case IDC_CK_BOOKMARK_LINE: return OnCkBookmarkLine();
        case IDC_BTN_NEXT_BOOKMARK: return OnBtnNextBookmark();
        case IDC_BTN_PREVIOUS_BOOKMARK: return OnBtnPreviousBookmark();
        case IDC_BTN_DELETE_BOOKMARKS: return OnBtnDeleteBookmarks();
        case IDC_BTN_DELETE_CURRENT_BOOKMARK: return OnBtnDeleteCurrentBookmark();
        case IDC_BTN_HELP: return OnBtnHelp();
		case IDC_BTN_SELECT_FOLDER: return OnSelectFolder();
        case IDC_BTN_STOP_ALL: return OnStopAllThreads();
		case IDC_EDT_BOOKMARK:
            if (HIWORD(wParam) == EN_CHANGE)
            {
                mMarker = getDlgItemInt(IDC_EDT_BOOKMARK);
            }
            return 1;
        }
		return FALSE;
    }break;
    case WM_NOTIFY: return OnWmNotify((UINT)wParam, (NMHDR*)lParam);
    case WM_HELP: return OnWmHelp((HELPINFO*)lParam);
    case WM_DESTROY: return OnWmDestroy();
        

	default: return DockingDlgInterface::run_dlgProc(message, wParam, lParam);
	}
}

INT_PTR SynchronizeTimeStamps::OnSyncronizeFolder()
{
    return OnSynchronize(TRUE);
}

INT_PTR SynchronizeTimeStamps::OnSyncronizeOpenFiles()
{
    return OnSynchronize(FALSE);
}

INT_PTR SynchronizeTimeStamps::OnInitDialog()
{
	TRACE(Logger::info, "SynchronizeTimeStamps::OnInitDialog()");
	RECT rc;
    GetClientRect(getDlgItem(IDC_LIST_TIME_FORMAT), &rc);
    INT_PTR fDateCx = 40;
    std::string sText = "Supported time formats";
	LVCOLUMNA fLvc1 = { 0 };
	fLvc1.mask = LVCF_WIDTH | LVCF_TEXT;
	fLvc1.cx = static_cast<int>(static_cast<INT_PTR>(rc.right - rc.left) - fDateCx);
	fLvc1.pszText = &sText[0];
	fLvc1.cchTextMax = static_cast<int>(sText.size());
	fLvc1.cxMin = 10;
//		= { , 0, , &sText[0], (int)sText.size(), 0, 0, 0, 10, 0, 0  };
    sendDlgItemMsg(IDC_LIST_TIME_FORMAT, LVM_INSERTCOLUMNA, 0, (LPARAM)&fLvc1);
    sText = "Date";
    LVCOLUMNA fLvc2 = { 0 };
	fLvc2.mask = LVCF_WIDTH | LVCF_TEXT;
	fLvc2.cx = static_cast<int>(fDateCx);
	fLvc2.pszText = &sText[0];
	fLvc2.cchTextMax = static_cast<int>(sText.size());
	fLvc2.cxMin = 10;
	sendDlgItemMsg(IDC_LIST_TIME_FORMAT, LVM_INSERTCOLUMNA, 1, (LPARAM)&fLvc2);

    mTestParser.reset(new TimeParser);

    TCHAR* szPredefinedTimeFormat[] = 
	{
        _T("[%a %b %e %H:%M:%S %Y]"),
        _T("[%d.%m.%Y-%H:%M:%S]"),
        _T("[%d.%m.%Y-%H:%M:%S.%i]"),
        _T("[%m/%d/%Y %H:%M:%S.%i %X %x]"),
        _T("%Y-%m-%dT%H:%M:%S.%i%x+%x:%x")
	};
    const size_t fPredefinedFormats = lengthof(szPredefinedTimeFormat);
	size_t fTimeFormats = ::GetPrivateProfileInt(sectionName, keyFormats, 0, getIniFilePath());
    TCHAR szTimeFormatKey[256], szTimeFormat[256];
    if (fTimeFormats == 0)
    {
        fTimeFormats = fPredefinedFormats;
    }
    for (size_t i = 0; i < fTimeFormats; ++i)
    {
        wsprintf(szTimeFormatKey, keyFormat, i+1);
        memset(szTimeFormat, 0, sizeof(szTimeFormat));
        GetPrivateProfileString(sectionName, szTimeFormatKey, i<fPredefinedFormats ? szPredefinedTimeFormat[i] : _T(""), szTimeFormat, lengthof(szTimeFormat), getIniFilePath());
        LVITEM fItem = { LVIF_TEXT, static_cast<int>(i), 0, 0, 0, &szTimeFormat[0], static_cast<int>(_tcslen(szTimeFormat)), 0, 0, 0 };
        sendDlgItemMsg(IDC_LIST_TIME_FORMAT, LVM_INSERTITEM, i, (LPARAM)&fItem);
        std::string fTimeFormat;
		convertToMBCS(szTimeFormat, fTimeFormat);

        mParserToFormatMap[fTimeFormat].reset(new TimeParser());
        mParserToFormatMap[fTimeFormat]->setFormatString(fTimeFormat);
        _tcscpy(szTimeFormat, (mParserToFormatMap[fTimeFormat]->hasDate()) ? _T("yes") : _T("no"));
        fItem.iSubItem = 1;
        fItem.cchTextMax = (int)_tcslen(szTimeFormat);
        sendDlgItemMsg(IDC_LIST_TIME_FORMAT, LVM_SETITEM, i, (LPARAM)&fItem);
    }
    
    mSelectFoundLine   = ::GetPrivateProfileInt(sectionName, keySelectLine, mSelectFoundLine, getIniFilePath());
    sendDlgItemMsg(IDC_CK_SELECT_LINE, BM_SETCHECK, mSelectFoundLine);
    mBookmarkFoundLine = ::GetPrivateProfileInt(sectionName, keyBookmarkLine, mBookmarkFoundLine, getIniFilePath());
    sendDlgItemMsg(IDC_CK_BOOKMARK_LINE, BM_SETCHECK, mBookmarkFoundLine);
    mMarker = ::GetPrivateProfileInt(sectionName, keyMarker, mMarker, getIniFilePath());
    setDlgItemInt(IDC_EDT_BOOKMARK, mMarker);
    mUseThreads = ::GetPrivateProfileInt(sectionName, keyUseThreads, mUseThreads, getIniFilePath());
    mLineLimit = ::GetPrivateProfileInt(sectionName, keyLineLimit, mLineLimit, getIniFilePath());
    mSearchInSubFolders = ::GetPrivateProfileInt(sectionName, keySearchInSubFolders, mSearchInSubFolders, getIniFilePath());
    sendDlgItemMsg(IDC_CK_SEARCH_IN_SUBFOLDERS, BM_SETCHECK, mSearchInSubFolders ? BST_CHECKED : BST_UNCHECKED);
    GetPrivateProfileString(sectionName, keyMaxSearchDifference, _T("1:00:00"), szTimeFormat, lengthof(szTimeFormat), getIniFilePath());
    long fHour, fMinute, fSecond;
    INT_PTR fScanResult = _stscanf(szTimeFormat, _T("%02d:%02d:%02d"), &fHour, &fMinute, &fSecond);
    if (fScanResult == 3)
    {
        SYSTEMTIME fST = { 0, 1, 0, 0, static_cast<WORD>(fHour), static_cast<WORD>(fMinute), static_cast<WORD>(fSecond), 0 };
        mTestParser->setSystemTime(fST);
        mMaxSearchDifference_us = mTestParser->calculateTime();
    }

	setDlgItemInt(IDC_TXT_MILLISECONDS, 0);
	setDlgItemInt(IDC_TXT_MICROSECONDS_PART, 0);

    ScintillaWnd fwndSC(getScintillaWindowHandle(), TRUE);
    INT_PTR i = 0;
    COLORREF fForegound = RGB(255, 0, 0);
    fwndSC.defineMarker(i++, SC_MARK_CIRCLE, fForegound);
    fwndSC.defineMarker(i++, SC_MARK_ROUNDRECT, fForegound);
    fwndSC.defineMarker(i++, SC_MARK_BOOKMARK, fForegound);
    fwndSC.defineMarker(i++, SC_MARK_ARROW, fForegound);
    fwndSC.defineMarker(i++, SC_MARK_SMALLRECT, fForegound);
    fwndSC.defineMarker(i++, SC_MARK_SHORTARROW, fForegound);
    fwndSC.defineMarker(i++, SC_MARK_ARROWDOWN, fForegound);
    fwndSC.defineMarker(i++, SC_MARK_MINUS, fForegound);
    fwndSC.defineMarker(i++, SC_MARK_PLUS, fForegound);
    fwndSC.defineMarker(i++, SC_MARK_ARROWS, fForegound);
    fwndSC.defineMarker(i++, SC_MARK_DOTDOTDOT, fForegound);

    sendDlgItemMsg(IDC_SPIN_BOOKMARK, UDM_SETRANGE, 0, MAKELPARAM(0, i-1));

    enableToolTip();
    for (UINT nID = IDC_TIME; nID <= IDC_LAST_IDD_SYNCHRONIZE_TIMESTAMPS; ++nID)
    {
        addToolTip(nID);
    }
  
	SetDlgItemTextA(_hSelf, IDC_EDT_WILDCARD, "*.*");
    
    //determine Messages for:
     // - open file
     // - close file
    return TRUE;
}

INT_PTR SynchronizeTimeStamps::OnWmDestroy()
{
    OnStopAllThreads();
    INT_PTR fTimeFormats = sendDlgItemMsg(IDC_LIST_TIME_FORMAT, LVM_GETITEMCOUNT);
    TCHAR szFormatKey[64], szTimeFormat[256];
    wsprintf(szFormatKey, _T("%d"), fTimeFormats);
    ::WritePrivateProfileString(sectionName, keyFormats, szFormatKey, getIniFilePath());
    for (INT_PTR i = 1; i <= fTimeFormats; ++i)
    {
        wsprintf(szFormatKey, keyFormat, i);
        LVITEM fItem = { LVIF_TEXT, static_cast<int>(i-1), 0, 0, 0, &szTimeFormat[0], static_cast<int>(lengthof(szTimeFormat)), 0, 0, 0 };
        sendDlgItemMsg(IDC_LIST_TIME_FORMAT, LVM_GETITEM, 0, (LPARAM)&fItem);
        wsprintf(szFormatKey, keyFormat, i);
        WritePrivateProfileString(sectionName, szFormatKey, szTimeFormat, getIniFilePath());
    }
    wsprintf(szFormatKey, _T("%d"), mSelectFoundLine);
    ::WritePrivateProfileString(sectionName, keySelectLine, szFormatKey, getIniFilePath());
    wsprintf(szFormatKey, _T("%d"), mBookmarkFoundLine);
    ::WritePrivateProfileString(sectionName, keyBookmarkLine, szFormatKey, getIniFilePath());
    wsprintf(szFormatKey, _T("%d"), mMarker);
    ::WritePrivateProfileString(sectionName, keyMarker, szFormatKey, getIniFilePath());
    wsprintf(szFormatKey, _T("%d"), mUseThreads);
    ::WritePrivateProfileString(sectionName, keyUseThreads, szFormatKey, getIniFilePath());
    wsprintf(szFormatKey, _T("%d"), mLineLimit);
    ::WritePrivateProfileString(sectionName, keyLineLimit, szFormatKey, getIniFilePath());
    wsprintf(szFormatKey, _T("%d"), mSearchInSubFolders);
    ::WritePrivateProfileString(sectionName, keySearchInSubFolders, szFormatKey, getIniFilePath());
    SYSTEMTIME fDifference = TimeParser::calcSystemTime(mMaxSearchDifference_us);
    wsprintf(szFormatKey, _T("%02d:%02d:%02d"), fDifference.wHour, fDifference.wMinute, fDifference.wSecond);
    ::WritePrivateProfileString(sectionName, keyMaxSearchDifference, szFormatKey, getIniFilePath());

    return TRUE;
}

INT_PTR SynchronizeTimeStamps::OnWmHelp(HELPINFO* aHelp)
{
    aHelp->iCtrlId;
    return 0;
}

INT_PTR SynchronizeTimeStamps::OnWmNotify(UINT , NMHDR* pNmHdr)
{
    INT_PTR fReturn = 0;
    switch (pNmHdr->code)
    {
    case NM_DBLCLK:
        if (pNmHdr->idFrom == IDC_LIST_TIME_FORMAT)
        {
			INT_PTR fSelected = sendDlgItemMsg(IDC_LIST_TIME_FORMAT, LVM_GETSELECTIONMARK);
            if (fSelected != -1)
            {
                char fTimeFormat[256];
                LVITEMA fItem = { LVIF_TEXT, static_cast<int>(fSelected), 0, 0, 0, &fTimeFormat[0], static_cast<int>(lengthof(fTimeFormat)), 0, 0, 0 };
                sendDlgItemMsg(IDC_LIST_TIME_FORMAT, LVM_GETITEMA, 0, (LPARAM)&fItem);
                SetDlgItemTextA(_hSelf, IDC_EDT_TIME_FORMAT, fTimeFormat);
            }
        }
        break;
    }
    return fReturn;
}

INT_PTR SynchronizeTimeStamps::OnInsertFormat()
{
    AutoMutex fM(&mMutex);
    std::string fTimeFormatA;
    getDlgItemText(IDC_EDT_TIME_FORMAT, fTimeFormatA);

    mParserToFormatMap[fTimeFormatA].reset(new TimeParser());
    if (mParserToFormatMap[fTimeFormatA]->setFormatString(fTimeFormatA))
    {
        std::wstring fTimeFormat;
        getDlgItemText(IDC_EDT_TIME_FORMAT, fTimeFormat);
        LVITEMW fItem = { LVIF_TEXT, 0, 0, 0, 0, &fTimeFormat[0], (int)fTimeFormat.size(), 0, 0, 0 };
        sendDlgItemMsg(IDC_LIST_TIME_FORMAT, LVM_INSERTITEMW, 0, (LPARAM)&fItem);
        SetDlgItemTextA(_hSelf, IDC_EDT_TIME_FORMAT, "");
    }
    else
    {
        mParserToFormatMap.erase(fTimeFormatA);
    }
    mLastFoundParser = mParserToFormatMap.end();

    return TRUE;
}

INT_PTR SynchronizeTimeStamps::OnDeleteFormat()
{
    AutoMutex fM(&mMutex);
    INT_PTR fSelected = ListView_GetSelectionMark(getDlgItem(IDC_LIST_TIME_FORMAT));
    if (fSelected != -1)
    {
        char fTimeFormat[256];
        LVITEMA fItem = { LVIF_TEXT, static_cast<int>(fSelected), 0, 0, 0, &fTimeFormat[0], static_cast<int>(lengthof(fTimeFormat)), 0, 0, 0 };
        sendDlgItemMsg(IDC_LIST_TIME_FORMAT, LVM_GETITEMA, 0, (LPARAM)&fItem);
        ParserToFormatMap::iterator fIt = mParserToFormatMap.find(fTimeFormat);
        if (fIt != mParserToFormatMap.end())
        {
            removeDocParser(fIt->second.get());
            mParserToFormatMap.erase(fIt);
        }
        ListView_DeleteItem(getDlgItem(IDC_LIST_TIME_FORMAT), fSelected);
        mLastFoundParser = mParserToFormatMap.end();
    }
    return TRUE;
}

INT_PTR SynchronizeTimeStamps::OnCkSelectLine()
{
    mSelectFoundLine = sendDlgItemMsg(IDC_CK_SELECT_LINE, BM_GETCHECK) != 0;
    return TRUE;
}

INT_PTR SynchronizeTimeStamps::OnCkBookmarkLine()
{
    mBookmarkFoundLine = sendDlgItemMsg(IDC_CK_BOOKMARK_LINE, BM_GETCHECK) != 0;
    return TRUE;
}

INT_PTR SynchronizeTimeStamps::OnBtnNextBookmark()
{
    ScintillaWnd(getScintillaWindowHandle()).gotoMarker(mMarker, NEXT);
    return TRUE;
}

INT_PTR SynchronizeTimeStamps::OnBtnPreviousBookmark()
{
    ScintillaWnd(getScintillaWindowHandle()).gotoMarker(mMarker, PREVIOUS);
    return TRUE;
}

INT_PTR SynchronizeTimeStamps::OnBtnHelp()
{
    std::string  fPath;
	convertToMBCS(_modulePath, fPath);

    fPath = "explorer " + fPath; 
    fPath += "\\doc\\LogProfiler\\info.html";
    system(fPath.c_str());
    return TRUE;
}

LRESULT SynchronizeTimeStamps::OnBtnDeleteBookmarks()
{
    ScintillaWnd(getScintillaWindowHandle()).setMarker(mMarker, FALSE);
    return TRUE;
}

LRESULT SynchronizeTimeStamps::OnBtnDeleteCurrentBookmark()
{
    ScintillaWnd(getScintillaWindowHandle()).setMarker(mMarker, FALSE, getCurrentLine());
    return TRUE;
}

LRESULT SynchronizeTimeStamps::getCurrentLine() const
{
    ScintillaWnd fWnd(getScintillaWindowHandle(), FALSE);
	INT_PTR fPos = fWnd.sendMessage(SCI_GETCURRENTPOS);
    return fWnd.sendMessage(SCI_LINEFROMPOSITION, fPos);
}

TimeParser* SynchronizeTimeStamps::getCurrentParser()  
{
    AutoMutex fM(&mMutex);
    std::string fFileName;
    getActivePathFileName(fFileName);
    ParserToFileMap::const_iterator fIt = mParserToFileMap.find(fFileName);
    if (fIt != mParserToFileMap.end())
    {
        return fIt->second.get();
    }
    else
    {
        return findDocParser();
    }
}

TimeParser* SynchronizeTimeStamps::findDocParser()
{
    ITextInterface *pTI = 0;
    
    ScintillaWnd fWnd(getScintillaWindowHandle(), TRUE);
    
    if (mActiveFile)
    {
        pTI = mActiveFile;
    }
    else
    {
        pTI = &fWnd;
    }
    if (pTI)
    {
        uint32_t fLines = 10;
        std::string fText;

        for (uint32_t fLine=0; fLine < fLines; ++fLine)
        {
            if (pTI->getLineText(fLine, fText))
            {
                // try last found parser first
                if (mLastFoundParser != mParserToFormatMap.end() && mLastFoundParser->second->parseTimeString(fText))
                {
                    std::string fPathFileName;
                    getActivePathFileName(fPathFileName);
                    mParserToFileMap[fPathFileName] = mLastFoundParser->second;
                    return mParserToFileMap[fPathFileName].get();
                }
                for (auto fParserIt = mParserToFormatMap.begin(); fParserIt != mParserToFormatMap.end(); ++fParserIt)
                {
                    if (fParserIt != mLastFoundParser && fParserIt->second->parseTimeString(fText))
                    {
                        std::string fPathFileName;
                        getActivePathFileName(fPathFileName);
                        mParserToFileMap[fPathFileName] = fParserIt->second;
                        mLastFoundParser = fParserIt;
                        LVFINDINFOA fFI = { LVFI_STRING, fParserIt->first.c_str(), 0, {0,0}, 0 };
                        INT_PTR fFound = sendDlgItemMsg(IDC_LIST_TIME_FORMAT, LVM_FINDITEMA, 0, (LPARAM)&fFI);
                        ListView_SetSelectionMark(getDlgItem(IDC_LIST_TIME_FORMAT), fFound);
                        ListView_SetItemState(getDlgItem(IDC_LIST_TIME_FORMAT), fFound, LVIS_FOCUSED | LVIS_SELECTED, 0x000F);

                        return mParserToFileMap[fPathFileName].get();
                    }
                }
            }
        }
    }
	if (mActiveFile)
	{
		TRACE(Logger::to_function, "No parser found for %s", mActiveFile->getFileName());
	}
	else
	{
		std::string fPath;
		getCurrentPathFileName(fPath);
		TRACE(Logger::to_function, "No parser found for %s", fPath.c_str());
	}

    return NULL;
}

// TODO: show progress bar

INT_PTR SynchronizeTimeStamps::StartThread(UINT nID)
{
    if (mUseThreads)
    {
        mMutex.lock();
        mThreadFunctionID = nID;
        mMutex.unlock();

        DWORD fThreadID;
        HANDLE hThread = CreateThread(NULL, 0, ThreadProc, this, 0, &fThreadID);

        mMutex.lock();
        mThreads[fThreadID] = hThread;
        mMutex.unlock();

        EnableWindow(getDlgItem(IDC_BTN_STOP_ALL), TRUE);
    }
    else
    {
        mThreadFunctionID = nID;
        ThreadProc();
    }
    return TRUE;
}

DWORD WINAPI SynchronizeTimeStamps::ThreadProc(LPVOID lpParameter)
{
    SynchronizeTimeStamps*pThis = (SynchronizeTimeStamps*)lpParameter;
    DWORD fResult = pThis->ThreadProc();
    ExitThread(fResult);
//    return fResult;
}

DWORD WINAPI SynchronizeTimeStamps::ThreadProc()
{
    UINT fThreadID = GetCurrentThreadId();
    UINT  fID = 0;
    {
        AutoMutex fM(&mMutex);
        fID = mThreadFunctionID;
        mThreadFunctionID = 0;
    }

    DWORD fResult = 0;
    switch (fID)
    {
    case IDC_BTN_GET_TIME:           fResult = static_cast<DWORD>(OnGetTime()); break;
    case IDC_BTN_FIND_TIME:          fResult = static_cast<DWORD>(OnFindTime()); break;
	case IDC_BTN_DIFF_TIME:          fResult = static_cast<DWORD>(OnDiffTime()); break;
	case IDC_BTN_SYNCHRONIZE:        fResult = static_cast<DWORD>(OnSyncronizeOpenFiles()); break;
    case IDC_BTN_SYNCHRONIZE_FOLDER: fResult = static_cast<DWORD>(OnSyncronizeFolder());    break;
    }

    EnableWindow(getDlgItem(IDC_BTN_STOP_ALL), FALSE);
    {
        AutoMutex fM(&mMutex);
        ThreadHandleMap::iterator fThreadParam = mThreads.find(fThreadID);
        if (fThreadParam != mThreads.end())
        {
            CloseHandle(fThreadParam->second);
            mThreads.erase(fThreadParam);
        }
    }

    return fResult;
}

INT_PTR SynchronizeTimeStamps::OnStopAllThreads()
{
    AutoMutex fM(&mMutex);
    for (auto fThread : mThreads)
    {
        TerminateThread(fThread.second, (DWORD) -1);
    }
    return TRUE;
}

void  SynchronizeTimeStamps::removeDocParser(TimeParser* aParser)
{
    for (ParserToFileMap::iterator fParser = mParserToFileMap.begin(); fParser != mParserToFileMap.end(); ++fParser)
    {
        if (fParser->second.get() == aParser)
        {
            mParserToFileMap.erase(fParser);
            fParser = mParserToFileMap.begin();
        }
    }
}

INT_PTR  SynchronizeTimeStamps::findLineOfTimeValue(int64_t aTimeValue)
{
    INT_PTR fResultLine = -1;
    HWND curScintilla = getScintillaWindowHandle();
    if (curScintilla)
    {
        ScintillaWnd fWnd(curScintilla, TRUE);
        SearchLine fSearch(fWnd, getCurrentParser());
        fResultLine = fSearch.searchLine(aTimeValue);
    }
    return fResultLine;
}

void SynchronizeTimeStamps::getActivePathFileName(std::string& aFileName)
{
    AutoMutex fM(&mMutex);
    if (mActiveFile)
    {
        aFileName = mActiveFile->getFileName();
    }
    else
    {
        getCurrentPathFileName(aFileName);
    }
}

SYSTEMTIME  SynchronizeTimeStamps::getCtrlTime()
{
    SYSTEMTIME fTime, fDate;

    SendDlgItemMessageA(_hSelf, IDC_TIME, DTM_GETSYSTEMTIME, GDT_VALID, (LPARAM)&fTime);
    SendDlgItemMessageA(_hSelf, IDC_DATE, DTM_GETSYSTEMTIME, GDT_VALID, (LPARAM)&fDate);

    fTime.wDay = fDate.wDay;
    fTime.wDayOfWeek = fDate.wDayOfWeek;
    fTime.wMonth = fDate.wMonth;
    fTime.wYear = fDate.wYear;
    fTime.wMilliseconds = static_cast<WORD>(getDlgItemInt(IDC_TXT_MILLISECONDS));
    return fTime;
}

INT_PTR SynchronizeTimeStamps::OnFindTime()
{
    TimeParser* fParser = getCurrentParser();
    if (fParser)
    {
        fParser->setSystemTime(getCtrlTime(), static_cast<WORD>(getDlgItemInt(IDC_TXT_MICROSECONDS_PART)));

        ScintillaWnd fWnd(getScintillaWindowHandle(), TRUE);
        INT_PTR fLine = findLineOfTimeValue(fParser->calculateTime());
        if (fLine >= 0)
        {
            if (mSelectFoundLine)
            {
                fWnd.selectLine(fLine);
            }
            if (mBookmarkFoundLine)
            {
                fWnd.setMarker(mMarker, TRUE, fLine);
            }
            fWnd.gotoLine(fLine);
        }
    }
    return TRUE;
}

tstring toTime(const SYSTEMTIME& fST, WORD fMicrosecondPart)
{
	TCHAR szText[MAX_PATH];
	if (fMicrosecondPart)
	{
		int fMicroseconds = fST.wMilliseconds * 1000 + fMicrosecondPart;
		_stprintf(szText, _T("[%02d:%02d:%02d.%06d, %02d.%02d.%04d]"), fST.wHour, fST.wMinute, fST.wSecond, fMicroseconds, fST.wDay, fST.wMonth, fST.wYear);
	}
	else
	{
		_stprintf(szText, _T("[%02d:%02d:%02d.%03d, %02d.%02d.%04d]"), fST.wHour, fST.wMinute, fST.wSecond, fST.wMilliseconds, fST.wDay, fST.wMonth, fST.wYear);
	}
	return szText;
}

INT_PTR SynchronizeTimeStamps::OnSynchronize(BOOL bFolder)
{
    TimeParser* fParser = getCurrentParser();

    if (fParser)
    {
        SYSTEMTIME fST = getCtrlTime();
		auto fMicrosecondPart = static_cast<WORD>(getDlgItemInt(IDC_TXT_MICROSECONDS_PART));
        fParser->setSystemTime(fST, fMicrosecondPart);
        int64_t fTime = fParser->calculateTime();

        std::vector<std::string> fFiles;
        if (bFolder)
        {
            getFolderFiles(fFiles);
        }
        else
        {
            getOpenFiles(&fFiles, NULL);
        }

        CTreeView fTree(mTimeStampResults->getDlgItem(IDC_TREE_FINDRESULTS));

		TCHAR szText[2048];

		tstring fTimeString = toTime(fST, fMicrosecondPart);
        mMutex.lock();
        HTREEITEM fTimeItem = fTree.insertTo(0, const_cast<TCHAR*>(fTimeString.c_str()), SynchronizeTimeResults::TimeEntry);
        mMutex.unlock();

        tstring fTstring1, fTstring2;
        std::string fLineText;
        for (uint32_t i = 0; i < fFiles.size(); ++i)
        {
            TextFile fFile((LPCSTR)fFiles[i].c_str());
            if (fFile.getLines())
            {
                fFile.setLineLimit(mLineLimit);
                {
                    AutoMutex fM(&mMutex);
                    mActiveFile = &fFile;
                    fParser = getCurrentParser();
                    mActiveFile = 0;
                } 
                
                SearchLine fSearch(fFile, fParser);
                size_t fLine = fSearch.searchLine(fTime);
                if (fLine != -1)
                {
                    fFile.getLineText(static_cast<uint32_t>(fLine), fLineText);
                    int64_t fDifference_us = fSearch.getDifference();
                    if (fDifference_us > mMaxSearchDifference_us) continue;
					SYSTEMTIME fDifference = TimeParser::calcSystemTime(fDifference_us);
                    convertToUnicode(fFiles[i], fTstring1);
                    convertToUnicode(fLineText, fTstring2);
                    {
                        AutoMutex fM(&mMutex);
                        HTREEITEM fFileItem = fTree.insertTo(fTimeItem, &fTstring1[0], SynchronizeTimeResults::FilenameEntry);
                        _stprintf(szText, _T("Line %d: %s"), static_cast<int>(fLine + 1), fTstring2.c_str());
                        fTree.insertTo(fFileItem, szText, SynchronizeTimeResults::LineNoEntry);
                        _stprintf(szText, _T("Difference: %02d:%02d:%02d.%06d"), 
                            fDifference.wHour, fDifference.wMinute, fDifference.wSecond, fDifference.wMilliseconds);
                        fTree.insertTo(fFileItem, szText, SynchronizeTimeResults::TimeDifference);
                    }
                }
            }
        }
        fTree.expandItems(TVE_EXPAND, fTimeItem);
		::UpdateWindow(fTree.handle());
    }
    return TRUE;
}

void SynchronizeTimeStamps::getFolderFiles(std::vector<std::string>& aFiles)
{
    std::string fPath;
    std::string fWildcard;
    mSearchInSubFolders = sendDlgItemMsg(IDC_CK_SEARCH_IN_SUBFOLDERS, BM_GETCHECK) == BST_CHECKED;
    getDlgItemText(IDC_EDT_FOLDER, fPath);
    getDlgItemText(IDC_EDT_WILDCARD, fWildcard);
	if (fPath.empty())
	{
		getCurrentPathFileName(fPath);
		int fLastBackslash =  static_cast<int>(fPath.rfind('\\'));
		if (fLastBackslash != -1)
		{
			fPath = fPath.substr(0, fLastBackslash);
		}
	}
    EnumFiles fEnum(fPath, fWildcard, mSearchInSubFolders != 0);
    fEnum.enumFiles(aFiles);
}
#define  _Browse 1

INT_PTR SynchronizeTimeStamps::OnSelectFolder()
{
#ifdef _Browse
    TCHAR fFolder[MAX_PATH];
    GetDlgItemText(_hSelf, IDC_EDT_FOLDER, fFolder, MAX_PATH);
    BROWSEINFO fBI = { 0 };
    fBI.hwndOwner = _hSelf;
    fBI.pszDisplayName = fFolder;
    fBI.lpszTitle = _T("Select a Folder containing Log Files");
    fBI.ulFlags = BIF_STATUSTEXT| BIF_EDITBOX | BIF_NEWDIALOGSTYLE | BIF_NONEWFOLDERBUTTON;
    PIDLIST_ABSOLUTE fPID = SHBrowseForFolder(&fBI);
    if (fPID)
    {
        SHGetPathFromIDList(fPID, fFolder);
        SetDlgItemText(_hSelf, IDC_EDT_FOLDER, fFolder);
        CoTaskMemFree(fPID);
    }
#else
    OPENFILENAMEW ofn;
    TCHAR fileName[MAX_PATH] = _T("");
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = _hSelf;
    ofn.lpstrFilter = _T("All Files (*.*)\0*.*\0");
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;  ofn.lpstrDefExt = _T("");
    if (GetOpenFileName(&ofn))
    {
        std::wstring fileNameStr = fileName;
        size_t fPos = fileNameStr.rfind(_T("\\"));
        if (fPos != std::wstring::npos)
        {
            fileNameStr = fileNameStr.substr(0, fPos);
        }
        SetDlgItemText(_hSelf, IDC_EDT_FOLDER, fileNameStr.c_str());
    }
#endif
	return TRUE;
}


void SynchronizeTimeStamps::openTimeStampResults()
{
	if (mTimeStampResults == NULL)
	{
		mTimeStampResults = new SynchronizeTimeResults();
		mTimeStampResults->init(_hInst, nppData._nppHandle);
	}
	if (!mTimeStampResults->isCreated())
	{
		tTbData	data = { 0 };
		mTimeStampResults->create(&data);

		// define the default docking behaviour
		data.uMask = DWS_DF_CONT_BOTTOM;

		data.pszModuleName = mTimeStampResults->getPluginFileName();
		_tcscat(data.pszName, _T(" Find Results"));

		// the dlgDlg should be the index of funcItem where the current function pointer is
		// in this case is DOCKABLE_DEMO_INDEX
		data.dlgID = DisplayLogProfiler + 1;
		::SendMessage(nppData._nppHandle, NPPM_DMMREGASDCKDLG, 0, (LPARAM)&data);
	}
	mTimeStampResults->display();
	Sleep(100);
}
//
// [%a %b %e %H:%M:%S %Y]
// 
INT_PTR SynchronizeTimeStamps::OnGetTime()
{
	std::string fTextLine;
    std::string fTimeFormatString;
    SYSTEMTIME fST;
    TimeParser* fParser = mTestParser.get();
    LRESULT fLine = getCurrentLine();
    setDlgItemInt(IDC_TXT_LINE, static_cast<UINT>(fLine + 1));
    ScintillaWnd fWnd(getScintillaWindowHandle(), TRUE);
    fWnd.getLineText(static_cast<UINT>(fLine), fTextLine);
    getDlgItemText(IDC_EDT_TIME_FORMAT, fTimeFormatString);

    BOOL fFormatSet = FALSE;
    if (fTimeFormatString.empty())
    {
        fParser = getCurrentParser();
        fFormatSet = TRUE;
    }
    else
    {
        fFormatSet = fParser->setFormatString(fTimeFormatString);
    }
 
    if (fFormatSet && fParser)
    { 
        BOOL fResult = fParser->parseTimeString(fTextLine);
        if (fResult)
        {
			WORD wMicroSecondPart = 0;
            fParser->getSystemTime(fST, wMicroSecondPart);
			if (!fParser->hasDate())
			{
				SYSTEMTIME fDate;
				SendDlgItemMessageA(_hSelf, IDC_TIME, DTM_GETSYSTEMTIME, 0, (LPARAM)&fDate);
				fST.wYear  = fDate.wYear;
				fST.wMonth = fDate.wMonth;
				fST.wDay   = fDate.wDay;
			}
			if (fST.wYear == 0)
			{
				SYSTEMTIME fDate;
				SendDlgItemMessageA(_hSelf, IDC_TIME, DTM_GETSYSTEMTIME, 0, (LPARAM)&fDate);
				fST.wYear = fDate.wYear;
			}

            SendDlgItemMessageA(_hSelf, IDC_TIME, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&fST);
            SendDlgItemMessageA(_hSelf, IDC_DATE, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&fST);
            setDlgItemInt(IDC_TXT_MILLISECONDS, fST.wMilliseconds);
			setDlgItemInt(IDC_TXT_MICROSECONDS_PART, wMicroSecondPart);
		}
        else
        {
			TRACE(Logger::warning, "Could not parse %s", fTextLine.c_str());
            SendDlgItemMessageA(_hSelf, IDC_TIME, DTM_GETSYSTEMTIME, 0, (LPARAM)&fST);
            fST.wHour   = 0;
            fST.wMinute = 0;
            fST.wSecond = 0;
            SendDlgItemMessageA(_hSelf, IDC_TIME, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&fST);
            setDlgItemInt(IDC_TXT_MILLISECONDS, 0);
			setDlgItemInt(IDC_TXT_MICROSECONDS_PART, 0);
		}
    }
    return TRUE;
}

INT_PTR SynchronizeTimeStamps::OnDiffTime()
{
	BOOL fTranslated { false };
	LRESULT fStoredLine   = getDlgItemInt(IDC_TXT_LINE, FALSE, &fTranslated) - 1;
	LRESULT fSelectedLine = getCurrentLine();
	if (fTranslated && 	fStoredLine >= 0 && fSelectedLine != fStoredLine)
	{
		TimeParser* fParser = getCurrentParser();

		if (fParser)
		{
			SYSTEMTIME fST = getCtrlTime();
			fParser->setSystemTime(fST, static_cast<WORD>(getDlgItemInt(IDC_TXT_MICROSECONDS_PART)));
			int64_t fStoredTime = fParser->calculateTime();
			if (OnGetTime())
			{
				fST = getCtrlTime();
				fParser->setSystemTime(fST, static_cast<WORD>(getDlgItemInt(IDC_TXT_MICROSECONDS_PART)));
				int64_t fSelectedTime = fParser->calculateTime();
				fSelectedLine = getCurrentLine();
				if (fSelectedLine != fStoredLine)
				{
					// calculate difference
					int64_t fDifference;
					if (fStoredTime > fSelectedTime)
					{
						fDifference = fStoredTime - fSelectedTime;
					}
					else
					{
						fDifference = fSelectedTime - fStoredTime;
					}

					// convert difference to timestring
					WORD fMicrosecondPart = 0;
					fST = TimeParser::calcSystemTime(fDifference, &fMicrosecondPart);
					tstring fTimeString = _T("Measured difference: ");
					fTimeString += toTime(fST, fMicrosecondPart);

					// get current file
					ScintillaWnd fWnd(getScintillaWindowHandle(), TRUE);
					std::string fPathFileName;
					tstring fTFileString, fTStoredLineString, fTSelectedLineString;
					getCurrentPathFileName(fPathFileName);
					convertToUnicode(fPathFileName, fTFileString);

					std::string fLineText;
					fWnd.getLineText(static_cast<uint32_t>(fStoredLine), fLineText);
					convertToUnicode(fLineText, fTStoredLineString);
					fWnd.getLineText(static_cast<uint32_t>(fSelectedLine), fLineText);
					convertToUnicode(fLineText, fTSelectedLineString);

					CTreeView fTree(mTimeStampResults->getDlgItem(IDC_TREE_FINDRESULTS));
					HTREEITEM fTimeItem;
					{
						TCHAR szText[2048];
						AutoMutex fM(&mMutex);
						fTimeItem = fTree.insertTo(0, const_cast<TCHAR*>(fTimeString.c_str()), SynchronizeTimeResults::TimeEntry);
						HTREEITEM fFileItem = fTree.insertTo(fTimeItem, &fTFileString[0], SynchronizeTimeResults::FilenameEntry);
						_stprintf(szText, _T("Line %d: %s"), static_cast<int>(fStoredLine + 1), fTStoredLineString.c_str());
						fTree.insertTo(fFileItem, szText, SynchronizeTimeResults::LineNoEntry);
						_stprintf(szText, _T("Line %d: %s"), static_cast<int>(fSelectedLine + 1), fTSelectedLineString.c_str());
						fTree.insertTo(fFileItem, szText, SynchronizeTimeResults::LineNoEntry);
					}
					fTree.expandItems(TVE_EXPAND, fTimeItem);
					::UpdateWindow(fTree.handle());
				}
			}
		}
	}
	else
	{
		TRACE(Logger::to_function, "Calculate time difference between lines failed due to selected %d to stored %d line", fSelectedLine, fStoredLine);
	}
	return TRUE;
}

