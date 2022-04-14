/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SecIDArray.cpp $
// ARCHIVE:		$Archive: /Project/CIPC/SecIDArray.cpp $
// CHECKIN:		$Date: 20.01.06 9:28 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 19.01.06 16:10 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$


#include "stdcipc.h"

#include "SecIDArray.h"
#include "wkclasses\WK_Trace.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* CSecIDArray_Overview|
 <c CSecIDArray>, <c CSecID> 

NYD
*/

/////////////////////////////////////////////////////////////////////////////
/*Function: Fills the array from a colon-separated list.
By default the old content is dropped.
*/
void CSecIDArray::FillFromString(const CString &sIDs, BOOL bRemoveContent)
{
	if (bRemoveContent) 
	{
		RemoveAll();
	}
	// loop over the input string, seperator is , 
	// take the slice fom lastIX up to ix-1
	CString sOne;
	int ix=0;
	int lastIX=0;
	while (lastIX<sIDs.GetLength()) 
	{
		// search for the next seperator
		while (ix<sIDs.GetLength() && sIDs[ix]!=',') 
		{
			ix++;
		}
		sOne = sIDs.Mid(lastIX,ix-lastIX);	// second arg is count not position
		sOne.TrimLeft();
		sOne.TrimRight();

		if (sOne.GetLength())
		{
			TRACE(_T("Got idString '%s'\n"), sOne);
		} 
		else
		{
			// an empty field
		}

		ix++;
		lastIX = ix;
	}
}

/*Function: return a colon-seperated list of the ids.
*/
CString CSecIDArray::CreateString() const
{
	CString sResult;
	CString sOne;

	for (int i=0;i<GetSize();i++) 
	{
		sOne.Format(_T("%08x"), GetAtFast(i));	// OOPS use %08x or just %x
		if (i>0) 
		{
			sResult += ',';
		}
		sResult += sOne;
	}

	return sResult;
}
