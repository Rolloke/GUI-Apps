// SaveCurveAsWave.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "CARA2DV.h"
#include "SaveCurveAsWave.h"
#include "..\CurveLabel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CSaveCurveAsWave 
extern "C" __declspec(dllimport) BOOL DDX_TextVar(CDataExchange*, int, int, double&, bool bThrow=true);


#define SAMPLE_RATE_8000   0
#define SAMPLE_RATE_11025  1
#define SAMPLE_RATE_22050  2
#define SAMPLE_RATE_44100  3
#define WFMT_MONO          0
#define WFMT_STEREO        1
#define BLOCK_ALIGN_8_BIT  0
#define BLOCK_ALIGN_16_BIT 1

IMPLEMENT_DYNAMIC(CSaveCurveAsWave, CFileDialog)

long CSaveCurveAsWave::gm_lOldProc = 0;

CSaveCurveAsWave::CSaveCurveAsWave(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
   DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
   CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
   m_lpszTemplateName = MAKEINTRESOURCE(IDD_SAVE_WAVE_CURVE);
   if (m_ofn.Flags & OFN_ENABLETEMPLATE)
   {
      m_ofn.lpTemplateName = m_lpszTemplateName;
   }

	//{{AFX_DATA_INIT(CSaveCurveAsWave)
	m_nBlockAlign   = 1;
	m_nChannels     = 0;
	m_nLeftChannel  = 0;
	m_nRightChannel = 0;
	//}}AFX_DATA_INIT

   m_nFileTypeSel  = -1;

	m_nSampleRate   = SAMPLE_RATE_22050;

   m_dCurve1Min    = -1.0;
	m_dCurve1Max    =  1.0;
	m_dCurve2Min    = -1.0;
	m_dCurve2Max    =  1.0;
   m_dCurve1Offset =  0.0;
   m_dCurve1Level  = 90.0;
   m_dCurve2Offset =  0.0;
   m_dCurve2Level  = 90.0;
   m_fFrequency    = 1;
   m_nCurveCount   = 1;
   m_nCurveValues  = 0;
   m_nFileSize     = 0;
	m_fWaveTime     = 0;
   m_pclLeftIn     = NULL;
   m_pclRightIn    = NULL;
   m_pclLeftOut    = NULL;
   m_pclRightOut   = NULL;
   m_bConcat1stValue = false;
   ZeroMemory(&m_wfEX, sizeof(WAVEFORMATEX));
   m_wfEX.wFormatTag = WAVE_FORMAT_PCM;
//   strncpy(m_wfEX.sData, "data", 4);
}

CSaveCurveAsWave::~CSaveCurveAsWave()
{
}

void CSaveCurveAsWave::DoDataExchange(CDataExchange* pDX)
{
	CFileDialog::DoDataExchange(pDX);
   if (m_ofn.Flags & OFN_ENABLETEMPLATE)
   {
	   //{{AFX_DATA_MAP(CSaveCurveAsWave)
	   DDX_Control(pDX, IDC_WD_CURVE_COUNT_BUDDY, m_cCurveCountSpin);
	   DDX_Control(pDX, IDC_WD_RIGHT_CHANNEL, m_cRightChannel);
	   DDX_Control(pDX, IDC_WD_LEFT_CHANNEL, m_cLeftChannel);
	   DDX_CBIndex(pDX, IDC_WD_BLOCK_ALIGN, m_nBlockAlign);
	   DDX_CBIndex(pDX, IDC_WD_SAMPLE_RATE, m_nSampleRate);
	   DDX_Radio(pDX, IDC_WD_CANNEL1, m_nChannels);
	   DDX_CBIndex(pDX, IDC_WD_LEFT_CHANNEL, m_nLeftChannel);
	   DDX_CBIndex(pDX, IDC_WD_RIGHT_CHANNEL, m_nRightChannel);
	   //}}AFX_DATA_MAP

      DDX_Check(pDX, IDC_WD_CONCAT_1ST_VALUE, m_bConcat1stValue);
	   DDX_Text(pDX, IDC_WD_CURVE1_MIN, m_dCurve1Min);
	   DDX_Text(pDX, IDC_WD_CURVE1_MAX, m_dCurve1Max);
	   DDX_Text(pDX, IDC_WD_CURVE2_MIN, m_dCurve2Min);
	   DDX_Text(pDX, IDC_WD_CURVE2_MAX, m_dCurve2Max);

      DDX_TextVar(pDX, IDC_WD_CURVE1_OFFSET, FLT_DIG+1, m_dCurve1Offset);
	   DDX_TextVar(pDX, IDC_WD_CURVE1_LEVEL , FLT_DIG+1, m_dCurve1Level);
	   DDX_TextVar(pDX, IDC_WD_CURVE2_OFFSET, FLT_DIG+1, m_dCurve2Offset);
	   DDX_TextVar(pDX, IDC_WD_CURVE2_LEVEL , FLT_DIG+1, m_dCurve2Level);

      if (pDX->m_bSaveAndValidate)
      {
         bool bChanged = false;
         if (m_dCurve1Min > m_dCurve1Max)
         {
            swap(m_dCurve1Min, m_dCurve1Max);
            bChanged = true;
         }
         if (m_dCurve2Min > m_dCurve2Max)
         {
            swap(m_dCurve2Min, m_dCurve2Max);
            bChanged = true;
         }
         if (bChanged)
         {
            pDX->m_bSaveAndValidate = false;
	         DDX_Text(pDX, IDC_WD_CURVE1_MIN, m_dCurve1Min);
	         DDX_Text(pDX, IDC_WD_CURVE1_MAX, m_dCurve1Max);
	         DDX_Text(pDX, IDC_WD_CURVE2_MIN, m_dCurve2Min);
	         DDX_Text(pDX, IDC_WD_CURVE2_MAX, m_dCurve2Max);
            pDX->m_bSaveAndValidate = true;
         }
      }
      else
      {
         DDX_Text(pDX, IDC_WD_FILE_SIZE, m_nFileSize);
	      DDX_Text(pDX, IDC_WD_WAVE_TIME, m_fWaveTime);
         DDX_Text(pDX, IDC_WD_WAVE_FRQ , m_fFrequency);
      }
   }
}


BEGIN_MESSAGE_MAP(CSaveCurveAsWave, CFileDialog)
	//{{AFX_MSG_MAP(CSaveCurveAsWave)
	ON_BN_CLICKED(IDC_WD_CANNEL1, OnWdCannels)
	ON_CBN_SELCHANGE(IDC_WD_SAMPLE_RATE, OnChangeParams)
	ON_BN_CLICKED(IDC_WD_DEFAULT_SETTINGS, OnWdDefaultSettings)
	ON_BN_CLICKED(IDC_WD_CONCAT_1ST_VALUE, OnChangeWdCurveCount)
	ON_EN_CHANGE(IDC_WD_CURVE_COUNT, OnChangeWdCurveCount)
	ON_BN_CLICKED(IDC_WD_CANNEL2, OnWdCannels)
	ON_CBN_SELCHANGE(IDC_WD_BLOCK_ALIGN, OnChangeParams)
	ON_CBN_SELCHANGE(IDC_WD_RIGHT_CHANNEL, OnSetChannels)
	ON_CBN_SELCHANGE(IDC_WD_LEFT_CHANNEL, OnSetChannels)
	ON_EN_KILLFOCUS(IDC_WD_CURVE1_MAX, OnKillfocusWdCurveMinMax)
	ON_EN_KILLFOCUS(IDC_WD_CURVE1_MIN, OnKillfocusWdCurveMinMax)
	ON_EN_KILLFOCUS(IDC_WD_CURVE2_MAX, OnKillfocusWdCurveMinMax)
	ON_EN_KILLFOCUS(IDC_WD_CURVE2_MIN, OnKillfocusWdCurveMinMax)
	ON_EN_KILLFOCUS(IDC_WD_CURVE1_OFFSET, OnKillfocusOffsetAndLevel)
	ON_EN_KILLFOCUS(IDC_WD_CURVE1_LEVEL, OnKillfocusOffsetAndLevel)
	ON_EN_KILLFOCUS(IDC_WD_CURVE2_OFFSET, OnKillfocusOffsetAndLevel)
	ON_EN_KILLFOCUS(IDC_WD_CURVE2_LEVEL, OnKillfocusOffsetAndLevel)
	ON_EN_CHANGE(IDC_WD_CURVE1_OFFSET, OnChangeOffsetAndLevel1)
	ON_EN_CHANGE(IDC_WD_CURVE2_OFFSET, OnChangeOffsetAndLevel2)
	ON_EN_CHANGE(IDC_WD_CURVE1_MAX, OnChangeMinMax1)
	ON_EN_CHANGE(IDC_WD_CURVE2_MAX, OnChangeMinMax2)
	ON_EN_CHANGE(IDC_WD_CURVE1_LEVEL, OnChangeOffsetAndLevel1)
	ON_EN_CHANGE(IDC_WD_CURVE2_LEVEL, OnChangeOffsetAndLevel2)
	ON_EN_CHANGE(IDC_WD_CURVE1_MIN, OnChangeMinMax1)
	ON_EN_CHANGE(IDC_WD_CURVE2_MIN, OnChangeMinMax2)
   ON_MESSAGE(WM_HELP, OnHelp)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CSaveCurveAsWave 

void CSaveCurveAsWave::OnChangeOffsetAndLevel1()
{
   m_bOffsetOrLevel1Changed = true;
}
void CSaveCurveAsWave::OnChangeOffsetAndLevel2()
{
   m_bOffsetOrLevel2Changed = true;
}
void CSaveCurveAsWave::OnChangeMinMax1()
{ 
   m_bMinOrMax1Changed = false;
}
void CSaveCurveAsWave::OnChangeMinMax2()
{
   m_bMinOrMax2Changed = false;
}

BOOL CSaveCurveAsWave::OnInitDialog() 
{
 	CFileDialog::OnInitDialog();
   if (!(m_ofn.Flags & OFN_ENABLETEMPLATE)) return true;

   if ((m_pclLeftIn == NULL) && (m_pclRightIn == NULL)) return false;

   HWND hwndParent = ::GetParent(m_hWnd);
   HWND hwndCombo  = ::GetDlgItem(hwndParent, 0x0470);
   gm_lOldProc = ::SetWindowLong(hwndCombo, GWL_WNDPROC, (LONG)WindowProc);
   if (::GetWindowLong(hwndCombo, GWL_USERDATA) == 0)
   {
      ::SetWindowLong(hwndCombo, GWL_USERDATA, (LONG)this);
   }
   m_nFileTypeMax = ::SendMessage(hwndCombo, CB_GETCOUNT, 0, 0);

   m_bMinOrMax1Changed = false;
   m_bMinOrMax2Changed = false;

   m_cCurveCountSpin.SetPos(1);

   if (m_pclLeftIn)
   {
      m_cLeftChannel.AddString(m_pclLeftIn->GetText());
      m_cRightChannel.AddString(m_pclLeftIn->GetText());
      m_nCurveValues  = m_pclLeftIn->GetNumValues();
   }

   if (m_pclRightIn)
   {
      m_cLeftChannel.AddString(m_pclRightIn->GetText());
      m_cRightChannel.AddString(m_pclRightIn->GetText());
      m_nCurveValues  = min(m_nCurveValues, m_pclRightIn->GetNumValues());
   }

   if ((m_pclLeftIn) && (m_pclRightIn))      // Beide Kurven belegt Stereo
   {
      m_nChannels     = WFMT_STEREO;
      m_nLeftChannel  = 0;
      m_nRightChannel = 1;
   }
   else                                      // Eine Kurve belegt Mono
   {
      m_nChannels     = WFMT_MONO;
      m_nLeftChannel  = 0;
      m_nRightChannel = 0;
   }
   m_cLeftChannel.SetCurSel(m_nLeftChannel);
   m_cRightChannel.SetCurSel(m_nRightChannel);

   CDataExchange dx(this, false);
   DDX_Radio(&dx, IDC_WD_CANNEL1, m_nChannels);
   m_nChannels = -1;
   OnWdCannels();

   bool bRight = (m_pclRightOut != NULL) ? true: false;
   if (!bRight) m_pclRightOut = m_pclLeftOut;
   m_bOffsetOrLevel1Changed = true;
   m_bOffsetOrLevel2Changed = true;
   OnKillfocusOffsetAndLevel();
   if (!bRight) m_pclRightOut = NULL;

   OnWdDefaultSettings();
   GetCurSelFileType(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CSaveCurveAsWave::OnWdCannels() 
{
   CDataExchange dx(this, true);
   int nChannels = m_nChannels;
   DDX_Radio(&dx, IDC_WD_CANNEL1, m_nChannels);
   if (nChannels != m_nChannels)
   {
      bool bEnable2 = (m_nChannels == WFMT_MONO) ? false : true;

      m_cRightChannel.EnableWindow(bEnable2);
      GetDlgItem(IDC_WD_CURVE2_MIN)->EnableWindow(bEnable2);
      GetDlgItem(IDC_WD_CURVE2_MAX)->EnableWindow(bEnable2);
      GetDlgItem(IDC_WD_CURVE2_OFFSET)->EnableWindow(bEnable2);
      GetDlgItem(IDC_WD_CURVE2_LEVEL)->EnableWindow(bEnable2);

      CalcFileSize();
      OnSetChannels();
   }
}

void CSaveCurveAsWave::OnOK() 
{
   if (UpdateData(true))
   {
      CalcFileSize();
      OnSetChannels();
   }
	CFileDialog::OnOK();
}

BOOL CSaveCurveAsWave::OnFileNameOK( )
{
   if (UpdateData(true))
   {
      CalcFileSize();
      OnSetChannels();
      return CFileDialog::OnFileNameOK();
   }
   return false;
}

void CSaveCurveAsWave::OnCancel() 
{
	CFileDialog::OnCancel();
}

void CSaveCurveAsWave::OnChangeParams()
{
   if (UpdateData(true))
   {
      CalcFileSize();
   }
}

void CSaveCurveAsWave::OnChangeWdCurveCount()
{
   if (m_cCurveCountSpin.m_hWnd == NULL) return;
   char  text[64], *ptr = NULL;
   CDataExchange dx(this, true);
   DDX_Check(&dx, IDC_WD_CONCAT_1ST_VALUE, m_bConcat1stValue);

   GetDlgItem(IDC_WD_CURVE_COUNT)->GetWindowText(text, 64);
   ptr = strstr(text, ".");
   while (ptr)
   {
      for (int i=0; ptr[i]!=0; i++) ptr[i] = ptr[i+1];
      ptr = strstr(text, ".");
   }

   m_nCurveCount = atoi(text);
   
   m_nSampleLength = (m_nCurveValues * m_nCurveCount);
   if (m_bConcat1stValue) m_nSampleLength++;
   m_nSampleLength *= m_wfEX.nBlockAlign;

   m_nFileSize = m_nSampleLength >> 10;

   m_fWaveTime = (float)m_nCurveValues*(float)m_nCurveCount * 1000.0f / (float)m_wfEX.nSamplesPerSec;

   if (m_nCurveValues>0)
      m_fFrequency = (float)m_wfEX.nSamplesPerSec / (float)m_nCurveValues;

   UpdateData(false);
}


void CSaveCurveAsWave::CalcFileSize()
{
   if (m_pclLeftIn)
   {
      if (m_nChannels == WFMT_MONO)
         m_wfEX.nChannels = 1;
      else
         m_wfEX.nChannels = 2;


      if (m_nBlockAlign == BLOCK_ALIGN_8_BIT)
      {
         m_wfEX.nBlockAlign    = m_wfEX.nChannels;             // 1 Byte
         m_wfEX.wBitsPerSample = 8;
      }
      else
      {
         m_wfEX.nBlockAlign    = 2 * m_wfEX.nChannels;         // 2 Byte
         m_wfEX.wBitsPerSample = 16;
      }

      long  nMax = 2147483648 / m_nCurveValues / m_wfEX.nBlockAlign;
      if (m_nCurveCount > nMax)
      {
         m_cCurveCountSpin.SetPos(1);
      }
      m_cCurveCountSpin.SetRange32(1, nMax);

      switch (m_nSampleRate)
      {
         case SAMPLE_RATE_8000 : m_wfEX.nSamplesPerSec =  8000; break;
         case SAMPLE_RATE_11025: m_wfEX.nSamplesPerSec = 11025; break;
         case SAMPLE_RATE_22050: m_wfEX.nSamplesPerSec = 22050; break;
         case SAMPLE_RATE_44100: m_wfEX.nSamplesPerSec = 44100; break;
         default: return;
      }
      m_wfEX.nAvgBytesPerSec = m_wfEX.nSamplesPerSec * m_wfEX.nBlockAlign;

      OnChangeWdCurveCount();
   }
}

void CSaveCurveAsWave::OnWdDefaultSettings() 
{
   if (m_pclLeftOut)
   {
      GetMinMaxZeroLevel(m_pclLeftOut, m_dCurve1Min, m_dCurve1Max);
   }

   if (m_pclRightOut)
   {
      GetMinMaxZeroLevel(m_pclRightOut, m_dCurve2Min, m_dCurve2Max);
   }
   GetLevelAndOffset();
   UpdateData(false);	
}

void CSaveCurveAsWave::OnKillfocusOffsetAndLevel()
{
   CDataExchange dx(this, true);
   if (m_bOffsetOrLevel1Changed)
   {
      DDX_TextVar(&dx, IDC_WD_CURVE1_OFFSET, FLT_DIG+1, m_dCurve1Offset, false);
	   DDX_TextVar(&dx, IDC_WD_CURVE1_LEVEL , FLT_DIG+1, m_dCurve1Level , false);
   }
   if(m_bOffsetOrLevel2Changed)
   {
	   DDX_TextVar(&dx, IDC_WD_CURVE2_OFFSET, FLT_DIG+1, m_dCurve2Offset, false);
	   DDX_TextVar(&dx, IDC_WD_CURVE2_LEVEL , FLT_DIG+1, m_dCurve2Level , false);
   }

   GetMinMaxValues();

   dx.m_bSaveAndValidate = false;
   if (m_bOffsetOrLevel1Changed)
   {
	   DDX_Text(&dx, IDC_WD_CURVE1_MIN, m_dCurve1Min);
	   DDX_Text(&dx, IDC_WD_CURVE1_MAX, m_dCurve1Max);
   }
   if(m_bOffsetOrLevel2Changed)
   {
	   DDX_Text(&dx, IDC_WD_CURVE2_MIN, m_dCurve2Min);
	   DDX_Text(&dx, IDC_WD_CURVE2_MAX, m_dCurve2Max);
   }

   m_bOffsetOrLevel1Changed = false;
   m_bOffsetOrLevel2Changed = false;
   m_bMinOrMax1Changed      = false;
   m_bMinOrMax2Changed      = false;
}

void CSaveCurveAsWave::OnKillfocusWdCurveMinMax()
{
   CDataExchange dx(this, true);
   if (m_bMinOrMax1Changed)
   {
	   DDX_Text(&dx, IDC_WD_CURVE1_MIN, m_dCurve1Min);
	   DDX_Text(&dx, IDC_WD_CURVE1_MAX, m_dCurve1Max);
   }
   if (m_bMinOrMax2Changed)
   {
	   DDX_Text(&dx, IDC_WD_CURVE2_MIN, m_dCurve2Min);
	   DDX_Text(&dx, IDC_WD_CURVE2_MAX, m_dCurve2Max);
   }
   
   GetLevelAndOffset();

   dx.m_bSaveAndValidate = false;
   if (m_bMinOrMax1Changed)
   {
      DDX_Text(&dx, IDC_WD_CURVE1_OFFSET, m_dCurve1Offset);
	   DDX_Text(&dx, IDC_WD_CURVE1_LEVEL , m_dCurve1Level);
   }
   if (m_bMinOrMax2Changed)
   {
	   DDX_Text(&dx, IDC_WD_CURVE2_OFFSET, m_dCurve2Offset);
	   DDX_Text(&dx, IDC_WD_CURVE2_LEVEL , m_dCurve2Level);
   }

   m_bOffsetOrLevel1Changed = false;
   m_bOffsetOrLevel2Changed = false;
   m_bMinOrMax1Changed      = false;
   m_bMinOrMax2Changed      = false;
}

void CSaveCurveAsWave::GetMinMaxZeroLevel(CCurveLabel *pcl, double &dMinOut, double &dMaxOut)
{
   double dOffset, dMin, dMax;
   if (pcl)
   {
      dMin = pcl->GetYMin();
      dMax = pcl->GetYMax();
      GetOffset(pcl, dOffset);
      dMax = dMax - dOffset;
      dMin = dOffset - dMin;
      if (dMin < dMax)
      {
         dMinOut = dOffset - dMax;
	      dMaxOut = dOffset + dMax;
      }
      else
      {
         dMinOut = dOffset - dMin;
	      dMaxOut = dOffset + dMin;
      }
   }
}

void CSaveCurveAsWave::OnSetChannels()
{
   int nSel;
   CCurveLabel *pOldLeft  = m_pclLeftOut;
   CCurveLabel *pOldRight = m_pclRightOut;
   nSel = m_cLeftChannel.GetCurSel();
   if (nSel != CB_ERR) m_nLeftChannel = nSel;
   nSel = m_cRightChannel.GetCurSel();
   if (nSel != CB_ERR) m_nRightChannel = nSel;

   if (m_nChannels == WFMT_MONO)
   {
      m_pclLeftOut = (m_nLeftChannel == 0) ? m_pclLeftIn : m_pclRightIn;
   }
   else
   {
      m_pclLeftOut  = (m_nLeftChannel  == 0) ? m_pclLeftIn : m_pclRightIn;
      m_pclRightOut = (m_nRightChannel == 0) ? m_pclLeftIn : m_pclRightIn;
   }
   
   if (pOldLeft  != m_pclLeftOut ) m_bOffsetOrLevel1Changed = true;
   if (pOldRight != m_pclRightOut) m_bOffsetOrLevel2Changed = true;
   if (m_bOffsetOrLevel1Changed || m_bOffsetOrLevel2Changed)
      OnKillfocusOffsetAndLevel();
}

void CSaveCurveAsWave::GetOffset(CCurveLabel *pcl, double &dOffset)
{
   dOffset = 0;
   int i, nValues = pcl->GetNumValues();
   for (i=0; i<nValues; i++)
      dOffset +=  pcl->GetY_Value(i);
   if (nValues>1) dOffset /= (double) nValues;
}

void CSaveCurveAsWave::GetLevelAndOffset()
{
   double dOffset, dMin, dMax, dRange;
   if (m_pclLeftOut && m_bMinOrMax1Changed)
   {
      dMin = m_pclLeftOut->GetYMin();
      dMax = m_pclLeftOut->GetYMax();
      GetOffset(m_pclLeftOut, dOffset);
      dOffset = ((m_dCurve1Min + m_dCurve1Max) * 0.5 - dOffset);
      dRange  = m_dCurve1Max - m_dCurve1Min;
      m_dCurve1Offset = (dOffset / dRange * 100.0);
      m_dCurve1Level  = ((dMax - dMin) / dRange * 100.0);
   }

   if (m_pclRightOut && m_bMinOrMax2Changed)
   {
      dMin = m_pclRightOut->GetYMin();
      dMax = m_pclRightOut->GetYMax();
      GetOffset(m_pclRightOut, dOffset);
      dOffset = ((m_dCurve2Min + m_dCurve2Max) * 0.5 - dOffset);
      dRange  = m_dCurve2Max - m_dCurve2Min;
      m_dCurve2Offset = (dOffset / dRange * 100);
      m_dCurve2Level  = ((dMax - dMin) / dRange * 100.0);
   }
}

void CSaveCurveAsWave::GetMinMaxValues()
{
   double dOffset, dMin, dMax, dHalfRange;
   if (m_pclLeftOut && m_bOffsetOrLevel1Changed)
   {
      dMin = m_pclLeftOut->GetYMin();
      dMax = m_pclLeftOut->GetYMax();
      GetOffset(m_pclLeftOut, dOffset);
      dHalfRange = 50.0 * (dMax - dMin) / m_dCurve1Level;
      dOffset += (m_dCurve1Offset * dHalfRange * 0.02);
      m_dCurve1Max = dOffset + dHalfRange;
      m_dCurve1Min = dOffset - dHalfRange;
   }

   if (m_pclRightOut && m_bOffsetOrLevel2Changed)
   {
      dMin = m_pclRightOut->GetYMin();
      dMax = m_pclRightOut->GetYMax();
      GetOffset(m_pclRightOut, dOffset);
      dHalfRange = 50.0 * (dMax - dMin) / m_dCurve2Level;
      dOffset += (m_dCurve2Offset * dHalfRange * 0.02);
      m_dCurve2Max = dOffset + dHalfRange;
      m_dCurve2Min = dOffset - dHalfRange;
   }
}

void CSaveCurveAsWave::OnDestroy() 
{
   HWND hwndParent = ::GetParent(m_hWnd);
   HWND hwndCombo  = ::GetDlgItem(hwndParent, 0x0470);
   gm_lOldProc = ::SetWindowLong(hwndCombo, GWL_WNDPROC, (LONG)gm_lOldProc);
   gm_lOldProc = 0;
	CFileDialog::OnDestroy();
}

LRESULT CALLBACK CSaveCurveAsWave::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   if (gm_lOldProc == 0) return 0;
   if (uMsg == CB_GETCURSEL)
   {
      LRESULT lRes = CallWindowProc((WNDPROC) gm_lOldProc, hwnd, uMsg, wParam, lParam);
      CSaveCurveAsWave*pThis = (CSaveCurveAsWave*) ::GetWindowLong(hwnd, GWL_USERDATA);
      if (pThis) pThis->GetCurSelFileType(lRes);
      return lRes;
   }
   return CallWindowProc((WNDPROC) gm_lOldProc, hwnd, uMsg, wParam, lParam);
}

void CSaveCurveAsWave::GetCurSelFileType(long nSel)
{
   if (nSel != m_nFileTypeSel)
   {
      CRect rcArea;
      m_nFileTypeSel = nSel;
      GetDlgItem(IDC_WAVE_GROUP)->GetWindowRect(&rcArea);
      long lParam[2] = {(long)&rcArea, 0};
      if (m_nFileTypeSel == 2) lParam[1] = 1; // Wavedateien
      ::EnumChildWindows(m_hWnd, EnableItems, (LPARAM)lParam);
      GetDlgItem(IDC_WAVE_FORMAT_GROUP)->EnableWindow(lParam[1]);
      if (lParam[1])
      {
         m_nChannels = -1;
         OnWdCannels();
      }
   }
}

BOOL CALLBACK CSaveCurveAsWave::EnableItems(HWND hwnd, LPARAM lParam)
{
   CRect  rcWnd;
   CRect *prcWnd = (CRect*)((long*)lParam)[0];
   ::GetWindowRect(hwnd, &rcWnd);
   if (prcWnd->PtInRect(rcWnd.TopLeft()) && prcWnd->PtInRect(rcWnd.BottomRight()))
   {
      ::EnableWindow(hwnd, ((long*)lParam)[1]);
   }
   return true;
}

LRESULT CSaveCurveAsWave::OnHelp(WPARAM wParam, LPARAM lParam)
{
   HELPINFO *pHI  = (HELPINFO*) lParam;

   AfxGetApp()->WinHelp(HID_BASE_RESOURCE+IDD_SAVE_WAVE_CURVE);

   return true;
}

HBRUSH CSaveCurveAsWave::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
   int nID = pWnd->GetDlgCtrlID(); 
   if (nID == IDC_WD_CURVE1_LEVEL)
   {
      if (m_dCurve1Level >= 100.0) pDC->SetTextColor(RGB(255,0,0));
   }
   if (nID == IDC_WD_CURVE2_LEVEL)
   {
      if (m_dCurve2Level >= 100.0) pDC->SetTextColor(RGB(255,0,0));
   }
	
	// TODO: Anderen Pinsel zurückgeben, falls Standard nicht verwendet werden soll
	return hbr;
}
