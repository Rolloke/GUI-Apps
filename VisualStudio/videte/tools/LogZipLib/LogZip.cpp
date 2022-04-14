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

#include "LogZip.h"

#include <stdio.h>
#include <io.h>
#include <string.h>
#include <malloc.h>

#include <afxwin.h>

extern "C" {
#include "zlib.h"


#ifdef STDC
#  include <string.h>
#endif

#if defined(MSDOS) || defined(OS2) || defined(WIN32)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

#ifdef VMS
#  define GZ_SUFFIX "-gz"
#else
#  define GZ_SUFFIX ".lgz"
#endif
#define SUFFIX_LEN sizeof(GZ_SUFFIX)

#define BUFLEN 4096
#define MAX_NAME_LEN 1024

#define local static
/* For MSDOS and other systems with limitation on stack size. For Unix,
    #define local
   works also.
 */

// CAVEAT: Also external used in Project/Tools/LogZip/LogZip.cpp
#ifdef _UNICODE
wchar_t *prog;
#else
char *prog;
#endif

void error           OF((char *msg));
void gz_compress     OF((FILE   *in, gzFile out));
void gz_uncompress   OF((gzFile in, FILE   *out));
// OBSOLETE void file_compress   OF((char  *file));
// OBSOLETE void file_uncompress OF((char  *file));
// OBSOLETE int  main            OF((int argc, char *argv[]));

/* ===========================================================================
 * Display error message and exit
 */
void error(char *msg)
{
    fprintf(stderr, "%s: %s\n", prog, msg);
    exit(1);
}

/* ===========================================================================
 * Compress input to output then close both files.
 */
void gz_compress(FILE   *in,gzFile out)
{
    local char buf[BUFLEN];
    int len;
    int err;

    for (;;) {
        len = fread(buf, 1, sizeof(buf), in);
        if (ferror(in)) {
            perror("fread");
            exit(1);
        }
        if (len == 0) break;

        if (gzwrite(out, buf, len) != len) error(gzerror(out, &err));
    }
    fclose(in);
    if (gzclose(out) != Z_OK) error("failed gzclose");
}

/* ===========================================================================
 * Uncompress input to output then close both files.
 */
void gz_uncompress(gzFile in,FILE   *out)
{
    local char buf[BUFLEN];
    int len;
    int err;

    for (;;) {
        len = gzread(in, buf, sizeof(buf));
        if (len < 0) error (gzerror(in, &err));
        if (len == 0) break;

        if (fwrite(buf, 1, len, out) != (uInt)len) error("failed fwrite");
    }
    if (fclose(out)) error("failed fclose");

    if (gzclose(in) != Z_OK) error("failed gzclose");
}


}	// end of extern "C"

#ifdef _UNICODE
int CLogZip::CompressLogFile(const wchar_t* szFile, int iFactor)
#else
int CLogZip::CompressLogFile(const char*szFile, int iFactor)
#endif
{
	if (szFile==NULL)
	{
		return 0;
	}

	CFileStatus origState;
	if (CFile::GetStatus(szFile, origState))
	{
		// old state
	}
	else
	{
		return 0;	// file not found
	}

	CString sFile,sNewFile;
	sFile = szFile;

	int ix = sFile.ReverseFind('.');
	// NOT YET check extenstion length
	if (ix!=-1 
		&& (sFile[ix+1]=='l' || sFile[ix+1]=='L')
		&& (sFile[ix+2]=='o' || sFile[ix+2]=='O')
		&& (sFile[ix+3]=='g' || sFile[ix+3]=='G')
		)
	{
		sNewFile = sFile.Left(ix)+".lgz";
	}
	else
	{
		return 0;	// no log file
	}

	FILE  *in;
	gzFile out;

	in = _tfopen(sFile, _T("rb"));
	if (in == NULL) 
	{
		_tperror(sFile);
		return 0;
	}
	else
	{
		int nLen = sNewFile.GetLength();
		char *szNewFile = new char[nLen+1];
#ifdef _UNICODE
		nLen = WideCharToMultiByte(CP_ACP, 0, LPCTSTR(sNewFile), nLen, szNewFile, nLen, NULL, NULL);
#else
		strncpy(szNewFile, LPCTSTR(sNewFile), nLen);
#endif
		szNewFile[nLen] = 0;
		
		CString sCmd;
		if (iFactor<1 || iFactor>9)
		{
			iFactor=5;
		}

		char szCmd[20];
		sprintf(szCmd, "wb%d", iFactor);
		out = gzopen(szNewFile, szCmd); /* use "wb9" for maximal compression */
		
		if (out == NULL) 
		{
			fprintf(stderr, "%s: can't gzopen %s\n", prog, szNewFile);
			delete szNewFile;
			return 0;
		}
		else 
		{
			gz_compress(in, out);

			CFileStatus fileState;
			if (CFile::GetStatus(sNewFile, fileState))
			{
				_ftprintf(stderr, _T("Compressed:%s (%8d bytes)\n"), sNewFile, fileState.m_size);
			}
			else
			{
			}

			_tunlink(sFile);

			int bAllOkay=0;
			CFileStatus newState;
			if (CFile::GetStatus(sNewFile, newState))
			{
				// copy old attributes
				newState.m_ctime = origState.m_ctime;
				newState.m_mtime = origState.m_mtime;
				newState.m_atime = origState.m_atime;
				newState.m_attribute = origState.m_attribute;
				// new state
				TRY 
				{
					CFile::SetStatus(sNewFile, newState);
					// fine
					bAllOkay=1;
				}
				CATCH( CFileException, e )
				{
					// FAILED
				}
				END_CATCH
				
			}
			else
			{
				// OOPS outfile not found
			}


			delete szNewFile;
			return bAllOkay;
		}
	}

}
#ifdef _UNICODE
int CLogZip::DecompressLogFile(const wchar_t *szFile)
#else
int CLogZip::DecompressLogFile(const char *szFile)
#endif
{
	int bAllOkay=0;
   char *szInfile;
   FILE  *out;
   gzFile in;

	CString sFile,sNewFile;
	sFile = szFile;

	CFileStatus origState;
	if (CFile::GetStatus(szFile, origState))
	{
		// old state
	}
	else
	{
		return 0;	// file not found
	}

	int ix = sFile.ReverseFind('.');
	// NOT YET check extenstion length
	if (ix!=-1 
		&& (sFile[ix+1]=='l' || sFile[ix+1]=='L')
		&& (sFile[ix+2]=='g' || sFile[ix+2]=='G')
		&& (sFile[ix+3]=='z' || sFile[ix+3]=='Z')
		) 
	{
		sNewFile = sFile.Left(ix)+".log";
	} 
	else
	{
		return 0;	// no log file
	}

	int nLen = sFile.GetLength();
	szInfile = new char[nLen+1];
#ifdef _UNICODE
	nLen = WideCharToMultiByte(CP_ACP, 0, LPCTSTR(sFile), nLen, szInfile, nLen, NULL, NULL);
#else
	strncpy(szInfile, LPCTSTR(sFile), nLen);
#endif
	szInfile[nLen] = 0;

	in = gzopen(szInfile, "rb");
   if (in == NULL)
	{
		fprintf(stderr, "%s: can't gzopen %s\n", prog, szInfile);
		delete szInfile;
		return 0;
   }
	else
	{
		out     = _tfopen(sNewFile, _T("wb"));
		if (out == NULL)
		{
			// perror(file);
			delete szInfile;
			return 0;
		}

		gz_uncompress(in, out);

		CFileStatus newState;
		if (CFile::GetStatus(sNewFile, newState))
		{
			// copy old attributes
			newState.m_ctime = origState.m_ctime;
			newState.m_mtime = origState.m_mtime;
			newState.m_atime = origState.m_atime;
			newState.m_attribute = origState.m_attribute;
			// new state
			TRY
			{
				CFile::SetStatus(sNewFile, newState);
				// fine
				bAllOkay=1;
			}
			CATCH( CFileException, e )
			{
				// FAILED
			}
			END_CATCH
			
		} 
		else 
		{
			// OOPS outfile not found
		}


		unlink(szInfile);
		delete szInfile;
	}
	return bAllOkay;
}
#ifdef _UNICODE
int CLogZip::CompressFile(const wchar_t *szSource, const wchar_t *szDest, int iFactor)
#else
int CLogZip::CompressFile(const char *szSource, const char *szDest, int iFactor)
#endif
{
	if ( (szSource==NULL) || (szDest==NULL))
	{
		return 0;
	}

	if ( (szSource[0]=='\0') || (szDest[0]=='\0'))
	{
		return 0;
	}

	CFileStatus origState;
	if (!CFile::GetStatus(szSource, origState)) 
	{
		return 0;	// file not found
	}

	CString sSource, sDest;

	sSource = szSource;
	sDest   = szDest;

   FILE  *in;
   gzFile out;

   in = _tfopen(sSource, _T("rb"));
   if (in == NULL) 
	{
      _tperror(sSource);

		return 0;
   } 
	else 
	{
		int nLen = sDest.GetLength();
		char *szOutfile = new char[nLen+1];
#ifdef _UNICODE
		nLen = WideCharToMultiByte(CP_ACP, 0, LPCTSTR(sDest), nLen, szOutfile, nLen, NULL, NULL);
#else
		strncpy(szOutfile, LPCTSTR(sDest), nLen);
#endif
		szOutfile[nLen] = 0;
		
		CString sCmd;
		if (iFactor<1)
		{
			iFactor = 1;
		}
		if (iFactor>9) 
		{
			iFactor = 9;
		}
		char szCmd[20];
		sprintf(szCmd,"wb%d",iFactor);

		out = gzopen(szOutfile, szCmd); /* use "wb9" for maximal compression */
		if (out == NULL) 
		{
			fprintf(stderr, "%s: can't gzopen %s\n", prog, szOutfile);
			delete szOutfile;
			return 0;
		} 
		else 
		{
			gz_compress(in, out);
			_tunlink(sSource);
			delete szOutfile;
			return 1;
		}
	}
}

#ifdef _UNICODE
int CLogZip::DecompressFile(const wchar_t *szSource,const wchar_t *szDest)
#else
int CLogZip::DecompressFile(const char *szSource,const char *szDest)
#endif
{
	if ( (szSource==NULL) || (szDest==NULL))
	{
		return 0;
	}

	if ( (szSource[0]=='\0') || (szDest[0]=='\0'))
	{
		return 0;
	}

	int bAllOkay=0;
   char *szInfile;
   FILE  *out;
   gzFile in;

	CString sSource;
	sSource = szSource;

	CFileStatus origState;
	if (CFile::GetStatus(sSource, origState)) 
	{
		// old state
	} 
	else
	{
		return 0;	// file not found
	}

	int nLen = sSource.GetLength();
	szInfile = new char[nLen+1];
#ifdef _UNICODE
	nLen = WideCharToMultiByte(CP_ACP, 0, LPCTSTR(sSource), nLen, szInfile, nLen, NULL, NULL);
#else
	strncpy(szInfile, LPCTSTR(sSource), nLen);
#endif
	szInfile[nLen] = 0;


	in = gzopen(szInfile, "rb");
   if (in == NULL) 
	{
      fprintf(stderr, "%s: can't gzopen %s\n", prog, szInfile);
		delete szInfile;
		return 0;
    } 
	else 
	{
		out = _tfopen(szDest, _T("wb"));
		if (out == NULL) 
		{
			// perror(file);
			delete szInfile;
			return 0;
		}

		gz_uncompress(in, out);
		unlink(szInfile);
		delete szInfile;
	}
	return bAllOkay;
}

int CLogZip::CompressData(unsigned char*pDest, unsigned long *pdwDestLen, const unsigned char*pSource, unsigned long dwSourceLen)
{
	return compress(pDest, pdwDestLen, (Bytef*)pSource, dwSourceLen);
}
int CLogZip::UnCompressData(unsigned char*pDest, unsigned long *pdwDestLen, const unsigned char*pSource, unsigned long dwSourceLen)
{
	return uncompress(pDest, (uLongf*)pdwDestLen, (Bytef*)pSource, dwSourceLen);
}

