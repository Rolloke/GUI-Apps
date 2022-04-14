// SoftwarePage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "USBcamPage.h"
#include <ICPCONDll\ICP7000Module.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define FRAME_CIF 0
#define FRAME_VGA 1

#define ALL_CONTROLLS 2
#define MAX_CAMERAS	8

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CUSBcamPage, CSVPage)
/////////////////////////////////////////////////////////////////////////////
// CUSBcamPage dialog
CUSBcamPage::CUSBcamPage(CSVView*  pParent, int nPage) : CSVPage(CUSBcamPage::IDD)
{
	m_pParent = pParent;
	//{{AFX_DATA_INIT(CUSBcamPage)
	m_bCameraDetectors = FALSE;
	m_bMotionDetection = FALSE;
	m_bUnitActive = FALSE;
	m_nFramesPerSecond = 10;
	m_nFrameSize = FRAME_CIF;
	//}}AFX_DATA_INIT

	m_nPageNo = nPage;

	CWK_Profile &wkpBase = m_pParent->GetDocument()->GetProfile();
	m_sBase    = wkpBase.GetSection() + _T("\\");
	m_bIsLocal = m_pParent->GetDocument()->IsLocal();

	m_sDevice.Format(USB_CAM_UNIT_DEV, m_nPageNo);
	m_sDevice = m_sBase + USB_CAM_UNIT + m_sDevice;
	
	ReadRegistry();

	Create();
}
//////////////////////////////////////////////////////////////////////////
void CUSBcamPage::ReadRegistry()
{
	CWK_Profile wkpDev(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_sDevice, _T(""));
	CWK_Profile &wkpBase = m_pParent->GetDocument()->GetProfile();

	m_bMotionDetection = wkpDev.GetInt(SETTINGS_SECTION, MOTION_DETECTION, m_bMotionDetection);
	m_bCameraDetectors = wkpDev.GetInt(SETTINGS_SECTION, CAMERA_DETECTORS, m_bCameraDetectors);
	int nTemp = wkpDev.GetInt(SETTINGS_SECTION, REF_PICT_RATE, 50);
	m_nFramesPerSecond = 1000 / nTemp;
	nTemp = wkpDev.GetInt(SETTINGS_SECTION, PICT_RES_X, 352);
	if      (nTemp == 352) m_nFrameSize = FRAME_CIF;
	else if (nTemp == 640) m_nFrameSize = FRAME_VGA;

	m_bUnitActive = IsUnitActive(wkpBase, m_nPageNo);
}
//////////////////////////////////////////////////////////////////////////
void CUSBcamPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUSBcamPage)
	DDX_Control(pDX, IDC_LIST_CAMERAS, m_cCameraList);
	DDX_Check(pDX, IDC_CK_USBC_DETECTORS_ACTIVE, m_bCameraDetectors);
	DDX_Check(pDX, IDC_CK_USBC_MD_ACTIVE, m_bMotionDetection);
	DDX_Check(pDX, IDC_CK_USBC_UNIT_ACTIVE, m_bUnitActive);
	DDX_Text(pDX, IDC_EDT_USBC_FRAMES_PER_SECOND, m_nFramesPerSecond);
	DDV_MinMaxInt(pDX, m_nFramesPerSecond, 1, 50);
	DDX_Radio(pDX, IDC_RD_USBC_FS_0, m_nFrameSize);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CUSBcamPage, CSVPage)
	//{{AFX_MSG_MAP(CUSBcamPage)
	ON_BN_CLICKED(IDC_CK_USBC_DETECTORS_ACTIVE, OnChange)
	ON_BN_CLICKED(IDC_CK_USBC_MD_ACTIVE, OnChange)
	ON_BN_CLICKED(IDC_CK_USBC_UNIT_ACTIVE, OnChange)
	ON_EN_CHANGE(IDC_EDT_USBC_FRAMES_PER_SECOND, OnChange)
	ON_BN_CLICKED(IDC_RD_USBC_FS_0, OnChange)
	ON_BN_CLICKED(IDC_RD_USBC_FS_1, OnChange)
	ON_NOTIFY(NM_CLICK, IDC_LIST_CAMERAS, OnClickListCameras)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUSBcamPage message handlers
BOOL CUSBcamPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();
	CString sFormat, sText;
	CWnd *pWnd  = GetDlgItem(IDC_CK_USBC_UNIT_ACTIVE);
	pWnd->GetWindowText(sFormat);
	sText.Format(sFormat, m_nPageNo);
	pWnd->SetWindowText(sText);

	CWK_Profile &wkpBase = m_pParent->GetDocument()->GetProfile();

	BOOL bDisable = FALSE;
	if (m_nPageNo > 1)	// nicht die erste
	{// darf nicht aktiviert werden, wenn die davor liegende Unit nicht aktiv ist
		if (!IsUnitActive(wkpBase, m_nPageNo-1)) bDisable = ALL_CONTROLLS;
	}

	if ((bDisable == FALSE ) && m_nPageNo < SM_NO_OF_USBCAM_UNITS)	// nicht die letzte Unit
	{// darf nicht deaktiviert werden, wenn die dahinter liegende Unit aktiv ist
		if (IsUnitActive(wkpBase, m_nPageNo+1)) bDisable = TRUE;
	}


	if      (bDisable == ALL_CONTROLLS) EnableGrpWindows(IDC_GRP_USBC_MAIN, FALSE);
	else if (bDisable) pWnd->EnableWindow(FALSE);


	CRect rc;
	m_cCameraList.GetClientRect(&rc);
	m_cCameraList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES);
	m_cCameraList.InsertColumn(0, _T("Cam"), LVCFMT_LEFT, rc.right);

	if (!bDisable)
	{
		InitCameraList(wkpBase, m_nPageNo, &m_cCameraList);
	}


	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CUSBcamPage::CanNew()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CUSBcamPage::CanDelete()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CUSBcamPage::SaveChanges()
{
	CString sUnitName = GetUnitName(m_nPageNo),
		     sLocation, sModule = _T(""), sInput, sOutput;
	CWK_Profile wkpBase(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_sBase, _T(""));
	CWK_Profile wkpDev(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_sDevice, _T(""));
	BOOL  bChanged  = FALSE;
	DWORD dwChanged = 0;
	CSize szFrame (0, 0);
	CStringArray saDevCams;

	wkpDev.WriteInt(SETTINGS_SECTION, MOTION_DETECTION, m_bMotionDetection);
	wkpDev.WriteInt(SETTINGS_SECTION, CAMERA_DETECTORS, m_bCameraDetectors);
	int nTemp = 1000 / m_nFramesPerSecond;
	wkpDev.WriteInt(SETTINGS_SECTION, REF_PICT_RATE, nTemp);
	if      (m_nFrameSize == FRAME_CIF) szFrame = CSize(352, 288);
	else if (m_nFrameSize == FRAME_VGA) szFrame = CSize(640, 480);
	wkpDev.WriteInt(SETTINGS_SECTION, PICT_RES_X, szFrame.cx);
	wkpDev.WriteInt(SETTINGS_SECTION, PICT_RES_Y, szFrame.cy);
	
	int i, nCount = m_cCameraList.GetItemCount();
	nTemp = 0;

	for (i=0; i<nCount; i++)
	{
		if (m_cCameraList.GetCheck(i))
		{
			sLocation.Format(CAMERA_N, nTemp+1);
			sOutput = m_cCameraList.GetItemText(i, 0);
			saDevCams.Add(sOutput);
			wkpDev.WriteString(sLocation, _T(""), sOutput);
			nTemp++;
		}
	}
	wkpBase.WriteMultiString(USB_CAM_UNIT, GetUnitName(m_nPageNo), saDevCams);
	
	for (; nTemp<MAX_CAMERAS; nTemp++)
	{
		sLocation.Format(CAMERA_N, nTemp+1);
		wkpDev.DeleteSection(sLocation);
	}
	
	// Camera Outputs
	sOutput = SM_USBCAM_OUTPUT_CAMERA;
	sOutput += (TCHAR)(_T('0') + m_nPageNo);
	COutputList  * pOutputList  = m_pParent->GetDocument()->GetOutputs();
	COutputGroup * pOutputGroup = pOutputList->GetGroupBySMName(sOutput);
	if (pOutputGroup)
	{
		if (!m_bUnitActive)
		{
			pOutputList->DeleteGroup(sOutput);
			bChanged = TRUE;
		}
	}
	if (m_bUnitActive)
	{
		if (pOutputGroup == NULL)
		{
			pOutputGroup = pOutputList->AddOutputGroup(_T("USB Kamera"), nCount, sOutput);
		}
		else
		{
			pOutputGroup->SetSize((WORD)nCount);
		}
		if (pOutputGroup)
		{
			nTemp = 0;
			for (i=0; i<nCount; i++)
			{
				if (m_cCameraList.GetCheck(i))
				{
					COutput *pO = pOutputGroup->GetOutput(nTemp);
					pO->SetName(m_cCameraList.GetItemText(i, 0));
					pO->SetExOutputType(EX_OUTPUT_OFF);
					nTemp++;
				}
			}			
		}
		bChanged = TRUE;
	}

	if (bChanged)
	{
		pOutputList->Save(wkpBase, FALSE);
		dwChanged |= 1<<IMAGE_CAMERA | 1 << IMAGE_HARDWARE;
	}

	// Camera Detector
	bChanged = FALSE;
	sInput   = SM_USBCAM_INPUT;
	sInput  += (TCHAR)(_T('0') + m_nPageNo);
	CInputList  * pInputList  = m_pParent->GetDocument()->GetInputs();
	CInputGroup * pInputGroup = pInputList->GetGroupBySMName(sInput);
	if (pInputGroup)
	{
		if (!m_bCameraDetectors)
		{
			pInputList->DeleteGroup(sInput);
			bChanged = TRUE;
		}
	}
	else
	{
		if (m_bCameraDetectors)
		{
			pInputList->AddInputGroup(sUnitName, 1, sInput);
			bChanged = TRUE;
		}
	}

	// Camera MotionDetection
	bChanged = FALSE;
	sInput   = SM_USBCAM_MD_INPUT;
	sInput  += (TCHAR)(_T('0') + m_nPageNo);
	pInputList  = m_pParent->GetDocument()->GetInputs();
	pInputGroup = pInputList->GetGroupBySMName(sInput);
	if (pInputGroup)
	{
		if (!m_bMotionDetection)
		{
			pInputList->DeleteGroup(sInput);
			bChanged = TRUE;
		}
	}
	else
	{
		if (m_bMotionDetection)
		{
			pInputList->AddInputGroup(sUnitName + _T(" MD"), 1, sInput);
			bChanged = TRUE;
		}
	}
	
	if (bChanged)
	{
		pInputList->Save(wkpBase, FALSE);
		dwChanged |= 1<<IMAGE_INPUT;
	}
	
	// Launcher
	if (m_bUnitActive)
	{
		sModule = _T("USBcameraUnit.exe");
	}
	wkpBase.WriteString(theApp.GetModuleSection(), sUnitName, sModule);

	if (dwChanged)
	{
		m_pParent->PostMessage(WM_USER, USER_MSG_UPDATE_PARENT_TREE, dwChanged);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CUSBcamPage::CancelChanges()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CUSBcamPage::IsDataValid()
{
	return UpdateData();
}
/////////////////////////////////////////////////////////////////////////////
void CUSBcamPage::Create()
{
	CSVPage::Create(IDD,(CWnd*)m_pParent);
}
//////////////////////////////////////////////////////////////////////////
LPCTSTR CUSBcamPage::GetUnitName(int nPageNo)
{
	switch (nPageNo) 
	{
		case 1: return GetAppnameFromId(WAI_USBCAMUNIT_1);
		case 2: return GetAppnameFromId(WAI_USBCAMUNIT_2);
		case 3: return GetAppnameFromId(WAI_USBCAMUNIT_3);
		case 4: return GetAppnameFromId(WAI_USBCAMUNIT_4);
	}
	return _T("");
}
//////////////////////////////////////////////////////////////////////////
BOOL CUSBcamPage::IsUnitActive(CWK_Profile &wkpBase, int nPageNo)
{
	CString str, sUnitName = GetUnitName(nPageNo);
	ASSERT(IsBetween(nPageNo, 1, SM_NO_OF_USBCAM_UNITS));
	str = wkpBase.GetString(theApp.GetModuleSection(), sUnitName, _T(""));

	return !str.IsEmpty();
}
//////////////////////////////////////////////////////////////////////////
BOOL CUSBcamPage::CanActivateUnit(CWK_Profile &wkpBase, int nPageNo)
{
	return InitCameraList(wkpBase, nPageNo);
}
//////////////////////////////////////////////////////////////////////////
void CUSBcamPage::OnChange() 
{
	SetModified();
}
//////////////////////////////////////////////////////////////////////////
BOOL CUSBcamPage::InitCameraList(CWK_Profile &wkpBase, int nPageNo, CListCtrl *pCameraList)
{
	CString      sCam;
	int          i, j, nCameras, nDevCams, nOtherDevCams;
	CStringArray saCameras, saDevCams, saOtherDevCams;
	BOOL         bReturn = FALSE;
	
	wkpBase.GetMultiString(USB_CAM_UNIT, USB_CAMERAS, saCameras);
	wkpBase.GetMultiString(USB_CAM_UNIT, GetUnitName(nPageNo), saDevCams);
	
	for (i=1; i<=4; i++)
	{
		if (i != nPageNo)
		{
			wkpBase.GetMultiString(USB_CAM_UNIT, GetUnitName(i), saOtherDevCams);
		}
	}

	nCameras      = saCameras.GetSize();
	nDevCams      = saDevCams.GetSize();
	nOtherDevCams = saOtherDevCams.GetSize();

	if (pCameraList) pCameraList->DeleteAllItems();
	for (i=0; i<nCameras; i++)
	{
		sCam = saCameras.GetAt(i);
		for (j=0; j<nOtherDevCams; j++)
		{
			if (sCam == saOtherDevCams.GetAt(j))
			{
				break;
			}
		}
		if (j < nOtherDevCams) continue;

		bReturn = TRUE;
		if (pCameraList) pCameraList->InsertItem(i, sCam);
		else             continue;

		for (j=0; j<nDevCams; j++)
		{
			if (sCam == saDevCams.GetAt(j))
			{
				pCameraList->SetCheck(i);
				break;
			}
		}
	}
	return bReturn;
}
//////////////////////////////////////////////////////////////////////////
void CUSBcamPage::OnClickListCameras(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LVHITTESTINFO tvHTI;
	GetCursorPos(&tvHTI.pt);
	m_cCameraList.ScreenToClient(&tvHTI.pt);
	m_cCameraList.HitTest(&tvHTI);

	if (tvHTI.flags & LVHT_ONITEMSTATEICON)
	{
		OnChange();
	}
	
	*pResult = 0;
}
