// GraphCreatorDoc.h : interface of the CGraphCreatorDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHCREATORDOC_H__1AD67376_FC17_41AA_A267_3C1DF79301A0__INCLUDED_)
#define AFX_GRAPHCREATORDOC_H__1AD67376_FC17_41AA_A267_3C1DF79301A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CGraphCreatorDoc : public CDocument
{
protected: // create from serialization only
	CGraphCreatorDoc();
	DECLARE_DYNCREATE(CGraphCreatorDoc)

// Attributes
public:

// Operations
public:
	void CreateBasicFilters();
	void ReleaseBasicFilters();
	void ReleaseFilters();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphCreatorDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void OnCloseDocument();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGraphCreatorDoc();

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CGraphCreatorDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	IGraphBuilder*			m_pGraph;
	ICaptureGraphBuilder2*	m_pGraphBuilder;
	IMediaControl*			m_pMediaControl;
	IMediaEventEx*			m_pMediaEvent;
	afx_msg void OnGraphExternal();
	afx_msg void OnUpdateGraphExternal(CCmdUI *pCmdUI);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAPHCREATORDOC_H__1AD67376_FC17_41AA_A267_3C1DF79301A0__INCLUDED_)
