// LoadSaveSequencerDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Vision.h"
#include "LoadSaveSequencerDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_VALUE_NAME	256
/////////////////////////////////////////////////////////////////////////////
// CLoadSaveSequencerDialog dialog
CLoadSaveSequencerDialog::CLoadSaveSequencerDialog(BOOL bSave,const CString& sHost, CWnd* pParent /*=NULL*/)
	: CDialog(CLoadSaveSequencerDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLoadSaveSequencerDialog)
	m_SequenceName = _T("");
	//}}AFX_DATA_INIT
	m_bSave = bSave;
	m_sHost = sHost;
}
/////////////////////////////////////////////////////////////////////////////
void CLoadSaveSequencerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoadSaveSequencerDialog)
	DDX_Control(pDX, IDC_LIST_SEQUENCE, m_List);
	DDX_Control(pDX, IDOK, m_OKButton);
	DDX_Text(pDX, IDC_NAME, m_SequenceName);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CLoadSaveSequencerDialog, CDialog)
	//{{AFX_MSG_MAP(CLoadSaveSequencerDialog)
	ON_LBN_DBLCLK(IDC_LIST_SEQUENCE, OnDblclkListSequence)
	ON_LBN_SELCHANGE(IDC_LIST_SEQUENCE, OnSelchangeListSequence)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
CString CLoadSaveSequencerDialog::GetSequenceName()
{
	return m_SequenceName;
}
/////////////////////////////////////////////////////////////////////////////
void CLoadSaveSequencerDialog::OnDblclkListSequence() 
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
void CLoadSaveSequencerDialog::OnSelchangeListSequence() 
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
void CLoadSaveSequencerDialog::LoadSequenceNames()
{
	HKEY hKey;
	DWORD dwIndex;
	DWORD dwType = REG_NONE;
	TCHAR  ValueName[MAX_VALUE_NAME];
	DWORD dwcValueName = MAX_VALUE_NAME;
	long lResult;
	CString sValueName;
	CString sSection = _T("Vision\\Connections\\");
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
BOOL CLoadSaveSequencerDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CString sLoadOrSave;
	CString sTitle;

	sLoadOrSave.LoadString(m_bSave ? IDS_SAVE : IDS_LOAD);
	m_OKButton.SetWindowText(sLoadOrSave);
	GetWindowText(sTitle);
	sTitle += _T(" ");
	sTitle += sLoadOrSave;
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
void CLoadSaveSequencerDialog::OnOK() 
{
	UpdateData();
	BOOL bOK = TRUE;
	if (m_bSave)
	{
		CString sName;
		for (int i=0 ; i<m_List.GetCount() ; i++)
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
		CDialog::OnOK();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CLoadSaveSequencerDialog::OnDelete() 
{
	int i;

	i = m_List.GetCurSel();
	if (i!=LB_ERR)
	{
		CWK_Profile wkp;
		m_List.GetText(i,m_SequenceName);
		CString sSection = _T("Vision\\Connections\\");
		sSection += m_sHost;
		wkp.WriteString(sSection,m_SequenceName,NULL);
		m_SequenceName.Empty();
		m_List.DeleteString(i);
		UpdateData(FALSE);
	}
}
