/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDIConfigRecord.h $
// ARCHIVE:		$Archive: /Project/Clients/SDIConfig/SDIConfigRecord.h $
// CHECKIN:		$Date: 16.01.98 9:57 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 14.01.98 14:24 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#if !defined(AFX_SDICONFIGRECORD_H__B2CBB2C2_67C4_11D1_9F29_0000C036AC0D__INCLUDED_)
#define AFX_SDICONFIGRECORD_H__B2CBB2C2_67C4_11D1_9F29_0000C036AC0D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
#include "SecID.h"

/////////////////////////////////////////////////////////////////////////////
// CSDIConfigRecord command target
class CSDIConfigRecord : public CObject
{
// Construction
public:
	CSDIConfigRecord(CSecID id, CString& sComment);
	virtual ~CSDIConfigRecord();

// Attributes
public:
	inline	CSecID			GetID() const;
	inline	const CString&	GetComment() const;

// Operations
public:
	void	SetComment(CString& sComment);

// Implementation
protected:

	CSecID	m_id;		// ID des Inputs
	CString	m_sComment;	// Kommentar zum entsprechenden ControlRecord
};
/////////////////////////////////////////////////////////////////////////////
CSecID CSDIConfigRecord::GetID() const
{
	return m_id;
}
/////////////////////////////////////////////////////////////////////////////
const CString& CSDIConfigRecord::GetComment() const
{
	return m_sComment;
}
/////////////////////////////////////////////////////////////////////////////
typedef CSDIConfigRecord* CSDIConfigRecordPtr;
WK_PTR_ARRAY_CS(CSDIConfigRecordArray, CSDIConfigRecordPtr, CSDIConfigRecordArray_CS);

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDICONFIGRECORD_H__B2CBB2C2_67C4_11D1_9F29_0000C036AC0D__INCLUDED_)
