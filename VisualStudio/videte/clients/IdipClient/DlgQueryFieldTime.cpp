// QueryFieldTimeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "DlgQueryFieldTime.h"
#include "DlgQuery.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgQueryFieldTime dialog


CDlgQueryFieldTime::CDlgQueryFieldTime(const CString& sName,CIPCField* pField,CDlgQuery* pParent)
	: CDlgQueryField(IDD_QUERY_FIELD_TIME,sName,pField,pParent)
{
	//{{AFX_DATA_INIT(CDlgQueryFieldTime)
	//}}AFX_DATA_INIT
	m_nInitToolTips = FALSE;
	MoveOnClientAreaClick(true);
	Create(IDD_QUERY_FIELD_TIME,pParent);
}


void CDlgQueryFieldTime::DoDataExchange(CDataExchange* pDX)
{
	CDlgQueryField::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgQueryFieldTime)
	DDX_Control(pDX, IDC_TIME, m_ctrlTime);
	DDX_Text(pDX, IDC_TXT_FIELD, m_sName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgQueryFieldTime, CDlgQueryField)
	//{{AFX_MSG_MAP(CDlgQueryFieldTime)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_TIME, OnDatetimechangeTime)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgQueryFieldTime message handlers

BOOL CDlgQueryFieldTime::OnInitDialog() 
{
	CDlgQueryField::OnInitDialog();
	AutoCreateSkinStatic();
	// TODO GF Workaround Resource Editor Bug
	m_ctrlTime.ModifyStyle(NULL, DTS_TIMEFORMAT);
	
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
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CDlgQueryFieldTime::OnDatetimechangeTime(NMHDR* pNMHDR, LRESULT* pResult) 
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
