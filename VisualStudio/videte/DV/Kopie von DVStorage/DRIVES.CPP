// Drives.cpp: implementation of the CDrives class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dvstorage.h"
#include "Drives.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDrives::CDrives()
{
	m_nNrOfDBDrives = 0;
}
//////////////////////////////////////////////////////////////////////
CDrives::~CDrives()
{
}
//////////////////////////////////////////////////////////////////////
// find's out the drive with most free space
CIPCDrive* CDrives::GetFreeDrive(WORD wCollectionIDpart)
{
	CIPCDrive* pDriveMax = NULL;
	CAutoCritSec acs(&m_cs);

	if (theApp.m_bEqualArchiveDistribution)	// tries to distribute the archives equally to the drives
	{
		int i;
		DWORD dwTotal = 0;
		CIPCDrive* pDrive;
		for (i=0;i<GetSize();i++)			// determine total size of DB drives
		{
			pDrive = GetAtFast(i);
			if (pDrive->IsDatabase())
			{
				if (m_nNrOfDBDrives == 1)	// only one DB drive
				{
					pDriveMax = pDrive;		// do not search, finnish
					break;
				}
				dwTotal += pDrive->GetMB();
			}
		}

		if (m_nNrOfDBDrives > 1)			// more than one drive
		{
			DWORD dwSize, dwPercent;
			WORD wDrive = 0;
			double dDist, dMin = 1000000;
			m_mapCollectionToDrive.Lookup(wCollectionIDpart, wDrive);
			// is a collection Id part already on a drive?
			for (i=0;i<GetSize();i++)
			{
				pDrive = GetAtFast(i);
				if (pDrive->IsDatabase())
				{
					if (wDrive == pDrive->GetRootString().GetAt(0))
					{	// force ring archive and alarm archive to the same drive
						pDriveMax = pDrive;
						break;
					}
					dwSize = pDrive->GetMB();
					// calculate drive size in percent to total available size on all drives
					dwPercent = MulDiv(dwSize, 100, dwTotal);
					dDist = (double)GetCollectionsOfDrive(pDrive->GetRootString()) / (double)dwPercent;
					if (dDist < dMin)	// distribute the archives equally to their sizes
					{
						dMin = dDist;
						pDriveMax = pDrive;
					}
				}
			}
		}
	}

	if (m_nNrOfDBDrives > 1 && pDriveMax)	// check available size
	{
		DWORD dwSize = pDriveMax->GetFreeMB(), dwTotal = pDriveMax->GetMB(), dwPercent;
		dwPercent = MulDiv(dwSize, 100, dwTotal);
		if (dwPercent < 15)					// only 15 percent free
		{
			pDriveMax = NULL;				// find annother drive with the old distribution algorithm
		}
	}

	if (pDriveMax == NULL)
	{
		CIPCDrive* pDrive;
		DWORD   dwFixed = 0;
		DWORD   dwFixedUsed = 0;
		DWORD	dwFreeDrive = 0;
		DWORD	dwFree		= 0;
		DWORD	dwFreeMax	= 0;
		int i;

		for (i=0;i<GetSize();i++)
		{
			pDrive = GetAtFast(i);
			if (pDrive->IsDatabase())
			{
				pDrive->CheckSpace();
				dwFixed = 0;
				dwFreeDrive = pDrive->GetFreeMB();

				if (dwFixed>0)
				{
					// is there any archive fixed to this drive ?
					// calc space use by these archives
					dwFixedUsed	= 0;
					if (dwFixed>=dwFixedUsed)
					{
						if (dwFreeDrive > (dwFixed - dwFixedUsed))
						{
							dwFree = dwFreeDrive - (dwFixed - dwFixedUsed);
						}
						else
						{
							dwFree = 0;
						}
					}
					else
					{
						dwFree = 0;
					}
				}
				else
				{
					dwFree = dwFreeDrive;
				}

				if (pDriveMax==NULL || dwFree>dwFreeMax)
				{
					pDriveMax = pDrive;
					dwFreeMax = dwFree;
				}
			}
		}
	}
	acs.Unlock();

	return pDriveMax;
}
//////////////////////////////////////////////////////////////////////
void CDrives::Initialize()
{
	m_nNrOfDBDrives = 0;
	for (int i=0;i<theApp.m_Drives.GetSize();i++)
	{
		CIPCDrive *pDrive = theApp.m_Drives.GetAtFast(i);
		if (pDrive->IsDatabase())
		{
			m_nNrOfDBDrives++;
		}
	}
}
//////////////////////////////////////////////////////////////////////
WORD CDrives::GetCollectionsOfDrive(CString sDrive)
{
	WORD wDrive = sDrive.GetAt(0);
	WORD wCollections = 0;
	m_mapDriveToCollections.Lookup(wDrive, wCollections);
	return wCollections;
}
//////////////////////////////////////////////////////////////////////
void CDrives::SetCollectionsToDrive(CString sDrive, WORD wCollectionIDpart, BOOL bAdd)
{
	WORD wCollections = CDrives::GetCollectionsOfDrive(sDrive);
	WORD wDrive = sDrive.GetAt(0);
	if (bAdd)	wCollections++;
	else		wCollections--;
	m_mapDriveToCollections.SetAt(wDrive, wCollections);
	m_mapCollectionToDrive.SetAt(wCollectionIDpart, wDrive);
}
