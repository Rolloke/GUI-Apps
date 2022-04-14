/////////////////////////////////////////////////////////////////////////////
// PROJECT:		Tashafirmare
// FILE:		$Workfile: CMDPoints.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/CMDPoints.cpp $
// CHECKIN:		$Date: 11.12.01 8:13 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 11.12.01 8:13 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
#include "tm.h"
#include "CMDPoints.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CMDPoints::CMDPoints()
{

}

//////////////////////////////////////////////////////////////////////
CMDPoints::~CMDPoints()
{
	for (int nI = 0; nI < GetSize(); nI++)
	{
		delete m_Points.GetAt(nI);
		delete m_nValues.GetAt(nI);
	}
	m_Points.RemoveAll();
	m_nValues.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
void CMDPoints::Add(CPoint point, int nValue)
{
	m_Points.Add(new CPoint(point));
	m_nValues.Add(new int(nValue));
}

//////////////////////////////////////////////////////////////////////
CPoint&	CMDPoints::GetPointAt(int nI)
{
	return *(CPoint*)m_Points.GetAt(nI);
}
											    
//////////////////////////////////////////////////////////////////////
int	CMDPoints::GetValueAt(int nI)
{
	return *(int*)m_nValues.GetAt(nI);
}

//////////////////////////////////////////////////////////////////////
void CMDPoints::SortByValue()
{
	for (int nI = 0; nI < m_Points.GetSize(); nI++)
	{
		for (int nJ = nI+1; nJ < m_Points.GetSize(); nJ++)
		{
			if (GetValueAt(nJ) > GetValueAt(nI))
			{
				CPoint* pPoint = (CPoint*)m_Points.GetAt(nI);
				int*	   pValue  = (int*)m_nValues.GetAt(nI);

				m_Points.SetAt(nI, m_Points.GetAt(nJ));
				m_Points.SetAt(nJ, pPoint);
				m_nValues.SetAt(nI, (int*)m_nValues.GetAt(nJ));
				m_nValues.SetAt(nJ, pValue);
			}
		}
	}
}
