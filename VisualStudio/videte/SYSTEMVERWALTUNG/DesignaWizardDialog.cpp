// DesignaWizardDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "DesignaWizardDialog.h"
#include ".\designawizarddialog.h"


// CDesignaWizardDialog dialog

IMPLEMENT_DYNAMIC(CDesignaWizardDialog, CDialog)
CDesignaWizardDialog::CDesignaWizardDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CDesignaWizardDialog::IDD, pParent)
{
	m_dwBFR = 1;
	m_dwTCC = 1;
	m_SDICT =  SDICT_DESIGNA_PM_100_ALARM;
}

CDesignaWizardDialog::~CDesignaWizardDialog()
{
}

void CDesignaWizardDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_BFR, m_editBFR);
	DDX_Control(pDX, IDC_EDIT_TCC, m_editTCC);
	DDX_Control(pDX, IDC_LIST_PM100_ALARM, m_listAlarm);
	DDX_Control(pDX, IDC_LIST_PM100_XML, m_listXML);
	DDX_Text(pDX,IDC_EDIT_TCC,m_dwTCC);
	DDX_Text(pDX,IDC_EDIT_BFR,m_dwBFR);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_comboType);

	if (pDX->m_bSaveAndValidate)
	{
		int i;
		// control 2 data
		m_PM100Alarm.RemoveAll();
		for (i=0;i<m_listAlarm.GetCount();i++)
		{
			if (m_listAlarm.GetCheck(i))
			{
				m_PM100Alarm.Add(m_listAlarm.GetItemData(i));
			}
		}
		m_PM100XML.RemoveAll();
		for (i=0;i<m_listXML.GetCount();i++)
		{
			if (m_listXML.GetCheck(i))
			{
				m_PM100XML.Add(m_listXML.GetItemData(i));
			}
		}
	}
	else
	{
		// data 2 control
		int i;
		for (i=0;i<m_listAlarm.GetCount();i++)
		{
			m_listAlarm.SetCheck(i,Find(m_PM100Alarm,m_listAlarm.GetItemData(i)));
		}
		for (i=0;i<m_listXML.GetCount();i++)
		{
			m_listXML.SetCheck(i,Find(m_PM100XML,m_listXML.GetItemData(i)));
		}
	}
}


BEGIN_MESSAGE_MAP(CDesignaWizardDialog, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, OnCbnSelchangeComboType)
END_MESSAGE_MAP()


// CDesignaWizardDialog message handlers

BOOL CDesignaWizardDialog::OnInitDialog()
{
	CDialog::OnInitDialog();


	int i = 0;
	DWORD dwAlarm;
	CString a,s;

	GetWindowText(s);
	s += _T(" ")+m_sCameraName;
	SetWindowText(s);

	s.LoadString(IDS_CASHBOX);
	m_comboType.AddString(s);
	s.LoadString(IDS_BARRIER);
	a.LoadString(IDS_CARDREADER);
	m_comboType.AddString(s+_T("/")+a);

	/*
	intialisierung als kasse
	SetCashBox(m_PM100Alarm,m_PM100XML);
	m_comboType.SetCurSel(0);
	*/

	for (dwAlarm=1, i=0; dwAlarm<90; dwAlarm++)
	{
		a = CSDIControl::GetPM100AlarmDescription(dwAlarm);
		if (!a.IsEmpty())
		{
			s.Format(_T("%s, %d"),a,dwAlarm);
			i = m_listAlarm.AddString(s);
			m_listAlarm.SetItemData(i,dwAlarm);
		}
	}
	for (dwAlarm=1, i=0; dwAlarm<200; dwAlarm++)
	{
		a = CSDIControl::GetPM100XMLDescription(dwAlarm);
		if (!a.IsEmpty())
		{
			s.Format(_T("%s, %d"),a,dwAlarm);
			i = m_listXML.AddString(s);
			m_listXML.SetItemData(i,dwAlarm);
		}
	}

	if (m_SDICT == SDICT_DESIGNA_PM_ABACUS)
	{
		CString s;
		GetDlgItemText(IDC_TXT_PM100_ALARM,s);
		s.Replace(_T("PM 100"),_T("PM Abacus"));
		SetDlgItemText(IDC_TXT_PM100_ALARM,s);
		GetDlgItemText(IDC_TXT_PM100_XML,s);
		s.Replace(_T("PM 100"),_T("PM Abacus"));
		SetDlgItemText(IDC_TXT_PM100_XML,s);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
///////////////////////////////////////////////////////////////////
void CDesignaWizardDialog::OnCbnSelchangeComboType()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	int i = m_comboType.GetCurSel();
	switch (i) 
	{
	case 0:
		SetCashBox(m_PM100Alarm,m_PM100XML);
		break;
	case 1:
		SetCardreaderBarrier(m_PM100Alarm,m_PM100XML);
		break;
	default:
		TRACE(_T("unknown Designa TCC type\n"));
		break;
	}
	UpdateData(FALSE);
}
///////////////////////////////////////////////////////////////////
void CDesignaWizardDialog::SetCardreaderBarrier(CDWordArray& alarm,CDWordArray& xml)
{
	alarm.RemoveAll();
	xml.RemoveAll();

	DWORD alarms[] = {3,15,31,40,79};
	DWORD xmls[] = {15,186,187,188,190};
	int i;

	for (i=0;i<sizeof(alarms)/sizeof(alarms[0]);i++)
	{
		alarm.Add(alarms[i]);
	}
	for (i=0;i<sizeof(xmls)/sizeof(xmls[0]);i++)
	{
		xml.Add(xmls[i]);
	}
}
///////////////////////////////////////////////////////////////////
void CDesignaWizardDialog::SetCashBox(CDWordArray& alarm, CDWordArray& xml)
{
	alarm.RemoveAll();
	xml.RemoveAll();

	DWORD alarms[] = {2,3,4,6,25,27,38,63,80,89};
	DWORD xmls[] = {15,188,190};
	int i;

	for (i=0;i<sizeof(alarms)/sizeof(alarms[0]);i++)
	{
		alarm.Add(alarms[i]);
	}
	for (i=0;i<sizeof(xmls)/sizeof(xmls[0]);i++)
	{
		xml.Add(xmls[i]);
	}
}
///////////////////////////////////////////////////////////////////
BOOL CDesignaWizardDialog::Find(const CDWordArray& a, DWORD dw)
{
	BOOL bFound = FALSE;
	
	for (int i=0;i<a.GetSize()&&!bFound;i++)
	{
		bFound = a[i] == dw;
	}

	return bFound;
}
