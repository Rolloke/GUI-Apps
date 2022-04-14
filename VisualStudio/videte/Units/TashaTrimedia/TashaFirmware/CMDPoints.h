/////////////////////////////////////////////////////////////////////////////
// PROJECT:		Tashafirmare
// FILE:		$Workfile: CMDPoints.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/CMDPoints.h $
// CHECKIN:		$Date: 11.12.01 8:13 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 11.12.01 8:13 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////

#ifndef __CMDPOINTS_H__
#define __CMDPOINTS_H__

#include "CTMArray.h"

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
	CTMArray	m_Points;
	CTMArray	m_nValues;
};

#endif // __CMDPOINTS_H__
