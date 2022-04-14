// InstanceCounter.cpp: implementation of the CInstanceCounter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CInstanceCounter.h"

//////////////////////////////////////////////////////////////////////
CInstanceCounter::CInstanceCounter(const CString &sSMName)
{
	SHAREDSTRUCT*		pShared	= NULL;
	HGLOBAL		 		hShared = NULL;
	
	m_nInstance		= -1;
	if (!sSMName.IsEmpty())
		m_sSMName	= sSMName;
	else						    
		m_sSMName	= "GenericUnit";
	
	m_AppID			=  WAI_INVALID;
	
	// Zugriffskonflikt mit anderen Instanzen verhindern
	m_pMutex = new CMutex(FALSE, m_sSMName + "Mutex", NULL);
	if (m_pMutex)
		m_pMutex->Lock();	

	// Pointer auf den Sharedmemory holen
	hShared = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, TRUE, sSMName);
	if (hShared == NULL)
		hShared = CreateFileMapping((HANDLE)-1, NULL, PAGE_READWRITE, 0, sizeof(SHAREDSTRUCT), m_sSMName);

	if (hShared != NULL)
		pShared = (SHAREDSTRUCT*) MapViewOfFile(hShared, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);	   	
					 
	if (!pShared)
	{
		if (m_pMutex)
			m_pMutex->Unlock();
		WK_DELETE(m_pMutex);
		return;
	}

	// Suche freie Instance							 
	for (WORD wI = 0; wI < 31; wI++)
	{
		if (!TSTBIT(pShared->dwCardRef, wI))
		{
			// Karte eintragen
			pShared->dwCardRef = SETBIT(pShared->dwCardRef, wI);
			m_nInstance = wI;
			break;
		}
	}

	// ApplicationID der GenericUnit ermitteln
	switch (m_nInstance)
	{
		case 0:
			m_AppID = WAI_JACOBUNIT_1;
			break;
		case 1:
			m_AppID = WAI_JACOBUNIT_2;
			break;
		case 2:
			m_AppID = WAI_JACOBUNIT_3;
			break;
		case 3:
			m_AppID = WAI_JACOBUNIT_4;
			break;
		default:
			m_AppID = WAI_INVALID;
	}

	if (m_pMutex)
		m_pMutex->Unlock();
	WK_DELETE(m_pMutex);
}

//////////////////////////////////////////////////////////////////////
CInstanceCounter::~CInstanceCounter()
{
	SHAREDSTRUCT*		pShared	= NULL;
	HGLOBAL				hShared = NULL;

	// Zugriffskonflikt mit anderen Instanzen verhindern
	m_pMutex = new CMutex(FALSE, m_sSMName + "Mutex", NULL);
	if (m_pMutex)
		m_pMutex->Lock();

	// Pointer auf den Sharedmemory holen
	hShared = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, TRUE, m_sSMName);
	if (hShared == NULL)
		hShared = CreateFileMapping((HANDLE)-1, NULL, PAGE_READWRITE, 0, sizeof(SHAREDSTRUCT), m_sSMName);

	if (hShared != NULL)
		pShared = (SHAREDSTRUCT*) MapViewOfFile(hShared, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);	   				 
	
	// Karte austragen
	if (pShared)
		pShared->dwCardRef = CLRBIT(pShared->dwCardRef, m_nInstance);

	if (m_pMutex)
		m_pMutex->Unlock();

	WK_DELETE(m_pMutex);
}

