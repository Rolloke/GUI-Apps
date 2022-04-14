/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCDrive.cpp $
// ARCHIVE:		$Archive: /Project/CIPC/CIPCDrive.cpp $
// CHECKIN:		$Date: 2.08.06 20:55 $
// VERSION:		$Revision: 53 $
// LAST CHANGE:	$Modtime: 2.08.06 20:52 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdcipc.h"
#include "CIPCDrive.h"

static const TCHAR szDrives[] = _T("Drives");
// kernel32 functions
typedef BOOL (APIENTRY* GET_DISK_FREE_SPACE_EX_PTR)(LPCTSTR,PULARGE_INTEGER,PULARGE_INTEGER,PULARGE_INTEGER);

HMODULE g_hKernel32 = NULL;
GET_DISK_FREE_SPACE_EX_PTR g_pGetDiskFreeSpaceEx = NULL;

#if _MFC_VER >= 0x0710
// MPR Network functions
typedef DWORD (APIENTRY*WNETADDCONNECTION2_FNC_PTR)(LPNETRESOURCE lpNetResource, LPCTSTR lpPassword, LPCTSTR lpUsername, DWORD dwFlags);
typedef DWORD (APIENTRY*WNETCANCELCONNECTION2_FNC_PTR)(LPCTSTR lpName, DWORD dwFlags, BOOL fForce);
typedef DWORD (APIENTRY*WNETGETCONNECTION_FNC_PTR)(LPCTSTR lpLocalName,LPTSTR lpRemoteName,LPDWORD lpnLength);

HMODULE g_hMPR = NULL;
WNETADDCONNECTION2_FNC_PTR		g_pWNetAddConnection2    = NULL;
WNETCANCELCONNECTION2_FNC_PTR	g_pWNetCancelConnection2 = NULL;
WNETGETCONNECTION_FNC_PTR		g_pWNetGetConnection     = NULL;

// NetApi functions
typedef NET_API_STATUS (NET_API_FUNCTION*NET_USE_ADD_FNC_PTR)(LPWSTR UncServerName, DWORD Level, LPBYTE Buf, LPDWORD ParmError);
typedef NET_API_STATUS (NET_API_FUNCTION*NET_USE_DEL_FNC_PTR)(LPWSTR UncServerName, LPWSTR UseName, DWORD ForceCond);
typedef NET_API_STATUS (NET_API_FUNCTION*NET_USE_GETINFO_FNC_PTR)(LPWSTR UncServerName, LPWSTR UseName, DWORD Level, LPBYTE* BufPtr);
typedef NET_API_STATUS (NET_API_FUNCTION*NET_API_BUFFER_FREE_FNC_PTR)(LPVOID Buffer);

HMODULE g_hNetApi32 = NULL;
NET_USE_ADD_FNC_PTR			g_pNetUseAdd = NULL;
NET_USE_DEL_FNC_PTR			g_pNetUseDel = NULL;
NET_USE_GETINFO_FNC_PTR		g_pNetUseGetInfo = NULL;
NET_API_BUFFER_FREE_FNC_PTR	g_pNetApiBufferFree = NULL;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIPCDrive::CIPCDrive()
{
	Init();
}
//////////////////////////////////////////////////////////////////////
CIPCDrive::CIPCDrive(const CIPCDrive& source)
{
	m_sRoot = source.m_sRoot;
	m_uSystemType = source.m_uSystemType;
	m_UsedAs = source.m_UsedAs;
	m_dwQuota = source.m_dwQuota;

	m_uFreeBytesAvailableToCaller = source.m_uFreeBytesAvailableToCaller;
	m_uTotalNumberOfBytes = source.m_uTotalNumberOfBytes;
	m_uTotalNumberOfFreeBytes = source.m_uTotalNumberOfFreeBytes;

	m_sVolume = source.m_sVolume;
	m_sFileSystem = source.m_sFileSystem;
	m_dwClusterSize = source.m_dwClusterSize;

	m_bMCIDeviceClosed = source.m_bMCIDeviceClosed;
	m_MCIDeviceID = source.m_MCIDeviceID;
}
//////////////////////////////////////////////////////////////////////
BOOL GetDiskFreeSpaceMap(LPCTSTR lpDirectoryName,
						 PULARGE_INTEGER lpFreeBytesAvailableToCaller,
						 PULARGE_INTEGER lpTotalNumberOfBytes,
						 PULARGE_INTEGER lpTotalNumberOfFreeBytes)
{
	DWORD	dwSectorsPerCluster		=	0;
    DWORD	dwBytesPerSector		=	0;
    DWORD	dwNumberOfFreeClusters	=	0;
    DWORD	dwTotalNumberOfClusters	=	0;

	if (GetDiskFreeSpace( lpDirectoryName, 
						  &dwSectorsPerCluster, 
						  &dwBytesPerSector, 
						  &dwNumberOfFreeClusters, 
						  &dwTotalNumberOfClusters ))
	{
		// total
		__int64 iTemp = (__int64)dwSectorsPerCluster * (__int64)dwBytesPerSector;
		lpTotalNumberOfBytes->QuadPart =  (__int64)dwTotalNumberOfClusters * iTemp; 
		// free
		lpFreeBytesAvailableToCaller->QuadPart = (__int64)dwNumberOfFreeClusters * iTemp;
		lpTotalNumberOfFreeBytes->QuadPart = lpFreeBytesAvailableToCaller->QuadPart;
		// used
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCDrive::InitNetworkApi()
{
#if _MFC_VER >= 0x0710
	if (g_hMPR == NULL)
	{
		g_hMPR = LoadLibrary(_T("MPR.dll"));
		if (g_hMPR) 
		{
#ifdef _UNICODE
			g_pWNetAddConnection2    = (WNETADDCONNECTION2_FNC_PTR)    GetProcAddress(g_hMPR, "WNetAddConnection2W");
			g_pWNetCancelConnection2 = (WNETCANCELCONNECTION2_FNC_PTR) GetProcAddress(g_hMPR, "WNetCancelConnection2W");
			g_pWNetGetConnection     = (WNETGETCONNECTION_FNC_PTR)     GetProcAddress(g_hMPR, "WNetGetConnectionW");
#else
			g_pWNetAddConnection2    = (WNETADDCONNECTION2_FNC_PTR)    GetProcAddress(g_hMPR, "WNetAddConnection2A");
			g_pWNetCancelConnection2 = (WNETCANCELCONNECTION2_FNC_PTR) GetProcAddress(g_hMPR, "WNetCancelConnection2A");
			g_pWNetGetConnection     = (WNETGETCONNECTION_FNC_PTR)     GetProcAddress(g_hMPR, "WNetGetConnectionA");
#endif // _UNICODE
		}
	}
	if (g_hNetApi32 == NULL)
	{
		g_hNetApi32 = LoadLibrary(_T("NetApi32.dll"));
		if (g_hNetApi32)
		{
			g_pNetUseAdd		= (NET_USE_ADD_FNC_PTR)			GetProcAddress(g_hNetApi32, "NetUseAdd");
			g_pNetUseDel		= (NET_USE_DEL_FNC_PTR)			GetProcAddress(g_hNetApi32, "NetUseDel");
			g_pNetUseGetInfo	= (NET_USE_GETINFO_FNC_PTR)		GetProcAddress(g_hNetApi32, "NetUseGetInfo");
			g_pNetApiBufferFree = (NET_API_BUFFER_FREE_FNC_PTR)	GetProcAddress(g_hNetApi32, "NetApiBufferFree");
		}
	}
#endif // _MFC_VER >= 0x0710

	if (g_hKernel32 == NULL)
	{
		g_hKernel32 = LoadLibrary(_T("KERNEL32.DLL"));

		if (g_hKernel32)
		{
#ifdef _UNICODE
			FARPROC theProc = ::GetProcAddress(g_hKernel32, "GetDiskFreeSpaceExW");
#else
			FARPROC theProc = ::GetProcAddress(g_hKernel32, "GetDiskFreeSpaceExA");
#endif // _UNICODE
			if (theProc)
			{
				g_pGetDiskFreeSpaceEx = (GET_DISK_FREE_SPACE_EX_PTR)theProc;
			}
		}
		else
		{
			WK_TRACE(_T("Cannot Get Module Kernel32.dll\n"));
		}
	}
#if _MFC_VER >= 0x0710
	return (g_hMPR != NULL || g_hNetApi32 != NULL) ? TRUE : FALSE;
#else
	return TRUE;
#endif // _MFC_VER >= 0x0710
}
//////////////////////////////////////////////////////////////////////
void CIPCDrive::FreeNetworkApi()
{
#if _MFC_VER >= 0x0710
	if (g_hMPR)
	{
		FreeLibrary(g_hMPR);
		g_hMPR                   = NULL;
		g_pWNetAddConnection2    = NULL;
		g_pWNetCancelConnection2 = NULL;
		g_pWNetGetConnection     = NULL;
	}
	if (g_hNetApi32)
	{
		FreeLibrary(g_hNetApi32);
		g_hNetApi32         = NULL;
		g_pNetUseAdd        = NULL;
		g_pNetUseDel        = NULL;
		g_pNetUseGetInfo    = NULL;
		g_pNetApiBufferFree = NULL;
	}
#endif // _MFC_VER >= 0x0710
	if (g_hKernel32)
	{
		FreeLibrary(g_hKernel32);
		g_hKernel32 = NULL;
		g_pGetDiskFreeSpaceEx = NULL;
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDrive::Init()
{
	m_uFreeBytesAvailableToCaller.QuadPart = 0;
	m_uTotalNumberOfBytes.QuadPart = 0;
	m_uTotalNumberOfFreeBytes.QuadPart = 0;
	m_dwClusterSize = 0;
	m_uSystemType = DRIVE_UNKNOWN;
	m_UsedAs = DVR_NORMAL_DRIVE;
	m_dwClusterSize = 0;
	m_dwQuota = 0;
	m_bMCIDeviceClosed=TRUE;
	m_MCIDeviceID = 0;
}
//////////////////////////////////////////////////////////////////////
CIPCDrive::CIPCDrive(_TCHAR cDriveLetter, CWK_Profile& wkp)
{
	Init();
	m_sRoot.Format(_T("%c:\\"),cDriveLetter);
	m_sRoot.MakeLower();
	m_uSystemType = GetDriveType(m_sRoot);

	CString s;
	s = m_sRoot.Left(1);
	s.MakeUpper();
	
	m_UsedAs = (DvrDriveUse)wkp.GetInt(szDrives, s, DVR_NORMAL_DRIVE);

	m_dwQuota = wkp.GetInt(szDrives, s+_T("Q"), 0);

	DWORD dwSize = 0, dwType = wkp.GetType(szDrives, s+_T("N"), &dwSize);
	if (dwSize && dwType == REG_BINARY)
	{
		int nStrLen;
#ifdef _UNICODE
		nStrLen =  dwSize / sizeof(_TCHAR);
#else
		nStrLen =  dwSize;
#endif // _UNICODE
		LPTSTR sBuffer = m_sVolume.GetBufferSetLength(nStrLen+1);
		wkp.GetBinary(szDrives, s+_T("N"), (BYTE**)&sBuffer, (UINT*)&dwSize);
		sBuffer[nStrLen] = 0;
		m_sVolume.ReleaseBuffer();
		m_uSystemType = DRIVE_REMOTE;
	}

	Init2();
}
//////////////////////////////////////////////////////////////////////
CIPCDrive::CIPCDrive(const CString sRoot, DvrDriveUse ddu)
{
	Init();
	m_sRoot = sRoot;
	m_sRoot.MakeLower();
	m_UsedAs = ddu;
	m_uSystemType = GetDriveType(m_sRoot);
	Init2();
}
//////////////////////////////////////////////////////////////////////
void CIPCDrive::Init2()
{
	DWORD dwVolumeSerialNumber;
	DWORD dwMaximumComponentLength;
	DWORD dwFileSystemFlags;
	CString sVolume;
	BOOL bNetworkDrive = IsNetworkDrive();
	BOOL bConnected = TRUE;
	BOOL bResult = FALSE;
	if (bNetworkDrive)
	{
#if _MFC_VER >= 0x0710
		UpdateNetworkState();
#endif // _MFC_VER >= 0x0710
		bConnected = IsConnected();
	}
	if (bConnected)
	{
		bResult = GetVolumeInformation(m_sRoot,
							 sVolume.GetBuffer(_MAX_PATH),
							 _MAX_PATH,
							 &dwVolumeSerialNumber, 
							 &dwMaximumComponentLength,
							 &dwFileSystemFlags,
							 m_sFileSystem.GetBuffer(_MAX_PATH),
							 _MAX_PATH);
		m_sFileSystem.ReleaseBuffer();
		sVolume.ReleaseBuffer();
	}

	if (bResult)
	{
		CalculateClusterSize();
	}

	if (!bNetworkDrive)
	{
		m_sVolume = sVolume;
 	}
}
//////////////////////////////////////////////////////////////////////
CIPCDrive::~CIPCDrive()
{

}
//////////////////////////////////////////////////////////////////////
void CIPCDrive::CalculateClusterSize()
{
	DWORD	dwSectorsPerCluster		=	0;
    DWORD	dwBytesPerSector		=	0;
    DWORD	dwNumberOfFreeClusters	=	0;
    DWORD	dwTotalNumberOfClusters	=	0;

	if (GetDiskFreeSpace( GetRootString(), 
						  &dwSectorsPerCluster, 
						  &dwBytesPerSector, 
						  &dwNumberOfFreeClusters, 
						  &dwTotalNumberOfClusters ))
	{
		m_dwClusterSize = dwBytesPerSector * dwSectorsPerCluster;
	}
	else
	{
		CheckSpace();

		DWORD dwMB = GetMB();

		if (0==m_sFileSystem.CompareNoCase(_T("fat")))
		{
			if (dwMB >= 8 * 1024) // 8 GB NT 4.0 only
			{
				m_dwClusterSize = 256 * 1024;
			}
			else if (dwMB >= 4 * 1024) // 4 GB NT 4.0 only
			{
				m_dwClusterSize = 128 * 1024;
			}
			else if (dwMB >= 2 * 1024) // 2 GB
			{
				m_dwClusterSize = 64 * 1024;
			}
			else if (dwMB >= 1 * 1024) // 1 GB
			{
				m_dwClusterSize = 32 * 1024;
			}
			else if (dwMB >= 512) // 512 MB
			{
				m_dwClusterSize = 16 * 1024;
			}
			else if (dwMB >= 256) // 256 MB
			{
				m_dwClusterSize = 8 * 1024;
			}
			else if (dwMB >= 128) // 128 MB
			{
				m_dwClusterSize = 4 * 1024;
			}
			else
			{
				m_dwClusterSize = 2 * 1024;
			}
		}
		else if (0==m_sFileSystem.CompareNoCase(_T("ntfs")))
		{
			// NT only
			if (dwMB >= 32 * 1024) // 32 GB
			{
				m_dwClusterSize = 64 * 1024;
			}
			else if (dwMB >= 16 * 1024) // 16 GB
			{
				m_dwClusterSize = 32 * 1024;
			}
			else if (dwMB >= 8 * 1024) // 8 GB
			{
				m_dwClusterSize = 16 * 1024;
			}
			else if (dwMB >= 4 * 1024) // 4 GB
			{
				m_dwClusterSize = 8 * 1024;
			}
			else if (dwMB >= 2 * 1024) // 2 GB
			{
				m_dwClusterSize = 4 * 1024;
			}
			else if (dwMB >= 1 * 1024) // 1 GB
			{
				m_dwClusterSize = 2 * 1024;
			}
			else if (GetMB() >= 512) // 512 MB
			{
				m_dwClusterSize = 1 * 1024;
			}
			else
			{
				m_dwClusterSize = 512;
			}
		}
		else if (0==m_sFileSystem.CompareNoCase(_T("fat32")))
		{
			// ??
			if (dwMB >= 32 * 1024) // > 32 GB
			{
				m_dwClusterSize = 32 * 1024;
			}
			else if (dwMB >= 16 * 1024) // > 16 GB
			{
				m_dwClusterSize = 16 * 1024;
			}
			else if (dwMB >= 8 * 1024) // > 8 GB
			{
				m_dwClusterSize = 8 * 1024;
			}
			else // < 8 GB
			{
				m_dwClusterSize = 4 * 1024;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDrive::Lock()
{
	m_cs.Lock();
}
//////////////////////////////////////////////////////////////////////
void CIPCDrive::Unlock()
{
	m_cs.Unlock();
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCDrive::CheckSpace()
{
	BOOL bRet = FALSE;

	Lock();
	if (g_pGetDiskFreeSpaceEx)
	{
		if (IsNetworkDrive() && !IsConnected())
		{
			m_uFreeBytesAvailableToCaller.QuadPart = 0;
			m_uTotalNumberOfBytes.QuadPart = 0;
			m_uTotalNumberOfFreeBytes.QuadPart = 0;
			bRet = FALSE;
		}
		else if (IsEnabled()
			     && g_pGetDiskFreeSpaceEx(m_sRoot,
								  &m_uFreeBytesAvailableToCaller,
								  &m_uTotalNumberOfBytes,
								  &m_uTotalNumberOfFreeBytes))
		{
			bRet = TRUE;
		}
		else
		{
			m_uFreeBytesAvailableToCaller.QuadPart = 0;
			m_uTotalNumberOfBytes.QuadPart = 0;
			m_uTotalNumberOfFreeBytes.QuadPart = 0;
			bRet = FALSE;
		}
	}
	else
	{
		if (GetDiskFreeSpaceMap(m_sRoot,
								&m_uFreeBytesAvailableToCaller,
								&m_uTotalNumberOfBytes,
								&m_uTotalNumberOfFreeBytes))
		{
			bRet = TRUE;
		}
		else
		{
			m_uFreeBytesAvailableToCaller.QuadPart = 0;
			m_uTotalNumberOfBytes.QuadPart = 0;
			m_uTotalNumberOfFreeBytes.QuadPart = 0;
			bRet = FALSE;
		}
	}
	Unlock();
	return bRet;
}
//////////////////////////////////////////////////////////////////////
CString CIPCDrive::Format()
{
	CString u;
	CString s;

	Lock();
	switch (m_UsedAs)
	{
	case DVR_INVALID_DRIVE:
		u = _T("undefined");
		break;
	case DVR_DB_DRIVE:
		u = _T("database");
		break;
	case DVR_WRITE_BACKUP_DRIVE:
		u = _T("backup");
		break;
	case DVR_READ_BACKUP_DRIVE:
		u = _T("read backup");
		break;
	case DVR_CDR_BACKUP_DRIVE:
		u = _T("cd backup");
		break;
	case DVR_SYSTEM_DRIVE:
		u = _T("system");
		break;
	case DVR_NORMAL_DRIVE:
		u = _T("normal");
		break;
	case DVR_REMOVABLE_DB_DRIVE:
		u = _T("removable database");
		break;
	}
	s.Format(_T("%s,Volume:<%s>,<%s> %d Cluster %s %d/%d MB"),
		m_sRoot,m_sVolume,m_sFileSystem,
		m_dwClusterSize,u,GetUsedMB(),GetMB());

	if (!IsEnabled())
	{
		s += _T(", disabled");
	}
	if (!IsConnected())
	{
		s += _T(", disconnected");
	}


	Unlock();

	return s;
}
//////////////////////////////////////////////////////////////////////
void CIPCDrive::Trace()
{
	WK_TRACE(Format()+'\n');
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCDrive::IsSystem() const
{
	CString s;

	GetSystemDirectory(s.GetBuffer(MAX_PATH),_MAX_PATH);
	s.ReleaseBuffer();
	s.MakeLower();
	return s.GetAt(0) == m_sRoot.GetAt(0);
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCDrive::IsEraseable() const
{
	if (GetType() == DRIVE_REMOVABLE)
	{
		return TRUE;
	}
	if (GetType() == DRIVE_FIXED)
	{
		return TRUE;
	}
	if (GetType() == DRIVE_CD_RW)
	{
		return TRUE;
	}
	if (GetType() == DRIVE_DVD_RAM)
	{
		return TRUE;
	}
	if (GetType() == DRIVE_DVD_RW)
	{
		return TRUE;
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCDrive::IsWritableRemovableDisk() const
{
	if (GetType() == DRIVE_REMOVABLE)
	{
		return TRUE;
	}
	if (GetType() == DRIVE_CD_R)
	{
		return TRUE;
	}
	if (GetType() == DRIVE_CD_RW)
	{
		return TRUE;
	}
	if (GetType() == DRIVE_DVD_RAM)
	{
		return TRUE;
	}
	if (GetType() == DRIVE_DVD_R)
	{
		return TRUE;
	}
	if (GetType() == DRIVE_DVD_RW)
	{
		return TRUE;
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
void CIPCDrive::SetType(UINT uType)
{
	m_uSystemType = uType;
}
//////////////////////////////////////////////////////////////////////
void CIPCDrive::Enable(BOOL bEnable)
{
	if (bEnable)
	{
		m_uSystemType &= ~DRIVE_DISABLED;
	}
	else
	{
		m_uSystemType |=  DRIVE_DISABLED;
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDrive::SetFreeBytesAvailableToCaller(ULARGE_INTEGER uBytes)
{
	m_uFreeBytesAvailableToCaller.QuadPart = uBytes.QuadPart;
}
//////////////////////////////////////////////////////////////////////
void CIPCDrive::SetTotalNumberOfBytes(ULARGE_INTEGER uBytes)
{
	m_uTotalNumberOfBytes.QuadPart = uBytes.QuadPart;
}
//////////////////////////////////////////////////////////////////////
void CIPCDrive::SetTotalNumberOfFreeBytes(ULARGE_INTEGER uBytes)
{
	m_uTotalNumberOfFreeBytes.QuadPart = uBytes.QuadPart;
}
//////////////////////////////////////////////////////////////////////
void CIPCDrive::SetVolume(const CString& sVolume)
{
	m_sVolume = sVolume;
}
//////////////////////////////////////////////////////////////////////
void CIPCDrive::SetFileSystem(const CString& sFileSystem)
{
	m_sFileSystem = sFileSystem;
}
//////////////////////////////////////////////////////////////////////
void CIPCDrive::SetClusterSize(DWORD dwClusterSize)
{
	m_dwClusterSize = dwClusterSize;
}
//////////////////////////////////////////////////////////////////////
void CIPCDrive::SetUsedAs(DvrDriveUse newUse)
{
	if (m_UsedAs != newUse)
	{
		m_UsedAs = newUse;
		CheckSpace();
		Init2();
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCDrive::SetQuota(DWORD dwQuotaMB)
{
	if (dwQuotaMB <= GetMB())
	{
		m_dwQuota = dwQuotaMB;
		return FALSE;
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
void CIPCDrive::Save(CWK_Profile& wkp)
{
	CString s;
	s = m_sRoot.Left(1);
	s.MakeUpper();
	if (m_uSystemType == DRIVE_DELETED)
	{
		wkp.DeleteEntry(szDrives, s);
		wkp.DeleteEntry(szDrives, s+_T("Q"));
		wkp.DeleteEntry(szDrives, s+_T("N"));
	}
	else
	{
		wkp.WriteInt(szDrives,s,m_UsedAs);
	 
		if (   (IsDatabase() || IsRemoveableDatabase())
			&& m_dwQuota>0)
		{
			wkp.WriteInt(szDrives, s+_T("Q"),m_dwQuota);
		}

		if (IsNetworkDrive())
		{
			wkp.WriteBinary(szDrives, s+_T("N"), (LPBYTE)LPCTSTR(m_sVolume), m_sVolume.GetLength() * sizeof(_TCHAR));
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL SearchNeroBurningROM(CString& sVersion)
{
	CString sNeroApi;
	CString sPath;

	sNeroApi = _T("%PROGRAMFILES%\\ahead\\nero\\NeroAPI.dll");

	ExpandEnvironmentStrings(sNeroApi,sPath.GetBufferSetLength(_MAX_PATH),_MAX_PATH);
	sPath.ReleaseBuffer();

	if (DoesFileExist(sPath))
	{
		sVersion = WK_GetFileVersion(sPath);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
//////////////////////////////////////////////////////////////////////
FS_Producer CIPCDrive::GetFSProducer() const
{
	CString sVersion;

	switch (LOWORD(m_uSystemType))
	{
	case DRIVE_UNKNOWN:
		// The drive type cannot be determined. 
		return FSP_INVALID;
		break;
	case DRIVE_NO_ROOT_DIR:
		// The root directory does not exist. 
		return FSP_INVALID;
		break;
	case DRIVE_REMOVABLE:
		// The disk can be removed from the drive. 
		if (   (m_sRoot[0] == 'a') || (m_sRoot[0] == 'A') 
			|| (m_sRoot[0] == 'b') || (m_sRoot[0] == 'B'))
		{
			return FSP_FLOPPYDISK;
		}
		else
		{
			return FSP_IOMEGA_JAZ;
		}
		break;
	case DRIVE_FIXED:
		// The disk cannot be removed from the drive. 
		return FSP_HARDDISK;
		break;
	case DRIVE_REMOTE:
		// The drive is a remote (network) drive. 
		return FSP_HARDDISK;
		break;
	case DRIVE_CDROM:
		// The drive is a CD-ROM drive. 
		if (SearchDirectCDDriver(sVersion))
		{
			return FSP_UDF_ADAPTEC_DIRECTCD;
		}
		else if (SearchPacketCDDriver(sVersion))
		{
			return FSP_UDF_CEQUADRAT_PACKETCD;
		}
		else if (SearchNeroBurningROM(sVersion))
		{
			return FSP_NERO_BURNING_ROM;
		}
		else
		{
			return FSP_INVALID;
		}
		break;
	case DRIVE_RAMDISK:
		// The drive is a RAM disk
		return FSP_HARDDISK;
		break;
	};
	return FSP_INVALID;
}
//////////////////////////////////////////////////////////////////////
CString CIPCDrive::GetFSVersion() const
{
	CString sVersion;
	if (SearchDirectCDDriver(sVersion))
	{
		return sVersion;
	}
	else if (SearchPacketCDDriver(sVersion))
	{
		return sVersion;
	}
	return sVersion;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIPCDrive::SearchPacketCDDriver(CString& sVersion) const
{
	OSVERSIONINFO osinfo;
	_TCHAR szSystem[_MAX_PATH];

	memset(&osinfo,sizeof(osinfo),0);
	osinfo.dwOSVersionInfoSize = sizeof(osinfo);
	GetVersionEx(&osinfo);
	GetSystemDirectory(szSystem, _MAX_PATH);

	if (osinfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		// Win95/98
		// Win95/98
		CString sDriver;
		sDriver = szSystem;
		sDriver += _T("\\iosubsys\\c2udffs.vxd");
		WK_TRACE(_T("searching PacketCD driver %s\n"), sDriver);
		if (DoesFileExist(sDriver))
		{
			sVersion = WK_GetFileVersion(sDriver);
			WK_TRACE(_T("9x PacketCD driver found %s, %s\n"), sDriver, sVersion);
			return TRUE;
		}
	}
	else if (osinfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		// Win NT
		CString sDriver;
		sDriver = szSystem;
		sDriver += _T("\\drivers\\c2udfmmc.sys");
		WK_TRACE(_T("searching PacketCD driver %s\n"), sDriver);
		if (DoesFileExist(sDriver))
		{
			sVersion = WK_GetFileVersion(sDriver);
			WK_TRACE(_T("NT PacketCD driver found %s, %s\n"), sDriver, sVersion);
			return TRUE;
		}
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIPCDrive::SearchDirectCDDriver(CString& sVersion) const
{
	OSVERSIONINFO osinfo;
	_TCHAR szSystem[_MAX_PATH];

	memset(&osinfo,sizeof(osinfo),0);
	osinfo.dwOSVersionInfoSize = sizeof(osinfo);
	GetVersionEx(&osinfo);
	GetSystemDirectory(szSystem, _MAX_PATH);

	if (osinfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		// Win95/98
		CString sDriver;
		sDriver = szSystem;
		sDriver += _T("\\iosubsys\\cdudf.vxd");
		WK_TRACE(_T("searching DirectCD driver %s\n"),sDriver);
		if (DoesFileExist(sDriver))
		{
			sVersion = WK_GetFileVersion(sDriver);
			WK_TRACE(_T("9x DirectCD driver found %s, %s\n"),sDriver,sVersion);
			return TRUE;
		}
	}
	else if (osinfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		// Win NT
		CString sDriver;
		sDriver = szSystem;
		sDriver += _T("\\drivers\\cdudf.sys");
		WK_TRACE(_T("searching DirectCD driver %s\n"), sDriver);
		if (DoesFileExist(sDriver))
		{
			sVersion = WK_GetFileVersion(sDriver);
			WK_TRACE(_T("NT DirectCD driver found %s, %s\n"), sDriver, sVersion);
			return TRUE;
		}
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCDrive::OpenCD()
{
	DWORD dwReturn;

	if(!IsCDROM())
	{
		WK_TRACE(_T("OpenCD: No CDROM\n"));
		return FALSE;
	}
	if(m_bMCIDeviceClosed)
	{
		if(!OpenMCICDROMDevice())
		{
			WK_TRACE(_T("OpenMCICDROMDevice failed\n"));
			return FALSE;
		}
	}
	
	MCI_SET_PARMS mciSetParms;

	dwReturn = mciSendCommand(m_MCIDeviceID,
							  MCI_SET, 
							  MCI_SET_DOOR_OPEN|MCI_WAIT, 
							  (DWORD)&mciSetParms);

	if (dwReturn!=0)
	{
		CString sError;
		if (mciGetErrorString(dwReturn,sError.GetBuffer(_MAX_PATH),_MAX_PATH))
		{
			sError.ReleaseBuffer();
			WK_TRACE(_T("OpenCD mciSendCommand failed %s\n"),sError);
		}
		else
		{
			sError.ReleaseBuffer();
			WK_TRACE(_T("OpenCD mciSendCommand failed %d\n"),LOWORD(dwReturn));
		}
	}
	return !dwReturn && CloseMCICDROMDevice();
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCDrive::OpenMCICDROMDevice()
{
	if(!m_bMCIDeviceClosed)
	{
		WK_TRACE(_T("OpenMCICDROMDevice :allready opened\n"));
		return FALSE;
	}

	_TCHAR cDriveletter;
	CString sDriveletter;

	cDriveletter = GetLetter();
	if(!cDriveletter)
	{
		WK_TRACE(_T("OpenMCICDROMDevice : No Driveletter\n"));
		return FALSE;
	}

	m_MCIDeviceID=0;
	MCI_OPEN_PARMS mciOpenParms;
	DWORD dwReturn;
	   
	mciOpenParms.lpstrDeviceType = _T("cdaudio");
	//mciOpenParms.lpstrElementName = sDriveletter;
	sDriveletter=(const _TCHAR)cDriveletter;
	sDriveletter=sDriveletter+_T(":");
	sDriveletter.MakeUpper();
	mciOpenParms.lpstrElementName = sDriveletter;

	dwReturn = mciSendCommand(NULL,MCI_OPEN, MCI_WAIT|MCI_OPEN_TYPE | MCI_OPEN_ELEMENT|MCI_WAIT, 
								  (DWORD)(LPVOID) &mciOpenParms);


	m_MCIDeviceID=mciOpenParms.wDeviceID;	
	WK_TRACE(_T("OpenMCICDROMDevice : MCIDeviceID=%lx\n"), m_MCIDeviceID);

	if (dwReturn==0)
	{
		m_bMCIDeviceClosed=FALSE;
		return TRUE;
	}
	else
	{
		CString sError;
		if (mciGetErrorString(dwReturn,sError.GetBuffer(_MAX_PATH),_MAX_PATH))
		{
			sError.ReleaseBuffer();
			WK_TRACE(_T("OpenMCI mciSendCommand failed %s\n"), sError);
		}
		else
		{
			sError.ReleaseBuffer();
			WK_TRACE(_T("OpenMCI mciSendCommand failed %d\n"), LOWORD(dwReturn));
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCDrive::CloseCD()
{
	DWORD dwReturn;

	if(!IsCDROM())
	{
		WK_TRACE(_T("OpenCD: No CDROM\n"));
		return FALSE;
	}
	if(m_bMCIDeviceClosed)
	{
		if(!OpenMCICDROMDevice())
		{
			WK_TRACE(_T("OpenMCICDROMDevice failed\n"));
			return FALSE;
		}
	}

	MCI_SET_PARMS mciSetParms;

	dwReturn=mciSendCommand(m_MCIDeviceID,
						    MCI_SET, 
							MCI_SET_DOOR_CLOSED|MCI_WAIT, 
							(DWORD) &mciSetParms);

	if (dwReturn!=0)
	{
		CString sError;
		if (mciGetErrorString(dwReturn,sError.GetBuffer(_MAX_PATH),_MAX_PATH))
		{
			sError.ReleaseBuffer();
			WK_TRACE(_T("CloseCD mciSendCommand failed %s\n"), sError);
		}
		else
		{
			sError.ReleaseBuffer();
			WK_TRACE(_T("Close CD mciSendCommand failed %d\n"), LOWORD(dwReturn));
		}
	}
	return !dwReturn && CloseMCICDROMDevice();	
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCDrive::CheckCD()
{
	MCI_STATUS_PARMS   mciStatusParms;

	if(!IsCDROM())
	{
		WK_TRACE(_T("OpenCD: No CDROM\n"));
		return FALSE;
	}
	if(m_bMCIDeviceClosed)
	{
		if(!OpenMCICDROMDevice())
		{
			WK_TRACE(_T("OpenMCICDROMDevice failed\n"));
			return FALSE;
		}
	}


	mciStatusParms.dwItem=(DWORD)MCI_STATUS_MEDIA_PRESENT;

	DWORD dwReturn = mciSendCommand(m_MCIDeviceID,
									MCI_STATUS,
									MCI_WAIT|MCI_STATUS_ITEM, 
									(DWORD)(LPMCI_STATUS_PARMS) &mciStatusParms);

	if (dwReturn!=0)
	{
		CString sError;
		if (mciGetErrorString(dwReturn,sError.GetBuffer(_MAX_PATH),_MAX_PATH))
		{
			sError.ReleaseBuffer();
			WK_TRACE(_T("mciSendCommand failed %s\n"), sError);
		}
		else
		{
			sError.ReleaseBuffer();
			WK_TRACE(_T("mciSendCommand failed %d\n"), LOWORD(dwReturn));
		}
	}
	return mciStatusParms.dwReturn && CloseMCICDROMDevice();	

}
//////////////////////////////////////////////////////////////////////
BOOL CIPCDrive::CloseMCICDROMDevice()
{
	DWORD dwReturn;
	MCI_GENERIC_PARMS  mciCloseParms;

	if(!IsCDROM())
	{
		WK_TRACE(_T("CloseMCICDROMDevice: No CDROM\n"));
		return FALSE;
	}
	if(m_bMCIDeviceClosed)
	{
		return TRUE;
	}

	dwReturn = mciSendCommand(m_MCIDeviceID, 
							MCI_CLOSE, 
							MCI_WAIT, 
							(DWORD)(LPVOID) &mciCloseParms);
	if (dwReturn==0)
	{
		m_bMCIDeviceClosed=TRUE;
		TRACE(_T("CDROM DEVICE CLOSED\n"));
		return TRUE;
	}
	else
	{
		CString sError;
		if (mciGetErrorString(dwReturn,sError.GetBuffer(_MAX_PATH),_MAX_PATH))
		{
			sError.ReleaseBuffer();
			WK_TRACE(_T("CloseMCI mciSendCommand failed %s\n"),sError);
		}
		else
		{
			sError.ReleaseBuffer();
			WK_TRACE(_T("CloseMCI mciSendCommand failed %d\n"),LOWORD(dwReturn));
		}
		WK_TRACE(_T("CloseMCICDROMDevice failed\n"));
		return FALSE;
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCDrive::CDDriveIsReady()
{
	MCI_STATUS_PARMS   mciStatusParms;
	BOOL bDriveStatus;


	if(!IsCDROM())
	{
		WK_TRACE(_T("OpenCD: No CDROM\n"));
		return FALSE;
	}
	if(m_bMCIDeviceClosed)
	{
		if(!OpenMCICDROMDevice())
		{
			WK_TRACE(_T("OpenMCICDROMDevice failed\n"));
			return FALSE;
		}
	}


	mciStatusParms.dwItem=(DWORD)MCI_STATUS_MODE;

	DWORD dwReturn = mciSendCommand(m_MCIDeviceID,
									MCI_STATUS, 
									MCI_WAIT|MCI_STATUS_ITEM , 
									(DWORD)(LPMCI_STATUS_PARMS) &mciStatusParms);
	
	if (dwReturn!=0)
	{
		CString sError;
		if (mciGetErrorString(dwReturn,sError.GetBuffer(_MAX_PATH),_MAX_PATH))
		{
			sError.ReleaseBuffer();
			WK_TRACE(_T("mciSendCommand failed %s\n"), sError);
		}
		else
		{
			sError.ReleaseBuffer();
			WK_TRACE(_T("mciSendCommand failed %d\n"), LOWORD(dwReturn));
		}
	}
	if(mciStatusParms.dwReturn==MCI_MODE_NOT_READY)
	{
		bDriveStatus=FALSE;
	}
	else if(mciStatusParms.dwReturn==MCI_MODE_OPEN)
	{
		bDriveStatus=FALSE;
	}
	else
	{
		bDriveStatus=TRUE;
	}

	return bDriveStatus && CloseMCICDROMDevice();
}
//////////////////////////////////////////////////////////////////////
#if _MFC_VER >= 0x0710
BOOL CIPCDrive::ConnectNetworkDrive()
{
	CStringArray sa;
	CString sLetter = m_sRoot.Left(2);
	CString sVolume, sDomain, sUser, sPassword;
	BOOL bUser = FALSE;
	SplitString(m_sVolume, sa, _T(';'));
	if (sa.GetSize() == 4)	// without a domain connection
	{						// all parameters are nessesary
		CString sPwd;
		sVolume = sa.GetAt(NETWORK_PATH);
		sDomain = sa.GetAt(NETWORK_DOMAIN);
		sDomain.TrimLeft();
		sUser	= sa.GetAt(NETWORK_USER);
		sPwd    = sa.GetAt(NETWORK_PASSWORD);
		Decode(sPwd);
		sPassword = sPwd;
		bUser = TRUE;
	}
	else					// within a domain 
	{						// the path is enough
		sVolume   = sa.GetAt(NETWORK_PATH);
	}

	if (g_pWNetAddConnection2)
	{
		LPCTSTR pszPassword = NULL, pszUserName = NULL;
		NETRESOURCE nr;
		ZERO_INIT(nr);

		nr.dwType = RESOURCETYPE_DISK;
		if (bUser)
		{
			if (!sDomain.IsEmpty())
			{
				sUser = sDomain + _T("\\") + sUser;
				pszUserName = sUser;
			}
			else
			{
				pszUserName = sUser;
			}
			pszPassword = sPassword;
		}
		nr.lpLocalName  = (LPTSTR) LPCTSTR(sLetter);
		nr.lpRemoteName = (LPTSTR) LPCTSTR(sVolume);
		DWORD dwRes = g_pWNetAddConnection2(&nr, pszPassword, pszUserName, 0);//CONNECT_UPDATE_PROFILE);
		if (dwRes == 0)
		{
			m_uSystemType &= ~DRIVE_NOT_CONNECTED;
			return TRUE;
		}
		return FALSE;
	}
	else if (g_pNetUseAdd)
	{
		USE_INFO_2 ui2;
		ZERO_INIT(ui2);
		if (bUser)
		{
			if (!sDomain.IsEmpty())
			{
#ifdef _UNICODE
				ui2.ui2_domainname = (LPWSTR)LPCWSTR(sDomain);
#else
				ui2.ui2_domainname = (LPWSTR)sDomain.AllocSysString();
#endif // _UNICODE
			}
#ifdef _UNICODE
			ui2.ui2_username = (LPWSTR)LPCWSTR(sUser);
			ui2.ui2_password = (LPWSTR)LPCWSTR(sPassword);
#else
			ui2.ui2_username = (LPWSTR)sUser.AllocSysString();
			ui2.ui2_password = (LPWSTR)sPassword.AllocSysString();
#endif // _UNICODE
		}
		else
		{
			sVolume = m_sVolume;
		}
#ifdef _UNICODE
		ui2.ui2_local  = (LPWSTR)LPCWSTR(sLetter);
		ui2.ui2_remote = (LPWSTR)LPCWSTR(sVolume);
#else
		ui2.ui2_local  = (LPWSTR)sLetter.AllocSysString();
		ui2.ui2_remote = (LPWSTR)sVolume.AllocSysString();
#endif // _UNICODE
		DWORD dwError = 0;

		NET_API_STATUS nas = g_pNetUseAdd(NULL, 2, (BYTE*)&ui2, &dwError);
#ifndef _UNICODE
		if (ui2.ui2_domainname) ::SysFreeString((BSTR)ui2.ui2_domainname);
		if (ui2.ui2_username) ::SysFreeString((BSTR)ui2.ui2_username);
		if (ui2.ui2_password) ::SysFreeString((BSTR)ui2.ui2_password);
		if (ui2.ui2_local) ::SysFreeString((BSTR)ui2.ui2_local);
		if (ui2.ui2_remote) ::SysFreeString((BSTR)ui2.ui2_remote);
#endif // _UNICODE

		if (nas == 0)
		{
			m_uSystemType &= ~DRIVE_NOT_CONNECTED;
			return TRUE;
		}
		else
		{
			WK_TRACE(_T("Error ConnectNetworkDrive(%s -> %s), nas: %x, err:%x\n"), m_sVolume, m_sRoot, nas, dwError);
			return FALSE;
		}
	}
	return ERROR_DLL_NOT_FOUND;
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCDrive::DisconnectNetworkDrive(DWORD dwForce/*=USE_LOTS_OF_FORCE*/)
{
	if (g_pWNetCancelConnection2)
	{
		BOOL bForce = dwForce != USE_NOFORCE ? TRUE : FALSE;
		DWORD dwResult = g_pWNetCancelConnection2(m_sRoot.Left(2), CONNECT_UPDATE_PROFILE, bForce);
		return (NO_ERROR == dwResult);
	}
	else if (g_pNetUseDel)
	{
		CString sRoot(m_sRoot.Left(2));
#ifdef _UNICODE
		NET_API_STATUS nas = g_pNetUseDel(NULL, (LPWSTR)LPCWSTR(sRoot), dwForce);
#else
		BSTR bstr = sRoot.AllocSysString();
		NET_API_STATUS nas = g_pNetUseDel(NULL, (LPWSTR)bstr, dwForce);
		::SysFreeString(bstr);
#endif // _UNICODE
		if (nas == 0)
		{
			m_uSystemType |= DRIVE_NOT_CONNECTED;
			return TRUE;
		}
		else
		{
			WK_TRACE(_T("Error DisconnectNetworkDrive(%s -> %s), nas: %x\n"), m_sVolume, m_sRoot, nas);
			return FALSE;
		}
	}
	return ERROR_DLL_NOT_FOUND;
}
//////////////////////////////////////////////////////////////////////
void CIPCDrive::UpdateNetworkState()
{
	DWORD dwResult = 1;
	CString	sVolume;
	if (g_pNetUseGetInfo && g_pNetApiBufferFree)
	{
		USE_INFO_2 *pui2 = NULL;
		dwResult = g_pNetUseGetInfo((LPWSTR)NULL, (LPWSTR)LPCTSTR(m_sRoot.Left(2)), 2, (BYTE**)&pui2);
		if (dwResult == 0)
		{
			sVolume = pui2->ui2_remote;
			if (pui2->ui2_domainname || pui2->ui2_username)
			{	// retrieve additional info
				sVolume += _T(";");
#ifdef _UNICODE
				sVolume += pui2->ui2_domainname != NULL ? pui2->ui2_domainname : _T("");
#else
				if (pui2->ui2_domainname)
				{
					CString sDomain(pui2->ui2_domainname);
					sVolume += sDomain; 
				}
#endif // _UNICODE
				sVolume += _T(";");
				sVolume += pui2->ui2_username;
				// password is never retrieved
			}

			if (pui2->ui2_status == USE_OK)
			{
				m_uSystemType &= ~DRIVE_NOT_CONNECTED;
			}
			else
			{
				m_uSystemType |= DRIVE_NOT_CONNECTED;
			}
		}
		if (pui2)
		{
			g_pNetApiBufferFree(pui2);
		}
	}
	else if (g_pWNetGetConnection)
	{
		DWORD dwLen = _MAX_PATH;
		dwResult = g_pWNetGetConnection(m_sRoot.Left(2), sVolume.GetBuffer(_MAX_PATH), &dwLen);
		sVolume.ReleaseBuffer();
		if (dwResult == NO_ERROR)
		{
			m_uSystemType &= ~DRIVE_NOT_CONNECTED;
		}
	}
	
	if (dwResult == NO_ERROR)
	{
		if (m_sVolume.IsEmpty())
		{
			m_sVolume = sVolume;
		}
		else
		{
			if (_tcsnicmp(m_sVolume, sVolume, sVolume.GetLength()) != 0)
			{
				CStringArray sa;
				SplitString(m_sVolume, sa, _T(';'));
				CString sDomain;
				if (sa.GetSize() > NETWORK_DOMAIN)
				{
					sDomain = sa.GetAt(NETWORK_DOMAIN);
					sDomain.TrimLeft();
				}
				if (!sDomain.IsEmpty()) // war die ursprünglich gesetzte Domäne leer
				{						// so ist die ermittelte Domäne für die Verbindung
										// nicht gültig.
					WK_TRACE(_T("Networkpath changed: %s -> %s\n"), m_sVolume, sVolume);
					m_sVolume = sVolume;
				}
			}
		}
	}
	else
	{
		m_uSystemType |= DRIVE_NOT_CONNECTED;
	}
}
#endif // _MFC_VER >= 0x0710
