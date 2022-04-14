// SoftwarePage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "AudioPage.h"
#include "IPCControlAudioUnit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ALL_CONTROLLS 2


#define SEQUENCE_62_5 0
#define SEQUENCE_125  1
#define SEQUENCE_250  2
#define SEQUENCE_500  3
#define SEQUENCE_1000 4

#define EXTENDED_SETTINGS _T("ExtendedSettings")
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CAudioPage, CSVPage)
/////////////////////////////////////////////////////////////////////////////
// CAudioPage dialog
CAudioPage::CAudioPage(CSVView*  pParent, int nPage) : CSVPage(CAudioPage::IDD)
{
	m_pParent = pParent;
	//{{AFX_DATA_INIT(CAudioPage)
	m_b2BytesPerSample = FALSE;
	m_nSequence  = -1;
	m_nThreshold = 0;
	m_nDwellTime = 0;
	m_bDetectorActive = FALSE;
	m_bUnitActive = FALSE;
	m_nChannel = 0;
	m_bListen = FALSE;
	m_bDefaultRecDev = FALSE;
	m_bDefaultPlayDev = FALSE;
	//}}AFX_DATA_INIT

	m_nPageNo = nPage;
	CWK_Profile &prof = m_pParent->GetDocument()->GetProfile();
	m_sBase           = prof.GetSection() + _T("\\");
	m_bIsLocal        = m_pParent->GetDocument()->IsLocal() ? true : false;

	m_sKey.Format(AUDIO_UNIT_DEVICE, m_nPageNo);
	m_sKey = m_sBase + m_sKey;

	m_sUnitName = GetUnitName(m_nPageNo);
	
	m_pCIPCcontrolUnit       = NULL;

	m_nInputPin              = -1;
	m_nAudioCapture          = -1;
	m_nRequestUnitTimer = 0;
	m_bOldUnitActive = IsUnitActive(prof, m_nPageNo);
	m_bRecordinLevelInitialized = false;
	m_bOutputVolumeInitialized  = false;
	m_bUpdateHardwareBranch     = false;
	m_bActivateUnit             = false;
	m_bDeactivateUnit           = false;
	m_bDeleteActivations        = false;
	m_bDeleteInputActivations   = false;
	m_lWndProcLevel = 0;
	m_lWndProcLevelPeak = 0;
	Create();
}
//////////////////////////////////////////////////////////////////////////
CAudioPage::~CAudioPage()
{
	if (m_bActivateUnit || m_bDeactivateUnit)
	{
		m_bUnitActive = m_bOldUnitActive;
		SaveLauncherSettings();
	}
	WK_DELETE(m_pCIPCcontrolUnit);
}
//////////////////////////////////////////////////////////////////////////
void CAudioPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAudioPage)
	DDX_Control(pDX, IDC_SLIDER_OUTPUT_VOLUME, m_cOutputVolume);
	DDX_Control(pDX, IDC_PROGRESS_INPUT_LEVEL_PEAK, m_cInputLevelPeak);
	DDX_Control(pDX, IDC_TXT_ACTIVATION, m_cActivating);
	DDX_Control(pDX, IDC_PROGRESS_INPUT_LEVEL, m_cInputLevel);
	DDX_Control(pDX, IDC_COMBO_AUDIO_QUALITY, m_cQuality);
	DDX_Control(pDX, IDC_SLIDER_INPUT_GAIN, m_cInputGain);
	DDX_Control(pDX, IDC_COMBO_AUDIO_INPUTPINS, m_cInputPins);
	DDX_Control(pDX, IDC_COMBO_AUDIO_SAMP_FREQUENCY, m_cSampFrq);
	DDX_Control(pDX, IDC_COMBO_AUDIO_RENDERER, m_cRenderer);
	DDX_Control(pDX, IDC_COMBO_AUDIO_COMPRESSOR, m_cCompressor);
	DDX_Control(pDX, IDC_COMBO_AUDIO_CAPTURE, m_cCapture);
	DDX_Check(pDX, IDC_CK_AUDIO_2BYTES_PER_SAMP, m_b2BytesPerSample);
	DDX_CBIndex(pDX, IDC_COMBO_AUDIO_LENGTH_OF_SEQUENCE, m_nSequence);
	DDX_Text(pDX, IDC_EDT_AUDIO_THRESHOLD, m_nThreshold);
	DDV_MinMaxInt(pDX, m_nThreshold, 0, 100);
	DDX_Text(pDX, IDC_EDT_AUDIO_DWELL_TIME, m_nDwellTime);
	DDV_MinMaxInt(pDX, m_nDwellTime, 1, 60);
	DDX_Check(pDX, IDC_CK_AUDIO_DETECTOR, m_bDetectorActive);
	DDX_Check(pDX, IDC_CK_AUDIO_UNIT_ACTIVE, m_bUnitActive);
	DDX_Radio(pDX, IDC_RD_AUDIO_LEFT_CHANNEL, m_nChannel);
	DDX_Check(pDX, IDC_CK_AUDIO_INPUT_LISTEN, m_bListen);
	DDX_Check(pDX, IDC_CK_AUDIO_DEFAULT_REC, m_bDefaultRecDev);
	DDX_Check(pDX, IDC_CK_AUDIO_DEFAULT_PLAY, m_bDefaultPlayDev);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
	}
	else
	{
		if (IsBetween(m_nInputPin, 0, m_cInputPins.GetCount()))
		{
			m_cInputPins.SetCurSel(m_nInputPin);
		}
		if (IsBetween(m_nQuality, 0, m_cQuality.GetCount()))
		{
			m_cQuality.SetCurSel(m_nQuality);
		}
		int nSel = m_cSampFrq.SetCurSel(m_cSampFrq.FindStringExact(-1, m_sSampleFrq));
		if (nSel == -1)
		{
			m_cSampFrq.SetCurSel(0);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CAudioPage, CSVPage)
	//{{AFX_MSG_MAP(CAudioPage)
	ON_BN_CLICKED(IDC_CK_AUDIO_2BYTES_PER_SAMP, OnChange)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO_AUDIO_CAPTURE, OnSelchangeComboAudioCapture)
	ON_WM_LBUTTONUP()
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_INPUT_GAIN, OnReleasedcaptureSliderInputGain)
	ON_CBN_SELCHANGE(IDC_COMBO_AUDIO_INPUTPINS, OnSelchangeComboAudioInputpins)
	ON_CBN_SELCHANGE(IDC_COMBO_AUDIO_COMPRESSOR, OnSelchangeComboAudioCompressor)
	ON_CBN_SELCHANGE(IDC_COMBO_AUDIO_LENGTH_OF_SEQUENCE, OnSelchangeComboAudioLengthOfSequence)
	ON_CBN_SELCHANGE(IDC_COMBO_AUDIO_RENDERER, OnSelchangeComboAudioRenderer)
	ON_CBN_SELCHANGE(IDC_COMBO_AUDIO_SAMP_FREQUENCY, OnSelchangeComboAudioSampFrequency)
	ON_BN_CLICKED(IDC_BTN_TEST_COMPRESSION_METHOD, OnBtnTestCompressionMethod)
	ON_EN_KILLFOCUS(IDC_EDT_AUDIO_THRESHOLD, OnKillfocusEdtAudioThreshold)
	ON_CBN_SELCHANGE(IDC_COMBO_AUDIO_QUALITY, OnSelchangeComboAudioQuality)
	ON_BN_CLICKED(IDC_CK_AUDIO_UNIT_ACTIVE, OnCkAudioUnitActive)
	ON_BN_CLICKED(IDC_CK_AUDIO_INPUT_LEVEL, OnCkAudioInputLevel)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_INPUT_GAIN, OnCustomdrawSliderInputGain)
	ON_BN_CLICKED(IDC_CK_AUDIO_INPUT_LISTEN, OnCkAudioInputListen)
	ON_BN_CLICKED(IDC_BTN_AUDIO_VOLUME_CTRL, OnBtnAudioVolumeCtrl)
	ON_BN_CLICKED(IDC_CK_AUDIO_DETECTOR, OnCkAudioDetector)
	ON_EN_CHANGE(IDC_EDT_AUDIO_DWELL_TIME, OnChange)
	ON_BN_CLICKED(IDC_CK_AUDIO_DEFAULT_PLAY, OnChange)
	ON_BN_CLICKED(IDC_CK_AUDIO_DEFAULT_REC, OnChange)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_OUTPUT_VOLUME, OnCustomdrawSliderOutputVolume)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_OUTPUT_VOLUME, OnReleasedcaptureSliderOutputVolume)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER, OnUser)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAudioPage message handlers
BOOL CAudioPage::OnInitDialog() 
{
	int i;
	Initialize();
	CSVPage::OnInitDialog();
	CWnd *pWnd = GetDlgItem(IDC_CK_AUDIO_UNIT_ACTIVE);
	if (pWnd)
	{
		CString sFormat, sText;
		pWnd->GetWindowText(sFormat);
		sText.Format(sFormat, m_nPageNo);
		pWnd->SetWindowText(sText);
	}

	m_cInputGain.SetRange(0, 1000);
	for (i=100; i<1000; i+=100)
	{
		m_cInputGain.SetTic(i); 
	}
	m_cInputLevel.SetRange32(0, 100);
	m_cInputLevelPeak.SetRange32(0, 100);

	m_cOutputVolume.SetRange(0, 1000);
	for (i=100; i<1000; i+=100)
	{
		m_cOutputVolume.SetTic(i);
	}

	if (!theApp.IsNT40())
	{	
		ASSERT(GetWindowLong(m_cInputLevel.m_hWnd, GWL_USERDATA) == 0);
		SetWindowLong(m_cInputLevel.m_hWnd, GWL_USERDATA, (long)this);
		m_lWndProcLevel = SetWindowLong(m_cInputLevel.m_hWnd, GWL_WNDPROC, (long)CAudioPage::WndProcLevel);

		ASSERT(GetWindowLong(m_cInputLevelPeak.m_hWnd, GWL_USERDATA) == 0);
		SetWindowLong(m_cInputLevelPeak.m_hWnd, GWL_USERDATA, (long)this);
		m_lWndProcLevelPeak = SetWindowLong(m_cInputLevelPeak.m_hWnd, GWL_WNDPROC, (long)CAudioPage::WndProcLevel);
	
		if (theApp.IsWinXP())
		{
			if (theApp.IsThemeActive())
			{
				theApp.SetWindowTheme(m_cInputLevelPeak.m_hWnd, L" ", L" ");
				theApp.SetWindowTheme(m_cInputLevel.m_hWnd, L" ", L" ");
			}
		}
	}

	CWK_Profile &wkpBase = m_pParent->GetDocument()->GetProfile();
	BOOL bDisable = FALSE;
	if (m_nPageNo > 1)	// nicht die erste
	{// darf nicht aktiviert werden, wenn die davor liegende Unit nicht aktiv ist
		if (!IsUnitActive(wkpBase, m_nPageNo-1)) bDisable = ALL_CONTROLLS;
	}

	if (m_nPageNo < SM_NO_OF_AUDIO_UNITS)	// nicht die letzte Unit
	{// darf nicht deaktiviert werden, wenn die dahinter liegende Unit aktiv ist
		if (IsUnitActive(wkpBase, m_nPageNo+1)) bDisable = TRUE;
	}

	if (bDisable)
	{
		GetDlgItem(IDC_CK_AUDIO_UNIT_ACTIVE)->EnableWindow(FALSE);
	}

	GetDlgItem(IDC_CK_AUDIO_DETECTOR)->EnableWindow(FALSE);
	EnableGrpWindows(IDC_GRP_AUDIO_RECORD, FALSE);
	EnableGrpWindows(IDC_GRP_AUDIO_TEST, FALSE);
	EnableGrpWindows(IDC_GRP_AUDIO_RENDERER, FALSE);
	EnableGrpWindows(IDC_GRP_AUDIO_REC_EX, FALSE);
	
	if (m_bUnitActive && (bDisable != 2))
	{
		ControlUnit(TRUE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CAudioPage::OnChange() 
{
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CAudioPage::OnDestroy() 
{
	HWND hWnd = m_hWnd;
	m_hWnd = NULL;			// to deny any PostMessage in ControlUnit(..)
	ControlUnit(FALSE);	
	m_hWnd = hWnd;
	CSVPage::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
void CAudioPage::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == m_nRequestUnitTimer)
	{
		ControlUnit(TRUE);
	}

	CSVPage::OnTimer(nIDEvent);
}
/////////////////////////////////////////////////////////////////////////////
void CAudioPage::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_pCIPCcontrolUnit)
	{
		CRect rect;
		if (!m_cInputGain.IsWindowEnabled())
		{
			m_cInputGain.GetWindowRect(&rect);
			ScreenToClient(&rect);
			if (rect.PtInRect(point))
			{
				m_pCIPCcontrolUnit->DoRequestValues(m_pCIPCcontrolUnit->GetSecID(), MEDIA_GET_VALUE|MEDIA_RECORDING_LEVEL, MEDIA_IDR_INPUT_PINS, 0);
			}
		}
		if (!m_cOutputVolume.IsWindowEnabled())
		{
			m_cOutputVolume.GetWindowRect(&rect);
			ScreenToClient(&rect);
			if (rect.PtInRect(point))
			{
				m_pCIPCcontrolUnit->DoRequestValues(m_pCIPCcontrolUnit->GetSecID(), MEDIA_GET_VALUE|MEDIA_VOLUME, MEDIA_IDR_AUDIO_RENDERERS, 0);
			}
		}
		if (m_cInputPins.GetCount() == 0)
		{
			m_pCIPCcontrolUnit->DoRequestHardware(m_pCIPCcontrolUnit->GetSecID(), MEDIA_IDR_INPUT_PINS);
		}
	}
	
	CSVPage::OnLButtonUp(nFlags, point);
}
//////////////////////////////////////////////////////////////////////////
void CAudioPage::OnBtnAudioVolumeCtrl() 
{
	ShellExecute(m_hWnd, _T("open"), _T("SNDVOL32.EXE"), NULL, NULL, SW_SHOW);
}
/////////////////////////////////////////////////////////////////////////////
void CAudioPage::OnReleasedcaptureSliderInputGain(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pCIPCcontrolUnit && m_bRecordinLevelInitialized)
	{
		DWORD dwValue = m_cInputGain.GetPos();
		m_pCIPCcontrolUnit->DoRequestValues(m_pCIPCcontrolUnit->GetSecID(), MEDIA_SET_VALUE|MEDIA_RECORDING_LEVEL, MEDIA_IDR_INPUT_PINS, dwValue);
	}
	
	*pResult = 0;
}
//////////////////////////////////////////////////////////////////////////
void CAudioPage::OnCustomdrawSliderInputGain(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (GetCapture() == 0)
	{
		OnReleasedcaptureSliderInputGain(pNMHDR, pResult);
	}

	*pResult = 0;
}
//////////////////////////////////////////////////////////////////////////
void CAudioPage::OnReleasedcaptureSliderOutputVolume(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pCIPCcontrolUnit && m_bOutputVolumeInitialized)
	{
		int nPos = m_cOutputVolume.GetPos();
		DWORD dwValue = (DWORD)(log10((double) nPos) * 1000.0 / 3.0);	// log
//		DWORD dwValue = m_cOutputVolume.GetPos();						// lin
		m_pCIPCcontrolUnit->DoRequestValues(m_pCIPCcontrolUnit->GetSecID(), MEDIA_SET_VALUE|MEDIA_SAVE_IN_REGISTRY|MEDIA_VOLUME, MEDIA_IDR_AUDIO_RENDERERS, dwValue);
	}
	
	*pResult = 0;
}
//////////////////////////////////////////////////////////////////////////
void CAudioPage::OnCustomdrawSliderOutputVolume(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (GetCapture() == 0)
	{
		OnReleasedcaptureSliderOutputVolume(pNMHDR, pResult);
	}
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CAudioPage::OnSelchangeComboAudioCapture() 
{
	int nSel = m_cCapture.GetCurSel();
	if (m_nAudioCapture != nSel)
	{
		m_nAudioCapture = nSel;
		if (m_pCIPCcontrolUnit)
		{
			CString str;
			CSecID secID = m_pCIPCcontrolUnit->GetSecID();
			m_cCapture.GetLBText(nSel, str);
			m_sCapture = str;
			CIPCExtraMemory em;
			em.Create(m_pCIPCcontrolUnit, str);
			m_pCIPCcontrolUnit->DoRequestValues(secID, CB_SELECTSTRING, MEDIA_IDR_AUDIOINPUTDEVICES, m_nAudioCapture, &em);
		}
		OnChange();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAudioPage::OnSelchangeComboAudioQuality() 
{
	int nQuality = m_cQuality.GetCurSel();
	if (m_nQuality != nQuality)
	{
		m_nQuality = nQuality;
		OnChange();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAudioPage::OnSelchangeComboAudioInputpins() 
{
	int nSel = m_cInputPins.GetCurSel();
	if (m_nInputPin != nSel)
	{
		m_nInputPin = nSel;
		if (m_pCIPCcontrolUnit)
		{
			CString str;
			CSecID secID = m_pCIPCcontrolUnit->GetSecID();
			m_pCIPCcontrolUnit->DoRequestValues(secID, CB_SELECTSTRING, MEDIA_IDR_INPUT_PINS, m_nInputPin, NULL);
			m_pCIPCcontrolUnit->DoRequestValues(secID, MEDIA_GET_VALUE|MEDIA_BALANCE, MEDIA_IDR_INPUT_PINS, m_nInputPin);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAudioPage::OnSelchangeComboAudioCompressor() 
{
	CString sCompressor;
	int nSel = m_cCompressor.GetCurSel();
	m_cCompressor.GetLBText(nSel, sCompressor);
	if (m_sCompressor != sCompressor)
	{
		m_sCompressor = sCompressor;
		if (m_pCIPCcontrolUnit)
		{
			m_pCIPCcontrolUnit->DoRequestValues(m_pCIPCcontrolUnit->GetSecID(), CB_SELECTSTRING, MEDIA_IDR_AUDIOCOMPRESSORS, nSel);
		}
		OnChange();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAudioPage::OnSelchangeComboAudioLengthOfSequence() 
{
	OnChange();
}
/////////////////////////////////////////////////////////////////////////////
void CAudioPage::OnSelchangeComboAudioRenderer() 
{
	m_cRenderer.GetLBText(m_cRenderer.GetCurSel(), m_sRenderer);
	OnChange();
}
/////////////////////////////////////////////////////////////////////////////
void CAudioPage::OnSelchangeComboAudioSampFrequency() 
{
	int nSel = m_cSampFrq.GetCurSel();
	CString str;
	m_cSampFrq.GetLBText(nSel, str);
	if (m_sSampleFrq != str)
	{
		m_sSampleFrq = str;
		if (m_pCIPCcontrolUnit)
		{
			CSecID secID = m_pCIPCcontrolUnit->GetSecID();
			m_pCIPCcontrolUnit->DoRequestValues(secID, CB_SETCURSEL, MEDIA_SAMPLE_FREQUENCY, nSel);
		}
		OnChange();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAudioPage::OnBtnTestCompressionMethod() 
{
	if (m_pCIPCcontrolUnit)
	{
		m_pCIPCcontrolUnit->DoRequestHardware(m_pCIPCcontrolUnit->GetSecID(), MEDIA_IDR_TEST_CONFIG);
	}
}
//////////////////////////////////////////////////////////////////////////
void CAudioPage::OnKillfocusEdtAudioThreshold() 
{
	int nOldThreshold = m_nThreshold;
	UpdateData();
	if (nOldThreshold != m_nThreshold)
	{
		if (m_pCIPCcontrolUnit)
		{
			CSecID secID  = m_pCIPCcontrolUnit->GetSecID();
			DWORD dwValue = m_nThreshold;
			m_pCIPCcontrolUnit->DoRequestValues(secID, MEDIA_SET_VALUE|MEDIA_THRESHOLD, MEDIA_IDR_INPUT_PINS, dwValue);
		}
		m_cInputLevel.InvalidateRect(NULL);
		m_cInputLevelPeak.InvalidateRect(NULL);
		OnChange();
	}
}
//////////////////////////////////////////////////////////////////////////
void CAudioPage::OnCkAudioUnitActive() 
{
	BOOL bOldUnitActive = m_bUnitActive;
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_AUDIO_UNIT_ACTIVE, m_bUnitActive);
	dx.m_bSaveAndValidate = FALSE;

	if (m_bUnitActive)											// aktivate ?
	{
		m_bActivateUnit           = true;
		m_bDeactivateUnit         = false;
		m_bDeleteInputActivations = false;
		m_bDeleteActivations      = false;
	}
	else														// deactivate
	{
		m_bDeactivateUnit = true;
		m_bActivateUnit   = false;

		if (m_bDetectorActive)
		{
			BOOL bCheck= CheckInputActivations(SVP_CO_CHECK);	// check with request
			if (bCheck > 0)										// inputs still active
			{
				m_bUnitActive = TRUE;
				DDX_Check(&dx, IDC_CK_AUDIO_UNIT_ACTIVE, m_bUnitActive);
			}
			else if (bCheck < 0)								// inputs will be deleted
			{
				m_bDeleteInputActivations = true;
				m_bDeleteActivations = true;
			}
		}

		if (   !m_bUnitActive									// deactivate Unit 
			&& !m_bDeleteActivations)							// request to delete not already confirmed
		{
			BOOL bCheck = CheckMediaActivations(SVP_CO_CHECK);
			if (bCheck > 0)										// Outputs still active
			{
				m_bUnitActive = TRUE;
				DDX_Check(&dx, IDC_CK_AUDIO_UNIT_ACTIVE, m_bUnitActive);
			}
			else if (bCheck < 0)								// Outputs will be deleted
			{
				m_bDeleteActivations = true;
			}
		}
		if (!m_bUnitActive)
		{
			if (m_pCIPCcontrolUnit)
			{
				m_pCIPCcontrolUnit->DoRequestStopMediaEncoding(m_pCIPCcontrolUnit->GetSecID(), 1);
			}
			GetDlgItem(IDC_CK_AUDIO_DETECTOR)->EnableWindow(FALSE);
			EnableGrpWindows(IDC_GRP_AUDIO_RECORD, FALSE);
			EnableGrpWindows(IDC_GRP_AUDIO_TEST, FALSE);
			EnableGrpWindows(IDC_GRP_AUDIO_RENDERER, FALSE);
			EnableGrpWindows(IDC_GRP_AUDIO_REC_EX, FALSE);
		}
	}
	if (bOldUnitActive != m_bUnitActive)
	{
		SaveLauncherSettings();											// start or stop module
		ControlUnit(m_bUnitActive);
	}
	else
	{
		m_bActivateUnit   = false;
		m_bDeactivateUnit = false;
	}
}
//////////////////////////////////////////////////////////////////////////
void CAudioPage::OnCkAudioDetector() 
{
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_AUDIO_DETECTOR, m_bDetectorActive);
	if (!m_bDetectorActive)
	{
		BOOL bCheck = CheckInputActivations();
		if (bCheck>0)
		{
			m_bDetectorActive = TRUE;
			dx.m_bSaveAndValidate = FALSE;
			DDX_Check(&dx, IDC_CK_AUDIO_DETECTOR, m_bDetectorActive);
			return; // no changes
		}
		else if (bCheck < 0)
		{
			m_bDeleteInputActivations = true;
		}
	}
	OnChange();	
}
//////////////////////////////////////////////////////////////////////////
BOOL CAudioPage::CheckInputActivations(int nCheckOptions)
{
	CString sInput;
	sInput.Format(SM_AudioUnitInput, m_nPageNo);
	CInputList  * pInputList  = m_pParent->GetDocument()->GetInputs();
	CInputGroup * pInputGroup = pInputList->GetGroupBySMName(sInput);
	if (pInputGroup)
	{
		CInput *pInput = pInputGroup->GetInput(0);
		if (pInput)
		{
			if (pInput->GetNumberOfInputToOutputs())
			{
				CString sMsg, sFormat;
				sFormat.LoadString(IDS_DELETE_INPUT);
				sMsg.Format(sFormat, pInput->GetName());
				if (nCheckOptions == SVP_CO_CHECK_ONLY)
				{
					return TRUE;
				}
				else if (nCheckOptions == SVP_CO_CHECK_SILENT)
				{
					pInput->DeleteAllInputToOutputs();
					return -1;
				}
				if (IDYES == AfxMessageBox(sMsg, MB_YESNO))
				{
					return -1;
				}
				else
				{
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////
BOOL CAudioPage::CheckMediaActivations(int bCheckOptions)
{
	CString sMedia;				// check for activations
	sMedia.Format(SM_AudioUnitMedia, m_nPageNo);
	CMediaList  * pMediaList  = m_pParent->GetDocument()->GetMedias();
	CMediaGroup * pMediaGroup = pMediaList->GetGroupBySMName(sMedia);
	if (pMediaGroup)
	{
		int i, nSize = pMediaGroup->GetSize();
		for (i=0; i<nSize; i++)
		{
			if (bCheckOptions == SVP_CO_CHECK)
			{
				if (CheckActivationsWithOutput(pMediaGroup->GetMedia(i)->GetID(), 0, SVP_CO_CHECK_ONLY) == FALSE)
				{
					if (AfxMessageBox(IDP_DELETE_AUDIO_WITH_ACTIVATION, MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
					{
						return -1;
					}
					else
					{
						return TRUE;
					}
				}
			}
			else
			{
				CheckActivationsWithOutput(pMediaGroup->GetMedia(i)->GetID(), 0, bCheckOptions);
			}
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////
void CAudioPage::OnCkAudioInputLevel() 
{
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_AUDIO_INPUT_LEVEL, m_bInputLevel);
	if (m_pCIPCcontrolUnit)
	{
		CSecID secID = m_pCIPCcontrolUnit->GetSecID();
		if (m_bInputLevel)
		{
			m_pCIPCcontrolUnit->DoRequestStartMediaEncoding(secID, MEDIA_ENC_CLIENT_REQUEST|MEDIA_ENC_DONT_CHANGE_STATES, 0);
		}
		else
		{
			m_cInputLevel.SetPos(0);
			m_cInputLevelPeak.SetPos(0);
		}
		m_cInputLevel.EnableWindow(m_bInputLevel);
		m_cInputLevelPeak.EnableWindow(m_bInputLevel);
		m_pCIPCcontrolUnit->DoRequestValues(secID, MEDIA_SET_VALUE|MEDIA_START_VOLUME_LEVEL, MEDIA_IDR_INPUT_PINS, m_bInputLevel);
	}
}
//////////////////////////////////////////////////////////////////////////
void CAudioPage::OnCkAudioInputListen() 
{
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_AUDIO_INPUT_LISTEN, m_bListen);
	if (m_pCIPCcontrolUnit)
	{
		CSecID secID = m_pCIPCcontrolUnit->GetSecID();
		m_pCIPCcontrolUnit->DoRequestValues(secID, MEDIA_SET_VALUE|MEDIA_START_LOCAL_REPLAY, MEDIA_IDR_INPUT_PINS, m_bListen);
	}
}
//////////////////////////////////////////////////////////////////////////
// Implementation
void CAudioPage::Create()
{
	CSVPage::Create(IDD,(CWnd*)m_pParent);
}
/////////////////////////////////////////////////////////////////////////////
void CAudioPage::ControlUnit(BOOL bControl)
{
	if (m_bIsLocal)
	{
		HWND hwnd = ::FindWindow(NULL, m_sUnitName);
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
				if ((dwError == 0) && (dwResult != 0))
				{
					CString str;
					str.Format(SM_AudioUnitMedia, m_nPageNo);
					str += _T("Control");
					m_pCIPCcontrolUnit = new CIPCControlAudioUnit(str, TRUE, this);
				}
			}
			else if (m_pCIPCcontrolUnit)
			{
				m_nRequestUnitTimer = 0xffffffff;
				CSecID secID = m_pCIPCcontrolUnit->GetSecID();
				if (m_bListen)
				{
					m_bListen = FALSE;
					m_pCIPCcontrolUnit->DoRequestValues(secID, MEDIA_SET_VALUE|MEDIA_START_LOCAL_REPLAY, MEDIA_IDR_INPUT_PINS, m_bListen);
				}
				if (m_bInputLevel)
				{
					m_bInputLevel = FALSE;
					m_pCIPCcontrolUnit->DoRequestValues(secID, MEDIA_SET_VALUE|MEDIA_START_VOLUME_LEVEL, MEDIA_IDR_INPUT_PINS, m_bInputLevel);
					m_pCIPCcontrolUnit->DoRequestStopMediaDecoding(secID, 0);
				}
				if (m_hWnd)
				{
					CDataExchange dx(this, FALSE);
					DDX_Check(&dx, IDC_CK_AUDIO_INPUT_LISTEN, m_bListen);
					DDX_Check(&dx, IDC_CK_AUDIO_INPUT_LEVEL, m_bInputLevel);
					PostMessage(WM_USER, 1);	// Deletes (m_pCIPCcontrolUnit)
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
			}
			else
			{
				m_cActivating.GetWindowText(str);
				str += _T(".");
				m_cActivating.SetWindowText(str);
			}
			bControl = FALSE;
		}
	}
	else
	{
		// nicht lokal nur bestimmte Einstellungen ?
	}

	if (m_pCIPCcontrolUnit && m_bActivateUnit)
	{
		m_bActivateUnit = false;
		OnChange();
	}
	else if (bControl == FALSE && m_bDeactivateUnit)
	{
		m_bDeactivateUnit = false;
		OnChange();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAudioPage::OnConfirmHardware(int errorCode, DWORD dwFlags, DWORD dwRequestID, const CIPCExtraMemory *pEM)
{
	CStringArray *psa   = NULL;
	CComboBox    *pCB   = NULL;
	CString      *pstr  = NULL;
	int           nSel  = -1,
                *pnSel = &nSel;
	
	if (dwRequestID == 0)
	{
		if (errorCode == 0)
		{
			if (pEM) psa = pEM->GetStrings();
			if (psa)
			{
				m_sCapture    = psa->GetAt(0);
				m_sCompressor = psa->GetAt(1);
				m_sRenderer   = psa->GetAt(2);
				delete psa;
			}
			m_Flags.Set(dwFlags);
			EnableGrpWindows(IDC_GRP_AUDIO_RECORD, (m_Flags.IsInput)?TRUE:FALSE);
			m_cInputGain.EnableWindow(FALSE);
			m_bRecordinLevelInitialized = false;
			GetDlgItem(IDC_CK_AUDIO_DEFAULT_REC)->EnableWindow(m_bEnableDefaultRecDev);
			EnableGrpWindows(IDC_GRP_AUDIO_REC_EX, (m_Flags.IsInput) && theApp.m_bProfessional ? TRUE:FALSE);

			GetDlgItem(IDC_CK_AUDIO_INPUT_LISTEN)->EnableWindow(!m_Flags.IsRendering);
			EnableGrpWindows(IDC_GRP_AUDIO_TEST, (m_Flags.IsInput)?TRUE:FALSE);

			EnableGrpWindows(IDC_GRP_AUDIO_RENDERER, (m_Flags.IsOutput)?TRUE:FALSE);
			m_cOutputVolume.EnableWindow(FALSE);
			m_bOutputVolumeInitialized = false;

			GetDlgItem(IDC_CK_AUDIO_DETECTOR)->EnableWindow(m_bUnitActive);
			GetDlgItem(IDC_CK_AUDIO_DEFAULT_PLAY)->EnableWindow(m_bEnableDefaultPlayDev);

			if (m_pCIPCcontrolUnit)
			{
				CSecID secID = m_pCIPCcontrolUnit->GetSecID();
				
				m_pCIPCcontrolUnit->DoRequestHardware(secID, MEDIA_IDR_AUDIO_RENDERERS);
				m_pCIPCcontrolUnit->DoRequestHardware(secID, MEDIA_IDR_AUDIOCOMPRESSORS);
				m_pCIPCcontrolUnit->DoRequestHardware(secID, MEDIA_IDR_AUDIOINPUTDEVICES);
				m_pCIPCcontrolUnit->DoRequestHardware(secID, MEDIA_IDR_INPUT_PINS);
				m_pCIPCcontrolUnit->DoRequestValues(secID, MEDIA_GET_VALUE|MEDIA_STATE, 0);
				m_pCIPCcontrolUnit->DoRequestValues(secID, MEDIA_GET_VALUE|MEDIA_RECORDING_LEVEL, MEDIA_IDR_INPUT_PINS);
				m_pCIPCcontrolUnit->DoRequestValues(secID, MEDIA_GET_VALUE|MEDIA_AUDIO_QUALITYS, MEDIA_IDR_AUDIOCOMPRESSORS);
				m_pCIPCcontrolUnit->DoRequestValues(secID, MEDIA_GET_VALUE|MEDIA_STATE, 0);
				m_pCIPCcontrolUnit->DoRequestValues(secID, MEDIA_GET_VALUE|MEDIA_VOLUME, MEDIA_IDR_AUDIO_RENDERERS, 0);
			}
		}
		else
		{
			AfxMessageBox(IDS_NO_AUDIO_HARDWARE, MB_OK|MB_ICONINFORMATION);
			return;
		}
	}
	else if (dwRequestID == MEDIA_IDR_AUDIO_RENDERERS)
	{
		pCB  = &m_cRenderer;
		pstr = &m_sRenderer;
	}
	else if (dwRequestID == MEDIA_IDR_AUDIOCOMPRESSORS)
	{
		pCB  = &m_cCompressor;
		pstr = &m_sCompressor;
	}
	else if (dwRequestID == MEDIA_IDR_AUDIOINPUTDEVICES)
	{
		pCB  = &m_cCapture;
		pstr = &m_sCapture;
		pnSel= &m_nAudioCapture;
	}
	else if (dwRequestID == MEDIA_IDR_INPUT_PINS)
	{
		pCB = &m_cInputPins;
		pnSel= &m_nInputPin;
		if (m_pCIPCcontrolUnit)
		{
			m_pCIPCcontrolUnit->DoRequestValues(m_pCIPCcontrolUnit->GetSecID(), MEDIA_GET_VALUE|MEDIA_BALANCE, MEDIA_IDR_INPUT_PINS, m_nInputPin);
		}
	}
	else if (dwRequestID == MEDIA_IDR_TEST_CONFIG)
	{
		if (errorCode == 0)
		{
			AfxMessageBox(IDS_AUDIO_TEST_SUCCUESS);
		}
		else
		{
			AfxMessageBox(IDS_AUDIO_TEST_FAILED);
		}
	}

	
	if (pCB && pEM)
	{
		psa = pEM->GetStrings();
		if (psa)
		{
			int i, nCount = psa->GetSize();
			pCB->ResetContent();
			for (i=0; i<nCount; i++)
			{
				pCB->AddString(psa->GetAt(i));
			}
			if (dwRequestID == MEDIA_IDR_AUDIO_RENDERERS)
			{
				CWK_Profile &wkpBase = m_pParent->GetDocument()->GetProfile();
				int nRenderers = wkpBase.GetInt(AUDIO_UNIT, AU_RENDERERS, -1);
				if (nRenderers != nCount)
				{
					wkpBase.WriteInt(AUDIO_UNIT, AU_RENDERERS, nCount);
					m_bUpdateHardwareBranch = true;
				}
				RemoveUsedDevices(*pCB, AUDIO_OUTPUT_DEVICE_GUID);
				nCount = pCB->GetCount();
				if (nCount == 0)
				{
					GetDlgItem(IDC_CK_AUDIO_DEFAULT_PLAY)->EnableWindow(FALSE);
					m_sRenderer.Empty();
				}
			}
			else if (dwRequestID == MEDIA_IDR_AUDIOINPUTDEVICES)
			{
				CWK_Profile &wkpBase = m_pParent->GetDocument()->GetProfile();
				int nCapturers = wkpBase.GetInt(AUDIO_UNIT, AU_CAPTURERS, -1);
				if (nCapturers != nCount)
				{
					wkpBase.WriteInt(AUDIO_UNIT, AU_CAPTURERS, nCount);
					m_bUpdateHardwareBranch = true;
				}
				RemoveUsedDevices(*pCB, AUDIO_INPUT_DEVICE_GUID);
				if (pCB->GetCount() == 0)
				{
					GetDlgItem(IDC_CK_AUDIO_DEFAULT_REC)->EnableWindow(FALSE);
					m_sCapture.Empty();
				}
			}
			if (pstr)
			{
				int nOld = *pnSel;
				i = pCB->FindStringExact(-1, *pstr);
				*pnSel = pCB->SetCurSel(i);
				if (   (dwRequestID == MEDIA_IDR_AUDIOINPUTDEVICES)
					 && (nOld != CB_ERR)
					 && (nOld != *pnSel))
				{
					*pnSel = -1;
					OnSelchangeComboAudioCapture();
				}
			}
			else
			{
				*pnSel = pCB->SetCurSel(dwFlags);
			}
			delete psa;
		}
		if (pCB->GetCount() == 0)
		{
			pCB->EnableWindow(FALSE);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAudioPage::Initialize()
{
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_sKey, _T(""));
	CWK_Profile &wkpBase = m_pParent->GetDocument()->GetProfile();

	m_nThreshold         = wkp.GetInt(   AU_AUDIO_SETTINGS, AU_THRESHOLD    , AU_DEFAULT_THRESHOLD);
	m_nDwellTime         = wkp.GetInt(   AU_AUDIO_SETTINGS, AU_HOLD_TIME    , AU_DEFAULT_DWELLTIME);
	m_nSequence          = wkp.GetInt(   AU_AUDIO_SETTINGS, AU_SAMPLE_LENGTH, AU_DEFAULT_SEQUENCE);
	m_b2BytesPerSample   = wkp.GetInt(   AU_AUDIO_SETTINGS, AU_TWO_BYTES    , AU_DEFAULT_TWO_BYTES);
	m_nChannel           = wkp.GetInt(   AU_AUDIO_SETTINGS, AU_CHANNEL      , AU_DEFAULT_CHANNEL);
	m_nInputPin          = wkp.GetInt(   AU_AUDIO_SETTINGS, AU_INPUT_CHANNEL, m_nInputPin);
	m_bDetectorActive    = wkp.GetInt(AU_CIPC_SETTINGS, AU_GENERATE_ALARM   , FALSE);
	m_nQuality           = wkp.GetInt(   AU_AUDIO_SETTINGS, AU_AUDIO_QUALITY, 0);
	m_sSampleFrq         = wkp.GetString(AU_AUDIO_SETTINGS, AU_SAMPLE_FRQ   , AU_DEFAULT_SAMPLE_FRQ);

	m_sCapture           = wkp.GetString(AU_DIRECT_SHOW, AUDIO_INPUT_DEVICE_GUID , NULL);
	m_sCompressor        = wkp.GetString(AU_DIRECT_SHOW, AUDIO_COMPRESSOR_GUID   , NULL);
	m_sRenderer          = wkp.GetString(AU_DIRECT_SHOW, AUDIO_OUTPUT_DEVICE_GUID, NULL);

	m_bUnitActive = m_bOldUnitActive;

	CString sMedia;
	int j;
	CMediaList  * pMediaList  = m_pParent->GetDocument()->GetMedias();
	m_bEnableDefaultRecDev  = TRUE;
	m_bEnableDefaultPlayDev = TRUE;
	for (j=1; j<=SM_NO_OF_AUDIO_UNITS; j++)
	{
		if (IsUnitActive(wkpBase, j))
		{
			sMedia.Format(SM_AudioUnitMedia, j);
			CMediaGroup * pMediaGroup = pMediaList->GetGroupBySMName(sMedia);
			if (pMediaGroup)
			{
				CMedia *pM;
				int i, n = pMediaGroup->GetSize();
				for (i=0; i<n; i++)
				{
					pM = pMediaGroup->GetMedia(i);
					if (j==m_nPageNo)
					{
						if (pM->GetType() == SEC_MEDIA_TYPE_INPUT)
						{
							m_bDefaultRecDev = pM->IsDefault();
						}
						else if (pM->GetType() == SEC_MEDIA_TYPE_OUTPUT)
						{
							m_bDefaultPlayDev = pM->IsDefault();
						}
					}
					else
					{
						if (pM->GetType() == SEC_MEDIA_TYPE_INPUT && pM->IsDefault())
						{
							m_bEnableDefaultRecDev = FALSE;
						}
						else if (pM->GetType() == SEC_MEDIA_TYPE_OUTPUT && pM->IsDefault())
						{
							m_bEnableDefaultPlayDev = FALSE;
						}
					}
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CAudioPage::RemoveUsedDevices(CComboBox &cDev, LPCTSTR sGuidKey)
{
	int j, nFind;
	CWK_Profile &wkpBase = m_pParent->GetDocument()->GetProfile();
	for (j=1; j<=SM_NO_OF_AUDIO_UNITS; j++)
	{
		if (j != m_nPageNo && IsUnitActive(wkpBase, j))
		{
			CString sKey, sDev;
			sKey.Format(AUDIO_UNIT_DEVICE, j);
			CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_sBase + sKey, _T(""));
			sDev = wkp.GetString(AU_DIRECT_SHOW, sGuidKey, NULL);
			if (!sDev.IsEmpty())
			{
				nFind = cDev.FindStringExact(-1, sDev);
				if (nFind != -1)
				{
					cDev.DeleteString(nFind);
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
LPCTSTR CAudioPage::GetUnitName(int nPageNo)
{
	switch (nPageNo) 
	{
		case 1: return GetAppnameFromId(WAI_AUDIOUNIT_1);
		case 2: return GetAppnameFromId(WAI_AUDIOUNIT_2);
		case 3: return GetAppnameFromId(WAI_AUDIOUNIT_3);
		case 4: return GetAppnameFromId(WAI_AUDIOUNIT_4);
	}
	return _T("");
}
//////////////////////////////////////////////////////////////////////////
BOOL CAudioPage::IsUnitActive(CWK_Profile &wkpBase, int nPageNo)
{
	if (nPageNo == -1)
	{
		return DoesFileExist(theApp.m_sApplicationPath + _T("AudioUnit.exe"));
	}
	CString str, sUnitName = GetUnitName(nPageNo);
	ASSERT(IsBetween(nPageNo, 1, SM_NO_OF_AUDIO_UNITS));
	str = wkpBase.GetString(theApp.GetModuleSection(), sUnitName, _T(""));

	return !str.IsEmpty();
}
//////////////////////////////////////////////////////////////////////////
BOOL CAudioPage::CanActivateUnit(CWK_Profile &wkpBase, int nPageNo)
{
	int i, nCapturer, nRenderer,
		 nCapturers = wkpBase.GetInt(AUDIO_UNIT, AU_CAPTURERS, 1),
		 nRenderers = wkpBase.GetInt(AUDIO_UNIT, AU_RENDERERS, 1);
	
	CString str;
	for (i=1; i<=4; i++)
	{
		str.Format(AU_CAPTURER_N, i);
		nCapturer = wkpBase.GetInt(AUDIO_UNIT, str, 0);
		str.Format(AU_RENDERER_N, i);
		nRenderer = wkpBase.GetInt(AUDIO_UNIT, str, 0);
		TRACE(_T("audio Unit%d  PageNo: %d Capturer: %d  Renderer: %d\n"), 
			i,nPageNo, nCapturer, nRenderer);
		if (i == nPageNo)
		{
			if (nCapturer || nRenderer)
			{
				return TRUE;
			}
		}
		if (nCapturer) nCapturers--;
		if (nRenderer) nRenderers--;
	}

	return (nCapturers == 0 && nRenderers == 0) ? FALSE : TRUE;
}
//////////////////////////////////////////////////////////////////////////
void CAudioPage::OnRequestDisconnect()
{
	GetDlgItem(IDC_CK_AUDIO_DETECTOR)->EnableWindow(FALSE);
	EnableGrpWindows(IDC_GRP_AUDIO_RECORD, FALSE);
	EnableGrpWindows(IDC_GRP_AUDIO_TEST, FALSE);
	EnableGrpWindows(IDC_GRP_AUDIO_RENDERER, FALSE);
	EnableGrpWindows(IDC_GRP_AUDIO_REC_EX, FALSE);
	if (m_nRequestUnitTimer == 0)
	{
		m_nRequestUnitTimer = SetTimer(0x0815, 1000, NULL);
	}
}
//////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK CAudioPage::WndProcLevel(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CAudioPage*pThis = (CAudioPage*) GetWindowLong(hwnd, GWL_USERDATA);
	long lWndProc = pThis->m_lWndProcLevel,
		  lCtrlID  = ::GetDlgCtrlID(hwnd);
	if (lCtrlID == IDC_PROGRESS_INPUT_LEVEL_PEAK)
	{
		lWndProc = pThis->m_lWndProcLevelPeak;
	}
	if (uMsg == WM_PAINT)
	{
		LRESULT lResult = CallWindowProc((WNDPROC)lWndProc, hwnd, uMsg, wParam, lParam);
		CRect rcClient;
		CWnd *pWnd = CWnd::FromHandle(hwnd);
		int nThreshold = pThis->m_nThreshold;
		if (lCtrlID == IDC_PROGRESS_INPUT_LEVEL)
		{
			if (nThreshold >= 15)
			{
				nThreshold = nThreshold - 10; // 10 % unter dem Einschaltwert
			}
			else if (nThreshold >= 2)
			{
				nThreshold = nThreshold / 2;
			}
		}
		
		pWnd->GetClientRect(&rcClient);
		CDC *pDC   = pWnd->GetDC();
		if (pDC)
		{
			CRect rcDraw(rcClient);
			CBrush red(RGB(255, 0, 0)), blue(RGB(0,0,255));
			PBRANGE pbRange;
			rcDraw.top++;
			pWnd->SendMessage(PBM_GETRANGE, 0, (LPARAM)&pbRange);
			int nPos  = pWnd->SendMessage(PBM_GETPOS, 0, 0);
			int nOver = MulDiv(pbRange.iHigh, 90, 100);
			pDC->SaveDC();
			pDC->SelectStockObject(NULL_PEN);
			if (nPos > nOver)
			{
				pDC->SelectObject(&red);
				rcDraw.left   = MulDiv(rcClient.right, nOver, pbRange.iHigh);
				rcDraw.right  = MulDiv(rcClient.right, nPos , pbRange.iHigh);
				pDC->Rectangle(&rcDraw);
			}
			
			pDC->SelectStockObject(BLACK_PEN);
			rcClient.left = MulDiv(rcClient.right, nThreshold, pbRange.iHigh);
			rcClient.right = rcClient.left+1;
			pDC->Rectangle(&rcClient);

			pDC->RestoreDC(-1);
			pWnd->ReleaseDC(pDC);
		}
		return lResult;
	}
	return CallWindowProc((WNDPROC)lWndProc, hwnd, uMsg, wParam, lParam); 
}

/////////////////////////////////////////////////////////////////////////////
BOOL CAudioPage::IsDataValid()
{
	return UpdateData();
}
/////////////////////////////////////////////////////////////////////////////
void CAudioPage::SaveChanges()
{
	CString str, sKey, sInput, sMedia, sInputName, sOutputName;
	CWK_Profile &wkpBase = m_pParent->GetDocument()->GetProfile();
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE, m_sKey, "");
	BOOL	      bChanged = FALSE, 
		         bDetectorActive = m_bDetectorActive && m_bUnitActive;
	LPARAM      lParamUpdate = m_bUpdateHardwareBranch ? 1<<IMAGE_HARDWARE : 0;

	if (m_bUnitActive)
	{
		wkp.WriteInt(   AU_AUDIO_SETTINGS, AU_THRESHOLD     , m_nThreshold);
		wkp.WriteInt(   AU_AUDIO_SETTINGS, AU_HOLD_TIME     , m_nDwellTime);
		wkp.WriteString(AU_AUDIO_SETTINGS, AU_SAMPLE_FRQ    , m_sSampleFrq);
		wkp.WriteInt(   AU_AUDIO_SETTINGS, AU_SAMPLE_LENGTH , m_nSequence);
		wkp.WriteInt(   AU_AUDIO_SETTINGS, AU_TWO_BYTES     , m_b2BytesPerSample);
		wkp.WriteInt(   AU_CIPC_SETTINGS , AU_GENERATE_ALARM, m_bDetectorActive);
		wkp.WriteInt(   AU_AUDIO_SETTINGS, AU_AUDIO_QUALITY , m_nQuality);

		wkp.WriteString(AU_DIRECT_SHOW, AUDIO_INPUT_DEVICE_GUID, m_sCapture);
		wkp.WriteString(AU_DIRECT_SHOW, AUDIO_COMPRESSOR_GUID, m_sCompressor);
		wkp.WriteString(AU_DIRECT_SHOW, AUDIO_OUTPUT_DEVICE_GUID, m_sRenderer);
		// These settings are adjusted on CAudioChannelPage
		//	wkp.WriteInt(   AU_AUDIO_SETTINGS, AU_CHANNEL       , m_nChannel);
		//	wkp.WriteInt(   AU_AUDIO_SETTINGS, AU_INPUT_CHANNEL , m_nInputPin);
		// Get the settings in case they where changed on this page
		m_nChannel  = wkp.GetInt(   AU_AUDIO_SETTINGS, AU_CHANNEL      , AU_DEFAULT_CHANNEL);
		m_nInputPin = wkp.GetInt(   AU_AUDIO_SETTINGS, AU_INPUT_CHANNEL, m_nInputPin);
	}
	else
	{
		wkp.DeleteSection(AU_AUDIO_SETTINGS);
		wkp.DeleteSection(AU_CIPC_SETTINGS);
		wkp.DeleteSection(AU_DIRECT_SHOW);
	}


	if (m_cInputPins.GetCount())
	{
		m_cInputPins.GetLBText(m_nInputPin, sInputName);
	}

	sOutputName.Format(_T("%s\\Output\\%s\\Audio"), AUDIO_UNIT, m_sRenderer);
	sOutputName = wkpBase.GetString(sOutputName, AU_OUTPUT_NAME, NULL);
	if (sOutputName.IsEmpty())
	{
		sOutputName.LoadString(IDS_AUDIO_OUTPUTNAME);
	}

	if (m_bDeleteInputActivations)
	{
		CheckInputActivations(SVP_CO_CHECK_SILENT);
	}
	if (m_bDeleteActivations)
	{
		CheckMediaActivations(SVP_CO_CHECK_SILENT);
	}
	// Audio Detector
	sInput.Format(SM_AudioUnitInput, m_nPageNo);
	CInputList  * pInputList  = m_pParent->GetDocument()->GetInputs();
	CInputGroup * pInputGroup = pInputList->GetGroupBySMName(sInput);
	if (pInputGroup)
	{
		if (!bDetectorActive)
		{
			pInputList->DeleteGroup(sInput);
			bChanged = TRUE;
		}
	}
	else
	{
		if (bDetectorActive)
		{
			pInputGroup = pInputList->AddInputGroup(m_sCapture, 1, sInput);
			if (pInputGroup)
			{
				pInputGroup->GetInput(0)->SetName(sInputName);
			}
			bChanged = TRUE;
		}
	}
	
	if (bChanged)
	{
		pInputList->Save(wkpBase, FALSE);
		lParamUpdate |= 1<<IMAGE_INPUT;
	}
	
	// Audio Group

	bChanged = FALSE;
	sMedia.Format(SM_AudioUnitMedia, m_nPageNo);
	CMediaList  * pMediaList  = m_pParent->GetDocument()->GetMedias();
	CMediaGroup * pMediaGroup = pMediaList->GetGroupBySMName(sMedia);
	if (pMediaGroup)
	{
		if (!m_bUnitActive)
		{
			pMediaList->DeleteGroup(sMedia);
			bChanged = ALL_CONTROLLS;
		}
	}

	if (m_bUnitActive)
	{
		WORD nItems = 0;
		CMedia *pM;
		bool    bDisable = false;
		if (m_Flags.IsInput  && !m_sCapture.IsEmpty() ) nItems++;
		if (m_Flags.IsOutput && !m_sRenderer.IsEmpty()) nItems++;
		if (pMediaGroup == NULL)
		{
			pMediaGroup = pMediaList->AddMediaGroup(m_sUnitName, nItems, sMedia, SEC_MEDIAGROUP_AUDIO);
			bDisable    = true;			// new created: disable
			bChanged = ALL_CONTROLLS;
		}
		else
		{
			bChanged = TRUE;
		}
		if (pMediaGroup)
		{
			if (pMediaGroup->GetSize() != nItems)
			{
				pMediaGroup->SetSize(nItems);
			}
			
			nItems = 0;
			if (m_Flags.IsInput && !m_sCapture.IsEmpty())
			{
				pM = pMediaGroup->GetMedia(nItems++);
				pM->SetType(SEC_MEDIA_TYPE_INPUT);
				pM->SetName(sInputName);
				pM->SetDefault(m_bDefaultRecDev ? true : false);
				pM->SetAudio(true);
				pM->SetDisabled(bDisable);
			}
			
			if (m_Flags.IsOutput && !m_sRenderer.IsEmpty())
			{
				pM = pMediaGroup->GetMedia(nItems++);
				pM->SetType(SEC_MEDIA_TYPE_OUTPUT);
				pM->SetName(sOutputName);
				pM->SetDefault(m_bDefaultPlayDev ? true : false);
				pM->SetAudio(true);
				pM->SetDisabled(bDisable);
			}
		}
	}
	else
	{
		CString sSection;
		sSection.Format(_T("%s\\Input\\%s"), AUDIO_UNIT, wkp.GetString(AU_DIRECT_SHOW, AUDIO_INPUT_DEVICE_GUID , NULL));
		wkpBase.DeleteSection(sSection);

		sSection.Format(_T("%s\\Output\\%s"), AUDIO_UNIT, wkp.GetString(AU_DIRECT_SHOW, AUDIO_OUTPUT_DEVICE_GUID, NULL));
		wkpBase.DeleteSection(sSection);
	}

	str.Format(AU_RENDERER_N, m_nPageNo);
	wkpBase.WriteInt(AUDIO_UNIT, str, m_sRenderer.IsEmpty() ? 0 : m_bUnitActive);

	str.Format(AU_CAPTURER_N, m_nPageNo);
	wkpBase.WriteInt(AUDIO_UNIT, str, m_sCapture.IsEmpty()  ? 0 : m_bUnitActive);

	if (bChanged)
	{
		pMediaList->Save(wkpBase, FALSE);
		if (bChanged == ALL_CONTROLLS) lParamUpdate |= 1<<IMAGE_HARDWARE;
	}
	
	// Launcher
	SaveLauncherSettings();
	m_bOldUnitActive = m_bUnitActive;
	if (lParamUpdate != 0)
	{
		m_pParent->PostMessage(WM_USER, USER_MSG_UPDATE_PARENT_TREE, lParamUpdate);
	}
}
void CAudioPage::SaveLauncherSettings()
{
	CWK_Profile &wkpBase = m_pParent->GetDocument()->GetProfile();
	CString sModule = _T(""), sName;
	if (m_bUnitActive)
	{
		sName = m_sUnitName.Left(m_sUnitName.GetLength()-1);
		sModule.Format(_T("%s.exe"), sName);
	}
	wkpBase.WriteString(theApp.GetModuleSection(), m_sUnitName, sModule);
}

/////////////////////////////////////////////////////////////////////////////
void CAudioPage::CancelChanges()
{
	BOOL bSaveLaucher = m_bOldUnitActive != m_bUnitActive;
	Initialize();
	UpdateData(FALSE);
	if (bSaveLaucher)
	{
		CDataExchange dx(this, FALSE);
		DDX_Check(&dx, IDC_CK_AUDIO_UNIT_ACTIVE, m_bOldUnitActive);
		OnCkAudioUnitActive();
		SaveLauncherSettings();
	}
	
	m_bDeleteActivations      = false;
	m_bDeleteInputActivations = false;

	int nSel;
	nSel = m_cCapture.FindStringExact(-1, m_sCapture);
	if (nSel != CB_ERR) m_cCapture.SetCurSel(nSel);

	nSel = m_cCompressor.FindStringExact(-1, m_sCompressor);
	if (nSel != CB_ERR) m_cCompressor.SetCurSel(nSel);
	
	nSel = m_cRenderer.FindStringExact(-1, m_sRenderer);
	if (nSel != CB_ERR) m_cRenderer.SetCurSel(nSel);
	
	m_cQuality.SetCurSel(m_nQuality);
}
/////////////////////////////////////////////////////////////////////////////
void CAudioPage::OnConfirmValues(CSecID mediaID, DWORD dwCmd, DWORD dwID, DWORD dwValue, const CIPCExtraMemory *pData)
{
	DWORD dwCommand = MEDIA_COMMAND_VALUE(dwCmd);
	BOOL  bErrorOccurred = (dwCmd & MEDIA_ERROR_OCCURRED) ? TRUE : FALSE;
	if (dwCmd & MEDIA_GET_VALUE)
	{
		if (dwID == MEDIA_IDR_INPUT_PINS)
		{
			if (dwCommand == MEDIA_RECORDING_LEVEL)
			{
				m_cInputGain.SetPos(dwValue);
				m_bRecordinLevelInitialized = true;
				m_cInputGain.EnableWindow(!bErrorOccurred);
			}
			else if (dwCommand == MEDIA_BALANCE)
			{
				GetDlgItem(IDC_RD_AUDIO_RIGHT_CHANNEL)->EnableWindow(!bErrorOccurred);
			}
		}
		else if (dwID == MEDIA_IDR_AUDIO_RENDERERS)
		{
			if (dwCommand == MEDIA_VOLUME)
			{
				int nPos = (DWORD)pow(10.0, dwValue / (1000.0 / 3.0));	// log
//				int nPos = (DWORD) dwValue;								// lin
				m_cOutputVolume.SetPos(nPos);
				m_bOutputVolumeInitialized = true;
				m_cOutputVolume.EnableWindow(!bErrorOccurred);
			}
		}
		else if ((dwCommand == MEDIA_AUDIO_QUALITYS) && (dwID == MEDIA_IDR_AUDIOCOMPRESSORS))
		{
			if (pData)
			{
				CStringArray *psa = pData->GetStrings();
				m_cQuality.ResetContent();
				int i, nSize = psa->GetSize();
				for (i=0; i<nSize; i++)
				{
					m_cQuality.InsertString(i, psa->GetAt(i));
				}
				WK_DELETE(psa);
				if (IsBetween(m_nQuality, 0, nSize))
				{
					m_cQuality.SetCurSel(m_nQuality);
				}
				else if (nSize)
				{
					m_cQuality.SetCurSel(0);
				}
			}
		}
		else if (dwCommand == MEDIA_STATE)
		{
			m_Flags.Set(dwValue); 
		}
	}
	else if (dwCmd & MEDIA_SET_VALUE)
	{
		if ((dwCommand == MEDIA_VOLUME_LEVEL) && (dwID == MEDIA_IDR_INPUT_PINS))
		{
			m_cInputLevel.SetPos(HIWORD(dwValue));
//			m_cInputLevel.EnableWindow((dwCmd & MEDIA_ERROR_OCCURRED) ? FALSE : TRUE);
			m_cInputLevelPeak.SetPos(LOWORD(dwValue));
//			m_cInputLevelPeak.EnableWindow((dwCmd & MEDIA_ERROR_OCCURRED) ? FALSE : TRUE);
		}
		else if (dwCommand == MEDIA_STATE)
		{
			m_Flags.Set(dwValue);
			if (m_bListen == FALSE)
			{
				GetDlgItem(IDC_CK_AUDIO_INPUT_LISTEN)->EnableWindow(!m_Flags.IsRendering);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CAudioPage::OnUser(WPARAM wParam, LPARAM)
{
	if (wParam == 1)
	{
		WK_DELETE(m_pCIPCcontrolUnit);
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CAudioPage::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
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
		else if (wParam == (WPARAM)m_cInputGain.m_hWnd)
		{
			int nPos = _ttoi(pDI->szText);
			if (nPos > 0)
			{
				wsprintf(pDI->szText, _T("%d.%01d %%"), nPos/10, nPos%10);
			}
		}

		*pResult = 0;
	}
	return CSVPage::OnNotify(wParam, lParam, pResult);
}
