// CARA2DVView.h : Schnittstelle der Klasse CCARA2DVView
//
/////////////////////////////////////////////////////////////////////////////
#include "CARA2DV.h"
#include "CARA2DVDoc.h"
#include "AverageDlg.h"
#include "SplineDlg.h"
#include "Derive.h"
#include "FftDlg.h"
#include "Integrate.h"
#include "CreateCurve.h"
#include "FitCurve.h"
#include "MarkerDlg.h"
#include "..\CaraView.h"
#include "..\RectLabel.h"
#include "..\LineLabel.h"
#include "..\PlotLabel.h"
#include "..\CurveLabel.h"	// Hinzugefügt von ClassView
#include "..\CaraViewProperties.h"


#if !defined(AFX_CARA2DVVIEW_H__30FA9EAD_14B6_11D2_9DB9_0000B458D891__INCLUDED_)
#define AFX_CARA2DVVIEW_H__30FA9EAD_14B6_11D2_9DB9_0000B458D891__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#define MK_MOUSELEAVE          0x8000
#define MK_DONTDROPLABEL       0x4000
#define MK_LINEMODE            0x2000
#define MK_NUMERICMODE         0x1000
#define MK_NO_LINE_EDIT        0x0800

#define ARROWCURSOR              0
#define ZOOMCURSOR               1
#define TRACKCURSOR              2
#define COPYCURSOR               3
#define OPENHANDCURSOR           4
#define SIZENSCURSOR             5
#define SIZEALLCURSOR            6
#define PENCURSOR                7
#define NODROPCURSOR             8
#define CROSSCURSOR              9
#define COPYCURVE               10
#define TRACKCURVE              11
#define NODROPCURVE             12
#define COPYPLOT                13
#define TRACKPLOT               14
#define NODROPPLOT              15
#define SIZEWECURSOR            16

class CListDlg;

class CCARA2DVView : public CCaraView
{
protected: // Nur aus Serialisierung erzeugen
	CCARA2DVView();
	DECLARE_DYNCREATE(CCARA2DVView)
// Attribute
public:
	CCARA2DVDoc* GetDocument();

// Operationen
public:

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CCARA2DVView)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void OnDraw(CDC* pDC);  // überladen zum Zeichnen dieser Ansicht
	virtual void OnInitialUpdate();
	protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView);
	virtual int OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	//}}AFX_VIRTUAL
// Implementierung
public:
   virtual ~CCARA2DVView();
   // Zugriff auf markierte Labels
   CPlotLabel *GetPickedPlot() {return m_pActualPlot;};
   int         GetPickedPlotIndex();
   CCurveLabel *Get1stPickedCurve();
   CCurveLabel *Get2ndPickedCurve();
   // Maus- und Cursorfunktionen
   void        SetMouseCapture();
   void        ReleaseMouseCapture();
   CPlotLabel *CheckCursorPosition(CPoint*, CDC *);
	int         CheckWindow(CPoint, HWND&, bool);
	void        CheckTilePlots(CPoint*, CDC *pDC=NULL);
   void        ChangeCursor(UINT);
   void        MousePosToStatusbar(CDC *, CPoint *, bool, LPDPOINT pdp=NULL);
   void        ResetPickedLabels();
   void        HitPlot(CPoint *, CDC *);
	CPlotLabel* Hit2ndPlot(CPlotLabel*, CPoint*);
   void        SetStepValue(bool bRound = false);
   // Updatefunktionen
   void        SetPlotSizes(CRect, CDC*);
   void        InvalidateLabelRgn(CLabel *, CDC *pDC=NULL);
   void        UpdateLabelRgn(CRgn &, CDC *, bool bErase=false);
   bool        GetPropertySheet();
   void        SetStatusPaneText(int, char*);
	void        ResizePlotsToView();

private:
   void        OnLButtonDownEdit(UINT, CPoint);
   void        OnMouseMoveEdit(UINT, CPoint);
   void        OnLButtonUpEdit(UINT, CPoint);

   void        OnLButtonDownZoom(UINT, CPoint);
   void        OnMouseMoveZoom(UINT, CPoint);
   void        OnLButtonUpZoom(UINT, CPoint);

   void        OnLButtonDownPick(UINT, CPoint);
   void        OnMouseMovePick(UINT, CPoint);
   void        OnLButtonUpPick(UINT, CPoint);

   void        OnArrowDown(UINT, UINT);
   void        OnArrowUp(UINT, UINT);
   void        OnArrowLeft(UINT, UINT);
   void        OnArrowRight(UINT, UINT);
   void        OnESC(UINT, UINT);
   void        OnTAB(UINT, UINT);
   void        OnEnter(UINT, UINT);

	void        TestCurveEditMode();
	void        ResetPickStatesQuiet();

   void        InitMouseMode();

   void        DestroyRenderThread();
	bool        InvalidateView(bool);

private:
   static int SetStandardOptions(CLabel *, void *);
   static int UpdateChanges(     CLabel *, void *);
   static int SetPlotRect(       CLabel *, void *);
	static int DrawLabel(CLabel*, void*);
	static int ShowOrHideCurves(CListDlg*, int);
   static unsigned __stdcall RenderThread(void *);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CCARA2DVView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnFilePrint();
	afx_msg void OnDefaultrange();
	afx_msg void OnRezoom();
	afx_msg void OnZoom();
	afx_msg void OnReferenceSelect();
	afx_msg void OnZeroReference();
	afx_msg void OnOptionsPlot();
	afx_msg void OnOptionsStandard();
	afx_msg void OnOptionsSetstandards();
	afx_msg void OnOperationAverage();
	afx_msg void OnOperationSpline();
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();
	afx_msg void OnEditDelete();
	afx_msg void OnDestroyrangelist();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnNumeditmode();
	afx_msg void OnLineeditmode();
	afx_msg void OnMinus();
	afx_msg void OnPlus();
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	afx_msg LRESULT OnHelpHitTest(WPARAM, LPARAM);
	afx_msg void OnFuncDeleterange();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnOperationDerive();
	afx_msg void OnOperationFFT();
	afx_msg void OnOperationIntegrate();
	afx_msg void OnOperationCreatecurve();
	afx_msg void OnOperationFitcurve();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnAcDlg();
	afx_msg void OnViewSetSize(UINT);
   afx_msg void ChangeMouseMode(UINT);
   afx_msg void OnUpdateMouseMode(CCmdUI* pCmdUI);
   afx_msg void OnSelectZoom();
   afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
   afx_msg LRESULT OnResetPickStates(WPARAM, LPARAM);
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnSetPlotMarker();
   afx_msg void OnUpdateSetPlotMarker(CCmdUI *pCmdUI);
   afx_msg void OnFuncPanZoom(UINT);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


private:
      CPlotLabel          *m_pActualPlot;       // Zeiger auf einen gepickten Plot
      CCaraViewProperties *m_pPropSheet;        // Zeiger auf ein Propertysheet Object
      UINT                 m_nMouseMode;        // Modi der Maus (IDC_EDITMODE, IDC_ZOOMMODE, IDC_PICKMODE)
      UINT                 m_nMouseFlags;       // Zwischenspeicher für die Mausflags
      HCURSOR              m_hActCursor;        // Aktueller Cursor für die Funktion Setcursor
      CRgn                 m_UpdateRgn;         // Updateregion für die alte Position eines Labels
      CPoint               m_OldMousePoint;     // alte Mauskoordinaten zur Bildung eines Differenzvektors in DeviceKoordinaten
      CLabelContainer      m_PickList;          // Liste für die gepickte CurveLabels
      int                  m_nArrowCursorStep;  // Cursorschrittweite für Tastatursteuerung
      double               m_dStepValue;        // Schritweite in Weltkoordinaten (double)
      bool                 m_bKeyControl;       // Tastatursteuerung 
//      int                  m_nMouseHover;       // Zähler für die Steuerung von Tooltops für die Kurven
      POINTS               m_ptViewSize;        // Viewvergrößerung in %
      long                 m_lTile;
      HBITMAP              m_hBkGndBmp;
      CAverageDlg          m_AvgDlg;            // Dialog für Mittelwertbildung
      CSplineDlg           m_SplineDlg;         // Dialog für Splineberechnung
      CDerive              m_DrvDlg;            // Dialog für Ableitung
      CIntegrate           m_IntDlg;            // Dialog für Integration
      CCreateCurve         m_CreateCurveDlg;    // Dialog für die Erzeugung einer Kurve aus einer Funktion
      CFitCurve            m_FitCurveDlg;       // Dialog für Kurvenregression
      CMarkerDlg           m_MarkerDlg;
      CFFTDlg              m_FFTDlg;
      CLabel              *m_pHilightedLabel;
      volatile DWORD       m_nRenderFlag;
   	HANDLE               m_hRenderThread;
      HBITMAP              m_hRenderBmp[2];
      HDC                  m_hRenderDC[2];
      HANDLE               m_hRenderEvent;
      CRITICAL_SECTION     m_csRenderDC;
public:

#ifdef ETS_OLE_SERVER
   bool m_bIPframeRecalcLayOut;
#endif
   afx_msg void OnCalcAverage();
   afx_msg void OnUpdateCalcAverage(CCmdUI *pCmdUI);
   afx_msg void OnCalcIntegral();
   afx_msg void OnUpdateCalcIntegral(CCmdUI *pCmdUI);
   afx_msg void OnCalcSlope();
   afx_msg void OnUpdateCalcSlope(CCmdUI *pCmdUI);
   afx_msg void OnCalcZero();
   afx_msg void OnUpdateCalcZero(CCmdUI *pCmdUI);
};

#ifndef _DEBUG  // Testversion in CARA2DVView.cpp
inline CCARA2DVDoc* CCARA2DVView::GetDocument()
   { return (CCARA2DVDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // !defined(AFX_CARA2DVVIEW_H__30FA9EAD_14B6_11D2_9DB9_0000B458D891__INCLUDED_)
