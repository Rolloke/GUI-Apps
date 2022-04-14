/* GlobalReplace: m_ctlAllow --> m_cbtAllow */
/* GlobalReplace: m_bRunVCUnit --> m_bRunVCSUnit */
/* GlobalReplace: m_ctbRunCDRWriter --> m_cbtRunCDRWriter */
/* GlobalReplace: GAAUnit --> GAUnit */
/* GlobalReplace: m_ctRunCDRWriter --> m_cbtRunCDRWriter */
// ProductViewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ProductView.h"
#include "ProductViewDlg.h"
#include "DialogDongle.h"
#include ".\productviewdlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProductViewDlg dialog
CProductViewDlg::CProductViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProductViewDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProductViewDlg)
	m_sProductName = _T("ProductName");
	m_bRunAKUUnit = FALSE;
	m_bRunCDRWriter = FALSE;
	m_bRunQUnit = FALSE;
	m_bRunCommUnit = FALSE;
	m_bRunGAUnit = FALSE;
	m_bRunISDNUnit = FALSE;
	m_bRunIPCameraUnit = FALSE;
	m_bRunPTUnit = FALSE;
	m_bRunTOBSUnit = FALSE;
	m_bRunSDIUnit = FALSE;
	m_bRunServer = FALSE;
	m_bRunSimUnit = FALSE;
	m_bRunSocketUnit = FALSE;
	m_bRunVCSUnit = FALSE;
	m_bRunExplorer = FALSE;
	m_sFullProductName = _T("");
	m_bRunSecAnalyzer = FALSE;
	m_bAllowLocalProcesses = FALSE;
	m_sFullVersionText = _T("");
	m_bRunSDIConfig = FALSE;
	m_bAllowInternalAccess = FALSE;
	m_sSerial = _T("");
	m_bAllowRouting = FALSE;
	m_bRunISDNUnit2 = FALSE;
	m_bRunPTUnit2 = FALSE;
	m_bAllowSDICashSystems = FALSE;
	m_bAllowSDIIndustrialEquipment = FALSE;
	m_bAllowSDIAccessControl = FALSE;
	m_bAllowSDIParkmanagment = FALSE;
	m_bAllowSoftDecodePT = FALSE;
	m_bRunSMSUnit = FALSE;
	m_bRunJaCobUnit1 = FALSE;
	m_bRunJaCobUnit2 = FALSE;
	m_bRunJaCobUnit3 = FALSE;
	m_bRunJaCobUnit4 = FALSE;
	m_bRunGemosUnit = FALSE;
	m_bAllowMotionDetection = FALSE;
	m_bAllowHTMLmaps = FALSE;
	m_bAllowMultiMonitor = FALSE;
	m_bAllowTimers = FALSE;
	m_bRunEMailUnit = FALSE;
	m_bRunFAXUnit = FALSE;
	m_bRunHTTP = FALSE;
	m_bRestrictedTo8CamerasPerBoard = FALSE;
	m_bRunSaVicUnit1 = FALSE;
	m_bRunSaVicUnit2 = FALSE;
	m_bRunSaVicUnit3 = FALSE;
	m_bRunSaVicUnit4 = FALSE;
	m_bRunTashaUnit1 = FALSE;
	m_bRunTashaUnit2 = FALSE;
	m_bRunTashaUnit3 = FALSE;
	m_bRunTashaUnit4 = FALSE;
	m_bRunAudioUnit = FALSE;
	m_bRunICPCONUnit = FALSE;
	m_bRunUSBCameraUnit = FALSE;
	m_iSerialOrHddID = 1;
	
	m_iNoOfHosts = 0;
	m_iNrOfCameras = 0;
	m_iNrOfIPCameras = 0;
	//}}AFX_DATA_INIT
	m_eToggleCodecs = eToggleCodecUnknown;

	FillAllButtonsArray();

	// OOPS duplicate load, first default, that from cmd-line
	if (theApp.GetFileToUse().GetLength()) {
		WK_TRACE(_T("Try '%s'\n"),theApp.GetFileToUse() );
		m_dongle.LoadFromCFG( theApp.GetFileToUse() );
	}
	SetFromDongle();

	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProductViewDlg)
	DDX_Control(pDX, IDC_EDIT_SERIAL, m_editSerial);
	DDX_Control(pDX, IDC_RADIO_SERIAL, m_btnSerial);
	DDX_Control(pDX, IDC_RADIO_HDD, m_btnHDD);
	DDX_Control(pDX, IDC_RUN_AUDIO_UNIT, m_cbtRunAudioUnit);
	DDX_Control(pDX, IDC_RUN_USB_CAMERA, m_cbtRunUSBCameraUnit);
	DDX_Control(pDX, IDC_RUN_ICPCON_UNIT, m_cbtRunICPCONUnit);
	DDX_Control(pDX, IDC_RUN_SAVIC4, m_cbtRunSaVicUnit4);
	DDX_Control(pDX, IDC_RUN_SAVIC3, m_cbtRunSaVicUnit3);
	DDX_Control(pDX, IDC_RUN_SAVIC2, m_cbtRunSaVicUnit2);
	DDX_Control(pDX, IDC_RUN_SAVIC1, m_cbtRunSaVicUnit1);
	DDX_Control(pDX, IDC_RUN_TASHA1, m_cbtRunTashaUnit1);
	DDX_Control(pDX, IDC_RUN_TASHA2, m_cbtRunTashaUnit2);
	DDX_Control(pDX, IDC_RUN_TASHA3, m_cbtRunTashaUnit3);
	DDX_Control(pDX, IDC_RUN_TASHA4, m_cbtRunTashaUnit4);
	DDX_Control(pDX, IDC_TXT_PRODUCT_VERSION, m_ctlVersion);
	DDX_Control(pDX, IDC_RESTRICTED_TO_8_CAMERAS_PER_BOARD, m_cbtRestrictedTo8CamerasPerBoard);
	DDX_Control(pDX, IDC_HDD_ID2, m_staticHDD2);
	DDX_Control(pDX, IDC_EDIT_HDD2, m_editHDD2);
	DDX_Control(pDX, IDC_EDIT_HDD1, m_editHDD1);
	DDX_Control(pDX, IDC_CRYPT, m_buttonCrypt);
	DDX_Control(pDX, IDC_RUN_HTTP, m_cbtRunHTTP);
	DDX_Control(pDX, IDC_RUN_FAX_UNIT, m_cbtRunFAXUnit);
	DDX_Control(pDX, IDC_RUN_EMAIL_UNIT, m_cbtRunEMailUnit);
	DDX_Control(pDX, IDC_ALLOW_MOTION_DETECTION, m_cbtAllowMotionDetection);
	DDX_Control(pDX, IDC_ALLOW_MULTI_MONITOR, m_cbtAllowMultiMonitor);
	DDX_Control(pDX, IDC_ALLOW_HTML_MAPS, m_cbtAllowHTMLmaps);
	DDX_Control(pDX, IDC_ALLOW_TIMERS, m_cbtAllowTimers);
	DDX_Control(pDX, IDC_RUN_GEMOS_UNIT, m_cbtRunGemosUnit);
	DDX_Control(pDX, IDC_RUN_JACOB4, m_cbtRunJaCobUnit4);
	DDX_Control(pDX, IDC_RUN_JACOB3, m_cbtRunJaCobUnit3);
	DDX_Control(pDX, IDC_RUN_JACOB2, m_cbtRunJaCobUnit2);
	DDX_Control(pDX, IDC_RUN_JACOB1, m_cbtRunJaCobUnit1);
	DDX_Control(pDX, IDC_RUN_SMS, m_cbtRunSMSUnit);
	DDX_Control(pDX, IDC_ALLOW_SOFT_DECODE_PT, m_cbtAllowSoftDecodePT);
	DDX_Control(pDX, IDC_ALLOW_SDI_ACCESS_CONTROL, m_cbtAllowSDIAccessControl);
	DDX_Control(pDX, IDC_ALLOW_SDI_PARK_MANAGMENT, m_cbtAllowSDIParkmanagment);
	DDX_Control(pDX, IDC_ALLOW_SDI_INDUSTRIAL, m_cbtAllowSDIIndustrialEquipment);
	DDX_Control(pDX, IDC_ALLOW_SDI_GA, m_cbtAllowSDICashSystems);
	DDX_Control(pDX, IDC_RUN_PRESENCE_UNIT2, m_cbtRunPTUnit2);
	DDX_Control(pDX, IDC_RUN_ISDN_UNIT2, m_cbtRunISDNUnit2);
	DDX_Control(pDX, IDC_ALLOW_ROUTING, m_cbtAllowRouting);
	DDX_Control(pDX, IDC_TXT_SERIAL, m_ctlSerialLabel);
	DDX_Control(pDX, IDC_SERIAL, m_ctlSerial);
	DDX_Control(pDX, IDC_RUN_CDR_WRITER, m_cbtRunCDRWriter);
	DDX_Control(pDX, IDC_ALLOW_INTERNAL_ACCESS, m_cbtAllowInternalAccess);
	DDX_Control(pDX, IDC_RUN_SDI_CONFIG, m_cbtRunSDIConfig);
	DDX_Control(pDX, IDC_OEM_CODE, m_cbOEMCode);
	DDX_Control(pDX, IDC_TXT_FULL_VERSION, m_ctFullVersion);
	DDX_Control(pDX, IDC_ALLOW_LOCAL_STORAGE_PROCESSES, m_cbtAllowLocalProcesses);
	DDX_Control(pDX, IDC_RUN_ANALYZER, m_cbtRunSecAnalyzer);
	DDX_Control(pDX, IDC_RUN_SIM_UNIT, m_cbtRunSimUnit);
	DDX_Control(pDX, IDC_RUN_SDI_UNIT, m_cbtRunSDIUnit);
	DDX_Control(pDX, IDC_RUN_GAA_UNIT, m_cbtRunGAUnit);
	DDX_Control(pDX, IDC_RUN_COMM_UNIT, m_cbtRunCommUnit);
	DDX_Control(pDX, IDC_RUN_AKU_UNIT, m_cbtRunAKUUnit);
	DDX_Control(pDX, IDC_RUN_SERVER, m_cbtRunServer);
	DDX_Control(pDX, IDC_TXT_FULL_PRODUCT_Name, m_ctFullProductName);
	DDX_Control(pDX, IDC_RUN_VCS_UNIT, m_cbtRunVCSUnit);
	DDX_Control(pDX, IDC_RUN_SOCKET_UNIT, m_cbtRunSocketUnit);
	DDX_Control(pDX, IDC_RUN_PRESENCE_UNIT, m_cbtRunPTUnit);
	DDX_Control(pDX, IDC_RUN_TOBS_UNIT, m_cbtRunTOBSUnit);
	DDX_Control(pDX, IDC_RUN_IP_UNIT, m_cbtRunIPCameraUnit);
	DDX_Control(pDX, IDC_RUN_ISDN_UNIT, m_cbtRunISDNUnit);
	DDX_Control(pDX, IDC_RUN_QUNIT, m_cbtRunQUnit);
	DDX_Control(pDX, IDC_DO_SAVE, m_buttonSave);
	DDX_Control(pDX, IDC_PRODUCT_NAME, m_cbProduct);
	DDX_Control(pDX, IDC_COMBO_NR_CAMERAS, m_cbNrOfCameras);
	DDX_Control(pDX, IDC_COMBO_NR_IP_CAMERAS, m_cbNrOfIPCameras);
	DDX_Control(pDX, IDC_SPIN_NO_OF_HOSTS, m_spinNoOfHosts);
	DDX_CBString(pDX, IDC_PRODUCT_NAME, m_sProductName);
	DDX_Check(pDX, IDC_RUN_AKU_UNIT, m_bRunAKUUnit);
	DDX_Check(pDX, IDC_RUN_CDR_WRITER, m_bRunCDRWriter);
	DDX_Check(pDX, IDC_RUN_QUNIT, m_bRunQUnit);
	DDX_Check(pDX, IDC_RUN_COMM_UNIT, m_bRunCommUnit);
	DDX_Check(pDX, IDC_RUN_GAA_UNIT, m_bRunGAUnit);
	DDX_Check(pDX, IDC_RUN_ISDN_UNIT, m_bRunISDNUnit);
	DDX_Check(pDX, IDC_RUN_IP_UNIT, m_bRunIPCameraUnit);
	DDX_Check(pDX, IDC_RUN_PRESENCE_UNIT, m_bRunPTUnit);
	DDX_Check(pDX, IDC_RUN_TOBS_UNIT, m_bRunTOBSUnit);
	DDX_Check(pDX, IDC_RUN_SDI_UNIT, m_bRunSDIUnit);
	DDX_Check(pDX, IDC_RUN_SERVER, m_bRunServer);
	DDX_Check(pDX, IDC_RUN_SIM_UNIT, m_bRunSimUnit);
	DDX_Check(pDX, IDC_RUN_SOCKET_UNIT, m_bRunSocketUnit);
	DDX_Check(pDX, IDC_RUN_VCS_UNIT, m_bRunVCSUnit);
	DDX_Text(pDX, IDC_TXT_FULL_PRODUCT_Name, m_sFullProductName);
	DDX_Check(pDX, IDC_RUN_ANALYZER, m_bRunSecAnalyzer);
	DDX_Check(pDX, IDC_ALLOW_LOCAL_STORAGE_PROCESSES, m_bAllowLocalProcesses);
	DDX_Text(pDX, IDC_TXT_FULL_VERSION, m_sFullVersionText);
	DDX_Check(pDX, IDC_RUN_SDI_CONFIG, m_bRunSDIConfig);
	DDX_Check(pDX, IDC_ALLOW_INTERNAL_ACCESS, m_bAllowInternalAccess);
	DDX_Text(pDX, IDC_SERIAL, m_sSerial);
	DDX_Check(pDX, IDC_ALLOW_ROUTING, m_bAllowRouting);
	DDX_Check(pDX, IDC_RUN_ISDN_UNIT2, m_bRunISDNUnit2);
	DDX_Check(pDX, IDC_RUN_PRESENCE_UNIT2, m_bRunPTUnit2);
	DDX_Check(pDX, IDC_ALLOW_SDI_GA, m_bAllowSDICashSystems);
	DDX_Check(pDX, IDC_ALLOW_SDI_INDUSTRIAL, m_bAllowSDIIndustrialEquipment);
	DDX_Check(pDX, IDC_ALLOW_SDI_ACCESS_CONTROL, m_bAllowSDIAccessControl);
	DDX_Check(pDX, IDC_ALLOW_SDI_PARK_MANAGMENT, m_bAllowSDIParkmanagment);
	DDX_Check(pDX, IDC_ALLOW_SOFT_DECODE_PT, m_bAllowSoftDecodePT);
	DDX_Check(pDX, IDC_RUN_SMS, m_bRunSMSUnit);
	DDX_Check(pDX, IDC_RUN_JACOB1, m_bRunJaCobUnit1);
	DDX_Check(pDX, IDC_RUN_JACOB2, m_bRunJaCobUnit2);
	DDX_Check(pDX, IDC_RUN_JACOB3, m_bRunJaCobUnit3);
	DDX_Check(pDX, IDC_RUN_JACOB4, m_bRunJaCobUnit4);
	DDX_Check(pDX, IDC_RUN_GEMOS_UNIT, m_bRunGemosUnit);
	DDX_Check(pDX, IDC_ALLOW_MOTION_DETECTION, m_bAllowMotionDetection);
	DDX_Check(pDX, IDC_ALLOW_MULTI_MONITOR, m_bAllowMultiMonitor);
	DDX_Check(pDX, IDC_ALLOW_HTML_MAPS, m_bAllowHTMLmaps);
	DDX_Check(pDX, IDC_ALLOW_TIMERS, m_bAllowTimers);
	DDX_Check(pDX, IDC_RUN_EMAIL_UNIT, m_bRunEMailUnit);
	DDX_Check(pDX, IDC_RUN_FAX_UNIT, m_bRunFAXUnit);
	DDX_Check(pDX, IDC_RUN_HTTP, m_bRunHTTP);
	DDX_Check(pDX, IDC_RESTRICTED_TO_8_CAMERAS_PER_BOARD, m_bRestrictedTo8CamerasPerBoard);
	DDX_Check(pDX, IDC_RUN_SAVIC1, m_bRunSaVicUnit1);
	DDX_Check(pDX, IDC_RUN_SAVIC2, m_bRunSaVicUnit2);
	DDX_Check(pDX, IDC_RUN_SAVIC3, m_bRunSaVicUnit3);
	DDX_Check(pDX, IDC_RUN_SAVIC4, m_bRunSaVicUnit4);
	DDX_Check(pDX, IDC_RUN_TASHA1, m_bRunTashaUnit1);
	DDX_Check(pDX, IDC_RUN_TASHA2, m_bRunTashaUnit2);
	DDX_Check(pDX, IDC_RUN_TASHA3, m_bRunTashaUnit3);
	DDX_Check(pDX, IDC_RUN_TASHA4, m_bRunTashaUnit4);
	DDX_Check(pDX, IDC_RUN_AUDIO_UNIT, m_bRunAudioUnit);
	DDX_Check(pDX, IDC_RUN_ICPCON_UNIT, m_bRunICPCONUnit);
	DDX_Check(pDX, IDC_RUN_USB_CAMERA, m_bRunUSBCameraUnit);
	DDX_Radio(pDX, IDC_RADIO_SERIAL, m_iSerialOrHddID);
	DDX_Text(pDX, IDC_EDT_NO_OF_HOSTS, m_iNoOfHosts);

	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate)
	{
		CString s;
		m_cbNrOfCameras.GetWindowText(s);
		_stscanf(s,_T("%d"),&m_iNrOfCameras);

		m_cbNrOfIPCameras.GetWindowText(s);
		_stscanf(s,_T("%d"),&m_iNrOfIPCameras);
	}
	else
	{
		int nPos = 0;
		switch (m_iNoOfHosts)
		{
			case 0: nPos = 0; break;
			case 5: nPos = 1; break;
			default:
				nPos = (m_iNoOfHosts / 10)+1;
			break;
		}
		m_spinNoOfHosts.SetPos(nPos);

		CString s;
		s.Format(_T("%d"),m_iNrOfCameras);
		m_cbNrOfCameras.SelectString(0,s);

		s.Format(_T("%d"),m_iNrOfIPCameras);
		m_cbNrOfIPCameras.SelectString(0,s);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::FillAllButtonsArray() 
{
	// base
	m_allButtons.Add(&m_cbtRunServer);
	m_allButtons.Add(&m_cbtRunCDRWriter);

	// codecs
	m_allButtons.Add(&m_cbtRunQUnit);
	m_allButtons.Add(&m_cbtRunIPCameraUnit);
	
	m_allButtons.Add(&m_cbtRunJaCobUnit1);
	m_allButtons.Add(&m_cbtRunJaCobUnit2);
	m_allButtons.Add(&m_cbtRunJaCobUnit3);
	m_allButtons.Add(&m_cbtRunJaCobUnit4);

	m_allButtons.Add(&m_cbtRunSaVicUnit1);
	m_allButtons.Add(&m_cbtRunSaVicUnit2);
	m_allButtons.Add(&m_cbtRunSaVicUnit3);
	m_allButtons.Add(&m_cbtRunSaVicUnit4);

	m_allButtons.Add(&m_cbtRunTashaUnit1);
	m_allButtons.Add(&m_cbtRunTashaUnit2);
	m_allButtons.Add(&m_cbtRunTashaUnit3);
	m_allButtons.Add(&m_cbtRunTashaUnit4);

	m_allButtons.Add(&m_cbtRestrictedTo8CamerasPerBoard);

	// link
	m_allButtons.Add(&m_cbtRunISDNUnit);
	m_allButtons.Add(&m_cbtRunISDNUnit2);
	m_allButtons.Add(&m_cbtRunSocketUnit);
	m_allButtons.Add(&m_cbtRunSMSUnit);
	m_allButtons.Add(&m_cbtRunEMailUnit);
	m_allButtons.Add(&m_cbtRunFAXUnit);

	// http
    m_allButtons.Add(&m_cbtRunHTTP);

	// units
	m_allButtons.Add(&m_cbtRunSimUnit);
	m_allButtons.Add(&m_cbtRunSDIUnit);
	m_allButtons.Add(&m_cbtRunGAUnit);
	m_allButtons.Add(&m_cbtRunCommUnit);
	m_allButtons.Add(&m_cbtRunAKUUnit);
	m_allButtons.Add(&m_cbtRunGemosUnit);

	// misc
	m_allButtons.Add(&m_cbtRunSecAnalyzer);
	m_allButtons.Add(&m_cbtRunSDIConfig);

	// boxes
	m_allButtons.Add(&m_cbtRunPTUnit);
	m_allButtons.Add(&m_cbtRunTOBSUnit);
	m_allButtons.Add(&m_cbtRunPTUnit2);
	m_allButtons.Add(&m_cbtRunVCSUnit);

	// functionalities
	m_allButtons.Add(&m_cbtAllowLocalProcesses);

	m_allButtons.Add(&m_cbtAllowSoftDecodePT);

	m_allButtons.Add(&m_cbtAllowRouting);

	m_allButtons.Add(&m_cbtAllowSDIAccessControl);
	m_allButtons.Add(&m_cbtAllowSDIIndustrialEquipment);
	m_allButtons.Add(&m_cbtAllowSDICashSystems);
	m_allButtons.Add(&m_cbtAllowSDIParkmanagment);

	m_allButtons.Add(&m_cbtAllowHTMLmaps);
	m_allButtons.Add(&m_cbtAllowMultiMonitor);
	m_allButtons.Add(&m_cbtAllowTimers);
}
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::ResetAllFlags()
{
	UpdateMembersFromDialog();

	m_bRunAKUUnit = FALSE;
	m_bRunCDRWriter = FALSE;
	m_bRunQUnit = FALSE;
	m_bRunCommUnit = FALSE;
	m_bRunGAUnit = FALSE;
	m_bRunISDNUnit = FALSE;
	m_bRunSMSUnit = FALSE;
	m_bRunIPCameraUnit = FALSE;
	m_bRunPTUnit = FALSE;
	m_bRunTOBSUnit = FALSE;
	m_bRunSDIUnit = FALSE;
	m_bRunServer = FALSE;
	m_bRunSimUnit = FALSE;
	m_bRunSocketUnit = FALSE;
	m_bRunVCSUnit = FALSE;
	m_bRunExplorer = FALSE;
	m_bRunSecAnalyzer = FALSE;
	m_bAllowLocalProcesses = FALSE;
	m_bRunSDIConfig = FALSE;
	m_bAllowInternalAccess = FALSE;
	m_bAllowRouting = FALSE;
	m_bRunISDNUnit2 = FALSE;
	m_bRunPTUnit2 = FALSE;
	m_bAllowSDICashSystems = FALSE;
	m_bAllowSDIIndustrialEquipment = FALSE;
	m_bAllowSDIAccessControl = FALSE;
	m_bAllowSDIParkmanagment = FALSE;
	m_bAllowSoftDecodePT = FALSE;
	
	m_bRunJaCobUnit1 = FALSE;
	m_bRunJaCobUnit2 = FALSE;
	m_bRunJaCobUnit3 = FALSE;
	m_bRunJaCobUnit4 = FALSE;
	
	m_bRunSaVicUnit1 = FALSE;
	m_bRunSaVicUnit2 = FALSE;
	m_bRunSaVicUnit3 = FALSE;
	m_bRunSaVicUnit4 = FALSE;
	
	m_bRunTashaUnit1 = FALSE;
	m_bRunTashaUnit2 = FALSE;
	m_bRunTashaUnit3 = FALSE;
	m_bRunTashaUnit4 = FALSE;

	m_bRunGemosUnit = FALSE;
	m_bRunFAXUnit = FALSE;
	m_bRunEMailUnit = FALSE;
    m_bRunHTTP = FALSE;
	m_bRestrictedTo8CamerasPerBoard = FALSE;

	UpdateDialogFromMembers();
}
/////////////////////////////////////////////////////////////////////////////
// CAVEAT InitProcutNames depends on m_cbOEMCode
void CProductViewDlg::InitProductNames()
{
	int iOEMCode = GetCurrentOEMCode();
	m_cbProduct.ResetContent();

	// NEWOEM
	if (   iOEMCode==CWK_Dongle::OEM_IDIP
		|| iOEMCode==CWK_Dongle::OEM_ALARMCOM
		|| iOEMCode==CWK_Dongle::OEM_DRESEARCH
		|| iOEMCode==CWK_Dongle::OEM_SANTEC
		|| iOEMCode==CWK_Dongle::OEM_SIEMENS
		) 
	{
		CString sProductName;
		CString sBrandName;

		sBrandName = COemGuiApi::GetBrandName((CWK_Dongle::OemCode)iOEMCode);
		for (int ipc=IPC_MIN_CODE;ipc<=IPC_MAX_CODE;ipc++) 
		{
			// skip some IPC's
			if (   (ipc!=IPC_FREE_STYLE)
				&& (ipc!=IPC_STORAGE_PLUS)
				&& (ipc!=IPC_STORAGE_TRANSMITTER)
				&& (ipc!=IPC_DTS_RECEIVER)
				&& (ipc!=IPC_TOBS_RECEIVER)
				&& (ipc!=IPC_DTS)
				&& (ipc!=IPC_DTS_IP_RECEIVER)
				&& (ipc!=IPC_TOBS_TO3K)
				)
			{
				TRACE(_T("ipc = %d %s\n"),(int)ipc,COemGuiApi::GetProductCodeName((CWK_Dongle::OemCode)iOEMCode,
							 									  (InternalProductCode)ipc));
				// new ones only new product names
				if (   (theApp.IsCryptEnabled() == FALSE)
					|| IsNewIdipGeneration((InternalProductCode)ipc)
					|| (ipc == IPC_INSPICIO)
					|| (ipc == IPC_INSPECTUS)
					)
				{
					sProductName = COemGuiApi::GetProductCodeName((CWK_Dongle::OemCode)iOEMCode,
							 									  (InternalProductCode)ipc);

					CString sInsert = sBrandName + _T(" ") + sProductName;
					
					if (   ipc == IPC_INSPICIO
						|| ipc == IPC_INSPECTUS)
					{
						sInsert = sProductName;
					}

					if (!sProductName.IsEmpty())
					{
						int ix = -1;
						// WORKAROUND for idip 4, insert before idip 8
						if (ipc == IPC_IDIP_4)
						{
							for (int iy=0 ; iy<m_cbProduct.GetCount() ; iy++)
							{
								if (m_cbProduct.GetItemData(iy) == IPC_IDIP_8)
								{
									ix = m_cbProduct.InsertString(iy,sInsert);
									break;
								}
							}
						}
						else
						{
							ix = m_cbProduct.AddString(sInsert);
						}
						m_cbProduct.SetItemData(ix, ipc);
					}
					else
					{
						WK_TRACE(_T("no product name for IPC %d\n"),ipc);
					}
				}
			}
		}
	} 
	else if (iOEMCode==CWK_Dongle::OEM_AKUBIS) 
	{
		int ix =0;
		int ipc=0;
		CString sProductName;

		sProductName = COemGuiApi::GetBrandName((CWK_Dongle::OemCode)iOEMCode);
		ipc = IPC_FREE_STYLE;
		ix = m_cbProduct.AddString(sProductName);
		m_cbProduct.SetItemData(ix, ipc);
	// NEWOEM
	} else {
		m_cbProduct.AddString(_T("Invalid Product"));
		m_cbProduct.SetItemData(m_cbProduct.GetCount()-1, IPC_FREE_STYLE);
	}
}
/////////////////////////////////////////////////////////////////////////////
// CAVEAT ProcutNames depends on m_cbOEMCode
void CProductViewDlg::UpdateProductNames()
{
	int iOEMCode = GetCurrentOEMCode();

	// NEWOEM
	if (   iOEMCode==CWK_Dongle::OEM_IDIP
		|| iOEMCode==CWK_Dongle::OEM_ALARMCOM
		|| iOEMCode==CWK_Dongle::OEM_DRESEARCH
		|| iOEMCode==CWK_Dongle::OEM_SANTEC
		|| iOEMCode==CWK_Dongle::OEM_SIEMENS
		) 
	{
		CString sBrandName = COemGuiApi::GetBrandName((CWK_Dongle::OemCode)iOEMCode);
		InternalProductCode ipc;
		CString sProductNameOld, sProductNameNew;
		// Save current selection
		InternalProductCode ipcCurrent = GetCurrentProduct();
		for (int i=0 ; i<m_cbProduct.GetCount() ; i++) 
		{
			ipc = (InternalProductCode)m_cbProduct.GetItemData(i);
			// update the Product names for
			// IPC_IDIP_PROJECT
			if (ipc == IPC_IDIP_PROJECT)
			{
				m_cbProduct.GetLBText(i, sProductNameOld);
				
				int iIndex1 = sProductNameOld.Find(_T(' '));
				iIndex1 = sProductNameOld.Find(_T(' '), iIndex1+1);
				// special case Siemens has oem name with 1 space
				if (iOEMCode==CWK_Dongle::OEM_SIEMENS)
				{
					iIndex1 = sProductNameOld.Find(_T(' '), iIndex1+1);
				}
				int iIndex2 = sProductNameOld.Find(_T(' '), iIndex1+1);
				if (   (iIndex1 != -1)
					&& (iIndex2 != -1)
					)
				{
					CString sCamerasOld = sProductNameOld.Mid(iIndex1+1, iIndex2-iIndex1-1);
					CString sCamerasNew;
					if (ipc == ipcCurrent)
					{
						int iBoardsTotal = 0;
						int iCamerasTotal = 0;
						int iBoards = 0;

						if		(m_bRunJaCobUnit4)	iBoards = 4;
						else if (m_bRunJaCobUnit3)	iBoards = 3;
						else if (m_bRunJaCobUnit2)	iBoards = 2;
						else if (m_bRunJaCobUnit1)	iBoards = 1;
						iBoardsTotal = iBoards;
						iCamerasTotal = (m_bRestrictedTo8CamerasPerBoard ? iBoards*8 : iBoards*16);
						
						if		(m_bRunSaVicUnit4)	iBoards = 4;
						else if (m_bRunSaVicUnit3)	iBoards = 3;
						else if (m_bRunSaVicUnit2)	iBoards = 2;
						else if (m_bRunSaVicUnit1)	iBoards = 1;
						else						iBoards = 0;
						iBoardsTotal  += iBoards;
						iCamerasTotal += (m_bRestrictedTo8CamerasPerBoard ? iBoards*2 : iBoards*4);
						
						if		(m_bRunTashaUnit4)	iBoards = 4;
						else if (m_bRunTashaUnit3)	iBoards = 3;
						else if (m_bRunTashaUnit2)	iBoards = 2;
						else if (m_bRunTashaUnit1)	iBoards = 1;
						else						iBoards = 0;
						iBoardsTotal  += iBoards;
						iCamerasTotal += (m_bRestrictedTo8CamerasPerBoard ? iBoards*4 : iBoards*8);

						sCamerasNew.Format(_T("%d-%d"), iBoardsTotal, iCamerasTotal);
					}
					else
					{
						// Has to match with the corresponding Constraints
						sCamerasNew.Format(_T("%d-%d"), 1, 16);
					}
					// must we really update?
					if (sCamerasNew != sCamerasOld)
					{
						sProductNameNew += sProductNameOld.Left(iIndex1+1);
						sProductNameNew += sCamerasNew;
						sProductNameNew += sProductNameOld.Mid(iIndex2);
						m_cbProduct.DeleteString(i);
						int ix = m_cbProduct.AddString(sProductNameNew);
						m_cbProduct.SetItemData(ix, ipc);
						// Reset selection
						SetValidProduct(ipcCurrent);
					}
				}
				else
				{
					sProductNameNew += sProductNameOld;
				}
			}
		}
	} 
}
/////////////////////////////////////////////////////////////////////////////
BOOL CProductViewDlg::SetCurSelByData(CComboBox &combo, DWORD dwData)
{
	BOOL bFound=FALSE;

	for (int i=0;bFound==FALSE && i<combo.GetCount();i++) {
		if (combo.GetItemData(i)==dwData) {
			combo.SetCurSel(i);
			bFound = TRUE;
		}
	}

	return bFound;
}
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::SetFromDongle()
{
	// CAVEAT this is not made via TRANSFER, since not all fields are in the dialog
	m_sProductName = _T("TheProduct");
	// base
	m_bRunServer = m_dongle.RunServer();
	m_bRunExplorer = m_dongle.RunExplorer();
	// database
	m_bRunAKUUnit = m_dongle.RunAKUUnit();
	m_bRunCDRWriter = m_dongle.RunCDRWriter();
	m_bRunQUnit = m_dongle.RunQUnit();
	m_bRunCommUnit = m_dongle.RunCommUnit();
	// codecs
	m_bRunIPCameraUnit = m_dongle.RunIPCameraUnit();

	m_bRunJaCobUnit1 = m_dongle.RunJaCobUnit1();
	m_bRunJaCobUnit2 = m_dongle.RunJaCobUnit2();
	m_bRunJaCobUnit3 = m_dongle.RunJaCobUnit3();
	m_bRunJaCobUnit4 = m_dongle.RunJaCobUnit4();

	m_bRunSaVicUnit1 = m_dongle.RunSaVicUnit1();
	m_bRunSaVicUnit2 = m_dongle.RunSaVicUnit2();
	m_bRunSaVicUnit3 = m_dongle.RunSaVicUnit3();
	m_bRunSaVicUnit4 = m_dongle.RunSaVicUnit4();

	m_bRunTashaUnit1 = m_dongle.RunTashaUnit1();
	m_bRunTashaUnit2 = m_dongle.RunTashaUnit2();
	m_bRunTashaUnit3 = m_dongle.RunTashaUnit3();
	m_bRunTashaUnit4 = m_dongle.RunTashaUnit4();

	m_bRestrictedTo8CamerasPerBoard = m_dongle.RestrictedTo8CamerasPerBoard();

	// links
	m_bRunISDNUnit = m_dongle.RunISDNUnit();
	m_bRunISDNUnit2 = m_dongle.RunISDNUnit2();
	m_bRunSocketUnit = m_dongle.RunSocketUnit();
	m_bAllowRouting = m_dongle.AllowRouting();
	m_bRunSMSUnit = m_dongle.RunSMSUnit();
	m_bRunEMailUnit = m_dongle.RunEMailUnit();
	m_bRunFAXUnit = m_dongle.RunFAXUnit();
	m_bRunHTTP = m_dongle.RunHTTP();

	// boxes
	m_bRunPTUnit = m_dongle.RunPTUnit();
	m_bRunPTUnit2 = m_dongle.RunPTUnit2();
	m_bRunVCSUnit = m_dongle.RunVCSUnit();
	m_bRunTOBSUnit = m_dongle.RunTOBSUnit();
	
	// misc units
	m_bRunGAUnit = m_dongle.RunGAUnit();
	m_bRunSDIUnit = m_dongle.RunSDIUnit();
	m_bRunSimUnit = m_dongle.RunSimUnit();
	m_bRunGemosUnit = m_dongle.RunGemosUnit();
	m_bRunAudioUnit = m_dongle.RunAudioUnit();
	m_bRunICPCONUnit = m_dongle.RunICPCONUnit();
	m_bRunUSBCameraUnit = m_dongle.RunUSBCameraUnit();

	// misc
	m_bRunSecAnalyzer = m_dongle.RunSecAnalyzer();
	m_bRunSDIConfig = m_dongle.RunSDIConfig();

	// functionalities
	m_bAllowSoftDecodePT = m_dongle.AllowSoftDecodePT();
	m_bAllowLocalProcesses = m_dongle.AllowLocalProcesses();
	m_bAllowMotionDetection = m_dongle.AllowMotionDetection();
	m_bAllowMultiMonitor = m_dongle.AllowMultiMonitor();
	m_bAllowHTMLmaps = m_dongle.AllowHTMLmaps();
	m_bAllowTimers = m_dongle.AllowTimers();

	// special
	m_bAllowInternalAccess = m_dongle.AllowInternalAccess();

	// SDI options
	m_bAllowSDICashSystems = m_dongle.AllowSDICashSystems();
	m_bAllowSDIAccessControl = m_dongle.AllowSDIAccessControl();
	m_bAllowSDIIndustrialEquipment = m_dongle.AllowSDIIndustrialEquipment();
	m_bAllowSDIParkmanagment = m_dongle.AllowSDIParkmanagment();
	
//	m_sFullProductName = m_dongle.CalcProductName();	// the noSave text
	m_sFullVersionText = m_dongle.GetVersionString();	// the noSave text

	m_iNoOfHosts = m_dongle.GetNrOfHosts();
	m_iNrOfCameras = m_dongle.GetNrOfCameras();
	m_iNrOfIPCameras = m_dongle.GetNrOfIPCameras();
}
/////////////////////////////////////////////////////////////////////////////
#define SetConstField(x,bInit) \
	m_b##x = bInit; \
	m_cbt##x.EnableWindow(bInit);
/////////////////////////////////////////////////////////////////////////////
#define SetOptionalField(x,bInit) \
	m_b##x = bInit; \
	m_cbt##x.EnableWindow(TRUE);
/////////////////////////////////////////////////////////////////////////////
#define NO_SERVER	\
	m_bRunServer=FALSE; \
	m_cbtRunServer.EnableWindow(FALSE);
/////////////////////////////////////////////////////////////////////////////
#define RUN_SERVER	\
	m_bRunServer=TRUE; \
	m_cbtRunServer.EnableWindow();
/////////////////////////////////////////////////////////////////////////////
#define NO_LINK \
	SetConstField(RunISDNUnit,FALSE);  \
	SetConstField(RunISDNUnit2,FALSE); \
	SetConstField(RunSocketUnit,FALSE);\
	SetConstField(RunSMSUnit,FALSE);   \
	SetConstField(AllowRouting,FALSE); \
	SetConstField(RunEMailUnit,FALSE); \
	SetConstField(RunFAXUnit,FALSE);   \
/////////////////////////////////////////////////////////////////////////////
#define SOME_LINK \
	m_cbtRunISDNUnit.EnableWindow(); \
	m_cbtRunISDNUnit2.EnableWindow(); \
	m_cbtRunSMSUnit.EnableWindow(); \
	m_cbtRunSocketUnit.EnableWindow(); \
	m_cbtRunEMailUnit.EnableWindow(); \
	m_cbtRunFAXUnit.EnableWindow(); \
/////////////////////////////////////////////////////////////////////////////
#define NO_BOXES \
	SetConstField(RunPTUnit,FALSE); \
	SetConstField(RunTOBSUnit,FALSE); \
	SetConstField(RunPTUnit2,FALSE); \
	SetConstField(RunVCSUnit,FALSE); 
/////////////////////////////////////////////////////////////////////////////
#define SOME_BOXES \
	m_cbtRunPTUnit.EnableWindow(TRUE); \
	m_cbtRunTOBSUnit.EnableWindow(TRUE); \
	m_cbtRunPTUnit2.EnableWindow(TRUE); \
	m_cbtRunVCSUnit.EnableWindow(TRUE);
/////////////////////////////////////////////////////////////////////////////
#define NO_PROCESSES SetConstField(AllowLocalProcesses,FALSE);
/////////////////////////////////////////////////////////////////////////////
#define ALLOW_PROCESSES SetConstField(AllowLocalProcesses,TRUE);
/////////////////////////////////////////////////////////////////////////////
#define NO_SOFT_DECODE_MICO SetConstField(AllowSoftDecodeMiCo,FALSE);
/////////////////////////////////////////////////////////////////////////////
#define ALLOW_SOFT_DECODE_MICO SetConstField(AllowSoftDecodeMiCo,TRUE);
/////////////////////////////////////////////////////////////////////////////
#define NO_MOTION_DETECTION SetConstField(AllowMotionDetection,FALSE);
/////////////////////////////////////////////////////////////////////////////
#define ALLOW_MOTION_DETECTION SetConstField(AllowMotionDetection,TRUE);
/////////////////////////////////////////////////////////////////////////////
#define NO_SB \
	SetConstField(RunGAUnit,FALSE); \
	SetConstField(RunSDIUnit,FALSE); \
/////////////////////////////////////////////////////////////////////////////
#define SOME_SB \
	SetOptionalField(RunGAUnit,m_bRunGAUnit); \
	SetOptionalField(AllowSDICashSystems,m_bRunSDIUnit && m_bAllowSDICashSystems); \
	SetOptionalField(AllowSDIAccessControl,m_bRunSDIUnit && m_bAllowSDIAccessControl); \
	SetOptionalField(AllowSDIParkmanagment,m_bRunSDIUnit && m_bAllowSDIParkmanagment); \
	SetConstField(AllowSDIIndustrialEquipment,FALSE);
/////////////////////////////////////////////////////////////////////////////
#define SOME_GEMOS SetOptionalField(RunGemosUnit,m_bRunGemosUnit);
/////////////////////////////////////////////////////////////////////////////
#define NO_GEMOS SetConstField(RunGemosUnit,FALSE);
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::NoQ()
{
	SetConstField(RunQUnit,FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::NoIP()
{
	SetConstField(RunIPCameraUnit,FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::NoJacob()
{
	SetConstField(RunJaCobUnit1,FALSE);
	SetConstField(RunJaCobUnit2,FALSE);
	SetConstField(RunJaCobUnit3,FALSE);
	SetConstField(RunJaCobUnit4,FALSE);
//	SetConstField(AllowMotionDetection,FALSE); // maybe SaVic
}
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::NoSavic()
{
	SetConstField(RunSaVicUnit1,FALSE);
	SetConstField(RunSaVicUnit2,FALSE);
	SetConstField(RunSaVicUnit3,FALSE);
	SetConstField(RunSaVicUnit4,FALSE);
//	SetConstField(AllowMotionDetection,FALSE); // maybe JaCob
}
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::NoTasha()
{
	SetConstField(RunTashaUnit1,FALSE);
	SetConstField(RunTashaUnit2,FALSE);
	SetConstField(RunTashaUnit3,FALSE);
	SetConstField(RunTashaUnit4,FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::NoMicoNorJacob()
{
	NoJacob();
}
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::NoCameraUnits()
{
	NoJacob();
	NoSavic();
	NoTasha();
	NoQ();
	SetConstField(AllowMotionDetection,FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::InitConstraints() 
{
	// enable all windows, OEMs hide window which they don't need
	for (int i=0;i<m_allButtons.GetSize();i++) {
		CButton *pButton = (CButton *)m_allButtons[i];
		pButton->EnableWindow();
	}

	UpdateMembersFromDialog();

	InternalProductCode ipc = GetCurrentProduct();
	WORD wCurrentOemCode = GetCurrentOEMCode();

	if (wCurrentOemCode !=CWK_Dongle::OEM_AKUBIS) {
		SetOptionalField(RunCDRWriter,FALSE);
		switch (ipc) {
			case IPC_STORAGE: 
				SetOptionalField(RunSDIConfig,FALSE);
				SetOptionalField(RunSimUnit,FALSE);
				SetOptionalField(RunSecAnalyzer,FALSE);
				break;
			case IPC_STORAGE_PLUS:
				SetOptionalField(RunSDIConfig,TRUE);
				SetOptionalField(RunSimUnit,TRUE);
				SetOptionalField(RunSecAnalyzer,TRUE);
				break;
			case IPC_ATM_SURVEILENCE:
				if (!m_bRunJaCobUnit1)
				{
					SetConstField(RunJaCobUnit1,TRUE);
				}
				break;
		}	// end of ipc switch
	}

	UpdateDialogFromMembers();
}
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::HideAllWindows(BOOL bHide)
{
	int iCode;
	if (bHide)
	{
		iCode = SW_HIDE;
	}
	else
	{
		iCode= SW_NORMAL;
	}

	for (int i=0;i<m_allButtons.GetSize();i++)
	{
		CButton *pButton = (CButton *)m_allButtons[i];
		pButton->ShowWindow(iCode);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::UpdateConstraints() 
{
	// special NoSave reading all values from the dongle
	if (!theApp.IsSaveEnabled()) 
	{
		SetFromDongle();	// simply undo user interaction, by re-reading the dongle
		UpdateDialogFromMembers();
		return;	// EXIT
	}

	UpdateMembersFromDialog();

	WORD wCurrentOemCode = (WORD) GetCurrentOEMCode();

	if (   (wCurrentOemCode==CWK_Dongle::OEM_IDIP)
		|| (wCurrentOemCode==CWK_Dongle::OEM_SANTEC)
		|| (wCurrentOemCode==CWK_Dongle::OEM_ALARMCOM)
		|| (wCurrentOemCode==CWK_Dongle::OEM_DRESEARCH)
		|| (wCurrentOemCode==CWK_Dongle::OEM_SIEMENS)
		) 
	{
		UpdateConstraintsIdip();
	} 
	else if (wCurrentOemCode==CWK_Dongle::OEM_AKUBIS)	
	{
		// no special rules
	} 
	else 
	{
		// NEWOEM
		// unknown OEM Code
		CString sMsg;
		sMsg.Format(IDS_OEM_ERROR, (int)GetCurrentOEMCode());
		AfxMessageBox(sMsg);
	}
	
	UpdateFunctionalityConstraints();

	UpdateDialogFromMembers();

	UpdateProductNames();
}
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::UpdateConstraintsIdip()
{
	InternalProductCode ipc = GetCurrentProduct();

	// idip
	// default state enabled
	RUN_SERVER;
	ALLOW_PROCESSES;
	SOME_GEMOS;
	SetConstField(RunSDIConfig,FALSE);
	SetConstField(RunSimUnit,TRUE);
	SetConstField(RunSecAnalyzer,TRUE);

	// new idip products IPC_IDIP_xxx
	if (IsNewIdipGeneration(ipc))
	{
		if (ipc == IPC_IDIP_4)
		{
			SetConstField(RunSaVicUnit1,TRUE);
			SetConstField(RunSaVicUnit2,FALSE);
		}
		else if (ipc == IPC_IDIP_PROJECT)
		{
			SomeCameraUnits();
		}
		else
		{
			SetConstField(RunJaCobUnit1,TRUE);
		}
		SOME_LINK;
		SetConstField(RunSocketUnit,TRUE);
		if (ipc!=IPC_IDIP_PROJECT)
		{
			NO_BOXES;
		}
		SetConstField(RunAKUUnit,FALSE);
//		SetConstField(RunCommUnit,TRUE);
		m_cbtAllowSoftDecodePT.EnableWindow(FALSE);
//		SetConstField(AllowRouting,(m_bRunSocketUnit && m_bRunISDNUnit));
		SetConstField(AllowMotionDetection,TRUE);
		SetConstField(RunHTTP,FALSE);
	}
	
	switch (ipc) 
	{
		case IPC_INSPICIO:
			SetOptionalField(RunIPCameraUnit,m_bRunIPCameraUnit);
			SetOptionalField(RunAKUUnit,m_bRunAKUUnit);
			SetOptionalField(RunSDIUnit,m_bRunSDIUnit);
			NoCameraUnits();
			SetConstField(RunISDNUnit,TRUE);
			SetConstField(RunISDNUnit2,TRUE);
			SetConstField(RunSocketUnit,TRUE);
			SetConstField(RunSMSUnit,TRUE);
			SetConstField(AllowRouting,TRUE);
			SetConstField(RunEMailUnit,TRUE);
			SetConstField(RunVCSUnit,FALSE);
			SetOptionalField(RunPTUnit,m_bRunPTUnit);
			SetOptionalField(RunPTUnit2,m_bRunPTUnit2);
			SetConstField(AllowSoftDecodePT,m_bRunPTUnit);
			break;
		case IPC_INSPECTUS:
			SetOptionalField(RunIPCameraUnit,m_bRunIPCameraUnit);
			SetOptionalField(RunAKUUnit,m_bRunAKUUnit);
			SetOptionalField(RunSDIUnit,m_bRunSDIUnit);
			NoCameraUnits();
			SetConstField(RunISDNUnit,TRUE);
			SetConstField(RunISDNUnit2,TRUE);
			SetConstField(RunSocketUnit,TRUE);
			SetConstField(RunSMSUnit,TRUE);
			SetConstField(AllowRouting,TRUE);
			SetConstField(RunEMailUnit,TRUE);
			SetConstField(RunVCSUnit,FALSE);
			SetOptionalField(RunPTUnit,m_bRunPTUnit);
			SetOptionalField(RunPTUnit2,m_bRunPTUnit2);
			SetConstField(AllowSoftDecodePT,m_bRunPTUnit);
			break;
		case IPC_DTS_RECEIVER:
			NO_SERVER;
			NoCameraUnits();
			NO_SB;
			NO_BOXES
			SetConstField(RunTOBSUnit,FALSE);
			SetConstField(RunAKUUnit,FALSE);
			SetConstField(RunCommUnit,FALSE);
			SetConstField(RunGemosUnit,FALSE);
			SetConstField(RunSimUnit,FALSE);
			SetConstField(AllowSoftDecodePT,FALSE);
			break;
		case IPC_IDIP_4:
			NoJacob();
			NoTasha();
			NoQ();
			SetConstField(RunSaVicUnit2,FALSE);
			SetConstField(RunSaVicUnit3,FALSE);
			SetConstField(RunSaVicUnit4,FALSE);
			SetOptionalField(RestrictedTo8CamerasPerBoard,FALSE);
			break;
		case IPC_IDIP_8:
			NoSavic();
			NoTasha();
			NoQ();
			SetConstField(RunJaCobUnit2,FALSE);
			SetConstField(RunJaCobUnit3,FALSE);
			SetConstField(RunJaCobUnit4,FALSE);
			SetConstField(RestrictedTo8CamerasPerBoard,TRUE);
			break;
		case IPC_IDIP_16:
			NoSavic();
			NoTasha();
			NoQ();
			SetConstField(RunJaCobUnit2,TRUE);
			SetConstField(RunJaCobUnit3,FALSE);
			SetConstField(RunJaCobUnit4,FALSE);
			SetConstField(RestrictedTo8CamerasPerBoard,TRUE);
			break;
		case IPC_IDIP_24:
			NoSavic();
			NoTasha();
			NoQ();
			SetOptionalField(RunJaCobUnit2,TRUE);
			SetOptionalField(RunJaCobUnit3,TRUE);
			SetConstField(RunJaCobUnit4,FALSE);
			SetOptionalField(RestrictedTo8CamerasPerBoard,TRUE);
			break;
		case IPC_IDIP_32:
			NoSavic();
			NoTasha();
			NoQ();
			SetConstField(RunJaCobUnit2,TRUE);
			SetConstField(RunJaCobUnit3,TRUE);
			SetConstField(RunJaCobUnit4,TRUE);
			SetConstField(RestrictedTo8CamerasPerBoard,TRUE);
			break;
		case IPC_IDIP_PROJECT:
			SetConstField(RunVCSUnit,FALSE);
			SetOptionalField(RunPTUnit,m_bRunPTUnit);
			SetOptionalField(RunPTUnit2,m_bRunPTUnit2);
			SetConstField(AllowSoftDecodePT,m_bRunPTUnit);
			SomeSavic();
			break;
		case IPC_IDIP_XL_4:
			NoSavic();
			NoJacob();
			NoQ();
			SetConstField(RunTashaUnit1,TRUE);
			SetConstField(RunTashaUnit2,FALSE);
			SetConstField(RunTashaUnit3,FALSE);
			SetConstField(RunTashaUnit4,FALSE);
			SetConstField(RestrictedTo8CamerasPerBoard,TRUE);
			break;
		case IPC_IDIP_XL_8:
			NoSavic();
			NoJacob();
			NoQ();
			SetConstField(RunTashaUnit1,TRUE);
			SetConstField(RunTashaUnit2,FALSE);
			SetConstField(RunTashaUnit3,FALSE);
			SetConstField(RunTashaUnit4,FALSE);
			SetConstField(RestrictedTo8CamerasPerBoard,FALSE);
			break;
		case IPC_IDIP_XL_16:
			NoSavic();
			NoJacob();
			NoQ();
			SetConstField(RunTashaUnit1,TRUE);
			SetConstField(RunTashaUnit2,TRUE);
			SetConstField(RunTashaUnit3,FALSE);
			SetConstField(RunTashaUnit4,FALSE);
			SetConstField(RestrictedTo8CamerasPerBoard,FALSE);
			break;
		case IPC_IDIP_XL_24:
			NoSavic();
			NoJacob();
			NoQ();
			SetConstField(RunTashaUnit1,TRUE);
			SetConstField(RunTashaUnit2,TRUE);
			SetConstField(RunTashaUnit3,TRUE);
			SetConstField(RunTashaUnit4,FALSE);
			SetConstField(RestrictedTo8CamerasPerBoard,FALSE);
			break;
		case IPC_IDIP_XL_32:
			NoSavic();
			NoJacob();
			NoQ();
			SetConstField(RunTashaUnit1,TRUE);
			SetConstField(RunTashaUnit2,TRUE);
			SetConstField(RunTashaUnit3,TRUE);
			SetConstField(RunTashaUnit4,TRUE);
			SetConstField(RestrictedTo8CamerasPerBoard,FALSE);
			break;
		case IPC_IDIP_ML:
			NoJacob();
			NoSavic();
			NoTasha();
			SetConstField(RunQUnit,TRUE);
			SetConstField(RestrictedTo8CamerasPerBoard,FALSE);
			break;
		case IPC_IDIP_IP:
			NoJacob();
			NoSavic();
			NoTasha();
			NoQ();
			SetConstField(RunIPCameraUnit,TRUE);
			SetConstField(RestrictedTo8CamerasPerBoard,FALSE);
		default:
			WK_TRACE(_T("invalid idip Product Code\n"));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::UpdateFunctionalityConstraints() 
{
	InternalProductCode ipc = GetCurrentProduct();
	// all units need a server
	if (
		// codecs
		   m_bRunQUnit
		|| m_bRunIPCameraUnit

		|| m_bRunJaCobUnit1
		|| m_bRunJaCobUnit2
		|| m_bRunJaCobUnit3
		|| m_bRunJaCobUnit4

		|| m_bRunSaVicUnit1
		|| m_bRunSaVicUnit2
		|| m_bRunSaVicUnit3
		|| m_bRunSaVicUnit4

		|| m_bRunTashaUnit1
		|| m_bRunTashaUnit2
		|| m_bRunTashaUnit3
		|| m_bRunTashaUnit4

		// misc units
		|| m_bRunAKUUnit
		|| m_bRunCommUnit
		|| m_bRunGAUnit
		|| m_bRunSimUnit
		|| m_bRunSDIUnit
		) {
		m_bRunServer = TRUE;
		m_cbtRunServer.EnableWindow(TRUE);
	}

	// JaCob	
	if (m_bRunJaCobUnit1) {
		if(   (ipc != IPC_IDIP_8)
		   && (ipc != IPC_IDIP_16)
		   )
		{
			m_cbtRunJaCobUnit2.EnableWindow(TRUE);
		}
	} else {
		SetConstField(RunJaCobUnit2,FALSE);
//		m_cbtRunJaCobUnit2.EnableWindow(FALSE);
	}
	if (m_bRunJaCobUnit2) {
		if(ipc != IPC_IDIP_32)
		{
			m_cbtRunJaCobUnit3.EnableWindow(TRUE);
		}
	} else {
		SetConstField(RunJaCobUnit3,FALSE);
//		m_cbtRunJaCobUnit3.EnableWindow(FALSE);
	}
	if (m_bRunJaCobUnit3) {
		if(ipc == IPC_IDIP_PROJECT)
		{
			SetConstField(RestrictedTo8CamerasPerBoard,TRUE);
		}
		m_cbtRunJaCobUnit4.EnableWindow(TRUE);
	} else {
		SetConstField(RunJaCobUnit4,FALSE);
//		m_cbtRunJaCobUnit4.EnableWindow(FALSE);
	}

	m_cbNrOfCameras.EnableWindow(m_bRunQUnit);
	m_cbNrOfIPCameras.EnableWindow(m_bRunIPCameraUnit);

	// SaVic	
	if (m_bRunSaVicUnit1) {
		if(   (ipc == IPC_IDIP_4)
		   && (ipc == IPC_IDIP_PROJECT)
		   )
		{
			m_cbtRunSaVicUnit2.EnableWindow(TRUE);
		}
	} else {
		SetConstField(RunSaVicUnit2,FALSE);
	}
	if (m_bRunSaVicUnit2) {
		if(ipc == IPC_IDIP_PROJECT)
		{
			m_cbtRunSaVicUnit3.EnableWindow(TRUE);
		}
	} else {
		SetConstField(RunSaVicUnit3,FALSE);
	}
	if (m_bRunSaVicUnit3) {
		if(ipc == IPC_IDIP_PROJECT)
		{
			m_cbtRunSaVicUnit4.EnableWindow(TRUE);
		}
	} else {
		SetConstField(RunSaVicUnit4,FALSE);
	}

	// Tasha	
	if (m_bRunTashaUnit1) {
		if(ipc == IPC_IDIP_PROJECT)
		{
			m_cbtRunTashaUnit2.EnableWindow(TRUE);
		}
	} else {
		SetConstField(RunTashaUnit2,FALSE);
	}
	if (m_bRunTashaUnit2) {
		if(ipc == IPC_IDIP_PROJECT)
		{
			m_cbtRunTashaUnit3.EnableWindow(TRUE);
		}
	} else {
		SetConstField(RunTashaUnit3,FALSE);
	}
	if (m_bRunTashaUnit3) {
		if(ipc == IPC_IDIP_PROJECT)
		{
			m_cbtRunTashaUnit4.EnableWindow(TRUE);
		}
	} else {
		SetConstField(RunTashaUnit4,FALSE);
	}

	// Link modules
	BOOL bAddCommunicationModules = (   (ipc != IPC_TOBS_RECEIVER)
									 && (ipc != IPC_DTS_RECEIVER)
									 && (ipc != IPC_TOBS_TO3K)
									);
	if (m_bRunISDNUnit || m_bRunSocketUnit)
	{
		SetConstField(RunEMailUnit, bAddCommunicationModules);
	}
	else
	{
		SetConstField(RunEMailUnit,FALSE);
	}

	if (IsNewIdipGeneration(ipc))
	{
		SetConstField(RunISDNUnit2, m_bRunISDNUnit);
		SetConstField(RunSMSUnit, m_bRunISDNUnit);
//		SetConstField(RunFAXUnit, m_bRunISDNUnit);
		SetConstField(RunFAXUnit,FALSE);
	}
	else
	{
		m_cbtRunISDNUnit2.EnableWindow(m_bRunISDNUnit);
		SetConstField(RunSMSUnit, m_bRunISDNUnit && bAddCommunicationModules);
//		SetConstField(RunFAXUnit, m_bRunISDNUnit && bAddCommunicationModules);
		SetConstField(RunFAXUnit,FALSE);
	}

	// Box modules
	if (m_bRunPTUnit) {
		m_cbtRunPTUnit2.EnableWindow(TRUE);
	} else {
		m_bRunPTUnit2 = FALSE;
		m_cbtRunPTUnit2.EnableWindow(FALSE);
	}

	// Misc modules
	m_cbtAllowSDICashSystems.EnableWindow(m_bRunSDIUnit);
	m_cbtAllowSDICashSystems.EnableWindow(m_bRunSDIUnit);
	m_cbtAllowSDIAccessControl.EnableWindow(m_bRunSDIUnit);
	m_cbtAllowSDIIndustrialEquipment.EnableWindow(m_bRunSDIUnit);
	m_cbtAllowSDIParkmanagment.EnableWindow(m_bRunSDIUnit);

	// Misc functionalities
	m_cbtAllowRouting.EnableWindow(m_bRunSocketUnit);
	// Motion detection only with JaCob or SaVic
	m_cbtAllowMotionDetection.EnableWindow(m_bRunJaCobUnit1 || m_bRunSaVicUnit1 || m_bRunTashaUnit1);
}
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::UpdateLabelsForOEM(WORD wOEM) 
{
	CWK_Dongle::OemCode oem = (CWK_Dongle::OemCode)wOEM;

	CString sAKUUnit = COemGuiApi::GetApplicationNameOem(WAI_AKUUNIT,oem);
	CString sBackup  = COemGuiApi::GetApplicationNameOem(WAI_AC_DC,oem);
	CString sCommUnit = COemGuiApi::GetApplicationNameOem(WAI_COMMUNIT,oem);
	CString sSDIUnit = COemGuiApi::GetApplicationNameOem(WAI_SDIUNIT,oem);
	CString sSDIConfig = COemGuiApi::GetApplicationNameOem(WAI_SDICONFIG,oem);
	CString sSimUnit = COemGuiApi::GetApplicationNameOem(WAI_SIMUNIT,oem);
	CString sSecAnalyzer = COemGuiApi::GetApplicationNameOem(WAI_ANALYZER,oem);

	if (wOEM==CWK_Dongle::OEM_AKUBIS)
	{
		sSDIConfig = _T("Kamerasteuerung");
		sSimUnit = _T("2-B-Verbindung");
		sSecAnalyzer = _T("Auswertung");
	}
	else
	{
		// use default names
	}

	m_cbtRunAKUUnit.SetWindowText(sAKUUnit);
	m_cbtRunCDRWriter.SetWindowText(sBackup);
	m_cbtRunCommUnit.SetWindowText(sCommUnit);
	m_cbtRunSecAnalyzer.SetWindowText(sSecAnalyzer);
	m_cbtRunSDIUnit.SetWindowText(sSDIUnit);
	m_cbtRunSDIConfig.SetWindowText(sSDIConfig );
	m_cbtRunSimUnit.SetWindowText(sSimUnit);
	m_cbtRunSecAnalyzer.SetWindowText(sSecAnalyzer);
}
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::UpdateVisibilityForOEM(WORD wOEM) 
{
	// default is to show all buttons
	HideAllWindows(FALSE);	// show all, the different OEM will hide, what they don't need

	// AKUBIS
	if (wOEM==CWK_Dongle::OEM_AKUBIS)
	{
		HideAllWindows(TRUE);	// hide all and show the three which are used
		// and enable the three used ones
		m_cbtRunSDIConfig.ShowWindow(SW_NORMAL);
		m_cbtRunSecAnalyzer.ShowWindow(SW_NORMAL);
		m_cbtRunSimUnit.ShowWindow(SW_NORMAL);
	}
	else
	{
		// only for DLR
		m_cbtRunGAUnit.ShowWindow(SW_HIDE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::SomeCameraUnits()
{
	// No one selected
	if (!m_bRunJaCobUnit1 && 
		!m_bRunSaVicUnit1 &&
		!m_bRunTashaUnit1 &&
		!m_bRunQUnit)
	{
		// check which was just chosen
		switch (m_eToggleCodecs)
		{
			case eToggleCodecToJacob:
				m_bRunJaCobUnit1 = TRUE;
				break;
			case eToggleCodecToSavic:
				m_bRunSaVicUnit1 = TRUE;
				break;
			case eToggleCodecToTasha:
				m_bRunTashaUnit1 = TRUE;
				break;
			case eToggleCodecToQ:
				m_bRunQUnit = TRUE;
				break;
			default:
				// ERROR, take JaCob only as default
				m_bRunJaCobUnit1 = TRUE;
				break;
		}
		m_eToggleCodecs = eToggleCodecUnknown;
	}

	if (m_bRunJaCobUnit1)
	{
		SetOptionalField(RunJaCobUnit1,m_bRunJaCobUnit1);
		SetOptionalField(RunJaCobUnit2,m_bRunJaCobUnit1 && m_bRunJaCobUnit2);
		SetOptionalField(RunJaCobUnit3,m_bRunJaCobUnit2 && m_bRunJaCobUnit3);
		SetOptionalField(RunJaCobUnit4,m_bRunJaCobUnit2 && m_bRunJaCobUnit3 && m_bRunJaCobUnit4);
		SetOptionalField(RunSaVicUnit1,m_bRunSaVicUnit1);
		SetOptionalField(RunTashaUnit1,m_bRunTashaUnit1);
		SetOptionalField(RunQUnit,m_bRunQUnit);
		SetConstField(AllowMotionDetection,TRUE);
	}
	else if (m_bRunSaVicUnit1)
	{
		SetOptionalField(RunSaVicUnit1,m_bRunSaVicUnit1);
		SetOptionalField(RunSaVicUnit2,m_bRunSaVicUnit1 && m_bRunSaVicUnit2);
		SetOptionalField(RunSaVicUnit3,m_bRunSaVicUnit2 && m_bRunSaVicUnit3);
		SetOptionalField(RunSaVicUnit4,m_bRunSaVicUnit2 && m_bRunSaVicUnit3 && m_bRunSaVicUnit4);
		SetOptionalField(RunJaCobUnit1,m_bRunJaCobUnit1);
		SetOptionalField(RunTashaUnit1,m_bRunTashaUnit1);
		SetOptionalField(RunQUnit,m_bRunQUnit);
		SetConstField(AllowMotionDetection,TRUE);
	}
	else if (m_bRunTashaUnit1)
	{
		SetOptionalField(RunTashaUnit1,m_bRunTashaUnit1);
		SetOptionalField(RunTashaUnit2,m_bRunTashaUnit1 && m_bRunTashaUnit2);
		SetOptionalField(RunTashaUnit3,m_bRunTashaUnit2 && m_bRunTashaUnit3);
		SetOptionalField(RunTashaUnit4,m_bRunTashaUnit2 && m_bRunTashaUnit3 && m_bRunTashaUnit4);
		SetOptionalField(RunSaVicUnit1,m_bRunSaVicUnit1);
		SetOptionalField(RunJaCobUnit1,m_bRunJaCobUnit1);
		SetOptionalField(RunQUnit,m_bRunQUnit);
		SetConstField(AllowMotionDetection,TRUE);
	}
	else if (m_bRunQUnit)
	{
		SetOptionalField(RunQUnit,m_bRunQUnit);
		SetOptionalField(RunSaVicUnit1,m_bRunSaVicUnit1);
		SetOptionalField(RunJaCobUnit1,m_bRunJaCobUnit1);
		SetOptionalField(RunTashaUnit1,m_bRunTashaUnit1);
		SetConstField(AllowMotionDetection,TRUE);
	}
	else
	{
		// should not possible, but...
		ASSERT(0);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::SomeSavic()
{
	SetOptionalField(RunSaVicUnit1,m_bRunSaVicUnit1);
	SetOptionalField(RunSaVicUnit2,m_bRunSaVicUnit1 && m_bRunSaVicUnit2);
	SetOptionalField(RunSaVicUnit3,m_bRunSaVicUnit2 && m_bRunSaVicUnit3);
	SetOptionalField(RunSaVicUnit4,m_bRunSaVicUnit2 && m_bRunSaVicUnit3 && m_bRunSaVicUnit4);
	SetConstField(AllowMotionDetection,TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::SomeIP()
{
	SetOptionalField(RunIPCameraUnit,m_bRunIPCameraUnit);
}
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::SetValidProduct(InternalProductCode ipc)
{
	BOOL bFound = SetCurSelByData (m_cbProduct, ipc);
	if (!bFound)
	{
		if (   theApp.IsCryptEnabled()
			&& (IsNewIdipGeneration(ipc) == FALSE)
			)
		{
			// Find a similar one
			SetCurSelByData (m_cbProduct, IPC_IDIP_PROJECT);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
// check for special rules
// if it is not according to a rule, a message box complains
// and FALSE is returned
BOOL CProductViewDlg::PreCheckSave()
{
	BOOL bOkay=TRUE;
	InternalProductCode ipc = GetCurrentProduct();

	// valid IP`?
	if (   (ipc < IPC_MIN_CODE)
		|| (ipc > IPC_MAX_CODE)
		)
	{
		AfxMessageBox(IDP_NO_VALID_PRODUCT);
		bOkay=FALSE;
	}
	else
	{
		// Regel 1)  Sta,Sto,Pro haben mindestens eins von ISDNUnit/Net
		if (   GetCurrentOEMCode()==CWK_Dongle::OEM_IDIP
			|| GetCurrentOEMCode()==CWK_Dongle::OEM_SANTEC
			|| GetCurrentOEMCode()==CWK_Dongle::OEM_ALARMCOM
			|| GetCurrentOEMCode()==CWK_Dongle::OEM_DRESEARCH
			|| GetCurrentOEMCode()==CWK_Dongle::OEM_SIEMENS
			) 
		{
			switch (ipc) 
			{
				case IPC_STORAGE_TRANSMITTER:
				case IPC_INSPICIO:
				case IPC_INSPECTUS:
					if (m_dongle.RunAnyLinkUnit()==FALSE) {
						AfxMessageBox(IDP_ISDN_OR_SOCKET);
						bOkay=FALSE;
					}
					break;
			}
		} 
		else if (GetCurrentOEMCode()==CWK_Dongle::OEM_AKUBIS) 
		{
			// no special conditions
		} 
		else 
		{
			// unknown OEMCode
			CString sMsg;
			sMsg.Format(IDS_OEM_ERROR, (int)GetCurrentOEMCode());
			AfxMessageBox(sMsg);
		}
	}
	return bOkay;
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CProductViewDlg, CDialog)
	//{{AFX_MSG_MAP(CProductViewDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_DO_SAVE, OnDoSave)
	ON_CBN_SELCHANGE(IDC_PRODUCT_NAME, OnSelchangeProductName)
	ON_CBN_SELCHANGE(IDC_COMBO_NR_CAMERAS, OnSelchangeNrOfCameras)
	ON_BN_CLICKED(IDC_RUN_IP_UNIT, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_QUNIT, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_ISDN_UNIT, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_PRESENCE_UNIT, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_TOBS_UNIT, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_SERVER, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_SOCKET_UNIT, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_VCS_UNIT, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_AKU_UNIT, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_ANALYZER, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_CDR_WRITER, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_COMM_UNIT, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_GAA_UNIT, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_SDI_UNIT, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_SIM_UNIT, UpdateConstraints)
	ON_BN_CLICKED(IDC_ALLOW_LOCAL_STORAGE_PROCESSES, UpdateConstraints)
	ON_BN_CLICKED(IDC_ALLOW_SOFT_DECODE_MICO, UpdateConstraints)
	ON_CBN_SELCHANGE(IDC_OEM_CODE, OnSelchangeOemCode)
	ON_BN_CLICKED(IDC_RUN_SDI_CONFIG, UpdateConstraints)
	ON_BN_CLICKED(IDC_ALLOW_INTERNAL_ACCESS, UpdateConstraints)
	ON_BN_CLICKED(IDC_ALLOW_MULTI_MONITOR, UpdateConstraints)
	ON_BN_CLICKED(IDC_ALLOW_HTML_MAPS, UpdateConstraints)
	ON_BN_CLICKED(IDC_ALLOW_TIMERS, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_MICO2, UpdateConstraints)
	ON_BN_CLICKED(IDC_ALLOW_ROUTING, UpdateConstraints)
	ON_BN_CLICKED(IDC_ALLOW_SDI_GA, UpdateConstraints)
	ON_BN_CLICKED(IDC_ALLOW_SDI_INDUSTRIAL, UpdateConstraints)
	ON_BN_CLICKED(IDC_ALLOW_SDI_ACCESS_CONTROL, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_PRESENCE_UNIT2, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_ISDN_UNIT2, UpdateConstraints)
	ON_BN_CLICKED(IDC_ALLOW_SOFT_DECODE_PT, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_MICO3, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_MICO4, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_SMS, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_JACOB1, OnRunJacob1)
	ON_BN_CLICKED(IDC_RUN_JACOB2, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_JACOB3, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_JACOB4, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_GEMOS_UNIT, UpdateConstraints)
	ON_BN_CLICKED(IDC_ALLOW_MOTION_DETECTION, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_EMAIL_UNIT, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_FAX_UNIT, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_HTTP, UpdateConstraints)
	ON_BN_CLICKED(IDC_CRYPT, OnCrypt)
	ON_BN_CLICKED(IDC_RESTRICTED_TO_8_CAMERAS_PER_BOARD, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_SAVIC1, OnRunSavic1)
	ON_BN_CLICKED(IDC_RUN_SAVIC2, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_SAVIC3, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_SAVIC4, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_TASHA1, OnRunTasha1)
	ON_BN_CLICKED(IDC_RUN_TASHA2, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_TASHA3, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_TASHA4, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_ICPCON_UNIT, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_USB_CAMERA, UpdateConstraints)
	ON_BN_CLICKED(IDC_RUN_AUDIO_UNIT, UpdateConstraints)
	ON_BN_CLICKED(IDC_RADIO_SERIAL, OnRadioSerialOrHddID)
	ON_BN_CLICKED(IDC_RADIO_HDD, OnRadioSerialOrHddID)
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_NO_OF_HOSTS, OnDeltaposSpinNoOfHosts)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CProductViewDlg message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CProductViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(theApp.m_pszAppName);
	m_spinNoOfHosts.SetRange32(0, 15);

	if (theApp.GetFileToUse().GetLength()) 
	{
		CString sTitle=theApp.GetFileToUse();
		while (sTitle.GetLength()>40) 
		{
			sTitle = sTitle.Right(sTitle.GetLength()-1);
		}
		SetWindowText(sTitle);
	}

	if (theApp.IsCryptEnabled())
	{
		m_ctFullProductName.ShowWindow(SW_HIDE);
		m_ctFullVersion.ShowWindow(SW_HIDE);
		m_ctlVersion.ShowWindow(SW_HIDE);
		m_ctlSerial.ShowWindow(SW_HIDE);
		m_ctlSerialLabel.ShowWindow(SW_HIDE);

		m_buttonSave.ShowWindow(SW_HIDE);
		m_buttonCrypt.ShowWindow(SW_SHOW);

		m_btnSerial.ShowWindow(SW_SHOW);
		m_editSerial.ShowWindow(SW_SHOW);
		m_btnHDD.ShowWindow(SW_SHOW);
		m_editHDD1.ShowWindow(SW_SHOW);
		m_staticHDD2.ShowWindow(SW_SHOW);
		m_editHDD2.ShowWindow(SW_SHOW);
		if (m_iSerialOrHddID == 0)
		{
			m_editSerial.EnableWindow(TRUE);
			m_editHDD1.EnableWindow(FALSE);
			m_editHDD2.EnableWindow(FALSE);
		}
		else
		{
			m_editSerial.EnableWindow(FALSE);
			m_editHDD1.EnableWindow(TRUE);
			m_editHDD2.EnableWindow(TRUE);
		}

		CRSA rsa;
		CString sSerialHigh,sSerialLow;
		sSerialHigh.Format(_T("%04hx"),rsa.GetSerialHigh());
		sSerialLow.Format(_T("%04hx"),rsa.GetSerialLow());
		sSerialHigh.MakeUpper();
		sSerialLow.MakeUpper();

		m_editHDD1.SetWindowText(sSerialHigh);
		m_editHDD2.SetWindowText(sSerialLow);
	}
	else if (theApp.IsSaveEnabled()) 
	{
		// Save is enabled
		// m_buttonSave
		// m_cbProduct
		// m_cbOEMCode
		// m_cbtAllowInternalAccess
		m_ctFullProductName.ShowWindow(SW_HIDE);
		m_ctFullVersion.ShowWindow(SW_HIDE);
		m_ctlSerial.ShowWindow(SW_HIDE);
		m_ctlSerialLabel.ShowWindow(SW_HIDE);
	} 
	else 
	{
		// no save
		// hide the configuration buttons
		m_buttonSave.ShowWindow(SW_HIDE);
		m_cbProduct.ShowWindow(SW_HIDE);
		m_cbOEMCode.ShowWindow(SW_HIDE);

		// m_ctFullVersion
		// m_ctFullProductName
#ifndef _DEBUG
		m_cbtRunUSBCameraUnit.ShowWindow(SW_HIDE);
#endif

		// check for "Serial.snt"

		CStdioFile f;
		BOOL bOkay = f.Open(_T("Serial.snt"),CFile::modeRead);
		if (bOkay) {
			CString sFirstLine;
			bOkay = f.ReadString(sFirstLine);
			if (bOkay) {
				m_sSerial = sFirstLine;
				m_ctlSerial.ShowWindow(SW_NORMAL);
				m_ctlSerialLabel.ShowWindow(SW_NORMAL);
			} else {
				m_sSerial = _T("???");	// OOPS
			}
		} else {
			// "Serial.snt" not found
			m_ctlSerial.ShowWindow(SW_HIDE);
			m_ctlSerialLabel.ShowWindow(SW_HIDE);
		}

		
		if (m_bAllowInternalAccess==FALSE) {
			// only show if set
			m_cbtAllowInternalAccess.ShowWindow(SW_HIDE);
		}
	}

	// CAVEAT order is important, InitProductName depends from the selected OEMCode
	m_cbOEMCode.ResetContent();
	
	int ix = m_cbOEMCode.AddString(COemGuiApi::GetOemName(CWK_Dongle::OEM_IDIP));
	m_cbOEMCode.SetItemData(ix, (WORD)CWK_Dongle::OEM_IDIP);

	ix = m_cbOEMCode.AddString(COemGuiApi::GetOemName(CWK_Dongle::OEM_ALARMCOM));
	m_cbOEMCode.SetItemData(ix,(WORD)CWK_Dongle::OEM_ALARMCOM);

	// NEWOEM

	m_cbNrOfCameras.ResetContent();
	m_cbNrOfCameras.AddString(_T("4"));
	m_cbNrOfCameras.AddString(_T("8"));
	m_cbNrOfCameras.AddString(_T("16"));
	m_cbNrOfCameras.AddString(_T("24"));
	m_cbNrOfCameras.AddString(_T("32"));

	m_cbNrOfIPCameras.ResetContent();
	m_cbNrOfIPCameras.AddString(_T("0"));
	m_cbNrOfIPCameras.AddString(_T("4"));
	m_cbNrOfIPCameras.AddString(_T("8"));
	m_cbNrOfIPCameras.AddString(_T("16"));
	m_cbNrOfIPCameras.AddString(_T("24"));
	m_cbNrOfIPCameras.AddString(_T("32"));

	BOOL bFound = SetCurSelByData(m_cbOEMCode,(WORD) m_dongle.GetOemCode());
	if (bFound==FALSE) 
	{
		WK_TRACE_ERROR(_T("OEMCode %d not found ?\n"),(WORD) m_dongle.GetOemCode());
		SetCurSelByData( m_cbOEMCode,(WORD)1);
	}

	// CAVEAT order is important, InitProductName depends from the selected OEMCode
	InitProductNames();

	// activate the selected product or a similar one
	SetValidProduct(m_dongle.GetProductCode());

	// initial update
	UpdateVisibilityForOEM(GetCurrentOEMCode());
	UpdateLabelsForOEM(GetCurrentOEMCode());
	UpdateConstraints();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	::SetWindowPos(m_hWnd, HWND_TOPMOST, NULL, NULL, NULL, NULL, SWP_NOSIZE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		COemGuiApi::AboutDialog(this);
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}
/////////////////////////////////////////////////////////////////////////////
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CProductViewDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}
/////////////////////////////////////////////////////////////////////////////
// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CProductViewDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::OnCrypt() 
{
	UpdateMembersFromDialog();
	m_dongle.TransferSettings(this);	// fill dongle subclass from dialog setting

	if (PreCheckSave()==FALSE) 
	{
		return;	// <<< EXIT >>>
	}

	CString sDcf;
	CString sCry = _T("c:\\security\\dongle.cry");

	CRSA rsa;
	CString sSerialHigh,sSerialLow;
	DWORD dwSerialHigh,dwSerialLow;

	if (m_iSerialOrHddID == 0)
	{
		CString sSerialNumber;
		m_editSerial.GetWindowText(sSerialNumber);
		CryptHDDIDFromSerial(sSerialNumber, sSerialHigh, sSerialLow);
	}
	else
	{
		m_editHDD1.GetWindowText(sSerialHigh);
		m_editHDD2.GetWindowText(sSerialLow);
	}

	_stscanf(sSerialHigh,_T("%x"),&dwSerialHigh);
	_stscanf(sSerialLow,_T("%x"),&dwSerialLow);

	if (  (dwSerialHigh != (DWORD)rsa.GetSerialHigh())
		|| (dwSerialLow  != (DWORD)rsa.GetSerialLow())
		)
	{
		CString sMsg;
		sMsg.LoadString(IDP_SERIAL_DIFFERENT);
		sMsg += _T("\n\n") + sSerialHigh + _T(" - ") + sSerialLow;
		if (IDYES==AfxMessageBox(sMsg,MB_YESNO))
		{
			rsa.Init((WORD)dwSerialHigh, (WORD)dwSerialLow);
		}
		else
		{
			return;
		}

		// prompt for file name!
		TCHAR szFilter[] = _T("Dongle Crypt Files (*.cry)|*.cry||");
		CFileDialog dlg(FALSE, _T("*.cry"), sDcf, 0, szFilter);
		if (dlg.DoModal()==IDOK) 
		{
			sCry = dlg.GetFileName();
		}
		else
		{
			return;
		}
	}

	sDcf = sCry.Left(sCry.ReverseFind(_T('.'))) + _T(".dcf");

	if (m_dongle.WriteDCF(sDcf))
	{
		if (rsa.IsValid())
		{
			if (rsa.Encode(sDcf,sCry))
			{
				AfxMessageBox(IDS_CRYPT_OK);
			}
			else
			{
				AfxMessageBox(IDS_CRYPT_ERROR);
			}
		}
		else
		{
			AfxMessageBox(IDS_CRYPT_ERROR);
		}
	}
	else
	{
		AfxMessageBox(IDS_CRYPT_ERROR);
	}

#ifndef _DEBUG
	DeleteFile(sDcf);
#endif
}
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::OnDoSave() 
{
	UpdateMembersFromDialog();

	m_dongle.TransferSettings(this);	// fill dongle subclass from dialog setting

	if (PreCheckSave()==FALSE) {
		return;	// <<< EXIT >>>
	}

	CString sDefaultFileName(_T("new.dcf"));

	if (theApp.GetFileToUse().GetLength()) {
		sDefaultFileName = theApp.GetFileToUse();
		CString sPath,sFilename;
		WK_SplitPath(
			theApp.GetFileToUse(),
			sPath,
			sDefaultFileName
			);
		// NOT YET chdir or the like
	}

	TCHAR szFilter[] = _T("Dongle Configuration Files (*.dcf)|*.dcf|EEPROM Files (*.eep)|*.eep||");
	CFileDialog myDialog(FALSE, _T("*.dcf"), sDefaultFileName, 0, szFilter);
	if (myDialog.DoModal()==IDOK) {
		CString sName;
		sName = myDialog.GetFileName();

		if (sName.GetLength()) {
			TRACE(_T("Got name %s\n"), sName);

			BOOL bOkay = m_dongle.WriteToFile(sName);
			if (bOkay==FALSE) {
				AfxMessageBox(_T("Write to file failed"));
			}
			m_dongle.WriteToRegistry();
		}
	}
	
}
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::OnSelchangeOemCode() 
{
	ResetAllFlags();
	// NOT YET what should be the initial product ? Could be some standard poduct
	// depending on the oem
	InitProductNames();

	SetValidProduct(IPC_STORAGE);

	UpdateVisibilityForOEM(	GetCurrentOEMCode());
	UpdateLabelsForOEM(GetCurrentOEMCode());

	InitConstraints();
	UpdateConstraints();
}
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::OnSelchangeProductName() 
{
	TRACE(_T("The selected product code is %d\n"),GetCurrentProduct());
	ResetAllFlags();
	InitConstraints();
	UpdateConstraints();
}
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::OnSelchangeNrOfCameras() 
{
	UpdateConstraints();
}
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::OnRunJacob1() 
{
	// Toggle Codecs
	m_eToggleCodecs = eToggleCodecToJacob;
	UpdateConstraints();
}
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::OnRunSavic1() 
{
	m_eToggleCodecs = eToggleCodecToSavic;
	UpdateConstraints();
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::OnRunTasha1() 
{
	m_eToggleCodecs = eToggleCodecToTasha;
	UpdateConstraints();
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
long CProductViewDlg::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
		::PostMessage(hWnd, WM_SELFCHECK, WAI_PRODUCT_VIEW, 0);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::OnRadioSerialOrHddID() 
{
	UpdateData();
	if (theApp.IsCryptEnabled())
	{
		if (m_iSerialOrHddID == 0)
		{
			m_editSerial.EnableWindow(TRUE);
			m_editHDD1.EnableWindow(FALSE);
			m_editHDD2.EnableWindow(FALSE);
		}
		else
		{
			m_editSerial.EnableWindow(FALSE);
			m_editHDD1.EnableWindow(TRUE);
			m_editHDD2.EnableWindow(TRUE);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CProductViewDlg::OnDeltaposSpinNoOfHosts(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if (theApp.IsSaveEnabled()) 
	{
		CDataExchange dx(this, FALSE);
		int nPos = pNMUpDown->iPos;
		m_spinNoOfHosts.SetPos(nPos);
		switch (nPos)
		{
			case 0: m_iNoOfHosts = 0; break;
			case 1: m_iNoOfHosts = 5; break;
			default: m_iNoOfHosts = (nPos-1)*10; break;
		}
		DDX_Text(&dx, IDC_EDT_NO_OF_HOSTS, m_iNoOfHosts);
	}
	*pResult = 0;
}
