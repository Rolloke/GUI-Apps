// HtmlViewDoc.h : interface of the CHtmlViewDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTMLVIEWDOC_H__11FDFA6F_0122_11D3_8D1C_004005A11E32__INCLUDED_)
#define AFX_HTMLVIEWDOC_H__11FDFA6F_0122_11D3_8D1C_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CHtmlViewDoc : public CDocument
{
protected: // create from serialization only
	CHtmlViewDoc();
	DECLARE_DYNCREATE(CHtmlViewDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHtmlViewDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHtmlViewDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CHtmlViewDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HTMLVIEWDOC_H__11FDFA6F_0122_11D3_8D1C_004005A11E32__INCLUDED_)
