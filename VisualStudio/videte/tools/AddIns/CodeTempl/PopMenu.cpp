/////////////////////////////////////////////////////////////////////////////
//
//                           Public Domain.
//
/////////////////////////////////////////////////////////////////////////////
//
//
//  File     :   PopMenu.cpp
//
//  Author   :   Darren Richards (darrenr@gissing-consulting.co.uk)
//
//  Date     :   20th March 98
//
//  Synopsis :   Simple modal popup menu wrapper.
//
////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  includes
//
///////////////

#include "stdafx.h"

#include "resource.h"
#include "PopMenu.h"

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//
//  CPopupMenu implementation
//
///////////////

CPopupMenu::CPopupMenu(POINT pt)
{
    m_pt = pt;
    m_hMenu = CreatePopupMenu();
}

CPopupMenu::~CPopupMenu()
{
    DestroyMenu(m_hMenu);
}

void CPopupMenu::AddItem(int nId, LPCSTR pszItem)
{
    MENUITEMINFO mi;

    ZeroMemory(&mi,sizeof(mi));
    mi.cbSize = sizeof(mi);

    mi.fMask        = MIIM_TYPE|MIIM_STATE|MIIM_ID;
    mi.fType        = MFT_STRING;
    mi.fState       = 0;
    mi.wID          = nId;

	string sName(pszItem);
	string sNameToUse(sName);	// create a local copy, to extract otional -->>

	// HEDU (hedu@softhome.net) 0698:
	// enhancement rules a "-->>" is used to mark a submenu
	// a trailing "-->>" creates an empty submenu entry 
	// Then one can Use #{Menu1-->>SubTopic1
	// NOTE: there is only one level of submenues
	// I guess the menu would look to complicated anyway, but it would not be to
	// difficult to search through the whole menu tree.

/* sample:
#{first-->>
#}
#{first-->>f1
aaaaaaaa
#}
#{first-->>f2
bbbbbbbb
#}
#{second-->>
#}
#{second-->>s1
11111111111111
#}
#{second-->>s2
22222222222222
#}

*/


	// three different cases
	// a) trailing "-->>" is a submenu entry
	// b) containing "-->>" insert in the approriate submenu (already existing!)
	// c) plain menu item

	HMENU hMenuToInsertIn = m_hMenu;	// default action is the main main menu

	string::size_type ix = sName.find("-->>");

	// sample: "Core-->>"
	//          012345678 length is 8
	if (ix == (sName.length()-strlen("-->>"))) {	// trailing "-->>" ?
		HMENU hSubMenu = CreatePopupMenu();
		mi.hSubMenu = hSubMenu;   
		mi.fMask |= MIIM_SUBMENU;
		sNameToUse.replace(ix,4,"");	// drop the -->>
	} else if (ix != -1) {	// contains "-->>" but not at the end (that is already handled in case a))
		// b) so this one has to be added in a submenu
		// pre: submenu already created

		// search through all submenu items
		// seperators are recognzized by zero length text
		char szText[500];
		int iCount = GetMenuItemCount( m_hMenu );
		for (int i=0;i<iCount && hMenuToInsertIn==m_hMenu;i++) {
			int iLen = GetMenuString( m_hMenu, i, szText,sizeof(szText)-1,MF_BYPOSITION);

			// OOPS the string compare might need some UNICODE variant
			if (iLen && sName.find(szText,0,strlen(szText))==0) {
				MENUITEMINFO menuInfoRead;
				ZeroMemory(&menuInfoRead,sizeof(menuInfoRead));
				menuInfoRead.cbSize = sizeof(menuInfoRead);
				menuInfoRead.fMask = MIIM_ID| MIIM_SUBMENU;

				BOOL bGetOkay = GetMenuItemInfo( m_hMenu, i, TRUE, &menuInfoRead);
				if (bGetOkay) {
					hMenuToInsertIn = menuInfoRead.hSubMenu;    // use the submenu fo the insert
					sNameToUse.replace(0,strlen(szText)+4,"");	// drop leading SubMenu-->>
				}
			}
  		}
 
	} else {
		// c) plain item, no need fo special action
	}

	// OOPS is the string data still valid after the scope is left
	mi.dwTypeData   = (char *)sNameToUse.c_str();	// sigh, unconst cast
    InsertMenuItem(hMenuToInsertIn, nId, false, &mi); 
}

void CPopupMenu::AddSeparator()
{
    MENUITEMINFO mi;

    ZeroMemory(&mi,sizeof(mi));
    mi.cbSize = sizeof(mi);

    mi.fMask = MIIM_TYPE|MIIM_ID;
    mi.fType = MFT_SEPARATOR;
    mi.wID   = -1;

    InsertMenuItem(m_hMenu, -1, false, &mi); 
}

int CPopupMenu::TrackPopup()
{
    return TrackPopupMenu(m_hMenu,
                          TPM_RETURNCMD|TPM_LEFTALIGN|TPM_LEFTBUTTON,
                          m_pt.x,
                          m_pt.y,
                          0,
                          GetFocus(),
                          NULL);
}

