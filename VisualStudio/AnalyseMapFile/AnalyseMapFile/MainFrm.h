// MainFrm.h : Schnittstelle der Klasse CMainFrame
//


#pragma once

class CAnalyseMapFileView;
class CLeftView;
class CCallStackView;

class CMainFrame : public CFrameWnd
{
	
protected: // Nur aus Serialisierung erstellen
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attribute
public:


protected:
	CSplitterWnd m_wndSplitter;
	CSplitterWnd m_wndSplitter2;
public:

// Operationen
public:
	void SetPaneText(UINT nID, CString& sText);
// Überschreibungen
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementierung
public:
	virtual ~CMainFrame();
	CLeftView*GetLeftPane();
	CAnalyseMapFileView* GetRightPane();
	CCallStackView* GetCallStackPane();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // Eingebundene Elemente der Steuerleiste
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// Generierte Funktionen für die Meldungstabellen
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateViewStyles(CCmdUI* pCmdUI);
	afx_msg void OnViewStyle(UINT nCommandID);
	afx_msg void OnEditFindAddress();
	afx_msg void OnEditFindFunction();
	afx_msg void OnEditFindNextFnc();
	afx_msg void OnUpdateEditFindNextFnc(CCmdUI *pCmdUI);
	afx_msg LRESULT OnOpenFile(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEditSearchInModules();
	afx_msg void OnUpdateEditSearchInModules(CCmdUI *pCmdUI);
};


