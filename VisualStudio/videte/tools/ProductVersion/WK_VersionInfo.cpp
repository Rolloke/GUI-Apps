
#include "stdafx.h"

#include "winver.h"
#include "WK_VersionInfo.h"
#include "WKClasses/WK_File.h"
#include "WKClasses/WK_WinCpp.h"

void CWK_VersionInfo::Init()
{
	m_sVersionText="-";
	m_dwFileSize = 0;
	m_tFileTime = CTime(1997,1,1,1,1,1);

	// m_sFileName;
	// m_sDirName;
	m_bIgnore = FALSE;
	
}

void CWK_VersionInfo::CheckOSVersion()
{
	COsVersionInfo  osInfo;
	// exclude NT file on non-NT systems
	if (m_sFileName.Find('!')==0) {
		m_sFileName = m_sFileName.Mid(1);
		if (osInfo.IsNT()==FALSE) {
			m_bIgnore = TRUE;
			return;
		}
	}
	// exclude 95 file on non-95 systems
	if (m_sFileName.Find('%')==0) {
		m_sFileName = m_sFileName.Mid(1);
		if (osInfo.IsWin95()==FALSE) {
			m_bIgnore = TRUE;
			return;
		}
	}
}

CWK_VersionInfo::CWK_VersionInfo(const CString &sDir,
								 const CString &sFileName,
								 BOOL bIgnoreOS
								 )
{
	// CAVEAT do not use GetFileName here

	Init();
	
	m_sFileName = sFileName;
	m_sDirName = sDir;

	CheckOSVersion();

	if (bIgnoreOS==FALSE && IgnoreFile()) {
		return;		// <EXIT>
	} else {
		// OS is ignored, so we can't ignore files by OS
		// this is usefull for creating .pvi files
	}

	BOOL bCanBeInSystem = (m_sFileName.Find('$')== 0);
	if (bCanBeInSystem) {
		// drop the the leading $
		m_sFileName = m_sFileName.Mid(1);
	}
	
	CString sFileNameToUse;

	// first try in current directory
	sFileNameToUse = sDir;
	sFileNameToUse += "\\";
	sFileNameToUse += m_sFileName;

	BOOL bFileExists = FALSE;
	if (DoesFileExist(sFileNameToUse)) {
		// fine
		bFileExists = TRUE;
	} else {
		// file not found !?
		if (bIgnoreOS==FALSE && bCanBeInSystem) {
			TCHAR szSystemDir[MAX_PATH+1];
			GetSystemDirectory(szSystemDir,MAX_PATH+1);
			sFileNameToUse = szSystemDir;
			sFileNameToUse += "\\";

			// within the system directory there are no subdirectories in .lis
			CString sResultPath; 
			CString sResultFileName;
			WK_SplitPath(m_sFileName, sResultPath, sResultFileName);

			sFileNameToUse += sResultFileName;

			if (DoesFileExist(sFileNameToUse)) {
				// fine
				bFileExists = TRUE;
			} else {
				// not even in the SYSTEMDirectory
				bFileExists = FALSE;
				WK_TRACE(_T("File not found '%s'\n"),
						sFileNameToUse
						);
			}
			/// CAVEAT redundant code
			// ditto for drivers/
			COsVersionInfo  osInfo;
			if (bFileExists==FALSE) {
				if (osInfo.IsNT()) {
					WK_TRACE(_T("Running on NT, should check drivers...\n"));
				} else {
					WK_TRACE(_T("NOT Running on NT, should check drivers...\n"));
				}
			} else {
			}
			if (bFileExists==FALSE && osInfo.IsNT()) {
				sFileNameToUse = szSystemDir;
				sFileNameToUse += _T("\\");

				// within the system directory there are no subdirectories in .lis
				CString sResultPath; 
				CString sResultFileName;
				WK_SplitPath(m_sFileName, sResultPath, sResultFileName);

				sFileNameToUse += _T("drivers\\");
				sFileNameToUse += sResultFileName;

				if (DoesFileExist(sFileNameToUse)) {
					WK_TRACE(_T("FOUND SystemFile '%s'\n"),
						sFileNameToUse
						);
					// fine
					bFileExists = TRUE;
				} else {
					// not even in the SYSTEMDirectory\drivers
					WK_TRACE(_T("SystemFile not found '%s'\n"),
						sFileNameToUse
						);
					bFileExists = FALSE;
				}
			}
		}
	}

	if (bFileExists==FALSE) {
		// no trace, there might be only one of CoCo/MiCo
		return;
	}

	const int MAX_INFO_LEN=5000;
	BYTE pData[MAX_INFO_LEN+1];
	DWORD dwDummy;
	LPTSTR szUnconst = sFileNameToUse.GetBuffer(0);
	DWORD dwLen = GetFileVersionInfoSize(szUnconst,&dwDummy);
	if (dwLen > MAX_INFO_LEN) {
		WK_TRACE_ERROR(_T("internal overflow %d !?\n"),dwLen);
		sFileNameToUse.ReleaseBuffer();
		return; // <<<EXIT >>>
	}


	BOOL bOkay = GetFileVersionInfo(
				szUnconst,	// pointer to filename string
				0,	// ignored 
				MAX_INFO_LEN,	// size of buffer
				pData// pointer to buffer to receive file-version info.
	   );	
	sFileNameToUse.ReleaseBuffer();


	BOOL bValueOkay = FALSE;
	TCHAR szValue[1000];
	UINT dwValueLen;
	void *pValueData = NULL;

	if (bOkay) {
		_stprintf(szValue,_T("\\StringFileInfo\\040904b0\\%s"),_T("FileVersion"));
		bValueOkay = VerQueryValue(
			pData,	// address of buffer for version resource
			szValue,	// address of value to retrieve
			&pValueData,	// address of buffer for version pointer
			&dwValueLen		// address of version-value length buffer
		   );

		if (bValueOkay==FALSE) {
			_stprintf(szValue,_T("\\StringFileInfo\\040704b0\\%s"),_T("FileVersion"));
			bValueOkay = VerQueryValue(
			pData,	// address of buffer for version resource
			szValue,	// address of value to retrieve
			&pValueData,	// address of buffer for version pointer
			&dwValueLen		// address of version-value length buffer
		   );
		}
	} else {
		// no version info
	}

	if (bValueOkay) {
		// printf(_T("%-20s: %s\n"),szValue,(const char*)pValueData);
		m_sVersionText = (const char*)pValueData;
		// insert missing leading zeros
		if (m_sVersionText.GetLength()>2 && m_sVersionText[1]==_T('/')) {
			m_sVersionText = _T("0") + m_sVersionText;
		}
		// at least 01/3/
		if (m_sVersionText.GetLength()>4 && m_sVersionText[4]==_T('/')) {
			m_sVersionText = m_sVersionText.Left(3)+_T("0") + m_sVersionText.Mid(3);
		}

	} else {
		// fprintf(stdout,_T("Value %s not found\n"),szValue);
		m_sVersionText = _T("-");
	}
	// NOT YET any other information than "FileVersion"
	
	CFileStatus fs;
	if (CFile::GetStatus(sFileNameToUse,fs)) {
		m_tFileTime = fs.m_mtime;
		m_dwFileSize = (DWORD)fs.m_size;
	} else {
		// failed to get file status
		m_tFileTime = CTime(1997, 1, 1, 1, 1, 1);
		m_dwFileSize = 0;
	}
}

// contructor from .pvi info
CWK_VersionInfo::CWK_VersionInfo(
		const CString & sFileName,
		const CString & sVersionText,
		DWORD	dwFileSize,
		CTime	tFileTime
		)
{
	Init();

	m_sFileName = sFileName;
	m_sVersionText = sVersionText;
	m_dwFileSize = dwFileSize;
	m_tFileTime = tFileTime;
	
	CheckOSVersion();
}

CString CWK_VersionInfo::GetString() const	// single line string representation
{
	CString sResult;
	sResult.Format(_T("%s;%d;%s;%02d.%02d.%02d,%02d:%02d"),
					m_sVersionText,
					m_dwFileSize,
					m_sFileName,
					m_tFileTime.GetDay(),
					m_tFileTime.GetMonth(),
					m_tFileTime.GetYear(),
					m_tFileTime.GetHour(),
					m_tFileTime.GetMinute()
					);
	return sResult;
}

BOOL CWK_VersionInfo::CompareWith(
			const CWK_VersionInfo &x,
			BOOL &bSameName,
			BOOL &bSameVersion,
			BOOL &bSameSize,
			BOOL &bSameTime,
			BOOL bIgnoreTime
			) const
{
	if (IgnoreFile() || x.IgnoreFile()) return TRUE;

	bSameName = (x.GetFileName()==GetFileName());
	bSameVersion = (x.GetVersionText()==GetVersionText());
	bSameSize = (x.GetFileSize()==GetFileSize());
	if (bIgnoreTime==FALSE) {
		bSameTime = (x.GetFileTime()==GetFileTime());
	} else {
		bSameTime=TRUE;
	}

	return (bSameName==TRUE && bSameVersion==TRUE && bSameSize==TRUE && bSameTime==TRUE);
}

////////////////////////////////////////////////////////////////////////
CWK_VersionDB::CWK_VersionDB()
{
	m_dwBuildNumber=0;
	//  m_sFileName;
	SetAutoDelete(TRUE);
}
BOOL CWK_VersionDB::ScanDirectory(DWORD dwBuildNumber, 
								  const CString &sDir, 
								  BOOL bIgnoreOS)
{
	CStringArray files;

	// read "vinfo.lis"
	CStdioFile listFile;
	CString sListName;
	sListName = sDir;
	sListName += _T('\\');
	sListName += _T("vinfo.lis");
	if (listFile.Open(sListName,  CFile::modeRead)) {
		CString sOneLine;
		while (listFile.ReadString(sOneLine)) {
			files.Add(sOneLine);
		}
		listFile.Close();
	} else {
		// could not read list file
	}
	// end fo read file list

	m_dwBuildNumber = dwBuildNumber;
	int i;
	for (i=0;i<files.GetSize();i++) {
		CWK_VersionInfo * pInfo = new CWK_VersionInfo(sDir,files[i],
			bIgnoreOS
			);

		if (pInfo && (pInfo->IgnoreFile()==FALSE || bIgnoreOS==TRUE)) {
			Add(pInfo);
		} else {
			WK_DELETE(pInfo);
		}
	}

	for (i=0;i<GetSize();i++) {
		CString sOneInfo = GetAt(i)->GetString();
	}
	return TRUE;
}


BOOL CWK_VersionDB::WriteToFile(const CString &sDatabase)
{
	BOOL bOkay = TRUE;
	CStdioFile fDatabase;
	if (fDatabase.Open(sDatabase,CFile::modeCreate | CFile::modeWrite)) {
		bOkay = TRUE;
		CString sHead;
		DWORD dwCheckSum=0;
		// version buildNumber number of records
		sHead.Format(_T("%d %d %d\n"),1,m_dwBuildNumber,GetSize());
		fDatabase.WriteString(sHead );
		for (int i=0;i<GetSize();i++) {
			const CWK_VersionInfo &oneRecord = * GetAt(i);
			fDatabase.WriteString(oneRecord.GetString());
			fDatabase.WriteString(_T("\n"));
		}	// end of loop over all records
		CString sFooter;
		sFooter.Format(_T("%x\n"),dwCheckSum);
		fDatabase.WriteString(sFooter);
		fDatabase.Close();
	} else {
		WK_TRACE_ERROR(_T("Could not create file '%s'\n"), sDatabase);
		bOkay = FALSE;
	}
	return bOkay;
}

static void SplitString(const CString &sIn, const char chDelimiter,
								CStringArray &result)
{
	CString s(sIn);	// make a local copy
	
	int ix=-1;
	while ((ix = s.Find(chDelimiter))!=-1) {
		result.Add( s.Left(ix) );
		s = s.Mid(ix+1);
	}
	if (s.GetLength()) {
		result.Add(s);	// trailing rest
	}

}

static CTime TimeFromString(const CString &sTime)
{
	CTime result;
	
	int iDay=0, iMonth=0, iYear=0, iHour=0, iMinutes=0;
	int iNumRead = _stscanf(sTime,_T("%d.%d.%d,%d:%d"),
					&iDay, &iMonth,&iYear,&iHour,&iMinutes
					);
	if (iNumRead == 5) {
		if (iYear<1900) {
			iYear+=1900;
		}
		result = CTime(iYear,iMonth,iDay,iHour,iMinutes,0);
	} else {
		WK_TRACE_ERROR(_T("Invalid time format '%s'\n"), sTime);
		result = CTime::GetCurrentTime();
	}

	return result;
}

// read from .pvi
BOOL CWK_VersionDB::ReadFromFile(const CString &sDatabase)
{
	BOOL bOkay = FALSE;

	m_sFileName = sDatabase;
	CStdioFile fDatabase;
	if (fDatabase.Open(sDatabase,CFile::modeRead)) {
		bOkay = TRUE;
		CString sHead;
		// version buildNumber number of records
		
		fDatabase.ReadString(sHead );
		int iVersion=0,iBuild=0,iNumber=0;
		int iNumRead=0;
		iNumRead = _stscanf(sHead,_T("%d %d %d\n"),&iVersion,&iBuild,&iNumber);
		if (iNumRead==3) {
			m_dwBuildNumber = iBuild;
			for (int i=0;i<iNumber && bOkay;i++) {
				CString sOneLine;
				if (fDatabase.ReadString(sOneLine)) {
					// format is 28/10/97,13:54;525312;DatabaseServer.exe;05.11.1997,15:35
					CStringArray args;
					SplitString(sOneLine,_T(';'),args);

					if (args.GetSize()==4) {
						DWORD dwFileSize = _ttol(args[1]);
						CTime	tFileTime = TimeFromString(args[3]);
						CWK_VersionInfo *pOneRecord = new CWK_VersionInfo (
							args[2], 
							args[0],
							dwFileSize,
							tFileTime
							);

						if (pOneRecord->IgnoreFile()==FALSE) {
							Add(pOneRecord);
						} else {
							WK_DELETE(pOneRecord);
						}
					} else {
						// invalid format for record
					}
				} else {
					bOkay = FALSE;
				}
			}	// end of loop over all records
	
		} else {
			// invalid header
			WK_TRACE_ERROR(_T("Invalid header in %s\n"), sDatabase);
			bOkay = FALSE;
		}

		CString sFooter;
		if (fDatabase.ReadString(sFooter)) {
			// sFooter.Format("%x\n",dwCheckSum);
		} else {
			WK_TRACE_ERROR(_T("Missing footer in %s\n"), sDatabase);
		}
		
		fDatabase.Close();
	} else {
		TCHAR szDirName[MAX_PATH+1];
		szDirName[0]=0;
		GetCurrentDirectory(MAX_PATH,szDirName);
		WK_TRACE_ERROR(_T("Could not read file '%s' in '%s'\n"),
						sDatabase, szDirName);
		bOkay = FALSE;
	}


	return bOkay;
}


const CWK_VersionInfo *CWK_VersionDB::GetByName(const CString &sName) const
{
	const CWK_VersionInfo *pResult = NULL;

	for (int i=0;i<GetSize() && pResult==NULL;i++) {
		WK_TRACE(_T("Compare '%s with '%s'\n"),sName,GetAt(i)->GetFileName());
		if (GetAt(i)->GetFileName()==sName) {
			pResult = GetAt(i);
		}
	}

	return pResult;
}
