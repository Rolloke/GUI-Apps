#include "StdAfx.h"
#include "cdrive.h"

///////////////////////////////////////////////////////////////////////////////
CDrive::CDrive(int nDrive)
{ 
	CString sFile;
	m_nDrive = nDrive;

	sFile.Format("\\\\.\\PHYSICALDRIVE%d", nDrive);

	m_hFile = CreateFile(sFile,				// file to open 
					GENERIC_ALL,			// open for reading and writing 
					FILE_SHARE_READ |      // share for reading and writing 
					FILE_SHARE_WRITE,
					NULL,					// default security 
					OPEN_EXISTING,			// existing file only 
					0, //FILE_ATTRIBUTE_NORMAL,	// normal file 
					NULL);					// no attr. template 
	 }

///////////////////////////////////////////////////////////////////////////////
CDrive::~CDrive(void)
{
	if (m_hFile != INVALID_HANDLE_VALUE) 
		CloseHandle(m_hFile);
}

///////////////////////////////////////////////////////////////////////////////
// Nur Partionieren wenn:
//  - DriveType = Festplatte
//  - Drive noch keine Partitionen besitzt.
BOOL CDrive::PartitionDrive()
{
	BOOL bResult = FALSE;

	DISK_GEOMETRY_EX DiskGeometry;
	ZeroMemory(&DiskGeometry, sizeof(DiskGeometry));
	if (GetDriveGeometry(DiskGeometry))
	{
		if (DiskGeometry.Geometry.MediaType == FixedMedia)
		{
			int nPartition = 0;
			if (GetPartitionCount(nPartition))
			{
				if (nPartition == 0)
				{
					printf(_T("There are no partitions on Drive%d. starting initialisation...\n"), m_nDrive);
					BIG_DRIVE_LAYOUT_INFORMATION_EX DriveLayout;
					ZeroMemory(&DriveLayout, sizeof(BIG_DRIVE_LAYOUT_INFORMATION_EX));

					DriveLayout.PartitionStyle								= PARTITION_STYLE_MBR;
					DriveLayout.PartitionCount								= 1;
					DriveLayout.Mbr.Signature								= 0x11071964;
					DriveLayout.PartitionEntry[0].PartitionStyle			= PARTITION_STYLE_MBR;
					DriveLayout.PartitionEntry[0].StartingOffset.QuadPart	= 0;
					DriveLayout.PartitionEntry[0].PartitionLength.QuadPart	= 0;
					DriveLayout.PartitionEntry[0].PartitionNumber			= 1;
					DriveLayout.PartitionEntry[0].RewritePartition			= TRUE;
					DriveLayout.PartitionEntry[0].Mbr.PartitionType			= PARTITION_EXTENDED;
					DriveLayout.PartitionEntry[0].Mbr.BootIndicator			= FALSE;
					DriveLayout.PartitionEntry[0].Mbr.RecognizedPartition	= TRUE;
					DriveLayout.PartitionEntry[0].Mbr.HiddenSectors			= 16065;

					bResult = SetDriveLayout(DriveLayout);
					GetDriveLayout(DriveLayout);
					Sleep(2000); // Ein wenig warten, bis Windows die neue Partition erkannt hat
					if (bResult)
						printf(_T("Partion created successfully on drive %d\n"), m_nDrive);
				}
				else
					printf(_T("There are already %d partitions on drive %d..Only empty disks will be initialized\n"), nPartition, m_nDrive);
			}
		}
		else
			printf(_T("Drive %d is not a harddisk\n"), m_nDrive);

		printf(_T("\n"));
	}
	

	return bResult;
}
///////////////////////////////////////////////////////////////////////////////
BOOL CDrive::GetLength(__int64& i64Len)
{
	GET_LENGTH_INFORMATION	LengthInformation;
	DWORD					dwBytesReturned = 0;
	BOOL					bResult	= FALSE;
	i64Len					= 0;

	if (m_hFile != INVALID_HANDLE_VALUE) 
	{
		bResult = DeviceIoControl(m_hFile,							// handle to device
								IOCTL_DISK_GET_LENGTH_INFO,		// dwIoControlCode
								NULL,							// lpInBuffer
								0,								// nInBufferSize
								(LPVOID) &LengthInformation,	// output buffer
								sizeof(GET_LENGTH_INFORMATION),	// size of output buffer
								(LPDWORD)&dwBytesReturned,		// number of bytes returned
								(LPOVERLAPPED)NULL				// OVERLAPPED structure
								);
		i64Len = (__int64)LengthInformation.Length.QuadPart; 
	}

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////
BOOL CDrive::GetDriveGeometry(DISK_GEOMETRY_EX& DiskGeometry)
{
	DWORD					dwBytesReturned = 0;
	BOOL					bResult	= FALSE;

	if (m_hFile != INVALID_HANDLE_VALUE) 
	{
		bResult = DeviceIoControl(m_hFile,							// handle to device
								IOCTL_DISK_GET_DRIVE_GEOMETRY_EX,	// dwIoControlCode
								NULL,								// lpInBuffer
								0,									// nInBufferSize
								(LPVOID) &DiskGeometry,				// output buffer
								sizeof(DISK_GEOMETRY_EX ),			// size of output buffer
								(LPDWORD)&dwBytesReturned,			// number of bytes returned
								(LPOVERLAPPED)NULL					// OVERLAPPED structure
								);
	}

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////
void CDrive::TraceDriveGeometry(DISK_GEOMETRY_EX& DiskGeometry)
{
	printf(_T("\nGeometry information about drive %d\n"), m_nDrive);

	printf(_T("\tCylinders=%I64d\n"), DiskGeometry.Geometry.Cylinders);		
	
	switch (DiskGeometry.Geometry.MediaType)
	{
		case Unknown:
			printf(_T("\tUnknown\n"));
			break;
		case F5_1Pt2_512:
			printf(_T("\tA 5.25"" floppy, with 1.2MB and 512 bytes/sector.\n"));
			break;
		case F3_1Pt44_512:
			printf(_T("\tA 3.5"" floppy, with 1.44MB and 512 bytes/sector.\n"));
			break;
		case F3_2Pt88_512:
			printf(_T("\tA 3.5"" floppy, with 2.88MB and 512 bytes/sector.\n"));
			break;
		case F3_20Pt8_512:
			printf(_T("\tA 3.5"" floppy, with 20.8MB and 512 bytes/sector.\n"));
			break;
		case F3_720_512:
			printf(_T("\tA 3.5"" floppy, with 720KB and 512 bytes/sector.\n"));
			break;
		case F5_360_512:
			printf(_T("\tA 5.25"" floppy, with 360KB and 512 bytes/sector.\n"));
			break;
		case F5_320_512:
			printf(_T("\tA 5.25"" floppy, with 320KB and 512 bytes/sector.\n"));
			break;
		case F5_320_1024:
			printf(_T("\tA 5.25"" floppy, with 320KB and 1024 bytes/sector.\n"));
			break;
		case F5_180_512:
			printf(_T("\tA 5.25"" floppy, with 180KB and 512 bytes/sector.\n"));
			break;
		case F5_160_512:
			printf(_T("\tA 5.25"" floppy, with 160KB and 512 bytes/sector.\n"));
			break;
		case RemovableMedia:
			printf(_T("\tRemovable media other than floppy.\n"));
			break;
		case FixedMedia:
			printf(_T("\tFixed hard disk media.\n"));
			break;
		case F3_120M_512:
			printf(_T("\tA 3.5"" floppy, with 120MB and 512 bytes/sector.\n"));
			break;
		case F3_640_512:
			printf(_T("\tA 3.5"" floppy, with 640MB and 512 bytes/sector.\n"));
			break;
		case F5_640_512:
			printf(_T("\tA 5.25"" floppy, with 640KB and 512 bytes/sector.\n"));
			break;
		case F5_720_512:
			printf(_T("\tA 5.25"" floppy, with 720KB and 512 bytes/sector.\n"));
			break;
		case F3_1Pt2_512:
			printf(_T("\tA 3.5"" floppy, with 1.2MB and 512 bytes/sector.\n"));
			break;
		case F3_1Pt23_1024:
			printf(_T("\tA 3.5"" floppy, with 1.23MB and 1024 bytes/sector.\n"));
			break;
		case F5_1Pt23_1024:
			printf(_T("\tA 5.25"" floppy, with 1.23KB and 1024 bytes/sector.\n"));
			break;
		case F3_128Mb_512:
			printf(_T("\tA 3.5"" floppy, with 128MB and 512 bytes/sector. \n"));
			break;
		case F3_230Mb_512:
			printf(_T("\tA 3.5"" floppy, with 230MB and 512 bytes/sector.\n"));
			break;
		case F8_256_128:
			printf(_T("\tAn 8"" floppy, with 256KB and 128 bytes/sector.\n"));
			break;
		case F3_200Mb_512:
			printf(_T("\tA 3.5"" floppy, with 200MB and 512 bytes/sector. (HiFD).\n"));
			break;
		case F3_240M_512:
			printf(_T("\tA 3.5"" floppy, with 240MB and 512 bytes/sector. (HiFD).\n"));
			break;
		case F3_32M_512:
			printf(_T("\tA 3.5"" floppy, with 32MB and 512 bytes/sector.\n"));
			break;
	}
	
	printf(_T("\tTracksPerCylinder=%d\n"), DiskGeometry.Geometry.TracksPerCylinder);		
	printf(_T("\tSectorsPerTrack=%d\n"), DiskGeometry.Geometry.SectorsPerTrack);		
	printf(_T("\tBytesPerSector =%d\n"), DiskGeometry.Geometry.BytesPerSector);		
}

///////////////////////////////////////////////////////////////////////////////
BOOL CDrive::GetPartitionInfo(PARTITION_INFORMATION_EX& PartInfo)
{
	DWORD					dwBytesReturned = 0;
	BOOL					bResult	= FALSE;

	if (m_hFile != INVALID_HANDLE_VALUE) 
	{
		bResult = DeviceIoControl(m_hFile,							// handle to device
								IOCTL_DISK_GET_PARTITION_INFO_EX,	// dwIoControlCode
								NULL,								// lpInBuffer
								0,									// nInBufferSize
								(LPVOID) &PartInfo,					// output buffer
								sizeof(PARTITION_INFORMATION_EX),	// size of output buffer
								(LPDWORD)&dwBytesReturned,			// number of bytes returned
								(LPOVERLAPPED)NULL					// OVERLAPPED structure
								);
	}

	return bResult;
}
 
///////////////////////////////////////////////////////////////////////////////
void CDrive::TraceAllPartitionInfos()
{
	DISK_GEOMETRY_EX DiskGeometry;
	ZeroMemory(&DiskGeometry, sizeof(DiskGeometry));
	if (GetDriveGeometry(DiskGeometry))
	{
		if (DiskGeometry.Geometry.MediaType == FixedMedia)
		{
			BIG_DRIVE_LAYOUT_INFORMATION_EX DriveLayout;
			ZeroMemory(&DriveLayout, sizeof(BIG_DRIVE_LAYOUT_INFORMATION_EX));
			if (GetDriveLayout(DriveLayout))
			{
				printf(_T("\nPartition information about drive %d:\n"), m_nDrive);
				for (int nI = 0; nI < (int)DriveLayout.PartitionCount; nI++)
				{
					if (DriveLayout.PartitionEntry[nI].PartitionLength.QuadPart > 0)
						TraceSinglePartitionInfo(DriveLayout.PartitionEntry[nI]);
				}
			}
		}
		printf(_T("\n"));
	}
}

///////////////////////////////////////////////////////////////////////////////
void CDrive::TraceSinglePartitionInfo(PARTITION_INFORMATION_EX& PartInfo)
{
	printf(_T("\tPartition %d:\n"), PartInfo.PartitionNumber); 

	switch (PartInfo.PartitionStyle)
	{
		case PARTITION_STYLE_MBR:
			printf(_T("\t\tPARTITION_STYLE_MBR\n"));
			break;
		case PARTITION_STYLE_GPT:
			printf(_T("\t\tPARTITION_STYLE_GPT\n"));
			break;
		case PARTITION_STYLE_RAW:
			printf(_T("\t\tPARTITION_STYLE_RAW\n"));
			break;
	}
	printf(_T("\t\tStarting offset=%I64d\n"), PartInfo.StartingOffset);
	printf(_T("\t\tPartition length=%I64d Bytes (%d MB)\n"), PartInfo.PartitionLength, (DWORD)(PartInfo.PartitionLength.QuadPart/(1024*1024)));
	printf(_T("\t\tPartition number=%d\n"), PartInfo.PartitionNumber);
	printf(_T("\t\tRewrite partition=%s\n"), PartInfo.RewritePartition ? _T("yes") : _T("no"));
	
	if (PartInfo.PartitionStyle == PARTITION_STYLE_MBR)
	{
		switch (PartInfo.Mbr.PartitionType)
		{
			case PARTITION_EXTENDED:
				printf(_T("\t\tPARTITION_EXTENDED\n"));
				break;
			case PARTITION_FAT_12:
				printf(_T("\t\tPARTITION_FAT_12\n"));
				break;
			case PARTITION_FAT_16:
				printf(_T("\t\tPARTITION_FAT_16\n"));
				break;
			case PARTITION_FAT32:
				printf(_T("\t\tPARTITION_FAT32\n"));
				break;
			case PARTITION_FAT32_XINT13:
				printf(_T("\t\tPARTITION_FAT32_XINT13\n"));
				break;
			case PARTITION_HUGE:
				printf(_T("\t\tPARTITION_HUGE\n"));
				break;
			case PARTITION_IFS:
				printf(_T("\t\tPARTITION_IFS\n"));
				break;
			case PARTITION_LDM:
				printf(_T("\t\tPARTITION_LDM\n"));
				break;
			case PARTITION_NTFT:
				printf(_T("\t\tPARTITION_NTFT\n"));
				break;
			case PARTITION_OS2BOOTMGR:
				printf(_T("\t\tPARTITION_OS2BOOTMGR\n"));
				break;
			case PARTITION_PREP:
				printf(_T("\t\tPARTITION_PREP\n"));
				break;
			case PARTITION_UNIX:
				printf(_T("\t\tPARTITION_UNIX\n"));
				break;
			case PARTITION_XENIX_1:
				printf(_T("\t\tPARTITION_XENIX_1\n"));
				break;
			case PARTITION_XENIX_2:
				printf(_T("\t\tPARTITION_XENIX_2\n"));
				break;
			case PARTITION_XINT13:
				printf(_T("\t\tPARTITION_XINT13\n"));
				break;
			case PARTITION_XINT13_EXTENDED:
				printf(_T("\t\tPARTITION_XINT13_EXTENDED\n"));
				break;
			case VALID_NTFT:
				printf(_T("\t\tVALID_NTFT\n"));
				break;				
		}
		
		printf(_T("\t\tBootIndicator=%s\n"), PartInfo.Mbr.BootIndicator ? _T("true"): _T("false") );
		printf(_T("\t\tRecognizedPartition=%s\n"), PartInfo.Mbr.RecognizedPartition ? _T("true"): _T("false") );
		printf(_T("\t\tHidden sectors=%d\n"), PartInfo.Mbr.HiddenSectors );
	
	}
	else if (PartInfo.PartitionStyle == PARTITION_STYLE_GPT)
	{
	}
}

///////////////////////////////////////////////////////////////////////////////
BOOL CDrive::CreateDisk(CREATE_DISK& Disk)
{
	DWORD					dwBytesReturned = 0;
	BOOL					bResult	= FALSE;

	if (m_hFile != INVALID_HANDLE_VALUE) 
	{
		bResult = DeviceIoControl(m_hFile,					// handle to device
								IOCTL_DISK_CREATE_DISK,		// dwIoControlCode
								(LPVOID)&Disk,				// lpInBuffer
								sizeof(CREATE_DISK),		// nInBufferSize
								(LPVOID) NULL,				// output buffer
								0,							// size of output buffer
								(LPDWORD)&dwBytesReturned,	// number of bytes returned
								(LPOVERLAPPED)NULL			// OVERLAPPED structure
								);
		if (!bResult)
		{
			DWORD dwError = GetLastError();
			printf(_T("IOCTL_DISK_CREATE_DISK failed (Errorcode=%d\n"), dwError);
		}
	
	}

	return bResult;

}

///////////////////////////////////////////////////////////////////////////////
BOOL CDrive::SetDriveLayout(BIG_DRIVE_LAYOUT_INFORMATION_EX& DriveLayout)
{
	DWORD					dwBytesReturned = 0;
	BOOL					bResult	= FALSE;

	if (m_hFile != INVALID_HANDLE_VALUE) 
	{
		bResult = DeviceIoControl(m_hFile,								// handle to device
								IOCTL_DISK_SET_DRIVE_LAYOUT_EX,			// dwIoControlCode
								(LPVOID)&DriveLayout,					// lpInBuffer
								sizeof(BIG_DRIVE_LAYOUT_INFORMATION_EX),// nInBufferSize
								(LPVOID) NULL,							// output buffer
								0,										// size of output buffer
								(LPDWORD)&dwBytesReturned,				// number of bytes returned
								(LPOVERLAPPED)NULL						// OVERLAPPED structure
								);
		if (!bResult)
		{
			DWORD dwError = GetLastError();
			printf(_T("IOCTL_DISK_SET_DRIVE_LAYOUT_EX failed (Errorcode=%d)\n"), dwError);
		}
	
	}

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////
BOOL CDrive::GetDriveLayout(DRIVE_LAYOUT_INFORMATION_EX& DriveLayout)
{
	DWORD					dwBytesReturned = 0;
	BOOL					bResult	= FALSE;

	if (m_hFile != INVALID_HANDLE_VALUE) 
	{
		bResult = DeviceIoControl(m_hFile,							// handle to device
								IOCTL_DISK_GET_DRIVE_LAYOUT_EX,		// dwIoControlCode
								NULL,								// lpInBuffer
								0,									// nInBufferSize
								(LPVOID)&DriveLayout,				// output buffer
								sizeof(DRIVE_LAYOUT_INFORMATION_EX),// size of output buffer
								(LPDWORD)&dwBytesReturned,			// number of bytes returned
								(LPOVERLAPPED)NULL					// OVERLAPPED structure
								);
		if (!bResult)
		{
			DWORD dwError = GetLastError();
			printf(_T("IOCTL_DISK_GET_DRIVE_LAYOUT_EX failed (Errorcode=%d)\n"), dwError);
		}
	
	}

	return bResult;
}
///////////////////////////////////////////////////////////////////////////////
BOOL CDrive::GetDriveLayout(BIG_DRIVE_LAYOUT_INFORMATION_EX& DriveLayout)
{
	DWORD					dwBytesReturned = 0;
	BOOL					bResult	= FALSE;

	if (m_hFile != INVALID_HANDLE_VALUE) 
	{
		bResult = DeviceIoControl(m_hFile,								// handle to device
								IOCTL_DISK_GET_DRIVE_LAYOUT_EX,			// dwIoControlCode
								NULL,									// lpInBuffer
								0,										// nInBufferSize
								(LPVOID)&DriveLayout,					// output buffer
								sizeof(BIG_DRIVE_LAYOUT_INFORMATION_EX),// size of output buffer
								(LPDWORD)&dwBytesReturned,				// number of bytes returned
								(LPOVERLAPPED)NULL						// OVERLAPPED structure
								);
		if (!bResult)
		{
			DWORD dwError = GetLastError();
			printf(_T("IOCTL_DISK_GET_DRIVE_LAYOUT_EX failed (Errorcode=%d)\n"), dwError);
		}
	
	}

	return bResult;
}

///////////////////////////////////////////////////////////////////////////////
BOOL CDrive::Format(const CString& sDrive)
{
	CString sCmdLine;
	CString sTextFile;
	CString sName;
	BOOL	bResult = FALSE;

	printf(_T("\n"));

	// Formatierung nur für nicht formatierte Festplatten erlauben
	if (GetDriveType(sDrive) == DRIVE_FIXED)
	{
		DWORD dwSysFlags; 
		char FileSysNameBuf[FILESYSNAMEBUFSIZE];
		
		if (!GetVolumeInformation(sDrive, NULL, 0, NULL, NULL, &dwSysFlags, FileSysNameBuf, FILESYSNAMEBUFSIZE))
		{
			if (GetLastError() == ERROR_UNRECOGNIZED_VOLUME)
			{
				printf(_T("Starting formating partion %s...\n"), sDrive);
				// Datei mit Tastatureingaben erzeugen
				CFile file("c:\\keys.txt", CFile::modeCreate|CFile::modeWrite);
				sTextFile.Format("j \r\n\r\n");
				file.Write(sTextFile,sTextFile.GetLength());
				file.Close();
				
				// Kommandozeile erzeugen.
				sTextFile.Format("Format.com %s /X /FS:NTFS /Q < c:\\keys.txt", sDrive.Left(2));

				// Und formatieren...
				bResult = MyExec(sTextFile, "c:\\");
			}
		}
		else
		{
			printf(_T("There is already a filesystem on partition %s.\n"), sDrive);
			printf(_T("Only empty partitions will be formated\n\n"));
			bResult = TRUE;
		}
	}
	return bResult;
}
////////////////////////////////////////////////////////////////////////////
// Startet einen Prozeß und kehr erst nach Beenden dieses zurück
BOOL CDrive::MyExec(const CString &sCmdLine, 
			const CString & sWorkingDirectory,
			BOOL bWait	/* = TRUE */,
			DWORD dwStartFlags, /* = SW_SHOWNORMAL, */
			DWORD dwStartTime /* =30000 */)
{
	STARTUPINFO			startUpInfo;
	PROCESS_INFORMATION prozessInformation;
	CString sExePathName = "Command.com";

	startUpInfo.cb				= sizeof(STARTUPINFO);
    startUpInfo.lpReserved      = NULL;
    startUpInfo.lpDesktop       = NULL;
    startUpInfo.lpTitle			= NULL;
    startUpInfo.dwX				= 0;
    startUpInfo.dwY				= 0;
    startUpInfo.dwXSize			= 0;
    startUpInfo.dwYSize			= 0;
    startUpInfo.dwXCountChars   = 0;
    startUpInfo.dwYCountChars   = 0;
    startUpInfo.dwFillAttribute = 0;
	startUpInfo.dwFlags			= /*STARTF_FORCEONFEEDBACK |*/ STARTF_USESHOWWINDOW;
	startUpInfo.wShowWindow		= (WORD)dwStartFlags;  
    startUpInfo.cbReserved2     = 0;
    startUpInfo.lpReserved2     = NULL;

	CString sCommandLine;
	sCommandLine.Format("%s /C %s", (const char *)sExePathName, (const char *)sCmdLine);
	
	// OOPS NT might be different
	BOOL bProcess = CreateProcess(
						NULL, // (LPCSTR)sExePathName,	// pointer to name of executable module 
						(LPSTR)sCommandLine.GetBuffer(0),	// pointer to command line string
						(LPSECURITY_ATTRIBUTES)NULL,	// pointer to process security attributes 
						(LPSECURITY_ATTRIBUTES)NULL,	// pointer to thread security attributes 
						FALSE,	// handle inheritance flag 
						NORMAL_PRIORITY_CLASS,//|DEBUG_PROCESS|DEBUG_ONLY_THIS_PROCESS,	// creation flags 
						(LPVOID)NULL,	// pointer to new environment block 
						(LPCTSTR)sWorkingDirectory,	// pointer to current directory name 
						(LPSTARTUPINFO)&startUpInfo,	// pointer to STARTUPINFO 
						(LPPROCESS_INFORMATION)&prozessInformation 	// pointer to PROCESS_INFORMATION  
						);

	sCommandLine.ReleaseBuffer();
	
	if (bProcess)
	{
		DWORD dwIdleTime=0;
		DWORD rWFII;

		rWFII=WaitForInputIdle(prozessInformation.hProcess,dwStartTime);
		if (rWFII==0)
		{
			dwIdleTime = GetTickCount();	
			printf("%s gestarted %d ms\n",(const char*)sCommandLine, dwIdleTime-dwStartTime);
		}
		else if(rWFII==WAIT_TIMEOUT)
			printf("%s wait for input idle timeout %d\n",(const char*)sCommandLine, dwStartTime);
		
		if (bWait)
			DWORD dwTermination = WaitForSingleObject(prozessInformation.hProcess,INFINITE);
	} else
	{
		printf("Could not start %s",sExePathName);
	}

	return bProcess;
}

////////////////////////////////////////////////////////////////////////////
BOOL CDrive::GetPartitionCount(int& nPartition)
{
	BOOL bResult = FALSE;
	nPartition = 0;

	DISK_GEOMETRY_EX DiskGeometry;
	ZeroMemory(&DiskGeometry, sizeof(DiskGeometry));
	if (GetDriveGeometry(DiskGeometry))
	{
		if (DiskGeometry.Geometry.MediaType == FixedMedia)
		{
			BIG_DRIVE_LAYOUT_INFORMATION_EX DriveLayout;
			ZeroMemory(&DriveLayout, sizeof(BIG_DRIVE_LAYOUT_INFORMATION_EX));
			if (GetDriveLayout(DriveLayout))
			{
				bResult = TRUE;
				for (int nI = 0; nI < (int)DriveLayout.PartitionCount; nI++)
				{
					if (DriveLayout.PartitionEntry[nI].PartitionLength.QuadPart > 0)
						nPartition++;
				}
			}
		}
	}

	return bResult;
}

BOOL CDrive::LowLevelFormat(PDISK_GEOMETRY lpGeometry)
{
   FORMAT_PARAMETERS FormatParameters;
   PBAD_TRACK_NUMBER lpBadTrack;
   UINT i;
   BOOL b;
   DWORD ReturnedByteCount;

   FormatParameters.MediaType = lpGeometry->MediaType;
   FormatParameters.StartHeadNumber = 0;
   FormatParameters.EndHeadNumber = lpGeometry->TracksPerCylinder - 1;
   lpBadTrack = (PBAD_TRACK_NUMBER) LocalAlloc(LMEM_ZEROINIT,lpGeometry->TracksPerCylinder*sizeof(*lpBadTrack));

   for (i = 0; i < lpGeometry->Cylinders.LowPart; i++)
   {
       FormatParameters.StartCylinderNumber = i;
       FormatParameters.EndCylinderNumber = i;

       b = DeviceIoControl(
               m_hFile,
               IOCTL_DISK_FORMAT_TRACKS,
               &FormatParameters,
               sizeof(FormatParameters),
               lpBadTrack,
               lpGeometry->TracksPerCylinder*sizeof(*lpBadTrack),
               &ReturnedByteCount,
               NULL
               );

       if (!b )
	   {
			DWORD dwErr = GetLastError();
		   LocalFree(lpBadTrack);
           return b;
       }
	}

   LocalFree(lpBadTrack);

   return TRUE;
}