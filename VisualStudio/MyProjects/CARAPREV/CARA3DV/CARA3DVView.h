// CARA3DVView.h : Schnittstelle der Klasse CCARA3DVView
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CARA3DVVIEW_H__30FA9E80_14B6_11D2_9DB9_0000B458D891__INCLUDED_)
#define AFX_CARA3DVVIEW_H__30FA9E80_14B6_11D2_9DB9_0000B458D891__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "..\CaraView.h"
#include "CARA3DVDoc.h"
#include "Cara3DGL.h"	// Hinzugefügt von der Klassenansicht

class CLabel;
class CLightDlg;
class CViewParams;
class CStandardDlg;

class CCARA3DVView : public CCaraView
{

protected: // Nur aus Serialisierung erzeugen
	CCARA3DVView();
	DECLARE_DYNCREATE(CCARA3DVView)

// Attribute
public:
	CCARA3DVDoc* GetDocument();

// Operationen
public:

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CCARA3DVView)
	public:
	virtual void OnDraw(CDC* pDC);  // überladen zum Zeichnen dieser Ansicht
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView);
	//}}AFX_VIRTUAL

// Implementierung
public:
   virtual ~CCARA3DVView();
	void     SetDefaultOptions();
	void     CalculateViewVolume();
	void     UpdateStandardDlg();
	void     UpdateLightDlg();
	void     UpdateViewParamsDlg();
	void     KillTimerSave(UINT&);
   void     SetFilmIndexToStatusPane();
	LRESULT  SendMsgToGLView(UINT, WPARAM, LPARAM);
	LRESULT  OnApplyToView(WPARAM, LPARAM);
	void     InvalidateLists();
	CLabel  *Paste3DLabel(int);
	void     InitDibSection();
	void     WaitOnRenderer();
	void     SwitchPhaseValues()
   {
      OnViewPhasevalues();
   }

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void     SaveReduceEffect();
	void     AdaptLightDistance();

// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CCARA3DVView)
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnViewPhasevalues();
	afx_msg void OnOptionsResetColors();
	//}}AFX_MSG
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
// Menu Command Funktionen
// File
	afx_msg void OnFileSaveAsBitmap();
   afx_msg void OnFilePrint();
   afx_msg void OnFilePrintPreview();
// Edit
	afx_msg void OnEditCopyAsDib();

   // View
	afx_msg void OnViewFull();
	afx_msg void OnViewLight();

   // Options
   afx_msg void OnOptionsView();
	afx_msg void OnOptionsLight();
	afx_msg void OnOptionsStandard();
	afx_msg void OnOptionsLoadColors();
	afx_msg void OnOptionsSaveColors();
	afx_msg void OnOptionsSetDefault();

   // Film ToolBar
	afx_msg void OnFuncFilmstepminus();
   afx_msg void OnFuncFilmstepplus();
   afx_msg void OnFuncFilmTimeStep();
   afx_msg void OnViewOptionAxis();
   afx_msg void OnViewOptionGrid();
   // Function ToolBar
   afx_msg BOOL OnFuncMoveObject(CVector &);
   afx_msg BOOL OnFuncRotateObject(double, double);
   afx_msg BOOL OnFuncZoomObject(CVector &);

DECLARE_MESSAGE_MAP()

private:
   // Standard-Dialoge
   CLightDlg        *m_pLightDlg;      // Lichtparameter
   CViewParams      *m_pViewParams;    // Darstellungsparameter
   CStandardDlg     *m_pStdDlg;        // Standardparameter
   // OpenGL-Parameter
	CEts3DGL          m_GLDummy;        // OpenGL 
	CCara3DGL        *m_pGLView;        // OpenGL Darstellungsobjekt
   // Gitterparameter
   DWORD             m_dwAxes;         // Axen Flags
   // Vollbilddarstellung
   HMENU             m_hMenu;          // Menuhandlespeicher
   WINDOWPLACEMENT   m_WndPlacement;   // Alte Fensterdaten
   // Effect Reduzierung der Auflösung
	UINT              m_nEffectTimer;   // Event Effect Update
   bool              m_bLightOnSave;   // Licht war an
   int               m_nDetailSave;    // Detailierung war größer
   bool              m_bMouseButtonDown;//
   // Filmsteuerung
	UINT              m_nUpdateTimer;   // Event Filmindexausgabe
   int               m_nFilmRun;       // Film Flags
	int               m_nFilmTimeCount; // Zähler
	int               m_nFilmTimeMax;   // Zähler Maximum
   bool              m_bFilmRepeat;    // Autorepeat
   bool              m_bFilmTimer;     // Zeitsteuerung
   // Animation
   int               m_nPictureIndexOld;// Alter BildIndex
   CString           m_PictureTextOld; // Alte Bildbeschreibung
   // Farbdirectory
   CString           m_strDefaultColorDir;

public:
	void InvalidateFrameContent();
	void UpdateMousePos();

#ifdef ETS_OLE_SERVER
   bool m_bIPframeRecalcLayOut;
#endif

};

#ifndef _DEBUG  // Testversion in CARA3DVView.cpp
inline CCARA3DVDoc* CCARA3DVView::GetDocument()
   { return (CCARA3DVDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // !defined(AFX_CARA3DVVIEW_H__30FA9E80_14B6_11D2_9DB9_0000B458D891__INCLUDED_)
