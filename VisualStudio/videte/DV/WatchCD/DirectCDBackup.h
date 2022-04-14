// DirectCDBackup.h: interface for the CDirectCDBackup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRECTCDBACKUP_H__40B4FE67_EB4F_11D3_AE54_004005A1D890__INCLUDED_)
#define AFX_DIRECTCDBACKUP_H__40B4FE67_EB4F_11D3_AE54_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWatchCDDlg;
class CDirectCDError;

class CDirectCDBackup
{

// Construction
public:

	CDirectCDBackup(CWatchCDDlg* pWatchCD, CDirectCDError* pDCDError);
	virtual ~CDirectCDBackup();

protected:

	// Implementation
public:
	BOOL IsBackupComplete();
	BOOL StartBackup();
	BOOL WaitForBackupComplete();

	// Implementation
protected:
	void OnBackupStartBackup();
	void OnBackupWaitForBackupComplete();
	void OnBackupError();

	// Implementation
private:
	void ChangeDCDBackupStatus(DirectCDStatus DCDBackupStatus);

	// data member
private:

	CWatchCDDlg* m_pWatchCD;
	CDirectCDError* m_pDCDError;
	DirectCDStatus m_DCDBackupStatus;
};

#endif // !defined(AFX_DIRECTCDBACKUP_H__40B4FE67_EB4F_11D3_AE54_004005A1D890__INCLUDED_)
