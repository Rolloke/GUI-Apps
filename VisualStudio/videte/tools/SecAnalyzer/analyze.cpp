
#include "stdafx.h"
#include "Analyze.h"
#include "SecAnalyzerDoc.h"

#include "SecAnalyzer.h"
#include "Analyze.h"
#include "MainFrm.h"

const int iMaxLineLength=1000;

// known log names
static CString sProcessLog("ProcessLog");
static CString sResetLog("ResetLog");
static CString sAlarmLog("AlarmLog");
static CString sTimerLog("TimerLog");
static CString sUnitLog("UnitLog");

static CTime cachedTime;
static int iCachedCurrentYear=0;
static int iCachedMonth=0;
static int iCachedDay=0;
static int iCachedHour=0;
static int iCachedMinute=0;
static int iCachedSecond=0;

static void StringToTime(const CString &sTime, DWORD &dwTick,
								CTime &timeResult)
{
	int year = 1999;
	int day=0,month=0;
	int h=0,m=0,s=0;

	int yx=sTime.Find(_T(".19"));
	if (yx==-1) {
		yx=sTime.Find(_T(".20"));
	}
	// NOT YET more than 2099

	int num=0;

	if (yx != -1) {
		num = _stscanf(sTime,_T("%d.%d.%d,%d:%d:%d,%u"),
			&day,&month,
			&year,
			&h,&m,&s,
			&dwTick);
		if (num==7) {
			// fine
		} else if (num==6) {
			// no dwTick given
			dwTick = (DWORD)-1;
		} else {
			WK_TRACE_ERROR(_T("Invalid time format in '%s'\n"),sTime);
		}
	} else {
		year = CTime::GetCurrentTime().GetYear();
		// no year, old format
		num = _stscanf(sTime,_T("%d.%d,%d:%d:%d,%u"),&day,&month,&h,&m,&s,&dwTick);

		if (num==6) {
			// all done
		} else if (num==5) {
			// no tick in there
			dwTick = (DWORD)-1;
		} else {
			num = _stscanf(sTime,_T("%d:%d:%d"),&h,&m,&s);
			if (num!=3) {
				// OOPS error
				TRACE(_T("invalid time format '%s'\n"),sTime);
			}
			day=CTime::GetCurrentTime().GetDay();
			month=CTime::GetCurrentTime().GetMonth();
			dwTick = (DWORD)-1;
		}
	}

	if (year==iCachedCurrentYear
		&& month==iCachedMonth
		&& day==iCachedDay
		&& h==iCachedHour
		&& m==iCachedMinute
		&& s==iCachedSecond
	) {
		timeResult= cachedTime;
	} else {
		timeResult=CTime(year, month, day,h,m,s);
		cachedTime = timeResult;
		iCachedCurrentYear = year;
		iCachedMonth = month;
		iCachedDay = day;
		iCachedHour = h;
		iCachedMinute = m;
		iCachedSecond = s;
	}
}

static void SplitDetail(CString &sName, int delim1,
					   BOOL bDoLevel1, BOOL bDoLevel2
				)
{
	CString sMain,sDetail1,sDetail2;
	sMain = sName.Left(delim1);
	sDetail1 = sName.Right(sName.GetLength()-delim1-1);
	int delim2 = sDetail1.Find(_T('|'));
	if (delim2 != -1) {
		sDetail2 = sDetail1.Right(sDetail1.GetLength()-delim2-1);
		sDetail1 = sDetail1.Left(delim2);
	}
	sName = sMain;
	if (bDoLevel1 && sDetail1.GetLength()) {
		sName += _T("|");
		sName += sDetail1;
	}
	if (bDoLevel2 && sDetail2.GetLength()) {
		sName += _T("|");
		sName += sDetail2;
	}
}

// split in one path to avoid multiple Find('@')
static void SplitFields(const CString &sIn,
					CString &sLogName,
					CString &sTime,
					CString &sState,
					CString &sName
				   )
{
	// there is no need to clear the string, the assignment will do the job

	static TCHAR szTmp[iMaxLineLength];
	int x=0;

	szTmp[0]=0;
	// sIn looks like this BitrateLog@20.07,11:42:26@12@FPS
	// in states that is 00000@111111@22222222@done
	int iState=0;
	// 0: collect logname
	// 1: collect time
	// 2: collect state
	// 3: 'collect' name not used
	// 4: done
	int iLength = sIn.GetLength();
	LPCTSTR szIn = sIn;
	TCHAR ch;
	for (int i=0;iState!=4 && i<iLength;i++) {
		ch = szIn[i];
		if (ch == '@') {
			switch(iState) {
				case 0:
					sLogName = szTmp;
					// sTmp.Empty();
					szTmp[0]=0;
					x=0;
					iState = 1;
					break;
				case 1:
					sTime= szTmp;
					szTmp[0]=0;
					x=0;
					iState = 2;
					break;
				case 2:
					sState= szTmp;
					szTmp[0]=0;
					x=0;
					sName = ((LPCTSTR)sIn)+i+1;

					//
					// NOT YET CSecAnalyzerDoc::alreadyKnownNames;
					//
					iState = 4;
					break;
			}	// end of state switch
		} else {
			szTmp[x++]=ch;
			szTmp[x]=0;
		}
	}

}

static void ReadValues(const CString &sIn,
					   CString &sName,
					   CTime &t,
					   int &iValue,
					   CString &sLogName,
					   DWORD &dwTick,
					   BOOL bDoLevel1,
					   BOOL bDoLevel2,
					   BOOL &bFullPeak
					   )
{
	// int x=sIn.Find('@');

	dwTick = (DWORD)-1;

	static CString sTime;
	static CString sState;

	SplitFields(sIn,sLogName, sTime, sState, sName);
	// try to reduce CString constructor/destructor
	// use the static strings if possible

	if (sLogName==sResetLog) sLogName=sResetLog;
	else if (sLogName==sProcessLog) sLogName=sProcessLog;
	else if (sLogName==sAlarmLog) sLogName=sAlarmLog;
	else if (sLogName==sTimerLog) sLogName=sTimerLog;
	else if (sLogName==sUnitLog) sLogName=sUnitLog;

	StringToTime(sTime,dwTick,t);

	int ixAlready = CSecAnalyzerDoc::m_knownGroups.MapCheck(sLogName)-1;

	if (ixAlready>=0) {
		// already known
		sLogName = * CSecAnalyzerDoc::m_knownGroups[ixAlready];
	} else {
		// add new group
		CSecAnalyzerDoc::m_knownGroups.Add(new CWK_String(sLogName));
		CSecAnalyzerDoc::m_knownGroups.UpdateMap();
	}

	// full name or selected details ?
	if (bDoLevel1==FALSE || bDoLevel2==FALSE) {
		int delim1=sName.Find('|');
		if (delim1 != -1) {
			SplitDetail(sName,delim1, bDoLevel1, bDoLevel2);
		} else {
			// no detail in name, nothing to do
		}
	}
	
	if (sState[0]=='~') {
		iValue = _ttoi(((LPCTSTR)sState)+1);	// skip leading ~
		bFullPeak=TRUE;
	} else {
		iValue = _ttoi(sState);
		bFullPeak=FALSE;
	}
}



// CAVEAT looks for pattern at start of line only
static inline BOOL IsLog(const CString &sOneLine, const CString &pattern)
{
	int len=pattern.GetLength();
	return (_tcsncmp(sOneLine,pattern,len)==0);
}

void CSecAnalyzerDoc::ScanFile()
{
	CWaitCursor myCursor;

	GetValuesFromDialog();
	CString sTitle = "Reading "+m_sFilename+" ...";
	SetTitle(sTitle);

	CString sOneLine;

	WK_DELETE(m_pRecords);
	m_pRecords = new CStatRecords;
	
	m_lastUpdateTime = CTime::GetCurrentTime();
	CTime ct=m_lastUpdateTime;

	m_startTick = (DWORD)-1;

	CWK_Strings theLines;
	theLines.SetSize(0,500);

	CStringArray upFiles;
	CFileFind fileFinder;
	CString sPattern;
	CString sPath;
	CString sBasename;
	WK_SplitPath(m_sFilename, sPath, sBasename);

	if (m_iNumOldFiles>0 && m_sFilename.Find(_T("UP"))==-1) {
		sPattern.Format(_T("%s\\UP%s*"),sPath,sBasename);
		BOOL bFoundSome = fileFinder.FindFile(sPattern);
		if (bFoundSome) {
			while (fileFinder.FindNextFile()) {
				CString sUpFile = fileFinder.GetFileName();
				BOOL bInserted=FALSE;
				CString sFullPath = fileFinder.GetRoot();
				sFullPath += _T("\\");
				sFullPath += sUpFile;
				for (int i=0;i<upFiles.GetSize() && bInserted==FALSE;i++) {
					if (sFullPath < upFiles[i]) {
						upFiles.InsertAt(i,sFullPath);
						bInserted=TRUE;
					}
				}
				if (bInserted==FALSE) {
					upFiles.Add(sFullPath);
				}
			}
			// OOPS redundant code for the last file
				CString sUpFile = fileFinder.GetFileName();
				BOOL bInserted=FALSE;
				CString sFullPath = fileFinder.GetRoot();
				sFullPath += _T("\\");
				sFullPath += sUpFile;
				for (int i=0;i<upFiles.GetSize() && bInserted==FALSE;i++) {
					if (sFullPath < upFiles[i]) {
						upFiles.InsertAt(i,sFullPath);
						bInserted=TRUE;
					}
				}
				if (bInserted==FALSE) {
					upFiles.Add(sFullPath);
				}


		} else {
			// no files matched the pattern
		}
		for (int i=0;i<upFiles.GetSize();i++) {
			TRACE(_T("UPfile[%d]: %s\n"),i,upFiles[i]);
		}
	}
	while (m_iNumOldFiles>0 && upFiles.GetSize()>m_iNumOldFiles) {
		upFiles.RemoveAt(0);	// dropped oldest files
	}
	
	upFiles.Add(m_sFilename);	// always add this one

	
	for (int fi=0;fi<upFiles.GetSize();fi++) {
		// update dialog window
		if (GetMain()) {
			CString sCount;
			sCount.Format(_T("%d/%d"), fi+1, upFiles.GetSize());
			GetMain()->m_wndSecAnalyzerBar.SetDlgItemText(
								IDC_NUM_FILES,
								sCount
								);
		}

	CFile f;
	static char tmpLine[iMaxLineLength];
	CString sOneFile(upFiles[fi]);

	BOOL bDecompressed =FALSE;

	if (sOneFile.Find(_T(".lgz")) != -1 || sOneFile.Find(_T(".LGZ")) != -1) {
		// a compressed file
		// needs decompression
		CString sNewName;
		bDecompressed = DecompressLGZFile(sOneFile, sNewName);
		if (bDecompressed) {
			sOneFile= sNewName;
		} else {

		}
	}

	if (f.Open(sOneFile,CFile::modeRead|CFile::shareDenyNone)) {
		TRACE(_T("READING %s\n"),sOneFile);
		// NOT YET what about .lgz
		// should delete ALL Tmp file afterwards

		// on the fly search
		// mode 0 nothing
		//		1 L
		//		2 o
		//		3 g
		//		4 @
		DWORD dwLen = f.GetLength();
		
		// read whole file in one Read
		BYTE *pTmp = new BYTE[dwLen];
		int iNumRead = f.Read(pTmp,dwLen);
		if ((DWORD)iNumRead!=dwLen) {
			TRACE(_T("Could not read whole file!?\n"));
			ASSERT(0);
		}
		f.Close();
	
		Decode(pTmp, dwLen);

		int ix=0;
		int lx=0;
		int mode=0;

		int lastSpace=0;

		while ((DWORD)ix<dwLen) {
			BYTE ch=pTmp[ix++];	// read ch 
			ASSERT(lx>=0 && lx<iMaxLineLength);
			tmpLine[lx++]=ch;	// fill line
			
			switch (mode) {
				case 0:	// nothing found yet
					if (ch=='L') mode=1; 
					break;
				case 1:	// L found
					if (ch=='o') mode=2; 
					else mode=0;
					break;
				case 2:	// Lo found
					if (ch=='g') mode=3; 
					else mode=0; 
					break;
				case 3:	// Log found
					if (ch=='@') mode=4;
					else mode=0;
					break;
				case 4:
					// fine Log@ found
					break;
				default:
					TRACE("Wrong mode %d\n",mode);
			}
			// OLD FORMAT "16.09, 15:23:57->Server--> ResetLog@16.09,15:23:57@1@ServerActive"
			// OLD FORMAT so the string is valid right after the -->SPACE
			// NEW FORMAT "ResetLog@23.04,07:22:52,67318629@0@NrBChannels"

			if (mode==0 && (ch==' ' || ch=='\t')) {
				lastSpace=lx-1;
				// CAVEAT used in IsLog()!
			}
			if (ch=='\n' || ch=='\r') lx--;	// suppress
			if (ch=='\n' || lx==iMaxLineLength-1) {
				tmpLine[lx]=0;	// terminate string
				if (mode==4) {
					if (lastSpace) {
						theLines.Add(new CWK_String(tmpLine+lastSpace+1));	// OOPS OBSOLTE ???
					} else {
						theLines.Add(new CWK_String(tmpLine));
					}
				} else {
					// collect as reportLines tmpLine;
				}
				// and again
				lx=0;
				mode=0;
				lastSpace=0;
			}
		}	// end of loop over dwLen
		WK_DELETE_ARRAY(pTmp);
	} else {
		TRACE("Failed to open %s\n",sOneFile);
	}
	if (bDecompressed) {
		// delete tmp file, sigh each time
		CFile::Remove(sOneFile);
	} 
		// update dialog window
		if (GetMain()) {
			CString sCount;
			sCount.Format(_T("%d"), theLines.GetSize());
			if ( IsWindow( GetMain()->m_wndSecAnalyzerBar ) ) {
				GetMain()->m_wndSecAnalyzerBar.SetDlgItemText(
								IDC_NUM_LINES_READ,
								sCount
								);
			}
		}
	}	// end of fi loop over upFiles

	// line values
	CString sName,sLogName;
	CTime t;
	int iValue;
	DWORD dwTick;
//	DWORD dwMinTickFound = -1;
	DWORD dwFirstTick = (DWORD)-1;
	BOOL bIsFullPeak = FALSE;

	// set reasonable start default value
	if (theLines.GetSize()) {
		const CString &sFirstLine = *theLines[0];
		ReadValues(sFirstLine, sName, t, iValue, sLogName,dwTick,m_bIncludeLevel1,m_bIncludeLevel2,
						bIsFullPeak
						);
		if (m_bLastReset && !m_bRelativeEnd) {
			m_startTime = t;	// in case no reset is found use first time
		}
		dwFirstTick = dwTick;
	}

	if (m_bLastReset && !m_bRelativeEnd) {
		// calc start time automagically
		CString sName;
		for (int i=0;i<theLines.GetSize();i++) {
			const CString &sOneLine = *theLines[i];
			if (sOneLine.Find(_T("Active"))!=-1
				&& (sOneLine.Find(_T("@ServerActive"))!=-1
						|| sOneLine.Find(_T("@Active"))!=-1
					)
				) {
				if (IsLog(sOneLine,sResetLog) ) {
					TRACE(_T("RESET LINE %s\n"),sOneLine);
					// "ProcessLog@15:07:08@OFF@SWAlarm1"
					ReadValues(sOneLine, sName, t, iValue, 
								sLogName,dwTick,
								m_bIncludeLevel1,m_bIncludeLevel2,
								bIsFullPeak
							);

					if (iValue && (m_startTime<t)) {
						TRACE("RESET from %s\n",sOneLine);
						m_startTime=t;
						m_startTick = dwTick;
					} else if (iValue) {
						CString sTime1, sTime2;
						sTime1 = t.Format("[%H:%M:%S, %d.%m.%y]");
						sTime2 = m_startTime.Format("[%H:%M:%S, %d.%m.%y]");
						TRACE("ResetTimes %s <= %s\n",sTime1,sTime2);
					}
				}
			}
		}
		m_bUseStartTime=TRUE;
	}
	
	if (m_bRelativeEnd) {
		// calc end time automagically
		m_endTime = CTime(1994,1,1,1,1,1);
		for (int i=0;i<theLines.GetSize();i++) {
			const CString &sOneLine = *theLines[i];
			// "ProcessLog@15:07:08@OFF@SWAlarm1"
			ReadValues(sOneLine, sName, t, iValue,sLogName,dwTick,m_bIncludeLevel1,m_bIncludeLevel2,
										bIsFullPeak
						);

			if (t>m_endTime) {
				m_endTime=t;
				m_endTick = dwTick;
			}
		}
		m_bUseEndTime=TRUE;
		m_bUseStartTime=TRUE;
		m_startTime = m_endTime - m_endSpan;
		m_startTick = max(0,m_endTick - m_endSpan.GetTotalSeconds()*1000);
	}	// end of m_bRelativeEnd


//	int iNumServerResets=0;
	BOOL bFirstOne=TRUE;
	CString sTmpLogName;

	if (theLines.GetSize()) {
		CString sName;
		for (int i=0;i<theLines.GetSize();i++) {
			const CString &sOneLine = *theLines[i];
			// "ProcessLog@15:07:08@OFF@SWAlarm1"
			dwTick = (DWORD)-1;
			BOOL bInclude;

			int ix2=sOneLine.Find('@');
			sTmpLogName=sOneLine.Left(ix2);
			if (CSecAnalyzerDoc::m_usedGroups.GetSize()==0) {
				bInclude=TRUE;
			} else {
				int ixAlready = CSecAnalyzerDoc::m_usedGroups.MapCheck(sTmpLogName)-1;
				if (ixAlready>=0) {
					bInclude = TRUE;
				} else {
					bInclude=FALSE;
				}
			}

			CTime t;
			int iValue=0;
			if (bInclude || 
				(sOneLine.Find(_T("Active"))!=-1
					&& (sOneLine.Find(_T("@ServerActive"))!=-1
						|| sOneLine.Find(_T("@Active"))!=-1
						)
					)
				) {
				// this is the main ReadValues
				ReadValues(sOneLine, sName, t, iValue,sLogName,dwTick,m_bIncludeLevel1,m_bIncludeLevel2,
											bIsFullPeak
						);
			}

			if (bInclude && m_includePatterns.GetSize()) {
				BOOL bFound=FALSE;
				for (int ip=0;ip<m_includePatterns.GetSize() && bFound==FALSE;ip++) {
					bFound = (sName.Find(m_includePatterns[ip])!=-1);
				}
				bInclude = bFound;
			} 
			if (bInclude && m_excludePatterns.GetSize() ) {
				for (int ep=0;ep<m_excludePatterns.GetSize() && bInclude==TRUE;ep++) {
					bInclude = (sName.Find(m_excludePatterns[ep]) == -1);
				}
			}
			DWORD dwValue=abs(iValue);

			BOOL bExcludedByTime=FALSE;
			if (bInclude && m_bUseStartTime && t<m_startTime) {
				// set value at startTime
				m_pRecords->AddRecord(sLogName, sName,m_startTime, dwValue, m_startTick, TRUE);
				bInclude = FALSE;
				bExcludedByTime=TRUE;
			}
			if (bInclude && m_bUseEndTime && t>m_endTime) {
				bInclude = FALSE;
				bExcludedByTime=TRUE;
			}
		
			if (bInclude) {
				if (bFirstOne) {
					if (m_bUseStartTime==FALSE) {
						m_startTick = dwTick;
						m_endTick = dwTick;
					} else {
						// if the 'first' value is more than 15 minutes OOPS away
						// from the startTime diasable miliseconds
						int deltaSeconds = (t-m_startTime).GetTotalSeconds();
						if (deltaSeconds<60*15) { 
							m_startTick = dwTick;
							m_endTick = dwTick;
						} else {
							m_startTick = (DWORD)-1;
							m_endTick = (DWORD)-1;
						}
					}
					bFirstOne = FALSE;
				}
				if (dwTick!=-1 && dwTick < m_startTick) {
					m_startTick = (DWORD)-1;	// mot than one resets
				}
				if (dwTick!=-1 && dwTick > m_endTick) {
					m_endTick = dwTick;
				}


				if (iValue<0) {
					// add peek value
					m_pRecords->AddRecord(sLogName, sName,t, dwValue+1,dwTick);
				}
				// real value
				// OOPS not correct for unsorted time
				m_pRecords->AddRecord(sLogName,sName,t, dwValue,dwTick);
				if (bIsFullPeak) {
					m_pRecords->AddRecord(sLogName, sName,t, 0,dwTick);
				}
			}	// end of bInclude
		}	// end of read string loop
	} else {
		TRACE(_T("Could not open %s\n"),m_sFilename);
	}
	SetTitle(m_sFilename);
	theLines.DeleteAll();

	if (GetMain() && IsWindow(GetMain()->m_wndSecAnalyzerBar )) {
		CString sMilli;
		if (m_startTick != -1) {
			sMilli = _T("mili\nsec.");
		}
		GetMain()->m_wndSecAnalyzerBar.SetDlgItemText(
							IDC_TXT_DOES_MILLI,
							sMilli
							);
	}

	CString sTime;
	sTime = m_startTime.Format(_T("[%H:%M:%S]"));
	TRACE(_T("Start %s,%u\n"), sTime,m_startTick);
	sTime = m_endTime.Format(_T("[%H:%M:%S]"));
	TRACE(_T("End %s,%u (ms %u)\n"), sTime,m_endTick,m_endTick-m_startTick);

	if (m_pRecords) {

	}

	// update  the check listbox
	if (GetMain() && m_usedGroups.GetSize()==0) {
		GetMain()->m_wndSecAnalyzerBar.UpdateGroups();
	}


}	// end of ScanFile

void CStatRecord::AddValue(CTime t, DWORD dwValue, DWORD dwTick)
{
	CStatValue *pValue = new CStatValue(t,dwValue,dwTick);
	if (m_values.GetSize()) {
		if (t >= m_values.GetAt(m_values.GetSize()-1)->m_time) {
			// NOT YET sort dwTick too
			m_values.Add(pValue);
		} else {	// insert
			CString t1 = m_values.GetAt(m_values.GetSize()-1)->m_time.Format(_T("%c"));
			CString t2 = t.Format(_T("%c"));
			TRACE(_T("unsorted time new %s, last %s for '%s'\n"),t2,t1,m_sName);
			int i=0;
			while (i<m_values.GetSize() && m_values.GetAt(i)->m_time < t) {
				i++;
			}
			m_values.InsertAt(i,pValue);
		}
		if ((DWORD)(abs((double)dwValue)) > m_dwMaxValue) {
			m_dwMaxValue = abs((double)dwValue);
		}
		if ((DWORD)(abs((double)dwValue)) < m_dwMinValue) {
			m_dwMinValue = abs((double)dwValue);
		}
	} else {
		// no values at all
		m_values.Add(pValue);
		m_dwMaxValue = dwValue;
		m_dwMinValue = dwValue;
		m_Sum = 0;
	}
	m_Sum += dwValue;
}	

int CStatRecord::GetAverage()
{
	if (m_values.GetSize())
	{
		return (int)(m_Sum / m_values.GetSize());
	}
	else
	{
		return 0;
	}
}
//////////////////////


void CStatRecords::AddRecord(const CString &sLog,
							 const CString &sName, 
							 CTime t, 
							 DWORD dwValue,
							 DWORD dwTick,
							 BOOL bAsMinStartTime
							 )
{
	if (sName.GetLength()<3) {
		TRACE(_T("Short Name '%s'\n"),sName);
	}
	CStatRecord *pRecord = NULL;
	// search for existing names
	int i;
	static int lastIndex=-1;
	if (lastIndex!=-1 && lastIndex <GetSize()) {
		if (GetAt(lastIndex)->GetName()==sName) {
			pRecord = GetAt(lastIndex);		
		}
	}
	for (i=0;i<GetSize() && pRecord==NULL;i++) {
		if (GetAt(i)->GetName()==sName) {
			pRecord = GetAt(i);		
			lastIndex=i;
		}
	}
	if (pRecord==NULL) {
		// add new record
		pRecord = new CStatRecord(sName);
		if (GetSize()==0) {
			Add(pRecord);
		} else {
			i=0;
			while (i<GetSize() && GetAt(i)->GetName() > sName) {
				i++;
			}
			InsertAt(i,pRecord);
		}
	}
	if (bAsMinStartTime==FALSE) {	// plain add
		pRecord->AddValue(t,dwValue,dwTick);
	} else {	// collect latest start time
		if (pRecord->m_values.GetSize()==0) {
			pRecord->AddValue(t,dwValue,dwTick);
		} else {
			// OOPS assumes right order
			pRecord->m_values[0]->m_dwValue = dwValue;
		}
	}
	if (pRecord->m_sType!=sLog) {
		pRecord->m_sType= sLog;
	}
}

DWORD CStatRecord::GetMinMax(CTime &startTime, CTime &endTime) const
{
	startTime = m_values[0]->m_time;
	endTime = m_values[m_values.GetSize()-1]->m_time;
	return m_values[0]->m_dwTick;
}

DWORD CStatRecords::GetMinMax(CTime &startTime, CTime &endTime) const
{
	CStatRecord *pRecord = NULL;
	DWORD minTick = (DWORD)-1;
	if (GetSize()) {
		pRecord=GetAt(0);
		minTick = pRecord->GetMinMax(startTime,endTime);
	}
	
	for (int i=1;i<GetSize();i++) {
		pRecord=GetAt(i);
		CTime rmin,rmax;
		DWORD dwTmp = pRecord->GetMinMax(rmin,rmax);
		if (rmin < startTime) {
			startTime = rmin;
			minTick = dwTmp;
		}
		if (rmax > endTime) endTime = rmax;
	}
	return minTick;
}

void CWK_HashStrings::UpdateMap()
{
	TRACE(_T("UpdateMap for %d entries\n"),GetSize());
	// fill map for faster lookups
	m_map.RemoveAll();
	CString sKey;

	for (int i=0;i<GetSize();i++) {
		void *ptr = (void *)-1;
		sKey = *GetAt(i);

		BOOL bFound = m_map.Lookup(sKey, ptr);
		if (bFound==FALSE) {
			TRACE(_T("Insert key for %s\n"),sKey);
			m_map.SetAt(sKey,(void *)(i+1));	// ad the index as value
		}
	}
}

int CWK_HashStrings::MapCheck(const CWK_String &s)
{
	void *ptr = (void *)-1;

	if (m_map.Lookup(s, ptr)) {
	} else {
		ptr = (void *) -1;
	}

	return (int)ptr;
}






