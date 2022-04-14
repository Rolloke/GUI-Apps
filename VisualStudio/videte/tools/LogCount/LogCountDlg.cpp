// LogCountDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LogCount.h"
#include "LogCountDlg.h"
#include "SearchThread.h"
#include <io.h>
#include "DirDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Timer Definition
#define TIMER_PROGRESS 1
static char BASED_CODE szFilter[] = "All Files (*.*)|*.*||";
static char BASED_CODE szLog[] = "Log";
static char BASED_CODE szLogPathEntry[] = "LogPath";
static char BASED_CODE szLogPath[] = "c:\\log";

/////////////////////////////////////////////////////////////////////////////
// CLogCountDlg dialog
CLogCountDlg::CLogCountDlg(CWnd* pParent /*=NULL*/)
	: CWK_Dialog(CLogCountDlg::IDD, pParent)
{
	CString sProgress, sDirectory;
	sProgress.LoadString(IDS_START);
	CWK_Profile profile;
	sDirectory = profile.GetString(szLog, szLogPathEntry, szLogPath);
	//{{AFX_DATA_INIT(CLogCountDlg)
	m_dwPatternCounter = 0;
	m_sFilesCounter = _T("0");
	m_sProgress = sProgress;
	m_dwErrorCounter = 0;
	m_sErrorPattern = _T("");
	m_sSearchPattern = _T("");
	m_iRadioSearch = 1;
	m_iLogfilePattern = 0;
	m_sDirectory = sDirectory;
	m_bSearchAll = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pSearchThread = NULL;
	m_eProgress = START;
	m_bCanceled = FALSE;
	m_dwLockCounter = 0;
	m_dwUnlockCounter = 0;
	m_bSomethingChanged = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
CLogCountDlg::~CLogCountDlg() 
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLogCountDlg::StretchElements() 
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CLogCountDlg::Lock() 
{
	m_dwLockCounter++;
//	TRACE("Lock %u\n", m_dwLockCounter);
	if (m_dwLockCounter > m_dwUnlockCounter+1) {
		// next lock before unlock
		TRACE("Lock %u Unlock %u\n", m_dwLockCounter, m_dwUnlockCounter);
	}
	m_cs.Lock();
	return m_dwLockCounter;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CLogCountDlg::Unlock() 
{
	m_dwUnlockCounter++;
//	TRACE("Unlock %u\n", m_dwUnlockCounter);
	if (m_dwLockCounter > m_dwUnlockCounter+1) {
		// next lock before unlock
		TRACE("Lock %u Unlock %u\n", m_dwLockCounter, m_dwUnlockCounter);
	}
	m_cs.Unlock();
	return m_dwUnlockCounter;
}
/////////////////////////////////////////////////////////////////////////////
void CLogCountDlg::AppendSearchFile(const CString& sFullPath) 
{
	Lock();
	m_sFileArraySearch.Add(sFullPath);
	m_bSomethingChanged = TRUE;
	Unlock();
}
/////////////////////////////////////////////////////////////////////////////
CString CLogCountDlg::GetNextSearchFile() 
{
	CString sFileName;
	Lock();
	if (m_iCounterSearchedFiles < m_sFileArraySearch.GetSize() ) {
		sFileName = m_sFileArraySearch.GetAt(m_iCounterSearchedFiles);
	}
	Unlock();
	return sFileName;
}
/////////////////////////////////////////////////////////////////////////////
void CLogCountDlg::SearchFileProcessed() 
{
	Lock();
	m_iCounterSearchedFiles++;
	m_bSomethingChanged = TRUE;
	Unlock();
}
/////////////////////////////////////////////////////////////////////////////
int CLogCountDlg::GetSearchFilesCount() 
{
	int iCount = 0;
	Lock();
	iCount = m_sFileArraySearch.GetSize();
	Unlock();
	return iCount;
}
/////////////////////////////////////////////////////////////////////////////
int CLogCountDlg::GetSearchedFilesCount() 
{
	int iCount = 0;
	Lock();
	iCount = m_iCounterSearchedFiles;
	Unlock();
	return iCount;
}
/////////////////////////////////////////////////////////////////////////////
void CLogCountDlg::UpdateSearch(DWORD dwPatternCounter, DWORD dwErrorCounter) 
{
	Lock();
	m_dwPatternCounter = dwPatternCounter;
	m_dwErrorCounter = dwErrorCounter;
	m_bSomethingChanged = TRUE;
	Unlock();
}
/////////////////////////////////////////////////////////////////////////////
DWORD CLogCountDlg::GetPatternCounter() 
{
	DWORD dwCount = 0;
	Lock();
	dwCount = m_dwPatternCounter;
	Unlock();
	return dwCount;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CLogCountDlg::GetErrorCounter() 
{
	DWORD dwCount = 0;
	Lock();
	dwCount = m_dwErrorCounter;
	Unlock();
	return dwCount;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLogCountDlg::IsDateEarlierThanSpan(const CSystemTime& stDate)
{
	BOOL bReturn = FALSE;
	if (m_bSearchAll) {
		// ignorieren
		bReturn = FALSE;
	}
	else if (CSystemTime() < stDate){
		// stDate ist gueltig
		if (m_DateTimeStart > stDate) {
			bReturn = TRUE;
		}
	}
	else {
		// keine gueltige SystemTime
		bReturn = TRUE;
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLogCountDlg::IsDateInSpan(const CSystemTime& stDate)
{
	BOOL bReturn = FALSE;
	if (m_bSearchAll) {
		// ignorieren
		bReturn = TRUE;
	}
	else if (CSystemTime() < stDate){
		// stDate ist gueltig
		if (	(m_DateTimeStart < stDate || m_DateTimeStart == stDate)
			&&	(m_DateTimeEnd > stDate || m_DateTimeEnd == stDate)
			)
		{
			bReturn = TRUE;
		}
	}
	else {
		// keine gueltige SystemTime
		bReturn = FALSE;
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLogCountDlg::IsDateLaterThanSpan(const CSystemTime& stDate)
{
	BOOL bReturn = FALSE;
	if (m_bSearchAll) {
		// ignorieren
		bReturn = FALSE;
	}
	else if (CSystemTime() < stDate){
		// stDate ist gueltig
		if (m_DateTimeEnd < stDate) {
			bReturn = TRUE;
		}
	}
	else {
		// keine gueltige SystemTime
		bReturn = FALSE;
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CLogCountDlg::FillSearchBox() 
{
	CString s;
	int iIndex;
	// German Parcel gescannte Pakete
	s.LoadString(IDS_GERMAN_PARCEL_SCANNED_PACKETS);
	iIndex = m_comboSearchFor.AddString(s);
	m_comboSearchFor.SetItemData(iIndex, GERMAN_PARCEL_SCANNED_PACKETS);
}
/////////////////////////////////////////////////////////////////////////////
void CLogCountDlg::FillLogfileBox()
{
	m_comboLogfile.ResetContent();

	CStringArray sFilesArray;
	WK_SearchFiles(sFilesArray, m_sDirectory, "*.log", FALSE);
//	WK_SearchFiles(sFilesArray, m_sDirectory, "*.lgz", FALSE);

	CString sLogName;
	for (int i=0 ; i<sFilesArray.GetSize() ; i++) {
		sLogName = sFilesArray.GetAt(i);
		m_comboLogfile.AddString(sLogName);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CLogCountDlg::ClearData() 
{
	Lock();
	m_eProgress = START;
	m_iCounterSearchedFiles = 0;
	m_sFileArraySearch.RemoveAll();
	m_sFilesCounter.Format("% 3u / % 3u",m_iCounterSearchedFiles ,m_sFileArraySearch.GetSize() );
	m_dwPatternCounter = 0;
	m_dwErrorCounter = 0;
	Unlock();

	m_dwLockCounter = 0;
	m_dwUnlockCounter = 0;
	UpdateData(FALSE);
	UpdateWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CLogCountDlg::EnableDlgControls() 
{
	UpdateData();

	// bEnable = TRUE, wenn freie Kriterien, sonst FALSE
	BOOL bFreeCriteria = (m_iRadioSearch == 1);
	BOOL bHasSearchCriteria = SearchCriteriaNotEmpty();
	BOOL bIsNotWorking = (m_eProgress==START || m_eProgress==READY || m_eProgress==CANCELED);

	m_btnSearch1.EnableWindow(bIsNotWorking);
	m_btnSearch2.EnableWindow(bIsNotWorking);
	m_comboSearchFor.EnableWindow(!bFreeCriteria && bIsNotWorking);
	m_editDirectory.EnableWindow(bFreeCriteria && bIsNotWorking);
	m_btnDirectory.EnableWindow(bFreeCriteria && bIsNotWorking);
	m_comboLogfile.EnableWindow(bFreeCriteria && bIsNotWorking);
	m_editSearchPattern.EnableWindow(bFreeCriteria && bIsNotWorking);
	m_editErrorPattern.EnableWindow(bFreeCriteria && bIsNotWorking);

	m_btnSearchAll.EnableWindow(bIsNotWorking);
	m_dtpDateStart.EnableWindow(bIsNotWorking && !m_bSearchAll);
	m_dtpDateEnd.EnableWindow(bIsNotWorking && !m_bSearchAll);
	m_dtpTimeStart.EnableWindow(bIsNotWorking && !m_bSearchAll && 0);
	m_dtpTimeEnd.EnableWindow(bIsNotWorking && !m_bSearchAll && 0);

	m_btnOK.EnableWindow(bHasSearchCriteria && bIsNotWorking);
	m_btnCancel.EnableWindow(!m_bCanceled && !bIsNotWorking);
	m_btnEnd.EnableWindow(bIsNotWorking);
}
/////////////////////////////////////////////////////////////////////////////
void CLogCountDlg::SelectLogfileInBox() 
{
	CString sLogFileName = m_sLogfilePattern + ".log";
	m_iLogfilePattern = m_comboLogfile.FindStringExact(-1, sLogFileName) ;
	m_comboLogfile.SetCurSel(m_iLogfilePattern);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLogCountDlg::SearchCriteriaNotEmpty() 
{
	BOOL bReturn = FALSE;
	bReturn = m_sSearchPattern.IsEmpty() == FALSE
				|| m_sErrorPattern.IsEmpty() == FALSE;
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CLogCountDlg::SetDateAndTimeControls() 
{
	CSystemTime st;
	st.GetLocalTime();
	m_DateTimeStart = CSystemTime(st.GetDay(), st.GetMonth(), st.GetYear(), 0, 0, 0);
	m_DateTimeEnd = CSystemTime(st.GetDay(), st.GetMonth(), st.GetYear(), 23, 59, 59);
	m_dtpDateStart.SetTime(&m_DateTimeStart);
	m_dtpDateEnd.SetTime(&m_DateTimeEnd);
	m_dtpTimeStart.SetTime(&m_DateTimeStart);
	m_dtpTimeEnd.SetTime(&m_DateTimeEnd);
}
/////////////////////////////////////////////////////////////////////////////
void CLogCountDlg::SetDateAndTime() 
{
	CSystemTime stDateStart, stDateEnd, stTimeStart, stTimeEnd;
	DWORD dwReturn;
	dwReturn= m_dtpDateStart.GetTime(&stDateStart);
	dwReturn = m_dtpDateEnd.GetTime(&stDateEnd);
	dwReturn = m_dtpTimeStart.GetTime(&stTimeStart);
	dwReturn = m_dtpTimeEnd.GetTime(&stTimeEnd);

	m_DateTimeStart	= CSystemTime(stDateStart.GetDay(),
							  stDateStart.GetMonth(),
							  stDateStart.GetYear(),
							  stTimeStart.GetHour(),
							  stTimeStart.GetMinute(),
							  stTimeStart.GetSecond());
	m_DateTimeEnd	= CSystemTime(stDateEnd.GetDay(),
							  stDateEnd.GetMonth(),
							  stDateEnd.GetYear(),
							  stTimeEnd.GetHour(),
							  stTimeEnd.GetMinute(),
							  stTimeEnd.GetSecond());
}
/////////////////////////////////////////////////////////////////////////////
void CLogCountDlg::WorkingProgress() 
{
	if (m_eProgress==START) {
		// Suche starten
		SearchInFiles();
	}
	else if (m_eProgress==SEARCH) {
		m_sProgress.LoadString(IDS_SEARCH);
		int iSearchFilesCount = GetSearchFilesCount();
		m_sFilesCounter.Format("% 3u / % 3u",m_iCounterSearchedFiles ,iSearchFilesCount);
		if ( m_iCounterSearchedFiles >= iSearchFilesCount
			 || (m_pSearchThread && m_pSearchThread->IsRunning() == FALSE) )
		{
			DWORD dwStopSearchTime = GetCurrentTime();
			TRACE("Stop Search %lu : %lu\n",
					dwStopSearchTime, dwStopSearchTime-m_dwStartSearchTime);
			WK_TRACE_USER("%i Files of %i searched\n", m_iCounterSearchedFiles, iSearchFilesCount);
//			StopSearchThread();
			// Animation stoppen
			m_Animation.Close();
			m_Animation.RedrawWindow();
			// Fertig
			if (m_bCanceled) {
				m_eProgress = CANCELED;
				m_bCanceled = FALSE;
			}
			else {
				m_eProgress = READY;
			}
		}
	}
	else if (m_eProgress==READY) {
		KillTimer(TIMER_PROGRESS);
		m_sProgress.LoadString(IDS_READY);
		TRACE("Lock %u Unlock %u\n", m_dwLockCounter, m_dwUnlockCounter);
	}
	else if (m_eProgress==CANCELED) {
		KillTimer(TIMER_PROGRESS);
		m_sProgress.LoadString(IDS_CANCELLED);
		TRACE("Lock %u Unlock %u\n", m_dwLockCounter, m_dwUnlockCounter);
	}
	if (m_bSomethingChanged) {
		UpdateData(FALSE);
		EnableDlgControls();
		m_bSomethingChanged = FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CLogCountDlg::SearchInFiles() 
{
	m_eProgress = SEARCH;
	m_iCounterSearchedFiles = 0;

	m_Animation.Open(IDR_AVI_SEARCH_FILE);
//	m_Animation.Play(0,((UINT)-1),((UINT)-1));
	m_Animation.RedrawWindow();

	// Lgz-Files ins Array haengen, aber nur, wenn LofilePattern vorhanden
	if (m_sLogfilePattern.IsEmpty() == FALSE) {
		CString sFilePattern = "*" + m_sLogfilePattern + "*.lgz";
		// WKSearchFiles liefert nur die reinen Dateinamen ohne Pfad, deshalb zwischenlagern
		CStringArray sTempArray;
		WK_SearchFiles(sTempArray, m_sDirectory, sFilePattern, FALSE);
		// Mit Pfad ergaenzen und ins ZipArray kopieren
		CString sFullPath;
		for (int i=0 ; i<sTempArray.GetSize() ; i++) {
			sFullPath = m_sDirectory + "\\" + sTempArray.GetAt(i);
			m_sFileArraySearch.Add(sFullPath);
		}
		// Sortiere die Liste
		SortList(m_sFileArraySearch);
		// Log-File noch hinzufuegen
		// Mit Pfad ergaenzen und ins SearchArray kopieren
		sFullPath = m_sDirectory + "\\" + m_sLogfilePattern + ".log";
		m_sFileArraySearch.Add(sFullPath);
	}
	DWORD dwFiles = m_sFileArraySearch.GetSize();
	if (dwFiles==0) {
		WK_TRACE_USER( "No Files to search found\n");
	}
	else {
		WK_DELETE(m_pSearchThread);
		m_dwStartSearchTime = GetCurrentTime();
		m_pSearchThread = new CSearchThread(this, m_sSearchPattern, m_sErrorPattern);
		BOOL bStarted = m_pSearchThread->StartThread();
		TRACE("m_pSearchThread Started %i %lu\n", bStarted, m_dwStartSearchTime);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLogCountDlg::SortList(CStringArray &FileNames)
{
	if (FileNames.GetSize() < 2)
		return FALSE;

	for (int i=0 ; i < FileNames.GetSize()-1 ; i++)
	{
		for (int j = i+1; j < FileNames.GetSize(); j++)
		{
			if (FileNames.GetAt(i) > FileNames.GetAt(j))
			{
				CString sTemp	= FileNames.GetAt(j);
				FileNames.GetAt(j) = FileNames.GetAt(i);
				FileNames.GetAt(i)	= sTemp;
			}
		}
	}
		
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLogCountDlg::StopSearchThread()
{
	BOOL bReturn = TRUE;
	if (m_pSearchThread && m_pSearchThread->IsRunning()) {
		bReturn = m_pSearchThread->StopSearchThread();
		if (bReturn) {
			TRACE("m_pSearchThread Stopped\n");
		}
		else {
			TRACE("m_pSearchThread NOT Stopped\n");
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CLogCountDlg::DoDataExchange(CDataExchange* pDX)
{
	CWK_Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogCountDlg)
	DDX_Control(pDX, IDC_BTN_DIRECTORY_SEARCH, m_btnDirectory);
	DDX_Control(pDX, IDC_RADIO_SEARCH_2, m_btnSearch2);
	DDX_Control(pDX, IDC_RADIO_SEARCH_1, m_btnSearch1);
	DDX_Control(pDX, IDC_CHECK_ALL, m_btnSearchAll);
	DDX_Control(pDX, IDC_TIMEPICKER_END, m_dtpTimeEnd);
	DDX_Control(pDX, IDC_TIMEPICKER_START, m_dtpTimeStart);
	DDX_Control(pDX, IDC_DATEPICKER_END, m_dtpDateEnd);
	DDX_Control(pDX, IDC_DATEPICKER_START, m_dtpDateStart);
	DDX_Control(pDX, IDC_EDIT_DIRECTORY, m_editDirectory);
	DDX_Control(pDX, IDC_COMBO_LOGFILE, m_comboLogfile);
	DDX_Control(pDX, IDC_EDIT_SEARCH_PATTERN, m_editSearchPattern);
	DDX_Control(pDX, IDC_EDIT_ERROR_PATTERN, m_editErrorPattern);
	DDX_Control(pDX, IDC_COMBO_SEARCH_FOR, m_comboSearchFor);
	DDX_Control(pDX, IDC_ANIMATE, m_Animation);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_BUTTON_END, m_btnEnd);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Text(pDX, IDC_EDIT_COUNTER, m_dwPatternCounter);
	DDX_Text(pDX, IDC_EDIT_FILES, m_sFilesCounter);
	DDX_Text(pDX, IDC_TXT_PROGRESS, m_sProgress);
	DDX_Text(pDX, IDC_EDIT_ERRORS, m_dwErrorCounter);
	DDX_Text(pDX, IDC_EDIT_ERROR_PATTERN, m_sErrorPattern);
	DDX_Text(pDX, IDC_EDIT_SEARCH_PATTERN, m_sSearchPattern);
	DDX_Radio(pDX, IDC_RADIO_SEARCH_1, m_iRadioSearch);
	DDX_CBIndex(pDX, IDC_COMBO_LOGFILE, m_iLogfilePattern);
	DDX_Text(pDX, IDC_EDIT_DIRECTORY, m_sDirectory);
	DDX_Check(pDX, IDC_CHECK_ALL, m_bSearchAll);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CLogCountDlg, CWK_Dialog)
	//{{AFX_MSG_MAP(CLogCountDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_END, OnButtonEnd)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RADIO_SEARCH_1, OnRadioSearch)
	ON_CBN_SELCHANGE(IDC_COMBO_SEARCH_FOR, OnSelchangeComboSearchFor)
	ON_CBN_SELCHANGE(IDC_COMBO_LOGFILE, OnSelchangeComboLogfile)
	ON_EN_CHANGE(IDC_EDIT_SEARCH_PATTERN, OnChangeEditSearchPattern)
	ON_EN_CHANGE(IDC_EDIT_ERROR_PATTERN, OnChangeEditErrorPattern)
	ON_EN_KILLFOCUS(IDC_EDIT_DIRECTORY, OnKillfocusEditDirectory)
	ON_BN_CLICKED(IDC_BTN_DIRECTORY_SEARCH, OnBtnDirectorySearch)
	ON_BN_CLICKED(IDC_CHECK_ALL, OnCheckAll)
	ON_CBN_DROPDOWN(IDC_COMBO_LOGFILE, OnDropdownComboLogfile)
	ON_BN_CLICKED(IDC_RADIO_SEARCH_2, OnRadioSearch)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CLogCountDlg message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CLogCountDlg::OnInitDialog()
{
	CWK_Dialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	FillSearchBox();
	FillLogfileBox();
	SelectLogfileInBox();
	SetDateAndTimeControls();
	EnableDlgControls();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}
/////////////////////////////////////////////////////////////////////////////
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CLogCountDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CWK_Dialog::OnPaint();
	}
}
/////////////////////////////////////////////////////////////////////////////
// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLogCountDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
/////////////////////////////////////////////////////////////////////////////
void CLogCountDlg::OnTimer(UINT nIDEvent) 
{
	WorkingProgress();
	
	CWK_Dialog::OnTimer(nIDEvent);
}
/////////////////////////////////////////////////////////////////////////////
void CLogCountDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	OnButtonEnd();
}
/////////////////////////////////////////////////////////////////////////////
void CLogCountDlg::OnRadioSearch() 
{
	UpdateData();
	if (m_iRadioSearch == 0) {
		OnSelchangeComboSearchFor();
	}
	EnableDlgControls();
}
/////////////////////////////////////////////////////////////////////////////
void CLogCountDlg::OnSelchangeComboSearchFor() 
{
	UpdateData();
	int iIndex = m_comboSearchFor.GetCurSel();
	EnumSearchFor enumSearch = SEARCH_UNKNOWN;
	if (iIndex != LB_ERR) {
		enumSearch = (EnumSearchFor)m_comboSearchFor.GetItemData(iIndex);
	}
	CWK_Profile profile;
	CString sDirectory = profile.GetString(szLog, szLogPathEntry, szLogPath);
	switch (enumSearch) {
		case GERMAN_PARCEL_SCANNED_PACKETS:
			m_sDirectory = sDirectory;
			m_sLogfilePattern = "SDIUnit";
			SelectLogfileInBox();
			m_sSearchPattern = "GERMAN_PARCEL Indicate Alarm";
			m_sErrorPattern = "Daten/Prüfziffer falsch";
			break;
		default:
			m_sSearchPattern = "";
			m_sErrorPattern = "";
			break;
	}
	UpdateData(FALSE);
	EnableDlgControls();
}
/////////////////////////////////////////////////////////////////////////////
void CLogCountDlg::OnKillfocusEditDirectory() 
{
	FillLogfileBox();
}
/////////////////////////////////////////////////////////////////////////////
void CLogCountDlg::OnBtnDirectorySearch() 
{
	CDirDialog dlg(TRUE,NULL,"trick.dat",OFN_HIDEREADONLY,szFilter);
	dlg.m_ofn.lpstrInitialDir = m_sDirectory.GetBuffer(0);
	dlg.m_sDir = m_sDirectory;
	int res = dlg.DoModal();
	if (res==IDOK) {
		CString sDirectory, sFileName;
		WK_SplitPath(dlg.GetPathName(), sDirectory, sFileName);
		m_sDirectory = sDirectory.Left(sDirectory.GetLength()-1);
		UpdateData(FALSE);
		FillLogfileBox();
	}
	m_sDirectory.ReleaseBuffer(0);
}
/////////////////////////////////////////////////////////////////////////////
void CLogCountDlg::OnDropdownComboLogfile() 
{
	// TODO: Add your control notification handler code here
	FillLogfileBox();
	SelectLogfileInBox();
}
/////////////////////////////////////////////////////////////////////////////
void CLogCountDlg::OnSelchangeComboLogfile() 
{
	UpdateData();
	if (m_iLogfilePattern >= 0) {
		CString sLogfilePattern, sExtension;
		m_comboLogfile.GetLBText(m_iLogfilePattern, sLogfilePattern);
		WK_SplitExtension(sLogfilePattern, m_sLogfilePattern, sExtension);
	}
	else {
		m_sLogfilePattern = "";
	}
}
/////////////////////////////////////////////////////////////////////////////
void CLogCountDlg::OnChangeEditSearchPattern() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	EnableDlgControls();
}
/////////////////////////////////////////////////////////////////////////////
void CLogCountDlg::OnChangeEditErrorPattern() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	EnableDlgControls();
}
/////////////////////////////////////////////////////////////////////////////
void CLogCountDlg::OnCheckAll() 
{
	// TODO: Add your control notification handler code here
	EnableDlgControls();
}
/////////////////////////////////////////////////////////////////////////////
void CLogCountDlg::OnOK() 
{
	if (m_eProgress==START || m_eProgress==READY || m_eProgress==CANCELED) {
		if ( SearchCriteriaNotEmpty() ) {
			UpdateData();
			SetDateAndTime();
			ClearData();
			EnableDlgControls();
			SetTimer(TIMER_PROGRESS,100,NULL);
			WorkingProgress();
		}
		else {
			// Keine Suchkriterien angegeben
		}
	}
	else {
		// Falscher Zustand, Start nicht moeglich
	}
}
/////////////////////////////////////////////////////////////////////////////
void CLogCountDlg::OnCancel() 
{
	m_bCanceled = TRUE;
	m_bSomethingChanged = TRUE;
	if (m_eProgress==START) {
		// Haben ja noch gar nicht angefangen
		m_bCanceled = FALSE;
	}
	else if (m_eProgress==SEARCH) {
		WK_TRACE_USER( "Search cancelled\n");
		StopSearchThread();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CLogCountDlg::OnButtonEnd() 
{
	if ( StopSearchThread() ) {
		WK_DELETE(m_pSearchThread);
		CWK_Dialog::OnOK();
	}
}
