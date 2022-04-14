// rlr.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.

	DWORD dwHandle = 0;
	DWORD dwSize = GetFileVersionInfoSize(lpCmdLine,&dwHandle);

	if (dwSize>0)
	{
		PBYTE pData = new BYTE[dwSize];
		if (GetFileVersionInfo(lpCmdLine,0,dwSize,pData))
		{
			struct LANGANDCODEPAGE 
			{
				WORD wLanguage;
				WORD wCodePage;
			} *lpTranslate;
			UINT cbTranslate = 0;

			// Read the list of languages and code pages.

			VerQueryValue(pData, 
						  TEXT("\\VarFileInfo\\Translation"),
						  (LPVOID*)&lpTranslate,
						  &cbTranslate);
			
			char szLanguage[5];
			szLanguage[4] = 0;
			sprintf(szLanguage,"%04x",lpTranslate->wLanguage);


			HKEY hKey = NULL;
			
			if (ERROR_SUCCESS==RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\DVRT\\Version",0,KEY_ALL_ACCESS,&hKey))
			{
				RegSetValueEx(hKey,"Language",0,REG_SZ,(const unsigned char*)&szLanguage[0],strlen(szLanguage));
				RegCloseKey(hKey);
			}
		}

		delete [] pData;
	}

	return 0;
}



