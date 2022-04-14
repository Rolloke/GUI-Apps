#if !defined(AFX_DlgBackup_H__08892793_C018_11D2_B57C_004005A19028__INCLUDED_)
#define AFX_DlgBackup_H__08892793_C018_11D2_B57C_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBackup.h : header file
//

class CServer;
class CViewIdipClient;
class CCopyReadOnlyVersion;

/////////////////////////////////////////////////////////////////////////////
#define TIMER_FOR_OLD_BACKUP	1
#define TIMER_FOR_NEW_BACKUP	2
#define TIMER_FOR_DATA_CARRIER	3
#define TIMER_FOR_DRIVE_INFO	4


class CDlgSelectBackupDrive;
/////////////////////////////////////////////////////////////////////////////
// CDlgBackup dialog
class CDlgBackup : public CSkinDialog
{
// Construction
public:
	CDlgBackup(UINT uDlgID, CServer* pServer, CViewIdipClient* pParent);   // standard constructor
	virtual ~CDlgBackup();

	// operations
public:
	inline WORD				GetServerID();
	inline CViewIdipClient*	GetParent();
	CServer *				GetServer();
protected:
	inline void Lock(LPCTSTR sFkt = NULL);
	inline void Unlock();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBackup)
	protected:
	virtual void PostNcDestroy();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

public:
	virtual void AddArchive(CIPCArchivRecord* pArchiv) PURE_VIRTUAL(;)
	virtual BOOL DeleteArchive(WORD wArchivNr, BOOL bDeleteSequences) PURE_VIRTUAL(0)

	virtual void BackupConfirmed(WORD wArchiveNr, WORD wSequenceNr) PURE_VIRTUAL(;)
	virtual void CancelBackupConfirmed(DWORD dwBackupID) PURE_VIRTUAL(;)
	virtual void BackupError(LPARAM lParam) PURE_VIRTUAL(;)
	virtual void HandleDeviceChange(BOOL bInsert, BOOL bFormat, DWORD dwData) PURE_VIRTUAL(;)

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
	void Close();

	// Generated message map functions
	//{{AFX_MSG(CDlgBackup)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);
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
protected:
	//{{AFX_DATA(CDlgBackup)
	CSkinListBox	m_ctrlArchives;
	CSkinButton	m_ctrlCDR;
	CSkinStatic	m_staticState;
	CSkinStatic	m_staticToCopy;
	CSkinStatic	m_staticType;
	CSkinStatic	m_staticFree;
	CSkinButton	m_ctrlClose;
	CSkinButton	m_ctrlCancel;
	CSkinButton	m_ctrlOK;
	CSkinProgress	m_ctrlProgress;
	CSkinComboBox	m_comboDestination;
	//}}AFX_DATA
protected:
	CViewIdipClient*	m_pParent;
	WORD				m_wServerID;
	WORD				m_wDummy;

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
	CDlgSelectBackupDrive *m_pSelectDrive;
};
////////////////////////////////////////////////////////////////
inline WORD CDlgBackup::GetServerID()
{
	return m_wServerID;
}
////////////////////////////////////////////////////////////////
inline CViewIdipClient* CDlgBackup::GetParent()
{
	return m_pParent;
}
/////////////////////////////////////////////////////////////////////////////
inline void CDlgBackup::Lock(LPCTSTR sFkt)
{
	m_ArchivesToCopy.Lock(sFkt);
}
/////////////////////////////////////////////////////////////////////////////
inline void CDlgBackup::Unlock()
{
	m_ArchivesToCopy.Unlock();
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DlgBackup_H__08892793_C018_11D2_B57C_004005A19028__INCLUDED_)
