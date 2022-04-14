/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: CMDPoints.cpp $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicDll/CMDPoints.cpp $
// CHECKIN:		$Date: 24.06.02 13:14 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 24.06.02 12:03 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CMDPoints.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

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
		 delete m_Points.GetAt(nI);

	m_Points.RemoveAll();
	m_dwValues.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
void CMDPoints::Add(CPoint point, int nValue)
{
	m_Points.Add(new CPoint(point));
	m_dwValues.Add(nValue);
}

//////////////////////////////////////////////////////////////////////
CPoint&	CMDPoints::GetPointAt(int nI)
{
	return *(CPoint*)m_Points.GetAt(nI);
}
											    
//////////////////////////////////////////////////////////////////////
int	CMDPoints::GetValueAt(int nI)
{
	return (int)m_dwValues.GetAt(nI);
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
				int	   nValue  = m_dwValues.GetAt(nI);

				m_Points.SetAt(nI, m_Points.GetAt(nJ));
				m_Points.SetAt(nJ, pPoint);
				m_dwValues.SetAt(nI, m_dwValues.GetAt(nJ));
				m_dwValues.SetAt(nJ, nValue);

			}
		}
	}
}
