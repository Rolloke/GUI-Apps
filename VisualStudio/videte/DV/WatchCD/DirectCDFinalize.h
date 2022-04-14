// DirectCDFinalize.h: interface for the CDirectCDFinalize class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRECTCDFINALIZE_H__40B4FE66_EB4F_11D3_AE54_004005A1D890__INCLUDED_)
#define AFX_DIRECTCDFINALIZE_H__40B4FE66_EB4F_11D3_AE54_004005A1D890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWatchCDDlg;
class CDirectCDError;
class CDirectCDStart;

class CDirectCDFinalize 
{

// Construction
public:
	CDirectCDFinalize(CWatchCDDlg* pWatchCD, 
					  CDirectCDStart* pDCDStart,
					  CDirectCDError* pDCDError);

	virtual ~CDirectCDFinalize();
	

	// Implementation
public:
	BOOL IsFinalized();
	BOOL FindFinalizeDlg();
	BOOL FindCollectInfoDlg();
	BOOL FindFinalizingDlg();
	static BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam);
	BOOL IsDeciveLocked();

	// Implementation
protected:
		
	void OnFinalizeExecDirectCD();
	void OnFinalizeIsCDRReadable();
	void OnFinalizeFindFinalizeDlg();
	void OnFinalizeSetProtectDisc();
	void OnFinalizeClickFinish();
	void OnFinalizeFindCollectInfoDlg();
	void OnFinalizeFindEjectingDlg();
	void OnFinalizeWaitForSuccess();
	void OnFinalizeError();	

	void ChangeDCDFinalizeStatus(DirectCDStatus DCDFinalizeStatus);

	BOOL IsCDRReadable();
	// Implementation
private:

	void CloseFinalizeDialogs();
	// data member
private:

	CWatchCDDlg*		m_pWatchCD;
	CDirectCDError*		m_pDCDError;
	CDirectCDStart*		m_pDCDStart;
	DirectCDStatus		m_DCDFinalizeStatus;
	DirectCDErrorStatus	m_DCDErrorStatus;

	HWND			m_hDCDFinalizeDlg;
	HWND			m_hDCDCollectInfoDlg;
	HWND			m_hDCDFinalizingDlg;
	DWORD			m_dwTickCount;
	DWORD			m_dwIDCheckbox;
	DWORD			m_dwIDFinishButton;
	HWND			m_hFinalizeCheckbox;
	CString			m_sDirectCD;
	BOOL			m_bDeviceLockStatus;
};



#endif // !defined(AFX_DIRECTCDFINALIZE_H__40B4FE66_EB4F_11D3_AE54_004005A1D890__INCLUDED_)
