/* GlobalReplace: CSDITestIbmDoc --> CDocIbm */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: DocIbm.h $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/SDITest/DocIbm.h $
// CHECKIN:		$Date: 10.05.99 11:56 $
// VERSION:		$Revision: 6 $
// LAST CHANGE:	$Modtime: 10.05.99 11:18 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#if !defined(AFX_SDITESTIBMDOC_H__FE93CAC5_367A_11D2_AFCB_00C095EC2267__INCLUDED_)
#define AFX_SDITESTIBMDOC_H__FE93CAC5_367A_11D2_AFCB_00C095EC2267__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
#include "SDITestDoc.h"

enum eParameter
{
	PARAM_ACCOUNT,
	PARAM_BANKCODE,
	PARAM_VALUE,
	PARAM_CURRENCY,
	PARAM_WORKSTATION,
	PARAM_TRANSAKTION,
	PARAM_DAY,
	PARAM_MONTH,
	PARAM_YEAR,
	PARAM_HOUR,
	PARAM_MINUTE,
	PARAM_SECOND,
	PARAM_LAST,
};

/////////////////////////////////////////////////////////////////////////////
// CDocIbm document
class CDocIbm : public CSDITestDoc
{
protected:
	CDocIbm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDocIbm)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual	BOOL	OpenCom();
	virtual void	CreateData();
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDocIbm)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDocIbm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
	void	CreateDataIntern();
	void	CreateNormalData();
	void	CreateStatusRequest();
	void	ReplaceVarData(int iStart, int iEnd, CString sReplace);

	// Generated message map functions
protected:
	//{{AFX_MSG(CDocIbm)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Data
public:
	int		m_iPointOfData;
	CString	m_sDataFormat;
	BOOL	m_bStatusRequest;
	BOOL	m_bWithTerminalID;
protected:
	CString	m_sVarData;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDITESTIBMDOC_H__FE93CAC5_367A_11D2_AFCB_00C095EC2267__INCLUDED_)
