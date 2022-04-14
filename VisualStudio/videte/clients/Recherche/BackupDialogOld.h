// BackupDialogOld.h: interface for the CBackupDialogOld class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BACKUPDIALOGOLD_H__6BC21BF7_E86E_425F_861D_65FBB7D7218B__INCLUDED_)
#define AFX_BACKUPDIALOGOLD_H__6BC21BF7_E86E_425F_861D_65FBB7D7218B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BackupDialog.h"

class CBackupDialogOld : public CBackupDialog  
{
// Construction
public:
	CBackupDialogOld(CServer* pServer, CRechercheView* pParent);
	virtual ~CBackupDialogOld();

// operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBackupDialogOld)
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
	//{{AFX_MSG(CBackupDialogOld)
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
	//{{AFX_DATA(CBackupDialogOld)
	enum { IDD = IDD_BACKUP };
	CListBox	m_ctrlArchives;
	CButton	m_ctrlCDR;
	CStatic	m_staticState;
	CStatic	m_staticToCopy;
	CStatic	m_staticType;
	CStatic	m_staticFree;
	CStatic	m_staticTime;
	CButton	m_ctrlClose;
	CButton	m_ctrlCancel;
	CButton	m_ctrlOK;
	CProgressCtrl	m_ctrlProgress;
	CListBox	m_ctrlSequences;
	CComboBox	m_comboDestination;
	CString	m_sSize;
	CString	m_sVelocity;
	CString	m_sBackupName;
	//}}AFX_DATA
};

#endif // !defined(AFX_BACKUPDIALOGOLD_H__6BC21BF7_E86E_425F_861D_65FBB7D7218B__INCLUDED_)
