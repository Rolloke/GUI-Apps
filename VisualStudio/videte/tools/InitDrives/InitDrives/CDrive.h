#pragma once

#define FILESYSNAMEBUFSIZE MAX_PATH

typedef struct BIG_DRIVE_LAYOUT_INFORMATION_EX
{
	DWORD PartitionStyle;
	DWORD PartitionCount;
	union {
		DRIVE_LAYOUT_INFORMATION_MBR Mbr;
		DRIVE_LAYOUT_INFORMATION_GPT Gpt;
	}; PARTITION_INFORMATION_EX PartitionEntry[100];
} BIG_DRIVE_LAYOUT_INFORMATION_EX;

class CDrive
{
public:
	CDrive(int nDrive);
	virtual ~CDrive(void);

private:
	HANDLE  m_hFile; 
	int		m_nDrive;
public:
	BOOL PartitionDrive();
	static BOOL Format(const CString& Drive);
	
	BOOL GetPartitionCount(int& nPartition);

	BOOL GetLength(__int64& i64Len);
	BOOL GetPartitionInfo(PARTITION_INFORMATION_EX& PartInfo);
	void TraceAllPartitionInfos();
	void TraceSinglePartitionInfo(PARTITION_INFORMATION_EX& PartInfo);

	BOOL GetDriveGeometry(DISK_GEOMETRY_EX& DiskGeometry);
	void TraceDriveGeometry(DISK_GEOMETRY_EX& DiskGeometry);

	BOOL CreateDisk(CREATE_DISK& Disk);
	
	BOOL SetDriveLayout(BIG_DRIVE_LAYOUT_INFORMATION_EX&	DriveLayout);
	BOOL GetDriveLayout(BIG_DRIVE_LAYOUT_INFORMATION_EX&	DriveLayout);
	BOOL GetDriveLayout(DRIVE_LAYOUT_INFORMATION_EX&		DriveLayout);

	BOOL LowLevelFormat(PDISK_GEOMETRY lpGeometry);
private:
	static BOOL CDrive::MyExec(const CString &sCmdLine, 
				const CString & sWorkingDirectory,
				BOOL bWait= TRUE,
				DWORD dwStartFlags = SW_SHOWNORMAL,
				DWORD dwStartTime = 30000);

};
