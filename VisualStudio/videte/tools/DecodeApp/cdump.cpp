/////////////////////////////////////////////////////////////////////////////
//	Project:	Akubis
//
//	File:		cdump.cpp : implementation file
//
//	Description:
//
//	Start:		01.06.95
//
//	Last change:21.07.95
//
//	Author:		Georg Feddern
//				Martin Lück
//
//	Copyright:	w+k Video Communication GmbH & Co.KG
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "afxdisp.h"
#include "stdarg.h"
#include "commonrc.h"
#include "cdump.h"
#include "time.h"

///////////////////////////////////////////////////////////////////////////
// CDump::CDump
CDump::CDump(BOOL bFile)
{
	m_bOK = FALSE;
	TRACE("CDump::CDump\n");
	m_bFile = bFile;
	
    char szAppPathName[_MAX_PATH];
    memset( szAppPathName, 0, sizeof(szAppPathName) );

	HINSTANCE hInst = AfxGetApp()->m_hInstance;
    GetModuleFileName(hInst, szAppPathName, sizeof(szAppPathName));

    strcpy( strrchr( szAppPathName, '.' ), ".ini\0" );
	m_lMaxFileLen = 1024L * (long)GetPrivateProfileInt("Debug", "DebugFileLen", 512, szAppPathName);

    strcpy( (strrchr( szAppPathName, '\\' )+6), "dbg.log\0" );
	m_strFilename = szAppPathName;

    strcpy( (strrchr( szAppPathName, '\\' )+6), "dbo.log\0" );
	m_strOldFilename = szAppPathName;


	m_strFilename    = "C:\\LOG\\PTDecode.LOG";
	m_strOldFilename = "C:\\LOG\\PTDecode1.LOG";

	CFileStatus rStatus;
	CString sOldLog;
	BOOL bFound=TRUE;
	int i=1;
	CTime tOldestTime = CTime::GetCurrentTime();
	CString sOldestFile;

	// search for existing PTDECODEXXXX files
	while (bFound){
		sOldLog.Format("C:\\LOG\\PTDecode%04d.LOG",i);
		bFound = CFile::GetStatus( sOldLog, rStatus );
		if (bFound) 
		{
			if (rStatus.m_mtime<tOldestTime)
			{
				tOldestTime = rStatus.m_mtime;
				sOldestFile = sOldLog;
			}
			i++;
		}
	}

	if (i>10)
	{
		TRY
		{		
			CFile::Remove(sOldestFile);
		}
		CATCH (CFileException, cfe)
		{
		}
		END_CATCH
	}
	
	if (CFile::GetStatus( m_strFilename, rStatus ))
	{
		// now sOldLog is a 'free' name for the next backup
		CFile::Rename(m_strFilename ,sOldLog);
		// ... and write again in to CoCoSrv.log
	}
		
	if ( m_bFile )
	{
		TRY
		{
			CFile cfile((LPCSTR)m_strFilename, CFile::modeCreate|CFile::modeReadWrite);
			TRACE("File %s was created\n", (LPCSTR)m_strFilename);
			char szTime[512];
			char szDate[256];
			memset(szDate,0, sizeof(szDate));
			memset(szTime,0, sizeof(szTime));
			_strtime(szTime);
			_strdate(szDate);
			strcat(szTime, "->");
			strcat(szTime, szDate);
			strcat(szTime, "\r\n");
			TRY
			{
				cfile.Write( szTime, strlen(szTime) );	
			}
			CATCH( CFileException, e )
			{
				TRACE("File %s could not be written! Cause %i\n", (LPCSTR)m_strFilename, e->m_cause);
			}
			END_CATCH
		}
		CATCH( CFileException, e )
		{
			TRACE("File %s could not be created or opened! Cause %i\n", (LPCSTR)m_strFilename, e->m_cause);
			AfxMessageBox(MSG_NO_DEBUG_FILE, MB_OK, MSG_NO_DEBUG_FILE);
			return;
		}
		END_CATCH
	}
	m_bOK = TRUE;
}

///////////////////////////////////////////////////////////////////////////
// CDump::~CDump
CDump::~CDump()
{
	TRACE("CDump::~CDump\n");
}

///////////////////////////////////////////////////////////////////////////
// CDump::IsValid
BOOL CDump::IsValid()
{
	return m_bOK;
}

///////////////////////////////////////////////////////////////////////////
// CDump::TRACE
void __cdecl CDump::Trace(LPCSTR lpszFormat, ...)
{
	static char szString[512];
	char	*pS = NULL;
	char szTime[10];
	char szDate[10];
	static char szBuffer[512];
   	LPCSTR lpszLocalFormat;
	
	memset(szTime,0, sizeof(szTime));
	memset(szDate,0, sizeof(szDate));
	memset(szBuffer,0, sizeof(szBuffer));

	_strtime(szTime);
	_strdate(szDate);

	lpszLocalFormat = lpszFormat;

	static va_list args;
	va_start(args, lpszFormat);

	_vsnprintf(szString, sizeof(szString)-1, lpszLocalFormat, args);

	// Am Ende des Debugstrings soll IMMER \r\n stehen !
	// GF TODO
	pS = strrchr(szString, '\n');
	if ( pS==&szString[strlen(szString)-1] || pS==&szString[strlen(szString)-2] )
	{
		*pS = 0;
	}
	pS = strrchr(szString, '\r');
	if ( pS==&szString[strlen(szString)-1] || pS==&szString[strlen(szString)-2] )
	{
		*pS = 0;
	}
	strcat(szString, "\r\n");

	TRACE(szString);

	if ( m_bFile )
	{   
		memset(szBuffer,0, sizeof(szBuffer));
		strcat(szBuffer,szDate);
		strcat(szBuffer, ",");
		strcat(szBuffer,szTime);
		strcat(szBuffer, "-");
		strncat(szBuffer, szString, (sizeof(szBuffer)-strlen(szString)-1) );

		static BOOL bFileExist;
		bFileExist = FALSE;
		static CFileStatus fileStatus;

		if( CFile::GetStatus( (LPCSTR)m_strFilename, fileStatus ) )
		{
			bFileExist = TRUE;
			if ( fileStatus.m_size >= m_lMaxFileLen )
			{
				if( CFile::GetStatus( (LPCSTR)m_strOldFilename, fileStatus ) )
				{
					TRY
					{
						CFile::Remove((LPCSTR)m_strOldFilename);
						TRACE("File %s was removed\n", (LPCSTR)m_strOldFilename);
					}
					CATCH( CFileException, e )
					{
						TRACE("File %s could not be removed! Cause %i\n", (LPCSTR)m_strOldFilename, e->m_cause);
					}
					END_CATCH
				}
				TRY
				{
					CFile::Rename( (LPCSTR)m_strFilename, (LPCSTR)m_strOldFilename );
					TRACE("File %s was renamed to %s\n", (LPCSTR)m_strFilename, (LPCSTR)m_strOldFilename);
					bFileExist = FALSE;
				}
				CATCH( CFileException, e )
				{
					TRACE("File %s could not be renamed to %s! Cause %i\n", (LPCSTR)m_strFilename, (LPCSTR)m_strOldFilename, e->m_cause);
				}
				END_CATCH
			} 
		}

		if ( !bFileExist )
		{
			TRY
			{
				CFile cfile((LPCSTR)m_strFilename, CFile::modeCreate|CFile::modeReadWrite);
				TRACE("File %s was created\n", (LPCSTR)m_strFilename);
			}
			CATCH( CFileException, e )
			{
				TRACE("File %s could not be created or opened! Cause %i\n", (LPCSTR)m_strFilename, e->m_cause);
			}
			END_CATCH
		}
		
		TRY
		{
			CFile cfile((LPCSTR)m_strFilename, CFile::modeReadWrite);
			TRY
			{
				cfile.SeekToEnd();
				TRY
				{
					cfile.Write( szBuffer, strlen(szBuffer) );	
				}
				CATCH( CFileException, e )
				{
					TRACE("File %s could not be written! Cause %i\n", (LPCSTR)m_strFilename, e->m_cause);
				}
				END_CATCH
			}
			CATCH( CFileException, e )
			{
				TRACE("ERROR: SeekToEnd()! File %s Cause %i\n", (LPCSTR)m_strFilename, e->m_cause);
			}
			END_CATCH
		}
		CATCH( CFileException, e )
		{
			TRACE("File %s could not be opened! Cause %i\n", (LPCSTR)m_strFilename, e->m_cause);
		}
		END_CATCH

	}
}

