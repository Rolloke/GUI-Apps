/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDIConfigKebaDoc.h $
// ARCHIVE:		$Archive: /Project/Clients/SDIConfig/SDIConfigKebaDoc.h $
// CHECKIN:		$Date: 26.01.98 15:00 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 26.01.98 12:11 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#if !defined(AFX_SDICONFIGKEBADOC_H__5A614CA3_5BFB_11D1_9F29_0000C036AC0D__INCLUDED_)
#define AFX_SDICONFIGKEBADOC_H__5A614CA3_5BFB_11D1_9F29_0000C036AC0D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
#include "SDIConfigDoc.h"
#include "SecID.h"

/////////////////////////////////////////////////////////////////////////////
class CSDIConfigKebaView;

/////////////////////////////////////////////////////////////////////////////
// CSDIConfigKebaDoc document
class CSDIConfigKebaDoc : public CSDIConfigDoc
{
	DECLARE_DYNCREATE(CSDIConfigKebaDoc)
// Construction/Destruction
public:
	virtual	~CSDIConfigKebaDoc();
protected:
			CSDIConfigKebaDoc();	// protected constructor used by dynamic creation

// Attributes
public:
			CSDIConfigKebaView* GetView();
	virtual inline	BOOL		WasExternProgramFound();

// Implementation
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void	Configurate(CSecID id);
	void	StartTransferConfigFile();
	void	StartConfiguration();

// Overrides
public:
	virtual	BOOL	IsControlRecordOk(SDIControlType ControlType);
	virtual void	OnIdle();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSDIConfigKebaDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CSDIConfigKebaDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Data
private:
	BOOL	m_bExternProgramFound;
};
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSDIConfigKebaDoc::WasExternProgramFound()
{
	return m_bExternProgramFound;
}
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDICONFIGKEBADOC_H__5A614CA3_5BFB_11D1_9F29_0000C036AC0D__INCLUDED_)
