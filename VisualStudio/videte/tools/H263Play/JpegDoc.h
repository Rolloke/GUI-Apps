#if !defined(AFX_JPEGDOC_H__23C294C5_EE27_11D1_B51E_00C095EC9EFA__INCLUDED_)
#define AFX_JPEGDOC_H__23C294C5_EE27_11D1_B51E_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// JpegDoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CJpegDoc document

class CJpegDoc : public CDocument
{
protected:
	CJpegDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CJpegDoc)

// Attributes
public:
	LPBYTE	GetBuffer();
	DWORD	GetBufferLength();

// Operations
public:
	CSize GetJpegSize();
	BOOL GetJpegSize(LPBYTE lpBuffer, DWORD dwLen, SIZE &Size);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJpegDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CJpegDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CJpegDoc)
	afx_msg void OnFileSaveAs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL	StretchBmpToDoubleHeigth(const CString &sSource,
									 const CString &sDest);

private:
	LPBYTE	m_pBuffer;
	DWORD	m_dwLen;
	CSize	m_Size;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JPEGDOC_H__23C294C5_EE27_11D1_B51E_00C095EC9EFA__INCLUDED_)
