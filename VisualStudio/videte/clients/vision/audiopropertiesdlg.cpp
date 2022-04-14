// AudioPropertiesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vision.h"
#include "AudioPropertiesDlg.h"

#include "CIPCControlAudioUnit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAudioPropertiesDlg dialog
CAudioPropertiesDlg::CAudioPropertiesDlg(CIPCAudioVision *pCI,CIPCAudioVision *pCO, CWnd* pParent /*=NULL*/)
	: CDialog(CAudioPropertiesDlg::IDD, pParent)
{
	m_pControlInput  = pCI;
	m_pControlOutput = pCO;
	//{{AFX_DATA_INIT(CAudioPropertiesDlg)
	m_nStereoChannel = -1;
	//}}AFX_DATA_INIT
	m_bStopEncoding = false;
	m_bReduced = true;
	m_bSensitivityInitialized = false;
	m_bVolumeInitialized = false;
}
/////////////////////////////////////////////////////////////////////////////
void CAudioPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAudioPropertiesDlg)
	DDX_Control(pDX, IDC_SLIDER_AU_THRESHOLD, m_cThreshold);
	DDX_Control(pDX, IDC_SLIDER_AU_OUTPUT_VOLUME, m_cOutputVolume);
	DDX_Control(pDX, IDC_SLIDER_AU_INPUT_SENSITIVITY, m_cInputSensitivity);
	DDX_Control(pDX, IDC_COMBO_NAMED_INPUTS, m_cNamedInputs);
	DDX_Control(pDX, IDC_COMBO_AU_INPUT_CHANNELS, m_cAudioInput);
	DDX_Radio(pDX, IDC_RD_AU_LEFT, m_nStereoChannel);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CAudioPropertiesDlg, CDialog)
	//{{AFX_MSG_MAP(CAudioPropertiesDlg)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_AU_INPUT_SENSITIVITY, OnReleasedcaptureSliderAuInputSensitivity)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_AU_OUTPUT_VOLUME, OnReleasedcaptureSliderAuOutputVolume)
	ON_CBN_SELCHANGE(IDC_COMBO_NAMED_INPUTS, OnSelchangeComboNamedInputs)
	ON_CBN_SELCHANGE(IDC_COMBO_AU_INPUT_CHANNELS, OnSelchangeComboAuInputChannels)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_AU_THRESHOLD, OnReleasedcaptureSliderAuThreshold)
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_RD_AU_LEFT, OnRdStereoChannel)
	ON_BN_CLICKED(IDC_RD_AU_RIGHT, OnRdStereoChannel)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_NOTIFY_WINDOWI, OnNotifyWindowI)
	ON_MESSAGE(WM_NOTIFY_WINDOWO, OnNotifyWindowO)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAudioPropertiesDlg message handlers
BOOL CAudioPropertiesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	int i;
	m_cInputSensitivity.SetRange(0, 1000);
	m_cOutputVolume.SetRange(0, 1000);
	for (i=100; i<1000; i+=100)
	{
		m_cInputSensitivity.SetTic(i);
		m_cOutputVolume.SetTic(i);
	}
	
	m_cThreshold.SetRange(0, 100);
	for (i=10; i<100; i+=10)
	{
		m_cThreshold.SetTic(i);
	}

	if (m_pControlInput)
	{
		m_pControlInput->SetNotifyWindow(this, WM_NOTIFY_WINDOWI);
	}
	if (m_pControlOutput)
	{
		m_pControlOutput->SetNotifyWindow(this, WM_NOTIFY_WINDOWO);
		if (m_pControlOutput)
		{
			m_SecIDoutput = m_pControlOutput->GetDefaultOutputID();
			CIPCMediaRecord *pMR = m_pControlOutput->GetMediaRecordPtrFromSecID(m_SecIDoutput);
			if (pMR)
			{
				m_pControlOutput->DoRequestValues(m_SecIDoutput, MEDIA_GET_VALUE|MEDIA_VOLUME, MEDIA_IDR_AUDIO_RENDERERS);
			}
		}
	}
	
	if (m_pControlInput)
	{
		CIPCMediaRecord*pMR = m_pControlInput->GetMediaRecordPtrFromSecID(m_SecIDinput);
		if (pMR && pMR->IsInput())
		{
			if (pMR->IsCaptureInitialized())
			{
				m_pControlInput->RequestConfig(m_SecIDinput);
			}
			else
			{
				if (!m_bReduced)
				{
					m_pControlInput->DoRequestStartMediaEncoding(m_SecIDinput, MEDIA_ENC_ONLY_INITIALIZE|MEDIA_ENC_NO_COMPRESSION, 0);
					m_bStopEncoding = true;
				}
			}
		}
	}

	InitCtrls();
	CWnd *pGrpInput = GetDlgItem(IDC_GRP_AU_INPUT);
	ASSERT(pGrpInput != NULL);
		
	
	if (m_bReduced)
	{
		CRect rcGrp, rcChannels;
		pGrpInput->GetWindowRect(&rcGrp);
		ScreenToClient(&rcGrp);

		GetDlgItem(IDC_COMBO_AU_INPUT_CHANNELS)->GetWindowRect(&rcChannels);
		ScreenToClient(&rcChannels);
		int nY = rcGrp.bottom - rcChannels.top;
		rcGrp.bottom = rcChannels.top;
		GetDlgItem(IDC_GRP_AU_INPUT)->MoveWindow(rcGrp);
		
		m_cThreshold.ShowWindow(SW_HIDE);
		m_cAudioInput.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RD_AU_LEFT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RD_AU_RIGHT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TXT_AU_THRESHOLD)->ShowWindow(SW_HIDE);

		GetWindowRect(&rcGrp);
		SetWindowPos(NULL, 0, 0, rcGrp.Width(), rcGrp.Height()-nY, SWP_NOMOVE|SWP_NOZORDER);
	}

	CString sFmt, sWnd;
	pGrpInput->GetWindowText(sFmt);
	sWnd.Format(sFmt, m_sInput);
	pGrpInput->SetWindowText(sWnd);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CAudioPropertiesDlg::InitCtrls()
{
	if (m_pControlInput)
	{
		int i, j, nSep, nSize, nValue;
		DWORD dwChannel;
		CString      sName;
		nValue = m_pControlInput->GetInputSensitivity();
		if (nValue == -1)
		{
			m_cInputSensitivity.EnableWindow(FALSE);
			m_bSensitivityInitialized = false;
		}
		else
		{
			m_cInputSensitivity.SetPos(nValue);
			m_bSensitivityInitialized = true;
		}

		nValue = m_pControlInput->GetOutputVolume();
		if (nValue == -1)
		{
			m_cOutputVolume.EnableWindow(FALSE);
			m_bVolumeInitialized = false;
		}
		else
		{
			nValue = (int)pow(10.0, nValue / (1000.0 / 3.0));
			m_cOutputVolume.SetPos(nValue);
			m_bVolumeInitialized = true;
		}

		const CStringArray *par = m_pControlInput->GetInputChannels();
		if (par)
		{
			nSize = par->GetSize();
			for (i=0; i<nSize; i++)
			{
				m_cAudioInput.InsertString(i, par->GetAt(i));
			}
		}
		
		nSize = m_pControlInput->GetNamedInputs();
		for (i=0, j=0; i<nSize; i++)
		{
			sName = m_pControlInput->GetNamedInput(i, dwChannel);
			if (HIWORD(dwChannel) == 3)
			{
				nSep = sName.Find(_T(";"));
				if (nSep != -1)
				{
					m_cNamedInputs.InsertString(j, sName.Left(nSep));
					m_cNamedInputs.SetItemData(j++, MAKELONG(LOWORD(dwChannel), 0)); // links
					m_cNamedInputs.InsertString(j, sName.Mid(nSep+1));
					m_cNamedInputs.SetItemData(j++, MAKELONG(LOWORD(dwChannel), 1)); // rechts
				}
				else
				{
					ASSERT(FALSE);
				}
			}
			else if (HIWORD(dwChannel))
			{
				m_cNamedInputs.InsertString(j, sName);
				m_cNamedInputs.SetItemData(j++, MAKELONG(LOWORD(dwChannel), 0));
			}
		}
		m_cNamedInputs.EnableWindow(m_cNamedInputs.GetCount() ? TRUE : FALSE);

		if (m_cAudioInput.GetCount())
		{
			m_cAudioInput.SetCurSel(m_pControlInput->GetInputChannel());
			OnSelchangeComboAuInputChannels();
			m_cAudioInput.EnableWindow(TRUE);
		}
		else
		{
			m_cAudioInput.EnableWindow(FALSE);
		}
		

		nValue = m_pControlInput->GetThreshold();
		if (nValue == -1)
		{
			m_cThreshold.EnableWindow(FALSE);
		}
		else
		{
			m_cThreshold.SetPos(nValue);
		}

		m_nStereoChannel = m_pControlInput->GetStereoChannel();
		BOOL bEnable = FALSE;
		if (m_nStereoChannel != -1)
		{
			CDataExchange dx(this, FALSE);
			DDX_Radio(&dx, IDC_RD_AU_LEFT, m_nStereoChannel);
			bEnable = TRUE;
		}
		GetDlgItem(IDC_RD_AU_LEFT)->EnableWindow(bEnable);
		GetDlgItem(IDC_RD_AU_RIGHT)->EnableWindow(bEnable);
	}
	else
	{
		m_cOutputVolume.EnableWindow(FALSE);
		m_cInputSensitivity.EnableWindow(FALSE);
		m_cThreshold.EnableWindow(FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CAudioPropertiesDlg::OnReleasedcaptureSliderAuInputSensitivity(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pControlInput && m_bSensitivityInitialized)
	{
		TRACE(_T("OnReleasedcaptureSliderAuInputSensitivity\n"));
		m_pControlInput->SetInputSensitivity(m_SecIDinput, m_cInputSensitivity.GetPos());
	}

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CAudioPropertiesDlg::OnReleasedcaptureSliderAuOutputVolume(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pControlOutput && m_bVolumeInitialized)
	{
		TRACE(_T("OnReleasedcaptureSliderAuOutputVolume\n"));
		int nPos = m_cOutputVolume.GetPos();
		nPos = (int)(log10((double) nPos) * 1000.0 / 3.0);
		m_pControlOutput->SetOutputVolume(m_SecIDoutput, nPos);
	}
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CAudioPropertiesDlg::OnReleasedcaptureSliderAuThreshold(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pControlInput)
	{
		m_pControlInput->SetThreshold(m_SecIDinput, m_cThreshold.GetPos());
	}
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CAudioPropertiesDlg::OnSelchangeComboNamedInputs()
{
	int nSel = m_cNamedInputs.GetCurSel();
	if (nSel != CB_ERR)
	{
		DWORD dwChannel  = m_cNamedInputs.GetItemData(nSel);
		m_nStereoChannel = HIWORD(dwChannel);
		CDataExchange dx(this, FALSE);
		DDX_Radio(&dx, IDC_RD_AU_LEFT, m_nStereoChannel);
		int nSelIC = m_cAudioInput.SetCurSel(LOWORD(dwChannel));
		if (m_pControlInput)
		{
			BOOL bChanged = FALSE;
			if (nSelIC != m_pControlInput->GetInputChannel())
			{
				m_pControlInput->SetInputChannel(m_SecIDinput, nSelIC);
				bChanged = TRUE;
			}
			if (m_nStereoChannel != m_pControlInput->GetStereoChannel())
			{
				m_pControlInput->SetStereoChannel(m_SecIDinput, m_nStereoChannel);
				bChanged = TRUE;
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAudioPropertiesDlg::OnSelchangeComboAuInputChannels()
{
	int nIndex = CB_ERR, nSel = m_cAudioInput.GetCurSel();
	if (nSel == CB_ERR) return;

	int i, nSize = m_cNamedInputs.GetCount();
	DWORD dwChannel;
	BOOL  bStereoChanged = TRUE;
	for (i=0; i<nSize; i++)
	{
		dwChannel = m_cNamedInputs.GetItemData(i);
		if (LOWORD(dwChannel) == nSel)
		{
			if (HIWORD(dwChannel) == m_nStereoChannel)
			{
				bStereoChanged = FALSE;
				m_cNamedInputs.SetCurSel(i);
				break;
			}
			nIndex = i;
		}
	}
	
	if (nIndex != CB_ERR)
	{
		nIndex = m_cNamedInputs.SetCurSel(nIndex);
		if (bStereoChanged)
		{
			m_nStereoChannel = !m_nStereoChannel;
			if (m_pControlInput)
			{
				m_pControlInput->SetStereoChannel(m_SecIDinput, m_nStereoChannel);
			}
		}
	}
	CDataExchange dx(this, FALSE);
	DDX_Radio(&dx, IDC_RD_AU_LEFT, m_nStereoChannel);
	if (m_pControlInput)
	{
		m_pControlInput->SetInputChannel(m_SecIDinput, nSel);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAudioPropertiesDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	if (m_pControlInput)
	{
		if (m_bStopEncoding)
		{
			m_pControlInput->DoRequestStopMediaEncoding(m_SecIDinput, 0);
		}
		m_pControlInput->SetNotifyWindow(NULL);
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CAudioPropertiesDlg::OnNotifyWindowI(WPARAM dwCmd, LPARAM nID)
{
	return OnNotifyWindow(m_pControlInput, dwCmd, nID);
}
//////////////////////////////////////////////////////////////////////////
LRESULT CAudioPropertiesDlg::OnNotifyWindowO(WPARAM dwCmd, LPARAM nID)
{
	return OnNotifyWindow(m_pControlOutput, dwCmd, nID);
}
LRESULT CAudioPropertiesDlg::OnNotifyWindow(CIPCAudioVision*pCIPC, WPARAM dwCmd, LPARAM nID)
{
	bool bError = (dwCmd & MEDIA_ERROR_OCCURRED) ? TRUE : FALSE;
	if (nID == MEDIA_IDR_INPUT_PINS)
	{
		switch (MEDIA_COMMAND_VALUE(dwCmd))
		{
			case MEDIA_RECORDING_LEVEL: 
			{
				int nValue = pCIPC->GetInputSensitivity();
				m_cInputSensitivity.EnableWindow(!bError);
				m_cInputSensitivity.SetPos(nValue);
				m_bSensitivityInitialized = !bError;
				TRACE(_T("InitSensitivity %d\n"), m_bSensitivityInitialized);
			}break;
			case MEDIA_THRESHOLD:
				m_cThreshold.EnableWindow(!bError);
				m_cThreshold.SetPos(pCIPC->GetThreshold());
				break;
			case MEDIA_STEREO_CHANNEL:
			{
				CDataExchange dx(this, TRUE);
				m_nStereoChannel = pCIPC->GetStereoChannel();
				DDX_Radio(&dx, IDC_RD_AU_LEFT, m_nStereoChannel);
			}break;
		}
	}
	else if (nID == MEDIA_IDR_AUDIO_RENDERERS)
	{
		switch (MEDIA_COMMAND_VALUE(dwCmd))
		{
			case MEDIA_VOLUME: 
			{
				int nValue = pCIPC->GetOutputVolume();
				if (nValue != -1)
				{
					m_cOutputVolume.EnableWindow(!bError);
					nValue = (int)pow(10.0, nValue / (1000.0 / 3.0));
					m_cOutputVolume.SetPos(nValue);
					m_bVolumeInitialized = !bError;
					TRACE(_T("InitVolume%d\n"), m_bVolumeInitialized);
				}
			} break;
		}
	}
	else if (MEDIA_COMMAND_VALUE(dwCmd) == MEDIA_STATE)
	{
		if (nID == CIPC_MEDIA_REQUESTSTART_ENCODING)
		{
			pCIPC->RequestConfig(m_SecIDinput);
		}
	}
	else if (MEDIA_COMMAND_VALUE(dwCmd) == MEDIA_INPUT_PIN_NAME)
	{
		if (nID == 0)
		{
			InitCtrls();
		}
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAudioPropertiesDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	NMHDR *pnmhdr = (NMHDR*)lParam;
	if (pnmhdr->code == TTN_GETDISPINFO)
	{
		NMTTDISPINFO *pDI = (NMTTDISPINFO*)lParam;
		if (wParam == (WPARAM)m_cOutputVolume.m_hWnd)
		{
			int nPos = _ttoi(pDI->szText);
			wsprintf(pDI->szText, _T("%d.%01d %%"), nPos/10, nPos%10);
		}
		else if (wParam == (WPARAM)m_cInputSensitivity.m_hWnd)
		{
			int nPos = _ttoi(pDI->szText);
			if (nPos > 0)
			{
				wsprintf(pDI->szText, _T("%d.%01d %%"), nPos/10, nPos%10);
			}
			else
			{
				::LoadString(AfxGetInstanceHandle(), IDS_AU_AUTOMATIC_GAIN, pDI->szText, 80);
			}
		}
		else if (wParam == (WPARAM)m_cThreshold.m_hWnd)
		{
			_tcscat(pDI->szText, _T(" %"));
		}

		*pResult = 0;
	}
	return CDialog::OnNotify(wParam, lParam, pResult);
}
/////////////////////////////////////////////////////////////////////////////

void CAudioPropertiesDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CRect rect;
	if (!m_bSensitivityInitialized)
	{
		TRACE(_T("OnLButtonUp Sensitivity\n"));
		m_cInputSensitivity.GetWindowRect(&rect);
		ScreenToClient(&rect);
		if (m_pControlInput && rect.PtInRect(point))
		{
			m_pControlInput->DoRequestValues(m_pControlInput->GetSecID(), MEDIA_GET_VALUE|MEDIA_RECORDING_LEVEL, MEDIA_IDR_INPUT_PINS, 0);
		}
	}
	if (!m_bVolumeInitialized)
	{
		TRACE(_T("OnLButtonUp Volume\n"));
		m_cOutputVolume.GetWindowRect(&rect);
		ScreenToClient(&rect);
		if (m_pControlOutput && rect.PtInRect(point))
		{
			m_pControlOutput->DoRequestValues(m_pControlOutput->GetSecID(), MEDIA_GET_VALUE|MEDIA_VOLUME, MEDIA_IDR_AUDIO_RENDERERS, 0);
		}
	}
	CDialog::OnLButtonUp(nFlags, point);
}


void CAudioPropertiesDlg::OnRdStereoChannel() 
{
	CDataExchange dx(this, TRUE);
	int nOld = m_nStereoChannel;
	DDX_Radio(&dx, IDC_RD_AU_LEFT, m_nStereoChannel);
	if (nOld != m_nStereoChannel)
	{
		int i, nInput = m_pControlInput->GetInputChannel(), 
			    nSize  = m_cNamedInputs.GetCount();

		for (i=0; i<nSize; i++)
		{
			DWORD dwChannel = m_cNamedInputs.GetItemData(i);
			if (   LOWORD(dwChannel) == nInput
				 && HIWORD(dwChannel) == m_nStereoChannel)
			{
				m_cNamedInputs.SetCurSel(i);
				break;
			}
		}
		if (i==nSize)
		{
			m_cNamedInputs.SetCurSel(CB_ERR);
		}
	}
}
