/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDIUnit.h $
// ARCHIVE:		$Archive: /Project/Units/SDIUnit/SDIUnit.h $
// CHECKIN:		$Date: 7/20/06 2:20p $
// VERSION:		$Revision: 32 $
// LAST CHANGE:	$Modtime: 7/20/06 11:00a $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#ifndef _SDIUnitApp_H_
#define _SDIUnitApp_H_

#include "SDIControlRecord.h"

// Zeit in ms, die nach einem Reset erneute Resets abblockt (verhindert Dauerreset)
#define RESET_DELAY_TIME	1000

/////////////////////////////////////////////////////////////////////////////
// CSDIUnitApp:
// See SDIUnit.cpp for the implementation of this class
//
class CSDIUnitApp : public CWinApp
{
public:
	CSDIUnitApp();

// attributes
public:
	inline BOOL	TraceiSTData() const;
	inline BOOL	TraceRS232Data() const;
	inline BOOL	TraceRS232DataHex() const;
	inline BOOL	TraceRS232DataAscii() const;
	inline BOOL	TraceRS232Events() const;
	inline BOOL	IgnoreNonDigitsInValue() const;

	inline CSDIControlRecordArray& GetControlRecords();
	inline BOOL	IsInMainThread();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSDIUnitApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSDIUnitApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Implementation
public:
protected:
	void OnReset();
private:
	void LoadDebugConfiguration();
	void LoadSDIControlRecords();
	CSDIControlRecord* CreateControlRecord(CWK_Profile& wkp, SDIControlType type, int iCom);
	CSDIControlRecord* CreateControlRecord(CWK_Profile& wkp, SDIControlType type, int iPort, LPCTSTR szAddress);
	void AppendControlRecord(CWK_Profile& wkp, 
							 const CString& sSection,
							 CSDIControlRecord* pSDIControlRecord);

private:
	BOOL			m_bTraceiSTData;
	BOOL			m_bTraceRS232Data;
	BOOL			m_bTraceHex;
	BOOL			m_bTraceAscii;
	BOOL			m_bTraceEvents;
	BOOL			m_bIgnoreNonDigitsInValue;
	CWK_Dongle*		m_pDongle;
	CString			m_sOpenComs;
	DWORD			m_dwLastReset;
	CSDIControlRecordArray	m_CSDICRArray;

	friend class CSDIWindow;
};
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSDIUnitApp::TraceiSTData() const
{
	return m_bTraceiSTData;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSDIUnitApp::TraceRS232Data() const
{
	return m_bTraceRS232Data;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSDIUnitApp::TraceRS232DataHex() const
{
	return m_bTraceHex;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSDIUnitApp::TraceRS232DataAscii() const
{
	return m_bTraceAscii;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSDIUnitApp::TraceRS232Events() const
{
	return m_bTraceEvents;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL CSDIUnitApp::IgnoreNonDigitsInValue() const
{
	return m_bIgnoreNonDigitsInValue;
}
/////////////////////////////////////////////////////////////////////////////
extern CSDIUnitApp theApp;
/////////////////////////////////////////////////////////////////////////////
inline CSDIControlRecordArray& CSDIUnitApp::GetControlRecords()
{
	return m_CSDICRArray;
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CSDIUnitApp::IsInMainThread()
{
	return m_nThreadID == GetCurrentThreadId() ? TRUE : FALSE;
}
CString GetValue(const CString& sData, const CString& sParameter);
CSystemTime GetSystemTimeFromDesignaTime(DWORD dwDesignaTime);
/////////////////////////////////////////////////////////////////////////////
#endif
