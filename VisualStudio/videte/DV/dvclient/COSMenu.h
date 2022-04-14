// COSMenu.h: interface for the COSMenu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COSMENU_H__FA781BA6_C4E6_11D3_8E13_004005A11E32__INCLUDED_)
#define AFX_COSMENU_H__FA781BA6_C4E6_11D3_8E13_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_SUBMENU 128

class COSMenu  
{
public:
	COSMenu();
	virtual ~COSMenu();

	BOOL Open(int nResourceID);
								  
	BOOL IncMenuPos();
	BOOL DecMenuPos();

	BOOL IncSubMenuPos();
	BOOL DecSubMenuPos();

	BOOL IncSubMenuPos(int nResourceID);
	BOOL DecSubMenuPos(int nResourceID);

	void SetMenuPos(int nResourceID);
	void SetSubMenuPos(int nResourceID, int nSubMenuPos);

	int FindSubMenuPos(int nSubMenuID);
	int FindMenuPos(int nSubMenuID);

	int GetMenuPos();
	int GetSubMenuPos();
	int GetSubMenuPos(int nResourceID);

	int GetMenuID(int nMenuPos = -1, int nSubMenuPos = -1);

	CString GetTitleString();
	CString GetSubString();
	CString GetSubString(int nSubMenuID);

	BOOL SetString(int nResourceID, const CString& sText);
	BOOL SetSubString(const CString& sText, int nSubMenuID);

	void RemoveMenu(int nSubMenuID);
	void RemoveMenuTitle(int nSubMenuID);

	void EnableMenuItem(int nSubMenuID, UINT nState);
	void EnableMenuItemTitle(int nSubMenuID, UINT nState);
	void EnableAll(int nState = MF_ENABLED);

	BOOL CheckMenuRadioItem(UINT nFirstID, UINT nLastID, UINT nSubMenuID);
	UINT GetSubMenuState();
	UINT GetMenuState();

private:
	int			m_nResourceID;
	CMenu		m_Menu;
	int			m_nSubMenuPos[MAX_SUBMENU]; 
	int			m_nMenuPos;
};

#endif // !defined(AFX_COSMENU_H__FA781BA6_C4E6_11D3_8E13_004005A11E32__INCLUDED_)
