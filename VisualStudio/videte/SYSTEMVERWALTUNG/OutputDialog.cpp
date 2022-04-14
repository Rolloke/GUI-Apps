/* GlobalReplace: EX_OUTPUT_CAMERA_COLOR --> EX_OUTPUT_CAMERA_FBAS */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: OutputDialog.cpp $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/OutputDialog.cpp $
// CHECKIN:		$Date: 25.08.06 21:47 $
// VERSION:		$Revision: 68 $
// LAST CHANGE:	$Modtime: 25.08.06 21:47 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "globals.h"
#include "SystemVerwaltung.h"

#include "SVDoc.h"
#include "SVView.h"

#include "Output.h"
#include "OutputGroup.h"
#include "OutputList.h"

#include "Input.h"
#include "InputList.h"
#include "InputGroup.h"
#include "InputToOutput.h"

#include "OutputDialog.h"
#include "OutputPage.h"

#include "CameraControl.h"

#include "ComConfigurationDialog.h"
#include "PTZconfigDlg.h"
#include ".\outputdialog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static TCHAR szHTTP[] = _T("HTTP");
static TCHAR szAXISRS232[] = _T("AXIS RS232");
static TCHAR szAXISRS485[] = _T("AXIS RS485");
static TCHAR szQRS485[] = _T("Q RS485");

/////////////////////////////////////////////////////////////////////////////
// COutputDialog dialog

int COutputDialog::gm_nList[3] = {IMAGE_NULL, IMAGE_CAMERA_FBAS, IMAGE_CAMERA_SVHS};

COutputDialog::COutputDialog(COutputPage* pOutputPage, COutput* pOutput)
	: CWK_Dialog(COutputDialog::IDD)
{
	//{{AFX_DATA_INIT(COutputDialog)
	m_sNr = _T("1");
	m_sName = _T("");
	m_bSN = FALSE;
	m_sCom = _T("");
	m_bReference = FALSE;
	m_bTermination = TRUE;
	m_dwSNID = 0;
	//}}AFX_DATA_INIT
	m_pOutputPage	= pOutputPage;
	m_pOutput		= pOutput;
	m_iTypePos = CAMERA_OFF;
	m_eCameraType = CCT_UNKNOWN;
	m_eCameraCmd = CCC_INVALID;

	Create(IDD,pOutputPage);
}


void COutputDialog::DoDataExchange(CDataExchange* pDX)
{
	CWK_Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COutputDialog)
	DDX_Control(pDX, IDC_SN_AT_DISCONNECT, m_cbAtDisconnect);
	DDX_Control(pDX, IDC_STATIC_TYPE_CAMERA, m_StaticTypeCamera);
	DDX_Control(pDX, IDC_SPIN_TYPE_CAMERA, m_SpinTypeCamera);
	DDX_Control(pDX, IDC_CHECK_REFERENCE, m_checkReference);
	DDX_Control(pDX, IDC_CHECK_TERMINATION, m_checkTermination);
	DDX_Control(pDX, IDC_NAME_CAMERA, m_editName);
	DDX_Control(pDX, IDC_SN, m_checkSN);
	DDX_Control(pDX, IDC_EDIT_ID, m_editID);
	DDX_Control(pDX, IDC_MANUFACTURER, m_cbManu);
	DDX_Control(pDX, IDC_COM_CAMERA, m_cbCom);
	DDX_Text(pDX, IDC_NR_OUTPUT, m_sNr);
	DDX_Text(pDX, IDC_NAME_CAMERA, m_sName);
	DDX_Check(pDX, IDC_SN, m_bSN);
	DDX_CBString(pDX, IDC_COM_CAMERA, m_sCom);
	DDX_Check(pDX, IDC_CHECK_REFERENCE, m_bReference);
	DDX_Check(pDX, IDC_CHECK_TERMINATION, m_bTermination);
	DDX_Text(pDX, IDC_EDIT_ID, m_dwSNID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COutputDialog, CWK_Dialog)
	//{{AFX_MSG_MAP(COutputDialog)
	ON_BN_CLICKED(IDC_SN, OnSn)
	ON_CBN_SELCHANGE(IDC_COM_CAMERA, OnSelchangeCom)
	ON_EN_CHANGE(IDC_EDIT_ID, OnChangeEditId)
	ON_EN_CHANGE(IDC_NAME_CAMERA, OnChangeKommentar)
	ON_CBN_SELCHANGE(IDC_MANUFACTURER, OnSelchangeManufacturer)
	ON_BN_CLICKED(IDC_CHECK_REFERENCE, OnCheckReference)
	ON_BN_CLICKED(IDC_CHECK_TERMINATION, OnCheckTermination)
	ON_EN_CHANGE(IDC_EDIT_TYPE_CAMERA, OnChangeEditTypeCamera)
	ON_CBN_SELCHANGE(IDC_SN_AT_DISCONNECT, OnSelchangeSnAtDisconnect)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_CAM_COM_PARAMS, OnBnClickedBtnCamComParams)
	ON_BN_CLICKED(IDC_BTN_CAM_TYPE_SETTINGS, OnBnClickedBtnCamTypeSettings)
	ON_EN_SETFOCUS(IDC_NAME_CAMERA, OnEnSetfocusNameCamera)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputDialog message handlers

void COutputDialog::PostNcDestroy() 
{
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
void COutputDialog::OnOK()
{
}
/////////////////////////////////////////////////////////////////////////////
void COutputDialog::OnCancel()
{
}
/////////////////////////////////////////////////////////////////////////////
void COutputDialog::ShowHide()
{
	int nPos = m_SpinTypeCamera.GetPos();
	BOOL bOn = (nPos != CAMERA_OFF) ? TRUE : FALSE;

	m_editName.EnableWindow(bOn);
	m_checkReference.EnableWindow(bOn);
	
	m_checkTermination.EnableWindow(bOn && m_pOutputPage->m_bShowTermination);

	if (m_pOutputPage->m_bAnySN)
	{
		m_checkSN.EnableWindow((nPos != CAMERA_OFF) ? TRUE : FALSE);
		m_cbManu.EnableWindow(m_bSN);
		if (m_bSN)
		{
			m_cbCom.EnableWindow((m_eCameraType==CCT_RELAY_PTZ) ? FALSE:TRUE);
			m_editID.EnableWindow(IsCameraIDRequired(m_eCameraType));
			m_cbAtDisconnect.EnableWindow(m_cbAtDisconnect.GetCount() > 1);
			BOOL bEnable = m_cbCom.GetCurSel() != CB_ERR && m_eCameraType != CCT_UNKNOWN ? TRUE : FALSE;
			GetDlgItem(IDC_BTN_CAM_COM_PARAMS)->EnableWindow(bEnable);
			GetDlgItem(IDC_BTN_CAM_TYPE_SETTINGS)->EnableWindow(bEnable);
		}
		else
		{
			m_cbCom.EnableWindow(FALSE);
			m_editID.EnableWindow(FALSE);
			m_cbAtDisconnect.EnableWindow(FALSE);
			GetDlgItem(IDC_BTN_CAM_COM_PARAMS)->EnableWindow(FALSE);
			GetDlgItem(IDC_BTN_CAM_TYPE_SETTINGS)->EnableWindow(FALSE);
			m_eCameraCmd = CCC_INVALID;
		}
	}
	else
	{
		m_checkSN.ShowWindow(SW_HIDE);
		m_checkSN.EnableWindow(FALSE);
		m_editID.ShowWindow(SW_HIDE);
		m_editID.EnableWindow(FALSE);
		m_cbCom.ShowWindow(SW_HIDE);
		m_cbCom.EnableWindow(FALSE);
		m_cbManu.ShowWindow(SW_HIDE);
		m_cbManu.EnableWindow(FALSE);
		m_cbAtDisconnect.ShowWindow(SW_HIDE);
		m_cbAtDisconnect.EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_CAM_COM_PARAMS)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BTN_CAM_TYPE_SETTINGS)->ShowWindow(SW_HIDE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void COutputDialog::InitCOM()
{
	int i;
	DWORD dwComMask;
	DWORD dwBit = 1;
	CString sFill;
	
	CString sDevice, sPTZdevice;
	BOOL bCanBeUsed;
	if (m_bSN)
	{
		sPTZdevice.Format(_T("PTZ_%s"), NameOfEnum(m_eCameraType));
	}
	else
	{
		sPTZdevice.Format(_T("PTZ_%s"), NameOfEnum(CCT_UNKNOWN));
	}
	m_cbCom.ResetContent();
	dwComMask = m_pOutputPage->m_pParent->GetDocument()->GetCommPortInfo();
	for (dwBit = 1,i=1; dwBit && dwBit<=dwComMask; dwBit<<=1,i++)
	{
		bCanBeUsed = TRUE;
		sDevice = IsCommPortUsed(m_pOutputPage->GetProfile(), i);
		if (!sDevice.IsEmpty() && sDevice != sPTZdevice)
		{
			bCanBeUsed = FALSE;
		}
		if (dwBit & dwComMask && bCanBeUsed)
		{
			sFill.Format(_T("COM%d"),i);
			m_cbCom.AddString(sFill);
		}
	}
	if (m_pOutputPage->m_pOutputGroup->GetSMName()==SM_IP_CAMERA_OUTPUT_CAMERA)
	{
		m_cbCom.AddString(szHTTP);
	}

	/* NOT YET
	m_cbCom.AddString(szAXISRS232);
	m_cbCom.AddString(szAXISRS485);
	m_cbCom.AddString(szQRS485);
	*/


	if (!m_sCom.IsEmpty() && m_cbCom.GetCount())
	{
		m_cbCom.SelectString(0, m_sCom);
	}
}	
/////////////////////////////////////////////////////////////////////////////
void COutputDialog::InitSN()
{
	CameraControlType cType;
	int *pnType = (int*)&cType,
		nLast	= GetLastCameraControlType(TRUE),
		iIndex;
	cType = CCT_UNKNOWN;
	for ((*pnType)++; (*pnType)<=nLast; (*pnType)++)
	{
		iIndex = m_cbManu.AddString(NameOfEnumPTZRealName(cType));
		if (iIndex != LB_ERR)
		{
			m_cbManu.SetItemData(iIndex, cType);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL COutputDialog::StretchElements()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COutputDialog::OnInitDialog() 
{
	CWK_Dialog::OnInitDialog();
	if (CSkinDialog::SetChildWindowFont(m_hWnd))
	{
		if (m_ToolTip.m_hWnd)
		{
			m_ToolTip.SetFont(CSkinDialog::GetDialogItemGlobalFont(), 0);
		}
	}

	RemoveFromResize(IDC_SPIN_TYPE_CAMERA);    // Workaround für das Resizen
	RemoveFromResize(IDC_EDIT_TYPE_CAMERA);  // Workaround für das Resizen
	RemoveFromResize(IDC_STATIC_TYPE_CAMERA);  // Workaround für das Resizen
	RemoveFromResize(IDC_NR_OUTPUT);     // Workaround für das Resizen

	m_StaticTypeCamera.ModifyStyle(0, SS_ICON, 0);

	InitSN();
	OutputToControl();
	
	InitCOM();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void COutputDialog::SetOutput(COutput* pOutput)
{
	m_pOutput = pOutput;
	OutputToControl();
}
/////////////////////////////////////////////////////////////////////////////
void COutputDialog::OutputToControl()
{
	m_sNr.Format(_T("%d"),m_pOutput->GetID().GetSubID()+1);
	int iNr = _ttoi(m_sNr);
	if (iNr & 1)
	{
		// ungerade
		// FBAS und VHS
		m_SpinTypeCamera.SetRange32(CAMERA_SVHS, CAMERA_OFF);
	}
	else
	{
		// gerade kein VHS
		// nur FBAS
		m_SpinTypeCamera.SetRange32(CAMERA_FBAS, CAMERA_OFF);
	}
	if (!m_pOutputPage->m_bShowVHS)
	{
		m_SpinTypeCamera.SetRange32(CAMERA_FBAS, CAMERA_OFF);
	}

	if (m_pOutput->IsSTM())
	{
		// hide everything
//		m_Type.ShowWindow(SW_HIDE);
		m_StaticTypeCamera.ShowWindow(SW_HIDE);
		m_SpinTypeCamera.ShowWindow(SW_HIDE);
		m_editName.ShowWindow(SW_HIDE);
		m_checkSN.ShowWindow(SW_HIDE);
		m_cbManu.ShowWindow(SW_HIDE);
		m_cbCom.ShowWindow(SW_HIDE);
		m_editID.ShowWindow(SW_HIDE);
		m_cbAtDisconnect.ShowWindow(SW_HIDE);
		UpdateData(FALSE);
	}
	else
	{
		m_sName  = m_pOutput->GetName();
		m_iTypePos = CAMERA_OFF;
		switch (m_pOutput->GetExOutputType())
		{
		   case EX_OUTPUT_CAMERA_FBAS: m_iTypePos = CAMERA_FBAS; break;
		   case EX_OUTPUT_CAMERA_SVHS: m_iTypePos = CAMERA_SVHS; break;
		}

		m_SpinTypeCamera.SetPos(m_iTypePos);
		HICON hIcon = ::ImageList_GetIcon(HIMAGELIST(*((CSVApp*)AfxGetApp())->GetSmallImage()), gm_nList[m_iTypePos], ILD_TRANSPARENT);
		m_StaticTypeCamera.SetIcon(hIcon);

		m_bSN = m_pOutput->GetSN();
		m_bReference = m_pOutput->IsReference();
		m_bTermination = m_pOutput->IsTerminated();

		if (m_bSN)
		{
			int iPTZ = m_pOutput->GetPTZInterface();
			if (iPTZ<33)
			{
				m_sCom.Format(_T("COM%d"),m_pOutput->GetPTZInterface());
			}
			else
			{
				switch (iPTZ) 
				{
				case PTZ_INTERFACE_HTTP:
					m_sCom = szHTTP;
					break;
				case PTZ_INTERFACE_AXIS_RS232:
					m_sCom = szAXISRS232;
					break;
				case PTZ_INTERFACE_AXIS_RS485:
					m_sCom = szAXISRS485;
					break;
				case PTZ_INTERFACE_UDP_RS485:
					m_sCom = szQRS485;
					break;
				}
			}
			m_eCameraType = m_pOutput->GetCameraControlType();
			for (int i=0 ; i<m_cbManu.GetCount() ; i++)
			{
				if (m_eCameraType == (CameraControlType)m_cbManu.GetItemData(i))
				{
					m_cbManu.SetCurSel(i);
				}
			}
			InitCmdAtDisconnect();
			m_dwSNID = m_pOutput->GetSnId();
			m_eCameraCmd = m_pOutput->GetCameraControlCmd();
			for (i=0 ; i<m_cbAtDisconnect.GetCount() ; i++)
			{
				if (m_eCameraCmd == (CameraControlCmd)m_cbAtDisconnect.GetItemData(i))
				{
					m_cbAtDisconnect.SetCurSel(i);
				}
			}
		}
		UpdateData(FALSE);
		ShowHide();
	}
}
int ComStringToInt(const CString& sCom)
{
	int iCom = 0;
	if (1==_stscanf(sCom,_T("COM%d"),&iCom))
	{
	}
	else
	{
		if (sCom == szHTTP)
		{
			iCom = PTZ_INTERFACE_HTTP;
		}
		else if (sCom == szAXISRS232)
		{
			iCom = PTZ_INTERFACE_AXIS_RS232;
		}
		else if (sCom == szAXISRS485)
		{
			iCom = PTZ_INTERFACE_AXIS_RS485;
		}
		else if (sCom == szQRS485)
		{
			iCom = PTZ_INTERFACE_UDP_RS485;
		}
	}
	return iCom;
}
/////////////////////////////////////////////////////////////////////////////
void COutputDialog::ControlToOutput()
{
	UpdateData();
	int nPos = m_SpinTypeCamera.GetPos();

	m_pOutput->SetName(m_sName);
	if (nPos == CAMERA_OFF)
	{
		m_pOutput->SetExOutputType(EX_OUTPUT_OFF);
		m_pOutput->SetReference(FALSE);
		m_pOutput->SetSN(FALSE);
	}
	else
	{
		if ((nPos == CAMERA_SVHS) || (nPos == CAMERA_FBAS))
		{
			m_pOutput->SetExOutputType((nPos == CAMERA_SVHS) ? EX_OUTPUT_CAMERA_SVHS : EX_OUTPUT_CAMERA_FBAS);
			m_pOutput->SetReference(m_bReference);
			m_pOutput->SetTermination(m_bTermination);
			m_pOutput->SetSN(m_bSN);
			if (m_bSN)
			{
				int iCom = 0;
				// m_sCom may be empty -> results in invalid value for iCom
				if (m_sCom.IsEmpty() == FALSE)
				{
					iCom = ComStringToInt(m_sCom);
				}
				m_pOutput->SetPTZInterface(iCom);
				m_pOutput->SetCameraControlType(m_eCameraType);
				m_pOutput->SetSnId(m_dwSNID);
				m_pOutput->SetCameraControlCmd(m_eCameraCmd);
				CString sDevice = _T("PTZ_");
				sDevice += NameOfEnum(m_eCameraType);
				SetCommPortUsed(m_pOutputPage->GetProfile(), iCom, sDevice);
			}
			else
			{
				m_pOutput->SetSN(FALSE);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void COutputDialog::OnSn() 
{
	UpdateData();
	ShowHide();
	m_pOutputPage->SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void COutputDialog::OnChangeEditTypeCamera() 
{
	if (m_SpinTypeCamera.m_hWnd == NULL) return;
	int nPos = m_SpinTypeCamera.GetPos();
	if (nPos>=0 && nPos < 3)
	{
		HICON hIcon = ::ImageList_GetIcon(HIMAGELIST(*((CSVApp*)AfxGetApp())->GetSmallImage()), gm_nList[nPos], ILD_TRANSPARENT);
		m_StaticTypeCamera.SetIcon(hIcon);
		if (nPos == CAMERA_OFF)
		{
			if (!m_pOutputPage->CheckActivationsWithOutput(m_pOutput->GetID(), 0, SVP_CO_CHECK_ONLY))
			{
				if (IDYES == AfxMessageBox(IDP_DELETE_OUTPUT_WITH_ACTIVATION, MB_YESNO))
				{
					if (m_pOutput)
					{
						m_pOutput->SetDeleteActivation();
					}
				}
				else
				{
					m_bSN = FALSE;
					UpdateData(FALSE);
				}
			}
		}
		ShowHide();
		if (nPos != m_iTypePos)
		{
			m_pOutputPage->SetModified();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void COutputDialog::OnSelchangeCom() 
{
	m_pOutputPage->SetModified();
	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void COutputDialog::OnChangeEditCloseRelay() 
{
	m_pOutputPage->SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void COutputDialog::OnChangeEditId() 
{
	UpdateData();
//  m_sSNID changed to m_dwSNID
/*	CString sID = m_sSNID;
	int iCurSel = CheckDigits(sID);
	if (iCurSel>=0)
	{
		m_sSNID = sID;
		UpdateData(FALSE);
		m_editID.SetSel(iCurSel,iCurSel);
	}
*/
	m_pOutputPage->SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void COutputDialog::OnChangeKommentar() 
{
	// TODO! RKE: Implement CSkinEdit::SetForbiddenCharacters(CString s);
	m_editName.GetWindowText(m_sName);
	int nLength = m_sName.GetLength();
	if (nLength && m_sName.GetAt(nLength-1) == _T('\\'))
	{
		m_sName.SetAt(nLength-1, 0);
		m_editName.SetWindowText(m_sName);
	}
	m_pOutputPage->SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void COutputDialog::OnSelchangeManufacturer() 
{
	int iSel = m_cbManu.GetCurSel();
	if (iSel == LB_ERR)
	{
		m_eCameraType = CCT_UNKNOWN;
		m_pOutput->SetSN(FALSE);
	}
	else
	{
		m_eCameraType = (CameraControlType)m_cbManu.GetItemData(iSel);
		m_pOutput->SetSN(TRUE);
		m_pOutput->SetCameraControlType(m_eCameraType);
		InitCmdAtDisconnect();
	}
	m_pOutputPage->CheckComPorts();
	InitCOM();
	m_pOutputPage->SetModified();
	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
void COutputDialog::OnCheckReference() 
{
	m_pOutputPage->SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void COutputDialog::OnCheckTermination() 
{
	m_pOutputPage->SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void COutputDialog::InitCmdAtDisconnect()
{
	m_eCameraCmd = CCC_INVALID;
	m_cbAtDisconnect.ResetContent();

	CString sEntry;
	sEntry.LoadString(IDS_NOTHING);
	int iIndex = m_cbAtDisconnect.AddString(sEntry);
	m_cbAtDisconnect.SetItemData(iIndex, CCC_INVALID);
	m_cbAtDisconnect.SetCurSel(iIndex);

	DWORD dwFunctions = GetCameraControlFunctions(m_eCameraType);
	if ((dwFunctions & PTZF_POS_HOME) == PTZF_POS_HOME)
	{
		iIndex = m_cbAtDisconnect.AddString(_T("Position Home"));
		m_cbAtDisconnect.SetItemData(iIndex, CCC_POS_HOME);
	}
	if ((dwFunctions & PTZF_POS_1) == PTZF_POS_1)
	{
		iIndex = m_cbAtDisconnect.AddString(_T("Position 1"));
		m_cbAtDisconnect.SetItemData(iIndex, CCC_POS_1);
	}
	if ((dwFunctions & PTZF_POS_2) == PTZF_POS_2)
	{
		iIndex = m_cbAtDisconnect.AddString(_T("Position 2"));
		m_cbAtDisconnect.SetItemData(iIndex, CCC_POS_2);
	}
	if ((dwFunctions & PTZF_POS_3) == PTZF_POS_3)
	{
		iIndex = m_cbAtDisconnect.AddString(_T("Position 3"));
		m_cbAtDisconnect.SetItemData(iIndex, CCC_POS_3);
	}
	if ((dwFunctions & PTZF_POS_4) == PTZF_POS_4)
	{
		iIndex = m_cbAtDisconnect.AddString(_T("Position 4"));
		m_cbAtDisconnect.SetItemData(iIndex, CCC_POS_4);
	}
	if ((dwFunctions & PTZF_POS_5) == PTZF_POS_5)
	{
		iIndex = m_cbAtDisconnect.AddString(_T("Position 5"));
		m_cbAtDisconnect.SetItemData(iIndex, CCC_POS_5);
	}
	if ((dwFunctions & PTZF_POS_6) == PTZF_POS_6)
	{
		iIndex = m_cbAtDisconnect.AddString(_T("Position 6"));
		m_cbAtDisconnect.SetItemData(iIndex, CCC_POS_6);
	}
	if ((dwFunctions & PTZF_POS_7) == PTZF_POS_7)
	{
		iIndex = m_cbAtDisconnect.AddString(_T("Position 7"));
		m_cbAtDisconnect.SetItemData(iIndex, CCC_POS_7);
	}
	if ((dwFunctions & PTZF_POS_8) == PTZF_POS_8)
	{
		iIndex = m_cbAtDisconnect.AddString(_T("Position 8"));
		m_cbAtDisconnect.SetItemData(iIndex, CCC_POS_8);
	}
	if ((dwFunctions & PTZF_POS_9) == PTZF_POS_9)
	{
		iIndex = m_cbAtDisconnect.AddString(_T("Position 9"));
		m_cbAtDisconnect.SetItemData(iIndex, CCC_POS_9);
	}
}
/////////////////////////////////////////////////////////////////////////////
void COutputDialog::OnSelchangeSnAtDisconnect() 
{
	int iIndex = m_cbAtDisconnect.GetCurSel();
	m_eCameraCmd = (CameraControlCmd)m_cbAtDisconnect.GetItemData(iIndex);
	m_pOutputPage->SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void COutputDialog::OnBnClickedBtnCamComParams()
{
	int nSelCom = m_cbCom.GetCurSel();
	if (nSelCom != CB_ERR && m_eCameraType != CCT_UNKNOWN)
	{
		CString sSection;
		m_cbCom.GetLBText(nSelCom, m_sCom);
		int nCom = _ttoi(m_sCom.Mid(3));
		sSection.Format(_T("CommUnit\\COM%03d\\%s"), nCom, NameOfEnum(m_eCameraType));

		CComParameters comParams(sSection);
		comParams.LoadFromRegistry(m_pOutputPage->GetProfile());
		CComConfigurationDialog dlg(m_sCom, &comParams, this);
		dlg.m_bShowDefaultBtn = TRUE;
		INT_PTR nResult = dlg.DoModal();
		if (nResult == IDOK)
		{
			comParams.SaveToRegistry(m_pOutputPage->GetProfile());
			m_pOutputPage->SetModified();
		}
		else if (nResult == IDC_BTN_COM_DEFAULT)
		{
			m_pOutputPage->GetProfile().DeleteSection(sSection);
			m_pOutputPage->SetModified(0, 1);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void COutputDialog::OnBnClickedBtnCamTypeSettings()
{
	int nSelCom = m_cbCom.GetCurSel();
	if (nSelCom != CB_ERR && m_eCameraType != CCT_UNKNOWN)
	{
		CString sSection;
		m_cbCom.GetLBText(nSelCom, m_sCom);
		CPTZconfigDlg dlg(this);
		dlg.m_nCom = _ttoi(m_sCom.Mid(3));
		CWK_Profile &wkp = m_pOutputPage->GetProfile();
		dlg.m_pWKP = &wkp;
		dlg.m_CameraType = m_eCameraType;
		dlg.m_sSection.Format(_T("CommUnit\\COM%03d\\%s"), dlg.m_nCom, NameOfEnum(m_eCameraType));

		INT_PTR nResult = dlg.DoModal();
		if (nResult == IDOK)
		{
			m_pOutputPage->SetModified();
		}
	}
}
////////////////////////////////////////////////////////////////////
void COutputDialog::OnEnSetfocusNameCamera()
{
	m_pOutputPage->m_idSelectedCamera = m_pOutput->GetID();
}
