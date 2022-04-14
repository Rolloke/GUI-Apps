#pragma once

#include "SDITestView.h"
#include "afxwin.h"

// CViewPM100XML form view
class CDocPM100XML;

class CViewPM100XML : public CSDITestView
{
	DECLARE_DYNCREATE(CViewPM100XML)

protected:
	CViewPM100XML();           // protected constructor used by dynamic creation
	virtual ~CViewPM100XML();

public:
	CDocPM100XML* GetDocument();
	enum { IDD = IDD_PM100XML_FORM };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual	void InitializeData();

	DECLARE_MESSAGE_MAP()
public:
	DWORD m_dwBFR;
	DWORD m_dwTCC;
	DWORD m_dwAlarm;
	DWORD m_dwTicSerNum;
	CComboBox m_cbAlarm;
	afx_msg void OnEnChangeEditBfr();
	afx_msg void OnEnChangeEditTcc();
	afx_msg void OnCbnSelchangeComboAlarm();
	afx_msg void OnEnChangeEditTnr();
};
#ifndef _DEBUG  // debug version in SDITestNcrView.cpp
inline CDocPM100XML* CViewPM100XML::GetDocument()
{ return (CDocPM100XML*)m_pDocument; }
#endif


