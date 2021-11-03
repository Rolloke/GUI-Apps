#ifndef CARA_MENU_HEADER_INCLUDED
   #define CARA_MENU_HEADER_INCLUDED

class CCaraToolbar;

class AFX_EXT_CLASS CCaraMenu : public CMenu
{
   struct MenuPointers
   {
      CPtrList m_Menus;
      CPtrList m_MenuBitmaps;
      CPtrList m_ToolBars;
   };
public:

// Implementation
	CCaraMenu();
	virtual ~CCaraMenu();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
   void         AppendODMenuItem(UINT nID, const TCHAR* pszText, CBitmap *pBit);
	void         AddString(const TCHAR*);
   void         DeleteStrings();
	int          GetStringCount(){return m_Strings.GetCount();};
   void         ResetTabWidth() {m_nTabItemWidth =0;};
	void         AddToolBar(CCaraToolbar*);
	void         SetMenuBitmaps(CMenu *);
	CPtrList   * GetToolBars();

	static UINT  ContextMenu(UINT, CPtrList*, UINT, int, int, CWnd*);
   static void  SetMenuBitmaps(CPtrList*, CPtrList*, CPtrList*, CMenu *, CSize);           // Version 3
   static void  SysColorChange();
   static CSize gm_szBmp;

   static int      gm_nOwnerdraw;
private:
	bool            AllocMenuPointers();
	static void     HollowButton(HDC, RECT&, UINT);
	static void     DrawTransparentBmp(HDC, HBITMAP, int, int, int, int, bool);
   static void     DrawPatternedBkGnd(HDC, RECT &, bool);

   static COLORREF gm_cHiLight;
   static COLORREF gm_cHiLightText;
   static COLORREF gm_cMenu;
   static COLORREF gm_cMenuText;
   static COLORREF gm_cBtnHiLight;
   static COLORREF gm_cBtnShadow;
   static COLORREF gm_cGrayedText;
   static LOGFONT  gm_lfMenuFont;
   static HBITMAP  gm_hbmpBullet;

   int      m_nTabItemWidth;
   CStringArray m_Strings;
   MenuPointers *m_pMenuPt;
};

#endif // CARA_MENU_HEADER_INCLUDED
/////////////////////////////////////////////////////////////////////////////
