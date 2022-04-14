// QueryFieldTimeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "recherche.h"
#include "QueryFieldTimeDialog.h"
#include "QueryDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQueryFieldTimeDialog dialog


CQueryFieldTimeDialog::CQueryFieldTimeDialog(const CString& sName,CIPCField* pField,CQueryDialog* pParent)
	: CQueryFieldDialog(IDD_QUERY_FIELD_TIME,sName,pField,pParent)
{
	//{{AFX_DATA_INIT(CQueryFieldTimeDialog)
	//}}AFX_DATA_INIT
	Create(IDD_QUERY_FIELD_TIME,pParent);
}


void CQueryFieldTimeDialog::DoDataExchange(CDataExchange* pDX)
{
	CQueryFieldDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQueryFieldTimeDialog)
	DDX_Control(pDX, IDC_TIME, m_ctrlTime);
	DDX_Text(pDX, IDC_TXT_FIELD, m_sName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQueryFieldTimeDialog, CQueryFieldDialog)
	//{{AFX_MSG_MAP(CQueryFieldTimeDialog)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_TIME, OnDatetimechangeTime)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQueryFieldTimeDialog message handlers

BOOL CQueryFieldTimeDialog::OnInitDialog() 
{
	CQueryFieldDialog::OnInitDialog();
	
	CString sValue = m_pField->GetValue();

	if (sValue.IsEmpty())
	{
		m_ctrlTime.SendMessage(DTM_SETSYSTEMTIME,GDT_NONE);
	}
	else
	{
		CSystemTime t;
		unsigned int nHour, nMinute, nSecond;
		t.GetLocalTime();
		if (3 == _stscanf(sValue,_T("%02d%02d%02d"), &nHour, &nMinute, &nSecond))
		{
			t.wHour   = (WORD)nHour;
			t.wMinute = (WORD)nMinute;
			t.wSecond = (WORD)nSecond;
			m_ctrlTime.SetTime(&t);
		}
	}
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CQueryFieldTimeDialog::OnDatetimechangeTime(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CSystemTime t;
	CString s;
	if (GDT_NONE!=m_ctrlTime.GetTime(&t))
	{
		s = t.GetDBTime();
	}
	m_pField->SetValue(s);
	
	*pResult = 0;
}
