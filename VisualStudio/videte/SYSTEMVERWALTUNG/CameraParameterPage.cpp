// CameraParameterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "CameraParameterPage.h"

#include "CIPCOutputIPcamUnit.h"

#include "oemgui\OEMPassworddialog.h"
#include "PasswordCheckDialog.h"

#include "cipcstringdefs.h"
#include <wininet.h>
#include ".\cameraparameterpage.h"

/////////////////////////////////////////////////////////////////////////////
// class CIPCameraList
#define IPCL_NR				0
#define IPCL_NAME			1
#define IPCL_URL			2
#define IPCL_TYPE			3
#define IPCL_IO_NR			4
#define IPCL_ROTATION		5
#define IPCL_COMPRESSION	6

CIPCameraList::CIPCameraList()
{
	m_pnWidth = NULL;
	m_bSizeEnabled = TRUE;
}
/////////////////////////////////////////////////////////////////////////////
CIPCameraList::~CIPCameraList()
{
	if (m_pnWidth)
	{
		free(m_pnWidth);
	}
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CIPCameraList, CEdtSubItemListCtrl)
	ON_WM_SIZE()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
void CIPCameraList::DrawSubItem(CDC*pDC, int nItem, int nSubItem, CRect&rcSubItem, CString sText, LPARAM lParam)
{
	if (   lParam == 0 
		&& (   nItem    != m_Selected.Cell.wItem 
		    || nSubItem != m_Selected.Cell.wSubItem))
	{
		// GetSysColorBrush(COLOR_INFOBK)
		CBrush br(RGB(255,255,225));
		pDC->FillRect(rcSubItem, &br);
	}
	CEdtSubItemListCtrl::DrawSubItem(pDC, nItem, nSubItem, rcSubItem, sText, lParam);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIPCameraList::CanEditSubItem(int nItem, int nSubItem)
{
	return m_pPage->CanEditListItem(nItem, nSubItem);
}
/////////////////////////////////////////////////////////////////////////////
eEditType CIPCameraList::GetEditType(int nItem, int nSubItem)
{
	switch (nSubItem)
	{
		case IPCL_ROTATION:
		case IPCL_COMPRESSION:
			return COMBO_BOX;
	}
	return CEdtSubItemListCtrl::GetEditType(nItem, nSubItem);
}
/////////////////////////////////////////////////////////////////////////////
void CIPCameraList::InitComboBox(CSubItemCombo*pCombo, const CString& sText)
{
	if (m_Selected.Cell.wSubItem == IPCL_ROTATION)
	{
		CString s;
		int i, n = m_pPage->m_cRotations.GetCount();
		for (i=0; i<n; i++)
		{
			m_pPage->m_cRotations.GetLBText(i, s);
			pCombo->AddString(s);
		}
	}
	else if (m_Selected.Cell.wSubItem == IPCL_COMPRESSION)
	{
		CString s;
		int i, n = m_pPage->m_cCompression.GetCount();
		for (i=0; i<n; i++)
		{
			m_pPage->m_cCompression.GetLBText(i, s);
			pCombo->AddString(s);
		}
	}
	pCombo->SetDroppedHeight(100);
}
/////////////////////////////////////////////////////////////////////////////
void CIPCameraList::DoDataExchange(CDataExchange* pDX)
{
	CString sText, sOld = GetItemText(m_Selected.Cell.wItem, m_Selected.Cell.wSubItem);
	DDX_Text(pDX, GetSelectionID(), sText);
	if (!sText.IsEmpty() && sOld != sText)
	{
		CObject *pObject = (CObject*) GetItemData(m_Selected.Cell.wItem);
		if (   pObject 
			&& pObject->IsKindOf(RUNTIME_CLASS(COutput)))
		{
			if (m_Selected.Cell.wSubItem == IPCL_COMPRESSION)
			{
				((COutput*)pObject)->SetCompression(sText);
			}
			else if (m_Selected.Cell.wSubItem == IPCL_ROTATION)
			{
				((COutput*)pObject)->SetRotation(sText);
			}
		}
		m_pPage->SetModified();
		SetItemText(m_Selected.Cell.wItem, m_Selected.Cell.wSubItem, sText);
	}
}
/////////////////////////////////////////////////////////////////////////////
int CIPCameraList::InsertColumn(LPCTSTR pszName, int nWidth)
{
	int nSub = GetSubItemCount();
	m_pnWidth = (int*)realloc(m_pnWidth, sizeof(int)*(nSub+1));
	m_pnWidth[nSub] = nWidth;	// nWidth in percent
	CRect rc;
	GetClientRect(&rc);
	nWidth = MulDiv(rc.Width(), nWidth, 100); // calculate real width
	return CListCtrl::InsertColumn(nSub, pszName, LVCFMT_LEFT, nWidth);
}
/////////////////////////////////////////////////////////////////////////////
void CIPCameraList::OnSize(UINT nType, int cx, int cy)
{
	if (m_bSizeEnabled)
	{
		CEdtSubItemListCtrl::OnSize(nType, cx, cy);
		if (m_pnWidth)
		{
			int i;
			for (i=0; i<GetSubItemCount(); i++)
			{
				SetColumnWidth(i, MulDiv(cx, m_pnWidth[i], 100));
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
// CCameraParameterPage dialog
IMPLEMENT_DYNAMIC(CCameraParameterPage, CSVPage)
/////////////////////////////////////////////////////////////////////////////
CCameraParameterPage::CCameraParameterPage(CSVView* pParent /*=NULL*/)
	: CSVPage(CCameraParameterPage::IDD)
{
	m_pParent = pParent;
	m_pUnitControl = NULL;
	m_uPort = 80; // HTTP
	m_nCurrentCaptureDevice = CB_ERR;

	m_nRequestUnitTimer = 0;
	m_bCanNew = FALSE;
	m_bUnitActive = FALSE;

	m_nInitToolTips = MAKELONG(TOOLTIPS_SIMPLE, 300);
	m_cIPItems.m_pPage = this;

	Create(IDD,m_pParent);
}
/////////////////////////////////////////////////////////////////////////////
CCameraParameterPage::~CCameraParameterPage()
{
	WK_DELETE(m_pUnitControl);
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
		sText.TrimLeft();
		sText.TrimRight();
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
			s.Format(IDS_REQUEST_TO_INSERT, sCtrl);
			AfxMessageBox(s, MB_OK|MB_ICONERROR);
			pDX->Fail();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_cTypes);
	DDV_ComboSel(pDX, m_cTypes);
	DDX_Control(pDX, IDC_COMBO_CAPTURE_DEVICES, m_cCaptureDevices);
	DDV_ComboSel(pDX, m_cCaptureDevices);
	DDX_Control(pDX, IDC_COMBO_IP_ROTATION, m_cRotations);
	DDV_ComboSel(pDX, m_cRotations);
	DDX_Control(pDX, IDC_COMBO_IP_COMPRESSION, m_cCompression);
	DDV_ComboSel(pDX, m_cCompression);
	DDX_Control(pDX, IDC_COMBO_IP_URL, m_cURL);
	DDX_Text(pDX, IDC_EDT_IP_PORT, m_uPort);
	DDV_MinMaxUInt(pDX, m_uPort, 1, INTERNET_MAX_PORT_NUMBER_VALUE);
	DDX_Check(pDX, IDC_CK_IP_CAM_UNIT, m_bUnitActive);
	DDX_Control(pDX, IDC_LIST_IP_ITEMS, m_cIPItems);
	DDX_Control(pDX, IDC_TXT_ACTIVATION, m_cActivating);
	DDX_Text(pDX, IDC_EDT_IP_USER, m_sUserName);

	if (pDX->m_bSaveAndValidate)
	{
		pDX->m_idLastControl = IDC_COMBO_IP_URL;
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
BEGIN_MESSAGE_MAP(CCameraParameterPage, CSVPage)
	//{{AFX_MSG_MAP(CCameraParameterPage)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, OnCbnSelchangeComboType)
	ON_CBN_SELCHANGE(IDC_COMBO_CAPTURE_DEVICES, OnCbnSelchangeComboCaptureDevices)
	ON_CBN_EDITCHANGE(IDC_COMBO_CAPTURE_DEVICES, OnCbnEditchangeComboCaptureDevices)
	ON_BN_CLICKED(IDC_BTN_IP_SEND, OnBnClickedBtnSend)
	ON_BN_CLICKED(IDC_BTN_IP_SET_NAME, OnBnClickedBtnSetName)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_USER, OnUser)
	ON_EN_CHANGE(IDC_EDT_IP_PORT, OnEnChangeEdtPortOrUrl)
	ON_EN_CHANGE(IDC_COMBO_IP_URL, OnEnChangeEdtPortOrUrl)
	ON_CBN_SELCHANGE(IDC_COMBO_IP_URL, OnCbnSelchangeComboUrl)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CK_IP_CAM_UNIT, OnBnClickedCkIpCamUnit)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CK_IP_INPUT1, IDC_CK_IP_CAMERA8, OnBnClickCkIpItem)
	ON_CBN_EDITCHANGE(IDC_COMBO_IP_URL, OnCbnEditchangeComboIpUrl)
	ON_NOTIFY(LVN_ITEMACTIVATE, IDC_LIST_IP_ITEMS, OnLvnItemActivateListIpItems)
	ON_BN_CLICKED(IDC_BTN_IP_PASSWORD, OnBnClickedBtnIpPassword)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
BOOL CCameraParameterPage::CanNew()
{
	return m_bCanNew;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCameraParameterPage::CanDelete()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterPage::OnNew()
{
	CSVDoc *pDoc = m_pParent->GetDocument();
	CInputList*   pInputList  = pDoc->GetInputs();
	COutputList*  pOutputList = pDoc->GetOutputs();
	COutputGroup* pCameras    = pOutputList->GetGroupBySMName(SM_IP_CAMERA_OUTPUT_CAMERA);
	COutputGroup* pRelais     = pOutputList->GetGroupBySMName(SM_IP_CAMERA_OUTPUT_RELAY);
	CInputGroup*  pInputs     = pInputList->GetGroupBySMName(SM_IP_CAMERA_INPUT);
	CInputGroup*  pInputsMD   = pInputList->GetGroupBySMName(SM_IP_CAMERA_MD_INPUT);
	CInput*       pInput      = NULL;
	COutput*      pOutput     = NULL;
	BOOL		  bModified   = FALSE;
	BOOL		  bLimitReached = FALSE;
	const int	  nMaxCameras   = 32;

	if (UpdateParameter())
	{
		// create inputs and outputs
		CString sName, sNr, sType, sTemp;
		CStringArray sa;
		int i, n;
		WORD wGroupID, wMDGroupID, wSubID;
		i = m_cTypes.GetCurSel();
		if (IsInArray(i, m_saTypes.GetCount()))
		{
			sType = m_saTypes[i];
		}
		else
		{
			return ;
		}

		SplitString(m_sCameras, sa, _T(","));
		n = sa.GetSize();
		if (pCameras == NULL && n)
		{
			GetDlgItemText(IDC_TXT_IP_CAMERA, sTemp);	// camera names
			RemoveLastCharacter(sTemp, _T(':'));
			sName.Format(IDS_IP_DEVICE, sTemp);
			pCameras = pOutputList->AddOutputGroup(sName, 0, SM_IP_CAMERA_OUTPUT_CAMERA);
		}
		if (pInputsMD == NULL)
		{
			pInputsMD = pInputList->AddInputGroup(sName+_T("MD"), 0, SM_IP_CAMERA_MD_INPUT);
		}
		if (pCameras && pInputsMD)
		{
			wGroupID = pCameras->GetID().GetGroupID();
			wMDGroupID = pInputsMD->GetID().GetGroupID();
			wSubID = (WORD)pCameras->GetSize();
			for (i=0; i<n; i++)
			{
				if (wSubID<nMaxCameras)
				{
					pCameras->SetSize(wSubID+1);
					pInputsMD->SetSize(wSubID+1);
					pOutput = pCameras->GetOutput(wSubID);
					pOutput->SetUrl(m_sUrl);
					pOutput->SetPort(m_uPort);
					pOutput->SetDeviceName(m_sName);
					pOutput->SetOutputNr(_ttoi(sa[i]));
					pOutput->SetType(sType);
					pOutput->SetRotation(m_sRotation);
					pOutput->SetCompression(m_sCompression);
					wSubID = (WORD)pCameras->GetSize();
					bModified = TRUE;
				}
				else
				{
					bLimitReached = TRUE;
					break;
				}
			}
		}

		sa.RemoveAll();
		SplitString(m_sRelais, sa, _T(","));
		n = sa.GetSize();
		if (pRelais == NULL && n)
		{
			GetDlgItemText(IDC_TXT_IP_RELAIS, sTemp);	// relais names
			RemoveLastCharacter(sTemp, _T(':'));
			sName.Format(IDS_IP_DEVICE, sTemp);
			pRelais = pOutputList->AddOutputGroup(sName, 0, SM_IP_CAMERA_OUTPUT_RELAY);
		}
		if (pRelais)
		{
			wGroupID = pRelais->GetID().GetGroupID();
			wSubID = (WORD)pRelais->GetSize();
			for (i=0; i<n; i++)
			{
				if (wSubID<nMaxCameras)
				{
					pRelais->SetSize(wSubID+1);
					pOutput = pRelais->GetOutput(wSubID);
					pOutput->SetUrl(m_sUrl);
					pOutput->SetPort(m_uPort);
					pOutput->SetDeviceName(m_sName);
					pOutput->SetOutputNr(_ttoi(sa[i]));
					pOutput->SetType(sType);
					wSubID = (WORD)pRelais->GetSize();
					bModified = TRUE;
				}
				else
				{
					bLimitReached = TRUE;
					break;
				}
			}
		}

		sa.RemoveAll();
		SplitString(m_sInputs, sa, _T(","));
		n = sa.GetSize();
		if (pInputs == NULL && n)
		{
			GetDlgItemText(IDC_TXT_IP_INPUT, sTemp);	// input names
			RemoveLastCharacter(sTemp, _T(':'));
			sName.Format(IDS_IP_DEVICE, sTemp);
			pInputs = pInputList->AddInputGroup(sName, 0, SM_IP_CAMERA_INPUT);
		}
		if (pInputs)
		{
			wGroupID = pInputs->GetID().GetGroupID();
			wSubID = (WORD)pInputs->GetSize();
			for (i=0; i<n; i++)
			{
				if (wSubID<nMaxCameras)
				{
					pInputs->SetSize(wSubID+1);
					pInput = pInputs->GetInput(wSubID);
					pInput->SetUrl(m_sUrl);
					pInput->SetPort(m_uPort);
					pInput->SetInputNr(_ttoi(sa[i]));
					pInput->SetType(sType);
					wSubID = (WORD)pInputs->GetSize();
					bModified = TRUE;
				}
				else
				{
					bLimitReached = TRUE;
					break;
				}
			}
		}
	}
	if (bLimitReached)
	{
		CString sMsg;
		sMsg.Format(IDS_NO_OF_CAMS_LIMITED, nMaxCameras);
		AfxMessageBox(sMsg, MB_ICONINFORMATION|MB_OK);
	}
	if (bModified)
	{
		SetModified();
		CreateList();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterPage::OnDelete()
{
	// delete selected output?
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCameraParameterPage::StretchElements()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCameraParameterPage::GetToolTip(UINT nID, CString&sText)
{
	switch (nID)
	{
		case IDC_CK_IP_CAMERA2:
		case IDC_CK_IP_CAMERA3:
		case IDC_CK_IP_CAMERA4:
		case IDC_CK_IP_CAMERA5:
		case IDC_CK_IP_CAMERA6:
		case IDC_CK_IP_CAMERA7:
		case IDC_CK_IP_CAMERA8:
			return sText.LoadString(IDC_CK_IP_CAMERA1);
		case IDC_CK_IP_OUTPUT2:
		case IDC_CK_IP_OUTPUT3:
		case IDC_CK_IP_OUTPUT4:
		case IDC_CK_IP_OUTPUT5:
		case IDC_CK_IP_OUTPUT6:
		case IDC_CK_IP_OUTPUT7:
		case IDC_CK_IP_OUTPUT8:
			return sText.LoadString(IDC_CK_IP_OUTPUT1);
		case IDC_CK_IP_INPUT2:
		case IDC_CK_IP_INPUT3:
		case IDC_CK_IP_INPUT4:
		case IDC_CK_IP_INPUT5:
		case IDC_CK_IP_INPUT6:
		case IDC_CK_IP_INPUT7:
		case IDC_CK_IP_INPUT8:
			return sText.LoadString(IDC_CK_IP_INPUT1);
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterPage::SaveChanges()
{
	CSVDoc *pDoc = m_pParent->GetDocument();
	CInputList *pInputs = pDoc->GetInputs();
	COutputList *pOutputs = pDoc->GetOutputs();
	BOOL bGenerateHTML = FALSE;

	if (m_bUnitActive)
	{
		bGenerateHTML = TRUE;
		m_pParent->AddInitApp(WK_APP_NAME_IP_CAMERA_UNIT);
	}
	else
	{
		ControlUnit(FALSE);
		pDoc->GetProfile().DeleteEntry(theApp.GetModuleSection(), WK_APP_NAME_IP_CAMERA_UNIT);
		m_cIPItems.DeleteAllItems();

		pInputs->DeleteGroup(SM_IP_CAMERA_INPUT);
		pInputs->DeleteGroup(SM_IP_CAMERA_MD_INPUT);
		pOutputs->DeleteGroup(SM_IP_CAMERA_OUTPUT_CAMERA);
		pOutputs->DeleteGroup(SM_IP_CAMERA_OUTPUT_RELAY);
	}

	pInputs->Save(pDoc->GetProfile(), bGenerateHTML);
	pOutputs->Save(pDoc->GetProfile(), bGenerateHTML);
	SaveUserAndPassword();

	m_pParent->PostMessage(WM_USER, USER_MSG_UPDATE_PARENT_TREE, (1<<IMAGE_INPUT)|(1<<IMAGE_RELAY)|(1<<IMAGE_CAMERA));
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterPage::SaveUserAndPassword()
{
	if (!m_sUrl.IsEmpty())
	{
		CSVDoc *pDoc = m_pParent->GetDocument();
		CString sSection = WK_APP_NAME_IP_CAMERA_UNIT _T("\\") SETTINGS_SECTION _T("\\");
		sSection += m_sUrl;
		GetDlgItemText(IDC_EDT_IP_USER, m_sUserName);
		m_sUserName.TrimLeft();
		m_sUserName.TrimRight();
		pDoc->GetProfile().WriteString(sSection, REGKEY_USER, m_sUserName);
		if (!m_sPassword.IsEmpty())
		{
			CString sPwd(m_sPassword);
			CWK_Profile::Encode(sPwd);
			pDoc->GetProfile().WriteString(sSection, REGKEY_PASSWORD, sPwd);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterPage::CancelChanges()
{
	CSVDoc *pDoc = m_pParent->GetDocument();
	CInputList *pInputs = pDoc->GetInputs();
	COutputList *pOutputs = pDoc->GetOutputs();
	CWK_Profile &wkp = pDoc->GetProfile();
	pInputs->Load(wkp);
	pOutputs->Load(wkp);
	CString str;
	str = wkp.GetString(theApp.GetModuleSection(), WK_APP_NAME_IP_CAMERA_UNIT, _T(""));
	m_bUnitActive = !str.IsEmpty();
	ControlUnit(m_bUnitActive);

	EmptyControls();
	CreateList();
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCameraParameterPage::IsDataValid()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterPage::InitCaptureDeviceList()
{
	if (m_pUnitControl)
	{
		CSecID id(m_pUnitControl->GetGroupID(), CSV_CONTROL_ID);
		m_pUnitControl->DoRequestSetValue(id, CSD_CAM_TYPE, CSD_CAM_CAPTURE, 0);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterPage::InitStrings(const CString&sCsv, CComboBox&combo, BOOL bSelect/*=TRUE*/)
{
	CStringArray sa;
	SplitString(sCsv, sa, _T(","));
	int i, n = (int)sa.GetCount();

	combo.EnableWindow(n>0);
	combo.ResetContent();
	for (i=0; i<n; i++)
	{
		combo.AddString(sa[i]);
	}
	if (n && bSelect)
	{
		combo.SetCurSel(0);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterPage::CreateList()
{
	CSVDoc *pDoc = m_pParent->GetDocument();
	CInputList *pInputList = pDoc->GetInputs();
	COutputList *pOutputList = pDoc->GetOutputs();

	COutputGroup*pCameras = pOutputList->GetGroupBySMName(SM_IP_CAMERA_OUTPUT_CAMERA);
	COutputGroup*pRelais  = pOutputList->GetGroupBySMName(SM_IP_CAMERA_OUTPUT_RELAY);
	CInputGroup *pInputs  = pInputList->GetGroupBySMName(SM_IP_CAMERA_INPUT);
	CInput *pInput;
	COutput*pOutput;
	int i, n, nItem = 0;
	CString s;

	m_cIPItems.m_bSizeEnabled = FALSE;
	m_cIPItems.DeleteAllItems();

	if (pCameras)
	{
		GetDlgItemText(IDC_TXT_IP_CAMERA, s);
		m_cIPItems.InsertItem(nItem++, s);
		n = pCameras->GetSize();
		for (i=0; i<n; i++)
		{
			pOutput = pCameras->GetOutput(i);
			s.Format(_T("%d"), i+1);
			nItem = m_cIPItems.InsertItem(LVIF_TEXT|LVIF_PARAM, nItem, s, 0, 0, 0, (LPARAM)pOutput);
			m_cIPItems.SetItemText(nItem, IPCL_NAME, pOutput->GetDeviceName());
			m_cIPItems.SetItemText(nItem, IPCL_URL, pOutput->GetUrl());
			m_cIPItems.SetItemText(nItem, IPCL_TYPE, pOutput->GetType());
			s.Format(_T("%d"), pOutput->GetOutputNr());
			m_cIPItems.SetItemText(nItem, IPCL_IO_NR, s);
			m_cIPItems.SetItemText(nItem, IPCL_ROTATION, pOutput->GetRotation());
			m_cIPItems.SetItemText(nItem, IPCL_COMPRESSION, pOutput->GetCompression());

			nItem++;
		}
	}

	if (pRelais)
	{
		GetDlgItemText(IDC_TXT_IP_RELAIS, s);
		m_cIPItems.InsertItem(nItem++, s);
		n = pRelais->GetSize();
		for (i=0; i<n; i++)
		{
			pOutput = pRelais->GetOutput(i);
			s.Format(_T("%d"), i+1);
			nItem = m_cIPItems.InsertItem(LVIF_TEXT|LVIF_PARAM, nItem, s, 0, 0, 0, (LPARAM)pOutput);
			m_cIPItems.SetItemText(nItem, IPCL_NAME, pOutput->GetDeviceName());
			m_cIPItems.SetItemText(nItem, IPCL_URL, pOutput->GetUrl());
			m_cIPItems.SetItemText(nItem, IPCL_TYPE, pOutput->GetType());
			s.Format(_T("%d"), pOutput->GetOutputNr());
			m_cIPItems.SetItemText(nItem, IPCL_IO_NR, s);
			nItem++;
		}
	}

	if (pInputs)
	{
		GetDlgItemText(IDC_TXT_IP_INPUT, s);
		m_cIPItems.InsertItem(nItem++, s);
		n = pInputs->GetSize();
		for (i=0; i<n; i++)
		{
			pInput = pInputs->GetInput(i);
			s.Format(_T("%d"), i+1);
			nItem = m_cIPItems.InsertItem(LVIF_TEXT|LVIF_PARAM, nItem, s, 0, 0, 0, (LPARAM)pInput);
			m_cIPItems.SetItemText(nItem, IPCL_NAME, pInput->GetName());
			m_cIPItems.SetItemText(nItem, IPCL_URL, pInput->GetUrl());
			m_cIPItems.SetItemText(nItem, IPCL_TYPE, pInput->GetType());
			s.Format(_T("%d"), pInput->GetInputNr());
			m_cIPItems.SetItemText(nItem, IPCL_IO_NR, s);
			nItem++;
		}
	}
	m_cIPItems.m_bSizeEnabled = TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCameraParameterPage::CanEditListItem(int nItem, int nSubItem)
{
	CObject *pObject = (CObject*) m_cIPItems.GetItemData(nItem);
	if (   pObject 
		&& pObject->IsKindOf(RUNTIME_CLASS(COutput))
		&& !((COutput*)pObject)->GetCompression().IsEmpty())
	{
		if (   nSubItem == IPCL_ROTATION 
			&& m_cRotations.GetCount() > 1)
		{
			return TRUE;
		}
		else if (   nSubItem == IPCL_COMPRESSION
			     && m_cCompression.GetCount() > 1)
		{
			return TRUE;
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterPage::EmptyControls()
{
	CString sValue;
	m_cCompression.ResetContent();
	m_cRotations.ResetContent();

	m_cCompression.EnableWindow(FALSE);
	m_cRotations.EnableWindow(FALSE);

	for (int i=0; i<8; i++)
	{
		GetDlgItem(IDC_CK_IP_CAMERA1+i)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CK_IP_OUTPUT1+i)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CK_IP_INPUT1+i)->ShowWindow(SW_HIDE);
	}
	SetDlgItemText(IDC_COMBO_IP_RESOLUTIONS, sValue);
	SetDlgItemText(IDC_TXT_IP_FRAMERATES, sValue);
	SetDlgItemText(IDC_EDT_IP_NAME, sValue);
	m_bCanNew = FALSE;
	m_cIPItems.SelectSubItem(0, 0);
	m_sUrl.Empty();
	m_sCameras.Empty();
	m_sDeviceType.Empty();
	m_sRelais.Empty();
	m_sInputs.Empty();
	m_sCompression.Empty();
	m_sRotation.Empty();
	m_sName.Empty();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCameraParameterPage::UpdateParameter()
{
	if (UpdateData())
	{
		int i, nSel = m_cTypes.GetCurSel();
		ASSERT(nSel != CB_ERR);
		CString sFmt;
		if (IsInArray(nSel, m_saTypes.GetCount()))
		{
			m_sDeviceType = m_saTypes[nSel];
		}
		m_sCameras.Empty();
		GetDlgItemText(IDC_EDT_IP_NAME, m_sName);
		m_sName.TrimLeft();
		m_sName.TrimRight();
		if (m_sName.IsEmpty())
		{
			CString s, sCtrl;
			GetDlgItemText(IDC_TXT_IP_NAME, sCtrl);
			s.Format(IDS_REQUEST_TO_INSERT, sCtrl);
			AfxMessageBox(s, MB_OK|MB_ICONERROR);
			GotoDlgCtrl(GetDlgItem(IDC_EDT_IP_NAME));
			return FALSE;
		}
		if (nSel == 0)
		{
			m_cCaptureDevices.GetLBText(m_cCaptureDevices.GetCurSel(), m_sName);
			CWnd*pItem = GetDlgItem(IDC_CK_IP_CAMERA1);
			if (   pItem->IsWindowVisible()
				&& pItem->IsWindowEnabled()
				&& IsDlgButtonChecked(IDC_CK_IP_CAMERA1) == BST_CHECKED)
			{
				pItem->EnableWindow(FALSE);
				m_sCameras = _T("1");
			}
		}
		else
		{
			for (i=0; i<8; i++)
			{
				CWnd*pItem = GetDlgItem(IDC_CK_IP_CAMERA1+i);
				if (   pItem->IsWindowVisible()
					&& pItem->IsWindowEnabled())
				{
					if (IsDlgButtonChecked(IDC_CK_IP_CAMERA1+i) == BST_CHECKED)
					{
						pItem->EnableWindow(FALSE);
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
			CWnd*pItem = GetDlgItem(IDC_CK_IP_OUTPUT1+i);
			if (   pItem->IsWindowVisible()
				&& pItem->IsWindowEnabled())
			{
				if (IsDlgButtonChecked(IDC_CK_IP_OUTPUT1+i) == BST_CHECKED)
				{
					pItem->EnableWindow(FALSE);
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
			CWnd*pItem = GetDlgItem(IDC_CK_IP_INPUT1+i);
			if (   pItem->IsWindowVisible()
				&& pItem->IsWindowEnabled())
			{
				if (IsDlgButtonChecked(IDC_CK_IP_INPUT1+i) == BST_CHECKED)
				{
					pItem->EnableWindow(FALSE);
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

		if (m_cCompression.GetCurSel() != CB_ERR)
		{
			m_cCompression.GetLBText(m_cCompression.GetCurSel(), m_sCompression);
		}
		if (m_cRotations.GetCurSel() != CB_ERR)
		{
			m_cRotations.GetLBText(  m_cRotations.GetCurSel()  , m_sRotation);
		}
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCameraParameterPage::FindOutput(int nOutput, BOOL bCamera)
{
	CSVDoc *pDoc = m_pParent->GetDocument();
	COutputList *pOutputList = pDoc->GetOutputs();
	COutputGroup*pOutputs = pOutputList->GetGroupBySMName(bCamera ? SM_IP_CAMERA_OUTPUT_CAMERA : SM_IP_CAMERA_OUTPUT_RELAY);
	if (pOutputs)
	{
		COutput*pOutput;
		int i = m_cTypes.GetCurSel(), n;
		CString sType;
		if (IsInArray(i, m_saTypes.GetCount()))
		{
			sType = m_saTypes[i];
		}
		else
		{
			return FALSE;
		}

		if (pOutputs)
		{
			n = pOutputs->GetSize();
			for (i=0; i<n; i++)
			{
				pOutput = pOutputs->GetOutput(i);
				if (   pOutput->GetOutputNr() == nOutput
					&& pOutput->GetPort() == m_uPort
					&& pOutput->GetType() == sType
					&& pOutput->GetUrl() == m_sUrl)
				{
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCameraParameterPage::FindInput(int nInput, BOOL bMD)
{
	CSVDoc *pDoc = m_pParent->GetDocument();
	CInputList *pInputList = pDoc->GetInputs();
	CInputGroup*pInputs = pInputList->GetGroupBySMName(bMD ? SM_IP_CAMERA_MD_INPUT : SM_IP_CAMERA_INPUT);
	if (pInputs)
	{
		CInput*pInput;
		int i = m_cTypes.GetCurSel(), n;
		CString sType;
		if (IsInArray(i, m_saTypes.GetCount()))
		{
			sType = m_saTypes[i];
		}
		else
		{
			return FALSE;
		}

		if (pInputs)
		{
			n = pInputs->GetSize();
			for (i=0; i<n; i++)
			{
				pInput = pInputs->GetInput(i);
				if (   pInput->GetInputNr() == nInput
					&& pInput->GetPort() == m_uPort
					&& pInput->GetType() == sType
					&& pInput->GetUrl() == m_sUrl)
				{
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterPage::ControlUnit(BOOL bControl)
{
	HWND hwnd = ::FindWindow(NULL, WK_APP_NAME_IP_CAMERA_UNIT);
	if (hwnd)
	{
		if (m_nRequestUnitTimer)
		{
			if (m_nRequestUnitTimer != 0xffffffff)
			{
				KillTimer(m_nRequestUnitTimer);
			}
			m_nRequestUnitTimer = 0;
			m_cActivating.SetWindowText(_T(""));
		}
		if (bControl)
		{
			DWORD dwResult;
			::SendMessageTimeout(hwnd, WK_WINTHREAD_STOP_THREAD, TRUE, (LPARAM)m_hWnd, SMTO_BLOCK, 5000, &dwResult);
			DWORD dwError = GetLastError();
			if (   dwError == 0 
				&& dwResult != 0
				&& m_pUnitControl == NULL)
			{
				m_pUnitControl = new CIPCOutputIPcamUnit(this, SM_IP_CAMERA_OUTPUT_CAMERA _T("Control"), TRUE);
				PostMessage(WM_USER, WM_INITDIALOG);
			}
		}
		else if (!bControl)
		{
			DWORD dwResult;
			::SendMessageTimeout(hwnd, WK_WINTHREAD_STOP_THREAD, FALSE, (LPARAM)m_hWnd, SMTO_BLOCK, 5000, &dwResult);
			DWORD dwError = GetLastError();
			if (   dwError == 0 
				&& dwResult == 0)
			{
				WK_DELETE(m_pUnitControl);
			}
		}
		else if (m_pUnitControl)
		{
			if (m_nRequestUnitTimer)
			{
				KillTimer(m_nRequestUnitTimer);
			}
			m_nRequestUnitTimer = 0xffffffff;
			if (m_hWnd)
			{
				m_cTypes.EnableWindow(TRUE);
			}
			m_nRequestUnitTimer = 0;
		}
	}
	else if (m_hWnd)
	{
		CString str;
		if (m_nRequestUnitTimer == 0)
		{
			m_nRequestUnitTimer = SetTimer(0x0815, 1000, NULL);
			str.LoadString(IDS_STARTING_UNIT);
			m_cActivating.SetWindowText(str);
			m_cTypes.EnableWindow(FALSE);
		}
		else
		{
			m_cActivating.GetWindowText(str);
			str += _T(".");
			m_cActivating.SetWindowText(str);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
// CCameraParameterPage message handlers
BOOL CCameraParameterPage::OnInitDialog()
{
	CSVPage::OnInitDialog();

	CString s;	// init list columns
	s.LoadString(IDS_NUMBER);
	m_cIPItems.InsertColumn(s, 10);
	GetDlgItemText(IDC_TXT_IP_NAME, s);
	m_cIPItems.InsertColumn(s, 30);
	GetDlgItemText(IDC_TXT_IP_LOCATION, s);
	m_cIPItems.InsertColumn(s, 20);
	GetDlgItemText(IDC_TXT_IP_TYPE, s);
	m_cIPItems.InsertColumn(s, 10);
	s.LoadString(IDS_SUB_NR);
	m_cIPItems.InsertColumn(s, 10);
	GetDlgItemText(IDC_TXT_IP_ROTATION, s);
	m_cIPItems.InsertColumn(s, 10);
	GetDlgItemText(IDC_TXT_IP_COMPRESSION, s);
	m_cIPItems.InsertColumn(s, 10);

	m_cIPItems.SetExtendedStyle(LVS_EX_GRIDLINES);
	OnCbnSelchangeComboType();

	CancelChanges();
	if (m_bUnitActive)
	{
		ControlUnit(TRUE);
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterPage::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == m_nRequestUnitTimer)
	{
		ControlUnit(TRUE);
	}
	CSVPage::OnTimer(nIDEvent);
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterPage::OnCbnSelchangeComboType()
{
	int nSel = m_cTypes.GetCurSel();
	int nShowCapture = SW_HIDE;
	int nShowIP		 = SW_HIDE;
	BOOL bEnalbeNoCapt  = FALSE;
	if (   IsInArray(nSel, m_saTypes.GetCount())
		&& m_pUnitControl)
	{
		if (m_saTypes[nSel] == CSD_CAM_CAPTURE)
		{
			InitCaptureDeviceList();
			nShowCapture = SW_SHOW;
		}
		else
		{
			CSecID id(m_pUnitControl->GetGroupID(), CSV_CONTROL_ID);
			m_pUnitControl->DoRequestGetValue(id, m_saTypes[nSel], 0);
			bEnalbeNoCapt = TRUE;
			nShowIP = SW_SHOW;
		}
	}
		
	GetDlgItem(IDC_TXT_IP_DEVICE)->ShowWindow(nShowCapture);
	GetDlgItem(IDC_BTN_IP_SET_NAME)->ShowWindow(nShowCapture);
	GetDlgItem(IDC_TXT_IP_DEVICE)->ShowWindow(nShowCapture);
	m_cCaptureDevices.ShowWindow(nShowCapture);

	GetDlgItem(IDC_EDT_IP_NAME)->ShowWindow(nShowIP);
	GetDlgItem(IDC_TXT_IP_LOCATION)->ShowWindow(nShowIP);
	GetDlgItem(IDC_COMBO_IP_URL)->ShowWindow(nShowIP);
	GetDlgItem(IDC_BTN_IP_SEND)->ShowWindow(nShowIP);
	GetDlgItem(IDC_TXT_IP_USER)->ShowWindow(nShowIP);
	GetDlgItem(IDC_EDT_IP_USER)->ShowWindow(nShowIP);
	GetDlgItem(IDC_BTN_IP_PASSWORD)->ShowWindow(nShowIP);

	GetDlgItem(IDC_TXT_IP_LOCATION)->EnableWindow(bEnalbeNoCapt);
	GetDlgItem(IDC_COMBO_IP_URL)->EnableWindow(bEnalbeNoCapt);
	GetDlgItem(IDC_EDT_IP_PORT)->EnableWindow(bEnalbeNoCapt);

	EmptyControls();

	m_cCaptureDevices.ResetContent();
	m_cCaptureDevices.EnableWindow(FALSE);
	m_nCurrentCaptureDevice = CB_ERR;
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterPage::OnCbnSelchangeComboUrl()
{
	SaveUserAndPassword();
	int nSel = m_cURL.GetCurSel();
	if (nSel != CB_ERR)
	{
		SetDlgItemText(IDC_EDT_IP_NAME, m_saUrls[nSel]);
		OnBnClickedBtnSend();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterPage::OnCbnEditchangeComboIpUrl()
{
	CWnd *pSend = GetDlgItem(IDC_BTN_IP_SEND);
	if (pSend && !pSend->IsWindowEnabled())
	{
		SaveUserAndPassword();
		pSend->EnableWindow(TRUE);
		EmptyControls();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterPage::OnCbnSelchangeComboCaptureDevices()
{
	int nSel = m_cCaptureDevices.GetCurSel();
	if (nSel != CB_ERR && m_pUnitControl)
	{
		CSecID id(m_pUnitControl->GetGroupID(), CSV_CONTROL_ID);
		CString sCam;
		m_cCaptureDevices.GetLBText(nSel, sCam);
		m_pUnitControl->DoRequestSetValue(id, CSD_CAM_CAPTURE, sCam, 0);
		m_nCurrentCaptureDevice = nSel;
		EmptyControls();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterPage::OnCbnEditchangeComboCaptureDevices()
{
	GetDlgItem(IDC_BTN_IP_SET_NAME)->EnableWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterPage::OnBnClickedBtnSetName()
{
	if (m_nCurrentCaptureDevice != CB_ERR)
	{
		GetDlgItem(IDC_BTN_IP_SET_NAME)->EnableWindow(FALSE);

		CString sOldName, sNewName;
		m_cCaptureDevices.GetLBText(m_nCurrentCaptureDevice, sOldName);
		m_cCaptureDevices.GetWindowText(sNewName);
		if (sOldName != sNewName && m_pUnitControl)
		{
			CSecID id(m_pUnitControl->GetGroupID(), CSV_CONTROL_ID);
			m_pUnitControl->DoRequestSetValue(id, CSD_NAME, sOldName+_T(",")+sNewName, 0);
		}
	}
}
void CCameraParameterPage::OnEnChangeEdtPortOrUrl()
{
	GetDlgItem(IDC_BTN_IP_SEND)->EnableWindow(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterPage::OnBnClickedBtnSend()
{
	if (UpdateData())
	{
		int nSel = m_cTypes.GetCurSel();
		if (IsInArray(nSel, m_saTypes.GetCount()) && m_pUnitControl)
		{
			CSecID id(m_pUnitControl->GetGroupID(), CSV_CONTROL_ID);
			CString sValue;
			sValue.Format(_T("%s:%s"), m_saTypes[nSel], m_sUrl);
			m_pUnitControl->DoRequestSetValue(id, CSD_CAM_TYPE, sValue, m_uPort);
			GetDlgItem(IDC_BTN_IP_SEND)->EnableWindow(FALSE);
			m_cIPItems.SelectSubItem(0, 0);
			CSVDoc *pDoc = m_pParent->GetDocument();
			CString sSection = WK_APP_NAME_IP_CAMERA_UNIT _T("\\") SETTINGS_SECTION _T("\\");
			sSection += m_sUrl;
			m_sUserName	= pDoc->GetProfile().GetString(sSection, REGKEY_USER, NULL);
			m_sPassword = pDoc->GetProfile().GetString(sSection, REGKEY_PASSWORD, NULL);
			CWK_Profile::Decode(m_sPassword, FALSE);
			SetDlgItemText(IDC_EDT_IP_USER, m_sUserName);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterPage::OnBnClickedCkIpCamUnit()
{
	BOOL bOldUnitActive = m_bUnitActive;
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_IP_CAM_UNIT, m_bUnitActive);
	dx.m_bSaveAndValidate = FALSE;
	CWK_Profile wkp;
	if (m_bUnitActive)											// aktivate ?
	{
		// TODO! RKE: start unit immedeately
		wkp.WriteString(theApp.GetModuleSection(), WK_APP_NAME_IP_CAMERA_UNIT, WK_APP_NAME_IP_CAMERA_UNIT _T(".exe"));
		ControlUnit(TRUE);
	}
	else														// deactivate
	{
		CSVDoc *pDoc = m_pParent->GetDocument();
		CInputList *pInputList = pDoc->GetInputs();
		COutputList *pOutputList = pDoc->GetOutputs();
		COutputGroup*pOutputGroup = pOutputList->GetGroupBySMName(SM_IP_CAMERA_OUTPUT_CAMERA);
		COutput *pOutput;
		CString sMsg, sNames;
		int i, nSize;
		if (pOutputGroup)
		{
			CString sTemp;
			nSize = pOutputGroup->GetSize();
			for (i=0; i<nSize; i++)
			{
				pOutput = pOutputGroup->GetOutput(i);
				if (!CheckActivationsWithOutput(pOutput->GetID(), 0, SVP_CO_CHECK_ONLY))
				{
					sTemp += _T(" * ") + pOutput->GetName() + _T("\n");
				}
			}
		}
		pOutputGroup = pOutputList->GetGroupBySMName(SM_IP_CAMERA_OUTPUT_RELAY);
		if (pOutputGroup)
		{
			CString sTemp;
			nSize = pOutputGroup->GetSize();
			for (i=0; i<nSize; i++)
			{
				pOutput = pOutputGroup->GetOutput(i);
				if (!CheckActivationsWithOutput(pOutput->GetID(), 0, SVP_CO_CHECK_ONLY))
				{
					sTemp += _T(" * ") + pOutput->GetName() + _T("\n");
				}
			}
			if (!sTemp.IsEmpty())
			{
				GetDlgItemText(IDC_TXT_IP_RELAIS, sMsg);
				sNames += sMsg + _T("\n") + sTemp;
			}
		}
		if (!sNames.IsEmpty())
		{
			sMsg.Format(IDS_EXISTING_ACTIVATIONS, sNames);
			if (IDNO == AfxMessageBox(sMsg, MB_YESNO))
			{
				m_bUnitActive = bOldUnitActive;
			}
		}

		CInputGroup*pInputGroup   = pInputList->GetGroupBySMName(SM_IP_CAMERA_INPUT);
		if (pInputGroup && m_bUnitActive)
		{
			if (!CheckActivationsWithInputGroup(pInputGroup->GetID(), SVP_CO_CHECK))
			{
				m_bUnitActive = bOldUnitActive;
			}
		}
		pInputGroup   = pInputList->GetGroupBySMName(SM_IP_CAMERA_MD_INPUT);
		if (pInputGroup && m_bUnitActive)
		{
			if (!CheckActivationsWithInputGroup(pInputGroup->GetID(), SVP_CO_CHECK))
			{
				m_bUnitActive = bOldUnitActive;
			}
		}
		if (m_bUnitActive)
		{
			DDX_Check(&dx, IDC_CK_IP_CAM_UNIT, m_bUnitActive);
		}
		else
		{
			SetModified();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterPage::OnConfirmGetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwUserData)
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
		m_cTypes.EnableWindow(n>0 ? TRUE:FALSE);
	}
	else if (sKey == CSD_COMPRESSIONS)
	{
		InitStrings(sValue, m_cCompression);
	}
	else if (sKey == CSD_RESOLUTIONS)
	{
		SetDlgItemText(IDC_COMBO_IP_RESOLUTIONS, sValue);
	}
	else if (sKey == CSD_ROTATIONS)
	{
		InitStrings(sValue, m_cRotations);
	}
	else if (sKey == CSD_OUTPUTS)
	{
		int i, nID, n = min(_ttol(sValue), 8);
		for (i=0, nID=IDC_CK_IP_CAMERA1; i<n; i++,nID++)
		{
			GetDlgItem(nID)->ShowWindow(SW_SHOW);
			BOOL bFound = FindOutput(i+1, TRUE);
			GetDlgItem(nID)->EnableWindow(!bFound);
			CheckDlgButton(nID, bFound ? BST_CHECKED : BST_UNCHECKED);
		}
	}
	else if (sKey == CSD_INPUTS)
	{
		int i, nID, n = min(_ttol(sValue), 8);
		for (i=0, nID=IDC_CK_IP_INPUT1; i<n; i++,nID++)
		{
			GetDlgItem(nID)->ShowWindow(SW_SHOW);
			BOOL bFound = FindInput(i+1, FALSE);
			GetDlgItem(nID)->EnableWindow(!bFound);
			CheckDlgButton(nID, bFound ? BST_CHECKED : BST_UNCHECKED);
		}
	}
	else if (sKey == CSD_RELAIS)
	{
		int i, nID, n = min(_ttol(sValue), 8);
		for (i=0, nID=IDC_CK_IP_OUTPUT1; i<n; i++,nID++)
		{
			GetDlgItem(nID)->ShowWindow(SW_SHOW);
			BOOL bFound = FindOutput(i+1, FALSE);
			GetDlgItem(nID)->EnableWindow(!bFound);
			CheckDlgButton(nID, bFound ? BST_CHECKED : BST_UNCHECKED);
		}
	}
	else if (sKey == CSD_NAME)
	{
		SetDlgItemText(IDC_EDT_IP_NAME, sValue);
	}
	else if (sKey == CSD_FRAMERATES)
	{
		SetDlgItemText(IDC_TXT_IP_FRAMERATES, sValue);
	}
	else if (sKey == CSD_CAM_CAPTURE)
	{
		InitStrings(sValue, m_cCaptureDevices, FALSE);
	}
	else
	{
		int nSel = m_cTypes.GetCurSel();
		if (   IsInArray(nSel, m_saTypes.GetCount())
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
/*
			if (n)
			{
				m_cURL.SetCurSel(0);
				OnCbnSelchangeComboUrl();
			}
*/
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterPage::OnConfirmSetValue(CSecID id, const CString &sKey, const CString &sValue, DWORD dwUserData)
{
	if (sKey == CSD_CAM_TYPE)
	{
		if (sValue != CSD_OFF)
		{
			if (sValue == CSD_CAM_CAPTURE)
			{
				m_pUnitControl->DoRequestGetValue(id, sValue, 0);
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
void CCameraParameterPage::RequestParameterSet(CSecID id)
{
	if (m_pUnitControl)
	{
		m_pUnitControl->DoRequestGetValue(id, CSD_COMPRESSIONS, 0);
		m_pUnitControl->DoRequestGetValue(id, CSD_RESOLUTIONS, 0);
		m_pUnitControl->DoRequestGetValue(id, CSD_ROTATIONS, 0);
		m_pUnitControl->DoRequestGetValue(id, CSD_FRAMERATES, 0);
		m_pUnitControl->DoRequestGetValue(id, CSD_OUTPUTS, 0);
		m_pUnitControl->DoRequestGetValue(id, CSD_INPUTS, 0);
		m_pUnitControl->DoRequestGetValue(id, CSD_RELAIS, 0);
		m_pUnitControl->DoRequestGetValue(id, CSD_NAME, 0);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterPage::OnDestroy()
{
	HWND hWnd = m_hWnd;
	m_hWnd = NULL;			// to deny any PostMessage in ControlUnit(..)
	ControlUnit(FALSE);	
	m_hWnd = hWnd;
	CSVPage::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CCameraParameterPage::OnUser(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
		case WM_INITDIALOG:
		if (m_pUnitControl)
		{
			CSecID id(m_pUnitControl->GetGroupID(), CSV_CONTROL_ID);
			m_pUnitControl->DoRequestGetValue(id, CSD_CAM_TYPE, 0);
		}break;
		case WM_CLOSE:
			m_cTypes.ResetContent();
			OnCbnSelchangeComboType();
			m_cTypes.EnableWindow(FALSE);
			Sleep(500);
			ControlUnit(m_bUnitActive);
			break;
		case WM_USER:
		if (lParam)
		{
			CConfirmValue*pcv = (CConfirmValue*)lParam;
			if (HIWORD(wParam) == ONCONFIRMGETVALUE)
			{
				OnConfirmGetValue(pcv->m_id, pcv->m_sKey, pcv->m_sValue, pcv->m_dwUserData);
			}
			else if (HIWORD(wParam) == ONCONFIRMSETVALUE)
			{
				OnConfirmSetValue(pcv->m_id, pcv->m_sKey, pcv->m_sValue, pcv->m_dwUserData);
			}
			delete pcv;
		}break;
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterPage::OnBnClickCkIpItem(UINT nID)
{
	if (IsDlgButtonChecked(nID) == BST_CHECKED)
	{
		m_bCanNew = TRUE;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterPage::OnLvnItemActivateListIpItems(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	if (   pNMIA->hdr.idFrom == IDC_LIST_IP_ITEMS
		&& m_cIPItems.GetItemData(pNMIA->iItem) != NULL)
	{
		CString sOldType;
		int nSel = m_cTypes.GetCurSel();
		int i, n = m_saTypes.GetCount();
		if (IsInArray(nSel, n))
		{
			sOldType = m_saTypes[nSel];
		}

		CString sType = m_cIPItems.GetItemText(pNMIA->iItem, IPCL_TYPE);
		CString sUrl  = m_cIPItems.GetItemText(pNMIA->iItem, IPCL_URL);
		CString sName = m_cIPItems.GetItemText(pNMIA->iItem, IPCL_NAME);
		if (   sOldType != sType
			|| m_sUrl   != sUrl
			|| m_sName  != sName)
		{
			for (i=0; i<n; i++)
			{
				if (sType == m_saTypes[i])
				{
					m_cTypes.SetCurSel(i);
					nSel = i;
					break;
				}
			}
			m_sUrl  = sUrl;
			m_sName = sName;
			CSecID id(m_pUnitControl->GetGroupID(), CSV_CONTROL_ID);
			CString sValue;
			if (!m_sUrl.IsEmpty())
			{
				sValue.Format(_T("%s:%s"), m_saTypes[nSel], m_sUrl);
				m_pUnitControl->DoRequestSetValue(id, CSD_CAM_TYPE, sValue, m_uPort);
			}
			else if (!m_sName.IsEmpty())
			{
				m_pUnitControl->DoRequestSetValue(id, CSD_CAM_CAPTURE, m_sName, 0);
			}
		}
	}
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CCameraParameterPage::OnBnClickedBtnIpPassword()
{
	if (!m_sPassword.IsEmpty())
	{
		CPasswordCheckDialog dlg(this);
		if (IDOK==dlg.DoModal())
		{
			if (dlg.m_Password != m_sPassword)
			{	
				AfxMessageBox(IDP_PASSWORD_INCORRECT);
				return;
			}		    
		}							 
		else						 
		{
			return;
		}
	}

	COEMPasswordDialog dlg(this, m_sUserName, TRUE, TRUE);
	if (IDOK==dlg.DoModal())
	{
		m_sPassword = dlg.GetPassword1();
		SetModified(TRUE, FALSE);
	}
}
