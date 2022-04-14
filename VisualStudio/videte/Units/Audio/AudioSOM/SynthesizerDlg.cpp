// SynthesizerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AudioSOM.h"
#include "AudioSOMDlg.h"
#include "SynthesizerDlg.h"
#include ".\synthesizerdlg.h"


// CSynthesizerDlg dialog

IMPLEMENT_DYNAMIC(CSynthesizerDlg, CDialog)
CSynthesizerDlg::CSynthesizerDlg(CWnd* pParent /*=NULL*/)
   : CDialog((UINT)CSynthesizerDlg::IDD, (CWnd*)pParent)
{
   m_nMaxFrequency = 22050;
   m_nWaveForm   = theApp.GetProfileInt(SECTION_SYNTHESIZER, SYNTH_WAVEFORM, 0);
   m_bCorrelated = theApp.GetProfileInt(SECTION_SYNTHESIZER, SYNTH_CORRELATED, 0);
   m_nFrequency1 = theApp.GetProfileInt(SECTION_SYNTHESIZER, SYNTH_FREQUENCY1, 1000);
   m_nFrequency2 = theApp.GetProfileInt(SECTION_SYNTHESIZER, SYNTH_FREQUENCY2, 1000);
   m_nTime       = theApp.GetProfileInt(SECTION_SYNTHESIZER, SYNTH_TIME, 50);
   m_nPhase      = theApp.GetProfileInt(SECTION_SYNTHESIZER, SYNTH_PHASE, 0);
   m_nMagnitude1 = theApp.GetProfileInt(SECTION_SYNTHESIZER, SYNTH_MAGNITUDE1, 90);
   m_nMagnitude2 = theApp.GetProfileInt(SECTION_SYNTHESIZER, SYNTH_MAGNITUDE2, 90);
   m_nOffset1    = theApp.GetProfileInt(SECTION_SYNTHESIZER, SYNTH_OFFSET1, 0);
   m_nOffset2    = theApp.GetProfileInt(SECTION_SYNTHESIZER, SYNTH_OFFSET2, 0);
}

CSynthesizerDlg::~CSynthesizerDlg()
{
}

void CSynthesizerDlg::DDX_SliderMinMaxInt(CDataExchange* pDX, UINT nIDedt, UINT nIDslider, int &nValue)
{
   int nMin, nMax;
   ((CSliderCtrl*)GetDlgItem(nIDslider))->GetRange(nMin, nMax);
   DDX_Text10(pDX, nIDedt, nValue);
   DDV_MinMaxInt(pDX, nValue, nMin, nMax);
   if (pDX->m_bSaveAndValidate == 2)
   {
      pDX->m_bSaveAndValidate = FALSE;
   }
   DDX_Slider(pDX, nIDslider, nValue);
}

void CSynthesizerDlg::DDX_Text10(CDataExchange* pDX, UINT nIDedt, int &nValue)
{
   switch (nIDedt)
   {
      case IDC_EDT_MAGNITUDE1: case IDC_EDT_MAGNITUDE2:
      case IDC_EDT_OFFSET1:    case IDC_EDT_OFFSET2:
      {
         double dvalue = nValue*0.1;
         DDX_Text(pDX, nIDedt, dvalue);
         nValue = (int)(dvalue * 10.0 + 0.5);
      }  break;
      default:
         DDX_Text(pDX, nIDedt, nValue);
         break;
   }
}
void CSynthesizerDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Radio(pDX, IDC_BTN_WF_SINE, m_nWaveForm);
   DDX_Check(pDX, IDC_BTN_WF_KORR, m_bCorrelated);
   DDX_SliderMinMaxInt(pDX, IDC_EDT_FREQUENCY1, IDC_SLIDER_FREQUENCY1, m_nFrequency1);
   DDX_SliderMinMaxInt(pDX, IDC_EDT_FREQUENCY2, IDC_SLIDER_FREQUENCY2, m_nFrequency2);
   DDX_SliderMinMaxInt(pDX, IDC_EDT_TIME      , IDC_SLIDER_TIME      , m_nTime);
   DDX_SliderMinMaxInt(pDX, IDC_EDT_PHASE     , IDC_SLIDER_PHASE     , m_nPhase);
   DDX_SliderMinMaxInt(pDX, IDC_EDT_MAGNITUDE1, IDC_SLIDER_MAGNITUDE1, m_nMagnitude1);
   DDX_SliderMinMaxInt(pDX, IDC_EDT_MAGNITUDE2, IDC_SLIDER_MAGNITUDE2, m_nMagnitude2);
   DDX_SliderMinMaxInt(pDX, IDC_EDT_OFFSET1   , IDC_SLIDER_OFFSET1   , m_nOffset1);
   DDX_SliderMinMaxInt(pDX, IDC_EDT_OFFSET2   , IDC_SLIDER_OFFSET2   , m_nOffset2);
}


BEGIN_MESSAGE_MAP(CSynthesizerDlg, CDialog)
	//{{AFX_MSG_MAP(CSynthesizerDlg)
   ON_BN_CLICKED(IDC_BTN_WF_KORR, OnBnClickedBtnWfKorr)
   ON_BN_CLICKED(IDC_BTN_WF_SINE, OnBnClickedBtnWaveForm)
   ON_BN_CLICKED(IDC_BTN_WF_RECT, OnBnClickedBtnWaveForm)
   ON_BN_CLICKED(IDC_BTN_WF_TRIANGLE, OnBnClickedBtnWaveForm)
   ON_BN_CLICKED(IDC_BTN_WF_PINK_NOISE, OnBnClickedBtnWaveForm)
   ON_BN_CLICKED(IDC_BTN_WF_WHITE_NOISE, OnBnClickedBtnWaveForm)
   ON_BN_CLICKED(IDC_BTN_WF_SWEEP, OnBnClickedBtnWaveForm)
   ON_WM_HSCROLL()
   ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDT_FREQUENCY1, IDC_EDT_OFFSET2, OnEnKillFocusEdt)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNotify)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNotify)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CSynthesizerDlg message handlers

void CSynthesizerDlg::OnCancel()
{
   DestroyWindow();
}

void CSynthesizerDlg::OnOK()
{
}

BOOL CSynthesizerDlg::OnInitDialog()
{
   SendDlgItemMessage(IDC_SLIDER_FREQUENCY1, TBM_SETRANGE, 0, MAKELONG(1, m_nMaxFrequency));
   SendDlgItemMessage(IDC_SLIDER_FREQUENCY2, TBM_SETRANGE, 0, MAKELONG(1, m_nMaxFrequency));
   SendDlgItemMessage(IDC_SLIDER_PHASE    , TBM_SETRANGE, 0,  MAKELONG(0, 360));

   SendDlgItemMessage(IDC_SLIDER_MAGNITUDE1, TBM_SETRANGE, 0,  MAKELONG(0, 1000));
   SendDlgItemMessage(IDC_SLIDER_MAGNITUDE2, TBM_SETRANGE, 0,  MAKELONG(0, 1000));
   SendDlgItemMessage(IDC_SLIDER_OFFSET1, TBM_SETRANGE, 0,  MAKELONG(-1000, 1000));
   SendDlgItemMessage(IDC_SLIDER_OFFSET2, TBM_SETRANGE, 0,  MAKELONG(-1000, 1000));

   CDialog::OnInitDialog();
   if (HIMAGELIST(m_Images) == NULL)
   {
      m_Images.Create(IDB_IMAGES, 35, 1, RGB(255, 0, 255));
   }

   ((CButton*)GetDlgItem(IDC_BTN_WF_SINE))->SetIcon(m_Images.ExtractIcon(0));
   ((CButton*)GetDlgItem(IDC_BTN_WF_RECT))->SetIcon(m_Images.ExtractIcon(1));
   ((CButton*)GetDlgItem(IDC_BTN_WF_TRIANGLE))->SetIcon(m_Images.ExtractIcon(2));

   UpdateCtrls();

   EnableToolTips(true);
   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CSynthesizerDlg::OnBnClickedBtnWfKorr()
{
   CDataExchange dx(this, TRUE);
   DDX_Check(&dx, IDC_BTN_WF_KORR, m_bCorrelated);
   UpdateSynthesizer(IDC_BTN_WF_KORR);
}

void CSynthesizerDlg::OnBnClickedBtnWaveForm()
{
   CDataExchange dx(this, TRUE);
   DDX_Radio(&dx, IDC_BTN_WF_SINE, m_nWaveForm);
   UpdateCtrls();
   UpdateSynthesizer(IDC_BTN_WF_SINE);
}

void CSynthesizerDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
   UINT nIDEdit = 0;
   int *pnVar  = NULL;
   CSliderCtrl*pSlider = (CSliderCtrl*)pScrollBar;
   switch(pScrollBar->GetDlgCtrlID())
   { 
      case IDC_SLIDER_FREQUENCY1: nIDEdit = IDC_EDT_FREQUENCY1; pnVar = &m_nFrequency1; break;
      case IDC_SLIDER_FREQUENCY2: nIDEdit = IDC_EDT_FREQUENCY2; pnVar = &m_nFrequency2; break;
      case IDC_SLIDER_TIME:       nIDEdit = IDC_EDT_TIME;       pnVar = &m_nTime;       break;
      case IDC_SLIDER_PHASE:      nIDEdit = IDC_EDT_PHASE;      pnVar = &m_nPhase;      break;
      case IDC_SLIDER_MAGNITUDE1: nIDEdit = IDC_EDT_MAGNITUDE1; pnVar = &m_nMagnitude1; break;
      case IDC_SLIDER_MAGNITUDE2: nIDEdit = IDC_EDT_MAGNITUDE2; pnVar = &m_nMagnitude2; break;
      case IDC_SLIDER_OFFSET1:    nIDEdit = IDC_EDT_OFFSET1;    pnVar = &m_nOffset1;    break;
      case IDC_SLIDER_OFFSET2:    nIDEdit = IDC_EDT_OFFSET2;    pnVar = &m_nOffset2;    break;
      default: pSlider = NULL; break;
   }
   if (pnVar && pSlider)
   {
      if (nSBCode == SB_THUMBPOSITION || nSBCode == SB_THUMBTRACK)
      {
         *pnVar = nPos;
         CDataExchange dx(this, FALSE);
         DDX_Text10(&dx, nIDEdit, *pnVar);
         //SetDlgItemInt(nIDEdit, *pnVar);
      }
      else if (nSBCode == SB_ENDSCROLL)
      {
         *pnVar = pSlider->GetPos();
         UpdateSynthesizer(nIDEdit);
      }
      else
      {
         *pnVar = pSlider->GetPos();
         CDataExchange dx(this, FALSE);
         DDX_Text10(&dx, nIDEdit, *pnVar);
         //SetDlgItemInt(nIDEdit, *pnVar);
      }
   }
   CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSynthesizerDlg::OnEnKillFocusEdt(UINT nID)
{
   UINT nIDSlider= 0;
   int *pnVar  = NULL;
   switch(nID)
   { 
      case IDC_EDT_FREQUENCY1: nIDSlider = IDC_SLIDER_FREQUENCY1; pnVar = &m_nFrequency1; break;
      case IDC_EDT_FREQUENCY2: nIDSlider = IDC_SLIDER_FREQUENCY2; pnVar = &m_nFrequency2; break;
      case IDC_EDT_TIME:       nIDSlider = IDC_SLIDER_TIME;       pnVar = &m_nTime;       break;
      case IDC_EDT_PHASE:      nIDSlider = IDC_SLIDER_PHASE;      pnVar = &m_nPhase;      break;
      case IDC_EDT_MAGNITUDE1: nIDSlider = IDC_SLIDER_MAGNITUDE1; pnVar = &m_nMagnitude1; break;
      case IDC_EDT_MAGNITUDE2: nIDSlider = IDC_SLIDER_MAGNITUDE2; pnVar = &m_nMagnitude2; break;
      case IDC_EDT_OFFSET1:    nIDSlider = IDC_SLIDER_OFFSET1;    pnVar = &m_nOffset1;    break;
      case IDC_EDT_OFFSET2:    nIDSlider = IDC_SLIDER_OFFSET2;    pnVar = &m_nOffset2;    break;
   }
   if (pnVar)
   {
      TRY
      {
         CDataExchange dx(this, 2);
         DDX_SliderMinMaxInt(&dx, nID, nIDSlider, *pnVar);
      }
      CATCH(CUserException, e)
      {
      }
      AND_CATCH_ALL(e)
      {
         e->ReportError(MB_ICONEXCLAMATION, AFX_IDP_INTERNAL_FAILURE);
         delete (e);
      }
      END_CATCH_ALL
      UpdateSynthesizer(nID);
   }
}

void CSynthesizerDlg::SetParentWnd(CWnd*pParent)
{
   m_pParentWnd = pParent;
}

void CSynthesizerDlg::UpdateSynthesizer(UINT nID)
{
   if (m_pParentWnd)
   {
      ((CAudioSOMDlg*)m_pParentWnd)->UpdateSynthesizer(nID);
   }
}

void CSynthesizerDlg::UpdateCtrls()
{
   BOOL bFrq1 = TRUE;
   BOOL bFrq2 = FALSE;
   BOOL bCorrelation = FALSE;
   int  nShowTime  = SW_SHOW;
   int  nShowPhase = SW_HIDE;

   switch(m_nWaveForm)
   {
      case WAVEFORM_SINE:
         nShowTime  = SW_HIDE;
         nShowPhase = SW_SHOW;
         break;
      case WAVEFORM_RECT:
         break;
      case WAVEFORM_TRIANGLE:
         break;
      case WAVEFORM_SWEEP:
         nShowTime  = SW_HIDE;
         bFrq2 = TRUE;
         break;
      case WAVEFORM_PINK_NOISE:
         nShowTime  = SW_HIDE;
         bCorrelation = TRUE;
         bFrq1 = FALSE;
         break;
      case WAVEFORM_WHITE_NOISE:
         nShowTime  = SW_HIDE;
         bCorrelation = TRUE;
         bFrq1 = FALSE;
         break;
   }

   GetDlgItem(IDC_SLIDER_FREQUENCY1)->EnableWindow(bFrq1);
   GetDlgItem(IDC_EDT_FREQUENCY1)->EnableWindow(bFrq1);

   GetDlgItem(IDC_SLIDER_FREQUENCY2)->EnableWindow(bFrq2);
   GetDlgItem(IDC_EDT_FREQUENCY2)->EnableWindow(bFrq2);

   GetDlgItem(IDC_SLIDER_TIME)->ShowWindow(nShowTime);
   GetDlgItem(IDC_TXT_TIME)->ShowWindow(nShowTime);
   GetDlgItem(IDC_EDT_TIME)->ShowWindow(nShowTime);

   GetDlgItem(IDC_SLIDER_PHASE)->ShowWindow(nShowPhase);
   GetDlgItem(IDC_TXT_PHASE)->ShowWindow(nShowPhase);
   GetDlgItem(IDC_EDT_PHASE)->ShowWindow(nShowPhase);

   GetDlgItem(IDC_BTN_WF_KORR)->EnableWindow(bCorrelation);
}

BOOL CSynthesizerDlg::OnToolTipNotify(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
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
      switch(nID)
      {
         case IDC_EDT_FREQUENCY1: nID = IDC_SLIDER_FREQUENCY1;
         case IDC_EDT_FREQUENCY2: nID = IDC_SLIDER_FREQUENCY2;
         case IDC_EDT_TIME:       nID = IDC_SLIDER_TIME;
         case IDC_EDT_PHASE:      nID = IDC_SLIDER_PHASE;
         case IDC_EDT_MAGNITUDE1: nID = IDC_SLIDER_MAGNITUDE1;
         case IDC_EDT_MAGNITUDE2: nID = IDC_SLIDER_MAGNITUDE2;
         case IDC_EDT_OFFSET1:    nID = IDC_SLIDER_OFFSET1;
         case IDC_EDT_OFFSET2:    nID = IDC_SLIDER_OFFSET2;
      }
      pTTT->lpszText = MAKEINTRESOURCE(nID);
      pTTT->hinst = AfxGetResourceHandle();
      return(TRUE);
   }

   _tcsncpy(pTTT->szText, strTipText, sizeof(pTTT->szText));

   *pResult = 0;

   return TRUE;    // message was handled
}

