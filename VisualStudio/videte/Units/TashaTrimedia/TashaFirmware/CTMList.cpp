/////////////////////////////////////////////////////////////////////////////
// PROJECT:		JaCobDll
// FILE:		$Workfile: CTMList.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaFirmware/CTMList.cpp $
// CHECKIN:		$Date: 11.12.01 8:13 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 11.12.01 8:13 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
#include "tm.h"
#include "CTMList.h"

/////////////////////////////////////////////////////////////////////////////
CTMList::CTMList()
{
	m_pNext	= NULL;
	m_pPrev	= NULL;
	m_pCurr	= this;
	m_pData	= NULL;
	m_nSize= 0;
}

/////////////////////////////////////////////////////////////////////////////
CTMList::CTMList(CTMList* pList, void* pData)
{
	m_pPrev	= pList;
	m_pCurr	= this;
	m_pNext	= NULL;
	m_pData	= pData;
	m_nSize= 0;
}

/////////////////////////////////////////////////////////////////////////////
CTMList::~CTMList()
{
	delete m_pNext;
	m_pNext = NULL;
}

/////////////////////////////////////////////////////////////////////////////
void CTMList::AddHead(void* pData)
{
	CTMList* pNext	= m_pNext;
	CTMList* pCurr	= this;
	
	while (pNext != NULL)
	{
		pCurr = pNext->m_pCurr;
		pNext = pNext->m_pNext;
	}

	pCurr->m_pNext = new CTMList(pCurr, pData);
	m_nSize++;
}

/////////////////////////////////////////////////////////////////////////////
void* CTMList::RemoveHead()
{
	CTMList* pNext		= this;
	CTMList* pCurr	= NULL;
	
	if (m_nSize == 0)
		return NULL;

	do
	{
		pCurr = pNext->m_pCurr;
		pNext	 = pNext->m_pNext;
	}while (pNext != NULL);

	pCurr->m_pPrev->m_pNext = NULL;
	void* pData = pCurr->m_pData;
	delete(pCurr);
	m_nSize--;

	return pData;
}

/////////////////////////////////////////////////////////////////////////////
void CTMList::RemoveAll()
{
	void* pData;

	while((pData = RemoveHead()) != NULL)
		delete pData;
}

/////////////////////////////////////////////////////////////////////////////
void* CTMList::GetHead()
{
	CTMList* pNext = this;
	void*	 pData = NULL; 

	if (m_nSize == 0)
		return NULL;
	
	do
	{
		pData = pNext->m_pData;
		pNext = pNext->m_pNext;
	}while (pNext != NULL);

	return pData;
}

/////////////////////////////////////////////////////////////////////////////
void* CTMList::GetAt(int nIndex)
{
	CTMList* pNext = this;
	void*	 pData = NULL; 
	if ((nIndex < m_nSize) && (m_nSize > 0))
	{
		int nI = 0;
		do
		{
			pData = pNext->m_pData;
			pNext = pNext->m_pNext;
		}while ((pNext != NULL) && (nI++ != nIndex+1));
	}
	return pData;
}

/////////////////////////////////////////////////////////////////////////////
void CTMList::SetAt(int nIndex, void* pData)
{
	CTMList* pNext = this;
	CTMList* pCurr = NULL;
	if (nIndex < m_nSize)
	{
		int nI = 0;
		do
		{
			pCurr = pNext->m_pCurr;
			pNext = pNext->m_pNext;
		}while ((pNext != NULL) && (nI++ != nIndex+1));

		pCurr->m_pData	= pData;
	}
}	

/////////////////////////////////////////////////////////////////////////////
void CTMList::InsertAt(int nIndex, void* pData)
{
	CTMList* pNext = this;
	CTMList* pCurr = NULL;

	int nI = 0;
	do
	{
		pCurr = pNext->m_pCurr;
		pNext = pNext->m_pNext;
	}while ((pNext != NULL) && (nI++ != nIndex+1));

	CTMList* pNew = new CTMList(pCurr, pData);

	pNew->m_pPrev			= pCurr->m_pPrev;
	pNew->m_pNext			= pCurr;
	pCurr->m_pPrev->m_pNext = pNew;
	pCurr->m_pPrev			= pNew;	
	m_nSize++;
}	

/////////////////////////////////////////////////////////////////////////////
void* CTMList::RemoveAt(int nIndex)
{
	CTMList* pNext	= this;
	CTMList* pCurr	= NULL;
	void* pData		= NULL;

	if ((nIndex < m_nSize) && (m_nSize > 0))
	{
		int nI = 0;
		do
		{
			pCurr = pNext->m_pCurr;
			pNext = pNext->m_pNext;
		}while ((pNext != NULL) && (nI++ != nIndex+1));

		
		pCurr->m_pPrev->m_pNext	= pCurr->m_pNext;
		if (pCurr->m_pNext)
			pCurr->m_pNext->m_pPrev	= pCurr->m_pPrev;
		pCurr->m_pPrev			= NULL;
		pCurr->m_pNext			= NULL;

		pData = pCurr->m_pData;
		delete pCurr;

		m_nSize--;
	}

	return pData;
}	

/////////////////////////////////////////////////////////////////////////////
int CTMList::GetSize()
{
	return m_nSize;
}
