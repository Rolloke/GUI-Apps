/* GlobalReplace: VERSION\(.*@AUTO_UPDATE --> VERSION(26, 2, 13, 29, "26/2/97,13:29\0", // @AUTO_UPDATE */
/* GlobalReplace: VERSION\(.*@AUTO_UPDATE --> VERSION(26, 2, 13, 29, "26/2/97,13:29\0", // @AUTO_UPDATE */
/* minigzip.c -- simulate gzip using the zlib compression library
 * Copyright (C) 1995 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h 
 */

/*
 * minigzip is a minimal implementation of the gzip utility. This is
 * only an example of using zlib and isn't meant to replace the
 * full-featured gzip. No attempt is made to deal with file systems
 * limiting names to 14 or 8+3 characters, etc... Error checking is
 * very limited. So use minigzip only for testing; use gzip for the
 * real thing. On MSDOS, use only on file names without extension
 * or in pipe mode.
 */

/* $Id: minigzip.c,v 1.5 1995/05/03 17:27:11 jloup Exp $ */

#include <stdio.h>
#include <io.h>
#include <string.h>
#include <malloc.h>

#include <afxwin.h>
#include "LogZip.h"

#pragma warning(disable: 4100) // unreferenced formal parameter

// CAVEAT: Defined in Project/Tools/LogZipLib/LogZip.cpp
extern "C"
{
#ifdef _UNICODE
	extern wchar_t *prog;	// OOPS
#else
	extern char *prog;	// OOPS
#endif
}

static int iCompressionFactor = 5;

static void DoOneFile(int doCompress, LPCTSTR szOneFile)
{
	if (_tcsstr(szOneFile,_T(".lgz")) || _tcsstr(szOneFile,_T(".LGZ"))) {
		doCompress=0;	// auto decompress
	} else if (_tcsstr(szOneFile,_T(".log")) || _tcsstr(szOneFile,_T(".LOG"))) {
		doCompress=1;	// auto compress
	}

	
	CFileStatus s;
	if (CFile::GetStatus( szOneFile,s)) {
		if (doCompress==0) {
			//              Decompressed:
			_ftprintf(stderr,_T("File        :%s (%8d bytes)...\n"),szOneFile,s.m_size);
			CLogZip::DecompressLogFile(szOneFile);
		} else {
			//              Compressed:"
			_ftprintf(stderr,_T("File      :%s (%8d bytes)...\n"),szOneFile,s.m_size);
			CLogZip::CompressLogFile(szOneFile, iCompressionFactor);
		}
	} else {
		_ftprintf(stderr,_T("file %s not found\n"),szOneFile);
	}
}

static void MY_PRINT_VERSION(int day, int month,
					int h, int min, LPCTSTR sVersion,
					int dummy)
{
	_ftprintf(stderr,_T("%s"),sVersion);
}

#ifdef _UNICODE
int wmain(int argc,wchar_t *argv[])
#else
int main(int argc,char *argv[])
#endif
{
	int doCompress = 0;
   /*  gzFile file; */

    prog = argv[0];
    argc--, argv++;

	iCompressionFactor = 5;

    if (argc > 0) {
		if (_tcscmp(*argv, _T("-fast")) == 0) {
			iCompressionFactor = 1;
            argc--, argv++;
		} else if (_tcscmp(*argv, _T("-slow")) == 0) {
			iCompressionFactor = 9;
            argc--, argv++;
		}

    }
    if (argc == 0 && doCompress==0) {
		_ftprintf(stderr,_T("logzip, Version "));
		MY_PRINT_VERSION(26, 2, 13, 29, _T("26/2/97,13:29\0"), // @AUTO_UPDATE
					0);
		_ftprintf(stderr,_T("\n"));
		_ftprintf(stderr,_T("usage: logzip [-fast/-slow]...\n"));
		_ftprintf(stderr,_T("usage: logzip foo.log compresses to foo.lgz\n"));
		_ftprintf(stderr,_T("usage: logzip foo.lgz decompresses to foo.log\n"));
		_ftprintf(stderr,_T("usage: multiple files allowed, even wth pattern matching ISDN*.log or *.lgz\n"));
    } else {

		do {
			if (_tcschr(*argv,_T('*'))) {
				CFileFind fileFinder;
				BOOL bFoundSome = fileFinder.FindFile(*argv);
				if (bFoundSome) {
					_ftprintf(stderr,_T("files matching %s\n"),*argv);
					while (fileFinder.FindNextFile()) {
						CString sFile = fileFinder.GetFileName();
						DoOneFile(doCompress, sFile);
					}
					CString sLastFile = fileFinder.GetFileName();
					DoOneFile(doCompress, sLastFile);
				} else {
					//
					_ftprintf(stderr,_T("no files matched %s\n"),*argv);
				}

			} else {
				// no pattyern matching
				// DWORD tick=GetTickCount();
				DoOneFile(doCompress, *argv);
				// _ftprintf(stdout,_T("MS %d\n"),GetTickCount()-tick);
			}
        } while (argv++, --argc);
    }

    return 0;
}
