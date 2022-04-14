/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SecIDArray.h $
// ARCHIVE:		$Archive: /Project/CIPC/SecIDArray.h $
// CHECKIN:		$Date: 11.05.05 16:58 $
// VERSION:		$Revision: 7 $
// LAST CHANGE:	$Modtime: 11.05.05 16:56 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$


#ifndef _SECID_ARRAY_H
#define _SECID_ARRAY_H


#include "wk.h"
#include "SecID.h"

//
/////////////////////////////////////////////////////////////////////////////
// 
//A subclass from CDWordArray with special conversion fns.
//
// String interface: <mf CSecIDArray::CreateString>, <mf CSecIDArray::FillFromString>
//
class AFX_EXT_CLASS CSecIDArray : public CDWordArray
{
public:
	void FillFromString(const CString &sIDs, BOOL bRemoveContent=TRUE);
	CString CreateString() const;

	inline BOOL Has(CSecID id) const;
	inline BOOL Remove(CSecID id);
	inline CSecID GetAtFast(int nPos) const;
};

inline BOOL CSecIDArray::Has(CSecID id) const
{
	BOOL bResult = FALSE;
	
	for (int i=0; i<GetSize() && bResult==FALSE;i++) 
	{
		if (id==GetAtFast(i)) 
		{
			bResult=TRUE;
		}
	}

	return bResult;
}

inline BOOL CSecIDArray::Remove(CSecID id)
{
	BOOL bResult = FALSE;
	
	for (int i=0;i<GetSize() && bResult==FALSE;i++) 
	{
		if (id==GetAtFast(i)) 
		{
			bResult=TRUE;
			RemoveAt(i);
		}
	}

	return bResult;
}

inline CSecID CSecIDArray::GetAtFast(int nPos) const
{
	ASSERT(nPos >= 0 && nPos < m_nSize);
	return CSecID(m_pData[nPos]);
} 

#endif
