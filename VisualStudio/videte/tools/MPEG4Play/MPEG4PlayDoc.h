// MPEG4PlayDoc.h : interface of the CMPEG4PlayDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MPEG4PLAYDOC_H__8773860F_E592_4CF3_84F3_637130DFBFD3__INCLUDED_)
#define AFX_MPEG4PLAYDOC_H__8773860F_E592_4CF3_84F3_637130DFBFD3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MPEG4Decoder.h"
#include "MPEG4Encoder.h"

class CMPEG4PlayDoc : public CDocument
{
protected: // create from serialization only
	CMPEG4PlayDoc();
	DECLARE_DYNCREATE(CMPEG4PlayDoc)

// Attributes
public:
private:
	CMPEG4Decoder*  m_pMPEG4Decoder;
	CMPEG4Encoder*  m_pMPEG4Encoder;

// Operations
public:
	CMPEG4Decoder* GetMPEG4Decoder();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMPEG4PlayDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	CMPEG4Encoder* GetMPEG4Encoder();
	virtual ~CMPEG4PlayDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMPEG4PlayDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MPEG4PLAYDOC_H__8773860F_E592_4CF3_84F3_637130DFBFD3__INCLUDED_)
