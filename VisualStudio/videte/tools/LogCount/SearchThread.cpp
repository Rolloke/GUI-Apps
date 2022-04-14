// SearchThread.cpp : implementation file
//

#include "stdafx.h"
#include "logcount.h"
#include "logcountdlg.h"
#include "SearchThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSearchThread
CSearchThread::CSearchThread(CLogCountDlg* pDlg,
							 const CString& sSearchPattern,
							 const CString& sErrorPattern)
	: CWK_Thread("CSearchThread", NULL)
{
	m_pDlg = pDlg;
	m_sSearchPattern = sSearchPattern;
	m_sErrorPattern = sErrorPattern;
//	m_pMemFile = new CMemFile();
//	m_pStdioFile = NULL;
	// m_sLogFile;
	m_iCurrentIndex = 0;
	m_bSearch = FALSE;
	m_dwErrorCounter = 0;
	m_dwPatternCounter = 0;
	// m_sCurrentLine
}
//////////////////////////////////////////////////////////////////////
CSearchThread::~CSearchThread()
{
//	WK_DELETE(m_pMemFile);
}
//////////////////////////////////////////////////////////////////////
BOOL CSearchThread::Run(LPVOID /*lpContext*/)
{
	BOOL bReturn = TRUE;

	if (m_bSearch) {
		BOOL bReadLine = ReadLine();
		if (bReadLine) {
			CSystemTime stDate = ExtractDateTimeFromLine(m_sCurrentLine);
			if ( m_pDlg->IsDateInSpan(stDate) ) {
				Search();
			}
			else if ( m_pDlg->IsDateLaterThanSpan(stDate) ){
				m_bSearch = FALSE;
				m_pDlg->SearchFileProcessed();
				WK_TRACE_USER("Skip rest of file from %s, %s, too late\n",
							(LPCSTR)stDate.GetDate(), (LPCSTR)stDate.GetTime());
			}
			else {
				// date earlier, skip line
			}
		}
		else {
			// no more line, end of file reached
			m_bSearch = FALSE;
			m_pDlg->SearchFileProcessed();
		}
	}
	else {
		CString sPathFilenameExt = m_pDlg->GetNextSearchFile();
		if ( sPathFilenameExt.IsEmpty() ) {
			// Fertig
			if (m_sLastLineWithEntry.IsEmpty() == FALSE) {
				CSystemTime stDate = ExtractDateTimeFromLine(m_sLastLineWithEntry);
				WK_TRACE_USER("Last Found at %s, %s\n",
							(LPCSTR)stDate.GetDate(), (LPCSTR)stDate.GetTime());
			}
			bReturn = FALSE;
		}
		else {
			CString sTempPathFileName;
			CString sFileReadFrom;
			// Extension extrahieren
			CString sPathFileName;
			CString sExtension;
			WK_SplitExtension(sPathFilenameExt, sPathFileName, sExtension);
			// ggf. komprimierte Datei dekomprimieren
			if (sExtension.CompareNoCase(".lgz") == 0) {
				CString sPath;
				CString sFileName;
				WK_SplitPath(sPathFileName, sPath, sFileName);
				CSystemTime stDate = ExtractDateTimeFromFileName(sFileName);
				if ( m_pDlg->IsDateEarlierThanSpan(stDate) ) {
					// File braucht nicht durchsucht zu werden
					WK_TRACE_USER( "Skip File %s, to early\n", (LPCSTR)sPathFilenameExt);
					m_pDlg->SearchFileProcessed();
				}
				else {
					// Temporären Dateinamen für das zu durchsuchende File erzeugen
					char* pszTempName = _tempnam(sFileName, szTemp);
					sTempPathFileName = pszTempName;
					free(pszTempName);
					// Datei dekomprimieren
					if ( DecompressLGZFile(sPathFilenameExt, sTempPathFileName) ) {
						TRACE( "File %s decompressed to %s\n",
									(LPCSTR)sPathFilenameExt, (LPCSTR)sTempPathFileName );
						sFileReadFrom = sTempPathFileName;
					}
					else {
						WK_TRACE_ERROR( "File decompress failed %s\n", (LPCSTR)sPathFilenameExt );
						m_pDlg->SearchFileProcessed();
						sTempPathFileName.Empty();
					}
				}
			}
			else {
				sFileReadFrom = sPathFilenameExt;
				sTempPathFileName.Empty();
			}
			// Datei oeffnen
			if (sFileReadFrom.IsEmpty() == FALSE) {
				CFile file;
				if ( file.Open(sFileReadFrom, CFile::modeRead) ) {
					m_sLogFile.Empty();
					m_iCurrentIndex = 0;
					DWORD dwLen = file.GetLength();
					// Assume that logfiles smaller than 2GB
					char* pData = m_sLogFile.GetBuffer(dwLen);
					// File laden
					DWORD dwRead = file.ReadHuge(pData, dwLen);
					if (dwRead == dwLen) {
						// Decodieren
						Decode((BYTE*)pData, dwLen);
						m_sLogFile.ReleaseBuffer();
						m_bSearch = TRUE;
						WK_TRACE_USER( "Searching File %s\n", (LPCSTR)sPathFilenameExt);
					}
					else {
						WK_TRACE_ERROR( "File read failed %s read %lu of %lu\n",
											(LPCSTR)sFileReadFrom, dwRead, dwLen );
						m_pDlg->SearchFileProcessed();
					}
					file.Close();
				}
				else {
					WK_TRACE_ERROR( "File open failed %s\n", (LPCSTR)sFileReadFrom );
					m_pDlg->SearchFileProcessed();
				}
				DeleteTempFile(sTempPathFileName);
			}
			else {
				// Kein Filename, keine Arbeit
			}
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CSearchThread::Search()
{
	BOOL bFoundOne = FALSE;
	// Nur, wenn SearchPattern vorgegeben
	if (m_sSearchPattern.IsEmpty() == FALSE) {
		if ( -1 != m_sCurrentLine.Find(m_sSearchPattern) ) {
			bFoundOne = TRUE;
			m_dwPatternCounter++;
			m_sLastLineWithEntry = m_sCurrentLine;
		}
	}
	// Nur, wenn ErrorPattern vorgegeben
	if (m_sErrorPattern.IsEmpty() == FALSE) {
		if ( -1 != m_sCurrentLine.Find(m_sErrorPattern) ) {
			bFoundOne = TRUE;
			m_dwErrorCounter++;
			m_sLastLineWithEntry = m_sCurrentLine;
		}
	}
	m_pDlg->UpdateSearch(m_dwPatternCounter, m_dwErrorCounter);

	// Allerersten gefundenen Eintrag tracen
	if (bFoundOne) {
		if (	(m_dwPatternCounter==1 && m_dwErrorCounter==0)
			||	(m_dwPatternCounter==0 && m_dwErrorCounter==1)
		)
		{
			CSystemTime stDate = ExtractDateTimeFromLine(m_sLastLineWithEntry);
			WK_TRACE_USER("First Found at %s, %s\n",
						(LPCSTR)stDate.GetDate(), (LPCSTR)stDate.GetTime());
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSearchThread::StopSearchThread()
{
	BOOL bReturn = FALSE;
	if ( StopThread() ) {
		bReturn = TRUE;
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSearchThread::ReadLine()
{
	BOOL bReturn = FALSE;
	if (m_iCurrentIndex < m_sLogFile.GetLength() ) {
		int iFound = m_sLogFile.Find('\n', m_iCurrentIndex);
		if (iFound == -1){
			iFound = m_sLogFile.GetLength();
		}
		m_sCurrentLine = m_sLogFile.Mid(m_iCurrentIndex, iFound-m_iCurrentIndex);
		m_iCurrentIndex = iFound+1;
		bReturn = TRUE;
	}
	else {
		m_iCurrentIndex = 0;
		m_sLogFile.Empty();
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSearchThread::DecompressLGZFile(const CString & sName, const CString &sNew)
{
	BOOL bReturn = FALSE;
	CString sNewNameLog = sNew+".log";
	CString sNewNameLgz = sNew+".lgz";
	
	if (CopyFile(sName,sNewNameLgz,TRUE)) {
		if (WK_DecompressLogFile(sNewNameLgz)) {
			TRY {
				CFile::Rename(sNewNameLog, sNew);
				bReturn = TRUE;
			}
			CATCH( CFileException, e ) {
				WK_TRACE_ERROR( "Could not rename tmp file %s to %s\n",
									(LPCSTR)sNewNameLog, (LPCSTR)sNew);
			}
			END_CATCH
		}
		else
		{
			WK_TRACE_ERROR( "Could not decompress tmp file %s\n", (LPCSTR)sNewNameLgz);
		}
	}
	else
	{
		WK_TRACE_ERROR( "Could not copy tmp file %s to %s\n (%s)\n",
							(LPCSTR)sName, (LPCSTR)sNewNameLgz, GetLastErrorString() );
	}
	return bReturn;
}
//////////////////////////////////////////////////////////////////////
CSystemTime CSearchThread::ExtractDateTimeFromFileName(const CString& sFileName)
{
	// Im gezippten Logfilenamen gibt es ZWEI verschiedene Datumsformate
	// Altes Format
	//	UPLogfile.logDD_MM_hh_mm.lgz
	// Neues Format
	//	UPLogfile.log_JJJJMMDD_hhmmss.lgz

	int i;
	WORD wDay=0, wMonth=0, wYear=0, wHour=0, wMinute=0, wSecond=0;
	BOOL bFound = FALSE;

	// Ist es das neue Format
	CString sSearchFor = ".log_";
	i = sFileName.Find(sSearchFor, 0);
	if (i != -1) {
		int iDate = i+sSearchFor.GetLength();
		if (iDate + 15 <= sFileName.GetLength()) {
			wYear = (WORD)atoi(sFileName.Mid(iDate, 4));
			wMonth = (WORD)atoi(sFileName.Mid(iDate+4, 2));
			wDay = (WORD)atoi(sFileName.Mid(iDate+6, 2));
			wHour = (WORD)atoi(sFileName.Mid(iDate+9, 2));
			wMinute = (WORD)atoi(sFileName.Mid(iDate+11, 2));
			wSecond = (WORD)atoi(sFileName.Mid(iDate+13, 2));
			bFound = TRUE;
		}
	}

	if (bFound == FALSE) {
		// Oder ist es das alte Format
		sSearchFor = ".log";
		i = sFileName.Find(sSearchFor, 0);
		if (i != -1) {
			int iDate = i+sSearchFor.GetLength();
			if (iDate + 11 <= sFileName.GetLength()) {
				wDay = (WORD)atoi(sFileName.Mid(iDate, 2));
				wMonth = (WORD)atoi(sFileName.Mid(iDate+3, 2));
				// assume year was 1999,
				wYear = 1999;
				wHour = (WORD)atoi(sFileName.Mid(iDate+6, 2));
				wMinute = (WORD)atoi(sFileName.Mid(iDate+9, 2));
				// assume second was 00
				bFound = TRUE;
			}
		}
	}

	CSystemTime st;
	if (bFound) {
		st = CSystemTime(wDay, wMonth, wYear, wHour, wMinute, wSecond);
	}
	else {
		st = CSystemTime();
	}
	return st;
}
//////////////////////////////////////////////////////////////////////
CSystemTime CSearchThread::ExtractDateTimeFromLine(const CString& sLine)
{
	// Im Logfile gibt es VIER verschiedene Datumsformate
	// Normaler Log-Eintrag
	//	DD.MM, hh:mm:ss:
	//	DD.MM.JJJJ, hh:mm:ss:
	// StatLog-Eintrag
	//	xxxxx@D.M,h:m@
	//	xxxxx@DD.MM.JJJJ,hh:mm:ss@
	// Zugelassen werden nur die neuen Eintraege mit Jahreszahl, sonst wird 1999 angenommen

	// Datum und Zeit extrahieren
	int iStart=0, iEnd=0;
	WORD wDay=0, wMonth=0, wYear=0, wHour=0, wMinute=0, wSecond=0;

	// Ist es ein StatLog-Eintrag
	iStart = sLine.Find("@");
	if (iStart != -1) {
		iEnd = sLine.Find("@", iStart+1);
		if (iEnd != -1) {
			// End found
		}
		else {
			// Zeit geht bis zum Ende der Zeile???
			iEnd = sLine.GetLength()-1;
		}
		const char* pDate = sLine;
		if (iEnd-iStart == 19) {
			sscanf(&pDate[iStart], "%d.%d.%d,%d:%d:%d",
										&wDay, &wMonth, &wYear, &wHour, &wMinute, &wSecond);
		}
		else {
			sscanf(&pDate[iStart], "%d.%d,%d:%d",
										&wDay, &wMonth, &wHour, &wMinute);
			// assume Year was 1999
			wYear = 1999;
		}
	}
	// Oder ist es ein normaler Log-Eintrag
	else {
		const char* pDate = sLine;
		if (sLine.GetLength() >20 && sLine.GetAt(17) == ':' && sLine.GetAt(20) == ':') {
			sscanf(pDate, "%d.%d.%d, %d:%d:%d",
										&wDay, &wMonth, &wYear, &wHour, &wMinute, &wSecond);
		}
		else {
			sscanf(pDate, "%d.%d, %d:%d:%d",
										&wDay, &wMonth, &wHour, &wMinute, &wSecond);
			// assume Year was 1999
			wYear = 1999;
		}
	}

	// Auf gueltige Werte ueberpruefen
	CSystemTime st;
	if (	(1 <= wDay && wDay <= 31)
		&&	(1 <= wMonth && wMonth <= 12)
		&&	(1990 < wYear)
		&&	(0 <= wHour && wHour <= 23)
		&&	(0 <= wMinute && wMinute <= 59)
		&&	(0 <= wSecond && wSecond <= 59)
		)
	{
		// OK
		st = CSystemTime(wDay, wMonth, wYear, wHour, wMinute, wSecond);
	}
	else {
		// Ungueltiges Datum
		st = CSystemTime();
	}
	return st;
}
//////////////////////////////////////////////////////////////////////
BOOL CSearchThread::DeleteTempFile(const CString& sTempFileName)
{
	BOOL bReturn = FALSE;
	if (FALSE == sTempFileName.IsEmpty() ) {
		TRY {
			CFile::Remove(sTempFileName);
			bReturn = TRUE;
		}
		CATCH( CFileException, e ) {
			WK_TRACE_ERROR("Could not remove tmp file %s (%i)\n",
									(LPCSTR)sTempFileName, e->m_cause);
			bReturn = FALSE;
		}
		END_CATCH
	}
	else {
	}
	return bReturn;
}

