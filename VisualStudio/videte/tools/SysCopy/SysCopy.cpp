// SysCopy.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

/////////////////////////////////////////////////////////////////////////////
BOOL DoesFileExist(const char* szFileOrDirectory)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE h = FindFirstFile(szFileOrDirectory, &FindFileData);
	if (h == INVALID_HANDLE_VALUE)
	{
/*		h = FindFirstFile("d:\\project\\lib\\n*.dll", &FindFileData);
		if (h != INVALID_HANDLE_VALUE)
		{
			printf("file found %s %s\n",FindFileData.cFileName, FindFileData.cAlternateFileName);
			fflush(stdout);
			while (FindNextFile(h, &FindFileData))
			{
				printf("file found %s %s\n",FindFileData.cFileName, FindFileData.cAlternateFileName);
				fflush(stdout);
			}
			FindClose(h);
		}
*/		return FALSE;
	}
	FindClose(h);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	char szSystem[_MAX_PATH];
	char szExe[_MAX_PATH];

	GetSystemDirectory(szSystem,_MAX_PATH);

/*	printf("Search for file %s\n",lpCmdLine);
	fflush(stdout);

	int iCounter = 0;
	while (   (DoesFileExist(lpCmdLine) == FALSE)
		   && (iCounter < 10)
		   )
	{
		iCounter++;
		printf("Source file %s does not yet exist, wait %i. second\n",lpCmdLine, iCounter);
		fflush(stdout);
		Sleep(1000);
	}
*/
// gf command line may be contain ", remove them
	LPSTR szSign = "\"";
	int iLen = strlen(lpCmdLine);
	if (   (strncmp(&lpCmdLine[0],      "\"", 1) == 0)
		&& (strncmp(&lpCmdLine[iLen-1], "\"", 1) == 0)
		)
	{
//		printf("Source file %s contains "", remove\n", lpCmdLineOrig);
//		fflush(stdout);
		lpCmdLine[iLen-1] = '\0';
		lpCmdLine++;
//		printf("Source file %s corrected\n", lpCmdLine);
//		fflush(stdout);
	}

	if (DoesFileExist(lpCmdLine))
	{
		strcpy(szExe,strrchr(lpCmdLine,'\\')+1);
		strcat(szSystem,"\\");
		strcat(szSystem,szExe);
		printf("COPY %s %s\n",lpCmdLine,szSystem);
		fflush(stdout);
		if (CopyFile(lpCmdLine,szSystem,FALSE))
		{
			printf("%s COPY SUCCESS\n",lpCmdLine);
			fflush(stdout);
		}
		else
		{
			printf("%s COPY FAILED\n",lpCmdLine);
			fflush(stdout);
		}
	}
	else
	{
		printf("SOURCE FILE %s DOESN'T EXIST\n",lpCmdLine);
		fflush(stdout);
	}

	printf("SYSCOPY DONE by Videte IT\n",lpCmdLine);

	return 0;
}



