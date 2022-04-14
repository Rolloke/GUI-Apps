/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDIMultiDocTemplate.h $
// ARCHIVE:		$Archive: /Project/Clients/SDIConfig/SDIMultiDocTemplate.h $
// CHECKIN:		$Date: 16.01.98 9:57 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 5.12.97 13:21 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#if !defined(AFX_SDIMULTIDOCTEMPLATE_H__C64545C2_5B8D_11D1_8143_EAF88A4ACC28__INCLUDED_)
#define AFX_SDIMULTIDOCTEMPLATE_H__C64545C2_5B8D_11D1_8143_EAF88A4ACC28__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CSDIMultiDocTemplate document

// MDI support (zero or more documents)
// Code direkt aus MFC (Afxwin.h)
class CSDIMultiDocTemplate : public CMultiDocTemplate
{
	DECLARE_DYNAMIC(CSDIMultiDocTemplate)

// Constructors
public:
	CSDIMultiDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
		CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);

/*
// Implementation
public:
	// Menu and accel table for MDI Child windows of this type
	HMENU m_hMenuShared;
	HACCEL m_hAccelTable;

	virtual ~CSDIMultiDocTemplate();
	virtual void LoadTemplate();
	virtual void AddDocument(CDocument* pDoc);
	virtual void RemoveDocument(CDocument* pDoc);
	virtual POSITION GetFirstDocPosition() const;
	virtual CDocument* GetNextDoc(POSITION& rPos) const;
	virtual CDocument* OpenDocumentFile(
		LPCTSTR lpszPathName, BOOL bMakeVisible = TRUE);
*/	virtual void SetDefaultTitle(CDocument* pDocument);
/*
#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG

protected:  // standard implementation
	CPtrList m_docList;          // open documents of this type
	UINT m_nUntitledCount;   // start at 0, for "Document1" title
*/
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDIMULTIDOCTEMPLATE_H__C64545C2_5B8D_11D1_8143_EAF88A4ACC28__INCLUDED_)
