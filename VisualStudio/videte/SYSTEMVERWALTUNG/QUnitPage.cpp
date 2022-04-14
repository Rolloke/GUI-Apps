// QUnitPage.cpp : implementation file
//

#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "QUnitPage.h"
#include ".\qunitpage.h"

static TCHAR BASED_CODE szQUnitExe[] = _T("QUnit.exe");
//#define VIDEO_NORM_AUTO	0
#define VIDEO_NORM_PAL	1
#define VIDEO_NORM_NTSC	2

#define VIDEO_FORMAT_JPEG	0
#define VIDEO_FORMAT_MPEG4	1

#define VIDEO_AGC_ON   0	//Value are the same like in UDP API
#define VIDEO_AGC_OFF  3	//Value are the same like in UDP API

static TCHAR BASED_CODE szSectionDebug[] = _T("SecurityServer\\Debug");
static TCHAR BASED_CODE szSectionQUnitFrontEnd[] = _T("QUnit\\FrontEnd");

// CQUnitPage dialog

IMPLEMENT_DYNAMIC(CQUnitPage, CSVPage)
CQUnitPage::CQUnitPage(CSVView* pParent)
: CSVPage(CQUnitPage::IDD)
{
	m_pParent			= pParent;
	m_pInputList		= pParent->GetDocument()->GetInputs();
	m_pOutputList		= pParent->GetDocument()->GetOutputs();

	m_bQ = FALSE;
	m_iVideoNorm	= VIDEO_NORM_PAL;
	m_iCurrentVideoNorm = m_iVideoNorm;

	m_iVideoFormat	= VIDEO_FORMAT_MPEG4;
	m_iVideoAGC		= VIDEO_AGC_ON;	 
	m_bRestartQUnit = FALSE;

	m_sAppName = WK_APP_NAME_QUNIT;

	Create(IDD,pParent);
}

CQUnitPage::~CQUnitPage()
{
}

void CQUnitPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_Q, m_bQ);
	DDX_Radio(pDX, IDC_RADIO_JPEG, m_iVideoFormat);
	DDX_Radio(pDX, IDC_RADIO_AGC_ON, m_iVideoAGC);
	DDX_Radio(pDX, IDC_RADIO_AUTO, m_iVideoNorm);
}


BEGIN_MESSAGE_MAP(CQUnitPage, CSVPage)
	ON_BN_CLICKED(IDC_CHECK_Q, OnBnClickedCheckQ)
	ON_BN_CLICKED(IDC_RADIO_PAL, OnBnClickedRadioPal)
	ON_BN_CLICKED(IDC_RADIO_NTSC, OnBnClickedRadioNtsc)
	ON_BN_CLICKED(IDC_RADIO_JPEG, OnBnClickedRadioJpeg)
	ON_BN_CLICKED(IDC_RADIO_MPEG4, OnBnClickedRadioMpeg4)
	ON_BN_CLICKED(IDC_RADIO_AGC_ON, OnBnClickedRadioAgcOn)
	ON_BN_CLICKED(IDC_RADIO_AGC_OFF, OnBnClickedRadioAgcOff)
END_MESSAGE_MAP()


// CQUnitPage message handlers
////////////////////////////////////////////////////////////////////////////////////
void CQUnitPage::OnBnClickedCheckQ()
{
	UpdateData(TRUE);
	if (m_bQ)
	{
		SetModified();
	}
	else
	{
		if (AfxMessageBox(IDP_HARDWARE_GROUP_DELETE, MB_YESNO|MB_DEFBUTTON2) == IDNO )
			m_bQ = TRUE;
		else
			SetModified();
	}
	EnableDlgControlsQ();
}
////////////////////////////////////////////////////////////////////////////////////
void CQUnitPage::EnableDlgControlsQ()
{
	// Enable/Disable Dialogcontrols

	GetDlgItem(IDC_RADIO_MPEG4)->EnableWindow(m_bQ);	
	GetDlgItem(IDC_RADIO_JPEG)->EnableWindow(m_bQ);	
	GetDlgItem(IDC_RADIO_AGC_ON)->EnableWindow(m_bQ);	
	GetDlgItem(IDC_RADIO_AGC_OFF)->EnableWindow(m_bQ);	
	GetDlgItem(IDC_RADIO_PAL)->EnableWindow(m_bQ);	
	GetDlgItem(IDC_RADIO_NTSC)->EnableWindow(m_bQ);	
}
////////////////////////////////////////////////////////////////////////////////////
BOOL CQUnitPage::IsDataValid()
{
	UpdateData();			  
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////////
void CQUnitPage::SaveChanges()
{
	WK_TRACE_USER(_T("Q-Einstellungen wurden geändert\n"));
	UpdateData(TRUE);

	// Q aktualisieren
	if ( m_bQ ) 
	{
		GetDlgQData();
	}
	else 
	{
		// Gruppen loeschen
		CProcessList* pProcessList = m_pParent->GetDocument()->GetProcesses();
		CString sGroupname (_T("QUnit"));
		DeleteProcessesOfExtensionCards(pProcessList, m_pOutputList, sGroupname);

		m_pInputList->DeleteGroup(SM_Q_INPUT);
		m_pInputList->DeleteGroup(SM_Q_MD_INPUT);
		m_pOutputList->DeleteGroup(SM_Q_OUTPUT_CAMERA);
		m_pOutputList->DeleteGroup(SM_Q_OUTPUT_RELAY);
	}

	// Einstellungen speichern
	SaveQSettings();

	GetProfile().WriteString(theApp.GetModuleSection(),WK_APP_NAME_QUNIT,m_bQ ? szQUnitExe : _T(""));

	SaveInputsAndOutputsAfterCheckingActivations();
	SetModified(FALSE,FALSE);
	m_pParent->PostMessage(WM_USER, USER_MSG_UPDATE_PARENT_TREE, (1<<IMAGE_INPUT)|(1<<IMAGE_RELAY)|(1<<IMAGE_CAMERA));
}
////////////////////////////////////////////////////////////////////////////////////
void CQUnitPage::CancelChanges()
{
	LoadQSettings();
	UpdateData(FALSE);
	EnableDlgControlsQ();

	if ( IsDataValid() ) 
	{
		SetModified(FALSE,FALSE);
	}
	else 
	{
		SetModified();
	}
}
////////////////////////////////////////////////////////////////////////////////////
BOOL CQUnitPage::OnInitDialog()
{
	CSVPage::OnInitDialog();

	// Q Einstellungen aus der Registry laden.
	CancelChanges();

	if (!IsLocal())
	{
		DisableAll();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

////////////////////////////////////////////////////////////////////////////////////
void CQUnitPage::LoadQSettings()
{
	// Ist eine Q Unit im System eingetragen?
	m_bQ = (GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_QUNIT, _T("")) != _T(""));

	//load videonorm
	CString sNorm =	GetProfile().GetString(szSectionQUnitFrontEnd,_T("VideoNorm"),_T("pal"));

	if (0==sNorm.CompareNoCase(_T("pal")))
	{
		m_iVideoNorm = VIDEO_NORM_PAL;
	}
	else if (0==sNorm.CompareNoCase(_T("ntsc")))
	{
		m_iVideoNorm = VIDEO_NORM_NTSC;
	}

	m_iCurrentVideoNorm = m_iVideoNorm;

	//load videoformat
    int iFormat = VIDEO_FORMAT_MPEG4;
	m_iVideoFormat = GetProfile().GetInt(szSectionDebug,_T("Mpeg4Recording"), iFormat);

	//load AGC setting
	int iAGC = VIDEO_AGC_ON;
	iAGC = GetProfile().GetInt(szSectionQUnitFrontEnd,_T("AGCMode"), iAGC);
	switch (iAGC)
	{
	case 0:
		m_iVideoAGC = 0;
		break;
	case 3:
		m_iVideoAGC = 1;
		break;
	default:
		m_iVideoAGC = 0;
		break;
	}
}
////////////////////////////////////////////////////////////////////////////////////
void CQUnitPage::SaveQSettings()
{
	// Kameratype sichern
	CString sNorm = _T("pal");
	
	if(m_iCurrentVideoNorm != m_iVideoNorm)
	{
		m_bRestartQUnit = TRUE;
	}
	
	switch (m_iVideoNorm)
	{
	case VIDEO_NORM_PAL:
		sNorm = _T("pal");
		break;
	case VIDEO_NORM_NTSC:
		sNorm = _T("ntsc");
		break;
	}
	GetProfile().WriteString(szSectionQUnitFrontEnd,_T("VideoNorm"),sNorm);

	// Videoformat sichern
	GetProfile().WriteInt(szSectionDebug,_T("Mpeg4Recording"),m_iVideoFormat);

	//AGC Einstellung sichern
	//m_iVideoAGC == 0 -> AGC is ON -> VIDEO_AGC_ON (Q Unit value: 0)
	//m_iVideoAGC == 1 -> AGC is OFF -> VIDEO_AGC_OFF (Q Unit value: 3)
	int iAGC = VIDEO_AGC_ON;
	m_iVideoAGC == 0 ? iAGC = VIDEO_AGC_ON : iAGC = VIDEO_AGC_OFF;
	GetProfile().WriteInt(szSectionQUnitFrontEnd,_T("AGCMode"),iAGC);

	// Q Unit  beenden und neu starten
	if (m_bRestartQUnit)
	{
		WK_TRACE(_T("## m_bRestartQUnit ist true\n"));
		if (WK_IS_RUNNING(m_sAppName))
		{

			CWnd *pWnd = FindWindow(NULL, m_sAppName);
			if (pWnd && IsWindow(pWnd->m_hWnd))
			{
				CWnd *pWndOk = pWnd->GetDlgItem(IDOK);
				if (pWndOk && IsWindow(pWnd->m_hWnd))
				{
					pWnd->PostMessage(WM_COMMAND,(WPARAM)MAKELONG(IDOK,BN_CLICKED),(LPARAM)pWndOk->m_hWnd);
					WK_TRACE(_T("## Nachricht geschickt\n"));
				}
			}
			else
			{
				WK_TRACE(_T("## TKR Window QUnit nicht gefunden\n"));
			}
		}
		else
		{
			WK_TRACE(_T("## TKR QUnit Appname nicht gefunden\n"));
		}

	}
	else
	{
		WK_TRACE(_T("## m_bRestartQUnit ist false\n"));
	}

}
////////////////////////////////////////////////////////////////////////////////////
void CQUnitPage::GetDlgQData()
{
	CInputGroup* pInputGroup = NULL;
	CInputGroup* pInputGroupMD = NULL;
	COutputGroup* pOutputGroup = NULL;
	int iAnzahl = 0;

	// Anzahl der Inputs einlesen

	// Daten der Inputgruppe aktualisieren, wenn bereits vorhanden...
	pInputGroup = m_pInputList->GetGroupBySMName(SM_Q_INPUT);
	if ( pInputGroup)
	{
		pInputGroup->SetSize((WORD)NR_INPUTS_Q);
	}
	// ...sonst neue Gruppe anlegen
	else
	{
		pInputGroup = m_pInputList->AddInputGroup(NULL, NR_INPUTS_Q, SM_Q_INPUT);
	}

	iAnzahl = GetDongle().GetNrOfCameras();


	// Daten der MD-Inputgruppe aktualisieren, wenn bereits vorhanden...
	pInputGroupMD = m_pInputList->GetGroupBySMName(SM_Q_MD_INPUT);
	if (pInputGroupMD)
	{
		pInputGroupMD->SetSize((WORD)iAnzahl);
	}
	// ...sonst neue Gruppe anlegen
	else
	{
		pInputGroupMD = m_pInputList->AddInputGroup(NULL, iAnzahl, SM_Q_MD_INPUT);
	}

	// Daten der Kamera-Outputgruppe aktualisieren, wenn bereits vorhanden...
	pOutputGroup = m_pOutputList->GetGroupBySMName(SM_Q_OUTPUT_CAMERA);
	if ( pOutputGroup) 
	{
		pOutputGroup->SetSize((WORD)iAnzahl);
	}
	// ...sonst neue Gruppe anlegen
	else 
	{
		pOutputGroup = m_pOutputList->AddOutputGroup(NULL, iAnzahl, SM_Q_OUTPUT_CAMERA);
	}

	// Anzahl der Relay Outputs einlesen

	// Daten der Relay-Outputgruppe aktualisieren, wenn bereits vorhanden...
	pOutputGroup = m_pOutputList->GetGroupBySMName(SM_Q_OUTPUT_RELAY);
	if ( pOutputGroup) 
	{
		pOutputGroup->SetSize((WORD)NR_OUTPUTS_RELAY_Q);
	}
	// ...sonst neue Gruppe anlegen
	else 
	{
		pOutputGroup = m_pOutputList->AddOutputGroup(NULL, NR_OUTPUTS_RELAY_Q, SM_Q_OUTPUT_RELAY);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CQUnitPage::OnBnClickedRadioPal()
{
	UpdateData(TRUE);
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CQUnitPage::OnBnClickedRadioNtsc()
{
	UpdateData(TRUE);
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CQUnitPage::OnBnClickedRadioJpeg()
{
	UpdateData(TRUE);
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CQUnitPage::OnBnClickedRadioMpeg4()
{
	UpdateData(TRUE);
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CQUnitPage::OnBnClickedRadioAgcOn()
{
	UpdateData(TRUE);
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CQUnitPage::OnBnClickedRadioAgcOff()
{
	UpdateData(TRUE);
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CQUnitPage::SaveNewDongleFeaturesAfterExpansion()
{
	SaveChanges();
}