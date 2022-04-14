#if !defined(AFX_BACKUPDIALOG_H__08892793_C018_11D2_B57C_004005A19028__INCLUDED_)
#define AFX_BACKUPDIALOG_H__08892793_C018_11D2_B57C_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BackupDialog.h : header file
//

class CServer;
class CRechercheView;
class CCopyReadOnlyVersion;

/////////////////////////////////////////////////////////////////////////////
#define TIMER_FOR_OLD_BACKUP	1
#define TIMER_FOR_NEW_BACKUP	2
#define TIMER_FOR_DATA_CARRIER	3


/////////////////////////////////////////////////////////////////////////////
// CBackupDialog dialog
class CBackupDialog : public CWK_Dialog
{
// Construction
public:
	CBackupDialog(UINT uDlgID, CServer* pServer, CRechercheView* pParent);   // standard constructor
	virtual ~CBackupDialog();

	// operations
public:
	inline CServer*			GetServer();
	inline CRechercheView*	GetParent();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBackupDialog)
	protected:
	virtual void PostNcDestroy();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

public:
	virtual void AddArchive(CIPCArchivRecord* pArchiv) = 0;
	virtual BOOL DeleteArchive(WORD wArchivNr, BOOL bDeleteSequences) = 0;

	virtual void BackupConfirmed(WORD wArchiveNr, WORD wSequenceNr) = 0;
	virtual void CancelBackupConfirmed(DWORD dwBackupID) = 0;
	virtual void BackupError(LPARAM lParam) = 0;
	virtual void HandleDeviceChange(BOOL bInsert, BOOL bFormat, DWORD dwData) = 0;

	virtual void AddSequence(CIPCSequenceRecord* pSequence);
	virtual BOOL DeleteSequence(CIPCSequenceRecord* pSequence);
	virtual void DeleteSequences(WORD wArchivNr);

	virtual void OnConfirmBackupByTime(WORD wUserData,
										 DWORD dwID,
										 const CSystemTime& stStartLessMaxCDCapacity,
										 const CSystemTime& stEndMoreMaxCDCapacity,
										 WORD wFlags,
										 CIPCInt64 i64Size);

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
	//  Device Change for detecting CD changes
	BOOL OnDeviceChange(UINT nEventType, DWORD dwData);

	// Generated message map functions
	//{{AFX_MSG(CBackupDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	void	InitDirectCDLanguageStrings();
	CString	FindDirectCDPath();
	BOOL	FileExists(CString sPath, CString sFileName);
	CString GetStringFromTable(CString sPath, CString sFile, int iStringID);
	BOOL	OnDeviceChange95(UINT nEventType, DWORD dwData);
	BOOL	OnDeviceChangeNT(UINT nEventType, DWORD dwData);
	BOOL	CheckSpaceOnDestination(BOOL bFormat = TRUE);
	BOOL	AskForCDRFinalize();

	BOOL IsCDorDVD(UINT uSystemType);
	BOOL IsWritableCDorDVD(UINT uSystemType);
	BOOL IsRewritableCDorDVD(UINT uSystemType);

// Dialog Data
	//{{AFX_DATA(CBackupDialog)
	//}}AFX_DATA
protected:
	CServer*			m_pServer;
	CRechercheView*		m_pParent;

	CIPCArchivRecords	m_ArchivesToCopy;

	DWORD				m_dwBackupID;
	DWORD				m_dwStartTime;
	DWORD				m_dwCountDownInSeconds;
	DWORD				m_dwBytesPerSecond;

	CIPCInt64			m_i64BytesToCopy;
	CIPCInt64			m_i64BytesCopied;

	CIPCDrive*				m_pDrive;
	CCopyReadOnlyVersion*	m_pCopyReadOnlyVersion;
	BOOL					m_bDriveInfoChanged;

	BOOL			m_bHandleDeviceChange;

	CString	m_sDCDFormatting;
	CString	m_sDCDRemoveWriteProtection;
};
////////////////////////////////////////////////////////////////
inline CServer* CBackupDialog::GetServer()
{
	return m_pServer;
}
////////////////////////////////////////////////////////////////
inline CRechercheView* CBackupDialog::GetParent()
{
	return m_pParent;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BACKUPDIALOG_H__08892793_C018_11D2_B57C_004005A19028__INCLUDED_)
