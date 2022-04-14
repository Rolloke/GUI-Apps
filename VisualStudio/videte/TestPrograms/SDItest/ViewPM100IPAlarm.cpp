// ViewPM100IPAlarm.cpp : implementation file
//

#include "stdafx.h"
#include "SDITest.h"
#include "ViewPM100IPAlarm.h"
#include "Docpm100ipalarm.h"
#include ".\viewpm100ipalarm.h"


// CViewPM100IPAlarm

IMPLEMENT_DYNCREATE(CViewPM100IPAlarm, CSDITestView)

CViewPM100IPAlarm::CViewPM100IPAlarm()
	: CSDITestView(CViewPM100IPAlarm::IDD)
{
	m_dwBFR = 0;
	m_dwTCC = 0;
	m_dwAlarm = 1;
}

CViewPM100IPAlarm::~CViewPM100IPAlarm()
{
}

void CViewPM100IPAlarm::DoDataExchange(CDataExchange* pDX)
{
	CSDITestView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ALARM, m_cbAlarm);
	DDX_Text(pDX,IDC_EDIT_BFR,m_dwBFR);
	DDX_Text(pDX,IDC_EDIT_TCC,m_dwTCC);

	if (pDX->m_bSaveAndValidate)
	{
		int iCurSel = m_cbAlarm.GetCurSel();
		m_dwAlarm = m_cbAlarm.GetItemData(iCurSel);
		m_cbAlarm.GetLBText(iCurSel,m_sMessage);
	}
	else
	{
		for (int i=0;i<m_cbAlarm.GetCount();i++)
		{
			DWORD dwData = 	m_cbAlarm.GetItemData(i);
			if (dwData == m_dwAlarm)
			{
				m_cbAlarm.SetCurSel(i);
				break;
			}
		}
	}
}

BEGIN_MESSAGE_MAP(CViewPM100IPAlarm, CSDITestView)
	ON_EN_CHANGE(IDC_EDIT_BFR, OnEnChangeEditBfr)
	ON_EN_CHANGE(IDC_EDIT_TCC, OnEnChangeEditTcc)
	ON_CBN_SELCHANGE(IDC_COMBO_ALARM, OnCbnSelchangeComboAlarm)
END_MESSAGE_MAP()


// CViewPM100IPAlarm diagnostics

#ifdef _DEBUG
void CViewPM100IPAlarm::AssertValid() const
{
	CSDITestView::AssertValid();
}

void CViewPM100IPAlarm::Dump(CDumpContext& dc) const
{
	CSDITestView::Dump(dc);
}
/////////////////////////////////////////////////////////////////////////////
CDocPM100IPAlarm* CViewPM100IPAlarm::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDocPM100IPAlarm)));
	return (CDocPM100IPAlarm*)m_pDocument;
}
#endif //_DEBUG


// CViewPM100IPAlarm message handlers

void CViewPM100IPAlarm::InitializeData()
{
	int i = 0;
	CString s;

	for (DWORD dwAlarm = 1;dwAlarm<90;dwAlarm++)
	{
		s = CSDIControl::GetPM100AlarmDescription(dwAlarm);
		if (!s.IsEmpty())
		{
			i = m_cbAlarm.AddString(s);
			m_cbAlarm.SetItemData(i,dwAlarm);
		}
	}

	m_dwBFR = GetDocument()->m_dwBFR;
	m_dwTCC = GetDocument()->m_dwTCC;
	m_dwAlarm = GetDocument()->m_dwAlarm;

	UpdateData(FALSE);
}

void CViewPM100IPAlarm::OnEnChangeEditBfr()
{
	UpdateData();
	GetDocument()->m_dwBFR = m_dwBFR;
}

void CViewPM100IPAlarm::OnEnChangeEditTcc()
{
	UpdateData();
	GetDocument()->m_dwTCC = m_dwTCC;
}

void CViewPM100IPAlarm::OnCbnSelchangeComboAlarm()
{
	UpdateData();
	GetDocument()->m_dwAlarm = m_dwAlarm;
	GetDocument()->m_sMessage = m_sMessage;
}
