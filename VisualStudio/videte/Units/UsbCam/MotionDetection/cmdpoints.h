/////////////////////////////////////////////////////////////////////////////
// PROJECT:		USBCamUnit
// FILE:		$Workfile: cmdpoints.h $
// ARCHIVE:		$Archive: /Project/Units/USBCAM/MotionDetection/cmdpoints.h $
// CHECKIN:		$Date: 21.08.03 14:45 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 21.08.03 12:25 $
// BY AUTHOR:	$Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CPOINTS_H__5A2191F6_EB93_11D4_8FD8_004005A11E32__INCLUDED_)
#define AFX_CPOINTS_H__5A2191F6_EB93_11D4_8FD8_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AFX_EXT_CLASS CMDPoints
{
public:
	void Reset();
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
