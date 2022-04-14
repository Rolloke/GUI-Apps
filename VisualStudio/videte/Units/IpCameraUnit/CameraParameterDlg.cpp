/////////////////////////////////////////////////////////////////////////////
// PROJECT:		IPCameraUnit
// FILE:		$Workfile: CameraParameterDlg.cpp $
// ARCHIVE:		$Archive: /Project/Units/IPCameraUnit/CameraParameterDlg.cpp $
// CHECKIN:		$Date: 23.08.06 15:04 $
// VERSION:		$Revision: 22 $
// LAST CHANGE:	$Modtime: 23.08.06 14:07 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
// This is a test dialog to implement functionlities that are normally
// implemented in the SystemVerwaltung.
// This class may be removed later.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IPCameraUnit.h"
#include "CameraParameterDlg.h"

#include "CIPCOutputIPcamUnit.h"
#include ".\cameraparameterdlg.h"

#ifdef _TEST_SV_DLG
/////////////////////////////////////////////////////////////////////////////
// CCameraParameterDlg dialog
IMPLEMENT_DYNAMIC(CCameraParameterDlg, CDialog)
/////////////////////////////////////////////////////////////////////////////
CCameraParameterDlg::CCameraParameterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCameraParameterDlg::IDD, pParent)
{
	m_pOutputControl = NULL;
	m_uPort = 80; // HTTP
	m_nCurrentCaptureDevice = CB_ERR;
}
/////////////////////////////////////////////////////////////////////////////
CCameraParameterDlg::~CCameraParameterDlg()
{
	WK_DELETE(m_pOutputControl);
}
/////////////////////////////////////////////////////////////////////////////
void DDV_ComboSel(CDataExchange* pDX, CComboBox&combo)
{
	if (pDX->m_bSaveAndValidate)
	{
		if (combo.IsWindowEnabled() && combo.GetCurSel() == CB_ERR)
		{
			pDX->m_idLastControl = combo.GetDlgCtrlID();
			CString s, sCtrl;
			CWnd *pWnd = combo.GetWindow(GW_HWNDPREV);
			while (pWnd)
			{
				pWnd->GetWindowText(sCtrl);
				if (sCtrl.IsEmpty())
				{
					pWnd = pWnd->GetWindow(GW_HWNDPREV);
				}
				else
				{
					break;
				}
			}
			s.Format(_T("Bitte %s auswählen"), sCtrl);
			AfxMessageBox(s, MB_OK|MB_ICONERROR);
			pDX->Fail();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void DDV_MinText(CDataExchange* pDX, CString &sText)
{
	if (pDX->m_bSaveAndValidate)
	{
		CWnd *pWnd = pDX->m_pDlgWnd->GetDlgItem(pDX->m_idLastControl);
		if (   sText.GetLength() == 0
			&& pWnd
			&& pWnd->IsWindowEnabled())
		{
			CString s, sCtrl;
			CWnd *pWndPrev = pWnd->GetWindow(GW_HWNDPREV);
			if (pWndPrev)
			{
				pWndPrev->GetWindowText(sCtrl);
			}
			s.Format(_T("Bitte %s eingeben"), sCtrl);
			AfxMessageBox(s, MB_OK|MB_ICONERROR);
			pDX->Fail();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_cTypes);
	DDV_ComboSel(pDX, m_cTypes);
	DDX_Control(pDX, IDC_COMBO_CAPTURE_DEVICES, m_cCaptureDevices);
	DDV_ComboSel(pDX, m_cCaptureDevices);
	DDX_Control(pDX, IDC_COMBO_RESOLUTIONS, m_cResolutions);
	DDV_ComboSel(pDX, m_cResolutions);
	DDX_Control(pDX, IDC_COMBO_ROTATION, m_cRotations);
	DDV_ComboSel(pDX, m_cRotations);
	DDX_Control(pDX, IDC_COMBO_COMPRESSION, m_cCompression);
	DDV_ComboSel(pDX, m_cCompression);
	DDX_Control(pDX, IDC_COMBO_URL, m_cURL);
	DDX_Text(pDX, IDC_EDT_PORT, m_uPort);
	DDV_MinMaxUInt(pDX, m_uPort, 1, INTERNET_MAX_PORT_NUMBER_VALUE);

	if (pDX->m_bSaveAndValidate)
	{
		pDX->m_idLastControl = IDC_COMBO_URL;
		int nSel = m_cURL.GetCurSel();
		if (nSel != CB_ERR)
		{
			m_cURL.GetLBText(nSel, m_sUrl);
		}
		else
		{
			m_cURL.GetWindowText(m_sUrl);
		}
		DDV_MinText(pDX, m_sUrl);
	}
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CCameraParameterDlg, CDialog)
	//{{AFX_MSG_MAP(CCameraParameterDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, OnCbnSelchangeComboType)
	ON_CBN_SELCHANGE(IDC_COMBO_CAPTURE_DEVICES, OnCbnSelchangeComboCaptureDevices)
	ON_CBN_EDITCHANGE(IDC_COMBO_CAPTURE_DEVICES, OnCbnEditchangeComboCaptureDevices)
	ON_BN_CLICKED(IDC_BTN_SEND, OnBnClickedBtnSend)
	ON_BN_CLICKED(IDC_BTN_SET_NAME, OnBnClickedBtnSetName)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_USER, OnUser)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDT_PORT, OnEnChangeEdtPortOrUrl)
	ON_EN_CHANGE(IDC_COMBO_URL, OnEnChangeEdtPortOrUrl)
	ON_CBN_SELCHANGE(IDC_COMBO_URL, OnCbnSelchangeComboUrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterDlg::InitCaptureDeviceList()
{
	CSecID id(m_pOutputControl->GetGroupID(), CSV_CONTROL_ID);
	m_pOutputControl->DoRequestSetValue(id, CSD_CAM_TYPE, CSD_CAM_CAPTURE, 0);
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterDlg::InitStrings(const CString&saCsv, CComboBox&combo)
{
	CStringArray sa;
	SplitString(saCsv, sa, _T(","));
	int i, n = (int)sa.GetCount();

	combo.EnableWindow(n>0);
	combo.ResetContent();
	for (i=0; i<n; i++)
	{
		combo.AddString(sa[i]);
	}
	if (n)
	{
		combo.SetCurSel(0);
	}
}
/////////////////////////////////////////////////////////////////////////////
// CCameraParameterDlg message handlers
// TODO! RKE: remove defines, only for test
#ifdef _TEST_TASHA
#undef SM_IP_CAMERA_OUTPUT_CAMERA
#define SM_IP_CAMERA_OUTPUT_CAMERA SM_TASHA_OUTPUT_CAMERAS
#endif
BOOL CCameraParameterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pOutputControl = new CIPCOutputIPcamUnit(NULL, SM_IP_CAMERA_OUTPUT_CAMERA _T("Control"), TRUE);
#ifdef _TEST_SV_DLG
	m_pOutputControl->SetParamDlg(this);
#endif
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterDlg::OnCbnSelchangeComboType()
{
	int nSel = m_cTypes.GetCurSel();
	if (nSel != CB_ERR)
	{
		CSecID id(m_pOutputControl->GetGroupID(), CSV_CONTROL_ID);
		m_pOutputControl->DoRequestSetValue(id, CSD_CAM_TYPE, CSD_OFF, 0);
		int nShowCapture = SW_HIDE;
		BOOL bEnalbeNoCapt  = FALSE;

		if (m_saTypes[nSel] == CSD_CAM_CAPTURE)
		{
			InitCaptureDeviceList();
			nShowCapture = SW_SHOW;
		}
		else
		{
			CSecID id(m_pOutputControl->GetGroupID(), CSV_CONTROL_ID);
			m_pOutputControl->DoRequestGetValue(id, m_saTypes[nSel], 0);
			bEnalbeNoCapt = TRUE;
		}
		
		m_cCaptureDevices.ShowWindow(nShowCapture);
		GetDlgItem(IDC_TXT_DEVICE)->ShowWindow(nShowCapture);
		GetDlgItem(IDC_BTN_SET_NAME)->ShowWindow(nShowCapture);

		GetDlgItem(IDC_TXT_LOCATION)->EnableWindow(bEnalbeNoCapt);
		GetDlgItem(IDC_COMBO_URL)->EnableWindow(bEnalbeNoCapt);
		GetDlgItem(IDC_EDT_PORT)->EnableWindow(bEnalbeNoCapt);

		// empty Controls
		CString sValue;
		m_cCompression.ResetContent();
		m_cResolutions.ResetContent();
		m_cRotations.ResetContent();
		m_cCaptureDevices.ResetContent();

		m_cCompression.EnableWindow(FALSE);
		m_cResolutions.EnableWindow(FALSE);
		m_cRotations.EnableWindow(FALSE);
		m_cCaptureDevices.EnableWindow(FALSE);

		for (int i=0; i<8; i++)
		{
			GetDlgItem(IDC_CK_CAMERA1+i)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_CK_OUTPUT1+i)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_CK_INPUT1+i)->ShowWindow(SW_HIDE);
		}
		SetDlgItemText(IDC_TXT_FRAMERATES, sValue);
		SetDlgItemText(IDC_EDT_NAME, sValue);
		m_nCurrentCaptureDevice = CB_ERR;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterDlg::OnCbnSelchangeComboUrl()
{
	int nSel = m_cURL.GetCurSel();
	if (nSel != CB_ERR)
	{
		SetDlgItemText(IDC_EDT_NAME, m_saUrls[nSel]);
		GetDlgItem(IDC_BTN_SEND)->EnableWindow(TRUE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterDlg::OnCbnSelchangeComboCaptureDevices()
{
	int nSel = m_cCaptureDevices.GetCurSel();
	if (nSel != CB_ERR)
	{
		CSecID id(m_pOutputControl->GetGroupID(), CSV_CONTROL_ID);
		CString sCam;
		m_cCaptureDevices.GetLBText(nSel, sCam);
		m_pOutputControl->DoRequestSetValue(id, CSD_CAM_CAPTURE, sCam, 0);
		m_nCurrentCaptureDevice = nSel;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterDlg::OnCbnEditchangeComboCaptureDevices()
{
	GetDlgItem(IDC_BTN_SET_NAME)->EnableWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterDlg::OnBnClickedBtnSetName()
{
	if (m_nCurrentCaptureDevice != CB_ERR)
	{
		GetDlgItem(IDC_BTN_SET_NAME)->EnableWindow(FALSE);

		CString sOldName, sNewName;
		m_cCaptureDevices.GetLBText(m_nCurrentCaptureDevice, sOldName);
		m_cCaptureDevices.GetWindowText(sNewName);
		if (sOldName != sNewName)
		{
			CSecID id(m_pOutputControl->GetGroupID(), CSV_CONTROL_ID);
			m_pOutputControl->DoRequestSetValue(id, CSD_NAME, sOldName+_T(",")+sNewName, 0);
		}
	}
}
void CCameraParameterDlg::OnEnChangeEdtPortOrUrl()
{
	GetDlgItem(IDC_BTN_SEND)->EnableWindow(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterDlg::OnBnClickedBtnSend()
{
	if (UpdateData())
	{
		int nSel = m_cTypes.GetCurSel();
		CSecID id(m_pOutputControl->GetGroupID(), CSV_CONTROL_ID);
		m_pOutputControl->DoRequestSetValue(id, CSD_CAM_TYPE, CSD_OFF, 0);
		CString sValue;
		sValue.Format(_T("%s:%s"), m_saTypes[nSel], m_sUrl);
		m_pOutputControl->DoRequestSetValue(id, CSD_CAM_TYPE, sValue, m_uPort);
		GetDlgItem(IDC_BTN_SEND)->EnableWindow(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterDlg::OnConfirmGetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwUserData)
{
	if (sKey == CSD_CAM_TYPE)
	{
		CString sType;
		m_saTypes.RemoveAll();
		SplitString(sValue, m_saTypes, _T(","));
		int i, nFind, n = (int)m_saTypes.GetCount();
		m_cTypes.ResetContent();
		for (i=0; i<n; i++)
		{
			nFind = m_saTypes[i].Find(_T(":"));
			if (nFind != -1)
			{
				m_cTypes.AddString(m_saTypes[i].Left(nFind));
				m_saTypes[i] = m_saTypes[i].Mid(nFind+1);
			}
		}
	}
	else if (sKey == CSD_COMPRESSIONS)
	{
		InitStrings(sValue, m_cCompression);
	}
	else if (sKey == CSD_RESOLUTIONS)
	{
		InitStrings(sValue, m_cResolutions);
	}
	else if (sKey == CSD_ROTATIONS)
	{
		InitStrings(sValue, m_cRotations);
	}
	else if (sKey == CSD_OUTPUTS)
	{
		int n = min(_ttol(sValue), 8);
		for (int i=0; i<n; i++)
		{
			GetDlgItem(IDC_CK_CAMERA1+i)->ShowWindow(SW_SHOW);
		}
	}
	else if (sKey == CSD_INPUTS)
	{
		int n = min(_ttol(sValue), 8);
		for (int i=0; i<n; i++)
		{
			GetDlgItem(IDC_CK_INPUT1+i)->ShowWindow(SW_SHOW);
		}
	}
	else if (sKey == CSD_RELAIS)
	{
		int n = min(_ttol(sValue), 8);
		for (int i=0; i<n; i++)
		{
			GetDlgItem(IDC_CK_OUTPUT1+i)->ShowWindow(SW_SHOW);
		}
	}
	else if (sKey == CSD_NAME)
	{
		SetDlgItemText(IDC_EDT_NAME, sValue);
	}
	else if (sKey == CSD_FRAMERATES)
	{
		SetDlgItemText(IDC_TXT_FRAMERATES, sValue);
	}
	else if (sKey == CSD_CAM_CAPTURE)
	{
		InitStrings(sValue, m_cCaptureDevices);
	}
	else
	{
		int nSel = m_cTypes.GetCurSel();
		if (   nSel != CB_ERR 
			&& sKey == m_saTypes[nSel])
		{
			CString sType;
			SplitString(sValue, m_saUrls, _T(","));
			int i, nFind, n = (int)m_saUrls.GetCount();
			m_cURL.ResetContent();
			for (i=0; i<n; i++)
			{
				nFind = m_saUrls[i].Find(_T(":"));
				if (nFind != -1)
				{
					m_cURL.AddString(m_saUrls[i].Left(nFind));
					m_saUrls[i] = m_saUrls[i].Mid(nFind+1);
				}
			}
			if (n)
			{
				m_cURL.SetCurSel(0);
				OnCbnSelchangeComboUrl();
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterDlg::OnConfirmSetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwUserData)
{
	if (sKey == CSD_CAM_TYPE)
	{
		if (sValue != CSD_OFF)
		{
			if (sValue == CSD_CAM_CAPTURE)
			{
				m_pOutputControl->DoRequestGetValue(id, sValue, 0);
			}
			else
			{
				RequestParameterSet(id);
			}
		}
	}
	else if (sKey == CSD_CAM_CAPTURE)
	{
		RequestParameterSet(id);
	}
	else if (sKey == CSD_NAME)
	{
		if (dwUserData == 0)
		{
			m_cCaptureDevices.DeleteString(m_nCurrentCaptureDevice);
			int nSel = m_cCaptureDevices.InsertString(m_nCurrentCaptureDevice, sValue);
			if (nSel != -1)
			{
				m_cCaptureDevices.SetCurSel(nSel);
			}
		}
		else
		{
			m_cCaptureDevices.SetCurSel(m_nCurrentCaptureDevice);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterDlg::RequestParameterSet(CSecID id)
{
	m_pOutputControl->DoRequestGetValue(id, CSD_COMPRESSIONS, 0);
	m_pOutputControl->DoRequestGetValue(id, CSD_RESOLUTIONS, 0);
	m_pOutputControl->DoRequestGetValue(id, CSD_ROTATIONS, 0);
	m_pOutputControl->DoRequestGetValue(id, CSD_FRAMERATES, 0);
	m_pOutputControl->DoRequestGetValue(id, CSD_OUTPUTS, 0);
	m_pOutputControl->DoRequestGetValue(id, CSD_INPUTS, 0);
	m_pOutputControl->DoRequestGetValue(id, CSD_RELAIS, 0);
	m_pOutputControl->DoRequestGetValue(id, CSD_NAME, 0);
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterDlg::OnDestroy()
{
	CDialog::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CCameraParameterDlg::OnUser(WPARAM wParam, LPARAM lParam)
{
	if (LOWORD(wParam) == WM_INITDIALOG)
	{
		CSecID id(m_pOutputControl->GetGroupID(), CSV_CONTROL_ID);
		m_pOutputControl->DoRequestGetValue(id, CSD_CAM_TYPE, 0);
	}
	return 0;
}

#define TypeNr2TypeName(m_ExOutputType) _T("camera")

void CCameraParameterDlg::OnBnClickedOk()
{
	if (UpdateData())
	{
		int i, nSel = m_cTypes.GetCurSel();
		ASSERT(nSel != CB_ERR);
		CString sFmt;
		m_sDeviceType = m_saTypes[nSel];
		m_sCameras.Empty();
		if (nSel == 0)
		{
			m_cCaptureDevices.GetLBText(m_cCaptureDevices.GetCurSel(), m_sName);
		}
		else
		{
			GetDlgItemText(IDC_EDT_NAME, m_sName);
			for (i=0; i<8; i++)
			{
				if (GetDlgItem(IDC_CK_CAMERA1+i)->IsWindowVisible())
				{
					if (IsDlgButtonChecked(IDC_CK_CAMERA1+i) == BST_CHECKED)
					{
						sFmt.Format(_T("%d,"), i+1);
						m_sCameras += sFmt;
					}
					else
					{
						break;
					}
				}
			}
		}
		m_sRelais.Empty();
		for (i=0; i<8; i++)
		{
			if (GetDlgItem(IDC_CK_OUTPUT1+i)->IsWindowVisible())
			{
				if (IsDlgButtonChecked(IDC_CK_OUTPUT1+i) == BST_CHECKED)
				{
					sFmt.Format(_T("%d,"), i+1);
					m_sRelais += sFmt;
				}
			}
			else
			{
				break;
			}
		}
		m_sInputs.Empty();
		for (i=0; i<8; i++)
		{
			if (GetDlgItem(IDC_CK_INPUT1+i)->IsWindowVisible())
			{
				if (IsDlgButtonChecked(IDC_CK_INPUT1+i) == BST_CHECKED)
				{
					sFmt.Format(_T("%d,"), i+1);
					m_sInputs += sFmt;
				}
			}
			else
			{
				break;
			}
		}

		RemoveLastCharacter(m_sCameras, _T(','));
		RemoveLastCharacter(m_sRelais , _T(','));
		RemoveLastCharacter(m_sInputs , _T(','));

		m_cCompression.GetLBText(m_cCompression.GetCurSel(), m_sCompresion);
		m_cResolutions.GetLBText(m_cResolutions.GetCurSel(), m_sResolution);
		m_cRotations.GetLBText(  m_cRotations.GetCurSel()  , m_sRotation);

#ifdef _DEBUG
		// RKE: test camera string implementation
		// Samples for the registry
		// Camera: 
		// \COSpy Camera\TYcamera\SN0\CT4\REF0\TERM0\CamTCapture\NDesktop Rolf Port A\Turn0\ARffffffff
		// \COAxis 1\TYcamera\SN0\CT4\REF0\TERM0\CamTAxis\NAXIS 241Q Video Server Blade\Url192.168.0.233\Port80\CamNR1\Turn0
		// \COPanasonic 1\TYcamera\SN0\CT4\REF0\TERM0\CamTPanasonic\NPanasonic WV-NP 244 E\Url192.168.0.234\Port80\Turn0\ARffffffff
		CString sOutputDefine;

		BOOL m_bSN = FALSE;
		BOOL m_bReference = FALSE;
		BOOL m_bTerminated = FALSE;
		int m_ExOutputType = 4; // 6: EX_OUTPUT_CAMERA_IP, 7:EX_OUTPUT_CAMERA_CAPTURE

		sOutputDefine.Format(_T("%s%s%s%s\\SN%d%s%u%s%d%s%d"),
							INI_COMMENT, m_sName, 
							INI_OUTPUT_TYPESTR, TypeNr2TypeName(m_ExOutputType), 
							m_bSN,
							INI_OUTPUT_TYPE, m_ExOutputType,
							INI_REFERENCE, m_bReference,
							INI_TERMINATION, m_bTerminated);

		CString sIPdata;
		if (m_saTypes[nSel] == CSD_CAM_CAPTURE)
		{
			CString sName;
			m_cCaptureDevices.GetLBText(m_cCaptureDevices.GetCurSel(), sName);
			sIPdata.Format(_T("%s%s%s%s"),
				INI_CAMERA_TYPE, m_saTypes[nSel],
				INI_NAME, sName);
		}
		else
		{
			CString sName;
			GetDlgItemText(IDC_EDT_NAME, sName);
			sIPdata.Format(_T("%s%s%s%s%s%s%s%d"),
				INI_CAMERA_TYPE, m_saTypes[nSel],
				INI_NAME, sName,
				INI_URL, m_sUrl,
				INI_PORT, m_uPort);
		}

		CString sText = _T("1"); // Camera Number
		sIPdata += INI_CAMERA_NR + sText;

		if (m_cRotations.IsWindowEnabled())
		{
			CString sText;
			m_cRotations.GetWindowText(sText);
			sIPdata += INI_TURN + sText;
		}
		sOutputDefine += sIPdata;
		CWK_Profile wkp;
		wkp.WriteString(SEC_NAME_OUTPUTGROUPS, _T("TestOutput"), sOutputDefine);

		// Relais: 
		// \CORelais Flanke\TYrelay\CT1\SN0\RT1\RC0\CK0\KC0\RelTAxisVS\Url192.168.0.233\Port80\RelNR1
		// \CORelais Pegel\TYrelay\CT1\SN0\RT1\RC1\CK1\KC0\RelTAxisVS\Url192.168.0.233\Port80\RelNR2
		// \CORelais Flanke Öffner\TYrelay\CT1\SN0\RT1\RC0\CK1\KC0\RelTAxisVS\Url192.168.0.233\Port80\RelNR3
		// \CORelais Flanke Öffner Halten\TYrelay\CT1\SN0\RT1\RC0\CK1\KC1\RelTAxisVS\Url192.168.0.233\Port80\RelNR4
		// \CORelais Pegel Halten\TYrelay\CT1\SN0\RT1\RC1\CK1\KC1
		// Input:
		// \COMelder 1\A10001\E1\TDA1\InpTAxisVS\Url192.168.0.233\Port80\InpNR1
		// \COMelder 2\A10001\E1\TDA1\InpTAxisVS\Url192.168.0.233\Port80\InpNR2
		// \COMelder 3\A10001\E1\TDA1\InpTAxisVS\Url192.168.0.233\Port80\InpNR3
		// \COMelder 4\A10001\E1\TDA1\InpTAxisVS\Url192.168.0.233\Port80\InpNR4
#endif
		EndDialog(IDOK);
	}
}
#endif