#if !defined(AFX_DOCRONDO_H__09F35435_5F91_11D3_BA75_00400531137E__INCLUDED_)
#define AFX_DOCRONDO_H__09F35435_5F91_11D3_BA75_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DocRondo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
#include "SDITestDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CDocRondo document
class CDocRondo : public CSDITestDoc
{
protected:
	CDocRondo();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDocRondo)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual	BOOL	OpenCom();
	virtual void	CreateData();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDocRondo)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDocRondo();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
	void	CreateDataIntern();
	void	CreateStatusRequest();
	void	CreateNormalData();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDocRondo)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	int	m_iCommand;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOCRONDO_H__09F35435_5F91_11D3_BA75_00400531137E__INCLUDED_)
