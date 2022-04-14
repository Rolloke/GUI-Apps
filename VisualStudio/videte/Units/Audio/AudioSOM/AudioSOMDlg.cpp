// AudioSOMDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AudioSOM.h"
#include "AudioSOMDlg.h"

#include "strmif.h"
#include "mmsystem.h"
#include "mtype.h"

#include "common\mfcutil.h"
#include "common\dshowutil.h"
#include "include\dmoreg.h"
#include "common\namedguid.h"
#include "uuids.h"

#include <initguid.h>    // DEFINE_GUID to declare an EXTERN_C const.
#include "Synth\isynth.h"
#include "Scope\iscope.h"
#include "..\..\..\DirectShow\Filters\Dump\idump.h"
#include "control.h"
#include ".\audiosomdlg.h"

#include <map>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define OSZI_AUDIO   0
#define OSZI_WAVE    1
#define OSZI_FILE    2

#define FILE_AUDIO   0
#define FILE_WAVE    1

#define AUDIO_WAVE   0
#define AUDIO_FILE   1

#define SAFE_KILL_TIMER(X) { if (X) { KillTimer(X); X=0; } }
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
   CAboutDlg();

// Dialog Data
   enum { IDD = IDD_ABOUTBOX };

   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
   DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{

}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CAudioSOMDlg dialog



CAudioSOMDlg::CAudioSOMDlg(CWnd* pParent /*=NULL*/)
  :CDialog(CAudioSOMDlg::IDD, pParent)
  ,m_line_Osz1(&m_penOszi)
  ,m_line_Osz2(&m_penOszi)
  ,m_line_Osz3(&m_penOszi)
  ,m_line_Osz4(&m_penOszi)
  ,m_line_AO1 (&m_penAO)
  ,m_line_AO2 (&m_penAO)
  ,m_line_AO3 (&m_penAO)
  ,m_line_AO4 (&m_penAO)
  ,m_line_FO1 (&m_penFO)
  ,m_line_FO2 (&m_penFO)
  ,m_line_FO3 (&m_penFO)
  ,m_line_FO4 (&m_penFO)
{
   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

   m_pGB          = NULL;
   m_pCaptureGB   = NULL;
   m_pMC          = NULL;
   m_pME          = NULL;
   m_pInput       = NULL;
   m_pOsciloscope = NULL;
   m_pSynthesizer = NULL;
   m_ISynthesizer = NULL;
   m_pOutput      = NULL;
   m_pVolume      = NULL;
   m_pFileIn      = NULL;
   m_pEncode      = NULL;
   m_pFileOut     = NULL;
   m_pInfTeeFile  = NULL;
   m_pInfTeeInput = NULL;
   m_pInfTeeSynth = NULL;
   m_nAudioInput  = -1;
   m_nAudioOutput = -1;
   m_nOszi        = -1;
   m_nFileOut     = -1;
   m_nAudioOut    = -1;
   m_bURL_Input   = FALSE;
   m_bURL_Output  = FALSE;
   m_dwROT        = 0;
   m_nUpdatePositionTimer = 0;
   m_sInputFileName  = theApp.GetProfileString(SECTION_FILE, FILE_INPUT_NAME);
   m_sOutputFileName = theApp.GetProfileString(SECTION_FILE, FILE_OUTPUT_NAME);
}

void CAudioSOMDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_AUDIO_INPUT, m_cAudioInput);
    DDX_Control(pDX, IDC_COMBO_OUTPUT_FRQ, m_cOutputFrq);
    DDX_Control(pDX, IDC_COMBO_OUTPUT_BITS, m_cOutputBits);
    DDX_Control(pDX, IDC_COMBO_INPUT_BITS, m_cInputBits);
    DDX_Control(pDX, IDC_COMBO_AUDIO_OUTPUT, m_cAudioOutput);
    DDX_Control(pDX, IDC_COMBO_SAMPLE_LENGTH, m_cSampleLength);
    DDX_Control(pDX, IDC_COMBO_AUDIO_INPUT_PIN, m_cInputPin);
    DDX_Control(pDX, IDC_COMBO_INPUT_CAPS, m_cAudioInputCaps);
    DDX_Control(pDX, IDC_SLIDER_VOLUME, m_cVolume);
    DDX_Control(pDX, IDC_SLIDER_BALANCE, m_cBalance);
    DDX_Control(pDX, IDC_STATIC_CON_OSZ_1, m_line_Osz1);
    DDX_Control(pDX, IDC_STATIC_CON_OSZ_2, m_line_Osz2);
    DDX_Control(pDX, IDC_STATIC_CON_OSZ_3, m_line_Osz3);
    DDX_Control(pDX, IDC_STATIC_CON_OSZ_4, m_line_Osz4);
    DDX_Control(pDX, IDC_STATIC_CON_AO_1, m_line_AO1);
    DDX_Control(pDX, IDC_STATIC_CON_AO_2, m_line_AO2);
    DDX_Control(pDX, IDC_STATIC_CON_AO_3, m_line_AO3);
    DDX_Control(pDX, IDC_STATIC_CON_FO_1, m_line_FO1);
    DDX_Control(pDX, IDC_STATIC_CON_FO_2, m_line_FO2);
    DDX_Control(pDX, IDC_STATIC_CON_FO_3, m_line_FO3);
    DDX_Radio(pDX, IDC_RD_OSZI1, m_nOszi);
    DDX_Radio(pDX, IDC_RD_FO1, m_nFileOut);
    DDX_Radio(pDX, IDC_RD_AO1, m_nAudioOut);
    DDX_Control(pDX, IDC_SLIDER_INPUT_LEVEL, m_cInputLevel);
    DDX_Text(pDX, IDC_EDT_INPUT_FILE_NAME, m_sInputFileName);
    DDX_Text(pDX, IDC_EDT_OUTPUT_FILE_NAME, m_sOutputFileName);
    DDX_Control(pDX, IDC_SLIDER_MEDIA_POS, m_cMediaPosition);
    DDX_Check(pDX, IDC_CK_URL_INPUT, m_bURL_Input);
    DDX_Check(pDX, IDC_CK_URL_OUTPUT, m_bURL_Output);
}

BEGIN_MESSAGE_MAP(CAudioSOMDlg, CDialog)
	//{{AFX_MSG_MAP(CAudioSOMDlg)
   ON_WM_SYSCOMMAND()
   ON_WM_PAINT()
   ON_WM_QUERYDRAGICON()
   ON_WM_DESTROY()
   ON_BN_CLICKED(IDC_BTN_START, OnBnClickedBtnStart)
   ON_BN_CLICKED(IDC_BTN_STOP, OnBnClickedBtnStop)
   ON_BN_CLICKED(IDC_BTN_SAVE_SETTINGS, OnBnClickedBtnSaveSettings)
   ON_BN_CLICKED(IDC_BTN_RESET_ALL, OnBnClickedBtnResetAll)
   ON_CBN_SELCHANGE(IDC_COMBO_AUDIO_INPUT, OnCbnSelchangeComboAudioInput)
   ON_CBN_SELCHANGE(IDC_COMBO_AUDIO_INPUT_PIN, OnCbnSelchangeComboAudioInputPin)
   ON_BN_CLICKED(IDC_BTNSHOW_SYNTH, OnBnClickedBtnshowSynth)
   ON_CBN_SELCHANGE(IDC_COMBO_AUDIO_OUTPUT, OnCbnSelchangeComboAudioOutput)
   ON_WM_HSCROLL()
   ON_CBN_SELCHANGE(IDC_COMBO_OUTPUT_FRQ, OnCbnSelchangeComboOutputFrq)
   ON_BN_CLICKED(IDC_BTNSHOW_SYNTH_FILTER, OnBnClickedBtnshowSynthFilter)
   ON_CONTROL_RANGE(BN_CLICKED, IDC_RD_OSZI1, IDC_RD_AO2, OnRDOutClicked)
   ON_BN_CLICKED(IDC_BTN_INITIALIZE, OnBnClickedBtnInitialize)
   ON_BN_CLICKED(IDC_CK_ROT, OnBnClickedCkRot)
   ON_WM_TIMER()
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNotify)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNotify)
   //}}AFX_MSG_MAP
   ON_BN_CLICKED(IDC_BTN_FILE_OUTPUT, OnBnClickedBtnFileOutput)
   ON_BN_CLICKED(IDC_BTN_FILE_INPUT, OnBnClickedBtnFileInput)
END_MESSAGE_MAP()


// CAudioSOMDlg message handlers

BOOL CAudioSOMDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

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
   int nIntens = 180;
   m_penOszi.CreatePen(PS_SOLID, 2, RGB(nIntens,0,0));
   m_penAO.CreatePen(  PS_SOLID, 2, RGB(0,nIntens,0));
   m_penFO.CreatePen(  PS_SOLID, 2, RGB(0,0,nIntens));

   // Set the icon for this dialog.  The framework does this automatically
   //  when the application's main window is not a dialog
   SetIcon(m_hIcon, TRUE);         // Set big icon
   SetIcon(m_hIcon, FALSE);      // Set small icon

   HRESULT hr;
   int i;
   CString str;

   HR_REPORT(CoInitialize(NULL));
   for (i=0; i<15; i++)
   {
      str.Format(_T("%.2f"), 1000.0 / (i+1));
      m_cSampleLength.AddString(str);
   }

   hr = EnumFiltersWithMonikerToList(NULL, &CLSID_AudioInputDeviceCategory, &m_cAudioInput);
   hr = EnumFiltersWithMonikerToList(NULL, &CLSID_AudioRendererCategory, &m_cAudioOutput);

   m_cAudioInput.SetCurSel(theApp.GetProfileInt(SECTION_SETTINGS, AUDIO_SOURCE, 0));
   m_cSampleLength.SetCurSel(theApp.GetProfileInt(SECTION_SETTINGS, AUDIO_SAMPLE_LEN, m_cSampleLength.GetCount()-1));
   OnCbnSelchangeComboAudioInput();

   m_cVolume.SetRangeMin(0);
   m_cVolume.SetRangeMax(10000);
   m_cVolume.SetPos(theApp.GetProfileInt(SECTION_SETTINGS, AUDIO_OUT_VOLUME, 0));
   m_cBalance.SetRangeMin(0);
   m_cBalance.SetRangeMax(20000);
   m_cBalance.SetPos(theApp.GetProfileInt(SECTION_SETTINGS, AUDIO_OUT_BALANCE, 0));
   m_cInputLevel.SetRangeMin(0);
   m_cInputLevel.SetRangeMax(1000);
   m_cInputLevel.SetPos(theApp.GetProfileInt(SECTION_SETTINGS, AUDIO_SOURCE_LEVEL, 500));
   m_cMediaPosition.SetRange(0, 1000);

   m_cOutputFrq.SetCurSel(theApp.GetProfileInt(SECTION_SETTINGS, AUDIO_OUT_FRQ, m_cOutputFrq.GetCount()-1));
   m_cInputBits.SetCurSel(theApp.GetProfileInt(SECTION_SETTINGS, SYNTH_BITS_PER_SAMP, m_cInputBits.GetCount()-1));
   
   m_cOutputBits.SetCurSel(theApp.GetProfileInt(SECTION_SYNTHESIZER, SYNTH_BITS_PER_SAMP, m_cOutputBits.GetCount()-1));
   m_cAudioOutput.SetCurSel(theApp.GetProfileInt(SECTION_SETTINGS, AUDIO_RENDER, 0));
   OnCbnSelchangeComboAudioOutput();
   EnableToolTips(true);

   return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAudioSOMDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
   if ((nID & 0xFFF0) == IDM_ABOUTBOX)
   {
      CAboutDlg dlgAbout;
      dlgAbout.DoModal();
   }
   else
   {
      CDialog::OnSysCommand(nID, lParam);
   }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAudioSOMDlg::OnPaint() 
{
   if (IsIconic())
   {
      CPaintDC dc(this); // device context for painting

      SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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
      CPaintDC dc(this);
      dc.MoveTo(10, 10);
      dc.LineTo(100, 100);
   }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAudioSOMDlg::OnQueryDragIcon()
{
   return static_cast<HCURSOR>(m_hIcon);
}

/////////////////////////////////////////////////////////////////////////////
void CAudioSOMDlg::ReportError(ErrorStruct*ps, bool bMsgBox)
{
   ASSERT(ps != NULL);
   if (FAILED(ps->m_hr))
   {
      CString str, sError, sDescription;
      GetErrorStrings(ps->m_hr, sError, sDescription);
      CString sFile(ps->m_sFile);
      str.Format(_T("%s(%d): DirectShow Error %s(%x)\n%s\n"), LPCWSTR(sFile), ps->m_nLine, sError, ps->m_hr, sDescription);
      TRACE(str);
      if (bMsgBox)
      {
         AfxMessageBox(str, MB_ICONERROR|MB_OK);
      }
   }
   delete ps;
}

void CAudioSOMDlg::OnDestroy()
{
   SAFE_KILL_TIMER(m_nUpdatePositionTimer);
   ReleaseInputPins();
   OnBnClickedBtnSaveSettings();
   OnBnClickedBtnResetAll();
   ClearFilterListWithMoniker(m_cAudioInput);
   ClearFilterListWithMoniker(m_cAudioOutput);
   CoUninitialize();
   CDialog::OnDestroy();
}

void CAudioSOMDlg::InitAudioIn()
{
   HRESULT          hr        = NOERROR;
   IMoniker        *pMoniker  = NULL;
   IPin            *pPinOut   = NULL;
   IPin            *pPinIn    = NULL;
   IAMStreamConfig *pCfg      = NULL;
   IAMBufferNegotiation *pBN  = NULL;                     // Suggest the allocator properties
   ALLOCATOR_PROPERTIES ap;
   BOOL bStart = GetDlgItem(IDC_BTN_STOP)->IsWindowEnabled();
   OnBnClickedBtnStop();
   try
   {
      InitGraphBuilder();
      if (m_pInput == NULL)
      {
         pMoniker = (IMoniker*)m_cAudioInput.GetItemData(m_cAudioInput.GetCurSel());
         if (pMoniker == (IMoniker*)CB_ERR) throw HRESULT_ERROR_AT_POS(E_NOINTERFACE);
         HR_EXCEPTION(AddFilterToGraph(m_pGB, &m_pInput, pMoniker, L"Audio Input"));
      }

      int nFrequency = 44100, nBits = 16;
      int nAvgBytesPerSec = nFrequency;
      HR_EXCEPTION(GetPin(m_pInput, PINDIR_OUTPUT, 0, &pPinOut));// Query pins of the capture device
      hr = pPinOut->ConnectedTo(&pPinIn);
      if (SUCCEEDED(hr) && pPinIn)
      {
         hr = m_pGB->Disconnect(pPinOut);
         hr = m_pGB->Disconnect(pPinIn);
      }

      nFrequency = (int) m_cAudioInputCaps.GetItemData(m_cAudioInputCaps.GetCurSel());
      nBits      = (int) m_cInputBits.GetItemData(m_cInputBits.GetCurSel());
      hr = pPinOut->QueryInterface(IID_IAMStreamConfig, (void **)&pCfg);
      if (SUCCEEDED(hr))
      {
         AM_MEDIA_TYPE *pmt={0};

         hr = pCfg->GetFormat(&pmt);                           // Read current media type/format
         if (SUCCEEDED(hr))
         {
            ASSERT(pmt->formattype == FORMAT_WaveFormatEx);
            WAVEFORMATEX *pWF = (WAVEFORMATEX *) pmt->pbFormat;
            pWF->nSamplesPerSec = nFrequency;
            pWF->wBitsPerSample  = /*pWF->nChannels * */  nBits;
            pWF->nAvgBytesPerSec = pWF->nSamplesPerSec * pWF->wBitsPerSample / 8;
            pmt->lSampleSize     = pWF->wBitsPerSample / 8;
            nAvgBytesPerSec = pWF->nAvgBytesPerSec;
            HR_REPORT(pCfg->SetFormat(pmt));                     // Set the new formattype for the output pin

            DeleteMediaType(pmt);
         }
         hr = pCfg->GetFormat(&pmt);                           // Read current media type/format
         if (SUCCEEDED(hr))
         {
            WAVEFORMATEX *pWF = (WAVEFORMATEX *) pmt->pbFormat;
            nAvgBytesPerSec = pWF->nAvgBytesPerSec;
            DeleteMediaType(pmt);
         }
         RELEASE_OBJECT(pCfg);
      }

      hr = pPinOut->QueryInterface(IID_IAMBufferNegotiation, (void**)&pBN);
      if (SUCCEEDED(hr))
      {
         int nLength = m_cSampleLength.GetCurSel()+1;
         hr =pBN->GetAllocatorProperties(&ap);
         ap.cbAlign  = 1;
         ap.cbPrefix = 0;
         ap.cBuffers = 16;
         ap.cbBuffer = nAvgBytesPerSec / nLength;
         HR_REPORT(pBN->SuggestAllocatorProperties(&ap));      // Suggestion must be done before connection
      }
      if (m_pInfTeeInput == NULL)
      {
         HR_EXCEPTION(AddFilterToGraph(m_pGB, &m_pInfTeeInput, CLSID_InfTee, L"InfTeeInput"));
      }
      HR_EXCEPTION(ConnectFilters(m_pGB, m_pInput, 0, m_pInfTeeInput, 0));

      int nTeePin = 0;
      if (m_nOszi == OSZI_AUDIO)
      {
         ConnectToOsziloscope(m_pInfTeeInput, nTeePin++);
      }
      if (m_nFileOut == FILE_AUDIO)
      {
         ConnectToFileOut(m_pInfTeeInput, nTeePin++);
      }
   }
   catch(ErrorStruct *ps)
   {
      ReportError(ps);
      ReleaseAudioIn();
      bStart = FALSE;
   }
   RELEASE_OBJECT(pBN);
   RELEASE_OBJECT(pPinOut);
   RELEASE_OBJECT(pPinIn);
   if (bStart)
   {
      OnBnClickedBtnStart();
   }
}

void CAudioSOMDlg::InitSynthesizer()
{
   IPin            *pPinOut   = NULL;
   IPin            *pPinIn    = NULL;
   OnBnClickedBtnStop();
   try
   {
      HRESULT          hr        = NOERROR;
      InitGraphBuilder();
      if (m_pSynthesizer == NULL)
      {
         HR_EXCEPTION(AddFilterToGraph(m_pGB, &m_pSynthesizer, CLSID_SynthFilter, L"Synthesizer"));
         HR_EXCEPTION(m_pSynthesizer->QueryInterface(IID_ISynthesizer, (void**)&m_ISynthesizer));
         HR_EXCEPTION(m_ISynthesizer->put_SamplesPerSec((int)m_cOutputFrq.GetItemData(m_cOutputFrq.GetCurSel())));
         HR_EXCEPTION(m_ISynthesizer->put_Channels(theApp.GetProfileInt(SECTION_SYNTHESIZER, SYNTH_CHANNEL, 2)));
         //HR_EXCEPTION(m_ISynthesizer->put_BitsPerSample(theApp.GetProfileInt(SECTION_SYNTHESIZER, SYNTH_BITS_PER_SAMP, 16)));
         HR_EXCEPTION(m_ISynthesizer->put_BitsPerSample((int)m_cOutputBits.GetItemData(m_cOutputBits.GetCurSel())));
         HR_EXCEPTION(m_ISynthesizer->put_Waveform(m_SynthesizerDlg.m_nWaveForm));
         HR_EXCEPTION(m_ISynthesizer->put_Amplitude(m_SynthesizerDlg.m_nMagnitude1));
         HR_EXCEPTION(m_ISynthesizer->put_Amplitude2(m_SynthesizerDlg.m_nMagnitude2));
         HR_EXCEPTION(m_ISynthesizer->put_Frequency(m_SynthesizerDlg.m_nFrequency1));
         HR_EXCEPTION(m_ISynthesizer->put_SweepRange(m_SynthesizerDlg.m_nFrequency1, m_SynthesizerDlg.m_nFrequency2));
         HR_EXCEPTION(m_ISynthesizer->put_OffsetDegree(m_SynthesizerDlg.m_nPhase));
         HR_EXCEPTION(m_ISynthesizer->put_TimePercent(m_SynthesizerDlg.m_nTime));
         HR_EXCEPTION(m_ISynthesizer->put_Offset(m_SynthesizerDlg.m_nOffset1, m_SynthesizerDlg.m_nOffset2));
         HR_EXCEPTION(m_ISynthesizer->put_Correlated(m_SynthesizerDlg.m_bCorrelated));
         //HR_EXCEPTION(m_ISynthesizer->put_OutputFormat(..));
         // TODO! load Synth settings
      }

      HR_EXCEPTION(GetPin(m_pSynthesizer, PINDIR_OUTPUT, 0, &pPinOut));// Query pins of the capture device
      hr = pPinOut->ConnectedTo(&pPinIn);
      if (SUCCEEDED(hr) && pPinIn)
      {
         hr = m_pGB->Disconnect(pPinOut);
         hr = m_pGB->Disconnect(pPinIn);
      }
      RELEASE_OBJECT(pPinOut);
      RELEASE_OBJECT(pPinIn);
      if (m_pInfTeeSynth == NULL)
      {
         HR_EXCEPTION(AddFilterToGraph(m_pGB, &m_pInfTeeSynth, CLSID_InfTee, L"InfTeeSynth"));
      }
      HR_EXCEPTION(ConnectFilters(m_pGB, m_pSynthesizer, 0, m_pInfTeeSynth, 0));
      int nTeePin = 0;
      if (m_nOszi == OSZI_WAVE)
      {
         ConnectToOsziloscope(m_pInfTeeSynth, nTeePin++);
      }
      if (m_nFileOut == FILE_WAVE)
      {
         ConnectToFileOut(m_pInfTeeSynth, nTeePin++);
      }
      if (m_nAudioOut == AUDIO_WAVE)
      {
         ConnectToAudioOut(m_pInfTeeSynth, nTeePin++);
      }
   }
   catch(ErrorStruct *ps)
   {
      ReportError(ps);
      ReleaseSynth();
   }
   RELEASE_OBJECT(pPinOut);
   RELEASE_OBJECT(pPinIn);
}

void CAudioSOMDlg::InitFileIn()
{
   IPin            *pPinOut   = NULL;
   IPin            *pPinIn    = NULL;
   IFileSourceFilter *pFSF = NULL;
   OnBnClickedBtnStop();
   try
   {
      HRESULT          hr        = NOERROR;
      InitGraphBuilder();
      if (m_pFileIn == NULL)
      {
         if (m_bURL_Input)
         {
            HR_EXCEPTION(AddFilterToGraph(m_pGB, &m_pFileIn, CLSID_URLReader, L"AsyncURLReader"));
         }
         else
         {
            HR_EXCEPTION(AddFilterToGraph(m_pGB, &m_pFileIn, CLSID_AsyncReader, L"AsyncFileReader"));
         }
         HR_EXCEPTION(m_pFileIn->QueryInterface(IID_IFileSourceFilter, (void**) &pFSF));
         HR_EXCEPTION(pFSF->Load(m_sInputFileName, NULL));
      }

      HR_EXCEPTION(GetPin(m_pFileIn, PINDIR_OUTPUT, 0, &pPinOut));// Query pins of the capture device
      hr = pPinOut->ConnectedTo(&pPinIn);
      if (SUCCEEDED(hr) && pPinIn)
      {
         hr = m_pGB->Disconnect(pPinOut);
         hr = m_pGB->Disconnect(pPinIn);
      }
      RELEASE_OBJECT(pPinOut);
      RELEASE_OBJECT(pPinIn);
      if (m_pInfTeeFile == NULL)
      {
         HR_EXCEPTION(AddFilterToGraph(m_pGB, &m_pInfTeeFile, CLSID_InfTee, L"InfTeeFile"));
      }
      HR_EXCEPTION(ConnectFilters(m_pGB, m_pFileIn, 0, m_pInfTeeFile, 0));
      int nTeePin = 0;
      if (m_nOszi == OSZI_FILE)
      {
         ConnectToOsziloscope(m_pInfTeeFile, nTeePin++);
      }
      if (m_nAudioOut == AUDIO_FILE)
      {
         ConnectToAudioOut(m_pInfTeeFile, nTeePin++);
      }
   }
   catch(ErrorStruct *ps)
   {
      ReportError(ps);
      ReleaseFileIn();
   }
   RELEASE_OBJECT(pPinOut);
   RELEASE_OBJECT(pPinIn);
   RELEASE_OBJECT(pFSF);
}

struct SWaveFile
{
   char  sRIFF[4];
   long  nFileLength;
   char  sWAVEfmt_[8];
   long  nHeaderLength;
};

HRESULT WINAPI CAudioSOMDlg::ReceiveMediaSample(IMediaSample*pSample, long lNo, long lPtr)
{
   CAudioSOMDlg*pThis = (CAudioSOMDlg*)(void*)(lPtr);
   if (pThis->m_OutputFile.m_hFile == INVALID_HANDLE_VALUE)
   {
      pThis->m_OutputFile.Open(pThis->m_sOutputFileName, CFile::modeCreate|CFile::modeWrite);
      SWaveFile wf;
      AM_MEDIA_TYPE *pMT;
      pSample->GetMediaType(&pMT);
      ZeroMemory(&wf, sizeof(SWaveFile));
      strncpy((char*)&wf.sRIFF    , "RIFF"    , 4);
      strncpy((char*)&wf.sWAVEfmt_, "WAVEfmt ", 8);
      wf.nHeaderLength = sizeof(SWaveFile) - sizeof(long);
      wf.nFileLength   = wf.nHeaderLength + sizeof(WAVEFORMATEX);
      pThis->m_OutputFile.Write(&wf, sizeof(SWaveFile));
      pThis->m_OutputFile.Write(pMT->pbFormat, pMT->cbFormat);
      short wAlign = 16;
      long  lSampleSize = 0;
      pThis->m_OutputFile.Write(&wAlign, sizeof(wAlign));
      pThis->m_OutputFile.Write(&lSampleSize, sizeof(lSampleSize));

      DeleteMediaType(pMT);
   }
   BYTE*pPtr;
   pSample->GetPointer(&pPtr);
   pThis->m_OutputFile.Write(pPtr, pSample->GetSize());

   return S_OK;
}

void CAudioSOMDlg::ConnectToFileOut(IBaseFilter*pFilterIn, int nPinIn)
{
   HRESULT          hr        = NOERROR;
   if (m_pEncode == NULL)
   {
      HR_EXCEPTION(AddFilterToGraph(m_pGB, &m_pEncode, L"{3C78B8E2-6C4D-11D1-ADE2-0000F8754B99}", L"Wave Dest"));
   }

   HR_EXCEPTION(ConnectFilters(m_pGB, pFilterIn, nPinIn, m_pEncode, 0));
   if (m_pFileOut== NULL)
   {
      IFileSinkFilter*pFSF = NULL;
      HR_EXCEPTION(AddFilterToGraph(m_pGB, &m_pFileOut, CLSID_FileWriter, L"FileWriter"));
      HR_EXCEPTION(m_pFileOut->QueryInterface(IID_IFileSinkFilter, (void**) &pFSF));
      HR_EXCEPTION(pFSF->SetFileName(m_sOutputFileName, NULL));
      RELEASE_OBJECT(pFSF);
   }
   HR_EXCEPTION(ConnectFilters(m_pGB, m_pEncode, 0, m_pFileOut, 0));
}

void CAudioSOMDlg::ConnectToOsziloscope(IBaseFilter*pFilterIn, int nPinIn)
{
   HRESULT          hr        = NOERROR;
   if (m_pOsciloscope == NULL)
   {
      HR_EXCEPTION(AddFilterToGraph(m_pGB, &m_pOsciloscope, CLSID_Scope, L"Oscilloscope"));
   }
   HR_EXCEPTION(ConnectFilters(m_pGB, pFilterIn, nPinIn, m_pOsciloscope, 0));
}

void CAudioSOMDlg::ConnectToAudioOut(IBaseFilter*pFilterIn, int nPinIn)
{
   HRESULT          hr        = NOERROR;
   if (m_pOutput == NULL)
   {
      IMoniker*pMoniker = (IMoniker*)m_cAudioOutput.GetItemData(m_cAudioOutput.GetCurSel());
      if (pMoniker == (IMoniker*)CB_ERR) throw HRESULT_ERROR_AT_POS(E_NOINTERFACE);
      HR_EXCEPTION(AddFilterToGraph(m_pGB, &m_pOutput, pMoniker, L"Audio Output"));
      hr = m_pOutput->QueryInterface(IID_IBasicAudio, (void**)&m_pVolume);
   }
   HR_EXCEPTION(ConnectFilters(m_pGB, pFilterIn, nPinIn, m_pOutput, 0));
   UpdateVolume();
}

void CAudioSOMDlg::InitGraphBuilder()
{
   if (m_pGB == NULL)
   {
      HRESULT hr;
      // Create the  graph builder.
      HR_EXCEPTION(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (void **)&m_pGB));
      // Create the capture graph builder.
      HR_EXCEPTION(CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC, IID_ICaptureGraphBuilder2, (void **)&m_pCaptureGB));
      // Associate the filter graph with the capture graph builder
      HR_EXCEPTION(m_pCaptureGB->SetFiltergraph(m_pGB));                  
      // Query the Media Control Interface
      HR_EXCEPTION(m_pGB->QueryInterface(IID_IMediaControl, (void **)&m_pMC));

      // Query the Media Event Interface
//      HR_EXCEPTION(m_pGB->QueryInterface(IID_IMediaEventEx, (void **)&m_pME));
//      HR_REPORT(m_pME->SetNotifyWindow((OAHWND)m_hWnd, WM_GRAPHNOTIFY, 0));
   }
}
void CAudioSOMDlg::ReleaseOutputs()
{
   if (m_pGB)
   {
      if (m_pOutput)  m_pGB->RemoveFilter(m_pOutput);
      if (m_pOsciloscope)  m_pGB->RemoveFilter(m_pOsciloscope);
      if (m_pFileOut)  m_pGB->RemoveFilter(m_pFileOut);
      if (m_pEncode)  m_pGB->RemoveFilter(m_pEncode);
   }
   RELEASE_OBJECT(m_pOsciloscope);
   RELEASE_OBJECT(m_pVolume);
   RELEASE_OBJECT(m_pOutput);
   RELEASE_OBJECT(m_pEncode);
   RELEASE_OBJECT(m_pFileOut);
}

void CAudioSOMDlg::ReleaseSynth()
{
   if (m_pGB)
   {
      if (m_pSynthesizer)  m_pGB->RemoveFilter(m_pSynthesizer);
      if (m_pInfTeeSynth)  m_pGB->RemoveFilter(m_pInfTeeSynth);
   }
   RELEASE_OBJECT(m_ISynthesizer);
   RELEASE_OBJECT(m_pSynthesizer);
   RELEASE_OBJECT(m_pInfTeeSynth);
}

void CAudioSOMDlg::ReleaseAudioIn()
{
   if (m_pGB)
   {
      if (m_pInput)  m_pGB->RemoveFilter(m_pInput);
      if (m_pInfTeeInput) m_pGB->RemoveFilter(m_pInfTeeInput);
   }
   RELEASE_OBJECT(m_pInput);
   RELEASE_OBJECT(m_pInfTeeInput);
}
void CAudioSOMDlg::ReleaseFileIn()
{
   if (m_pGB)
   {
      if (m_pFileIn)     m_pGB->RemoveFilter(m_pFileIn);
      if (m_pInfTeeFile) m_pGB->RemoveFilter(m_pInfTeeFile);
   }
   RELEASE_OBJECT(m_pFileIn);
   RELEASE_OBJECT(m_pInfTeeFile);
}
void CAudioSOMDlg::ReleaseGraph()
{
   //if (m_pME)
   //{
   //   m_pME->SetNotifyWindow(NULL, 0, 0);
   //}
   if (m_dwROT)
   {
      RemoveGraphFromRot(m_dwROT);
      m_dwROT = 0;
   }
   RELEASE_OBJECT(m_pMC);
   RELEASE_OBJECT(m_pME);

   RELEASE_OBJECT(m_pGB);
   RELEASE_OBJECT(m_pCaptureGB);
}

void CAudioSOMDlg::ReleaseInputPins()
{
   int i,n;
    IAMAudioInputMixer *pAMaim = NULL;

   n = m_cInputPin.GetCount();
   for (i=0; i<n; i++)
   {
      pAMaim = (IAMAudioInputMixer*)m_cInputPin.GetItemData(i);
      RELEASE_OBJECT(pAMaim);
   }
}

void CAudioSOMDlg::OnBnClickedBtnStart()
{
   OAFilterState fs;
   if (   m_pMC
       && SUCCEEDED(m_pMC->GetState(0, &fs))
       && fs != State_Running)
   {
      HRESULT hr;
      hr = m_pMC->Run();
      if (FAILED(hr))
      {
         ReportError(HRESULT_ERROR_AT_POS(hr));
      }
      else
      {
         UpdateState();
         IMediaPosition *pMP = NULL;
         m_pGB->QueryInterface(IID_IMediaPosition, (void **)&pMP);
         if (pMP)
         {
            REFTIME refPos, refDuration;
            pMP->get_Duration(&refDuration);
            refPos = (REFTIME)(refDuration * m_cMediaPosition.GetPos() / 1000);
            pMP->put_CurrentPosition(refPos);
            pMP->Release();
            m_nUpdatePositionTimer = (UINT)SetTimer(2, 1000, NULL);
         }
      }
   }
}

void CAudioSOMDlg::OnBnClickedBtnStop()
{
   OAFilterState fs;
   if (   m_pMC
       && SUCCEEDED(m_pMC->GetState(0, &fs))
       && fs == State_Running)
   {
      HRESULT hr;
      hr = m_pMC->Stop();
      if (FAILED(hr))
      {
         ReportError(HRESULT_ERROR_AT_POS(hr));
      }
      else
      {
         UpdateState();
         if (m_OutputFile.m_hFile != INVALID_HANDLE_VALUE)
         {
            m_OutputFile.Seek(4, CFile::begin);
            long lOffset, lSize = (long)m_OutputFile.GetLength();
            m_OutputFile.Write(&lSize, sizeof(long));
            lOffset = sizeof(SWaveFile)+sizeof(WAVEFORMATEX)+sizeof(short);
            m_OutputFile.Seek(lOffset, CFile::begin);
            lSize = lSize - lOffset - sizeof(long);
            m_OutputFile.Write(&lSize, sizeof(long));
            m_OutputFile.Close();
         }
      }

      SAFE_KILL_TIMER(m_nUpdatePositionTimer);
   }
}

void CAudioSOMDlg::OnRDOutClicked(UINT uID)
{
   int n = -1, *pn = NULL;
   switch (uID)
   {
      case IDC_RD_OSZI1: n = 0; pn = &m_nOszi; break;
      case IDC_RD_OSZI2: n = 1; pn = &m_nOszi; break;
      case IDC_RD_OSZI3: n = 2; pn = &m_nOszi; break;
      case IDC_RD_FO1:   n = 0; pn = &m_nFileOut; break;
      case IDC_RD_FO2:   n = 1; pn = &m_nFileOut; break;
      case IDC_RD_AO1:   n = 0; pn = &m_nAudioOut; break;
      case IDC_RD_AO2:   n = 1; pn = &m_nAudioOut; break;
   }
   if (pn)
   {
      if (n == *pn)
      {
         *pn = -1;
         CheckDlgButton(uID, MF_UNCHECKED);
      }
      else
      {
         *pn = n;
      }
   }
}

void CAudioSOMDlg::UpdateState()
{
   OAFilterState fs;
   if (   m_pMC
       && SUCCEEDED(m_pMC->GetState(0, &fs)))
   {
      EnableDlgItem(IDC_BTN_START, fs != State_Running);
      EnableDlgItem(IDC_BTN_STOP , fs != State_Stopped);
   }
   else
   {
      EnableDlgItem(IDC_BTN_START);
      EnableDlgItem(IDC_BTN_STOP, FALSE);
   }
}

void CAudioSOMDlg::EnableDlgItem(UINT nID, BOOL bEnable/*=TRUE*/)
{
   CWnd *pWnd = GetDlgItem(nID);
   if (pWnd)
   {
      pWnd->EnableWindow(bEnable);
   }
}

void CAudioSOMDlg::UpdateVolume()
{
   if (m_pVolume)
   {
      HRESULT hr;
      long lVolume = 0, lBalance = 0;
      hr = m_pVolume->get_Volume(&lVolume);
      if (SUCCEEDED(hr))
      {
         m_cVolume.EnableWindow(TRUE);
         EnableDlgItem(IDC_TXT_VOLUME);

         if (m_cVolume.GetPos() != 0)
         {
            m_pVolume->put_Volume((int)m_cVolume.GetPos()-10000);
         }
         else
         {
            m_cVolume.SetPos(lVolume+10000);
         }
      }
      else
      {
         m_cVolume.EnableWindow(FALSE);
         EnableDlgItem(IDC_TXT_VOLUME, FALSE);
      }
      hr = m_pVolume->get_Balance(&lBalance);
      if (SUCCEEDED(hr))
      {
         m_cBalance.EnableWindow(TRUE);
         EnableDlgItem(IDC_TXT_BALANCE);
         if (m_cBalance.GetPos() != 0)
         {
            m_pVolume->put_Balance((int)m_cBalance.GetPos()-10000);
         }
         else
         {
            m_cBalance.SetPos(lBalance+10000);
         }
      }
      else
      {
         m_cBalance.EnableWindow(FALSE);
         EnableDlgItem(IDC_TXT_BALANCE, FALSE);
      }
   }
   else
   {
      m_cVolume.EnableWindow(FALSE);
      m_cBalance.EnableWindow(FALSE);
   }
}

void CAudioSOMDlg::OnCbnSelchangeComboAudioInput()
{
   int nCurSel = m_cAudioInput.GetCurSel();
   if (nCurSel != m_nAudioInput)
   {
      HRESULT  hr;
      IEnumPins  *pEnum = NULL;
      IPin *pPin = NULL;
      IAMAudioInputMixer *pAMaim = NULL;
      IAMStreamConfig *pCfg      = NULL;
      int nOutPin = 1;

      m_cAudioInputCaps.ResetContent();
      m_cInputPin.ResetContent();
      try
      {
         if (m_pGB && m_pInput)
         {
            m_pGB->RemoveFilter(m_pInput);
            m_pInput->Release();
            m_pInput = NULL;
         }
         InitGraphBuilder();
         IMoniker*pMoniker = (IMoniker*)m_cAudioInput.GetItemData(nCurSel);
         if (pMoniker == (IMoniker*)CB_ERR) throw HRESULT_ERROR_AT_POS(E_NOINTERFACE);
         HR_EXCEPTION(AddFilterToGraph(m_pGB, &m_pInput, pMoniker, L"Audio Input"));

         ReleaseInputPins();
         HR_EXCEPTION(m_pInput->EnumPins(&pEnum));
         pEnum->Reset();
         // Enumerate all pins on this filter
         while((hr = pEnum->Next(1, &pPin, 0)) == S_OK)
         {
            PIN_DIRECTION PinDirThis;

            HR_EXCEPTION(pPin->QueryDirection(&PinDirThis));
            if (PinDirThis == PINDIR_INPUT)
            {
               PIN_INFO pininfo={0};
               int nCur = CB_ERR;

               // Direction matches, so add pin name to listbox
               hr = pPin->QueryPinInfo(&pininfo);
               if (SUCCEEDED(hr))
               {
                  CString str(pininfo.achName);
                  nCur = m_cInputPin.AddString(str);
               }
               // The pininfo structure contains a reference to an IBaseFilter,
               // so you must release its reference to prevent resource a leak.
               pininfo.pFilter->Release();
               if (nCur != CB_ERR)
               {
                  hr = pPin->QueryInterface(IID_IAMAudioInputMixer, (void**)&pAMaim);
                  if (SUCCEEDED(hr))                                    // Query Input Mixer
                  {
                     BOOL bEnabled;
                     if (SUCCEEDED(pAMaim->get_Enable(&bEnabled)) && bEnabled)
                     {
                        m_cInputPin.SetCurSel(nCur);
                     }
                     m_cInputPin.SetItemData(nCur, (DWORD_PTR)pAMaim);
                     pAMaim = NULL;
                  }
                  RELEASE_OBJECT(pAMaim);
               }
            }
            else // output pin
            {
               hr = pPin->QueryInterface(IID_IAMStreamConfig, (void **)&pCfg);
               if (SUCCEEDED(hr))
               {
                  AUDIO_STREAM_CONFIG_CAPS *pASCC = NULL;
                  CString  str;
                  int  i, iSize, iCount;

                  hr = pCfg->GetNumberOfCapabilities(&iCount, &iSize);
                  if (SUCCEEDED(hr))
                  {
                     pASCC = (AUDIO_STREAM_CONFIG_CAPS*) CoTaskMemAlloc(iSize);
                     WAVEFORMATEX *pWFex;
                     AM_MEDIA_TYPE *pmt=NULL;
                     int bits;
                     std::map<int, CString> sortSampleFrq;
                     std::map<int, CString> sortBits;
                     for (i=0; i<iCount; i++)
                     {
                        hr = pCfg->GetStreamCaps(i, &pmt, (BYTE*)pASCC);
                        if (SUCCEEDED(hr))
                        {
                           pWFex = (WAVEFORMATEX*) pmt->pbFormat;
                           str.Format(_T("%d Hz"), pWFex->nSamplesPerSec);
                           sortSampleFrq[pWFex->nSamplesPerSec] = str;
                           bits = pWFex->wBitsPerSample/pWFex->nChannels;
                           switch(bits)
                           {
                           case 8: case 16: case 24: case 32:
                              str.Format(_T("%d bits"), bits);
                              sortBits[bits] = str;
                              break;
                           }
                           DeleteMediaType(pmt);
                        }
                     }
                     std::map<int, CString>::iterator iter;
                     for (iter =sortSampleFrq.begin(); iter != sortSampleFrq.end(); iter++)
                     {
                         iSize = m_cAudioInputCaps.AddString(iter->second);
                         m_cAudioInputCaps.SetItemData(iSize, iter->first);
                         iSize = m_cOutputFrq.AddString(iter->second);
                         m_cOutputFrq.SetItemData(iSize, iter->first);
                     }
                     for (iter =sortBits.begin(); iter != sortBits.end(); iter++)
                     {
                         iSize = m_cOutputBits.AddString(iter->second);
                         m_cOutputBits.SetItemData(iSize, iter->first);
                         iSize = m_cInputBits.AddString(iter->second);
                         m_cInputBits.SetItemData(iSize, iter->first);
                     }
                     SAFE_COTASKMEMFREE(pASCC);
                     RELEASE_OBJECT(pCfg);
                  }
               }
               nOutPin++;
            }
            RELEASE_OBJECT(pPin);
         }
      }
      catch(ErrorStruct *ps)
      {
         ReportError(ps);
         ReleaseAudioIn();
      }
      RELEASE_OBJECT(pPin);
      RELEASE_OBJECT(pEnum);
      RELEASE_OBJECT(pAMaim);
      m_cInputPin.SetCurSel(theApp.GetProfileInt(SECTION_SETTINGS, AUDIO_SOURCE_PIN, 0));
      m_cAudioInputCaps.SetCurSel(theApp.GetProfileInt(SECTION_SETTINGS, AUDIO_SOURCE_CAP, 0));

      m_nAudioInput = nCurSel;
   }
}

void CAudioSOMDlg::OnCbnSelchangeComboAudioOutput()
{
   int nCurSel = m_cAudioOutput.GetCurSel();
   if (nCurSel != m_nAudioOutput)
   {
      HRESULT  hr;
      try
      {
         if (m_pGB && m_pOutput)
         {
            m_pGB->RemoveFilter(m_pOutput);
            m_pOutput->Release();
            m_pOutput = NULL;
            RELEASE_OBJECT(m_pVolume);
         }
         InitGraphBuilder();
         IMoniker*pMoniker = (IMoniker*)m_cAudioOutput.GetItemData(nCurSel);
         if (pMoniker == (IMoniker*)CB_ERR) throw HRESULT_ERROR_AT_POS(E_NOINTERFACE);
         HR_EXCEPTION(AddFilterToGraph(m_pGB, &m_pOutput, pMoniker, L"Audio Output"));
         hr = m_pOutput->QueryInterface(IID_IBasicAudio, (void**)&m_pVolume);
         UpdateVolume();
      }
      catch(ErrorStruct *ps)
      {
         ReportError(ps);
         ReleaseSynth();
      }
      m_nAudioOutput = nCurSel;
   }
}

void CAudioSOMDlg::OnCbnSelchangeComboAudioInputPin()
{
   int i = m_cInputPin.GetCurSel();
   if(i != CB_ERR)
   {
      IAMAudioInputMixer *pAMaim = (IAMAudioInputMixer*)m_cInputPin.GetItemData(i);
      if (pAMaim)
      {
         HRESULT hr;
         pAMaim->put_Enable(TRUE);
         double dLevel;
         hr = pAMaim->get_MixLevel(&dLevel);
         if (SUCCEEDED(hr))
         {
            m_cInputLevel.EnableWindow();
            m_cInputLevel.SetPos((int)(dLevel*1000));
         }
         else
         {
            m_cInputLevel.EnableWindow(FALSE);
         }
      }
   }
}

void CAudioSOMDlg::OnCbnSelchangeComboOutputFrq()
{
//   m_cOutputFrq
}

void CAudioSOMDlg::OnBnClickedBtnSaveSettings()
{
   theApp.WriteProfileInt(SECTION_SETTINGS, AUDIO_SOURCE    , m_cAudioInput.GetCurSel());
   theApp.WriteProfileInt(SECTION_SETTINGS, AUDIO_SOURCE_PIN, m_cInputPin.GetCurSel());
   theApp.WriteProfileInt(SECTION_SETTINGS, AUDIO_SOURCE_LEVEL, m_cInputLevel.GetPos());
   theApp.WriteProfileInt(SECTION_SETTINGS, AUDIO_SOURCE_CAP, m_cAudioInputCaps.GetCurSel());
   theApp.WriteProfileInt(SECTION_SETTINGS, AUDIO_SAMPLE_LEN, m_cSampleLength.GetCurSel());
   theApp.WriteProfileInt(SECTION_SETTINGS, AUDIO_OUT_FRQ   , m_cOutputFrq.GetCurSel());
   theApp.WriteProfileInt(SECTION_SETTINGS, SYNTH_BITS_PER_SAMP, m_cInputBits.GetCurSel());

   theApp.WriteProfileInt(SECTION_SETTINGS, AUDIO_RENDER     , m_cAudioOutput.GetCurSel());
   theApp.WriteProfileInt(SECTION_SETTINGS, AUDIO_OUT_VOLUME , m_cVolume.GetPos());
   theApp.WriteProfileInt(SECTION_SETTINGS, AUDIO_OUT_BALANCE, m_cBalance.GetPos());

   theApp.WriteProfileInt(SECTION_SYNTHESIZER, SYNTH_WAVEFORM, m_SynthesizerDlg.m_nWaveForm);
   theApp.WriteProfileInt(SECTION_SYNTHESIZER, SYNTH_CORRELATED, m_SynthesizerDlg.m_bCorrelated);
   theApp.WriteProfileInt(SECTION_SYNTHESIZER, SYNTH_FREQUENCY1, m_SynthesizerDlg.m_nFrequency1);
   theApp.WriteProfileInt(SECTION_SYNTHESIZER, SYNTH_FREQUENCY2, m_SynthesizerDlg.m_nFrequency2);
   theApp.WriteProfileInt(SECTION_SYNTHESIZER, SYNTH_TIME, m_SynthesizerDlg.m_nTime);
   theApp.WriteProfileInt(SECTION_SYNTHESIZER, SYNTH_PHASE, m_SynthesizerDlg.m_nPhase);
   theApp.WriteProfileInt(SECTION_SYNTHESIZER, SYNTH_MAGNITUDE1, m_SynthesizerDlg.m_nMagnitude1);
   theApp.WriteProfileInt(SECTION_SYNTHESIZER, SYNTH_MAGNITUDE2, m_SynthesizerDlg.m_nMagnitude2);
   theApp.WriteProfileInt(SECTION_SYNTHESIZER, SYNTH_OFFSET1, m_SynthesizerDlg.m_nOffset1);
   theApp.WriteProfileInt(SECTION_SYNTHESIZER, SYNTH_OFFSET2, m_SynthesizerDlg.m_nOffset2);
   theApp.WriteProfileInt(SECTION_SYNTHESIZER, SYNTH_BITS_PER_SAMP, m_cOutputBits.GetCurSel());

   theApp.WriteProfileString(SECTION_FILE, FILE_INPUT_NAME, m_sInputFileName);
   theApp.WriteProfileString(SECTION_FILE, FILE_OUTPUT_NAME, m_sOutputFileName);
}

void CAudioSOMDlg::OnBnClickedBtnResetAll()
{
   ReleaseSynth();
   ReleaseAudioIn();
   ReleaseFileIn();
   ReleaseOutputs();
   ReleaseGraph();
   UpdateState();
}

void CAudioSOMDlg::OnBnClickedBtnshowSynth()
{
   if (m_SynthesizerDlg.m_hWnd)
   {
      m_SynthesizerDlg.BringWindowToTop();
   }
   else
   {
      m_SynthesizerDlg.Create(CSynthesizerDlg::IDD, this);
      m_SynthesizerDlg.SetParentWnd(this);
   }
}

void CAudioSOMDlg::OnBnClickedBtnshowSynthFilter()
{
   if (m_pSynthesizer)
   {
      HRESULT hr;
      hr = ShowFilterPropertyPage(m_pSynthesizer, m_hWnd);
      if (hr == S_OK)
      {
         int n;
         m_ISynthesizer->get_BitsPerSample(&n);
         theApp.WriteProfileInt(SECTION_SYNTHESIZER, SYNTH_BITS_PER_SAMP, n);
         m_ISynthesizer->get_Channels(&n);
         theApp.WriteProfileInt(SECTION_SYNTHESIZER, SYNTH_CHANNEL, n);

      }
      HR_MSG_BOX(hr);
   }
}

void CAudioSOMDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
   if (m_pVolume)
   {
      if ( pScrollBar->m_hWnd == m_cBalance.m_hWnd && m_cBalance.IsWindowEnabled())
      {
         nPos = m_cBalance.GetPos();
         m_pVolume->put_Balance((int)nPos-10000);
      }
      else if (pScrollBar->m_hWnd == m_cVolume.m_hWnd && m_cVolume.IsWindowEnabled())
      {
         nPos = m_cVolume.GetPos();
         m_pVolume->put_Volume((int)nPos-10000);
      }
      else if (pScrollBar->m_hWnd == m_cInputLevel.m_hWnd && m_cInputLevel.IsWindowEnabled())
      {
         int nSel = m_cInputPin.GetCurSel();
         if (nSel != -1)
         {
            IAMAudioInputMixer*pAMaim = (IAMAudioInputMixer*)m_cInputPin.GetItemData(nSel);
            double dLevel = (m_cInputLevel.GetPos()) / 1000.0;
            pAMaim->put_MixLevel(dLevel);
         }
      }
   }
   CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CAudioSOMDlg::UpdateSynthesizer(UINT nID)
{
   if (m_ISynthesizer)
   {
      switch(nID)
      { 
         case IDC_EDT_FREQUENCY1: 
            m_ISynthesizer->put_Frequency(m_SynthesizerDlg.m_nFrequency1);
            m_ISynthesizer->put_SweepRange(m_SynthesizerDlg.m_nFrequency1, m_SynthesizerDlg.m_nFrequency2);
            break;
         case IDC_EDT_FREQUENCY2: 
            m_ISynthesizer->put_SweepRange(m_SynthesizerDlg.m_nFrequency1, m_SynthesizerDlg.m_nFrequency2);
            break;
         case IDC_EDT_TIME:
            m_ISynthesizer->put_TimePercent(m_SynthesizerDlg.m_nTime);
            break;
         case IDC_EDT_PHASE:      
            m_ISynthesizer->put_OffsetDegree(m_SynthesizerDlg.m_nPhase);
            break;
         case IDC_EDT_MAGNITUDE1:
            m_ISynthesizer->put_Amplitude(m_SynthesizerDlg.m_nMagnitude1);
            break;
         case IDC_EDT_MAGNITUDE2:
            m_ISynthesizer->put_Amplitude2(m_SynthesizerDlg.m_nMagnitude2); 
            break;
         case IDC_EDT_OFFSET1:
         case IDC_EDT_OFFSET2:
            m_ISynthesizer->put_Offset(m_SynthesizerDlg.m_nOffset1, m_SynthesizerDlg.m_nOffset2);
            break;
         case IDC_BTN_WF_SINE:
            m_ISynthesizer->put_Waveform(m_SynthesizerDlg.m_nWaveForm);
            break;
         case IDC_BTN_WF_KORR:
            m_ISynthesizer->put_Correlated(m_SynthesizerDlg.m_bCorrelated);
            break;
      }
   }
}

void CAudioSOMDlg::OnBnClickedBtnInitialize()
{
   UpdateData();
   if (m_nOszi == OSZI_AUDIO || m_nFileOut == FILE_AUDIO)
   {
      InitAudioIn();
   }
   if (m_nOszi == OSZI_FILE || m_nAudioOut == AUDIO_FILE)
   {
      InitFileIn();
   }
   if (m_nOszi == OSZI_WAVE || m_nFileOut == FILE_WAVE || m_nAudioOut == AUDIO_WAVE)
   {
      InitSynthesizer();
   }
}

void CAudioSOMDlg::OnBnClickedCkRot()
{
   if (IsDlgButtonChecked(IDC_CK_ROT))
   {
      AddGraphToRot(m_pGB, &m_dwROT);
   }
   else if (m_dwROT)
   {
      RemoveGraphFromRot(m_dwROT);
      m_dwROT = 0;
   }
}

void CAudioSOMDlg::OnTimer(UINT nIDEvent)
{
   if (m_nUpdatePositionTimer == nIDEvent && m_pGB)
   {
      IMediaPosition *pMP = NULL;
      m_pGB->QueryInterface(IID_IMediaPosition, (void **)&pMP);
      if (pMP)
      {
         REFTIME refPos, refDuration;
         pMP->get_CurrentPosition(&refPos);
         pMP->get_Duration(&refDuration);
            m_cMediaPosition.SetPos((int)(refPos * 1000 / refDuration));
         pMP->Release();
      }
   }
   CDialog::OnTimer(nIDEvent);
}

BOOL CAudioSOMDlg::OnToolTipNotify(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
   // need to handle both ANSI and UNICODE versions of the message
   TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*)pNMHDR;
   CString strTipText;
   UINT nID = (UINT)pNMHDR->idFrom;
   if (pNMHDR->code == TTN_NEEDTEXT && (pTTT->uFlags & TTF_IDISHWND))
   {
      // idFrom is actually the HWND of the tool
      nID = (UINT)::GetDlgCtrlID((HWND)pNMHDR->idFrom);
   }

   if (nID != 0)
   {// will be zero on a separator

	  pTTT->lpszText = MAKEINTRESOURCE(nID);
      pTTT->hinst = AfxGetResourceHandle();
      return(TRUE);
   }

   _tcsncpy(pTTT->szText, strTipText, sizeof(pTTT->szText));

   *pResult = 0;

   return TRUE;    // message was handled
}


void CAudioSOMDlg::OnBnClickedBtnFileOutput()
{
   CFileDialog dlg(FALSE, 0, m_sOutputFileName);
   if (dlg.DoModal() == IDOK)
   {
      m_sOutputFileName = dlg.GetPathName();
      SetDlgItemText(IDC_EDT_OUTPUT_FILE_NAME, m_sOutputFileName);//dlg.GetFileName());
   }
}

void CAudioSOMDlg::OnBnClickedBtnFileInput()
{
   CFileDialog dlg(TRUE, 0, m_sInputFileName);
   if (dlg.DoModal() == IDOK)
   {
      m_sInputFileName = dlg.GetPathName();
      SetDlgItemText(IDC_EDT_INPUT_FILE_NAME, m_sInputFileName);//dlg.GetFileName());
   }
}
