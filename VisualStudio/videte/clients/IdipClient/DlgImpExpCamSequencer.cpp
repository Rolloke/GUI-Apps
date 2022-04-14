// DlgImpExpCamSequencer.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "DlgImpExpCamSequencer.h"
#include "oemgui\res\applicationdefines.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_VALUE_NAME	256
/////////////////////////////////////////////////////////////////////////////
// CDlgImpExpCamSequencer dialog
CDlgImpExpCamSequencer::CDlgImpExpCamSequencer(BOOL bSave, CWnd* pParent /*=NULL*/)
	: CSkinDialog(CDlgImpExpCamSequencer::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgImpExpCamSequencer)
	m_SequenceName = _T("");
	//}}AFX_DATA_INIT
	m_bSave = bSave;

	m_nInitToolTips = FALSE;
	if (UseOEMSkin())
	{
		CreateTransparent(StyleBackGroundColorChangeSimple, m_BaseColor);
		// MoveOnClientAreaClick(false);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgImpExpCamSequencer::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgImpExpCamSequencer)
	DDX_Control(pDX, IDC_LIST_SEQUENCE, m_List);
	DDX_Control(pDX, IDOK, m_OKButton);
	DDX_Text(pDX, IDC_NAME, m_SequenceName);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDlgImpExpCamSequencer, CSkinDialog)
	//{{AFX_MSG_MAP(CDlgImpExpCamSequencer)
	ON_LBN_DBLCLK(IDC_LIST_SEQUENCE, OnDblclkListSequence)
	ON_LBN_SELCHANGE(IDC_LIST_SEQUENCE, OnSelchangeListSequence)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
CString CDlgImpExpCamSequencer::GetSequenceName()
{
	return m_SequenceName;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgImpExpCamSequencer::OnDblclkListSequence() 
{
	int i;

	i = m_List.GetCurSel();
	if (i!=LB_ERR)
	{
		m_List.GetText(i,m_SequenceName);
		UpdateData(FALSE);
	}
	OnOK();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgImpExpCamSequencer::OnSelchangeListSequence() 
{
	int i;

	i = m_List.GetCurSel();
	if (i!=LB_ERR)
	{
		m_List.GetText(i,m_SequenceName);
		UpdateData(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgImpExpCamSequencer::LoadSequenceNames()
{
	HKEY hKey;
	DWORD dwIndex;
	DWORD dwType = REG_NONE;
	TCHAR  ValueName[MAX_VALUE_NAME];
	DWORD dwcValueName = MAX_VALUE_NAME;
	long lResult;
	CString sValueName;
	CString sSection = _T("IdipClient\\Connections\\");
	CWK_Profile wkp;
	sSection += m_sHost;

	hKey = wkp.GetSectionKey(sSection);

	dwIndex = 0;

	for (;;)
	{
		dwcValueName = MAX_VALUE_NAME;
        ValueName[0] = '\0';

		lResult = RegEnumValue(
								hKey,	// handle of key to query 
								dwIndex,	// index of value to query 
								ValueName,	// address of buffer for value string 
								&dwcValueName,	// address for size of value buffer 
								NULL,	// reserved 
								&dwType,	// address of buffer for type code 
								NULL,	// address of buffer for value data 
								NULL 	// address for size of data buffer 
								);
		
		if (lResult!=ERROR_SUCCESS)
		{
			break;
		}
		dwIndex++;
		if (dwType==REG_SZ)
		{
			sValueName = ValueName;
			m_List.AddString(sValueName);
		}
	}

	RegCloseKey(hKey);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgImpExpCamSequencer::OnInitDialog() 
{
	CSkinDialog::OnInitDialog();
	AutoCreateSkinButtons();
	AutoCreateSkinStatic();
	AutoCreateSkinEdit();
	m_List.UpdateFont();
	
	CString sLoadOrSave;
	// RKE: die zusammengebastelten Texte 'klingen' in unterschiedlichen Sprachen etwas komisch
	// deutsch:  Kamerasatz öffnen
	// englisch: Camera Set Open, besser wäre: Open Camera Set
	// Lässt man das Basteln ist es OK so.
	// Der Hostname ist noch sinnvoller
	CString sTitle;
	sLoadOrSave.LoadString(m_bSave ? IDS_SAVE : IDS_LOAD);
	m_OKButton.SetWindowText(sLoadOrSave);
	GetWindowText(sTitle);
	sTitle += _T(" [");
	sTitle += m_sHost;
	sTitle += _T("]");
	SetWindowText(sTitle);

	LoadSequenceNames();

	if (!m_bSave)
	{
		if (m_List.GetCount()==0)
		{
			m_OKButton.EnableWindow(FALSE);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CDlgImpExpCamSequencer::OnOK() 
{
	UpdateData();
	BOOL bOK = TRUE;
	if (m_bSave)
	{
		CString sName;
		//changes for VisualStudio 2005
		int i = 0;
		for (i=0 ; i<m_List.GetCount() ; i++)
		{
			m_List.GetText(i, sName);
			if (sName == m_SequenceName)
			{
				// overwrite?
				CString sMsg;
				sMsg.Format(IDP_REPLACE_EXISTING_CAM_SWITCH, sName);
				if (AfxMessageBox(sMsg, MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2) == IDNO)
				{
					bOK = FALSE;
				}
				break;
			}
		}
	}
	if (bOK)
	{
		CSkinDialog::OnOK();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgImpExpCamSequencer::OnDelete() 
{
	int i;
	CString sAsk;
	sAsk.Format(IDS_ASK_FOR_DELETE, m_SequenceName);
	if (AfxMessageBox(sAsk, MB_ICONQUESTION|MB_YESNO) == IDYES)
	{

		i = m_List.GetCurSel();
		if (i!=LB_ERR)
		{
			CWK_Profile wkp;
			m_List.GetText(i,m_SequenceName);
			CString sSection = _T("IdipClient\\Connections\\");
			sSection += m_sHost;
			wkp.WriteString(sSection,m_SequenceName,NULL);
			m_SequenceName.Empty();
			m_List.DeleteString(i);
			UpdateData(FALSE);
		}
	}
}
