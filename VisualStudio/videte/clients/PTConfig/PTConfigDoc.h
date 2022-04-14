// PTConfigDoc.h : interface of the CPTConfigDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PTCONFIGDOC_H__9297D985_3517_11D1_B8C8_0060977A76F1__INCLUDED_)
#define AFX_PTCONFIGDOC_H__9297D985_3517_11D1_B8C8_0060977A76F1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "wk.h"
#include "WK_Template.h"

class CPTOutputClient;
class CPTInputClient;
class CPostFetchResult;

#define THE_MESSAGE (WM_USER+555)

enum PTConfigState {
	PCS_CALLING=0,
	PCS_CONNECTED=1,
	PCS_FAILED=2,
	PCS_DISCONNECTED=3
};
class CPTConfigDoc : public CDocument
{
public:
	static int m_iInstances;
protected: // create from serialization only
	CPTConfigDoc();
	DECLARE_DYNCREATE(CPTConfigDoc)

// Attributes
public:
	const CString &GetState() const;
	void AppendState(const CString &sState);
	void HandleFetchResult(const CPostFetchResult &result);
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPTConfigDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPTConfigDoc();
	//
	void ForceUpdate();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CPTConfigDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CPTInputClient *m_pInput;
	CPTOutputClient *m_pOutput;
	CString m_sState;
	PTConfigState m_state;
	CString m_sRemoteHost;
	CTime m_onlineStartTime;
	CString m_sBoxName;
	CString m_sBoxNumber;
};

typedef CPTConfigDoc * CPTConfigDocPtr;
WK_PTR_ARRAY_CS(CPTConfigDocsPlain,CPTConfigDocPtr,CPTConfigDocs);

extern CPTConfigDocs g_theDocs;	// global instead of static member to avoid declaration problems

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PTCONFIGDOC_H__9297D985_3517_11D1_B8C8_0060977A76F1__INCLUDED_)
