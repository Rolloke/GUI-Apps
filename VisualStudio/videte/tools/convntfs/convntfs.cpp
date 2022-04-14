#include "windows.h"
#include "stdio.h"


int main()
{
	// alle lokalen Festplatten
	DWORD dwDrives = GetLogicalDrives();
	DWORD dwDrive = 8; // 'd'
	char szConvert[_MAX_PATH];
	char szRoot[_MAX_DRIVE];
	char szLine[_MAX_PATH];
	char c;
	char szVolume[255];
	DWORD dwVolumeSerialNumber = 0;
	DWORD dwMaximumComponentLength = 0; 
	DWORD dwFileSystemFlags = 0; 
	char szFileSystemNameBuffer[255]; 

	GetSystemDirectory(szConvert,sizeof(szConvert));
	strcat(szConvert,"\\convert.exe");

	for (dwDrive=8,c='d';dwDrive!=0;dwDrive<<=1,c++)
	{
		if (dwDrive & dwDrives)
		{
			wsprintf(szRoot,"%c:\\",c);
			if (DRIVE_FIXED == GetDriveType(szRoot))
			{
				if (GetVolumeInformation(szRoot,
										 szVolume,sizeof(szVolume),
										 &dwVolumeSerialNumber, 
										 &dwMaximumComponentLength,
										 &dwFileSystemFlags,
										 szFileSystemNameBuffer,sizeof(szFileSystemNameBuffer)))
				{
					printf("%s,Volume: %s,File System %s \n",szRoot,szVolume,szFileSystemNameBuffer);
					if (0==stricmp(szFileSystemNameBuffer,"FAT"))
					{
//						printf("convert %c: /FS:NTFS\n",c);
						wsprintf(szLine,"%s %c: /FS:NTFS",szConvert,c);
						system(szLine);
					}
					else if (0==stricmp(szFileSystemNameBuffer,"NTFS"))
					{
						printf("%c: bereits nach NTFS konvertiert\n",c);
					}
				}
			}
		}
	}

	printf("\nAlle Laufwerke erfolgreich konvertiert.\n");

	return 0;
}