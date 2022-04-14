#pragma once


#include "SDITestView.h"

// CViewPM100IPAlarm form view

class CDocPM100IPAlarm;

class CViewPM100IPAlarm : public CSDITestView
{
	DECLARE_DYNCREATE(CViewPM100IPAlarm)

protected:
	CViewPM100IPAlarm();           // protected constructor used by dynamic creation
	virtual ~CViewPM100IPAlarm();

	// Attributes
public:
	CDocPM100IPAlarm* GetDocument();

public:
	enum { IDD = IDD_PM100IPALARM_FORM };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual	void InitializeData();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	DWORD m_dwBFR;
	DWORD m_dwTCC;
	DWORD m_dwAlarm;
	CString m_sMessage;
	CComboBox m_cbAlarm;
	afx_msg void OnEnChangeEditBfr();
	afx_msg void OnEnChangeEditTcc();
	afx_msg void OnCbnSelchangeComboAlarm();
};
#ifndef _DEBUG  // debug version in SDITestNcrView.cpp
inline CDocPM100IPAlarm* CViewPM100IPAlarm::GetDocument()
{ return (CDocPM100IPAlarm*)m_pDocument; }
#endif


