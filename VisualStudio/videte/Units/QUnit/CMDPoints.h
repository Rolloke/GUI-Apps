/////////////////////////////////////////////////////////////////////////////
// PROJECT:		QUnit
// FILE:		$Workfile: CMDPoints.h $
// ARCHIVE:		$Archive: /Project/Units/QUnit/CMDPoints.h $
// CHECKIN:		$Date: 14.11.05 11:59 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 14.11.05 11:59 $
// BY AUTHOR:	$Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CPOINTS_H__5A2191F6_EB93_11D4_8FD8_004005A11E32__INCLUDED_)
#define AFX_CPOINTS_H__5A2191F6_EB93_11D4_8FD8_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMDPoints
{
public:
	CMDPoints();
	virtual ~CMDPoints();
	CPoint&	GetPointAt(int nI);
	int		GetValueAt(int nI);

	void Add(CPoint point, int nValue);
	int  GetSize(){return m_Points.GetSize();};
	void SortByValue();

private:
	CPtrArray	m_Points;
	CDWordArray m_dwValues;
};

#endif // !defined(AFX_CPOINTS_H__5A2191F6_EB93_11D4_8FD8_004005A11E32__INCLUDED_)
