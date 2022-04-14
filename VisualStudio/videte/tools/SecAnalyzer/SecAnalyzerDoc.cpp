// SecAnalyzerDoc.cpp : implementation of the CSecAnalyzerDoc class
//

#include "stdafx.h"
#include "SecAnalyzer.h"

#include "SecAnalyzerDoc.h"
#include "Analyze.h"
#include "MainFrm.h"
#include "AlarmPlayer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSecAnalyzerDoc

IMPLEMENT_DYNCREATE(CSecAnalyzerDoc, CDocument)

BEGIN_MESSAGE_MAP(CSecAnalyzerDoc, CDocument)
	//{{AFX_MSG_MAP(CSecAnalyzerDoc)
	ON_COMMAND(ID_ALARM_PLAYBACK, OnAlarmPlayback)
	ON_COMMAND(ID_STOP_PLAYER, OnStopPlayer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSecAnalyzerDoc construction/destruction

CWK_HashStrings CSecAnalyzerDoc::m_knownGroups;	// static member
CWK_HashStrings CSecAnalyzerDoc::m_usedGroups;	// static member

CSecAnalyzerDoc::CSecAnalyzerDoc()
{
	m_knownGroups.SetAutoDelete(TRUE);

	m_pRecords=NULL;
	//
	// CStringArray m_includePatterns;
	// CStringArray m_excludePatterns;
	//
	m_bLastReset=FALSE;
	// CTime	m_startTime;
	m_bUseStartTime=FALSE;
	// CTime	m_endTime;
	m_bUseEndTime=FALSE;
	//
	m_iNumOldFiles=0;
	//
	m_bRelativeEnd=FALSE;
	//
	m_bShowProcessInput=TRUE;
	m_bShowProcessOutput=TRUE;

	m_bAutoUpdate=FALSE;

	m_sFilename = GetApp()->m_sDefaultFile;	// OLD "C:\\log\\server.log";
	m_lastUpdateTime=CTime::GetCurrentTime();

	m_bIncludeLevel1=TRUE;
	m_bIncludeLevel2=TRUE;

	m_pPlayer=NULL;

	m_startTime = CTime(1994,1,1,1,1,1);
	m_endTime = CTime(1999,1,1,1,1,1);

	GetApp()->m_pDoc=this;
}

CSecAnalyzerDoc::~CSecAnalyzerDoc()
{
	DeleteTmpFile();
	//
	WK_DELETE(m_pRecords);
	WK_DELETE(m_pPlayer);
	m_includePatterns.RemoveAll();
	m_excludePatterns.RemoveAll();
	GetApp()->m_pDoc=NULL;
}


static void SliceQuotedStrings(const CString &sIn,
						  CStringArray &subStrings)
{
	CString s(sIn);
	s.TrimLeft();
	s.TrimRight();

	int ixFirst;
	int ixSecond;
	while ((ixFirst=s.Find('"'))!=-1)
	{
		// drop first quote
		if (ixFirst==0) {
			s = s.Mid(ixFirst+1);
			ixSecond = s.Find('"');
			if (ixSecond == -1) 
			{
				// missing closing quote
			}
			else 
			{
				//fine cut and slice
				CString one = s.Left(ixSecond);
				subStrings.Add(one);
				s = s.Mid(ixSecond+1);
			}
		}
		else
		{
			// non leading quote, sigh
			// OOPS what now,, which side to drop
			// NOT YET
			CString one = s.Left(ixFirst);
			subStrings.Add(one);
			s = s.Mid(ixFirst+1);
		}
		s.TrimLeft();
	}	// end of loop over quote

	if (s.GetLength())
	{
		subStrings.Add(s);	// trailing rest
	}
}

static void SlicePatternString(const CString &sPatternIn,
						  CStringArray &patterns,
						  const char chDelimiter=' '
						  )
{
	patterns.RemoveAll();	// drop old values
	if (sPatternIn.IsEmpty()) return;	// EXIT

	CStringArray subStrings;
	SliceQuotedStrings(sPatternIn,subStrings);

	for (int i=0;i<subStrings.GetSize();i++) {
		CString sPattern(subStrings[i]);
		TRACE(_T("SubString[%2d]='%s'\n"),i,sPattern);
		sPattern.TrimLeft();
		sPattern.TrimRight();
		int ixSpace;
		
		while ((ixSpace=sPattern.Find(chDelimiter))!=-1) {
				CString onePattern = sPattern.Left(ixSpace);
				patterns.Add(onePattern);
				sPattern=sPattern.Right(sPattern.GetLength()-ixSpace-1);
		}
		// last one
		patterns.Add(sPattern);
	}
}


void CSecAnalyzerDoc::GetValuesFromDialog()
{
	if (GetMain() && IsWindow(GetMain()->m_wndSecAnalyzerBar)) {
		// include patterns
		CWnd *pWnd = GetMain()->m_wndSecAnalyzerBar.GetDlgItem(IDC_INCLUDE_PATTERN);
		CString sPattern;
		if (pWnd) {
			pWnd->GetWindowText(sPattern);
			SlicePatternString(sPattern,m_includePatterns);
		}
		// exclude patterns
		pWnd = GetMain()->m_wndSecAnalyzerBar.GetDlgItem(IDC_EXCLUDE_PATTERN);
		if (pWnd) {
			pWnd->GetWindowText(sPattern);
			SlicePatternString(sPattern,m_excludePatterns);
		}
		// num old files
		pWnd = GetMain()->m_wndSecAnalyzerBar.GetDlgItem(IDC_NUM_OLD_FILES);
		if (pWnd) {
			pWnd->GetWindowText(sPattern);
			m_iNumOldFiles = _ttoi(sPattern);
		} else {
			m_iNumOldFiles = 0;
		}

		// start time
		CString sTime, sDate;
		pWnd = GetMain()->m_wndSecAnalyzerBar.GetDlgItem(IDC_START_TIME);
		if (pWnd) {
			pWnd->GetWindowText(sTime);
		}
		pWnd = GetMain()->m_wndSecAnalyzerBar.GetDlgItem(IDC_START_DATE);
		if (pWnd) {
			pWnd->GetWindowText(sDate);
		}
		CTime ct=CTime::GetCurrentTime();

		int hour=0,minutes=0,day=0,month=0,seconds=0;
		int iYear = ct.GetYear();

		m_bUseStartTime = TRUE;	// reset if no field is set
		if (sTime.GetLength()) {
			if (sDate.GetLength()) {	// time and date
				_stscanf(sTime,_T("%d:%d"),&hour,&minutes);
				if (StringHasYear(sDate)) {
					_stscanf(sDate,_T("%d.%d.%d"),&day,&month,&iYear);
				} else {
					_stscanf(sDate,_T("%d.%d"),&day,&month);
				}
				ValidateYear(iYear);
			} else {	// time only
				day = ct.GetDay();
				month = ct.GetMonth();
				if (sTime.Find(_T('~'))==-1) {
					_stscanf(sTime,_T("%d:%d"),&hour,&minutes);
				} else {
					_stscanf(sTime,_T("~%d"),&seconds);
					hour=0;
					minutes=0;
				}
			}
		} else {	// no start time, check for the date
			hour=minutes=0;
			if (sDate.GetLength()) {
				if (StringHasYear(sDate)) {
					_stscanf(sDate,_T("%d.%d.%d"),&day,&month,&iYear);
				} else {
					_stscanf(sDate,_T("%d.%d"),&day,&month);
				}
				ValidateYear(iYear);
			} else {
				// neithert StartTime nor StartDate
				m_bUseStartTime=FALSE;
			}
		}
		if (m_bUseStartTime) {
			m_startTime = CTime(iYear, month, day, hour, minutes, seconds);
		} else {
			m_startTime = CTime(1994,1,1,1,1,1);	// OOPS
		}
		m_endSpan = CTimeSpan(0,hour,minutes,seconds);	// OOPS month,day?

		// same procedure for end time
		
		// end time
		m_bRelativeEnd = GetMain()->m_wndSecAnalyzerBar.IsDlgButtonChecked(IDC_RELATIVE_END);

		pWnd = GetMain()->m_wndSecAnalyzerBar.GetDlgItem(IDC_END_TIME);
		if (pWnd) {
			pWnd->GetWindowText(sTime);
		}
		pWnd = GetMain()->m_wndSecAnalyzerBar.GetDlgItem(IDC_END_DATE);
		if (pWnd) {
			pWnd->GetWindowText(sDate);
		}
		m_bUseEndTime = TRUE;	// reset if no field is set
		if (sTime.GetLength()) {
			if (sDate.GetLength()) {	// time and date
				_stscanf(sTime,_T("%d:%d"),&hour,&minutes);
				if (StringHasYear(sDate)) {
					_stscanf(sDate,_T("%d.%d.%d"),&day,&month,&iYear);
				} else {
					_stscanf(sDate,_T("%d.%d"),&day,&month);
				}
				ValidateYear(iYear);
			} else {	// time only
				day = ct.GetDay();
				month = ct.GetMonth();
				_stscanf(sTime,_T("%d:%d"),&hour,&minutes);
			}
		} else {	// no time
			hour=minutes=0;
			if (sDate.GetLength()) {
				if (StringHasYear(sDate)) {
					_stscanf(sDate,_T("%d.%d.%d"),&day,&month,&iYear);
				} else {
					_stscanf(sDate,_T("%d.%d"),&day,&month);
				}
				ValidateYear(iYear);
			} else {
				m_bUseEndTime=FALSE;
			}
		}
		if (m_bUseEndTime) {
				m_endTime = CTime(iYear,
							month, day, hour, minutes, 0);
		} else {
		}

		// last reset
		m_bLastReset = GetMain()->m_wndSecAnalyzerBar.IsDlgButtonChecked(IDC_LAST_RESET);
		// process details
		m_bIncludeLevel1 = GetMain()->m_wndSecAnalyzerBar.IsDlgButtonChecked(IDC_SHOW_LEVEL_1);
		m_bIncludeLevel2 = GetMain()->m_wndSecAnalyzerBar.IsDlgButtonChecked(IDC_SHOW_LEVEL_2);
	}
}


BOOL CSecAnalyzerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// CAVEAT do not touch usedGroups here (it's a rescan)
	m_knownGroups.DeleteAll();
	m_knownGroups.UpdateMap();

	ScanFile();
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSecAnalyzerDoc serialization

void CSecAnalyzerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSecAnalyzerDoc diagnostics

#ifdef _DEBUG
void CSecAnalyzerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSecAnalyzerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSecAnalyzerDoc commands

BOOL CSecAnalyzerDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	if (GetMain()) {
		GetMain()->DoClearTimes();
		GetMain()->DoClearPatterns();
		GetMain()->DoClearMisc();
	}

	m_usedGroups.DeleteAll();
	m_usedGroups.UpdateMap();
	m_knownGroups.DeleteAll();
	m_knownGroups.UpdateMap();

	m_sFilename = lpszPathName;
	
	AfxGetApp()->AddToRecentFileList(m_sFilename);

	ScanFile();
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CSecAnalyzerDoc::OnCloseDocument() 
{
	DeleteTmpFile();

	WK_DELETE(m_pRecords);
	m_includePatterns.RemoveAll();
	m_excludePatterns.RemoveAll();
	
	CDocument::OnCloseDocument();
}


/////////////////////////////////////////////////////////////////////////////
void CSecAnalyzerDoc::DeleteTmpFile()
{
	if (m_sFilename.Find(_T("Tmp"))!=-1) {
		CFileStatus s;
		if (CFile::GetStatus( m_sFilename,s)) {
			CFile::Remove(m_sFilename);
			m_sFilename=_T("");
		}
	}
}

