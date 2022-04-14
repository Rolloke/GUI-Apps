#if !defined(AFX_DlgBackupNEW_H__08592773_C018_1AD2_B57C_004905A19038__INCLUDED_)
#define AFX_DlgBackupNEW_H__08592773_C018_1AD2_B57C_004905A19038__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBackupNew.h : header file
//

#include "DlgBackup.h"
#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgBackupNew dialog
class CDlgBackupNew : public CDlgBackup
{
	enum EnumStatus
	{
		BS_NORMAL,
		BS_CHECKING_DRIVE,
		BS_CALCULATING_END_STARTED,
		BS_CALCULATING_END,
		BS_CALCULATING_SIZE,
		BS_RUNNING_DATABASE,
		BS_RUNNING_DATACARRIER,
		BS_JOB_DONE,
	};	
	
// Construction
public:
	CDlgBackupNew(CServer* pServer, CViewIdipClient* pParent);   // standard constructor
	virtual ~CDlgBackupNew();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBackupNew)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	virtual void AddArchive(CIPCArchivRecord* pArchiv);
	virtual BOOL DeleteArchive(WORD wArchivNr, BOOL bDeleteSequences);

	virtual void BackupConfirmed(WORD wArchiveNr, WORD wSequenceNr);
	virtual void CancelBackupConfirmed(DWORD dwSessionID);
	virtual void BackupError(LPARAM lParam);
	virtual void HandleDeviceChange(BOOL bInsert, BOOL bFormat, DWORD dwData);

	virtual void OnConfirmBackupByTime(	WORD wUserData,
										DWORD dwID,
										const CSystemTime& stStartLessMaxCDCapacity,
										const CSystemTime& stEndMoreMaxCDCapacity,
										WORD wFlags,
										CIPCInt64 i64Size);
	void RefreshInfo();

	// CipcDataCarrier Confirms
	virtual void OnIndicateError(	DWORD dwCmd,
									CSecID id,
									CIPCError errorCode,
									int iUnitCode,
									const CString &sErrorMsg);
	virtual	void OnConfirmVolumeInfos(int iNumDrives, const CIPCDrive drives[]);
	virtual	void OnIndicateSessionProgress(DWORD dwSessionID, int iProgress);
	virtual	void OnConfirmSession(DWORD dwSessionID);
	virtual	void OnConfirmCancelSession(DWORD dwSessionID);
	virtual	void OnConfirmDeleteVolume(const CString& sVolume);


protected:
	virtual	void OnDrivesInfo();
	
// Implementation
public:
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgBackupNew)
	afx_msg void OnCalculateSize();
	afx_msg void OnCalculateEnd();
	afx_msg void OnResetStartEnd();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSelchangeComboDestination();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnCdr();
	afx_msg void OnDatetimechangeEndDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimechangeEndTime(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimechangeStartDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimechangeStartTime(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEditBackupName();
	afx_msg void OnDestroy();
	afx_msg LRESULT OnUpdateAndEnableControls(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHandleCDandDVDInfo(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnBackupDataCarrierFinished(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL InitDrives();
	void SelectDrive();
	void CalculateInfo();
	void EnableControls();
	void EnableTreeView(BOOL bEnable);
	void Calculate(BOOL bContinue);
	void RequestFirstImageInDB();
//	void RequestLastImageInDB();
	void StartCalculationForEndTime();
	void StartCalculationForSize();
	void StartCalculation(WORD wFlags);
	void StartDatabaseBackup();
	void StartDataCarrierBackup();
	void BackupCancelled(DWORD dwSessionID);

	void ArchivesChanged();
	void CheckDriveSpace(BOOL bFormat);
	void CalcDestinationCapacity();
	void RemoveArchiveFromList(WORD wID);
	void BackupDatabaseFinished();
	void CopyReadOnlyVersion();
	void CopyReadOnlyVersionFinished();

	void FirstCalculationEnd(	DWORD dwID,
							const CSystemTime& stStartLessMaxCDCapacity,
							const CSystemTime& stEndMoreMaxCDCapacity,
							CIPCInt64 i64Size);
	void NextCalculationEnd(	DWORD dwID,
							const CSystemTime& stStartLessMaxCDCapacity,
							const CSystemTime& stEndMoreMaxCDCapacity,
							CIPCInt64 i64Size);
	BOOL RequestNextCalculationEnd();
	void CalculationFinished();
	void SizeCalculation(	DWORD dwID,
							const CSystemTime& stStartLessMaxCDCapacity,
							const CSystemTime& stEndMoreMaxCDCapacity,
							CIPCInt64 i64Size);
	void BackupProgress(DWORD dwID, CIPCInt64 i64Size);
	void CheckForContinuation();

	BOOL CreateTempDir();
	BOOL DeleteTempDir();

// Dialog Data
	//{{AFX_DATA(CDlgBackupNew)
	enum  { IDD = IDD_BACKUP_NEW };
	CSkinButton	m_ctrlCalculateSize;
	CSkinButton	m_ctrlCalculateEnd;
	CSkinButton	m_ctrlResetStartEnd;
	CSkinDateTimeCtrl	m_ctrlEndTime;
	CSkinDateTimeCtrl	m_ctrlEndDate;
	CSkinDateTimeCtrl	m_ctrlStartTime;
	CSkinDateTimeCtrl	m_ctrlStartDate;
	CString	m_sSize;
	CString	m_sVelocity;
	CString	m_sBackupName;
	CString	m_sMBtoCopy;
	CString	m_sStatus;
	CString	m_sTime;
	CString	m_sFree;
	CString	m_sType;
	//}}AFX_DATA

	DWORD		m_dwSessionID;			// individual UserID for Backup, calculated by GetTickCount()
	EnumStatus	m_enumStatus;

	UINT		m_uTimerDriveInfo;
	UINT		m_uTimerRO;
	UINT		m_uTimerDataCarrier;
	
	CSystemTime	m_stFirstDBImage;			// time of first image in database, no earlier backup possible
//	CSystemTime	m_stLastDBImage;			// time of last image in database, no further backup possible
	CSystemTime	m_stStartDateTime;			// start time for backup, depends on database or user input
	CSystemTime	m_stEndDateTime;			// end time for backup, calculated automatically or user input
	CSystemTime	m_stEndLessMaxCapacity;		// end time for backup, calculated automatically
	CSystemTime	m_stEndMoreMaxCapacity;		// end time for backup, calculated automatically
	CSystemTime	m_stEndDateTimeConfirmed;	// end time for backup, calculated automatically

	DWORD		m_dwSeconds;

	CIPCInt64	m_i64DestCapacity;		// max. capacity of destination drive
	CIPCInt64	m_i64LessMaxCapacity;	// calculated size less than max. capacity of destination
	CIPCInt64	m_i64MoreMaxCapacity;	// calculated size more than max. capacity of destination

	BOOL	m_bStartDateTimeEntered;
	BOOL	m_bEndCalculationPossible;
	BOOL	m_bNeedCalculationSize;
	BOOL	m_bContinuationPossible;

	DWORD	m_dwReaderSoftwareMB;
	DWORD	m_dwReservedSpaceMB;
	DWORD	m_dwAbortAtDifference_kB;
	DWORD	m_dwTempDriveReservedSpaceMB;
	CString	m_sTempLocation;

	BOOL	m_bIsDataCarrierBackup;
	int		m_iProgressPercent;
	BOOL	m_bCleanRW;
	CString	m_sTempDir;
	BOOL	m_bIsCancelled;
	BOOL	m_bTypeIsHighlighted;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DlgBackupNEW_H__08592773_C018_1AD2_B57C_004905A19038__INCLUDED_)
