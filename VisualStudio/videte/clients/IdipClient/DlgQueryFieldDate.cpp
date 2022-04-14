// QueryFieldDateDialog.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "DlgQueryFieldDate.h"
#include "DlgQuery.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgQueryFieldDate dialog


CDlgQueryFieldDate::CDlgQueryFieldDate(const CString& sName, CIPCField* pField,CDlgQuery* pParent)
	: CDlgQueryField(IDD_QUERY_FIELD_DATE,sName,pField,pParent)
{
	//{{AFX_DATA_INIT(CDlgQueryFieldDate)
	//}}AFX_DATA_INIT
//	MoveOnClientAreaClick(true);
	m_nInitToolTips = FALSE;
	Create(IDD_QUERY_FIELD_DATE,pParent);
}


void CDlgQueryFieldDate::DoDataExchange(CDataExchange* pDX)
{
	CDlgQueryField::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgQueryFieldDate)
	DDX_Control(pDX, IDC_DATE, m_ctrlDate);
	DDX_Text(pDX, IDC_TXT_FIELD, m_sName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgQueryFieldDate, CDlgQueryField)
	//{{AFX_MSG_MAP(CDlgQueryFieldDate)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATE, OnDatetimechangeDate)
	ON_NOTIFY(DTN_CLOSEUP, IDC_DATE, OnCloseupDate)
	ON_NOTIFY(DTN_USERSTRING, IDC_DATE, OnUserstringDate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgQueryFieldDate message handlers

BOOL CDlgQueryFieldDate::OnInitDialog() 
{
	CDlgQueryField::OnInitDialog();
	AutoCreateSkinStatic();
	
	CString sValue = m_pField->GetValue();
	m_ctrlDate.SetFormat(_T("ddd, dd. MMM yyy"));
	if (sValue.IsEmpty())
	{
		m_ctrlDate.SendMessage(DTM_SETSYSTEMTIME,GDT_NONE);
	}
	else
	{
		CSystemTime t;
		t.GetLocalTime();
		unsigned int nYear, nMonth, nDay;
		if (3 == _stscanf(sValue,_T("%04d%02d%02d"),&nYear, &nMonth, &nDay))
		{
			t.wYear  = (WORD)nYear;
			t.wMonth = (WORD)nMonth;
			t.wDay   = (WORD)nDay;
			m_ctrlDate.SetTime(&t);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CDlgQueryFieldDate::OnDatetimechangeDate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData();
	CSystemTime t;
	CString s;
	if (GDT_NONE!=m_ctrlDate.GetTime(&t))
	{
		s = t.GetDBDate();
	}
	m_pField->SetValue(s);
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgQueryFieldDate::OnCloseupDate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData();
	CSystemTime t;
	CString s;
	if (GDT_NONE!=m_ctrlDate.GetTime(&t))
	{
		s = t.GetDBDate();
	}
	m_pField->SetValue(s);
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgQueryFieldDate::OnUserstringDate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData();
	CSystemTime t;
	CString s;
	if (GDT_NONE!=m_ctrlDate.GetTime(&t))
	{
		s = t.GetDBDate();
	}
	m_pField->SetValue(s);
	
	*pResult = 0;
}
