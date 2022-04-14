// COSMenu.cpp: implementation of the COSMenu class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dvclient.h"
#include "COSMenu.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
COSMenu::COSMenu()
{
	m_nMenuPos		= 0;
	m_nResourceID	= 0;

	for (WORD wI = 0; wI < MAX_SUBMENU; wI++)
		m_nSubMenuPos[wI] = 0;
}
//////////////////////////////////////////////////////////////////////
COSMenu::~COSMenu()
{
	m_Menu.DestroyMenu();
}
//////////////////////////////////////////////////////////////////////
BOOL COSMenu::Open(int nResourceID)
{
	if (m_Menu.m_hMenu)
	{
		m_Menu.DestroyMenu();
	}

	m_nResourceID = nResourceID;

	return m_Menu.LoadMenu(nResourceID);
}
//////////////////////////////////////////////////////////////////////
int COSMenu::GetMenuID(int nMenuPos/*=-1*/, int nSubMenuPos /*=-1*/)
{
	int iRet = -1;
	CMenu* pSub = NULL;
	
	if (nMenuPos == -1)
		pSub = m_Menu.GetSubMenu(m_nMenuPos);
	else
		pSub = m_Menu.GetSubMenu(nMenuPos);

	if (pSub)
	{
		// befinden uns im Submenu
		if (nSubMenuPos != -1)
		{
			iRet = pSub->GetMenuItemID(nSubMenuPos);
		}
		else
		{
			iRet = pSub->GetMenuItemID(m_nSubMenuPos[m_nMenuPos]);
		}
	}
	else
	{
		// befinden uns direkt im Menu
		iRet = m_Menu.GetMenuItemID(m_nMenuPos);
	}
	return iRet;
}
//////////////////////////////////////////////////////////////////////
BOOL COSMenu::IncMenuPos()
{
	m_nMenuPos++;
	if (m_nMenuPos >= (int)m_Menu.GetMenuItemCount())
		m_nMenuPos = 0;
	
	// Die Submenüposition soll sich nicht gemerkt werden (Wunsch: Alarmcom)
	// Dadurch könnte das m_nSubMenuPos[]-Array durch eine einfache int-Variable
	// ersetzt werden
	m_nSubMenuPos[m_nMenuPos] = 0;
	
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL COSMenu::DecMenuPos()
{
	m_nMenuPos--;
	if (m_nMenuPos < 0)
		m_nMenuPos = m_Menu.GetMenuItemCount()-1;

	// Die Submenüposition soll sich nicht gemerkt werden (Wunsch: Alarmcom)
	// Dadurch könnte das m_nSubMenuPos[]-Array durch eine einfache int-Variable
	// ersetzt werden
	m_nSubMenuPos[m_nMenuPos] = 0;

	return TRUE;	
}
//////////////////////////////////////////////////////////////////////
BOOL COSMenu::IncSubMenuPos()
{
	CMenu* pSub = m_Menu.GetSubMenu(m_nMenuPos);
	if (pSub)
	{
		m_nSubMenuPos[m_nMenuPos]++;
		if (m_nSubMenuPos[m_nMenuPos] >= (int)pSub->GetMenuItemCount())
			m_nSubMenuPos[m_nMenuPos] = 0;
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL COSMenu::DecSubMenuPos()
{							
	CMenu* pSub = m_Menu.GetSubMenu(m_nMenuPos);
	if (pSub)
	{
		m_nSubMenuPos[m_nMenuPos]--;
		if (m_nSubMenuPos[m_nMenuPos] < 0)
			m_nSubMenuPos[m_nMenuPos] = pSub->GetMenuItemCount()-1;
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL COSMenu::IncSubMenuPos(int nResourceID)
{
	int nMenuPos = FindMenuPos(nResourceID);
	if (nMenuPos != -1)
	{
		CMenu* pSub = m_Menu.GetSubMenu(nMenuPos);
		if (pSub)
		{
			m_nSubMenuPos[nMenuPos]++;
			if (m_nSubMenuPos[nMenuPos] >= (int)pSub->GetMenuItemCount())
				m_nSubMenuPos[nMenuPos] = 0;
		}
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL COSMenu::DecSubMenuPos(int nResourceID)
{							
	int nMenuPos = FindMenuPos(nResourceID);
	if (nMenuPos != -1)
	{
		CMenu* pSub = m_Menu.GetSubMenu(nMenuPos);
		if (pSub)
		{
			m_nSubMenuPos[nMenuPos]--;
			if (m_nSubMenuPos[nMenuPos] < 0)
				m_nSubMenuPos[nMenuPos] = pSub->GetMenuItemCount()-1;
		}
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
void COSMenu::SetMenuPos(int nResourceID)
{
	int nMenuPos = FindMenuPos(nResourceID);
	if (nMenuPos != -1)
	{
		m_nMenuPos = nMenuPos;
	
		// Die Submenüposition soll sich nicht gemerkt werden (Wunsch: Alarmcom)
		// Dadurch könnte das m_nSubMenuPos[]-Array durch eine einfache int-Variable
		// ersetzt werden
		m_nSubMenuPos[m_nMenuPos] = 0;
	}
}
//////////////////////////////////////////////////////////////////////
void COSMenu::SetSubMenuPos(int nResourceID, int nSubMenuPos)
{
	int nMenuPos = FindMenuPos(nResourceID);
	if (nMenuPos != -1)
	{
		CMenu* pSub = m_Menu.GetSubMenu(nMenuPos);
		if (pSub)
		{
			if (nSubMenuPos < (int)pSub->GetMenuItemCount()	&& nSubMenuPos >= 0)
			{
				if (!(pSub->GetMenuState(nSubMenuPos, MF_BYPOSITION) & MF_DISABLED))
					m_nSubMenuPos[nMenuPos] = nSubMenuPos;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
CString COSMenu::GetTitleString()
{
	CString sText;
	if (m_Menu.GetMenuString(m_nMenuPos, sText, MF_BYPOSITION) > 0)
		return sText;

	return _T("");
}
//////////////////////////////////////////////////////////////////////
CString COSMenu::GetSubString()
{
	CMenu* pSub = m_Menu.GetSubMenu(m_nMenuPos);
	if (pSub)
	{
		CString sText;
		if (pSub->GetMenuString(m_nSubMenuPos[m_nMenuPos], sText, MF_BYPOSITION) > 0)
			return sText;
	}

	return _T("");
}
//////////////////////////////////////////////////////////////////////
CString COSMenu::GetSubString(int nSubMenuID)
{
	CString sText;

	m_Menu.GetMenuString(nSubMenuID, sText, MF_BYCOMMAND);

	return sText;
}
//////////////////////////////////////////////////////////////////////
BOOL COSMenu::SetString(int nResourceID, const CString& sText)
{
	return m_Menu.ModifyMenu(nResourceID, MF_BYCOMMAND|MF_STRING, nResourceID, sText);
}
//////////////////////////////////////////////////////////////////////
BOOL COSMenu::SetSubString(const CString& sText, int nSubMenuID)
{
	BOOL bResult = FALSE;

	if (m_Menu.ModifyMenu(nSubMenuID, MF_BYCOMMAND|MF_STRING, nSubMenuID, sText) > 0)
		bResult = TRUE;

	return bResult;
}

//////////////////////////////////////////////////////////////////////
int COSMenu::GetMenuPos()
{
	return m_nMenuPos;
}
//////////////////////////////////////////////////////////////////////
int COSMenu::GetSubMenuPos()
{
	return m_nSubMenuPos[m_nMenuPos];
}
//////////////////////////////////////////////////////////////////////
int COSMenu::GetSubMenuPos(int nResourceID)
{
	int iReturn = -1;
	int nMenuPos = FindMenuPos(nResourceID);
	if (nMenuPos != -1)
	{
		iReturn = m_nSubMenuPos[nMenuPos];
	}
	return iReturn;
}
//////////////////////////////////////////////////////////////////////
void COSMenu::RemoveMenu(int nSubMenuID)
{
	int  nFindMenuPos = FindMenuPos(nSubMenuID);

	// Menüeintrag löschen
	m_Menu.RemoveMenu(nSubMenuID, MF_BYCOMMAND);

	// Ist das Menü leer, dann Menü ebenfalls entfernen.
	CMenu* pSub = m_Menu.GetSubMenu(nFindMenuPos);
	if (pSub)
	{
		if (pSub->GetMenuItemCount() == 0)
			m_Menu.RemoveMenu(nFindMenuPos, MF_BYPOSITION);
	}
}
//////////////////////////////////////////////////////////////////////
void COSMenu::RemoveMenuTitle(int nSubMenuID)
{
	int  nFindMenuPos = FindMenuPos(nSubMenuID);

	if (nFindMenuPos != -1)
		m_Menu.RemoveMenu(nFindMenuPos, MF_BYPOSITION);
}
//////////////////////////////////////////////////////////////////////
int COSMenu::FindMenuPos(int nSubMenuID)
{
	// Suche Menüposition, zu dem die SubMenuID gehört
	for (int nMenuPos = 0; nMenuPos < (int)m_Menu.GetMenuItemCount(); nMenuPos++)
	{
		CMenu* pSub = m_Menu.GetSubMenu(nMenuPos);
		if (pSub)
		{
			for (int nSubMenuPos = 0; nSubMenuPos < (int)pSub->GetMenuItemCount(); nSubMenuPos++)
			{
				if ((int)pSub->GetMenuItemID(nSubMenuPos) == nSubMenuID)
					return nMenuPos;			
			}
		}
	}

	return -1;
}
//////////////////////////////////////////////////////////////////////
int COSMenu::FindSubMenuPos(int nSubMenuID)
{
	// Suche Menüposition, zu dem die SubMenuID gehört
	for (int nMenuPos = 0; nMenuPos < (int)m_Menu.GetMenuItemCount(); nMenuPos++)
	{
		CMenu* pSub = m_Menu.GetSubMenu(nMenuPos);
		if (pSub)
		{
			for (int nSubMenuPos = 0; nSubMenuPos < (int)pSub->GetMenuItemCount(); nSubMenuPos++)
			{
				if ((int)pSub->GetMenuItemID(nSubMenuPos) == nSubMenuID)
					return nSubMenuPos;			
			}
		}
	}											 

	return -1;
}
//////////////////////////////////////////////////////////////////////
void COSMenu::EnableMenuItem(int nSubMenuID, UINT nState)
{
	// Menüeintrag sperren/freigeben
	m_Menu.EnableMenuItem(nSubMenuID, nState);
	
	// Wenn alle Submenüeinträge disabled sind den Titel ebenfalls disablen, ansonsten
	// den Title enablen.
	int  nFindMenuPos = FindMenuPos(nSubMenuID);
	if (nFindMenuPos != -1)
	{
		CMenu* pSub = m_Menu.GetSubMenu(nFindMenuPos);
		if (pSub)
		{
			int nCnt = 0;
			for (int nSubMenuPos = 0; nSubMenuPos < (int)pSub->GetMenuItemCount(); nSubMenuPos++)
			{
				if (pSub->GetMenuState(nSubMenuPos, MF_BYPOSITION) & MF_DISABLED)
					nCnt++;		
			}
			if (nCnt == (int)pSub->GetMenuItemCount())
				m_Menu.EnableMenuItem(nFindMenuPos, MF_DISABLED| MF_BYPOSITION);
			else
				m_Menu.EnableMenuItem(nFindMenuPos, MF_ENABLED| MF_BYPOSITION);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void COSMenu::EnableMenuItemTitle(int nSubMenuID, UINT nState)
{
	int  nFindMenuPos = FindMenuPos(nSubMenuID);

	if (nFindMenuPos != -1)
	{
		m_Menu.EnableMenuItem(nFindMenuPos, nState | MF_BYPOSITION);

		// Alle Submenüeinträge ebenfalls enablen/disablen
		CMenu* pSub = m_Menu.GetSubMenu(nFindMenuPos);
		if (pSub)
		{
			for (int nSubMenuPos = 0; nSubMenuPos < (int)pSub->GetMenuItemCount(); nSubMenuPos++)
				pSub->EnableMenuItem(nSubMenuPos, nState | MF_BYPOSITION);
		}
	}
}

//////////////////////////////////////////////////////////////////////
BOOL COSMenu::CheckMenuRadioItem(UINT nFirstID, UINT nLastID, UINT nSubMenuID)
{
	return m_Menu.CheckMenuRadioItem(nFirstID, nLastID, nSubMenuID, MF_BYPOSITION);
}

//////////////////////////////////////////////////////////////////////
UINT COSMenu::GetSubMenuState()
{
	int nState = 0;
	CMenu* pSub = m_Menu.GetSubMenu(m_nMenuPos);
	if (pSub)
		nState = pSub->GetMenuState(m_nSubMenuPos[m_nMenuPos], MF_BYPOSITION);

	return nState;
}

//////////////////////////////////////////////////////////////////////
UINT COSMenu::GetMenuState()
{
	return m_Menu.GetMenuState(m_nMenuPos, MF_BYPOSITION);
}

//////////////////////////////////////////////////////////////////////
void COSMenu::EnableAll(int nState /*= MF_ENABLE */)
{
	// Suche Menüposition, zu dem die SubMenuID gehört
	for (int nMenuPos = 0; nMenuPos < (int)m_Menu.GetMenuItemCount(); nMenuPos++)
	{
		CMenu* pSub = m_Menu.GetSubMenu(nMenuPos);
		if (pSub)
		{
			for (int nSubMenuPos = 0; nSubMenuPos < (int)pSub->GetMenuItemCount(); nSubMenuPos++)
			{
				int nSubMenuID = pSub->GetMenuItemID(nSubMenuPos);
				if (nSubMenuID != -1)
				{
					CString sText = _T("");
					EnableMenuItem(nSubMenuID, nState);
				}
			}
		}
	}
}
