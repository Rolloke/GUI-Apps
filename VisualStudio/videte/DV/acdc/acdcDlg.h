// acdcDlg.h : header file
//

#if !defined(AFX_ACDCDLG_H__F08B2698_4C0A_47A5_A5C1_82A6C32D778C__INCLUDED_)
#define AFX_ACDCDLG_H__F08B2698_4C0A_47A5_A5C1_82A6C32D778C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "exitcode.h"

class CNeroBurn;
class CNeroThread;
class CBurnContext;
class CProducer;



/////////////////////////////////////////////////////////////////////////////
// CAcdcDlg dialog

class CAcdcDlg : public CDialog
{
// Construction
public:
	CAcdcDlg(CWnd* pParent = NULL);	// standard constructor
// Dialog Data
	//{{AFX_DATA(CAcdcDlg)
	enum { IDD = IDD_ACDC_DIALOG };
	CButton	m_CtrlOK;
	CEdit	m_EdtMessage;
	CProgressCtrl	m_prgCtrl;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAcdcDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	
//functions
public:
	void OnIdle();

	//get the MainFunction. This MainFunktion is set fromout the CIPC
	//and indicates what this program (Acdc.exe) has to do next.
	MainFunction	GetMainFunction();

	//set the new MainFunktion.
	void			SetMainFunction(MainFunction mf);

	//set the progress bar in programs window (gui)
	void		SetProgress(DWORD dwPercent);

	//appends new message line in message window (gui)
	void AppendMessage(CString sMessage);

	void StopNeroThread();
	
	//set the time in seconds, hpw it`ll take to erase the not empty cdrw
	void SetCDRWErasingTime(DWORD dwTime);

	void	GetFilesToBackup(CStringArray& saFiles);
	CString GetVolumeName();
	void SetVolumeName(CString sName);
	BurnProducer GetBurnProducer();

	CNeroThread* GetNeroThread();

	void ConfirmVolumeInfos();
	void ConfirmSession();
	BurnProducer GetInstalledBurnSoftware();
	void SetFilesToBackup(const CStringArray& sPathnames);
	BOOL IsValidNeroVersion();
	EXITCODE SetVolumeProperties(CBurnContext* pBurnContext);
	
	void OnException();
	void OnAppExit();
	BOOL CanChangeDevice();
	void SetCurrentDrive(CString sDriveLetter);
	CString GetUsedDriveLetter();
	void SetIsInBackup(BOOL bIsInBackup);
	CProducer* GetProducer();

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CAcdcDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDummyAbort();
	virtual void OnOK();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnAbout();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
	afx_msg BOOL OnDeviceChange( UINT nEventType, DWORD dwData );
	afx_msg long OnBurnCDInfo(WPARAM wParam, LPARAM lParam);
	afx_msg long OnBurnWriteIso(WPARAM wParam, LPARAM lParam);
	afx_msg long OnNewMessage(WPARAM wParam, LPARAM lParam);
	afx_msg long OnBurnError(WPARAM wParam, LPARAM lParam);
	afx_msg long DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam);
	afx_msg long OnTrayClicked(WPARAM wParam, LPARAM lParam);
	afx_msg long OnNotifyDrive(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	void CDRWEraseProgress();	//set the progress of erasing cdrw

//members
private:
	CStringArray	m_saFilesToBackup;	//list of files/directories to backup
	CString			m_sVolumeName;		//name of the volume (CD,DVD) to burn
	DWORD			m_dwUsedDriveLetter;	//drive of the drive to burn to
	BurnProducer	m_BurnProducer;		//which burn software is installed (Nero, WinXP)


	MainFunction	m_MainFunktion;	//the current MainFunction
	CNeroThread*	m_pNeroThread;	//the Burn Thread (only one at present for burning)
	CNeroBurn*		m_pNeroBurn;	//the burn object which controls NeroAPI, runs in a thread (m_pNeroThread)
	DWORD			m_dwCDRWErasingTime;	//seconds to wait for complete erasing cdrw
	DWORD			m_dwCDRWProgress;		//countdown of the CDRW erasing time

	DWORD			m_dwTick;
	BOOL			m_bValidNeroVersion;
	CString			m_sUsedDriveLetter;
	BOOL			m_bIsInBackup;
	CProducer*		m_pProducer;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACDCDLG_H__F08B2698_4C0A_47A5_A5C1_82A6C32D778C__INCLUDED_)

