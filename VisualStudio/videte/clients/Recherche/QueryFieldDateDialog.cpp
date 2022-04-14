// QueryFieldDateDialog.cpp : implementation file
//

#include "stdafx.h"
#include "recherche.h"
#include "QueryFieldDateDialog.h"
#include "QueryDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQueryFieldDateDialog dialog


CQueryFieldDateDialog::CQueryFieldDateDialog(const CString& sName, CIPCField* pField,CQueryDialog* pParent)
	: CQueryFieldDialog(IDD_QUERY_FIELD_DATE,sName,pField,pParent)
{
	//{{AFX_DATA_INIT(CQueryFieldDateDialog)
	//}}AFX_DATA_INIT
	Create(IDD_QUERY_FIELD_DATE,pParent);
}


void CQueryFieldDateDialog::DoDataExchange(CDataExchange* pDX)
{
	CQueryFieldDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQueryFieldDateDialog)
	DDX_Control(pDX, IDC_DATE, m_ctrlDate);
	DDX_Text(pDX, IDC_TXT_FIELD, m_sName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQueryFieldDateDialog, CQueryFieldDialog)
	//{{AFX_MSG_MAP(CQueryFieldDateDialog)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATE, OnDatetimechangeDate)
	ON_NOTIFY(DTN_CLOSEUP, IDC_DATE, OnCloseupDate)
	ON_NOTIFY(DTN_USERSTRING, IDC_DATE, OnUserstringDate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQueryFieldDateDialog message handlers

BOOL CQueryFieldDateDialog::OnInitDialog() 
{
	CQueryFieldDialog::OnInitDialog();
	
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
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CQueryFieldDateDialog::OnDatetimechangeDate(NMHDR* pNMHDR, LRESULT* pResult) 
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
void CQueryFieldDateDialog::OnCloseupDate(NMHDR* pNMHDR, LRESULT* pResult) 
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
void CQueryFieldDateDialog::OnUserstringDate(NMHDR* pNMHDR, LRESULT* pResult) 
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
