// DocAvmCustomerSafe.h : header file
//
#if !defined(AFX_DOCAVMCUSTOMERSAFE_H__4C6F5A38_F7B6_11D2_B9FA_00400531137E__INCLUDED_)
#define AFX_DOCAVMCUSTOMERSAFE_H__4C6F5A38_F7B6_11D2_B9FA_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
#include "SDITestDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CDocAvmCustomerSafe document
class CDocAvmCustomerSafe : public CSDITestDoc
{
protected:
	CDocAvmCustomerSafe();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDocAvmCustomerSafe)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual	BOOL	OpenCom();
	virtual void	CreateData();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDocAvmCustomerSafe)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDocAvmCustomerSafe();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
	void	CreateDataIntern();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDocAvmCustomerSafe)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	int	m_iCommand;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOCAVMCUSTOMERSAFE_H__4C6F5A38_F7B6_11D2_B9FA_00400531137E__INCLUDED_)
