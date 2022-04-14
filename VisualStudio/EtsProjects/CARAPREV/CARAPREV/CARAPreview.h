#ifndef AFX_CARAPREVIEW_H__30FA9EDE_14B6_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_CARAPREVIEW_H__30FA9EDE_14B6_11D2_9DB9_0000B458D891__INCLUDED_

// CARAPreview.h : Header-Datei
//
#include "afxpriv.h"
#include "CaraViewProperties.h"	// Hinzugefügt von ClassView
#include "CaraToolbar.h"	      // Hinzugefügt von ClassView
#include "LabelContainer.h"	   

/////////////////////////////////////////////////////////////////////////////
// Ansicht CCARAPreview 
class CCaraWinApp;
class CLabel;
class CCaraMenu;

class AFX_EXT_CLASS CCARAPreview : public CPreviewView
{
private:
   struct ToolbarStates
   {
      unsigned char tb1CBRS      :8;
      unsigned char tb2CBRS      :8;
      unsigned char tb3CBRS      :8;
      unsigned char tb1visible   :1;
      unsigned char tb2visible   :1;
      unsigned char tb3visible   :1;
      unsigned char menuvisible  :1;
      unsigned char dummy        :4;
   };
   struct LabelStates
   {
      LOGFONT  TextLF;        // Schriftart
      COLORREF TextColor;     // Textfarbe
      COLORREF TextBkColor;   // Hintergrundfarbe Text
      UINT     TextAlign;     // Ausrichtung
      LOGPEN   TextLP;        // Textrahmen Stift
      WORD     TextFrame;     // Textrahmen
      WORD     TextBkGnd;     // Texthintergrund
      LOGPEN   LineLP;        // Linien Stift
      LOGPEN   RectLP;        // Rechteck Stift
      LOGBRUSH RectBrush;     // Rechteck Pinsel
      COLORREF RectBkColor;   // Rechteck Hintergrundfarbe
      LOGPEN   BezierLP;      // Bezier Stift
      LOGPEN   CircleLP;      // Kreis Stift
      LOGBRUSH CircleBrush;   // Kreis Pinsel
      COLORREF CircleBkColor; // Kreis Hintergrundfarbe
      COLORREF ShapePenColor; // Pickerfarbe
   };

   struct GridStates
   {
      bool   bCatchGrid;
      CPoint ptCatchGridOrg;
      CSize  szCatchGrid;
   };

protected:
	CCARAPreview();           // Dynamische Erstellung verwendet geschützten Konstruktor
	DECLARE_DYNCREATE(CCARAPreview)

// public Funktionen
public:
	CCaraMenu* GetMainMenu() const;
	HDC GetAttribDC();
	CLabelContainer *GetDocumentLabels();
	HENHMETAFILE     GetPreviewMetaFile();
   CDC             *GetPreviewDC();
   void             ReleasePreviewDC();
	bool             AllocMainMenu();
	void             OnDeactivateView();

// private Funktionen
private:
   void ChangeCursor(UINT);
   void SetMouseMode(UINT);
   void UpdateLabelRgn(CRgn &rgn, CDC*pDC);
   void CatchGrid(CPoint &ptCatch);

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CCARAPreview)
	public:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual BOOL DestroyWindow();
	protected:
	virtual void OnDraw(CDC* pDC);      // Überschrieben zum Zeichnen dieser Ansicht
	virtual void OnInitialUpdate();     // Zum ersten Mal nach der Konstruktion
   virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementierung
protected:
   virtual ~CCARAPreview();
	virtual CSize CalcScaleRatio(CSize windowSize, CSize actualSize);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CCARAPreview)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateFunction(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFuncGroup(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFuncUngroup(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTB(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTBTop(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTBBottom(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTB1(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTB1Left(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTB1Top(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTB1Right(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTB1Bottom(CCmdUI* pCmdUI);
	afx_msg void OnViewTB();
	afx_msg void OnViewTBTop();
	afx_msg void OnViewTBBottom();
	afx_msg void OnViewTB1();
	afx_msg void OnViewTB1Left();
	afx_msg void OnViewTB1Right();
	afx_msg void OnViewTB1Top();
	afx_msg void OnViewTB1Bottom();
	afx_msg void OnFuncPick();
	afx_msg void OnFuncCircle();
	afx_msg void OnFuncLine();
	afx_msg void OnFuncRectangle();
	afx_msg void OnFuncText();
	afx_msg void OnFuncBezier();
	afx_msg void OnOptionsCircle();
	afx_msg void OnOptionsLine();
	afx_msg void OnOptionsRectangle();
	afx_msg void OnOptionsText();
	afx_msg void OnOptionsBezier();
	afx_msg void OnOptionsGrid();
	afx_msg void OnFuncDown();
	afx_msg void OnFuncGroup();
	afx_msg void OnFuncHighest();
	afx_msg void OnFuncLowest();
	afx_msg void OnFuncUngroup();
	afx_msg void OnFuncUp();
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditDelete();
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditDelete(CCmdUI* pCmdUI);
	afx_msg void OnEditPasteSelectiv();
	afx_msg void OnFuncCatchGrid();
	afx_msg void OnUpdateFuncCatchGrid(CCmdUI* pCmdUI);
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	afx_msg void OnUpdateFuncUp(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFuncDown(CCmdUI* pCmdUI);
	afx_msg void OnOptionsPick();
	afx_msg void OnEditPasteSpecial();
	afx_msg void OnNoLogo();
	afx_msg void OnDestroy();
   afx_msg void OnPreviewClose();
	afx_msg void OnColormatching();
	afx_msg void OnFuncBeziercircle();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
   afx_msg void OnUpdateEditPasteSelective(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditPasteSpecial(CCmdUI* pCmdUI);
	afx_msg void OnUpdateColorMatching(CCmdUI* pCmdUI);
	afx_msg void OnContextMenuAcc();
	afx_msg void OnViewMenu();
	afx_msg void OnUpdateViewMenu(CCmdUI* pCmdUI);
	afx_msg void OnAddPage();
	afx_msg void OnRemovePage();
	//}}AFX_MSG
	afx_msg LRESULT OnHelpHitTest(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()

private:
   void        OnLButtonDownPick(UINT, CPoint);
   void        OnLButtonUpPick(UINT, CPoint);

   void        OnLButtonDownNew(UINT, CPoint);
   void        OnLButtonUpNew(UINT, CPoint);

private:
	void PasteLabel(CCaraWinApp *, CLabelContainer *, CLabel *);
   static int SetChanged(CLabel*pl, void*);

   CCaraToolbar     m_PreviewToolbar;
   UINT             m_nMouseMode;
   CLabelContainer  m_PickList;
   CRgn             m_PickUpdateRgn;
   CPoint           m_OldMousePoint;
   HCURSOR          m_hActCursor;
   bool             m_bSizeChanged;
   int              m_nSelect;
   int              m_nPreviewDCInit;
   CDC             *m_pDC;
   bool             m_bCatchGrid;
   bool             m_bCopyLabel;
   CPoint           m_ptCatchGridOrg;
   CSize            m_szCatchGrid;
   ToolbarStates    m_nBarState;
   CCaraMenu       *m_pMainMenu;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_CARAPREVIEW_H__30FA9EDE_14B6_11D2_9DB9_0000B458D891__INCLUDED_
