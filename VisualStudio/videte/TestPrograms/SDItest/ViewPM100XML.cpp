// ViewPM100XML.cpp : implementation file
//

#include "stdafx.h"
#include "SDITest.h"
#include "ViewPM100XML.h"
#include "DocPM100XML.h"


// CViewPM100XML

IMPLEMENT_DYNCREATE(CViewPM100XML, CSDITestView)

CViewPM100XML::CViewPM100XML()
	: CSDITestView(CViewPM100XML::IDD)
{
	m_dwBFR = 0;
	m_dwTCC = 0;
	m_dwAlarm = 0;
}

CViewPM100XML::~CViewPM100XML()
{
}

void CViewPM100XML::DoDataExchange(CDataExchange* pDX)
{
	CSDITestView::DoDataExchange(pDX);
	DDX_Text(pDX,IDC_EDIT_TCC,m_dwTCC);
	DDX_Text(pDX,IDC_EDIT_BFR,m_dwBFR);
	DDX_Text(pDX,IDC_EDIT_TNR,m_dwTicSerNum);
	DDX_Control(pDX, IDC_COMBO_ALARM, m_cbAlarm);
	if (pDX->m_bSaveAndValidate)
	{
		int iCurSel = m_cbAlarm.GetCurSel();
		m_dwAlarm = m_cbAlarm.GetItemData(iCurSel);
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

BEGIN_MESSAGE_MAP(CViewPM100XML, CSDITestView)
	ON_EN_CHANGE(IDC_EDIT_BFR, OnEnChangeEditBfr)
	ON_EN_CHANGE(IDC_EDIT_TCC, OnEnChangeEditTcc)
	ON_CBN_SELCHANGE(IDC_COMBO_ALARM, OnCbnSelchangeComboAlarm)
	ON_EN_CHANGE(IDC_EDIT_TNR, OnEnChangeEditTnr)
END_MESSAGE_MAP()


// CViewPM100XML diagnostics

#ifdef _DEBUG
void CViewPM100XML::AssertValid() const
{
	CSDITestView::AssertValid();
}

void CViewPM100XML::Dump(CDumpContext& dc) const
{
	CSDITestView::Dump(dc);
}
/////////////////////////////////////////////////////////////////////////////
CDocPM100XML* CViewPM100XML::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDocPM100XML)));
	return (CDocPM100XML*)m_pDocument;
}
#endif //_DEBUG


// CViewPM100XML message handlers
void CViewPM100XML::InitializeData()
{
	int i = 0;
	CString s;

	for (DWORD dwAlarm = 1;dwAlarm<200;dwAlarm++)
	{
		s = CSDIControl::GetPM100XMLDescription(dwAlarm);
		if (!s.IsEmpty())
		{
			i = m_cbAlarm.AddString(s);
			m_cbAlarm.SetItemData(i,dwAlarm);
		}
	}

	m_dwBFR = GetDocument()->m_dwBFR;
	m_dwTCC = GetDocument()->m_dwTCC;
	m_dwAlarm = GetDocument()->m_dwAlarm;
	m_dwTicSerNum = GetDocument()->m_dwTicSerNum;

	UpdateData(FALSE);
}

void CViewPM100XML::OnEnChangeEditBfr()
{
	UpdateData();
	GetDocument()->m_dwBFR = m_dwBFR;
}

void CViewPM100XML::OnEnChangeEditTcc()
{
	UpdateData();
	GetDocument()->m_dwTCC = m_dwTCC;
}

void CViewPM100XML::OnCbnSelchangeComboAlarm()
{
	UpdateData();
	GetDocument()->m_dwAlarm = m_dwAlarm;
}

void CViewPM100XML::OnEnChangeEditTnr()
{
	UpdateData();
	GetDocument()->m_dwTicSerNum = m_dwTicSerNum;
}
