// CSkinsDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "skinsdemo.h"
#include "CSkinsDemoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSkinsDemoDlg dialog

/////////////////////////////////////////////////////////////////////////////
CSkinsDemoDlg::CSkinsDemoDlg(CWnd* pParent /*=NULL*/)
	: CTransparentDialog(CSkinsDemoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSkinsDemoDlg)
	//}}AFX_DATA_INIT
	m_nMode			= 0;
	m_nCamNr		= 1;
	m_nBrightness	= 50;
	m_nContrast		= 50;
	m_nSaturation	= 50;
	m_nDayOfWeek	= So;
	m_sPin			= "";
	m_sDate			= "";
	m_sTime			= "";
	m_eInputState	= InputPIN;
	m_sTimer[So]	= "00002359";
	m_sTimer[Mo]	= "00002359";
	m_sTimer[Di]	= "00002359";
	m_sTimer[Mi]	= "00002359";
	m_sTimer[Do]	= "00002359";
	m_sTimer[Fr]	= "00002359";
	m_sTimer[Sa]	= "00002359";
	m_sDayName[So]	= "So";
	m_sDayName[Mo]	= "Mo";
	m_sDayName[Di]	= "Di";
	m_sDayName[Mi]	= "Mi";
	m_sDayName[Do]	= "Do";
	m_sDayName[Fr]	= "Fr";
	m_sDayName[Sa]	= "Sa";
	m_nViewMode		= View1x1;
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CTransparentDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSkinsDemoDlg)
	DDX_Control(pDX, IDC_CHECK1, m_Check1);
	DDX_Control(pDX, IDC_RADIO1, m_Radio1);
	DDX_Control(pDX, IDC_RADIO2, m_Radio2);
	DDX_Control(pDX, IDC_SLIDER1, m_ctrlSlider1);
	DDX_Control(pDX, IDC_BUTTON_VIEW_LOWER, m_ctrlButtonViewLower);
	DDX_Control(pDX, IDC_BUTTON_VIEW_HIGHER, m_ctrlButtonViewHigher);
	DDX_Control(pDX, IDC_BUTTON_STATIC_VIEW, m_ctrlStaticView);
	DDX_Control(pDX, IDC_BUTTON_DAYOFWEEK_LOWER, m_ctrlButtonDowLower);
	DDX_Control(pDX, IDC_BUTTON_DAYOFWEEK_HIGHER, m_ctrlButtonDowHigher);
	DDX_Control(pDX, IDC_BUTTON_STATIC_TIMER, m_ctrlStaticTimer);
	DDX_Control(pDX, IDC_BUTTON_ASTERIX, m_ctrlButtonAsterix);
	DDX_Control(pDX, IDC_BUTTON_SEARCH, m_ctrlButtonSearch);
	DDX_Control(pDX, IDC_BUTTON_CLEAR, m_ctrlButtonClear);
	DDX_Control(pDX, IDC_BUTTON_SATURATION_HIGHER, m_ctrlButtonSaturationHigher);
	DDX_Control(pDX, IDC_BUTTON_SATURATION_LOWER, m_ctrlButtonSaturationLower);
	DDX_Control(pDX, IDC_BUTTON_STATIC_SATURATION, m_ctrlStaticSaturation);
	DDX_Control(pDX, IDC_BUTTON_STATIC_BRIGHTNESS, m_ctrlStaticBright);
	DDX_Control(pDX, IDC_BUTTON_BRIGHTNESS_HIGHER, m_ctrlButtonBrightHigher);
	DDX_Control(pDX, IDC_BUTTON_BRIGHTNESS_LOWER, m_ctrlButtonBrightLower);
	DDX_Control(pDX, IDC_BUTTON_STATIC_CONTRAST, m_ctrlStaticContrast);
	DDX_Control(pDX, IDC_BUTTON_CONTRAST_LOWER, m_ctrlButtonContrastLower);
	DDX_Control(pDX, IDC_BUTTON_CONTRAST_HIGHER, m_ctrlButtonContrastHigher);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_ctrlButtonStop);
	DDX_Control(pDX, IDC_BUTTON_REWIND, m_ctrlButtonRewind);
	DDX_Control(pDX, IDC_BUTTON_PLAY, m_ctrlButtonPlay);
	DDX_Control(pDX, IDC_BUTTON_PAUSE, m_ctrlButtonPause);
	DDX_Control(pDX, IDC_BUTTON_FF, m_ctrlButtonFF);
	DDX_Control(pDX, IDC_BUTTON_CAM_LOWER, m_ctrlButtonCamLower);
	DDX_Control(pDX, IDC_BUTTON_CAM_HIGHER, m_ctrlButtonCamHigher);
	DDX_Control(pDX, IDC_BUTTON_MODE, m_ctrlButtonMode);
	DDX_Control(pDX, IDC_BUTTON_OK, m_ctrlButtonOk);
	DDX_Control(pDX, IDC_BUTTON_NUM9, m_ctrlButtonNum9);
	DDX_Control(pDX, IDC_BUTTON_NUM8, m_ctrlButtonNum8);
	DDX_Control(pDX, IDC_BUTTON_NUM7, m_ctrlButtonNum7);
	DDX_Control(pDX, IDC_BUTTON_NUM6, m_ctrlButtonNum6);
	DDX_Control(pDX, IDC_BUTTON_NUM5, m_ctrlButtonNum5);
	DDX_Control(pDX, IDC_BUTTON_NUM4, m_ctrlButtonNum4);
	DDX_Control(pDX, IDC_BUTTON_NUM3, m_ctrlButtonNum3);
	DDX_Control(pDX, IDC_BUTTON_NUM2, m_ctrlButtonNum2);
	DDX_Control(pDX, IDC_BUTTON_NUM1, m_ctrlButtonNum1);
	DDX_Control(pDX, IDC_BUTTON_NUM0, m_ctrlButtonNum0);
	DDX_Control(pDX, IDC_DISPLAY_EMPTY2, m_ctrlDisplayEmpty2);
	DDX_Control(pDX, IDC_DISPLAY_EMPTY1, m_ctrlDisplayEmpty1);
	DDX_Control(pDX, IDC_DISPLAY_CAMERA, m_ctrlDisplayCamera);
	DDX_Control(pDX, IDC_DISPLAY_ALARM, m_ctrlDisplayAlarm);
	DDX_Control(pDX, IDC_DISPLAY_VIEWMODE, m_ctrlDisplayViewMode);
	DDX_Control(pDX, IDC_DISPLAY_DATE, m_ctrlDisplayDate);
	DDX_Control(pDX, IDC_DISPLAY_TIME, m_ctrlDisplayTime);
	DDX_Control(pDX, IDC_DISPLAY_MODE, m_ctrlDisplayMode);
	DDX_Control(pDX, IDC_BORDER1, m_ctrlBorder1);
	DDX_Control(pDX, IDC_BUTTON_STATIC_CAM, m_ctrlStaticCamera);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSkinsDemoDlg, CTransparentDialog)
	//{{AFX_MSG_MAP(CSkinsDemoDlg)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnButtonCancel)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_MODE, OnButtonMode)
	ON_BN_CLICKED(IDC_BUTTON_CAM_HIGHER, OnButtonCamHigher)
	ON_BN_CLICKED(IDC_BUTTON_CAM_LOWER, OnButtonCamLower)
	ON_BN_CLICKED(IDC_BUTTON_NUM1, OnButtonNum1)
	ON_BN_CLICKED(IDC_BUTTON_NUM2, OnButtonNum2)
	ON_BN_CLICKED(IDC_BUTTON_NUM3, OnButtonNum3)
	ON_BN_CLICKED(IDC_BUTTON_NUM4, OnButtonNum4)
	ON_BN_CLICKED(IDC_BUTTON_NUM5, OnButtonNum5)
	ON_BN_CLICKED(IDC_BUTTON_NUM6, OnButtonNum6)
	ON_BN_CLICKED(IDC_BUTTON_NUM7, OnButtonNum7)
	ON_BN_CLICKED(IDC_BUTTON_NUM8, OnButtonNum8)
	ON_BN_CLICKED(IDC_BUTTON_NUM9, OnButtonNum9)
	ON_BN_CLICKED(IDC_BUTTON_FF, OnButtonFF)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, OnButtonPause)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, OnButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_REWIND, OnButtonRewind)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_CONTRAST_LOWER, OnButtonContrastLower)
	ON_BN_CLICKED(IDC_BUTTON_CONTRAST_HIGHER, OnButtonContrastHigher)
	ON_BN_CLICKED(IDC_BUTTON_BRIGHTNESS_LOWER, OnButtonBrightnessLower)
	ON_BN_CLICKED(IDC_BUTTON_BRIGHTNESS_HIGHER, OnButtonBrightnessHigher)
	ON_BN_CLICKED(IDC_BUTTON_SATURATION_LOWER, OnButtonSaturationLower)
	ON_BN_CLICKED(IDC_BUTTON_SATURATION_HIGHER, OnButtonSaturationHigher)
	ON_BN_CLICKED(IDC_BUTTON_NUM0, OnButtonNum0)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, OnButtonSearch)
	ON_BN_CLICKED(IDC_BUTTON_ASTERIX, OnButtonAsterix)
	ON_BN_CLICKED(IDC_BUTTON_DAYOFWEEK_HIGHER, OnButtonDayofweekHigher)
	ON_BN_CLICKED(IDC_BUTTON_DAYOFWEEK_LOWER, OnButtonDayofweekLower)
	ON_BN_CLICKED(IDC_BUTTON_VIEW_HIGHER, OnButtonViewHigher)
	ON_BN_CLICKED(IDC_BUTTON_VIEW_LOWER, OnButtonViewLower)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER1, OnReleasedCaptureSlider1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, OnCustomDrawSlider1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkinsDemoDlg message handlers

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonOk() 
{
	CTransparentDialog::OnOK();	
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonCancel() 
{
	CTransparentDialog::OnCancel();	
}											   
/////////////////////////////////////////////////////////////////////////////
BOOL CSkinsDemoDlg::OnInitDialog() 
{
	CTransparentDialog::OnInitDialog();
	
	OpenStockTransparent(BackGround1);

	//------------------------------------------------------------
	m_ctrlSlider1.OpenStockSlider(this, SliderH1, TRUE);

	//------------------------------------------------------------
	m_ctrlButtonViewLower.OpenStockBitmapButton(this, ButtonDown3);
	m_ctrlButtonViewHigher.OpenStockBitmapButton(this, ButtonUp3);
	m_ctrlStaticView.OpenStockDisplay(this,		StaticGrey);
	
	//------------------------------------------------------------
	m_ctrlButtonDowLower.OpenStockBitmapButton(this, ButtonDown3);				    
	m_ctrlButtonDowHigher.OpenStockBitmapButton(this, ButtonUp3);				    
	m_ctrlStaticTimer.OpenStockDisplay(this,		StaticGrey);				    
	m_ctrlStaticTimer.SetTextAllignment(DT_RIGHT|DT_VCENTER|DT_SINGLELINE);

	//------------------------------------------------------------
	m_ctrlButtonSearch.OpenStockBitmapButton(this, ButtonSearch1); 

	//------------------------------------------------------------
	m_ctrlButtonSaturationLower.OpenStockBitmapButton(this,	ButtonLeft1);
	m_ctrlButtonSaturationHigher.OpenStockBitmapButton(this,	ButtonRight1);
	m_ctrlStaticSaturation.OpenStockDisplay(this,		StaticGrey);
	m_ctrlStaticSaturation.SetTextAllignment(DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	
	//------------------------------------------------------------
	m_ctrlButtonBrightLower.OpenStockBitmapButton(this,	ButtonLeft1);
	m_ctrlButtonBrightHigher.OpenStockBitmapButton(this,	ButtonRight1);
	m_ctrlStaticBright.OpenStockDisplay(this,		StaticGrey);
	m_ctrlStaticBright.SetTextAllignment(DT_RIGHT|DT_VCENTER|DT_SINGLELINE);

	//------------------------------------------------------------
	m_ctrlButtonContrastLower.OpenStockBitmapButton(this,		ButtonLeft1);
	m_ctrlButtonContrastHigher.OpenStockBitmapButton(this,	ButtonRight1);
	m_ctrlStaticContrast.OpenStockDisplay(this,			StaticGrey);
	m_ctrlStaticContrast.SetTextAllignment(DT_RIGHT|DT_VCENTER|DT_SINGLELINE);

	//------------------------------------------------------------
	m_ctrlButtonCamLower.OpenStockBitmapButton(this,	ButtonLeft1);
	m_ctrlButtonCamHigher.OpenStockBitmapButton(this,	ButtonRight1);
	m_ctrlStaticCamera.OpenStockDisplay(this, StaticGrey);
	m_ctrlStaticCamera.SetTextAllignment(DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	m_ctrlStaticCamera.EnableDebugTrace();

	//------------------------------------------------------------
	m_ctrlButtonRewind.OpenStockBitmapButton(this,	ButtonRewind1);
	m_ctrlButtonPlay.OpenStockBitmapButton(this,		ButtonPlay1);
	m_ctrlButtonPause.OpenStockBitmapButton(this,		ButtonPause1);
	m_ctrlButtonStop.OpenStockBitmapButton(this,		ButtonStop1);
	m_ctrlButtonFF.OpenStockBitmapButton(this,		ButtonFF1);

	//------------------------------------------------------------
	m_ctrlBorder1.CreateFrame(this);
	m_ctrlBorder1.EnableShadow(TRUE);
	
	//------------------------------------------------------------
	m_ctrlButtonOk.OpenStockBitmapButton(this,		ButtonRect2, TRUE);
	m_ctrlButtonOk.EnableRelief(TRUE);

	//------------------------------------------------------------
	m_ctrlButtonMode.OpenStockBitmapButton(this, ButtonRound2);
	m_ctrlButtonMode.SetTextOffset(CPoint(2,2));

	//------------------------------------------------------------
	m_NumButtons.Add(&m_ctrlButtonNum0);
	m_NumButtons.Add(&m_ctrlButtonNum1);
	m_NumButtons.Add(&m_ctrlButtonNum2);
	m_NumButtons.Add(&m_ctrlButtonNum3);
	m_NumButtons.Add(&m_ctrlButtonNum4);
	m_NumButtons.Add(&m_ctrlButtonNum5);
	m_NumButtons.Add(&m_ctrlButtonNum6);
	m_NumButtons.Add(&m_ctrlButtonNum7);
	m_NumButtons.Add(&m_ctrlButtonNum8);
	m_NumButtons.Add(&m_ctrlButtonNum9);
	m_NumButtons.Add(&m_ctrlButtonClear);
	m_NumButtons.Add(&m_ctrlButtonAsterix);
#if (1)
	m_NumButtons.OpenStockBitmapButton(this, ButtonRound1);
#else
	m_NumButtons.OpenStockBitmapButton(this, ButtonRect3);
#endif
	m_NumButtons.EnableRelief(TRUE);

	//------------------------------------------------------------
	m_ctrlDisplayMode.OpenStockDisplay(this, Display1);
	m_ctrlDisplayEmpty1.OpenStockDisplay(this, Display1);
	m_ctrlDisplayCamera.OpenStockDisplay(this, Display2);
	m_ctrlDisplayTime.OpenStockDisplay(this, Display2);
	m_ctrlDisplayDate.OpenStockDisplay(this, Display2);
	m_ctrlDisplayAlarm.OpenStockDisplay(this, Display2);
	m_ctrlDisplayEmpty2.OpenStockDisplay(this, Display2);
	m_ctrlDisplayViewMode.OpenStockDisplay(this, Display2);

	SetTimer(2, 1000, NULL);

	OnButtonNum("");

	SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	
	// Make this window 70% alpha
	SetLayeredWindowAttributes(m_hWnd, 0, (255 * 70) / 100, LWA_ALPHA);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSkinsDemoDlg::DestroyWindow() 
{
	KillTimer(2);
	
	return CTransparentDialog::DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnTimer(UINT nIDEvent) 
{
	CTime time = CTime::GetCurrentTime();

	CString sTime = time.Format("%H:%M:%S");
	CString sDate = time.Format("%d.%m.%y");
	m_ctrlDisplayTime.SetWindowText(sTime);
	m_ctrlDisplayTime.InvalidateRect(NULL);
	m_ctrlDisplayTime.UpdateWindow();
	m_ctrlDisplayDate.SetDisplayText(sDate);
	
	CTransparentDialog::OnTimer(nIDEvent);

	int nRand = rand()*100 / RAND_MAX;
	if (nRand >= 90)
	{
		CString sText;
		sText.Format("Alarm %d", nRand & 0x0f);
		m_ctrlDisplayAlarm.SetDisplayText(sText);
	}
	if ((nRand >= 80) && (nRand < 90))
	{
		m_ctrlDisplayAlarm.SetDisplayText("");
	}

}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonMode() 
{
	CString sMode[] = {"Live",
					   "Playback"};
	
	m_nMode++;
	m_nMode %= 2;

	m_ctrlButtonRewind.EnableWindow((m_nMode == 1));
	m_ctrlButtonPlay.EnableWindow((m_nMode == 1));
	m_ctrlButtonPause.EnableWindow((m_nMode == 1));
	m_ctrlButtonStop.EnableWindow((m_nMode == 1));
	m_ctrlButtonFF.EnableWindow((m_nMode == 1));
	m_ctrlButtonSearch.EnableWindow((m_nMode == 1));

	m_ctrlDisplayMode.SetDisplayText(sMode[m_nMode], SCROLL_TEXT);
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonCamHigher() 
{
	m_nCamNr++;
	if (m_nCamNr > MAX_CAM)
		m_nCamNr = 1;
	CString sCam;
	sCam.Format("Camera %02d", m_nCamNr);
	m_ctrlDisplayCamera.SetDisplayText(sCam);
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonCamLower() 
{
	m_nCamNr--;
	if (m_nCamNr <= 0)
		m_nCamNr = MAX_CAM;
	CString sCam;
	sCam.Format("Camera %02d", m_nCamNr);

	m_ctrlDisplayCamera.SetDisplayText(sCam);
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonClear() 
{
	switch (m_eInputState)
	{
		case InputPIN:
			m_sPin = "";
			OnButtonNum("");
			break;
		case InputDate:
			m_sDate = "";
			OnButtonNum("");
			break;
		case InputTime:
			m_sTime = "";
			OnButtonNum("");
			break;
		case InputTimer:
			m_sTimer[m_nDayOfWeek] = "";
			OnButtonNum("");
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonNum0() 
{
	OnButtonNum("0");
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonNum1() 
{
	OnButtonNum("1");
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonNum2() 
{
	OnButtonNum("2");
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonNum3() 
{
	OnButtonNum("3");
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonNum4() 
{
	OnButtonNum("4");
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonNum5() 
{
	OnButtonNum("5");
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonNum6() 
{
	OnButtonNum("6");
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonNum7() 
{
	OnButtonNum("7");
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonNum8() 
{
	OnButtonNum("8");
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonNum9() 
{
	OnButtonNum("9");
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonAsterix() 
{
	OnButtonNum("*");
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonNum(const CString &s)
{
	CString sInput = s;

	// Eingabe der Pin
	if (m_eInputState == InputPIN)
	{
		m_sPin += sInput;
		sInput	= "";
		if (m_ctrlDisplayEmpty1.SetFormatDisplayText(CString('*',m_sPin.GetLength()), "Pin: ____") == 4)
		{
			if (m_sPin == "4711")
			{
				m_eInputState = InputNo;
				m_ctrlDisplayEmpty1.SetDisplayText("Success");
				Sleep(1000);
				m_ctrlDisplayEmpty1.SetDisplayText("");
			}
			else
			{
				m_ctrlDisplayEmpty1.SetDisplayText("Error");
				Sleep(1000);
				m_sPin = "";
				m_ctrlDisplayEmpty1.SetDisplayText("Pin: ____");
			}
		}
	}

	// Eingabe des Suchdatums
	if (m_eInputState == InputDate)
	{
		m_sDate += sInput;
		sInput	= "";
		if (m_ctrlDisplayEmpty1.SetFormatDisplayText(m_sDate, "Date: __.__.__", 10) == 6)
		{
			m_eInputState	= InputTime;
			Sleep(1000);
			m_ctrlDisplayEmpty1.SetDisplayText("");
		}
	}

	// Eingabe der Suchzeit
	if (m_eInputState == InputTime)
	{
		m_sTime += sInput;
		sInput	= "";
		if (m_ctrlDisplayEmpty1.SetFormatDisplayText(m_sTime, "Time: __:__:__", 10) == 6)
		{
			m_eInputState	= InputNo;
			Sleep(1000);
			m_ctrlDisplayEmpty1.SetDisplayText("");
		}
	}

	// Eingabe des Wochenkalenders
	if (m_eInputState == InputTimer)
	{
		m_sTimer[m_nDayOfWeek] += sInput;
		sInput	= "";
		m_ctrlDisplayEmpty1.SetFormatDisplayText(m_sTimer[m_nDayOfWeek], m_sDayName[m_nDayOfWeek] + ": __:__-__:__", 10);
	}
	// Eingabe der Kameranummer
	if (m_eInputState == InputNo)
	{
		int nNum = atoi(sInput);
		if ((nNum >= 1) && (nNum <=MAX_CAM))
		{
			m_nCamNr = nNum;
			CString sCam;
			sCam.Format("Camera %02d", m_nCamNr);
			m_ctrlDisplayCamera.SetDisplayText(sCam);
		}
	}
}				 
		   
/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonFF() 
{
	m_ctrlDisplayEmpty2.SetDisplayText("Forwarding");
	m_ctrlButtonPlay.SetState(FALSE);
	m_ctrlButtonRewind.SetState(FALSE);
	m_ctrlButtonFF.SetState(TRUE);
	m_ctrlButtonPause.SetState(FALSE);
	m_ctrlButtonStop.SetState(FALSE);

}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonPause() 
{
	m_ctrlDisplayEmpty2.SetDisplayText("Paused");
	m_ctrlButtonPlay.SetState(FALSE);
	m_ctrlButtonRewind.SetState(FALSE);
	m_ctrlButtonFF.SetState(FALSE);
	m_ctrlButtonPause.SetState(TRUE);
	m_ctrlButtonStop.SetState(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonPlay() 
{					
	m_ctrlDisplayEmpty2.SetDisplayText("Playing");
	m_ctrlButtonPlay.SetState(TRUE);
	m_ctrlButtonRewind.SetState(FALSE);
	m_ctrlButtonFF.SetState(FALSE);
	m_ctrlButtonPause.SetState(FALSE);
	m_ctrlButtonStop.SetState(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonRewind() 
{
	m_ctrlDisplayEmpty2.SetDisplayText("Rewinding");
	m_ctrlButtonPlay.SetState(FALSE);
	m_ctrlButtonRewind.SetState(TRUE);
	m_ctrlButtonFF.SetState(FALSE);
	m_ctrlButtonPause.SetState(FALSE);
	m_ctrlButtonStop.SetState(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonStop() 
{
	m_ctrlDisplayEmpty2.SetDisplayText("Stoped", 0, 5);
	m_ctrlButtonPlay.SetState(FALSE);
	m_ctrlButtonRewind.SetState(FALSE);
	m_ctrlButtonFF.SetState(FALSE);
	m_ctrlButtonPause.SetState(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonContrastLower() 
{
	m_nContrast--;
	m_nContrast = max(m_nContrast, 0);

	CString sText;
	sText.Format("Contr:%d%%", m_nContrast);

	m_ctrlDisplayEmpty2.SetDisplayText(sText,0 , 3);
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonContrastHigher() 
{
	m_nContrast++;
	m_nContrast = min(m_nContrast, 100);

	CString sText;
	sText.Format("Contr:%d%%", m_nContrast);

	m_ctrlDisplayEmpty2.SetDisplayText(sText,0 , 3);
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonBrightnessLower() 
{
	m_nBrightness--;
	m_nBrightness = max(m_nBrightness, 0);

	CString sText;
	sText.Format("Bright:%d%%", m_nBrightness);

	m_ctrlDisplayEmpty2.SetDisplayText(sText,0, 3);
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonBrightnessHigher() 
{
	m_nBrightness++;
	m_nBrightness = min(m_nBrightness, 99);

	CString sText;
	sText.Format("Bright:%d%%", m_nBrightness);

	m_ctrlDisplayEmpty2.SetDisplayText(sText,0, 3);
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonSaturationLower() 
{
	m_nSaturation--;
	m_nSaturation = max(m_nSaturation, 0);

	CString sText;
	sText.Format("Satur:%d%%", m_nSaturation);

	m_ctrlDisplayEmpty2.SetDisplayText(sText,0, 3);
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonSaturationHigher() 
{
	m_nSaturation++;
	m_nSaturation = min(m_nSaturation, 99);

	CString sText;
	sText.Format("Satur:%d%%", m_nSaturation);

	m_ctrlDisplayEmpty2.SetDisplayText(sText,0, 3);
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonSearch() 
{
	m_sTime = "";
	m_sDate = "";

	m_eInputState = InputDate;
	OnButtonNum("");
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonDayofweekHigher() 
{
	m_eInputState = InputTimer;
	m_nDayOfWeek++;
	if (m_nDayOfWeek > So)
		m_nDayOfWeek = Mo;
	OnButtonNum("");
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonDayofweekLower() 
{
	m_eInputState = InputTimer;
	m_nDayOfWeek--;
	if (m_nDayOfWeek < Mo)
		m_nDayOfWeek = So;
	OnButtonNum("");
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonViewHigher() 
{
	m_nViewMode++;
	if (m_nViewMode > View4x4)
		m_nViewMode = View1x1;

	switch (m_nViewMode)
	{
		case View1x1:
			m_ctrlDisplayViewMode.SetDisplayText("View: 1x1");
			break;
		case View2x2:
			m_ctrlDisplayViewMode.SetDisplayText("View: 2x2");
			break;
		case View3x3:
			m_ctrlDisplayViewMode.SetDisplayText("View: 3x3");
			break;
		case View4x4:
			m_ctrlDisplayViewMode.SetDisplayText("View: 4x4");
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnButtonViewLower() 
{
	m_nViewMode--;
	if (m_nViewMode < View1x1)
		m_nViewMode = View4x4;

	switch (m_nViewMode)
	{
		case View1x1:
			m_ctrlDisplayViewMode.SetDisplayText("View: 1x1");
			break;
		case View2x2:
			m_ctrlDisplayViewMode.SetDisplayText("View: 2x2");
			break;
		case View3x3:
			m_ctrlDisplayViewMode.SetDisplayText("View: 3x3");
			break;
		case View4x4:
			m_ctrlDisplayViewMode.SetDisplayText("View: 4x4");
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CSkinsDemoDlg::OnReleasedCaptureSlider1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CString sText;
	sText.Format("%d%%", m_ctrlSlider1.GetPos()); 
	m_ctrlDisplayEmpty2.SetDisplayText(sText,0, 3);
	
	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////
// Diese Message kommt leider nicht!!?
void CSkinsDemoDlg::OnCustomDrawSlider1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CString sText;
	sText.Format("%d%%", m_ctrlSlider1.GetPos()); 
	m_ctrlDisplayEmpty2.SetDisplayText(sText,0, 3);

	*pResult = 0;
}
