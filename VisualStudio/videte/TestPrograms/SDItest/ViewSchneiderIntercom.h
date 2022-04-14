#pragma once
#include "SDITestView.h"

class CDocSchneiderIntercom;
// CViewSchneiderIntercom form view

class CViewSchneiderIntercom : public CSDITestView
{
	DECLARE_DYNCREATE(CViewSchneiderIntercom)

protected:
	CViewSchneiderIntercom();           // protected constructor used by dynamic creation
	virtual ~CViewSchneiderIntercom();

public:
	CDocSchneiderIntercom* GetDocument();
	enum { IDD = IDD_VIEWSCHNEIDERINTERCOM };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_sSubscriberNr;
	int m_iAlarmType;
	afx_msg void OnEnChangeEditSubscriber();
	afx_msg void OnBnClickedRadioOn();
	afx_msg void OnBnClickedRadioOff();
};
#ifndef _DEBUG  // debug version in SDITestNcrView.cpp
inline CDocSchneiderIntercom* CViewSchneiderIntercom::GetDocument()
{ return (CDocSchneiderIntercom*)m_pDocument; }
#endif




