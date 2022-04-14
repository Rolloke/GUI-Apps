/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: savepage.cpp $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/savepage.cpp $
// CHECKIN:		$Date: 29.08.06 14:56 $
// VERSION:		$Revision: 31 $
// LAST CHANGE:	$Modtime: 29.08.06 14:56 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "systemverwaltung.h"

#include "ProcessList.h"
#include "SVPage.h"
#include "SavePage.h"

#include "SVDoc.h"
#include "SVView.h"

#include "Input.h"
#include "InputList.h"
#include "InputGroup.h"
#include "InputToOutput.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TCHAR BASED_CODE szSectionDebug[] = _T("SecurityServer\\Debug");

/////////////////////////////////////////////////////////////////////////////
// CSavePage dialog
IMPLEMENT_DYNAMIC(CSavePage, CSVPage)

/////////////////////////////////////////////////////////////////////////////
CSavePage::CSavePage(CSVView* pParent) : CSVPage(CSavePage::IDD)
{
	m_pParent = pParent;
	//{{AFX_DATA_INIT(CSavePage)
	m_bHold = FALSE;
	m_bInterval = FALSE;
	m_sName = _T("");
	m_bForever = FALSE;
	m_iCompression = -1;
	m_iResolution = -1;
	m_sRecTime = _T("1");
	m_sMainTime = _T("0");
	m_sPauseTime = _T("0");
	m_sNrPict = _T("1");
	//}}AFX_DATA_INIT

	m_pInputList = pParent->GetDocument()->GetInputs();
	m_pProcessList = pParent->GetDocument()->GetProcesses();
	m_pArchivInfoArray = pParent->GetDocument()->GetArchivs();
	m_pSelectedProcess = NULL;
	m_iSelectedItem = -1;

	m_bShowCompression = GetDongle().RunJaCobUnit1() || GetDongle().RunSaVicUnit1() || GetDongle().RunQUnit();


	Create(IDD,(CWnd*)m_pParent);
}

/////////////////////////////////////////////////////////////////////////////
CSavePage::~CSavePage()
{
}
/////////////////////////////////////////////////////////////////////////////
void CSavePage::ProcessToControl(CProcess* pProcess)
{
	if (pProcess)
	{
		// data from new selection to controls
		m_sRecTime.Format(_T("%d"),pProcess->GetRecordTime()/1000);
		m_sNrPict.Format(_T("%d"),pProcess->GetNrPict());
		m_bHold			= pProcess->IsHold();
		m_bInterval		= pProcess->IsInterval();
		m_sPauseTime.Format(_T("%d"),pProcess->GetPause()/1000);
		m_sMainTime.Format(_T("%d"),pProcess->GetDuration()/1000);
		if (m_bInterval && m_sMainTime == _T("0")) 
		{
			m_bForever	= TRUE;
		}
		else 
		{
			m_bForever	= FALSE;
		}
		m_sName	= pProcess->GetName();
		switch ( pProcess->GetResolution() )
		{
			case RESOLUTION_NONE:
				m_iResolution = -1;
				break;
			case RESOLUTION_QCIF:
				m_iResolution = 0;
				break;
			case RESOLUTION_2CIF:
				m_iResolution = 1;
				break;
			case RESOLUTION_4CIF:
				m_iResolution = 2;
				break;
			default:
				break;
		}
		switch ( pProcess->GetCompression() )
		{
			case COMPRESSION_NONE:
				m_iCompression = -1;
				break;
			case COMPRESSION_1:
				m_iCompression = 0;
				break;
			case COMPRESSION_2:
				m_iCompression = 1;
				break;
			case COMPRESSION_3:
				m_iCompression = 2;
				break;
			case COMPRESSION_4:
				m_iCompression = 3;
				break;
			case COMPRESSION_5:
				m_iCompression = 4;
				break;
			default:
				break;
		}
	}
	else 
	{
		m_sName.Empty();
		m_sRecTime = _T("1");
		m_sNrPict = _T("0");
		m_iResolution = -1;
		m_iCompression = -1;
		m_bHold = FALSE;
		m_bInterval = FALSE;
		m_sPauseTime = _T("0");
		m_sMainTime = _T("0");
		m_bForever = FALSE;
	}
	UpdateData(FALSE);

	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void CSavePage::ControlToProcess(CProcess* pProcess)
{
	Resolution res;
	Compression comp;

	if (pProcess)
	{
		UpdateData();
		if ( m_bInterval && !m_bHold && !m_bForever && m_sMainTime == _T("0") ) 
		{
			AfxMessageBox(IDP_INTERVAL_NO_FOREVER_NOR_MAINTIME, MB_OK);
			m_bForever = TRUE;
			UpdateData(FALSE);
			ShowHide();
		}
		pProcess->ClearArchivs();

		switch ( m_iResolution )
		{
			case 0:
				res = RESOLUTION_QCIF;
				break;
			case 1:
				res = RESOLUTION_2CIF;
				break;
			case 2:
				res = RESOLUTION_4CIF;
				break;
			default:
				res = RESOLUTION_NONE;
				break;
		}
		switch ( m_iCompression )
		{
			case 0:
				comp = COMPRESSION_1;
				break;
			case 1:
				comp = COMPRESSION_2;
				break;
			case 2:
				comp = COMPRESSION_3;
				break;
			case 3:
				comp = COMPRESSION_4;
				break;
			case 4:
				comp = COMPRESSION_5;
				break;
			default:
				comp = COMPRESSION_NONE;
				break;
		}

		pProcess->SetSave(m_sName,
						  m_bForever ? 0L : 1000 * _ttoi(m_sMainTime),
						  1000 * _ttoi(m_sPauseTime),
						  (WORD)_ttoi(m_sNrPict),
						  1000 * _ttoi(m_sRecTime),
						  res,
						  comp,
						  m_bHold,
						  m_bInterval
						  );
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSavePage::ShowHide()
{
	if ( m_pSelectedProcess ) 
	{
		m_editName.EnableWindow(TRUE);
		m_editRecTime.EnableWindow(TRUE);
		m_editNr.EnableWindow(TRUE);
		m_cbResolution.EnableWindow(TRUE);
		m_cbCompression.EnableWindow(TRUE);
		m_btnHold.EnableWindow(TRUE);
		m_btnInterval.EnableWindow(TRUE);
	}
	else 
	{
		m_bInterval = FALSE;
		m_editName.EnableWindow(FALSE);
		m_editRecTime.EnableWindow(FALSE);
		m_editNr.EnableWindow(FALSE);
		m_cbResolution.EnableWindow(FALSE);
		m_cbCompression.EnableWindow(FALSE);
		m_btnHold.EnableWindow(FALSE);
		m_btnInterval.EnableWindow(FALSE);
	}

	if (m_bInterval) 
	{
		m_sSec1.EnableWindow(TRUE);
		m_sSec2.EnableWindow(TRUE);
		m_btnMainTime.EnableWindow(TRUE);
		m_btnIntervalGrp.EnableWindow(TRUE);
		m_editPauseTime.EnableWindow(TRUE);
		m_editMainTime.EnableWindow(TRUE);
		m_sTxtPause.EnableWindow(TRUE);
		m_sTxtNotForever.EnableWindow(TRUE);
		if (m_bHold) 
		{
			m_btnForever.EnableWindow(FALSE);
		}
		else 
		{
			m_btnForever.EnableWindow(TRUE);

		}
		if (m_bForever) 
		{
			m_btnHold.EnableWindow(FALSE);
			m_editMainTime.EnableWindow(FALSE);
			m_sTxtNotForever.EnableWindow(FALSE);
		}
		else 
		{
			m_btnHold.EnableWindow(TRUE);
			m_editMainTime.EnableWindow(TRUE);
			m_sTxtNotForever.EnableWindow(TRUE);
		}
	}
	else 
	{
		m_sPauseTime = _T("0");
		m_bForever = FALSE;
		m_sMainTime = _T("0");
		m_sSec1.EnableWindow(FALSE);
		m_sSec2.EnableWindow(FALSE);
		m_btnForever.EnableWindow(FALSE);
		m_btnMainTime.EnableWindow(FALSE);
		m_btnIntervalGrp.EnableWindow(FALSE);
		m_editPauseTime.EnableWindow(FALSE);
		m_editMainTime.EnableWindow(FALSE);
		m_sTxtPause.EnableWindow(FALSE);
		m_sTxtNotForever.EnableWindow(FALSE);
	}

	if (!m_bShowCompression)
	{
		m_cbCompression.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TXTCOMPRESSION)->ShowWindow(SW_HIDE);
	}

	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSavePage::IsDataValid()
{
	if (m_pSelectedProcess) 
	{
		UpdateData();
		if (m_bHold && m_bInterval) 
		{
			if (m_bForever) 
			{
				AfxMessageBox(IDP_HOLD_INTERVAL_FOREVER, MB_OK);
				return FALSE;
			}
			else if (m_sMainTime.IsEmpty()) 
			{
				AfxMessageBox(IDP_HOLD_INTERVAL_NOTIME, MB_OK);
				return FALSE;
			}
		}

		if ( m_sNrPict == _T("0") ) 
		{
			AfxMessageBox(IDP_SAVE_NR_PICTURE_WRONG, MB_OK);
			m_editNr.SetFocus();
			m_editNr.SetSel(0,-1);
			return FALSE;
		}
		if ( _ttoi(m_sRecTime) < 1 ) 
		{
			AfxMessageBox(IDP_SAVE_ACTIVE_TIME_WRONG, MB_OK);
			m_editRecTime.SetFocus();
			m_editRecTime.SetSel(0,-1);
			return FALSE;
		}
		else if ( (_ttoi(m_sNrPict) / _ttoi(m_sRecTime)) > 50 ) 
		{
			AfxMessageBox(IDP_SAVE_PICTURE_FREQUENCE_WRONG, MB_OK);
			m_editNr.SetFocus();
			m_editNr.SetSel(0,-1);
			return FALSE;
		}
		else if ( (_ttoi(m_sNrPict) / _ttoi(m_sRecTime)) > 25 ) 
		{
			AfxMessageBox(IDS_SAVE_PICTURE_FREQUENCE_OVER25, MB_OK);
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CSavePage::ClearListBoxProcess()
{
	m_lbList.ResetContent();
}
/////////////////////////////////////////////////////////////////////////////
void CSavePage::FillListBoxProcess()
{
	ClearListBoxProcess();
	CProcess* pProcess;
	int iIndex = -1;
	for (int i=0;i<m_pProcessList->GetSize();i++) 
	{
		pProcess = m_pProcessList->GetAt(i);
		if (pProcess && pProcess->IsSave()) 
		{
			iIndex = m_lbList.AddString(pProcess->GetName());
			if (iIndex != -1) 
			{
				m_lbList.SetItemDataPtr(iIndex, (void*)pProcess);
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
void CSavePage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSavePage)
	DDX_Control(pDX, IDC_RESOLUTION, m_cbResolution);
	DDX_Control(pDX, IDC_COMPRESSION, m_cbCompression);
	DDX_Control(pDX, IDC_SAVE_INTERVAL, m_btnInterval);
	DDX_Control(pDX, IDC_SAVE_HOLD, m_btnHold);
	DDX_Control(pDX, IDC_SAVE_NAME, m_editName);
	DDX_Control(pDX, IDC_SAVE_LIST_PROCESS, m_lbList);
	DDX_Control(pDX, IDC_TXT_NOTFOREVER, m_sTxtNotForever);
	DDX_Control(pDX, IDC_TXT_SEC0, m_sTxtSec0);
	DDX_Control(pDX, IDC_RECTIME, m_editRecTime);
	DDX_Control(pDX, IDC_NR_PICTURE, m_editNr);
	DDX_Control(pDX, IDC_TXT_RECTIME, m_sTxtRecTime);
	DDX_Control(pDX, IDC_TXT_NR, m_sTxtNr);
	DDX_Control(pDX, IDC_TXTPAUSE, m_sTxtPause);
	DDX_Control(pDX, IDC_SAVE_PAUSETIME, m_editPauseTime);
	DDX_Control(pDX, IDC_SAVE_MAINTIME, m_editMainTime);
	DDX_Control(pDX, IDC_TXT_SEC2, m_sSec2);
	DDX_Control(pDX, IDC_TXT_SEC1, m_sSec1);
	DDX_Control(pDX, IDC_INTERVALGRP, m_btnIntervalGrp);
	DDX_Control(pDX, IDC_GESTIMEGRP, m_btnMainTime);
	DDX_Control(pDX, IDC_SAVE_FOREVER, m_btnForever);
	DDX_Check(pDX, IDC_SAVE_HOLD, m_bHold);
	DDX_Check(pDX, IDC_SAVE_INTERVAL, m_bInterval);
	DDX_Text(pDX, IDC_SAVE_NAME, m_sName);
	DDV_MaxChars(pDX, m_sName, 31);
	DDX_Check(pDX, IDC_SAVE_FOREVER, m_bForever);
	DDX_CBIndex(pDX, IDC_COMPRESSION, m_iCompression);
	DDX_CBIndex(pDX, IDC_RESOLUTION, m_iResolution);
	DDX_Text(pDX, IDC_RECTIME, m_sRecTime);
	DDX_Text(pDX, IDC_SAVE_MAINTIME, m_sMainTime);
	DDX_Text(pDX, IDC_SAVE_PAUSETIME, m_sPauseTime);
	DDX_Text(pDX, IDC_NR_PICTURE, m_sNrPict);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSavePage, CSVPage)
	//{{AFX_MSG_MAP(CSavePage)
	ON_BN_CLICKED(IDC_SAVE_INTERVAL, OnInterval)
	ON_EN_CHANGE(IDC_RECTIME, OnChangeRectime)
	ON_EN_CHANGE(IDC_NR_PICTURE, OnChangeNr)
	ON_EN_CHANGE(IDC_SAVE_MAINTIME, OnChangeMaintime)
	ON_BN_CLICKED(IDC_SAVE_FOREVER, OnForever)
	ON_LBN_SELCHANGE(IDC_SAVE_LIST_PROCESS, OnSelchangeListproc)
	ON_CBN_SELCHANGE(IDC_COMPRESSION, OnSelchangeCompression)
	ON_CBN_SELCHANGE(IDC_RESOLUTION, OnSelchangeResolution)
	ON_EN_CHANGE(IDC_SAVE_NAME, OnChangeName)
	ON_BN_CLICKED(IDC_SAVE_HOLD, OnHold)
	ON_EN_CHANGE(IDC_SAVE_PAUSETIME, OnChangePausetime)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSavePage message handlers
BOOL CSavePage::OnInitDialog() 
{
	CSVPage::OnInitDialog();

	m_cbResolution.AddString(_T("CIF (352x288)"));
	m_cbResolution.AddString(_T("2CIF(704x288)"));

	if (   GetDongle().RunTashaUnit1()
		|| GetDongle().RunQUnit()
		|| GetDongle().RunIPCameraUnit())
	{
		m_cbResolution.AddString(_T("4CIF(704x576)"));
	}

	BOOL bKB = GetDongle().RunJaCobUnit1() || GetDongle().RunSaVicUnit1();

	if (GetDongle().RunQUnit())
	{
		BOOL bMPEG4 = GetProfile().GetInt(szSectionDebug,_T("Mpeg4Recording"), TRUE);

		if (bMPEG4)
		{
			bKB = FALSE;
		}
	}

	if (bKB)
	{
		m_cbCompression.AddString(_T("40 kB"));
		m_cbCompression.AddString(_T("32 kB"));
		m_cbCompression.AddString(_T("24 kB"));
		m_cbCompression.AddString(_T("16 kB"));
		m_cbCompression.AddString(_T(" 8 kB"));
	}
	else
	{
		m_cbCompression.AddString(_T("2.0 MBit"));
		m_cbCompression.AddString(_T("1.5 MBit"));
		m_cbCompression.AddString(_T("1.0 MBit"));
		m_cbCompression.AddString(_T("0.5 MBit"));
		m_cbCompression.AddString(_T("0.25 MBit"));
	}

	FillListBoxProcess();
	if (m_lbList.GetCount()>0)
	{
		m_iSelectedItem = m_lbList.SetCurSel(0);
		m_pSelectedProcess = (CProcess*)m_lbList.GetItemDataPtr(m_iSelectedItem);
		ProcessToControl(m_pSelectedProcess);
	}
	else
	{
		m_iSelectedItem = -1;
		m_pSelectedProcess = NULL;
		ProcessToControl(NULL);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSavePage::CanNew()
{
	return m_pArchivInfoArray->GetSize()>0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSavePage::CanDelete()
{
	return m_lbList.GetCount()>0;
}
/////////////////////////////////////////////////////////////////////////////
void CSavePage::OnNew() 
{
	if (m_pSelectedProcess)
	{
		if (IsDataValid())
		{
			ControlToProcess(m_pSelectedProcess);
		}
		else
		{
			m_lbList.SetCurSel(m_iSelectedItem);
			return;
		}
	}
	m_pSelectedProcess = m_pProcessList->AddProcess();
	CString s;

	s.LoadString(IDS_NEW);
	m_pSelectedProcess->SetName(s);
	m_sName = s;
	UpdateData(FALSE);
	SetModified();
	m_iSelectedItem = m_lbList.AddString(m_sName);
	m_lbList.SetCurSel(m_iSelectedItem);
	m_lbList.SetItemDataPtr(m_iSelectedItem, m_pSelectedProcess);
	ProcessToControl(m_pSelectedProcess);
	ShowHide();

	m_editName.SetFocus();
	m_editName.SetSel(0,-1);
}
/////////////////////////////////////////////////////////////////////////////
void CSavePage::OnDelete() 
{
	if (m_pSelectedProcess)
	{
		// first check wether there is any Activation (CInputToOutput) with or 
		// process macro
		CInputGroup* pInputGroup;
		CInput* pInput;
		CInputToOutput* pInputToOutput;
		int i,j,k,c,d,e;
		BOOL bShowMB = TRUE;

		c = m_pInputList->GetSize();
		for (i=0;i<c;i++)
		{
			pInputGroup = m_pInputList->GetGroupAt(i);
			d = pInputGroup->GetSize();
			for (j=0;j<d;j++)
			{
				pInput = pInputGroup->GetInput(j);
				e = pInput->GetNumberOfInputToOutputs();
				for (k=e-1;k>=0;k--)
				{
					pInputToOutput = pInput->GetInputToOutput(k);
					if (pInputToOutput->GetProcessID() == m_pSelectedProcess->GetID())
					{
						// we still have one
						if (bShowMB)
						{
							if (IDYES==AfxMessageBox(IDP_DELETE_PROCESS_WITH_ACTIVATION,MB_YESNO))
							{
								bShowMB = FALSE;
							}
							else
							{
								return;
							}
						}
						pInput->DeleteInputToOutput(pInputToOutput);
					}
				} // inputstooutputs
			} // inputs
		}//groups
		if (bShowMB==FALSE)
		{
			m_pInputList->Save(GetProfile(),IsLocal());
		}

		m_pProcessList->DeleteProcess(m_pSelectedProcess);
		m_pSelectedProcess = NULL;
		m_lbList.DeleteString(m_iSelectedItem);

		int iCount = m_lbList.GetCount();
		if (iCount>0)
		{
			if (m_iSelectedItem==iCount) 
			{
				m_iSelectedItem = m_lbList.SetCurSel(m_iSelectedItem-1);
			}
			else 
			{
				m_iSelectedItem = m_lbList.SetCurSel(m_iSelectedItem);
			}
			m_pSelectedProcess = (CProcess*)m_lbList.GetItemDataPtr(m_iSelectedItem);
			ProcessToControl(m_pSelectedProcess);
		}
		else
		{
			ProcessToControl(NULL);
		}
		SetModified();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSavePage::OnSelchangeListproc() 
{
	int iCurSel = m_lbList.GetCurSel();
	if (iCurSel!=m_iSelectedItem)
	{
		if (IsDataValid())
		{
			ControlToProcess(m_pSelectedProcess);
			m_iSelectedItem = iCurSel;
			m_pSelectedProcess = (CProcess*)m_lbList.GetItemDataPtr(m_iSelectedItem);
			ProcessToControl(m_pSelectedProcess);
			
			// Change ML: Wenn der unterste Entrag in der Liste gewählt wird,
			//  dann die Liste automatisch um einen Eintrag nach oben scrollen.
			CRect ItemRect, ListRect;
			m_lbList.GetItemRect(0, ItemRect);
			m_lbList.GetClientRect(ListRect);
			int nVisibleEntries = ListRect.Height() / ItemRect.Height();
			if (iCurSel - m_lbList.GetTopIndex() >= nVisibleEntries-1)
			{
				if (m_lbList.GetTopIndex() + nVisibleEntries < m_lbList.GetCount())
				{
					m_lbList.SetTopIndex(m_lbList.GetTopIndex( ) + 1 ); 	
				}
			}
		}
		else
		{
			m_lbList.SetCurSel(m_iSelectedItem);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSavePage::OnChangeName() 
{
	SetModified();
	if( m_pSelectedProcess && UpdateData()) 
	{
		m_lbList.DeleteString(m_iSelectedItem);
		m_iSelectedItem = m_lbList.AddString(m_sName);
		m_lbList.SetItemDataPtr(m_iSelectedItem, m_pSelectedProcess);
		m_lbList.SetCurSel(m_iSelectedItem);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSavePage::OnChangeRectime() 
{
	UpdateData();
	CString sRecTime = m_sRecTime;
	int iCurSel = CheckDigits(sRecTime);
	if (iCurSel>=0)
	{
		m_sRecTime = sRecTime;
		UpdateData(FALSE);
		m_editRecTime.SetSel(iCurSel,iCurSel);
	}
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CSavePage::OnChangeNr() 
{
	UpdateData();
	CString sNrPict = m_sNrPict;
	int iCurSel = CheckDigits(sNrPict);
	if (iCurSel>=0)
	{
		m_sNrPict = sNrPict;
		UpdateData(FALSE);
		m_editNr.SetSel(iCurSel,iCurSel);
	}
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CSavePage::OnSelchangeCompression() 
{
	UpdateData();
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CSavePage::OnSelchangeResolution() 
{
	UpdateData();
	SetModified();
}

/////////////////////////////////////////////////////////////////////////////
void CSavePage::OnHold() 
{
	UpdateData();
	ShowHide();
	SetModified();
	if (!m_bHold && m_bInterval && !m_bForever && m_sMainTime == _T("0")) 
	{
		m_editMainTime.SetFocus();
		m_editMainTime.SetSel(0,-1);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CSavePage::OnInterval() 
{
	UpdateData();
	ShowHide();
	SetModified();
	if (   !m_bInterval 
		&&  m_bHold 
		&& !m_bForever 
		&& m_sMainTime == _T("0")) 
	{
		m_editMainTime.SetFocus();
		m_editMainTime.SetSel(0,-1);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSavePage::OnChangePausetime() 
{
	UpdateData();
	CString sPauseTime = m_sPauseTime;
	int iCurSel = CheckDigits(sPauseTime);
	if (iCurSel>=0)
	{
		m_sPauseTime = sPauseTime;
		UpdateData(FALSE);
		m_editPauseTime.SetSel(iCurSel,iCurSel);
	}
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CSavePage::OnForever() 
{
	UpdateData();
	ShowHide();
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CSavePage::OnChangeMaintime() 
{
	UpdateData();
	CString sMainTime = m_sMainTime;
	int iCurSel = CheckDigits(sMainTime);
	if (iCurSel>=0)
	{
		m_sMainTime = sMainTime;
		UpdateData(FALSE);
		m_editMainTime.SetSel(iCurSel,iCurSel);
	}
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CSavePage::SaveChanges() 
{
	WK_TRACE_USER(_T("Speicherprozess-Einstellungen wurden geändert\n"));
	UpdateData();
	ControlToProcess(m_pSelectedProcess);
	m_pProcessList->Save(GetProfile());
}
/////////////////////////////////////////////////////////////////////////////
void CSavePage::CancelChanges() 
{
	m_pProcessList->Reset();
	m_pProcessList->Load(GetProfile());
	ClearListBoxProcess();
	FillListBoxProcess();
	int iCount = m_lbList.GetCount();
	if (iCount>0)
	{
		if (m_iSelectedItem==iCount) 
		{
			m_iSelectedItem = m_lbList.SetCurSel(m_iSelectedItem-1);
		}
		else 
		{
			m_iSelectedItem = m_lbList.SetCurSel(m_iSelectedItem);
		}
		if (m_iSelectedItem != LB_ERR)
		{
			m_pSelectedProcess = (CProcess*)m_lbList.GetItemDataPtr(m_iSelectedItem);
			ProcessToControl(m_pSelectedProcess);
		}
	}
	else
	{
		m_pSelectedProcess = NULL;
		m_iSelectedItem = -1;
		ProcessToControl(NULL);
	}
	SetModified(FALSE,FALSE);
}
/////////////////////////////////////////////////////////////////////////////
