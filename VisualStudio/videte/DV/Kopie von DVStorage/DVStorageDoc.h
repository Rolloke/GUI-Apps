// DVStorageDoc.h : interface of the CDVStorageDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DVSTORAGEDOC_H__F4B60C11_8ACE_11D3_99EA_004005A19028__INCLUDED_)
#define AFX_DVSTORAGEDOC_H__F4B60C11_8ACE_11D3_99EA_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CDVStorageDoc : public CDocument
{
protected: // create from serialization only
	CDVStorageDoc();
	DECLARE_DYNCREATE(CDVStorageDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDVStorageDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDVStorageDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDVStorageDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DVSTORAGEDOC_H__F4B60C11_8ACE_11D3_99EA_004005A19028__INCLUDED_)
