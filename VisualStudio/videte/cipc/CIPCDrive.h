/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCDrive.h $
// ARCHIVE:		$Archive: /Project/CIPC/CIPCDrive.h $
// CHECKIN:		$Date: 2.08.06 20:55 $
// VERSION:		$Revision: 37 $
// LAST CHANGE:	$Modtime: 2.08.06 20:52 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#if !defined(AFX_CIPCDRIVE_H__13AF4795_38BF_11D2_B58F_00C095EC9EFA__INCLUDED_)
#define AFX_CIPCDRIVE_H__13AF4795_38BF_11D2_B58F_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CIPCInt64.h"
#pragma warning(disable : 4201)	// nonstandard extension used : nameless struct/union
#include <Mmsystem.h>

#if _MFC_VER >= 0x0710
 #include <Lmuse.h>
#endif // _MSC_VER >= 1000

//////////////////////////////////////////////////////////////////////
enum DvrDriveUse
{
	DVR_INVALID_DRIVE = 0,
	DVR_DB_DRIVE = 1,
	DVR_WRITE_BACKUP_DRIVE = 2,
	DVR_READ_BACKUP_DRIVE = 3,
	DVR_CDR_BACKUP_DRIVE = 4,
	DVR_SYSTEM_DRIVE = 5,
	DVR_NORMAL_DRIVE = 6,
	DVR_REMOVABLE_DB_DRIVE = 7
};
//////////////////////////////////////////////////////////////////////
enum FS_Producer
{
	FSP_INVALID = 0,
	FSP_UDF_ADAPTEC_DIRECTCD = 1,
	FSP_UDF_CEQUADRAT_PACKETCD = 2,
	FSP_IOMEGA_JAZ = 3,
	FSP_HARDDISK = 4,
	FSP_FLOPPYDISK = 5,
	FSP_NERO_BURNING_ROM = 6
};
//////////////////////////////////////////////////////////////////////
// Drive Types returned by GetDriveType
// #define DRIVE_UNKNOWN     0
// #define DRIVE_NO_ROOT_DIR 1
// #define DRIVE_REMOVABLE   2
// #define DRIVE_FIXED       3
// #define DRIVE_REMOTE      4
// #define DRIVE_CDROM       5
// #define DRIVE_RAMDISK     6
//////////////////////////////////////////////////////////////////////
#define DRIVE_CD_R			 10
#define DRIVE_CD_RW			 11
#define DRIVE_DVD_ROM		 12
#define DRIVE_DVD_RAM		 13
#define DRIVE_DVD_R			 14
#define DRIVE_DVD_RW		 15
#define DRIVE_DELETED		 30
#define DRIVE_NOT_CONNECTED	 0x00010000
#define DRIVE_DISABLED		 0x00020000


//////////////////////////////////////////////////////////////////////
// network drive parameter SetVolume(CString& sVolume)
// sVolume: path[;[domain];user;password]
// if the domain is not set, set a space character instead
// the separator for the optional additional parameters is (;)
// the parameter password must be encoded with Encode(CString&) -> wk_util.h
#define NETWORK_PATH		0
#define NETWORK_DOMAIN		1
#define NETWORK_USER		2
#define NETWORK_PASSWORD	3
//////////////////////////////////////////////////////////////////////
class CIPCDrives;
//////////////////////////////////////////////////////////////////////
class AFX_EXT_CLASS CIPCDrive  
{
	// construction / destruction
public:
	CIPCDrive();
	CIPCDrive(const CIPCDrive& source);
	CIPCDrive(_TCHAR cDriveLetter, CWK_Profile& wkp);
	CIPCDrive(const CString sRoot, DvrDriveUse ddu);
	virtual ~CIPCDrive();

	// attributes
public:
	inline _TCHAR	GetLetter() const;
	inline CString	GetRootString() const;
	inline CString	GetVolumeString() const;
	inline UINT		GetType() const;
	
	inline DWORD	GetMB() const;
	inline CIPCInt64 GetUsedBytes() const;
	inline DWORD	GetUsedMB() const;
	inline DWORD	GetFreeMB() const;
	inline DWORD	GetClusterSize() const;
	inline DWORD	GetQuotaMB() const;
	inline DWORD	GetAvailableMB() const;
	inline DvrDriveUse	GetUsedAs() const;

	inline BOOL		IsDatabase() const;
	inline BOOL		IsRemoveableDatabase() const;
	inline BOOL		IsWriteBackup() const;
	inline BOOL		IsReadBackup() const;
	inline BOOL	    IsNetworkDrive() const;
	inline BOOL	    IsConnected() const;
	inline BOOL	    IsEnabled() const;
	inline BOOL	    IsCDROM() const;
	inline BOOL	    IsCDR() const;
	inline BOOL	    IsCDRW() const;
	inline BOOL	    IsDVDROM() const;
	inline BOOL	    IsDVDRAM() const;
	inline BOOL	    IsDVDR() const;
	inline BOOL	    IsDVDRW() const;
		   BOOL		IsSystem() const;
		   BOOL		IsEraseable() const;
		   BOOL		IsWritableRemovableDisk() const;
	inline CString  GetFileSystem() const;


	// operations
public:
	BOOL    CDDriveIsReady();
	BOOL	CheckSpace();
	CString Format();
	void	Trace();

#if _MFC_VER >= 0x0710
	BOOL	ConnectNetworkDrive();
	BOOL	DisconnectNetworkDrive(DWORD dwForce=USE_LOTS_OF_FORCE);
	void	UpdateNetworkState();
#endif

	void SetType(UINT uType);
	void SetFreeBytesAvailableToCaller(ULARGE_INTEGER uBytes);
	void SetTotalNumberOfBytes(ULARGE_INTEGER uBytes);
	void SetTotalNumberOfFreeBytes(ULARGE_INTEGER uBytes);
	void SetVolume(const CString& sVolume);
	void SetFileSystem(const CString& sFileSystem);
	void SetClusterSize(DWORD dwClusterSize);
	void SetUsedAs(DvrDriveUse newUse);
	BOOL SetQuota(DWORD dwQuotaMB);
	void Enable(BOOL bEnable);

	void Save(CWK_Profile& wkp);

	FS_Producer GetFSProducer() const;
	CString	   GetFSVersion() const;

	BOOL    CheckCD();
	BOOL    CloseCD();
	BOOL    OpenCD();

	// Net API functions
	static BOOL InitNetworkApi();
	static void FreeNetworkApi();


	// implementation
protected:
	void Init();
	void Init2();
	void CalculateClusterSize();
	void Lock();
	void Unlock();

	BOOL SearchDirectCDDriver(CString& sVersion) const;
	BOOL SearchPacketCDDriver(CString& sVersion) const;
	BOOL CloseMCICDROMDevice();
	BOOL OpenMCICDROMDevice();

	// private data member
private:
	CString		m_sRoot;
	UINT		m_uSystemType;
	DvrDriveUse	m_UsedAs;
	DWORD		m_dwQuota;

	// sizes
	ULARGE_INTEGER m_uFreeBytesAvailableToCaller;
	ULARGE_INTEGER m_uTotalNumberOfBytes;
	ULARGE_INTEGER m_uTotalNumberOfFreeBytes;

	// file system
	CString m_sVolume;
	CString	m_sFileSystem; 
	DWORD	m_dwClusterSize;

	CCriticalSection m_cs;

	// MCI for CD
	MCIDEVICEID m_MCIDeviceID;
	BOOL		m_bMCIDeviceClosed;
	friend class CIPCDrives;
};
//////////////////////////////////////////////////////////////////////
inline _TCHAR CIPCDrive::GetLetter() const
{
	if (!m_sRoot.IsEmpty())
	{
		return m_sRoot[0];
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////
inline CString CIPCDrive::GetRootString() const
{
	return m_sRoot;
}
//////////////////////////////////////////////////////////////////////
inline CString CIPCDrive::GetVolumeString() const
{
	return m_sVolume;
}
//////////////////////////////////////////////////////////////////////
inline UINT CIPCDrive::GetType() const
{
	return (UINT)LOWORD(m_uSystemType);
}
//////////////////////////////////////////////////////////////////////
inline DWORD CIPCDrive::GetFreeMB() const
{
	if (m_dwQuota == 0)
	{
		return CIPCInt64(m_uFreeBytesAvailableToCaller).GetInMB();
	}
	else
	{
		DWORD dwUsed = GetUsedMB();
		if (m_dwQuota>dwUsed)
		{
			return m_dwQuota - dwUsed;
		}
		return 0;
	}
}
//////////////////////////////////////////////////////////////////////
inline DWORD CIPCDrive::GetAvailableMB() const
{
	DWORD dwRet = 0;
	if (   IsDatabase()
		|| IsRemoveableDatabase())
	{
		if (m_dwQuota == 0)
		{
			dwRet = GetMB();
		}
		else
		{
			dwRet = m_dwQuota;
		}
		// CAVEAT: never return a "negative" value
		DWORD dwSafetySpace = 20;
		DWORD dw1Percent = dwRet / 100;
		if (dw1Percent>dwSafetySpace)
		{
			dwSafetySpace = dw1Percent;
		}
		if (dwRet < dwSafetySpace)
		{
			dwRet = 0;
		}
		else
		{
			dwRet -= dwSafetySpace;
		}
	}
	else
	{
		dwRet = GetMB();
	}
	return dwRet;
}
//////////////////////////////////////////////////////////////////////
inline DvrDriveUse CIPCDrive::GetUsedAs() const
{
	return m_UsedAs;;
}
//////////////////////////////////////////////////////////////////////
inline DWORD CIPCDrive::GetMB() const
{
	return CIPCInt64(m_uTotalNumberOfBytes).GetInMB();
}
//////////////////////////////////////////////////////////////////////
inline CIPCInt64 CIPCDrive::GetUsedBytes() const
{
	return CIPCInt64(m_uTotalNumberOfBytes.QuadPart-
		m_uFreeBytesAvailableToCaller.QuadPart);
}
//////////////////////////////////////////////////////////////////////
inline DWORD CIPCDrive::GetUsedMB() const
{
	return CIPCInt64(m_uTotalNumberOfBytes.QuadPart-
		m_uFreeBytesAvailableToCaller.QuadPart).GetInMB();
}
//////////////////////////////////////////////////////////////////////
inline DWORD CIPCDrive::GetClusterSize() const
{
	return m_dwClusterSize;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CIPCDrive::IsNetworkDrive() const
{
	return GetType() == DRIVE_REMOTE;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CIPCDrive::IsConnected() const
{
	return  m_uSystemType & DRIVE_NOT_CONNECTED ? FALSE : TRUE;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CIPCDrive::IsEnabled() const
{
	return  m_uSystemType & DRIVE_DISABLED ? FALSE : TRUE;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CIPCDrive::IsCDROM() const
{
	return GetType() == DRIVE_CDROM;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CIPCDrive::IsCDR() const
{
	return GetType() == DRIVE_CD_R;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CIPCDrive::IsCDRW() const
{
	return GetType() == DRIVE_CD_RW;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CIPCDrive::IsDVDROM() const
{
	return GetType() == DRIVE_DVD_ROM;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CIPCDrive::IsDVDRAM() const
{
	return GetType() == DRIVE_DVD_RAM;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CIPCDrive::IsDVDR() const
{
	return GetType() == DRIVE_DVD_R;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CIPCDrive::IsDVDRW() const
{
	return GetType() == DRIVE_DVD_RW;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CIPCDrive::IsDatabase() const
{
	return m_UsedAs == DVR_DB_DRIVE;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CIPCDrive::IsRemoveableDatabase() const
{
	return m_UsedAs == DVR_REMOVABLE_DB_DRIVE;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CIPCDrive::IsWriteBackup() const
{
	return m_UsedAs == DVR_WRITE_BACKUP_DRIVE;
}
//////////////////////////////////////////////////////////////////////
inline BOOL	CIPCDrive::IsReadBackup() const
{
	return m_UsedAs == DVR_READ_BACKUP_DRIVE;
}
//////////////////////////////////////////////////////////////////////
inline DWORD CIPCDrive::GetQuotaMB() const
{
	return m_dwQuota;
}
//////////////////////////////////////////////////////////////////////
inline CString CIPCDrive::GetFileSystem() const
{
	return m_sFileSystem;
}
#endif // !defined(AFX_DRIVE_H__13AF4795_38BF_11D2_B58F_00C095EC9EFA__INCLUDED_)
