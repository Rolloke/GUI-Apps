// DlgPTZVideo.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "CIPCInputIdipClient.h"
#include "CIPCOutputIdipClient.h"

#include "WndLive.h"
#include "DlgPTZVideo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define AUX_OUT_NAME	_T("PTZF_EX_AUXILIARY_OUT")

/////////////////////////////////////////////////////////////////////////////
//***************************************************************************
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CDlgPTZVideo, CSkinDialog)
// CDlgPTZVideo dialog
CDlgPTZVideo::CDlgPTZVideo(	CIPCInputIdipClient* pInput,CIPCOutputIdipClient* pOutput,
								CameraControlType ccType, DWORD dwPTZFunctionsEx,
								CWnd* pParent /*=NULL*/)
	: CSkinDialog(CDlgPTZVideo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgPTZVideo)
	//}}AFX_DATA_INIT
	m_pWndLive = (CWndLive*)pParent;
	m_pInput = pInput;
	m_pOutput = pOutput;
	m_ccType = ccType;
	m_dwPTZFunctionsEx = dwPTZFunctionsEx;
	m_commID = pInput->GetCommID();
	m_camID = m_pWndLive->GetCamID();

	m_nInitToolTips = FALSE;
	if (UseOEMSkin())
	{
		CreateTransparent(StyleBackGroundColorChangeSimple, m_BaseColor);
	}
	Create(IDD,pParent);
	ShowWindow(SW_SHOW);
}
/////////////////////////////////////////////////////////////////////////////
CDlgPTZVideo::~CDlgPTZVideo()
{

}
/////////////////////////////////////////////////////////////////////////////
void CDlgPTZVideo::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPTZVideo)
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDlgPTZVideo, CSkinDialog)
	//{{AFX_MSG_MAP(CDlgPTZVideo)
	ON_WM_DESTROY()
	ON_CONTROL_RANGE(BN_CLICKED, IDC_PTZ_BTN_SWICTH1, IDC_PTZ_BTN_SWICTH8, OnClicked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CDlgPTZVideo message handlers
BOOL CDlgPTZVideo::OnInitDialog() 
{
	CSkinDialog::OnInitDialog();
	AutoCreateSkinButtons();

	CString sText;
	GetWindowText(sText);
	sText += ' ';
	sText += m_pWndLive->GetName();
	SetWindowText(sText);

	int i, j;
	for (i=IDC_PTZ_BTN_SWICTH1, j=1; i<= IDC_PTZ_BTN_SWICTH8; i++, j++)
	{
		sText.Format(IDS_AUX_OUT, j);
		SetDlgItemText(i, sText);
	}
	DWORD dwTemp = m_dwPTZFunctionsEx;
	m_dwPTZFunctionsEx = 0;
	SetPTZFunctionsEx(dwTemp);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CDlgPTZVideo::SetPTZType(CameraControlType ccType)
{
	m_ccType = ccType;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgPTZVideo::SetPTZFunctionsEx(DWORD dwPTZ_FktEx)
{
	if (dwPTZ_FktEx != m_dwPTZFunctionsEx)
	{
		m_dwPTZFunctionsEx = dwPTZ_FktEx;
		int i;
		_TCHAR cNum;
		DWORD dwFlag = PTZF_EX_AUXILIARY_OUT1;
		CString sKey = AUX_OUT_NAME;
		CString sCam;
		sCam.Format(_T("(%08x)"), m_camID);
		for (i=IDC_PTZ_BTN_SWICTH1, cNum=_T('1'); i<= IDC_PTZ_BTN_SWICTH8; i++, cNum++)
		{
			if (dwFlag & dwPTZ_FktEx)
			{
				GetDlgItem(i)->EnableWindow(TRUE);
				if (m_pInput)
				{
					m_pInput->DoRequestGetValue(m_commID, sKey+cNum+sCam, 0);
				}
			}
			else
			{
				GetDlgItem(i)->EnableWindow(FALSE);
			}
			dwFlag <<= 1;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgPTZVideo::SetFunctionName(LPTSTR pszName)
{
	CString sName(pszName);
	int nLen = _tcslen(AUX_OUT_NAME);
	if (_tcsncmp(AUX_OUT_NAME, sName, nLen) == 0)
	{
		int n = _ttoi(sName.Mid(nLen, 1))-1;
		int nFind = sName.Find(_T(")"));
		if (nFind!= -1)
		{
			sName = sName.Mid(nFind+1);
			SetDlgItemText(IDC_PTZ_BTN_SWICTH1+n, sName);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgPTZVideo::PostNcDestroy() 
{
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgPTZVideo::OnDestroy() 
{
	m_pWndLive->m_pDlgPTZVideo = NULL;
	CSkinDialog::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgPTZVideo::OnCancel() 
{
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgPTZVideo::OnOK() 
{
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgPTZVideo::OnClicked(UINT nID)
{
	int nCheck = SendDlgItemMessage(nID, BM_GETCHECK);
	int nNumber = nID - IDC_PTZ_BTN_SWICTH1 + 1;
	BOOL bSwitch = (BST_CHECKED == nCheck) ? TRUE : FALSE;

	if (m_pInput)
	{
		m_pInput->DoRequestCameraControl(m_commID, m_camID, CCC_AUXOUT, MAKELONG(nNumber, bSwitch));
	}
}
