



#include <stdlib.h>
#include <stdio.h>
//#include <windows.h>
#include <afxwin.h>
#include <winver.h>

const DWORD MAX_INFO_LEN=5000;
static BYTE pData[MAX_INFO_LEN];

static void PrintValue(const char *szValue)
{
	BOOL bValueOkay;
	void *pValueData;
	UINT dwValueLen;
	static char sValue[1000];
	sprintf(sValue,"\\StringFileInfo\\040904b0\\%s",szValue);
	bValueOkay = VerQueryValue(
		pData,	// address of buffer for version resource
		sValue,	// address of value to retrieve
		&pValueData,	// address of buffer for version pointer
		&dwValueLen		// address of version-value length buffer
	   );

	if (bValueOkay==FALSE) {
		sprintf(sValue,"\\StringFileInfo\\040704b0\\%s",szValue);
		bValueOkay = VerQueryValue(
		pData,	// address of buffer for version resource
		sValue,	// address of value to retrieve
		&pValueData,	// address of buffer for version pointer
		&dwValueLen		// address of version-value length buffer
	   );
	}

	if (bValueOkay) {
		printf("%-20s: %s\n",szValue,(const char*)pValueData);
	} else {
		// retry english
		fprintf(stdout,"Value %s not found\n",szValue);
	}
}

/*
%D   Total days in this CTime

%H   Hours in the current day

%M   Minutes in the current hour

%S   Seconds in the current minute
*/
static void PrintFileTime(const CFileStatus &f)
{

	CTime fTime = f.m_mtime;
	CString sTime =  fTime.Format("%d.%m.%y,%H:%M");
	printf("%-20s: %s\n","FileTime",(const char*)sTime);

}


static void Usage()
{
	fprintf(stdout,"printVersion: Version 1.1\n");
	fprintf(stdout,"usage: printVersion [/date] foo.exe\n");
}

int main(int argc, char **argv)
{
	BOOL bDateOnly = FALSE;
	char *szFilename=NULL;

	if (argc==2) {
		szFilename = argv[1];
	} else if (argc==3) {
		if (strcmp(argv[1],"/terse")==0
			|| strcmp(argv[1],"/kurz")==0) {
			bDateOnly=TRUE;
			szFilename = argv[2];
		} else {
			Usage();
			exit(2);
		}
	} else {
		Usage();
		exit(1);
	}
	
	if (bDateOnly==FALSE) {
		printf ("----------------------------------------------\n");
		printf("%-20s: %s\n","File",szFilename);
		printf ("----------------------------------------------\n");
	} else {
		printf("%-20s: %s\n","File",szFilename);
	}
	
	CFileStatus fs;
	if (CFile::GetStatus(szFilename,fs)) {
		PrintFileTime(fs);
		// print fileSize info
		printf("%-20s: %8d\n","FileSize", fs.m_size);

		DWORD dwDummy;
		DWORD dwLen = GetFileVersionInfoSize(szFilename,&dwDummy);
		if (dwLen > MAX_INFO_LEN) {
			printf ("internal overflow %d !?\n",dwLen);
			exit(1);
		}

		BOOL bOkay = GetFileVersionInfo(
				szFilename,	// pointer to filename string
				0,	// ignored 
				MAX_INFO_LEN,	// size of buffer
				pData// pointer to buffer to receive file-version info.
	   );	
		if (bOkay) {
			if (bDateOnly) {
				PrintValue( "FileVersion");
			} else {
				PrintValue( "FileVersion");
				PrintValue( "FileDescription");
				PrintValue( "LegalCopyright");
				PrintValue( "OriginalFilename");
			}
		} else {
			fprintf(stdout,"%s kein VersionInfo. Möglicherweise 16-bit Programm!\n",szFilename);
		}
	} else {
 		fprintf(stdout,"%s nicht gefunden\n",szFilename);
	}
	printf ("\n");
	
	return 0;
}