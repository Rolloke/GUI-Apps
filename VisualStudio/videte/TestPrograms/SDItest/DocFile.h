#if !defined(AFX_FILEDOC_H__FCF4EAAD_7FE0_11D3_99D2_004005A19028__INCLUDED_)
#define AFX_FILEDOC_H__FCF4EAAD_7FE0_11D3_99D2_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileDoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
#include "SDITestDoc.h"
/////////////////////////////////////////////////////////////////////////////
// CFileDoc document

class CFileDoc : public CSDITestDoc
{
protected:
	CFileDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CFileDoc)

// Attributes
public:
	CComParameters GetComParameters();

// Operations
public:
	void LoadFile(const CString& sPathName);
	void SetComParameters(const CComParameters& com);

// Overrides
protected:
	virtual	BOOL	OpenCom();
	virtual BOOL	IsDataOK();
	virtual void	CreateData();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFileDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CFileDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CComParameters m_ComParameters;
	CByteArray m_Data;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEDOC_H__FCF4EAAD_7FE0_11D3_99D2_004005A19028__INCLUDED_)
