/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: outputpage.cpp $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/outputpage.cpp $
// CHECKIN:		$Date: 23.08.06 16:21 $
// VERSION:		$Revision: 68 $
// LAST CHANGE:	$Modtime: 23.08.06 16:20 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"
#include "globals.h"
#include "SystemVerwaltung.h"

#include "SVDoc.h"
#include "SVView.h"

#include "TreeList.h"
#include "Output.h"
#include "OutputPage.h"
#include "OutputGroup.h"
#include "OutputList.h"

#include "Input.h"
#include "InputList.h"
#include "InputGroup.h"
#include "InputToOutput.h"
#include ".\outputpage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define MAX_OUTPUTS_ON_PAGE	12

static TCHAR BASED_CODE szMicoUnitPCIExe[] = _T("MicoUnitPCI.exe");
static TCHAR BASED_CODE szMiCoSectionFormat[] = _T("MicoUnitPCI\\Device%d\\Mapping\\Card%d\\Source%d");

static TCHAR BASED_CODE szSourceType[] = _T("SourceType");
static TCHAR BASED_CODE szAVPortY[] = _T("AVPortY");
static TCHAR BASED_CODE szAVPortC[] = _T("AVPortC");

static TCHAR BASED_CODE szFBAS[] = _T("FBAS");
static TCHAR BASED_CODE szSVHS[] = _T("SVHS");

/////////////////////////////////////////////////////////////////////////////
// COutputPage property page
IMPLEMENT_DYNAMIC(COutputPage, CSVPage)

/////////////////////////////////////////////////////////////////////////////
COutputPage::COutputPage(CSVView* pParent,COutputGroup* pOutputGroup) : CSVPage(COutputPage::IDD)
{
//******************************************************************
// This is the Camera page now!!!
//******************************************************************
	m_pParent = pParent;
	//{{AFX_DATA_INIT(COutputPage)
	m_iScroll = 0;
	//}}AFX_DATA_INIT
	m_pOutputGroup		= pOutputGroup;
	m_sGroup			= pOutputGroup->GetName();
	m_idGroup			= pOutputGroup->GetID();
	m_pOutputList		= pParent->GetDocument()->GetOutputs();
	m_pInputList		= pParent->GetDocument()->GetInputs();
	m_bAnySN			= GetDongle().RunCommUnit();
	m_bShowTermination  = FALSE;
	m_bShowVHS = TRUE;

	if (0 == pOutputGroup->GetSMName().Find(SM_TASHA_OUTPUT_CAMERAS))
	{
		// alle Tasha's können die Terminierung abschalten
		m_bShowTermination = TRUE;
		m_bShowVHS = FALSE;
	}
	if (0 == pOutputGroup->GetSMName().Find(SM_Q_OUTPUT_CAMERA))
	{
		// alle Q's können die Terminierung nicht abschalten
		m_bShowTermination = FALSE;
		m_bShowVHS = FALSE;
	}
	if (0 == pOutputGroup->GetSMName().Find(SM_IP_CAMERA_OUTPUT_CAMERA))
	{
		// alle IP-Kameras können die Terminierung nicht abschalten
		m_bShowTermination = FALSE;
		m_bShowVHS = FALSE;
	}

	if (pOutputGroup->GetSMName() == SM_MICO_OUTPUT_CAMERAS)
	{
		// nur Jacob 3.1 kann Terminierung abschalten
		// Read Flag from registry
		m_bShowTermination	= GetProfile().GetInt(_T("JaCobUnit\\Device1\\General"), _T("SoftwareTermination"), TRUE);
	}
	else if (pOutputGroup->GetSMName() == SM_MICO_OUTPUT_CAMERAS2)
	{
		// nur Jacob 3.1 kann Terminierung abschalten
		// Read Flag from registry
		m_bShowTermination	= GetProfile().GetInt(_T("JaCobUnit\\Device2\\General"), _T("SoftwareTermination"), TRUE);
	}
	else if (pOutputGroup->GetSMName() == SM_MICO_OUTPUT_CAMERAS3)
	{
		// nur Jacob 3.1 kann Terminierung abschalten
		// Read Flag from registry
		m_bShowTermination	= GetProfile().GetInt(_T("JaCobUnit\\Device3\\General"), _T("SoftwareTermination"), TRUE);
	}
	else if (pOutputGroup->GetSMName() == SM_MICO_OUTPUT_CAMERAS4)
	{
		// nur Jacob 3.1 kann Terminierung abschalten
		// Read Flag from registry
		m_bShowTermination	= GetProfile().GetInt(_T("JaCobUnit\\Device4\\General"), _T("SoftwareTermination"), TRUE);
	}
	m_nInitToolTips = TOOLTIPS_SIMPLE;

	Create(IDD,m_pParent);
}
/////////////////////////////////////////////////////////////////////////////
COutputPage::~COutputPage()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL COutputPage::StretchElements()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COutputPage::IsDataValid()
{
	int i,c;
	COutputDialog* pOutputDialog = NULL;
	c = m_OutputDialogs.GetSize();
	for (i=0;i<c;i++)
	{
		pOutputDialog = m_OutputDialogs.GetAt(i);
		pOutputDialog->ControlToOutput();
	}

	if (m_bAnySN)
	{
		return IsCameraOK() && IsSNOK();
	}
	else
	{
		return IsCameraOK();
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL COutputPage::IsCameraOK()
{
	if (m_pOutputGroup->HasCameras())
	{
		int i,c;
		COutputDialog* pOutputDialog1 = NULL;
		COutputDialog* pOutputDialog2 = NULL;
		ExOutputType exType1 = EX_OUTPUT_OFF;
		ExOutputType exType2 = EX_OUTPUT_OFF;

		c = m_OutputDialogs.GetSize();
		if (c&1) c--;	// test only even number of cameras

		for (i=0;i<c;i+=2)
		{
			// Kamera 1,3,5,7,9,11...
			pOutputDialog1 = m_OutputDialogs.GetAt(i); 
			// Kamera 2,4,6,8,10,12...
			pOutputDialog2 = m_OutputDialogs.GetAt(i+1); 
			exType1 = pOutputDialog1->m_pOutput->GetExOutputType();
			exType2 = pOutputDialog2->m_pOutput->GetExOutputType();
			if ( (exType1 == EX_OUTPUT_CAMERA_SVHS) && (exType2 != EX_OUTPUT_OFF))
			{
//				TRACE(_T("%d %s\n"),i,pOutputDialog1->m_pOutput->GetName());
//				TRACE(_T("%d %s\n"),i+1,pOutputDialog2->m_pOutput->GetName());
				AfxMessageBox(IDP_COCO_OUTPUT_CAMERA_ERROR, MB_ICONSTOP);
				return FALSE;
			}
		}
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COutputPage::IsSNOK()
{
	// now from 20.02.97
	// multiple com
	// multiple cam
	int i,c,j;
	COutputDialog* pOutputDialog = NULL;
	COutputDialog* pOutputDialog2 = NULL;

	c = m_OutputDialogs.GetSize();

	for (i=0;i<c;i++)
	{
		pOutputDialog = m_OutputDialogs.GetAt(i);
		if (pOutputDialog && pOutputDialog->m_bSN)
		{
			// check type
			if (pOutputDialog->m_eCameraType == CCT_UNKNOWN)
			{
				AfxMessageBox(IDP_NO_SN_TYPE_SELECTED, MB_ICONSTOP);
				return FALSE;
			}
			// check COM
			if (pOutputDialog->m_sCom.IsEmpty())
			{
				AfxMessageBox(IDP_NO_COM_SELECTED, MB_ICONSTOP);
				return FALSE;
			}
			for (j=i+1;j<c;j++)
			{
				pOutputDialog2 = m_OutputDialogs.GetAt(j);
				if (pOutputDialog2 && pOutputDialog2->m_bSN)
				{
					if (pOutputDialog->m_sCom == pOutputDialog2->m_sCom)
					{
						if (pOutputDialog->m_eCameraType != pOutputDialog2->m_eCameraType)
						{
							AfxMessageBox(IDP_COMM_DOUBLE, MB_ICONSTOP);
							return FALSE;
						}
						else
						{
							if (pOutputDialog->m_dwSNID == pOutputDialog2->m_dwSNID)
							{
								AfxMessageBox(IDP_COMM_DIFFERENT, MB_ICONSTOP);
								return FALSE;
							}
						}
					}
				}

			}
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void COutputPage::ShowHideStuff()
{
	if (!m_pOutputGroup || m_pOutputGroup->GetSize()==0 )
	{
		m_staticOutputNr.ShowWindow(SW_HIDE);
		m_staticOutputType.ShowWindow(SW_HIDE);
		m_staticOutputComment.ShowWindow(SW_HIDE);
		if (!m_bAnySN)
		{
			m_staticOutputSN.ShowWindow(SW_HIDE);
		}
	}
	else
	{
		if (m_bAnySN)
		{
			m_staticOutputSN.ShowWindow(SW_SHOW);
			m_staticManu.ShowWindow(SW_SHOW);
			m_staticCom.ShowWindow(SW_SHOW);
			m_staticID.ShowWindow(SW_SHOW);
		}
		m_staticOutputNr.ShowWindow(SW_SHOW);
		m_staticOutputType.ShowWindow(SW_SHOW);
		m_staticOutputComment.ShowWindow(SW_SHOW);
	}
}
/////////////////////////////////////////////////////////////////////////////
void COutputPage::CheckComPorts()
{
	int i, nFind, nG, nLS, nO, nGS;
	CString sPort, sAll = IsCommPortUsed(GetProfile(), GET_ALL_AS_CSV);
	CStringArray saAll;
	SplitString(sAll, saAll, _T(","));
	for (i=0; i<saAll.GetCount(); i++)
	{
		sPort = saAll.GetAt(i);
		nFind = sPort.Find(_T("PTZ_"));
		if (nFind != -1)
		{
			CameraControlType cct = NameToCameraControlType(sPort.Mid(nFind + 4));
			if (cct != CCT_UNKNOWN)
			{
				BOOL bFound = FALSE;
				nLS = m_pOutputList->GetSize();
				for (nG=0; nG<nLS; nG++)
				{
					COutputGroup *pGroup = m_pOutputList->GetGroupAt(nG);
					nGS = pGroup->GetSize();
					for (nO=0; nO<nGS; nO++)
					{
						COutput*pO = pGroup->GetOutput(nO);
						if (pO->IsCamera())
						{
							if (pO->GetSN() && pO->GetCameraControlType() == cct)
							{
								bFound = TRUE;
								break;
							}
						}
						else
						{
							break;
						}
					}
					if (bFound)
					{
						break;
					}
				}
				if (!bFound)
				{
					int nPort = 0;
					_stscanf(sPort, COMM_IDENTIFIER, &nPort);
					SetCommPortUsed(GetProfile(), nPort, _T("")); 
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void COutputPage::SaveChanges() 
{
	int i,c;
	BOOL bSN = FALSE;
	COutputDialog* pOutputDialog;

	WK_TRACE_USER(_T("Kamera-Einstellungen wurden geändert\n"));

	CheckComPorts();

	c = m_OutputDialogs.GetSize();
	for (i=0;i<c;i++)
	{
		pOutputDialog = m_OutputDialogs.GetAt(i);
		if (pOutputDialog)
		{
			pOutputDialog->ControlToOutput();
			bSN |= pOutputDialog->m_bSN;
		}
	}

	// Bei MiCo- oder CoCo-Unit die entsprechende Ini-Datei bearbeiten
	CString sSMName = m_pOutputGroup->GetSMName();
	if ( sSMName == SM_MICO_OUTPUT_CAMERAS )
	{
		WriteMiCoIni(MICO1);
	}
	if ( sSMName == SM_MICO_OUTPUT_CAMERAS2 )
	{
		WriteMiCoIni(MICO2);
	}
	
	else if ( sSMName == SM_COCO_OUTPUT_CAM0
				|| sSMName == SM_COCO_OUTPUT_CAM1
				|| sSMName == SM_COCO_OUTPUT_CAM2
				|| sSMName == SM_COCO_OUTPUT_CAM3 )
	{
		WriteCoCoIsaIni();
	}

	m_pOutputList->Save(GetProfile(),IsLocal());

	bSN |= GetProfile().GetInt(WK_APP_NAME_COMMUNIT,_T("COM"),0);

	GetProfile().WriteInt(WK_APP_NAME_COMMUNIT,_T("Enabled"),bSN);
	GetProfile().WriteString(theApp.GetModuleSection(), WK_APP_NAME_COMMUNIT, bSN ? _T("CommUnit.exe") : _T(""));

	SetModified(FALSE,FALSE); 
}

/////////////////////////////////////////////////////////////////////////////
void COutputPage::WriteMiCoIni(int nMiCoNr)
{
	ExOutputType exType = EX_OUTPUT_OFF;
	int iCoViNr		= 0;
	int iCameraNr	= 0;
	CString sSection;
	CString sEntryY;
	CString sEntryC;

	COutput* pOutput = NULL;
	int i,c;

	c = m_pOutputGroup->GetSize();
	for (i=0;i<c;i++)
	{
		pOutput = m_pOutputGroup->GetOutput(i);
		if (pOutput)
		{
			// CoViNr und CameraNr anhand der Anschluesse ermitteln
			iCoViNr = i / NR_OUTPUTS_CAMERA_COVI;
			iCameraNr = i % NR_OUTPUTS_CAMERA_COVI;
			exType = pOutput->GetExOutputType();
			
			sSection.Format(szMiCoSectionFormat, nMiCoNr, iCoViNr, iCameraNr);
			GetProfile().WriteInt(sSection, szAVPortY, iCameraNr);
			if (exType == EX_OUTPUT_CAMERA_FBAS )
			{
				GetProfile().WriteString(sSection, szSourceType, szFBAS);
				GetProfile().WriteInt(sSection, szAVPortC, iCameraNr);
			}
			else if ( exType == EX_OUTPUT_CAMERA_SVHS ) 
			{
				GetProfile().WriteString(sSection, szSourceType, szSVHS);
				GetProfile().WriteInt(sSection, szAVPortC, iCameraNr+1);
			}
			else 
			{
				GetProfile().DeleteEntry(sSection, szSourceType);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void COutputPage::WriteCoCoIsaIni()
{
	ExOutputType exType = EX_OUTPUT_OFF;
	int iCoCoNr		= 0;
	int iCoViNr		= 0;
	int iCameraNr	= 0;

	CString sIniSection;
	CString sEntryY;
	CString sEntryC;
	TCHAR szFormat[] = _T("%u");

	// CoCoNr anhand des SM-Names ermitteln
	CString sSMName = m_pOutputGroup->GetSMName();
	if ( sSMName == SM_COCO_OUTPUT_CAM1 ) 
	{
		iCoCoNr = 1;
	}
	else if ( sSMName == SM_COCO_OUTPUT_CAM2 ) 
	{
		iCoCoNr = 2;
	}
	else if ( sSMName == SM_COCO_OUTPUT_CAM3 ) 
	{
		iCoCoNr = 3;
	}

	COutput* pOutput = NULL;
	int i,c;

	c = m_pOutputGroup->GetSize();
	for (i=0;i<c;i++)
	{
		pOutput = m_pOutputGroup->GetOutput(i);
		if (pOutput)
		{
			// CoViNr und CameraNr anhand der Anschluesse ermitteln
			iCoViNr = i / NR_OUTPUTS_CAMERA_COCO;
			iCameraNr = i % NR_OUTPUTS_CAMERA_COCO;
			sIniSection.Format(_T("Source%u-%u-%u"), iCoCoNr, iCoViNr, iCameraNr);

			exType = pOutput->GetExOutputType();
			if (exType == EX_OUTPUT_CAMERA_FBAS )
			{
				sEntryY.Format(szFormat, iCameraNr);
				sEntryC.Format(szFormat, iCameraNr);
				WritePrivateProfileString(sIniSection, szSourceType, szFBAS, 
					m_pParent->GetDocument()->GetCoCoIni());
				WritePrivateProfileString(sIniSection, szAVPortY, sEntryY, 
					m_pParent->GetDocument()->GetCoCoIni());
				WritePrivateProfileString(sIniSection, szAVPortC, sEntryC, 
					m_pParent->GetDocument()->GetCoCoIni());
			}
			else if ( exType == EX_OUTPUT_CAMERA_SVHS ) 
			{
				sEntryY.Format(szFormat, iCameraNr);
				sEntryC.Format(szFormat, iCameraNr+1);
				WritePrivateProfileString(sIniSection, szSourceType, szSVHS, 
					m_pParent->GetDocument()->GetCoCoIni());
				WritePrivateProfileString(sIniSection, szAVPortY, sEntryY, 
					m_pParent->GetDocument()->GetCoCoIni());
				WritePrivateProfileString(sIniSection, szAVPortC, sEntryC, 
					m_pParent->GetDocument()->GetCoCoIni());
			}
			else 
			{
				WritePrivateProfileString(sIniSection, szSourceType, NULL, 
					m_pParent->GetDocument()->GetCoCoIni());
				WritePrivateProfileString(sIniSection, szAVPortY, NULL, 
					m_pParent->GetDocument()->GetCoCoIni());
				WritePrivateProfileString(sIniSection, szAVPortC, NULL, 
					m_pParent->GetDocument()->GetCoCoIni());
			}
		}
	}
	// flush the ini file, because it's cached by Windows 95
	WritePrivateProfileString(NULL,NULL,NULL,m_pParent->GetDocument()->GetCoCoIni());
}
/////////////////////////////////////////////////////////////////////////////
void COutputPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COutputPage)
	DDX_Control(pDX, IDC_TXT_AT_DISCONNECT, m_staticAtDisconnect);
	DDX_Control(pDX, IDC_SCROLLBAR, m_ScrollBar);
	DDX_Control(pDX, IDC_TXT_OUTPUTTYP, m_staticOutputType);
	DDX_Control(pDX, IDC_TXT_OUTPUTNR, m_staticOutputNr);
	DDX_Control(pDX, IDC_TXT_OUTPUTCOMMENT, m_staticOutputComment);
	DDX_Control(pDX, IDC_TXT_OUTPUTSN, m_staticOutputSN);
	DDX_Control(pDX, IDC_TXT_MANU, m_staticManu);
	DDX_Control(pDX, IDC_TXT_COM, m_staticCom);
	DDX_Control(pDX, IDC_TXT_ID, m_staticID);
	DDX_Scroll(pDX, IDC_SCROLLBAR, m_iScroll);
	//}}AFX_DATA_MAP
//	DDX_Control(pDX, IDC_TXT_TERMINATION, m_staticTermination);
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(COutputPage, CSVPage)
	//{{AFX_MSG_MAP(COutputPage)
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TYPE_CAMERA, OnDeltaposSpinTypeCamera)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputPage message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL COutputPage::OnInitDialog() 
{
	CString str;
	GetDlgItemText(IDC_TXT_TERMINATION, str);
	CreateOutputDialogs();

	CSVPage::OnInitDialog();

	RemoveFromResize(IDC_TXT_OUTPUTNR);
    RemoveFromResize(IDC_TXT_OUTPUTTYP); // Workaround für Resizing

	if (!m_bAnySN)
	{
		m_staticOutputSN.ShowWindow(SW_HIDE);
		m_staticManu.ShowWindow(SW_HIDE);
		m_staticCom.ShowWindow(SW_HIDE);
		m_staticID.ShowWindow(SW_HIDE);
		m_staticAtDisconnect.ShowWindow(SW_HIDE);
	}

	if (m_pOutputGroup->GetSize()>MAX_OUTPUTS_ON_PAGE)
	{
		m_ScrollBar.SetScrollRange(0,m_pOutputGroup->GetSize()-1);
		SetPageSize(m_ScrollBar, MAX_OUTPUTS_ON_PAGE);
	}
	else
	{
		m_ScrollBar.ShowWindow(SW_HIDE);
	}

	SetModified(FALSE,FALSE);
	// Page muss einmal neugezeichnet werden, nicht immer kommt ein OnSize durch!
	Resize();

//	if (theApp.m_bProfessional)
	{
		CWK_Dialog*pDlg = GetSubDlg(0);
		CWnd *pWndChild   = pDlg->GetDlgItem(IDC_SPIN_TYPE_CAMERA);
		CWnd *pWndMaster  = GetDlgItem(IDC_SPIN_TYPE_CAMERA);
		if (pWndMaster && pWndChild)
		{
			RemoveFromResize(IDC_SPIN_TYPE_CAMERA);
			pWndMaster->SendMessage(UDM_SETRANGE, 0, (LPARAM)pWndChild->SendMessage(UDM_GETRANGE));
			pWndMaster->SendMessage(UDM_SETPOS, 0, (LPARAM)pWndChild->SendMessage(UDM_GETPOS));

			pWndMaster->ShowWindow(SW_SHOW);
		}
	}

	return TRUE;	// return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void COutputPage::CreateOutputDialogs()
{
	int i,c;
	COutput* pOutput;
	COutputDialog* pOutputDialog;

	c = m_pOutputGroup->GetSize();
	for (i=0;i<MAX_OUTPUTS_ON_PAGE && i<c;i++)
	{
		pOutput = m_pOutputGroup->GetOutput(i);
		pOutputDialog = new COutputDialog(this,pOutput);
		m_OutputDialogs.Add(pOutputDialog);
	}

	Resize();
}
/////////////////////////////////////////////////////////////////////////////
void COutputPage::Reset(BOOL bDirection)
{
	int i,c;
	COutput* pOutput;
	COutputDialog* pOutputDialog;

	c = m_OutputDialogs.GetSize();

	// Es wurde nach oben gescrollt -> Elemente aufsteigend verschieben
	if (bDirection)
	{
		for (i=0;i<MAX_OUTPUTS_ON_PAGE && i<c;i++)
		{
//			if (i+m_iScroll<m_pOutputGroup->GetSize())
			{
				pOutputDialog = m_OutputDialogs.GetAt(i);
				pOutputDialog->ControlToOutput();
				pOutput = m_pOutputGroup->GetOutput(i+m_iScroll);
				pOutputDialog->SetOutput(pOutput);
			}
		}
	}
	else // Es wurde nach unten gescrollt -> Elemente absteigend verschieben
	{
		for (i=MAX_OUTPUTS_ON_PAGE-1;i>=0;i--)
		{
//			if (i+m_iScroll<m_pOutputGroup->GetSize())
			{
				pOutputDialog = m_OutputDialogs.GetAt(i);
				pOutputDialog->ControlToOutput();
				pOutput = m_pOutputGroup->GetOutput(i+m_iScroll);
				pOutputDialog->SetOutput(pOutput);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void COutputPage::Resize()
{
	if (m_staticOutputNr.m_hWnd)
	{
		CRect rect;
		COutputDialog* pOutputDialog;
		int i,c,height,off;
		CRect scrollrect;

		c = m_OutputDialogs.GetSize();

		m_staticOutputNr.GetWindowRect(rect);
		ScreenToClient(rect);
		off = rect.bottom + 10;

		if (c>0)
		{
			pOutputDialog = m_OutputDialogs.GetAt(0);
			pOutputDialog->GetClientRect(rect);
			rect.top += off;
			rect.bottom += off;
			pOutputDialog->MoveWindow(rect);
			pOutputDialog->ShowWindow(SW_SHOW);
			height = rect.Height() + 5;
			scrollrect.left = rect.right + 5;
			scrollrect.right = scrollrect.left + GetSystemMetrics(SM_CXVSCROLL);
			scrollrect.top = rect.top;
			
			for (i=1 ; i<c ; i++)
			{
				pOutputDialog = m_OutputDialogs.GetAt(i);
				pOutputDialog->GetClientRect(rect);
				rect.top += height*i+off;
				rect.bottom += height*i+off;
				pOutputDialog->MoveWindow(rect);
				pOutputDialog->ShowWindow(SW_SHOW);
			}
			scrollrect.bottom = rect.bottom;
			m_ScrollBar.MoveWindow(scrollrect);
		}

		m_ScrollBar.ShowWindow((m_pOutputGroup->GetSize()>MAX_OUTPUTS_ON_PAGE) ? SW_SHOW : SW_HIDE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void COutputPage::CancelChanges() 
{
	m_pOutputList->Reset();
	m_pOutputList->Load(GetProfile());

	// attention the m_pOutputGroup is invalid
	// so get a correct one

	m_pOutputGroup = m_pOutputList->GetGroupByID(m_idGroup);

	COutputDialog* pOutputDialog;
	int i,c,d;

	c = m_OutputDialogs.GetSize();
	d = m_pOutputGroup->GetSize();
	for (i=0;i<c && i<d;i++)
	{
		pOutputDialog = m_OutputDialogs.GetAt(i);
		pOutputDialog->SetOutput(m_pOutputGroup->GetOutput(m_iScroll+i));
	}
}
/////////////////////////////////////////////////////////////////////////////
void COutputPage::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int oldScroll = m_iScroll;
	CSVPage::OnVScroll(m_ScrollBar, nSBCode, nPos, m_pOutputGroup->GetSize(), m_iScroll);

	if (oldScroll != m_iScroll)
	{
		UpdateData(FALSE);
		Reset(m_iScroll < oldScroll);
	}
	
	CWK_Dialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

/////////////////////////////////////////////////////////////////////////////
void COutputPage::OnSize(UINT nType, int cx, int cy) 
{
	CSVPage::OnSize(nType, cx, cy);
	
	Resize();
}

int COutputPage::GetNoOfSubDlgs()
{
	return m_OutputDialogs.GetSize();
}

CWK_Dialog *COutputPage::GetSubDlg(int nDlg)
{
	if ((nDlg >=0) && (nDlg < m_OutputDialogs.GetSize()))
	{
		return m_OutputDialogs.GetAt(nDlg);
	}
	return NULL;
}

void COutputPage::OnDeltaposSpinTypeCamera(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	int i, c = m_OutputDialogs.GetSize(),
		   d = m_pOutputGroup->GetSize(),
		nPos = ::SendMessage(pNMUpDown->hdr.hwndFrom, UDM_GETPOS, 0, 0);

	COutputDialog* pOutputDialog;
	if (c)
	{
		pOutputDialog = m_OutputDialogs.GetAt(0);
		for (i=0; i<m_iScroll; i++)
		{
			pOutputDialog->SetOutput(m_pOutputGroup->GetOutput(i));
			pOutputDialog->GetDlgItem(pNMUpDown->hdr.idFrom)->SendMessage(UDM_SETPOS, 0, nPos);
			pOutputDialog->ControlToOutput();
		}
		for (i=m_iScroll+c; i<d; i++)
		{
			pOutputDialog->SetOutput(m_pOutputGroup->GetOutput(i));
			pOutputDialog->GetDlgItem(pNMUpDown->hdr.idFrom)->SendMessage(UDM_SETPOS, 0, nPos);
			pOutputDialog->ControlToOutput();
		}

		pOutputDialog->SetOutput(m_pOutputGroup->GetOutput(m_iScroll));
	}

	for (i=0;i<c;i++)
	{
		pOutputDialog = m_OutputDialogs.GetAtFast(i);
		if (pOutputDialog)
		{
			pOutputDialog->GetDlgItem(pNMUpDown->hdr.idFrom)->SendMessage(UDM_SETPOS, 0, nPos);
		}
	}
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COutputPage::GetToolTip(UINT nID, CString&sText)
{
	if (nID == IDC_SPIN_TYPE_CAMERA)
	{
		return sText.LoadString(IDS_DE_ACTIVATE_ALL);
	}
	return FALSE;
}
