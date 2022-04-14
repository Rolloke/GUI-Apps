// H263PlayDoc.h : interface of the CH263PlayDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_H263PLAYDOC_H__265171EE_5DC4_11D1_905F_444553540000__INCLUDED_)
#define AFX_H263PLAYDOC_H__265171EE_5DC4_11D1_905F_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CH263PlayDoc : public CDocument
{
protected: // create from serialization only
	CH263PlayDoc();
	DECLARE_DYNCREATE(CH263PlayDoc)

// Attributes
public:
	inline LPBYTE	GetBuffer();
	inline DWORD	GetBufferLen();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CH263PlayDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CH263PlayDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CH263PlayDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	LPBYTE	m_pBuffer;
	DWORD	m_dwLen;
};
inline LPBYTE CH263PlayDoc::GetBuffer()
{
	return m_pBuffer;
}
inline DWORD CH263PlayDoc::GetBufferLen()
{
	return m_dwLen;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_H263PLAYDOC_H__265171EE_5DC4_11D1_905F_444553540000__INCLUDED_)
