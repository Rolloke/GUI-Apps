// disablewritecache.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	OSVERSIONINFO os;

	ZeroMemory(&os,sizeof(os));
	GetVersionEx(&os);

	if (   (os.dwPlatformId==VER_PLATFORM_WIN32_NT) 
		&& (os.dwMajorVersion==5) 
		&& (os.dwMinorVersion==1))
	{
		HKEY hKey = NULL;
		char szKey[] = "System\\CurrentControlSet\\Enum\\IDE";
		DWORD result = 0;
		
		result = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			szKey,
			0,
			KEY_READ,
			&hKey);
		if (result == ERROR_SUCCESS)
		{
			DWORD dwMaxSubKeyLen = 0;
			DWORD dwMaxValueNameLen = 0;
			DWORD dwMaxValueLen = 0;
			
			result = RegQueryInfoKey(hKey,				// handle to key to query
				NULL,				// address of buffer for
				// class string
				NULL,				// address of
				// size of class string buffer
				NULL,				// reserved
				NULL,				// address of buffer for
				// number of subkeys
				&dwMaxSubKeyLen,	// address of buffer for
				// longest subkey name length
				NULL,				// address of buffer for
				// longest class string length
				NULL,				// address of buffer for
				// number of value entries
				&dwMaxValueNameLen,// address of buffer for
				// longest value name length
				&dwMaxValueLen,	// address of buffer for
				// longest value data length
				NULL,				// address of buffer for
				// security descriptor length
				NULL				// address of buffer for last write time
				);
			
			if (result == ERROR_SUCCESS)
			{
				dwMaxValueNameLen++;
				dwMaxValueLen++;
				dwMaxSubKeyLen++;
				
				TCHAR* szSubkey = new TCHAR[dwMaxSubKeyLen];
				DWORD dwSubkeySize;
				
				for (DWORD regEnumIndex = 0;;)
				{
					dwSubkeySize = dwMaxSubKeyLen;
					
					result = RegEnumKeyEx(  hKey, regEnumIndex++, szSubkey, &dwSubkeySize,
						NULL, NULL, NULL, NULL );
					
					if (ERROR_SUCCESS == result)
					{
						HKEY hSubkey = NULL;
						
						result = RegOpenKeyEx( hKey, szSubkey, 0, KEY_READ, &hSubkey);
						
						if (result == ERROR_SUCCESS)
						{	
							for (DWORD i=0;;)
							{
								dwSubkeySize = dwMaxSubKeyLen;
								result = RegEnumKeyEx(hSubkey,i++,szSubkey,&dwSubkeySize,NULL,NULL,NULL,NULL);
								if (result == ERROR_SUCCESS)
								{
									HKEY hSubSubkey = NULL;
									result = RegOpenKeyEx(hSubkey,szSubkey,0,KEY_READ,&hSubSubkey);
									if (result == ERROR_SUCCESS)
									{
										char szClass[255];
										DWORD dwType = 0;
										DWORD dwLen = 0;
										
										result = RegQueryValueEx(hSubSubkey,"Class",NULL,&dwType,NULL,&dwLen);
										if (   result == ERROR_SUCCESS
											&& dwType == REG_SZ)
										{
											result = RegQueryValueEx(hSubSubkey, "Class", NULL, &dwType,
												(LPBYTE)szClass, &dwLen);
											if (0 == stricmp("DiskDrive",szClass))
											{
												// it's a hard disc
												HKEY hKeyDP = NULL;
												result = RegOpenKeyEx(hSubSubkey,"Device Parameters",0,KEY_READ|KEY_WRITE,&hKeyDP);
												if (result == ERROR_SUCCESS)
												{
													HKEY hKeyDisk = NULL;
													DWORD dwDummy = 0;
													result = RegCreateKeyEx(hKeyDP,
														"Disk",
														0,
														NULL,
														REG_OPTION_NON_VOLATILE,
														KEY_READ|KEY_WRITE,
														NULL,
														&hKeyDisk,
														&dwDummy);
													if (result == ERROR_SUCCESS)
													{
														// turn off the write cache
														int nValue = 0;
														result = RegSetValueEx(hKeyDisk, "UserWriteCacheSetting", 
															NULL, REG_DWORD,
															(LPBYTE)&nValue, sizeof(nValue));
														RegCloseKey(hKeyDisk);
													}
													
													RegCloseKey(hKeyDP);
												}
											}
										}
										RegCloseKey(hSubSubkey);
									}
								}
								else
								{
									break;
								}
								RegCloseKey(hSubkey);
							}
						}
					}
					else
					{
						break;
					}
				}
				
				delete []szSubkey;
				
			}
			RegCloseKey(hKey);
		}
	}

	return 0L;
}



