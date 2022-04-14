#ifndef AFX_CARAVIEW_H__30FA9EDC_14B6_11D2_9DB9_0000B458D891__INCLUDED_
#define AFX_CARAVIEW_H__30FA9EDC_14B6_11D2_9DB9_0000B458D891__INCLUDED_

// CaraView.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Ansicht CCaraView 
class CCARADoc;

class AFX_EXT_CLASS CCaraView : public CScrollView
{
protected:
	CCaraView();           // Dynamische Erstellung verwendet geschützten Konstruktor
	DECLARE_DYNCREATE(CCaraView)

// Attribute
public:

// Operationen
public:
   CCARADoc* GetDocument();

	void OnFilePrintPreview();
   CSize GetScreenSize();
	CSize GetScrollBarSize();

	int m_nCurrentPage;
	int m_nPages;

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CCaraView)
	public:
	virtual void OnDraw(CDC* pDC);      // Überschrieben zum Zeichnen dieser Ansicht
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual void OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView);
	//}}AFX_VIRTUAL

// Implementierung
protected:
	virtual ~CCaraView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	//{{AFX_MSG(CCaraView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_CARAVIEW_H__30FA9EDC_14B6_11D2_9DB9_0000B458D891__INCLUDED_
