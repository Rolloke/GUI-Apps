// AudioInputDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vision.h"
#include "AudioInputDlg.h"

#include "CIPCControlAudioUnit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAudioInputDlg dialog
CAudioInputDlg::CAudioInputDlg(CIPCAudioVision *pCIPC, CWnd* pParent /*=NULL*/)
	: CDialog(CAudioInputDlg::IDD, pParent)
{
	m_pCIPC = pCIPC;
	//{{AFX_DATA_INIT(CAudioInputDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
//////////////////////////////////////////////////////////////////////////
void CAudioInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAudioInputDlg)
	DDX_Control(pDX, IDC_SLIDER_AU_INPUT_SENSITIVITY, m_cSensitivity);
	DDX_Control(pDX, IDC_COMBO_NAMED_INPUTS, m_cNamedInputs);
	//}}AFX_DATA_MAP
}

//////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CAudioInputDlg, CDialog)
	//{{AFX_MSG_MAP(CAudioInputDlg)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_AU_INPUT_SENSITIVITY, OnReleasedcaptureSliderAuInputSensitivity)
	ON_CBN_SELCHANGE(IDC_COMBO_NAMED_INPUTS, OnSelchangeComboNamedInputs)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_NOTIFY_WINDOW, OnNotifyWindow)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAudioInputDlg message handlers
void CAudioInputDlg::OnReleasedcaptureSliderAuInputSensitivity(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pCIPC)
	{
		m_pCIPC->SetInputSensitivity(m_SecID, m_cSensitivity.GetPos());
	}
	
	*pResult = 0;
}
//////////////////////////////////////////////////////////////////////////
BOOL CAudioInputDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	NMHDR *pnmhdr = (NMHDR*)lParam;
	if (pnmhdr->code == TTN_GETDISPINFO)
	{
		NMTTDISPINFO *pDI = (NMTTDISPINFO*)lParam;
		if (wParam == (WPARAM)m_cSensitivity.m_hWnd)
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

		*pResult = 0;
	}
	return CDialog::OnNotify(wParam, lParam, pResult);
}
//////////////////////////////////////////////////////////////////////////
BOOL CAudioInputDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	int i;
	m_cSensitivity.SetRange(-1, 1000);
	for (i=100; i<1000; i+=100)
	{
		m_cSensitivity.SetTic(i);
	}
	if (m_pCIPC)
	{
		m_pCIPC->SetNotifyWindow(this);
#ifdef _DEBUG
		CIPCMediaRecord *pMR = m_pCIPC->GetMediaRecordPtrFromSecID(m_SecID);
		ASSERT(pMR != NULL);
		if (pMR)
		{
			ASSERT(pMR->IsInput());
			ASSERT(pMR->IsCaptureInitialized());
		}
#endif

		int nValue = m_pCIPC->GetInputSensitivity();
		if (nValue == -1)
		{
			m_pCIPC->DoRequestValues(m_SecID, MEDIA_GET_VALUE|MEDIA_RECORDING_LEVEL, MEDIA_IDR_INPUT_PINS);
			m_cSensitivity.EnableWindow(FALSE);
		}
		else
		{
			m_cSensitivity.SetPos(nValue);
		}

		if (m_pCIPC->GetNamedInputs() == 0)
		{
			m_pCIPC->DoRequestHardware(m_SecID, MEDIA_IDR_INPUT_PINS);
		}
		else
		{
			InitCtrls();
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
//////////////////////////////////////////////////////////////////////////
LRESULT CAudioInputDlg::OnNotifyWindow(WPARAM dwCmd, LPARAM nID)
{
	if (nID == MEDIA_IDR_INPUT_PINS)
	{
		if (dwCmd & MEDIA_GET_VALUE|MEDIA_SET_VALUE)
		{
			switch(MEDIA_COMMAND_VALUE(dwCmd))
			{
				case   MEDIA_RECORDING_LEVEL:
					m_cSensitivity.EnableWindow();
					m_cSensitivity.SetPos(m_pCIPC->GetInputSensitivity());
					break;
				case MEDIA_STEREO_CHANNEL:
					SelectNamedInput();
					break;
			}
		}
		else if (dwCmd == CB_GETCURSEL)
		{
			SelectNamedInput();
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
//////////////////////////////////////////////////////////////////////////
void CAudioInputDlg::OnSelchangeComboNamedInputs() 
{
	int nSel = m_cNamedInputs.GetCurSel();
	if (nSel != CB_ERR)
	{
		DWORD dwChannel = m_cNamedInputs.GetItemData(nSel);
		if (m_pCIPC->GetInputChannel() != LOWORD(dwChannel))
		{
			m_pCIPC->SetInputChannel(m_SecID, LOWORD(dwChannel));
		}
		if (m_pCIPC->GetStereoChannel() != HIWORD(dwChannel))
		{
			m_pCIPC->SetStereoChannel(m_SecID, HIWORD(dwChannel));
		}
	}
}
//////////////////////////////////////////////////////////////////////////
// Implementation
void CAudioInputDlg::InitCtrls()
{
	if (m_pCIPC)
	{
		m_cNamedInputs.ResetContent();
		int i, j, nSep, nC = m_pCIPC->GetNamedInputs();
		DWORD dwChannel;
		CString sName;
		
		for (i=0, j=0; i<nC; i++)
		{
			sName = m_pCIPC->GetNamedInput(i, dwChannel);
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
		BOOL bSelect = TRUE;
		if (m_pCIPC->GetInputChannel() == -1)
		{
			m_pCIPC->DoRequestValues(m_SecID, CB_GETCURSEL, MEDIA_IDR_INPUT_PINS);
			bSelect = FALSE;
		}

		if (m_pCIPC->GetStereoChannel() == -1)
		{
			m_pCIPC->DoRequestValues(m_SecID, MEDIA_GET_VALUE|MEDIA_STEREO_CHANNEL, MEDIA_IDR_INPUT_PINS);
			bSelect = FALSE;
		}
		if (bSelect)
		{
			SelectNamedInput();
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void	CAudioInputDlg::SelectNamedInput()
{
	if (m_pCIPC)
	{
		DWORD dwSelected = MAKELONG(m_pCIPC->GetInputChannel(), m_pCIPC->GetStereoChannel());
		int i, nC = m_cNamedInputs.GetCount();
		for (i=0; i<nC; i++)
		{
			if (dwSelected == m_cNamedInputs.GetItemData(i))
			{
				m_cNamedInputs.SetCurSel(i);
				break;
			}
		}
	}
}