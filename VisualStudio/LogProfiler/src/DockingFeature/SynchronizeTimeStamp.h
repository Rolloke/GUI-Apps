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

#ifndef SYNCHRONIZETIMESTAMPS_DLG_H
#define SYNCHRONIZETIMESTAMPS_DLG_H

#include "DockingDlgInterface.h"
#include "resource.h"
#include <string>
#include <vector>
#include <memory>

#include "..\TimeParser.h"
#include "..\Mutex.h"

typedef std::map<std::string, std::shared_ptr<TimeParser>> ParserToFileMap;
typedef std::map<std::string, std::shared_ptr<TimeParser>> ParserToFormatMap;
typedef std::map<DWORD, HANDLE> ThreadHandleMap;

class TextFile;
class SynchronizeTimeResults;

class SynchronizeTimeStamps : public DockingDlgInterface
{
public :
    SynchronizeTimeStamps();
    ~SynchronizeTimeStamps();

    virtual void display(bool toShow = true) const {
        DockingDlgInterface::display(toShow);
        if (toShow)
            ::SetFocus(::GetDlgItem(_hSelf, IDC_BTN_GET_TIME));
    };

	void setParent(HWND parent2set){
		_hParent = parent2set;
	};

	void openTimeStampResults();

protected :
	virtual INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam);

private :

    INT_PTR OnInitDialog();
    INT_PTR OnGetTime();
    INT_PTR OnFindTime();
    INT_PTR OnInsertFormat();
    INT_PTR OnDeleteFormat();
    INT_PTR OnCkSelectLine();
    INT_PTR OnCkBookmarkLine();
    INT_PTR OnBtnNextBookmark();
    INT_PTR OnBtnPreviousBookmark();
    INT_PTR OnBtnDeleteBookmarks();
    INT_PTR OnBtnDeleteCurrentBookmark();
    INT_PTR OnBtnHelp();
	INT_PTR OnSelectFolder();
    INT_PTR OnStopAllThreads();
    INT_PTR OnSyncronizeFolder();
    INT_PTR OnSyncronizeOpenFiles();
    INT_PTR OnSynchronize(BOOL bFolder = FALSE);

    INT_PTR      StartThread(UINT nID);
    DWORD WINAPI ThreadProc();
    static DWORD WINAPI ThreadProc(LPVOID lpParameter);


    INT_PTR OnWmNotify(UINT aID, NMHDR* pNmHdr);
    INT_PTR OnWmDestroy();
    INT_PTR OnWmHelp(HELPINFO* aHelp);
    INT_PTR OnRemoveThread(UINT aID);


    int  getCurrentLine() const;
    int  findLineOfTimeValue(int64_t aTimeValue);
    TimeParser* getCurrentParser();
    TimeParser* findDocParser();
    void        removeDocParser(TimeParser* aParser);
    SYSTEMTIME  getCtrlTime();
    void        getActivePathFileName(std::string& aFileName);
    void        getFolderFiles(std::vector<std::string>& aFiles);

    ParserToFormatMap mParserToFormatMap;
    ParserToFileMap   mParserToFileMap;
    ThreadHandleMap   mThreads;
    UINT              mThreadFunctionID;
    std::shared_ptr<TimeParser> mTestParser;
    BOOL              mSelectFoundLine;
    BOOL              mBookmarkFoundLine;
    int               mMarker;
    BOOL              mUseThreads;
    int               mLineLimit;
    BOOL              mSearchInSubFolders;
    int64_t           mMaxSearchDifference_ms;

    TextFile*                   mActiveFile;
    ParserToFormatMap::iterator mLastFoundParser;
    SynchronizeTimeResults*     mTimeStampResults;
    Mutex                       mMutex;
};


#endif //SYNCHRONIZETIMESTAMPS_DLG_H
