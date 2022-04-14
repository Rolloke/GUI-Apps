// MainFrm.h : Schnittstelle der Klasse CMainFrame
//
/////////////////////////////////////////////////////////////////////////////
#include "..\CaraToolbar.h"
#include "..\CaraFrameWnd.h"
#include "InfoToolBar.h"

#if !defined(AFX_MAINFRM_H__30FA9E7C_14B6_11D2_9DB9_0000B458D891__INCLUDED_)
#define AFX_MAINFRM_H__30FA9E7C_14B6_11D2_9DB9_0000B458D891__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define NO_OF_MAIN_INDICATORS 2

#define PASTECOLORSCALE       0
#define PASTE3DPICTURE        1
#define PASTEPICTUREINDEX     2
#define PASTEHEADLINE         3

class CCARA3DVView;
class CCARA3DVDoc;

class CMainFrame : public CCaraFrameWnd
{
protected: // Nur aus Serialisierung erzeugen
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attribute
public:

// Operationen
public:
	virtual ~CMainFrame();

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual void ActivateFrame(int nCmdShow = -1);
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual CLabel * PasteSpecial(CPtrList *);
	bool    SetMainPaneInfo();
	void    EnableTBDocking(DWORD);
	void    AttachColorArray(CColorArray*);
	void    OnUpdateFrameContent();
	CCARA3DVView * Get3DVView();

   //	void    UpdateToolbarContext(CCARA3DVDoc*);
   //	void    ShowChildWindows(int);

	LRESULT OnApplyToView(WPARAM, LPARAM);
	LRESULT OnExitSizeMove(WPARAM, LPARAM);
	LRESULT OnEnterSizeMove(WPARAM, LPARAM);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // Eingebundene Elemente der Steuerleiste
	CCaraToolbar  m_wndToolBar;
	CCaraToolbar  m_wndFuncToolBar;
	CCaraToolbar  m_wndFilmToolBar;
	CColorBar     m_wndColorBar;
   bool          m_bFrameContentInvalid;
public:
	void InvalidateFrameContent();
// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewFunctions();
	afx_msg void OnUpdateViewFunctions(CCmdUI* pCmdUI);
	afx_msg void OnViewFilmrun();
	afx_msg void OnUpdateViewFilmrun(CCmdUI* pCmdUI);
	afx_msg void OnViewInfobar();
	afx_msg void OnUpdateViewInfobar(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnSetMessageString(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClose();
	//}}AFX_MSG
	afx_msg LRESULT OnExitMenuLoop(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()

	LPCTSTR GetDocumentHeading();
private:
   static UINT gm_nMainIndicators[NO_OF_MAIN_INDICATORS];
//	static BOOL CALLBACK ShowChildWindowsEnum(HWND hwnd, LPARAM lParam);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // !defined(AFX_MAINFRM_H__30FA9E7C_14B6_11D2_9DB9_0000B458D891__INCLUDED_)
