// GAUnit.h : main header file for the GAUNIT application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "CameraCommandRecord.h"
#include "ControlRecord.h"
#include "CameraControlRecord.h"

class CCommInput;
class CCommWindow;
class CRS232;

/////////////////////////////////////////////////////////////////////////////
// CCommUnitApp:
// See GAUnit.cpp for the implementation of this class
//

class CCommUnitApp : public CWinApp
{
public:
	CCommUnitApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCommUnitApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	// Implementation
public:
	CameraControlType GetControlType(CSecID id, CControlRecord**ppCR=NULL, CCameraControlRecord**ppCCR=NULL);
	DWORD			  GetControlFunctions(CSecID id);
	DWORD			  GetControlFunctionsEx(CSecID id);
	void OnReadData(int iCom);
	void TraceData(int iCom, LPVOID pBuf, DWORD dwLen, BOOL bRead = TRUE);

#if _MFC_VER >= 0x0710
	int SetLanguageParameters(UINT uCodePage, DWORD dwCPbits);
#endif

public:
	void OnReset();
protected:
	void OnWriteData();
	void OnCameraCommand(CCameraCommandRecord* pCCommandR);

	void OnTest();

	//{{AFX_MSG(CCommUnitApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void LoadDebugOptions();
	void LoadCameraControls();

public:
	BOOL	m_bTraceCommand;
	BOOL	m_bTraceData;
	BOOL	m_bTraceRS232Events;
	int		m_iTestResponse;
private:
	CCommInput*	m_pInput;
	CCameraControlRecordArray m_CameraControlRecordArray;
	CControlRecordArray m_ControlRecordArray;
	CRS232* m_pRS232Transparent;
	CString m_sOpenComs;
	CSecID	m_RelayPTZID;

	friend class CCommWindow;
};
/////////////////////////////////////////////////////////////////////////////
extern CCommUnitApp theApp;
/////////////////////////////////////////////////////////////////////////////
