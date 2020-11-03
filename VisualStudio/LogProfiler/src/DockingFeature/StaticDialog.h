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

#pragma once

#include "Window.h"
#include "..\Notepad_plus_msgs.h"
#include <vector>
#include <string>

enum class PosAlign { left, right, top, bottom };

#define lengthof(X) (sizeof(X) / sizeof(*X))

struct DLGTEMPLATEEX {
      WORD   dlgVer;
      WORD   signature;
      DWORD  helpID;
      DWORD  exStyle;
      DWORD  style; 
      WORD   cDlgItems;
      short  x;
      short  y;    
      short  cx;
      short  cy;
      // The structure has more fields but are variable length
} ;

class StaticDialog : public Window
{
public :
    StaticDialog();
    virtual ~StaticDialog();

    virtual void create(int dialogID, bool isRTL = false);
	INT_PTR doModal(int dialogID);

    virtual bool isCreated() const
    {
		return (_hSelf != NULL);
	};

	void goToCenter();
    void destroy();
    HWND getDlgItem(int aID);

protected :
    void getDlgItemText(int aID, std::string& aText);
    void getDlgItemText(int aID, std::wstring& aText);
    BOOL setDlgItemInt(int aID, UINT aValue, BOOL aSigned=FALSE);
    UINT getDlgItemInt(int aID, BOOL aSigned=FALSE, BOOL* aTranslated=0);
    INT_PTR sendDlgItemMsg(int aID, UINT aMsg, WPARAM wParam=0, LPARAM lParam=0);
    void enableToolTip();
    BOOL addToolTip(UINT aID, TCHAR* aText=0);

    RECT _rc;
	static INT_PTR CALLBACK dlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual INT_PTR CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam) = 0;

    void alignWith(HWND handle, HWND handle2Align, PosAlign pos, POINT & point);
	HGLOBAL makeRTLResource(int dialogID, DLGTEMPLATE **ppMyDlgTemplate);
private:
    HWND mHwndTip;
    std::vector<tstring> mHelpStrings;
};

class CTreeView
{
public:
    /*!
    \brief ensures visiblity of text line in given scintilla window
    \param window handle of treeview
    */
    CTreeView(HWND aWnd);

    /*!
    \brief inserts a treeview item
    \param parent parent tree item handle
    \param itemStr inserted text
    \param imgIndex image index text
    \return inserted treevie item handle
    */
    HTREEITEM insertTo(HTREEITEM parent, TCHAR *itemStr, int imgIndex);
    /*!
    \brief expands or collapses treeview item
    \param aExpand (TVE_COLLAPSE, TVE_COLLAPSERESET, TVE_EXPAND, TVE_EXPANDPARTIAL, TVE_TOGGLE)
    \param parent parent item to be expanded or collapsed
    */
    void      expandItems(DWORD aExpand, HTREEITEM parent);

    /*!
    \brief expands or collapses treeview item
    \param aGet (TVGN_CARET, TVGN_CHILD, TVGN_NEXT, TVGN_PREVIOUS, TVGN_PARENT, TVGN_ROOT,
    TVGN_FIRSTVISIBLE, TVGN_NEXTVISIBLE, TVGN_PREVIOUSVISIBLE, TVGN_LASTVISIBLE
    TVGN_DROPHILITE, TVGN_NEXTSELECTED)
    \param parent parent item to be expanded or collapsed
    \return desired treevie item handle
    */
    HTREEITEM getItem(DWORD aGet = TVGN_CARET, HTREEITEM parent = TVI_ROOT);

    void      getItemText(HTREEITEM parent, tstring& aText, int &aImage);
	void      getItemImage(HTREEITEM parent, int& aImage);
    BOOL      deleteItem(HTREEITEM parent = TVI_ROOT);
	HWND      handle();
private:
    HWND        mHwnd;
};
