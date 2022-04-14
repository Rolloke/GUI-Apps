// DBSDoc.h : interface of the CDBSDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DBSDOC_H__AB5B236B_3834_11D2_B58E_00C095EC9EFA__INCLUDED_)
#define AFX_DBSDOC_H__AB5B236B_3834_11D2_B58E_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CDBSView;

class CDBSDoc : public CDocument
{
protected: // create from serialization only
	CDBSDoc();
	DECLARE_DYNCREATE(CDBSDoc)

// Attributes
public:

// Operations
public:
	CDBSView* GetView();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDBSDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDBSDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDBSDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBSDOC_H__AB5B236B_3834_11D2_B58E_00C095EC9EFA__INCLUDED_)
