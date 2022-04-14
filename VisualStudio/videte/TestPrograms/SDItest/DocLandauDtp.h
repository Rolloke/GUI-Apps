/* GlobalReplace: CSDITestDtpDoc --> CDocLandauDtp */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: DocLandauDtp.h $
// ARCHIVE:		$Archive: /PROJECT/tools/TstTools/SDITest/DocLandauDtp.h $
// CHECKIN:		$Date: 21.04.99 16:33 $
// VERSION:		$Revision: 11 $
// LAST CHANGE:	$Modtime: 21.04.99 11:09 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#if !defined(AFX_SDITESTDTPDOC_H__2D22C52D_A2C7_11D1_9F29_0000C036AC0D__INCLUDED_)
#define AFX_SDITESTDTPDOC_H__2D22C52D_A2C7_11D1_9F29_0000C036AC0D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
#include "SDITestDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CDocLandauDtp document
class CDocLandauDtp : public CSDITestDoc
{
protected:
	CDocLandauDtp();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDocLandauDtp)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual	BOOL	OpenCom();
	virtual void	CreateData();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDocLandauDtp)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDocLandauDtp();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
	void CreateDataIntern(CString sPortNr, CString sCameraNr, CString sTAN) ;

	// Generated message map functions
protected:
	//{{AFX_MSG(CDocLandauDtp)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	int	m_iPortNr;
	int	m_iCameraNr;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDITESTDTPDOC_H__2D22C52D_A2C7_11D1_9F29_0000C036AC0D__INCLUDED_)
