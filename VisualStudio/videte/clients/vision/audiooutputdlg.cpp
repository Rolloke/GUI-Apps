// AudioOutputDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vision.h"
#include "AudioOutputDlg.h"

#include "CIPCControlAudioUnit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAudioOutputDlg dialog
CAudioOutputDlg::CAudioOutputDlg(CIPCAudioVision *pCIPC, CWnd* pParent /*=NULL*/)
	: CDialog(CAudioOutputDlg::IDD, pParent)
{
	m_pCIPC = pCIPC;
	//{{AFX_DATA_INIT(CAudioOutputDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
//////////////////////////////////////////////////////////////////////////
void CAudioOutputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAudioOutputDlg)
	DDX_Control(pDX, IDC_SLIDER_AU_OUTPUT_VOLUME, m_cVolume);
	//}}AFX_DATA_MAP
}
//////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CAudioOutputDlg, CDialog)
	//{{AFX_MSG_MAP(CAudioOutputDlg)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_AU_OUTPUT_VOLUME, OnReleasedcaptureSliderAuOutputVolume)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_NOTIFY_WINDOW, OnNotifyWindow)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAudioOutputDlg message handlers
void CAudioOutputDlg::OnReleasedcaptureSliderAuOutputVolume(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pCIPC)
	{
		int nPos = m_cVolume.GetPos();
		nPos = (int)(log10((double) nPos) * 1000.0 / 3.0);
		m_pCIPC->SetOutputVolume(m_SecID, nPos);
	}
	
	*pResult = 0;
}
//////////////////////////////////////////////////////////////////////////
BOOL CAudioOutputDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	NMHDR *pnmhdr = (NMHDR*)lParam;
	if (pnmhdr->code == TTN_GETDISPINFO)
	{
		NMTTDISPINFO *pDI = (NMTTDISPINFO*)lParam;
		if (wParam == (WPARAM)m_cVolume.m_hWnd)
		{
			int nPos = _ttoi(pDI->szText);
			wsprintf(pDI->szText, _T("%d.%01d %%"), nPos/10, nPos%10);
		}

		*pResult = 0;
	}
	return CDialog::OnNotify(wParam, lParam, pResult);
}
//////////////////////////////////////////////////////////////////////////
LRESULT CAudioOutputDlg::OnNotifyWindow(WPARAM dwCmd, LPARAM nID)
{
	if (nID == MEDIA_IDR_AUDIO_RENDERERS)
	{
		if (MEDIA_COMMAND_VALUE(dwCmd) == MEDIA_VOLUME)
		{
			m_cVolume.EnableWindow();
			int nValue = (int)pow(10.0, m_pCIPC->GetOutputVolume() / (1000.0 / 3.0));
			m_cVolume.SetPos(nValue);
		}
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
BOOL CAudioOutputDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	int i;
	m_cVolume.SetRange(0, 1000);
	for (i=100; i<1000; i+=100)
	{
		m_cVolume.SetTic(i);
	}

	if (m_pCIPC)
	{
		m_pCIPC->SetNotifyWindow(this);
#ifdef _DEBUG
		CIPCMediaRecord *pMR = m_pCIPC->GetMediaRecordPtrFromSecID(m_SecID);
		ASSERT(pMR != NULL);
		if (pMR)
		{
			ASSERT(pMR->IsOutput());
			ASSERT(pMR->IsRenderInitialized());
		}
#endif
		int nValue = m_pCIPC->GetOutputVolume();
		if (nValue == -1)
		{
			m_pCIPC->DoRequestValues(m_SecID, MEDIA_GET_VALUE|MEDIA_VOLUME, MEDIA_IDR_AUDIO_RENDERERS);
			m_cVolume.EnableWindow(FALSE);
		}
		else
		{
			nValue = (int)pow(10.0, nValue / (1000.0 / 3.0));
			m_cVolume.SetPos(nValue);
		}
	}
	
	return TRUE;
}
