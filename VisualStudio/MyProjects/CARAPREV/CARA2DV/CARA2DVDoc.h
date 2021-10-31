// CARA2DVDoc.h : Schnittstelle der Klasse CCARA2DVDoc
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CARA2DVDOC_H__30FA9EAB_14B6_11D2_9DB9_0000B458D891__INCLUDED_)
#define AFX_CARA2DVDOC_H__30FA9EAB_14B6_11D2_9DB9_0000B458D891__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "MacroEditorDlg.h"
#include "..\CARADoc.h"
#include "..\PlotLabel.h"
#include "..\CurveLabel.h"
#include "..\PictureLabel.h"

#define MENU_STATES 0x0001
#define EDIT_STATES 0x0002

class CMainFrame;
class CCaraMenu;
class CSaveCurveAsWave;
class CInputSocket;

//struct WAVEFORMATEX;
 
class CCARA2DVDoc : public CCARADoc
{
protected: // Nur aus Serialisierung erzeugen
	CCARA2DVDoc();
	DECLARE_DYNCREATE(CCARA2DVDoc)
   friend class CInputSocket;

// Attribute
public:

#ifdef ETS_OLE_SERVER
protected:
	virtual COleServerItem* OnGetEmbeddedItem();

public:
   void SetModifiedFlag(BOOL);
   bool m_bOleChanged;
   virtual void OnDeactivateUI( BOOL bUndoable );
	virtual void OnDeactivate();
	virtual void OnSaveEmbedding(LPSTORAGE lpStorage);
#endif

// Operationen
public:

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CCARA2DVDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void SetTitle(LPCTSTR lpszTitle);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementierung
public:
   virtual ~CCARA2DVDoc();
   virtual bool Undo(CUndoParams *, CLabel*, LPARAM);

   // Zugriff auf die Daten im Document
   CPlotLabel *InsertCurve(CCurveLabel *, CPlotLabel *pActual=NULL, long nPos = 0x0000ffff);
   CPlotLabel *RemoveCurve(CCurveLabel *, int &);
   void        InsertPlot(CPlotLabel*, int nPos = -1);
   void        RemovePlot(CPlotLabel*, int &);
   void        DeletePlots();
   void        SavePlotRects();
   bool        RestorePlotRects(CDC*, CRect);
   // Menu Update und Erstellung
   void        CheckEditable();
   const CCaraMenu* GetPlotMenu() {return m_pPlotMenu;};
   void        AppendPlotMenu();
   void        BuildNewPlotMenu();
   void        DeletePlotMenu();
   void        InvalidatePlotMenu() {m_nInvalidStates |= MENU_STATES;};
   void        InvalidatePlotEdit() {m_nInvalidStates |= EDIT_STATES;};
   // Datei lesen und schreiben
   void        ReadCalgraphData(HANDLE);
   void        SaveDocument(LPCTSTR);
   void        ReleaseThreadFile();
   void        StartReadWaveThread();
	void        SaveCurveAsWave(CArchive&, CSaveCurveAsWave*);

	LRESULT     OnUserOptUpdate(WPARAM, LPARAM);

private:
   static UINT ReadWaveThread(void *pParam);
   static UINT ReadTextThread(void *pParam);
   static UINT ListenToPortThread(void *pParam);
   static int  CheckEditable(   CLabel *, void *);
   static int  InsertCurve(     CLabel *, void *);
   static int  RemoveCurve(     CLabel *, void *);
   static int  InsertPlot(      CLabel *, void *);
   static int  RemovePlot(      CLabel *, void *);
   static int  SavePlotRects(   CLabel *, void *);
   static int  RestorePlotRects(CLabel *, void *);
   static int  BuildNewPlotMenu(CLabel *, void *);

//   static int  ConvertLabel(    CLabel *, void *);

public:
	static int FindPlotOrCurveByID(CLabel*, void*);
	static int RemoveReferenceCurve(CLabel *pl, void *pParam);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generierte Message-Map-Funktionen
protected:
	void ReadFile(HANDLE, void*, DWORD);
	//{{AFX_MSG(CCARA2DVDoc)
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrint(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrintDirect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnFuncMakroedit();
	afx_msg void OnFuncSaveCurve();
	afx_msg void OnFuncLoadCurve();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFuncLoadCurve(CCmdUI* pCmdUI);
	afx_msg void OnNocecksum();
	afx_msg void OnUpdateFileOpen(CCmdUI* pCmdUI);
    afx_msg void OnFileLoadReadMacros();
    afx_msg void OnUpdateFileLoadReadMacros(CCmdUI *pCmdUI);
    afx_msg void OnFileSaveReadMacros();
    afx_msg void pp(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
   static const char* GetCurveString(CLabelContainer*, SControl);
   static double  GetCurveValue(CLabelContainer*, SControl, int);
   static int     ExtractString(char *, int, char*, SControl *, int&, CString&);
   static void    AllocateNewCurves(CMainFrame *, CLabelContainer *, int);
   static void    SendAndRemoveCurves(CMainFrame *, CLabelContainer*);

   int             m_nPlotRects;
   CRect          *m_pPlotRects;
   CCaraMenu      *m_pPlotMenu;
   CFile          *m_pFile;
   MacroEditorDlg  m_TextMacros;
   int             m_nTextMacro;
   UINT            m_nInvalidStates;
   bool            m_bLoadDocument;
   CEtsLsmb	       m_LsmbDoc;
   CInputSocket*   m_pInputSocket;
public:
};

class CInputSocket : public CSocket
{
    enum {eTitle = 1, eUnitX, eUnitY, eColor, eNext, eFloatValues, eRed, eGreen, eBlue, eYellow, eMagenta, eCyan, eBlack} eReceive;
    enum {eDefault, eExpectingValues} eReceiveState;

	DECLARE_DYNAMIC(CInputSocket)

public:
	CInputSocket(CCARA2DVDoc* pDoc);
// Attributes
public:
	CCARA2DVDoc* m_pDoc;

// Overridable callbacks
protected:
	virtual void OnAccept(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);

// Implementation
public:
	virtual ~CInputSocket();

    void endianconvert(BYTE *buf, int size);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	CInputSocket(const CInputSocket& rSrc);         // no implementation
	void operator=(const CInputSocket& rSrc);  // no implementation
    CLabelContainer m_Container;
    int             m_State;
    int             m_CurveFormat;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // !defined(AFX_CARA2DVDOC_H__30FA9EAB_14B6_11D2_9DB9_0000B458D891__INCLUDED_)
