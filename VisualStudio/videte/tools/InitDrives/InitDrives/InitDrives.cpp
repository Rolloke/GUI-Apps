// InitDrives.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "CDrive.h"

#define BUFSIZE MAX_PATH        
#define FILESYSNAMEBUFSIZE MAX_PATH

BOOL InitializeDrive(int nDrive);
void FormatAllUnformatedFixedVolumes();

///////////////////////////////////////////////////////////////////////////////
int _tmain(int argc, _TCHAR* argv[])
{
	int nDrive		= 1;

	for (int nDrive = 0; nDrive < 16; nDrive++)
		InitializeDrive(nDrive);

	FormatAllUnformatedFixedVolumes();

	printf(_T("\npress a key\n"));
	getch();

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
BOOL InitializeDrive(int nDrive)
{
	BOOL bResult = FALSE;

	CDrive Drive(nDrive);

	// Diskgeometrie informationejn holen und ausgeben.
	DISK_GEOMETRY_EX DiskGeometry;
	if (Drive.GetDriveGeometry(DiskGeometry))
		Drive.TraceDriveGeometry(DiskGeometry);

	// Partitionsinformationen holen und ausgeben
	Drive.TraceAllPartitionInfos();

	// Partitioniere laufwerk mit einer Partition.
	bResult = Drive.PartitionDrive();

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////
void FormatAllUnformatedFixedVolumes()
{
	TCHAR Volumes[100];

	DWORD dwSysFlags;            // flags that describe the file system
	char FileSysNameBuf[FILESYSNAMEBUFSIZE];
	DWORD dwI = GetLogicalDriveStrings(sizeof(Volumes), Volumes);
	
	int nI		= 0;
	int nVolume = 0;

	while (Volumes[nI] != '\0')
	{
		CString sVolume(&Volumes[nI+=4]);
		if (!sVolume.IsEmpty())
		{
			if (GetDriveType(sVolume) == DRIVE_FIXED)
			{
				// Hat diese Volume ein gültiges Filesystem?
				if (GetVolumeInformation(sVolume, NULL, 0, NULL, NULL, &dwSysFlags, FileSysNameBuf, FILESYSNAMEBUFSIZE))
					printf(_T("Volume:%d Volumename:%s Filesystem=%s\n"), nVolume++, sVolume, FileSysNameBuf);
				else
				{
					if (GetLastError() == ERROR_UNRECOGNIZED_VOLUME)
					{
						printf(_T("Volume: %s has no valid filesystem\n"), sVolume);
						CDrive::Format(sVolume);
					}
				}
			}
		}
	}
}
