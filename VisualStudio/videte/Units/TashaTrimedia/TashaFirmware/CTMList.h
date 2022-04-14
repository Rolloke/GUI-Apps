/////////////////////////////////////////////////////////////////////////////
// PROJECT:		JaCobDll
// FILE:		$Workfile: CTMList.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/CTMList.h $
// CHECKIN:		$Date: 11.12.01 8:13 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 11.12.01 8:13 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

class CTMList
{
public:
	CTMList();
	virtual ~CTMList();

	void  AddHead(void* pData);
	void* RemoveHead();
	void  RemoveAll();
	void* GetHead();
	void* GetAt(int nIndex);
	void  SetAt(int nIndex, void* pData);
	void  InsertAt(int nIndex, void* pData);
	void* RemoveAt(int nIndex);
	int	  GetSize();

protected:
	CTMList(CTMList* pList, void* pData);

protected:
	CTMList*	m_pPrev;
	CTMList*	m_pCurr;
	CTMList*	m_pNext;
	void*		m_pData;
	
	int			m_nSize;
};
