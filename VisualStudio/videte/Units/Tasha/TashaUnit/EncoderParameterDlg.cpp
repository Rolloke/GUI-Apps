// EncoderParameterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tashaunit.h"
#include "EncoderParameterDlg.h"
#include "TashaUnitDefs.h"
#include ".\encoderparameterdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_UPDATE WM_USER+10

/////////////////////////////////////////////////////////////////////////////
// CEncoderParameterDlg dialog


/////////////////////////////////////////////////////////////////////////////
CEncoderParameterDlg::CEncoderParameterDlg(CWnd* pParent /*=NULL*/, CCodec* pCodec/*=NULL*/)
	: CDialog(CEncoderParameterDlg::IDD, pParent)
{
	m_bVisible		= FALSE;
	m_pCodec		= pCodec;
	
	//{{AFX_DATA_INIT(CEncoderParameterDlg)
	m_EncParam.nBitrate	= 1024;
	m_EncParam.nFps		= 12;
	m_EncParam.nIFrameIntervall = 5;
	m_EncParam.nStreamID = 0;
	m_nCamera	= 0;
	m_nProzessID = 0;
	//}}AFX_DATA_INIT

	m_EncParam.eRes				= eImageResMid;
	m_EncParam.eType			= eTypeMpeg4;  

	CDialog::Create(IDD, pParent);
}


/////////////////////////////////////////////////////////////////////////////
void CEncoderParameterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEncoderParameterDlg)
	DDX_Text(pDX, IDC_EDIT_BITRATE, m_EncParam.nBitrate);
	DDX_Text(pDX, IDC_EDIT_FPS, m_EncParam.nFps);
	DDX_Text(pDX, IDC_EDIT_IFRAME_INTERVALL, m_EncParam.nIFrameIntervall);
	DDX_Text(pDX, IDC_EDIT_PROZESSID, m_nProzessID);
	DDX_Text(pDX, IDC_EDIT_STREAMID, m_EncParam.nStreamID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEncoderParameterDlg, CDialog)
	//{{AFX_MSG_MAP(CEncoderParameterDlg)
	ON_BN_CLICKED(IDC_SEND, OnSendEncoderParameter)
	ON_BN_CLICKED(IDC_RADIO_RESOLUTION_HIGH, OnResolutionHigh)
	ON_BN_CLICKED(IDC_RADIO_RESOLUTION_MID, OnResolutionMid)
	ON_BN_CLICKED(IDC_RADIO_RESOLUTION_LOW, OnResolutionLow)
	ON_BN_CLICKED(IDC_RADIO_MPEG4, OnMpeg4)
	ON_BN_CLICKED(IDC_RADIO_YUV422, OnYuv422)
	ON_MESSAGE(WM_UPDATE, OnUpdate)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_READ, OnReadEncoderParameter)
	ON_BN_CLICKED(IDC_REQ_SINGLE_FIELD, OnBnClickedReqSingleField)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEncoderParameterDlg message handlers

/////////////////////////////////////////////////////////////////////////////

long CEncoderParameterDlg::OnUpdate(WPARAM wParam, LPARAM lParam)
{
	UpdateDlg();

	return 0;
}

void CEncoderParameterDlg::OnSendEncoderParameter() 
{
	if (m_pCodec)
	{  
		UpdateData(TRUE);
	
		m_pCodec->DoRequestSetEncoderParam(m_nProzessID, m_nCamera, m_EncParam);
	}
		
}

/////////////////////////////////////////////////////////////////////////////
void CEncoderParameterDlg::OnReadEncoderParameter()
{
	if (m_pCodec)
	{
		UpdateData(TRUE);
		m_pCodec->DoRequestGetEncoderParam(m_nProzessID, m_nCamera, m_EncParam.nStreamID);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CEncoderParameterDlg::ShowConfigDlg(HWND hWndParent,WORD wSource) 
{
	m_nCamera = (int)wSource;

	if (m_pCodec)
		m_pCodec->DoRequestGetEncoderParam(m_nProzessID, m_nCamera, m_EncParam.nStreamID);

	m_bVisible = TRUE;

	SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);

	if (hWndParent != NULL)
		CenterWindow(FromHandle(hWndParent));

	UpdateDlg();

	ShowWindow(SW_SHOW);
}

/////////////////////////////////////////////////////////////////////////////
void CEncoderParameterDlg::HideConfigDlg() 
{
	m_bVisible = FALSE;
	ShowWindow(SW_HIDE);
}

/////////////////////////////////////////////////////////////////////////////
void CEncoderParameterDlg::OnOK() 
{
	HideConfigDlg();
}

/////////////////////////////////////////////////////////////////////////////
void CEncoderParameterDlg::OnResolutionHigh() 
{
	m_EncParam.eRes = eImageResHigh;
	OnSendEncoderParameter();
}

/////////////////////////////////////////////////////////////////////////////
void CEncoderParameterDlg::OnResolutionMid() 
{
	m_EncParam.eRes = eImageResMid;
	OnSendEncoderParameter();
}

/////////////////////////////////////////////////////////////////////////////
void CEncoderParameterDlg::OnResolutionLow() 
{
	m_EncParam.eRes = eImageResLow;
	OnSendEncoderParameter();
}

/////////////////////////////////////////////////////////////////////////////
void CEncoderParameterDlg::OnMpeg4() 
{
	m_EncParam.eType = eTypeMpeg4;
	OnSendEncoderParameter();
}

/////////////////////////////////////////////////////////////////////////////
void CEncoderParameterDlg::OnYuv422() 
{
	m_EncParam.eType = eTypeYUV422;
	OnSendEncoderParameter();
}

/////////////////////////////////////////////////////////////////////////////
void CEncoderParameterDlg::UpdateDlg()
{
	switch (m_EncParam.eRes)
	{
		case eImageResUltra:// 4CIF (Vollbild)
			// TODO not yet implemented
			break;
		case eImageResHigh: // 2CIF (Halbbild)
			CheckRadioButton(IDC_RADIO_RESOLUTION_HIGH, IDC_RADIO_RESOLUTION_LOW, IDC_RADIO_RESOLUTION_HIGH);
			break;
		case eImageResMid:	//CIF
			CheckRadioButton(IDC_RADIO_RESOLUTION_HIGH, IDC_RADIO_RESOLUTION_LOW, IDC_RADIO_RESOLUTION_MID);
			break;
		case eImageResLow: //QCIF
			CheckRadioButton(IDC_RADIO_RESOLUTION_HIGH, IDC_RADIO_RESOLUTION_LOW, IDC_RADIO_RESOLUTION_LOW);
			break;
		case eImageResUnvalid:
			break;
		default:
			WK_TRACE_WARNING(_T("CEncoderParameterDlg::UpdateDlg: Unknown Imageresolution (%d)\n"), m_EncParam.eRes);
	}
	switch (m_EncParam.eType)
	{
		case eTypeMpeg4:
			CheckRadioButton(IDC_RADIO_MPEG4, IDC_RADIO_YUV422, IDC_RADIO_MPEG4);
			break;
		case eTypeYUV422:
			CheckRadioButton(IDC_RADIO_MPEG4, IDC_RADIO_YUV422, IDC_RADIO_YUV422);
			break;
		case eTypeUnvalid:
			break;
		default:
			WK_TRACE_WARNING(_T("CEncoderParameterDlg::UpdateDlg: Unknown Datatype (%d)\n"), m_EncParam.eType);
	}


	UpdateData(FALSE);
}

//////////////////////////////////////////////////////////////////////
BOOL CEncoderParameterDlg::OnConfirmSetEncoderParam(DWORD dwUserData, WORD wSource, EncoderParam EncParam)
{
	m_nCamera  = wSource;
	m_EncParam = EncParam;
	PostMessage(WM_UPDATE);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CEncoderParameterDlg::OnConfirmGetEncoderParam(DWORD dwUserData, WORD wSource, EncoderParam EncParam)
{
	m_nCamera  = wSource;
	m_EncParam = EncParam;
	PostMessage(WM_UPDATE);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
void CEncoderParameterDlg::OnBnClickedReqSingleField()
{
	if (m_pCodec)
	{
		UpdateData(TRUE);
		m_pCodec->DoRequestNewSingleFrame(m_nProzessID, m_nCamera);
	}
}
