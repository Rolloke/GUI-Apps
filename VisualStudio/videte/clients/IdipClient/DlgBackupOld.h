// DlgBackupOld.h: interface for the CDlgBackupOld class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DlgBackupOLD_H__6BC21BF7_E86E_425F_861D_65FBB7D7218B__INCLUDED_)
#define AFX_DlgBackupOLD_H__6BC21BF7_E86E_425F_861D_65FBB7D7218B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DlgBackup.h"

class CDlgBackupOld : public CDlgBackup  
{
// Construction
public:
	CDlgBackupOld(CServer* pServer, CViewIdipClient* pParent);
	virtual ~CDlgBackupOld();

// operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBackupOld)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	virtual void AddArchive(CIPCArchivRecord* pArchiv);
	virtual BOOL DeleteArchive(WORD wArchivNr, BOOL bDeleteSequences);

	virtual void BackupConfirmed(WORD wArchiveNr, WORD wSequenceNr);
	virtual void CancelBackupConfirmed(DWORD dwBackupID);
	virtual void BackupError(LPARAM lParam);
	virtual void HandleDeviceChange(BOOL bInsert, BOOL bFormat, DWORD dwData);

	virtual void AddSequence(CIPCSequenceRecord* pSequence);
	virtual BOOL DeleteSequence(CIPCSequenceRecord* pSequence);
	virtual void DeleteSequences(WORD wArchivNr);
protected:

// Implementation
public:
	// Generated message map functions
	//{{AFX_MSG(CDlgBackupOld)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSelchangeComboDestination();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnCdr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void OnChanged();
	BOOL InitDrives();
	void SelectDrive(CIPCDrive* pDrive);
	void RemoveArchiveFromList(WORD wID);
	void BackupFinished();
	void CopyReadOnlyVersion();
	void CopyReadOnlyVersionFinished();

	void CountDownToControl();
	void RemoveSequenceFromList(DWORD dwID);

protected:
	CIPCSequenceRecords m_SequencesToCopy;

// Dialog Data
	//{{AFX_DATA(CDlgBackupOld)
	enum  { IDD = IDD_BACKUP };
	CSkinStatic	m_staticTime;
	CSkinListBox	m_ctrlSequences;
	CString	m_sSize;
	CString	m_sVelocity;
	CString	m_sBackupName;
	//}}AFX_DATA
};

#endif // !defined(AFX_DlgBackupOLD_H__6BC21BF7_E86E_425F_861D_65FBB7D7218B__INCLUDED_)
