#ifndef AFX_CARATOOLBAR_H__94973B62_F60A_11D1_9DB9_0000B458D891__INCLUDED_
#define AFX_CARATOOLBAR_H__94973B62_F60A_11D1_9DB9_0000B458D891__INCLUDED_

// CaraToolbar.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Fenster CCaraToolbar 
#define ID_TB_RIGHT_EXT 2000
#define TBSTYLE_TIMER   0x40

#define  TOOLBAR_REPEAT_ALL       0x00000001  // Alle Tasten mit Timer wiederholen
#define  TOOLBAR_REPEAT_SOME      0x00000002  // einige Tasten mit Timer wiederholen

#define  TOOLBAR_USERCMD_FLOATING 0x00000100  // Toolbar wurde durch Userbefehl floating
#define  TOOLBAR_USERCMD_VISIBLE  0x00000200  // Toolbar wurde durch Userbefehl sichtbar

#define  TOOLBAR_HTGRIPPER        0x00001000  // Hit Gripper
#define  TOOLBAR_LBUTTONDOWN      0x00002000  // MouseButtonDown

class AFX_EXT_CLASS CCaraToolbar : public CToolBar
{
// Konstruktion
public:
	CCaraToolbar();

// Attribute
public:

// Operationen
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CCaraToolbar)
	//}}AFX_VIRTUAL

// Implementierung
public:
	static void EnableUserCmds(bool );
	virtual ~CCaraToolbar();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

   CSize    GetImageSize() {return m_sizeImage;};
	CSize    GetButtonSize(){return m_sizeButton;};
	void     SetRepeatTime(int nTime);
	CBitmap *GetButtonBitmap(int nID, CSize szBmp);
	UINT     IdFromPoint(CPoint, bool);
	void     ModifyFlags(UINT, UINT);
	bool     IsButtonPressed(UINT);
	void     UnpressOld();
	bool     SetButtonTimer(UINT, bool);
	bool     IsVisiblebyUserCmd();
	bool     IsFloatbyUserCmd();
	bool     IsFloating();

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CCaraToolbar)
	afx_msg LRESULT OnNcHitTest(CPoint);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	int m_nMessageTime;
	int m_nTimerCount;
	TBBUTTON m_tbbPressed;
   UINT m_nFlags;
   static bool gm_bUserCmd;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_CARATOOLBAR_H__94973B62_F60A_11D1_9DB9_0000B458D891__INCLUDED_
