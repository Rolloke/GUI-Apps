// WaveGenView.h : Schnittstelle der Klasse CWaveGenView
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAVEGENVIEW_H__349819BE_B864_11D3_B6EC_0000B458D891__INCLUDED_)
#define AFX_WAVEGENVIEW_H__349819BE_B864_11D3_B6EC_0000B458D891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CaraView.h"
#include "LabelContainer.h"

struct CF_GET_CURVE_PARAMS;

class CWaveGenView : public CCaraView
{
protected: // Nur aus Serialisierung erzeugen
	CWaveGenView();
	DECLARE_DYNCREATE(CWaveGenView)

// Attribute
public:
	CWaveGenDoc* GetDocument();

// Operationen
public:

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CWaveGenView)
	public:
	virtual void OnDraw(CDC* pDC);  // überladen zum Zeichnen dieser Ansicht
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementierung
public:
	void CopyChildItems(HTREEITEM, HTREEITEM);
	void ExpandAllSubItems(HTREEITEM, UINT);
	void DeleteSubItems(HTREEITEM);
	void SetFilterPlot();
	void SetFrqPlot(float *, char*);
	void DetachAttachedPlotArrays();
	void SetTimePlot(float *, int, char*);
	void SetPlotSizes(CRect *prcPlot=NULL);
	virtual ~CWaveGenView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generierte Message-Map-Funktionen
protected:
	static int GetFourierFrqValue(CF_GET_CURVE_PARAMS*);
   static int DetachAttachedPlotArrays(CLabel*, void*);
   static int SetPlotSizes(CLabel*, void*);
	//{{AFX_MSG(CWaveGenView)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnUpdateViewPlots(CCmdUI* pCmdUI);
	afx_msg void OnViewPlots();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
   afx_msg void OnFilePrint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
   CTreeCtrl    m_Tree;
   HTREEITEM    m_hCommon;
   HTREEITEM    m_hWave;
   HTREEITEM    m_hFourier;
   HTREEITEM    m_hHistory;
};

#ifndef _DEBUG  // Testversion in WaveGenView.cpp
inline CWaveGenDoc* CWaveGenView::GetDocument()
   { return (CWaveGenDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_WAVEGENVIEW_H__349819BE_B864_11D3_B6EC_0000B458D891__INCLUDED_)
