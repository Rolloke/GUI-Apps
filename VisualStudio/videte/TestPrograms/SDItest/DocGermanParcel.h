/* GlobalReplace: CSDITestGermanParcelDoc --> CDocGermanParcel */
#if !defined(AFX_SDITESTGERMANPARCELDOC_H__FA98AAB4_3E3D_11D2_AFDB_00C095EC2267__INCLUDED_)
#define AFX_SDITESTGERMANPARCELDOC_H__FA98AAB4_3E3D_11D2_AFDB_00C095EC2267__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SDITestGermanParcelDoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
#include "SDITestDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CDocGermanParcel document

class CDocGermanParcel : public CSDITestDoc
{
protected:
	CDocGermanParcel();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDocGermanParcel)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual	BOOL	OpenCom();
	virtual void	CreateData();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDocGermanParcel)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDocGermanParcel();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
	void	CreateDataIntern();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDocGermanParcel)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDITESTGERMANPARCELDOC_H__FA98AAB4_3E3D_11D2_AFDB_00C095EC2267__INCLUDED_)
