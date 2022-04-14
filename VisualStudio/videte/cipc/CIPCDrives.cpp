/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCDrives.cpp $
// ARCHIVE:		$Archive: /Project/CIPC/CIPCDrives.cpp $
// CHECKIN:		$Date: 23.03.06 13:48 $
// VERSION:		$Revision: 30 $
// LAST CHANGE:	$Modtime: 23.03.06 13:39 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdcipc.h"
#include "CIPCDrives.h"
#include "CIPC.h"
#include "CipcExtraMemory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static const TCHAR szDrives[] = _T("Drives");
// for old 30 reg format
static const TCHAR szExcludedDrives[] = _T("ExcludedDrives");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIPCDrives::CIPCDrives()
{

}
//////////////////////////////////////////////////////////////////////
CIPCDrives::~CIPCDrives()
{
	SafeDeleteAll();
}
//////////////////////////////////////////////////////////////////////
void CIPCDrives::Init(CWK_Profile& wkp)
{
	m_sFailedDrives.Empty();
	SafeDeleteAll();
	WK_TRACE(_T("loading drive data\n"));
	// gf needed for update from old versions
	BOOL bOldVersion = CheckForOld30RegFormat(wkp);
	Load(wkp);
	CheckSpace();
	// if it was an old version
	if (bOldVersion) 
	{	// save to write new version
		Save(wkp);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDrives::CheckSpace()
{
	int i,c;

	Lock();
	c = GetSize();
	for (i=0;i<c;i++)
	{
		GetAtFast(i)->CheckSpace();
	}
	Unlock();
}
//////////////////////////////////////////////////////////////////////
void CIPCDrives::FromArray(int iNumDrives, const CIPCDrive drives[])
{

	Lock();
	DeleteAll();

	for (int i=0;i<iNumDrives;i++)
	{
		Add(new CIPCDrive(drives[i]));
	}

	Unlock();
}
//////////////////////////////////////////////////////////////////////
DWORD CIPCDrives::GetTotalHDSizeMB()
{
	DWORD dwMB = 0;
	CIPCDrive* pDrive;

	Lock();
	for (int i=0 ; i<GetSize() ; i++)
	{
		pDrive = GetAtFast(i);
		if (   pDrive
			&& pDrive->IsEnabled()
			&& pDrive->IsCDROM() == FALSE
			&& pDrive->IsNetworkDrive() == FALSE)		
		{
			dwMB += pDrive->GetMB();
		}
	}
	Unlock();
	return dwMB;
}
//////////////////////////////////////////////////////////////////////
DWORD CIPCDrives::GetMB()
{
	int i,c;
	DWORD dwMB = 0;
	CIPCDrive* pDrive;

	Lock();
	c = GetSize();
	for (i=0;i<c;i++)
	{
		pDrive = GetAtFast(i);
		if (   pDrive->IsEnabled() 
			&& (pDrive->IsDatabase() || pDrive->IsRemoveableDatabase()))
		{
			dwMB += pDrive->GetMB();
		}
	}
	Unlock();
	return dwMB;
}
//////////////////////////////////////////////////////////////////////
DWORD CIPCDrives::GetAvailableMB()
{
	int i,c;
	DWORD dwMB = 0;
	CIPCDrive* pDrive;

	Lock();
	c = GetSize();
	for (i=0;i<c;i++)
	{
		pDrive = GetAtFast(i);
		if (   pDrive->IsEnabled() 
			&& (pDrive->IsDatabase() || pDrive->IsRemoveableDatabase()))
		{
			dwMB += pDrive->GetAvailableMB();
		}
	}
	Unlock();
	return dwMB;
}
//////////////////////////////////////////////////////////////////////
CIPCInt64 CIPCDrives::GetUsedBytes()
{
	int i;
	CIPCInt64 iUsed = 0;
	CIPCDrive* pDrive;

	Lock();
	for (i=0;i<GetSize();i++)
	{
		pDrive = GetAtFast(i);
		if (   pDrive->IsEnabled() 
			&& (pDrive->IsDatabase() || pDrive->IsRemoveableDatabase()))
		{
			iUsed += pDrive->GetUsedBytes();
		}
	}
	Unlock();
	return iUsed;
}
//////////////////////////////////////////////////////////////////////
DWORD CIPCDrives::GetUsedMB()
{
	return GetUsedBytes().GetInMB();
}
//////////////////////////////////////////////////////////////////////
DWORD CIPCDrives::GetFreeMB()
{
	int i,c;
	DWORD dwFree = 0;
	CIPCDrive* pDrive;

	Lock();
	c = GetSize();
	for (i=0;i<c;i++)
	{
		pDrive = GetAtFast(i);
		if (   pDrive->IsEnabled() 
			&& (pDrive->IsDatabase() || pDrive->IsRemoveableDatabase()))
		{
			dwFree += pDrive->GetFreeMB();
		}
	}
	Unlock();
	return dwFree;
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCDrives::Load(CWK_Profile& wkp)
{
	Lock();
	// no Parameter
	DWORD dwLD = GetLogicalDrives();
	DWORD dwBit = 4;
	DWORD dwNetwork = 0;
	char c = 'c';
	_TCHAR sNetKey[] = _T("XN");
	_TCHAR sKey[] = _T("X");

	while (dwBit!=0)
	{
		if (dwLD & dwBit)
		{
			CIPCDrive*pNew = new CIPCDrive(c,wkp);
			SafeAdd(pNew);
			if (pNew->IsNetworkDrive())
			{
				dwNetwork |= dwBit;
			}
		}
		else
		{
			DWORD dwSize = 0;
			sNetKey[0] = c;
			wkp.GetType(szDrives, sNetKey, &dwSize);
			if (dwSize)
			{
				CIPCDrive*pNew = new CIPCDrive(c,wkp);
				SafeAdd(pNew);
			}
			sKey[0] = c;
			int nDriveUsedAs = wkp.GetInt(szDrives, sKey, DVR_INVALID_DRIVE);
			if (nDriveUsedAs == DVR_DB_DRIVE)
			{
				m_sFailedDrives += sKey;
			}
		}
		dwBit <<= 1;
		c++;
	}
	Unlock();
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
CIPCDrive* CIPCDrives::GetDrive(char c)
{
	int i;
	CIPCDrive* pDrive = NULL;
	Lock();
	for (i=0;i<GetSize();i++)
	{
		if (GetAtFast(i)->GetLetter()==c)
		{
			pDrive = GetAtFast(i);
			break;
		}
	}
	Unlock();
	if (pDrive && !pDrive->IsEnabled())
	{
		pDrive = NULL;
	}
	return pDrive;
}
//////////////////////////////////////////////////////////////////////
CIPCDrive* CIPCDrives::GetDrive(const CString& sRoot)
{
	int i;
	CIPCDrive* pDrive = NULL;
	Lock();
	for (i=0;i<GetSize();i++)
	{
		if (0==sRoot.CompareNoCase(GetAtFast(i)->GetRootString()))
		{
			pDrive = GetAtFast(i);
			break;
		}
	}
	Unlock();
	if (pDrive && !pDrive->IsEnabled())
	{
		pDrive = NULL;
	}
	return pDrive;
}
//////////////////////////////////////////////////////////////////////
void CIPCDrives::Trace()
{
	int i,c;

	Lock();
	c = GetSize();
	for (i=0;i<c;i++)
	{
		GetAtFast(i)->Trace();
	}
	Unlock();
}
//////////////////////////////////////////////////////////////////////
void CIPCDrives::Save(CWK_Profile& wkp)
{
	int i,c;

	Lock();
	wkp.DeleteSection(szDrives);
	c = GetSize();
	for (i=0;i<c;i++)
	{
		GetAtFast(i)->Save(wkp);
	}
	Unlock();
}
//////////////////////////////////////////////////////////////////////
CIPCExtraMemory* CIPCDrives::BubbleFromDrives(CIPC *pCipc) const
{
	// CString		m_sRoot;
	// 4 UINT		m_uSystemType;
	// 4 DvrDriveUse	m_UsedAs;
	// 4 DWORD		m_dwQuota;

	// 8 ULARGE_INTEGER m_uFreeBytesAvailableToCaller;
	// 8 ULARGE_INTEGER m_uTotalNumberOfBytes;
	// 8 ULARGE_INTEGER m_uTotalNumberOfFreeBytes;

	// CString m_sVolume;
	// CString	m_sFileSystemNameBuffer; 
	// 4 DWORD	m_dwClusterSize;
	int i;
	DWORD dwSum = 4;
#ifdef _UNICODE
	BOOL bUnicode = pCipc->GetCodePage() == CODEPAGE_UNICODE;
#endif
	CIPCDrives*pThis = (CIPCDrives*)this;
	pThis->Lock();
	// calc size of bubble
	for (i=0;i<GetSize();i++) 
	{
		dwSum +=  4 * sizeof(DWORD)
				+ 3 * sizeof(ULARGE_INTEGER)
				+ 3 * sizeof(WORD)
#ifdef _UNICODE
				+ CWK_String(GetAtFast(i)->m_sRoot).CopyToMemory(bUnicode, NULL, -1, pCipc->GetCodePage())
				+ CWK_String(GetAtFast(i)->m_sVolume).CopyToMemory(bUnicode, NULL, -1, pCipc->GetCodePage())
				+ CWK_String(GetAtFast(i)->m_sFileSystem).CopyToMemory(bUnicode, NULL, -1, pCipc->GetCodePage());
#else
				+ GetAtFast(i)->m_sRoot.GetLength()
				+ GetAtFast(i)->m_sVolume.GetLength()
				+ GetAtFast(i)->m_sFileSystem.GetLength();
#endif
	}
	CIPCExtraMemory *pBubble = new CIPCExtraMemory();

	if (pBubble->Create(pCipc,dwSum))
	{
		BYTE *pByte = (BYTE*) pBubble->GetAddressForWrite();

		CIPCExtraMemory::DWORD2Memory(pByte,(DWORD)GetSize());

		for (i=0;i<GetSize();i++) 
		{
#ifdef _UNICODE
			// root
			CIPCExtraMemory::CString2Memory(pByte, GetAtFast(i)->m_sRoot, pCipc->GetCodePage());
			// volume
			CIPCExtraMemory::CString2Memory(pByte, GetAtFast(i)->m_sVolume, pCipc->GetCodePage());
			// file system
			CIPCExtraMemory::CString2Memory(pByte, GetAtFast(i)->m_sFileSystem, pCipc->GetCodePage());
#else
			// root
			CIPCExtraMemory::CString2Memory(pByte,GetAtFast(i)->m_sRoot);
			// volume
			CIPCExtraMemory::CString2Memory(pByte,GetAtFast(i)->m_sVolume);
			// file system
			CIPCExtraMemory::CString2Memory(pByte,GetAtFast(i)->m_sFileSystem);
#endif
			// m_uSystemType
			CIPCExtraMemory::DWORD2Memory(pByte,(DWORD)GetAtFast(i)->m_uSystemType);
			// m_dwClusterSize
			CIPCExtraMemory::DWORD2Memory(pByte,(DWORD)GetAtFast(i)->m_dwClusterSize);
			// m_dwQuota
			CIPCExtraMemory::DWORD2Memory(pByte,(DWORD)GetAtFast(i)->m_dwQuota);
			// m_UsedAs
			CIPCExtraMemory::DWORD2Memory(pByte,(DWORD)GetAtFast(i)->m_UsedAs);
			// m_uFreeBytesAvailableToCaller;
			CIPCExtraMemory::ULARGE_INTEGER2Memory(pByte,
				GetAtFast(i)->m_uFreeBytesAvailableToCaller);
			// m_uTotalNumberOfBytes;
			CIPCExtraMemory::ULARGE_INTEGER2Memory(pByte,
				GetAtFast(i)->m_uTotalNumberOfBytes);
			// m_uTotalNumberOfFreeBytes;
			CIPCExtraMemory::ULARGE_INTEGER2Memory(pByte,
				GetAtFast(i)->m_uTotalNumberOfBytes);
		}

		DWORD dwDelta=pByte-(BYTE*)pBubble->GetAddressForWrite();
		WK_ASSERT(dwDelta==dwSum);
	}
	else
	{
		WK_DELETE(pBubble);
		WK_TRACE_ERROR(_T("Create CIPCExtraMemory failed in BubbleFromDrives\n"));
	}
	pThis->Unlock();
	return pBubble;
}
//////////////////////////////////////////////////////////////////////
CIPCDrive* CIPCDrives::DrivesFromBubble(int iNumRecords,
				  				        const CIPCExtraMemory *pExtraMem)
{
	// CString		m_sRoot;
	// 4 UINT		m_uSystemType;
	// 1 DvrDriveUse	m_UsedAs;
	// 4 DWORD		m_dwQuota;

	// 8 ULARGE_INTEGER m_uFreeBytesAvailableToCaller;
	// 8 ULARGE_INTEGER m_uTotalNumberOfBytes;
	// 8 ULARGE_INTEGER m_uTotalNumberOfFreeBytes;

	// CString m_sVolume;
	// CString	m_sFileSystemNameBuffer; 
	// 4 DWORD	m_dwClusterSize;

	const BYTE *pByte = (const BYTE*)pExtraMem->GetAddress();
	DWORD dw;

	dw = CIPCExtraMemory::Memory2DWORD(pByte);
	WK_ASSERT(dw==(DWORD)iNumRecords);

	if (iNumRecords==0)
	{
		return NULL;
	}

	CIPCDrive *pData = new CIPCDrive[iNumRecords];
	// loop over records
	for (int i=0;i<iNumRecords;i++) 
	{
		CIPCDrive& drive = pData[i];

#ifdef _UNICODE
		// root
		drive.m_sRoot = CIPCExtraMemory::Memory2CString(pByte, pExtraMem->GetCIPC()->GetCodePage());
		// volume
		drive.m_sVolume = CIPCExtraMemory::Memory2CString(pByte, pExtraMem->GetCIPC()->GetCodePage());
		// file system
		drive.m_sFileSystem = CIPCExtraMemory::Memory2CString(pByte, pExtraMem->GetCIPC()->GetCodePage());
#else
		// root
		drive.m_sRoot = CIPCExtraMemory::Memory2CString(pByte);
		// volume
		drive.m_sVolume = CIPCExtraMemory::Memory2CString(pByte);
		// file system
		drive.m_sFileSystem = CIPCExtraMemory::Memory2CString(pByte);
#endif
		// m_uSystemType
		drive.m_uSystemType = CIPCExtraMemory::Memory2DWORD(pByte);
		// m_dwClusterSize
		drive.m_dwClusterSize = CIPCExtraMemory::Memory2DWORD(pByte);
		// m_dwQuota
		drive.m_dwQuota = CIPCExtraMemory::Memory2DWORD(pByte);
		// m_UsedAs
		drive.m_UsedAs = (DvrDriveUse)CIPCExtraMemory::Memory2DWORD(pByte);
		// m_uFreeBytesAvailableToCaller;
		drive.m_uFreeBytesAvailableToCaller = CIPCExtraMemory::Memory2ULARGE_INTEGER(pByte);
		// m_uTotalNumberOfBytes;
		drive.m_uTotalNumberOfBytes = CIPCExtraMemory::Memory2ULARGE_INTEGER(pByte);
		// m_uTotalNumberOfFreeBytes;
		drive.m_uTotalNumberOfFreeBytes = CIPCExtraMemory::Memory2ULARGE_INTEGER(pByte);
	}

	return pData;
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCDrives::CheckForOld30RegFormat(CWK_Profile& wkp)
{
	BOOL bReturn = FALSE;
	Lock();
	CString sDriveToExclude = wkp.GetString(szDrives, szExcludedDrives, _T(""));
	if (!sDriveToExclude.IsEmpty())
	{
		bReturn = TRUE;
		DWORD dwLD = GetLogicalDrives();
		// at old versions drive c is never used for database!
		// must start at d
		DWORD dwBit = 8;
		_TCHAR c = 'D';
		CString sKey;
		
		while (dwBit!=0)
		{
			if (dwLD & dwBit)
			{
				if (sDriveToExclude.Find(c) == -1)
				{
					// drive should be used as database
					sKey = c;
					wkp.WriteInt(szDrives, sKey, DVR_DB_DRIVE);
				}
				else
				{
					// drive should NOT be used
					// this is the standard if not found
//					sKey = c;
//					wkp.WriteInt(szDrives, sKey, DVR_NORMAL_DRIVE);
				}
			}
			dwBit <<= 1;
			c++;
		}
	}
	Unlock();
	return bReturn;
}
//////////////////////////////////////////////////////////////////////
const CString& CIPCDrives::GetFailedDrives() const
{
	return m_sFailedDrives;
}
