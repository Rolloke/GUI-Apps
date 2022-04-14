#if !defined(AFX_DOCDIALOGBANK_H__09F35437_5F91_11D3_BA75_00400531137E__INCLUDED_)
#define AFX_DOCDIALOGBANK_H__09F35437_5F91_11D3_BA75_00400531137E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DocDialogbank.h : header file
//

/////////////////////////////////////////////////////////////////////////////
#include "SDITestDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CDocDialogbank document
class CDocDialogbank : public CSDITestDoc
{
protected:
	CDocDialogbank();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDocDialogbank)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual	BOOL	OpenCom();
	virtual void	CreateData();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDocDialogbank)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDocDialogbank();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
	void	CreateDataIntern();
	void	CreateECCard();
	void	CreateTANCard();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDocDialogbank)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	int	m_iCommand;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOCDIALOGBANK_H__09F35437_5F91_11D3_BA75_00400531137E__INCLUDED_)
