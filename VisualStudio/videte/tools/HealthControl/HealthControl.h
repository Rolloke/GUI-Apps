// HealthControl.h : Hauptheaderdatei für die HealthControl-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error 'stdafx.h' muss vor dieser Datei in PCH eingeschlossen werden.
#endif

#include "resource.h"		// Hauptsymbole


// CHealthControlApp:
// Siehe HealthControl.cpp für die Implementierung dieser Klasse
//
class CHealthControlDlg;


#define SEND_RTE				_T("SendRTE")
#define POP_BALLOON				_T("PopBalloon")

#define REG_PATH_FAN			REG_PATH_PC_HEALTH _T("\\FAN")
#define FAN1_RPM_THRESHOLD		_T("Fan1RpmThreshold")
#define FAN2_RPM_THRESHOLD		_T("Fan2RpmThreshold")
#define FAN3_RPM_THRESHOLD		_T("Fan3RpmThreshold")
#define	FAN1_DEVICE				_T("Fan1Device")
#define	FAN2_DEVICE				_T("Fan2Device")
#define	FAN3_DEVICE				_T("Fan3Device")

#define REG_PATH_TEMPERATURE	REG_PATH_PC_HEALTH _T("\\Temperature")
#define	TEMP1_THRESHOLD			_T("Temp1Threshold")
#define	TEMP2_THRESHOLD			_T("Temp2Threshold")
#define	TEMP3_THRESHOLD			_T("Temp3Threshold")
#define	TEMP1_DEVICE			_T("Temp1Device")
#define	TEMP2_DEVICE			_T("Temp2Device")
#define	TEMP3_DEVICE			_T("Temp3Device")

#define REG_PATH_VOLTAGE		REG_PATH_PC_HEALTH _T("\\Voltage")
#define VCOREAMINTHRESHOLD		_T("VCoreAMinThreshold")
#define VCOREAMAXTHRESHOLD		_T("VCoreAMaxThreshold")
#define VCOREBMINTHRESHOLD		_T("VCoreBMinThreshold")
#define VCOREBMAXTHRESHOLD		_T("VCoreBMaxThreshold")
#define V12MINTHRESHOLD			_T("12VMinThreshold")
#define V12MAXTHRESHOLD			_T("12VMaxThreshold")
#define V5MINTHRESHOLD			_T("5VMinThreshold")
#define V5MAXTHRESHOLD			_T("5VMaxThreshold")
#define V3_3MINTHRESHOLD		_T("3_3VMinThreshold")
#define V3_3MAXTHRESHOLD		_T("3_3VMaxThreshold")
#define N12VMINTHRESHOLD		_T("N12VMinThreshold")
#define N12VMAXTHRESHOLD		_T("N12VMaxThreshold")
#define N5VMINTHRESHOLD			_T("N5VMinThreshold")
#define N5VMAXTHRESHOLD			_T("N5VMaxThreshold")

class CHealthControlApp : public CWinApp
{
public:
	CHealthControlApp();
	virtual ~CHealthControlApp();

// Überschreibungen
	//{{AFX_VIRTUAL(CHealthControlApp)
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL
	inline BOOL IsDts() const;
	inline BOOL IsShellMode() const;

// Implementierung
#if _MFC_VER >= 0x0710
	int SetLanguageParameters(UINT uCodePage, DWORD dwCPbits);
#endif

protected:
	//{{AFX_MSG(CHealthControlApp)
	afx_msg void OnAppExit();
	afx_msg void OnUpdateAppExit(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CHealthControlDlg *m_pDlg;
	BOOL	  m_bStandalone;
	BOOL	  m_bIconCreated;
private:
	BOOL      m_bIsDts;
	BOOL      m_bIsShellMode;
public:
   int About(void);
};

inline BOOL CHealthControlApp::IsDts() const
{
	return m_bIsDts;
}

inline BOOL CHealthControlApp::IsShellMode() const
{
	return m_bIsShellMode;
}

extern CHealthControlApp theApp;
