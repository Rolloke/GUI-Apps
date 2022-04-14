// ViewSchneiderIntercom.cpp : implementation file
//

#include "stdafx.h"
#include "SDITest.h"
#include "ViewSchneiderIntercom.h"
#include "DocSchneiderIntercom.h"
#include ".\viewschneiderintercom.h"


// CViewSchneiderIntercom

IMPLEMENT_DYNCREATE(CViewSchneiderIntercom, CSDITestView)

CViewSchneiderIntercom::CViewSchneiderIntercom()
	: CSDITestView(CViewSchneiderIntercom::IDD)
{
	m_sSubscriberNr = _T("");
	m_iAlarmType = 0;
}

CViewSchneiderIntercom::~CViewSchneiderIntercom()
{
}

void CViewSchneiderIntercom::DoDataExchange(CDataExchange* pDX)
{
	CSDITestView::DoDataExchange(pDX);
	DDX_Text(pDX,IDC_EDIT_SUBSCRIBER,m_sSubscriberNr);
	DDX_Radio(pDX,IDC_RADIO_ON,m_iAlarmType);
}

BEGIN_MESSAGE_MAP(CViewSchneiderIntercom, CSDITestView)
	ON_EN_CHANGE(IDC_EDIT_SUBSCRIBER, OnEnChangeEditSubscriber)
	ON_BN_CLICKED(IDC_RADIO_ON, OnBnClickedRadioOn)
	ON_BN_CLICKED(IDC_RADIO_OFF, OnBnClickedRadioOff)
END_MESSAGE_MAP()


// CViewSchneiderIntercom diagnostics

#ifdef _DEBUG
void CViewSchneiderIntercom::AssertValid() const
{
	CSDITestView::AssertValid();
}

void CViewSchneiderIntercom::Dump(CDumpContext& dc) const
{
	CSDITestView::Dump(dc);
}
CDocSchneiderIntercom* CViewSchneiderIntercom::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDocSchneiderIntercom)));
	return (CDocSchneiderIntercom*)m_pDocument;
}
#endif //_DEBUG


// CViewSchneiderIntercom message handlers

void CViewSchneiderIntercom::OnEnChangeEditSubscriber()
{
	UpdateData();
	GetDocument()->m_sSubscriberNr = m_sSubscriberNr;
}

void CViewSchneiderIntercom::OnBnClickedRadioOn()
{
	UpdateData();
	GetDocument()->m_iAlarmType = m_iAlarmType;
}

void CViewSchneiderIntercom::OnBnClickedRadioOff()
{
	UpdateData();
	GetDocument()->m_iAlarmType = m_iAlarmType;
}
