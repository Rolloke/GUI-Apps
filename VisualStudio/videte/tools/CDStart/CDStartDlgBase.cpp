// CDStartDlgBase.cpp : implementation file
//

#include "stdafx.h"
#include "CDStart.h"
#include "CDStartDlgBase.h"
#include "XCopyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////
static const TCHAR szSectionControl[]				= _T("CDStartControl");
static const TCHAR szEntrySkinTest[]				= _T("SkinTest");
static const TCHAR szEntryAutoRun[]					= _T("AutoRun");
static const TCHAR szEntryWidth[]					= _T("Width");
static const TCHAR szEntryHeight[]					= _T("Height");

static const TCHAR szSectionBackground[]			= _T("CDStartBackground");
static const TCHAR szEntryBackgroundStyle[]			= _T("BackgroundStyle");
static const TCHAR szEntryBackgroundColor[]			= _T("BackgroundColor");

static const TCHAR szSectionTitle[]					= _T("CDStartTitle");
static const TCHAR szEntryTitleColor[]				= _T("TitleColor");
static const TCHAR szEntryTitleColorShadow[]		= _T("TitleColorShadow");
static const TCHAR szEntryTitleFontName[]			= _T("TitleFontName");
static const TCHAR szEntryTitleFontHeight[]			= _T("TitleFontHeight");

static const TCHAR szSectionButtons[]				= _T("CDStartButtons");
static const TCHAR szEntryButtonShape[]				= _T("ButtonShape");
static const TCHAR szEntryButtonSurface[]			= _T("ButtonSurface");
static const TCHAR szEntryButtonColor[]				= _T("ButtonColor");
static const TCHAR szEntryButtonColorText[]			= _T("ButtonColorText");
static const TCHAR szEntryButtonColorTextHighlight[]= _T("ButtonColorTextHighlight");
static const TCHAR szEntryButtonFontName[]			= _T("ButtonFontName");
static const TCHAR szEntryButtonFontHeight[]		= _T("ButtonFontHeight");

static const TCHAR szSectionDisplays[]				= _T("CDStartDisplays");
static const TCHAR szEntryDisplayColor[]			= _T("DisplayColor");
static const TCHAR szEntryDisplayColorShadow[]		= _T("DisplayColorShadow");
static const TCHAR szEntryDisplayColorBackGround[]	= _T("DisplayColorBackGround");
static const TCHAR szEntryDisplayFontName[]			= _T("DisplayFontName");
static const TCHAR szEntryDisplayFontHeight[]		= _T("DisplayFontHeight");

static const TCHAR szSectionPrograms[]				= _T("CDStartPrograms");

/////////////////////////////////////////////////////////////////////////////
// CCDStartDlgBase dialog
CCDStartDlgBase::CCDStartDlgBase(CWnd* pParent /*=NULL*/)
	: CTransparentDialog(IDD, pParent)
{
	InitConstructorRest();
	// Nothing else here!!!
}
/////////////////////////////////////////////////////////////////////////////
// CCDStartDlgBase dialog
CCDStartDlgBase::CCDStartDlgBase(UINT uIDD, CWnd* pParent /*=NULL*/)
	: CTransparentDialog(uIDD, pParent)
{
	InitConstructorRest();
	// Nothing else here!!!
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::InitConstructorRest()
{
	//{{AFX_DATA_INIT(CCDStartDlgBase)
	m_sSize = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon				= AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bSkinTest			= FALSE;
	m_bAutoRun			= FALSE;
	m_iDisplayShownLast = 0;
	m_iWindowWidth		= 0;	// uebernimmt Dialogwerte, wenn nicht vorgegeben
	m_iWindowHeight		= 0;	// uebernimmt Dialogwerte, wenn nicht vorgegeben
	// m_sIniFile;
	// m_sActualDrive;
	// m_sActualDir;
	// m_Buttons;
	// m_Displays;

	// Background
	m_eStyleBackground	= StyleBackGroundColorChangeBrushed;
	m_BaseColor		= RGB(150,150,255);
	m_dMaxColorChangeRange	= 128;

	// Title
	m_colTitle				= RGB(250,250,250);
	m_colTitleShadow		= SKIN_COLOR_UNDEFINED;
	m_sTitleFontName		= szFontArialBlack;
	m_iTitleFontHeight		= 30;
	m_sTitle				= _T("Titel der CD");

	// Buttons
	// m_fontButtons;
	m_eButtonShape			= ShapeRect;
	m_eButtonSurface		= SurfaceColorChangeBrushed;
	m_colButton				= SKIN_COLOR_GOLD_METALLIC;
	m_colButtonText			= RGB(0,0,0);
	m_colButtonTextHighlight= RGB(0,0,0);
	m_sButtonFontName		= szFontMSSansSerif;
	m_iButtonFontHeight		= 8;
	m_sButton1				= _T("Test &button color");
	m_sButton2				= _T("Test button text &normal color");
	m_sButton3				= _T("Test button text &highlight color");
	m_sButton4				= _T("Test &display text color");
	m_sButton5				= _T("Test Back&ground color");

	// Displays
	m_colDisplay			= RGB(250,250,250);
	m_colDisplayShadow		= SKIN_COLOR_UNDEFINED;
	m_colDisplayBackGround	= SKIN_COLOR_UNDEFINED;
	m_sDisplayFontName		= szFontArial;
	m_iDisplayFontHeight	= 14;


	m_sDisplay1 = _T("Hier wird Display 1 angezeigt");
	m_sDisplay2 = _T("Hier wird Display 2 angezeigt");
	m_sDisplay3 = _T("Hier wird Display 3 angezeigt");
	m_sDisplay4 = _T("Hier wird Display 4 angezeigt");
	m_sDisplay5 = _T("Hier wird Display 5 angezeigt");

	m_sProgram1 = _T("");
	m_sProgram2 = _T("");
	m_sProgram3 = _T("");
	m_sProgram4 = _T("");
	m_sProgram5 = _T("");

	CreateIniFileName();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCDStartDlgBase::StretchElements()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::InitDlgControls()
{
#if 0
	// Enumerate all installed fonts and save them in an Stringarray
	CDC* pDC = GetDC();
	if (pDC)
	{
		LOGFONT Logfont;
		memset(Logfont.lfFaceName, NULL, 32);

//		Logfont.lfHeight		=;	// LONG
//		Logfont.lfWidth			=;	// LONG
//		Logfont.lfEscapement	=;	// LONG
//		Logfont.lfOrientation	=;	// LONG
//		Logfont.lfWeight		=;	// LONG
//		Logfont.lfItalic		=;	// BYTE
//		Logfont.lfUnderline		=;	// BYTE
//		Logfont.lfStrikeOut		=;	// BYTE
		Logfont.lfCharSet		= DEFAULT_CHARSET;	// BYTE
//		Logfont.lfOutPrecision	=;	// BYTE
//		Logfont.lfClipPrecision	=;	// BYTE
//		Logfont.lfQuality		=;	// BYTE
		Logfont.lfPitchAndFamily= 0;	// BYTE
		Logfont.lfFaceName[0]	= _T('\0');	// TCHAR

		m_saFontNames.RemoveAll();
		EnumFontFamiliesEx(pDC->m_hDC,				// handle to device context
							&Logfont,				// pointer to logical font information
							(FONTENUMPROC)MyEnumFontFamExProc,	// pointer to callback function
							(LPARAM)&m_saFontNames,	// application-supplied data
							0						// reserved; must be zero
		);
	}
#endif
	ReadIniFile();

	CollectAllButtons();
	CollectAllDisplays();

	InitWindowSize();
	InitBackground();
	InitTitle();
	InitButtons();
	InitDisplays();
	InitCustomColors();
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::ReadIniFile()
{
	CString sLangAbbreviation = _T("EN");
	CString sEntry;

	// check for supported language
	CString sBuffer;
	int iSize = 10;
	int iChars = GetLocaleInfo(LOCALE_SYSTEM_DEFAULT,
							   LOCALE_SABBREVLANGNAME,
							   sBuffer.GetBuffer(iSize),
							   iSize
							   );
	sBuffer.ReleaseBuffer();
	if (iChars != 0)
	{
		CString sAbbreviation = sBuffer.Left(2);
		sEntry = _T("Title_") + sAbbreviation;
		CString sTextNotFound = _T("_Text_not_found_");
		CString sTest = GetIniString(szSectionTitle, sEntry, sTextNotFound);
		if (sTest != sTextNotFound)
		{
			sLangAbbreviation = sAbbreviation;
		}
	}
	if (GetIniInt(szSectionControl, _T("ShowLocalLanguage"), 0))
	{
		CString sDebug;
		sDebug.Format(_T("Language %s %s"), sBuffer, sLangAbbreviation);
		AfxMessageBox(sDebug);
	}

	m_bSkinTest		= (BOOL)GetIniInt(szSectionControl, szEntrySkinTest, m_bSkinTest);
	m_iWindowWidth	= GetIniInt(szSectionControl, szEntryWidth, m_iWindowWidth);
	m_iWindowHeight	= GetIniInt(szSectionControl, szEntryHeight, m_iWindowHeight);

	// Background
	m_eStyleBackground	= GetBackgroundStyle(GetIniInt(szSectionBackground, szEntryBackgroundStyle, m_eStyleBackground));
	m_BaseColor		= GetColorRef(GetIniString(szSectionBackground, szEntryBackgroundColor, _T("")), m_BaseColor);

	// Title
	m_colTitle			= GetColorRef(GetIniString(szSectionTitle, szEntryTitleColor, _T("")), m_colTitle);
	m_colTitleShadow	= GetColorRef(GetIniString(szSectionTitle, szEntryTitleColorShadow, _T("")), m_colTitleShadow);
	m_iTitleFontHeight	= GetIniInt(szSectionTitle, szEntryTitleFontHeight, m_iTitleFontHeight);
	m_sTitleFontName	= GetValidFontName(GetIniString(szSectionTitle, szEntryTitleFontName, m_sTitleFontName), m_sTitleFontName);
	sEntry  = _T("Title_") + sLangAbbreviation;
	m_sTitle			= GetIniString(szSectionTitle, sEntry, m_sTitle);

	// Buttons
	m_eButtonShape			= GetButtonShape(GetIniInt(szSectionButtons, szEntryButtonShape, m_eButtonShape));
	m_eButtonSurface		= GetButtonSurface(GetIniInt(szSectionButtons, szEntryButtonSurface, m_eButtonSurface));
	m_colButton				= GetColorRef(GetIniString(szSectionButtons, szEntryButtonColor, _T("")), m_colButton);
	m_colButtonText			= GetColorRef(GetIniString(szSectionButtons, szEntryButtonColorText, _T("")), m_colButtonText);
	m_colButtonTextHighlight= GetColorRef(GetIniString(szSectionButtons, szEntryButtonColorTextHighlight, _T("")), m_colButtonTextHighlight);
	m_sButtonFontName		= GetValidFontName(GetIniString(szSectionButtons, szEntryButtonFontName, m_sButtonFontName), m_sButtonFontName);
	m_iButtonFontHeight		= GetIniInt(szSectionButtons, szEntryButtonFontHeight, m_iButtonFontHeight);
	sEntry		= _T("1_") + sLangAbbreviation;
	m_sButton1	= GetIniString(szSectionButtons, sEntry, m_sButton1);
	sEntry		= _T("2_") + sLangAbbreviation;
	m_sButton2	= GetIniString(szSectionButtons, sEntry, m_sButton2);
	sEntry		= _T("3_") + sLangAbbreviation;
	m_sButton3	= GetIniString(szSectionButtons, sEntry, m_sButton3);
	sEntry		= _T("4_") + sLangAbbreviation;
	m_sButton4	= GetIniString(szSectionButtons, sEntry, m_sButton4);
	sEntry		= _T("5_") + sLangAbbreviation;
	m_sButton5	= GetIniString(szSectionButtons, sEntry, m_sButton5);

	// Displays
	m_colDisplay			= GetColorRef(GetIniString(szSectionDisplays, szEntryDisplayColor, _T("")), m_colDisplay);
	m_colDisplayShadow		= GetColorRef(GetIniString(szSectionDisplays, szEntryDisplayColorShadow, _T("")), m_colDisplayShadow);
	m_colDisplayBackGround	= GetColorRef(GetIniString(szSectionDisplays, szEntryDisplayColorBackGround, _T("")), m_colDisplayBackGround);
	m_sDisplayFontName		= GetValidFontName(GetIniString(szSectionDisplays, szEntryDisplayFontName, m_sDisplayFontName), m_sDisplayFontName);
	m_iDisplayFontHeight	= GetIniInt(szSectionDisplays, szEntryDisplayFontHeight, m_iDisplayFontHeight);
	sEntry		= _T("1_") + sLangAbbreviation;
	m_sDisplay1	= GetIniString(szSectionDisplays, sEntry, m_sDisplay1);
	sEntry		= _T("2_") + sLangAbbreviation;
	m_sDisplay2	= GetIniString(szSectionDisplays, sEntry, m_sDisplay2);
	sEntry		= _T("3_") + sLangAbbreviation;
	m_sDisplay3	= GetIniString(szSectionDisplays, sEntry, m_sDisplay3);
	sEntry		= _T("4_") + sLangAbbreviation;
	m_sDisplay4	= GetIniString(szSectionDisplays, sEntry, m_sDisplay4);
	sEntry		= _T("5_") + sLangAbbreviation;
	m_sDisplay5	= GetIniString(szSectionDisplays, sEntry, m_sDisplay5);

	// Programs
	sEntry		= _T("1_") + sLangAbbreviation;
	m_sProgram1 = GetIniString(szSectionPrograms, sEntry, m_sProgram1);
	sEntry		= _T("2_") + sLangAbbreviation;
	m_sProgram2 = GetIniString(szSectionPrograms, sEntry, m_sProgram2);
	sEntry		= _T("3_") + sLangAbbreviation;
	m_sProgram3 = GetIniString(szSectionPrograms, sEntry, m_sProgram3);
	sEntry		= _T("4_") + sLangAbbreviation;
	m_sProgram4 = GetIniString(szSectionPrograms, sEntry, m_sProgram4);
	sEntry		= _T("5_") + sLangAbbreviation;
	m_sProgram5 = GetIniString(szSectionPrograms, sEntry, m_sProgram5);
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::InitBackground()
{
	CreateTransparent(m_eStyleBackground, m_BaseColor);
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::InitTitle()
{
	m_DisplayTitle.EnableTransparency(TRUE);
	m_DisplayTitle.EnableShadow(TRUE);
	m_DisplayTitle.SetDisplayText(m_sTitle);
	m_DisplayTitle.ShowWindow(m_sTitle.IsEmpty()?SW_HIDE:SW_SHOW);
	if (!m_sTitle.IsEmpty())
	{
		SetWindowText(m_sTitle);
	}

	CFont font;
	font.CreatePointFont(m_iTitleFontHeight*10, m_sTitleFontName);
	m_DisplayTitle.SetFont(font);

	DSPCOL DisplayColors;
	DisplayColors.dwFlags = DSP_COL_TEXT;
	if (SKIN_COLOR_UNDEFINED != m_colTitleShadow)
	{
		DisplayColors.dwFlags |= DSP_COL_SHADOW;
		DisplayColors.colShadow = m_colTitleShadow;
	}
	else
	{
		DisplayColors.dwFlags = DSP_COL_TEXT;
	}
	DisplayColors.colText = m_colTitle;
	m_DisplayTitle.SetDisplayColors(DisplayColors);
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::InitButtons()
{
	m_fontButtons.CreatePointFont(m_iButtonFontHeight*10, m_sButtonFontName);
	m_Buttons.SetTextFont(&m_fontButtons);
	m_Buttons.SetShape(m_eButtonShape);
	m_Buttons.SetSurface(SurfaceColorChangeBrushed);
	m_Buttons.SetBaseColor(SKIN_COLOR_GOLD_METALLIC);
	m_Buttons.SetTextColorNormal(m_colButtonText);
	m_Buttons.SetTextColorHighlighted(m_colButtonTextHighlight);
	m_Buttons.SetBrushEffectFactor(20);
	m_Buttons.EnableChangeOnlyBrightness();
	m_Buttons.EnableDebugTrace();

	m_btn1.SetWindowText(m_sButton1);
	m_btn1.ShowWindow(m_sButton1.IsEmpty()?SW_HIDE:SW_SHOW);

	m_btn2.SetWindowText(m_sButton2);
	m_btn2.ShowWindow(m_sButton2.IsEmpty()?SW_HIDE:SW_SHOW);

	m_btn3.SetWindowText(m_sButton3);
	m_btn3.ShowWindow(m_sButton3.IsEmpty()?SW_HIDE:SW_SHOW);

	m_btn4.SetWindowText(m_sButton4);
	m_btn4.ShowWindow(m_sButton4.IsEmpty()?SW_HIDE:SW_SHOW);

	m_btn5.SetWindowText(m_sButton5);
	m_btn5.ShowWindow(m_sButton5.IsEmpty()?SW_HIDE:SW_SHOW);

	if (m_bSkinTest)
	{
		m_btn1.SetShape(ShapeRect);
		m_btn1.SetSurface(SurfaceColorChangeBrushed);
//		m_btn1.SetBaseColor(SKIN_COLOR_GOLD_METALLIC);
		m_btn1.EnableDebugTrace();

		m_btn2.SetShape(ShapeRect);
		m_btn2.SetSurface(SurfaceColorChangeSimple);
//		m_btn2.SetBaseColor(SKIN_COLOR_GOLD_METALLIC);
//		m_btn1.EnableDebugTrace();

		m_btn3.SetShape(ShapeRect);
		m_btn3.SetSurface(Surface3DPlaneEdge10);
//		m_btn3.SetBaseColor(SKIN_COLOR_GOLD_METALLIC);
//		m_btn1.EnableDebugTrace();

		m_btn4.SetShape(ShapeRect);
		m_btn4.SetSurface(Surface3DPlaneEdge10);
//		m_btn4.SetBaseColor(SKIN_COLOR_GOLD_METALLIC);
//		m_btn1.EnableDebugTrace();

		m_btn5.SetShape(ShapeRound);
		m_btn5.SetSurface(SurfaceColorChangeSimple);
//		m_btn5.SetBaseColor(SKIN_COLOR_GOLD_METALLIC);
//		m_btn1.EnableDebugTrace();

		RemoveFromResize(m_btnTest1.GetDlgCtrlID());
		m_btnTest1.SetShape(ShapeRect);
		m_btnTest1.SetSurface(SurfaceColorChangeSimple);
//		m_btnTest1.SetBaseColor(SKIN_COLOR_GOLD_METALLIC);
//		m_btnTest1.EnableDebugTrace();

		RemoveFromResize(m_btnTest2.GetDlgCtrlID());
		m_btnTest2.SetShape(ShapeRound);
		m_btnTest2.SetSurface(SurfaceColorChangeSimple);
//		m_btnTest2.SetBaseColor(SKIN_COLOR_GOLD_METALLIC);
//		m_btnTest2.EnableDebugTrace();

		CRect rc1, rc2;
		m_btnTest1.GetWindowRect(rc1);
		m_btnTest2.GetWindowRect(rc2);
		ScreenToClient(rc1);
		ScreenToClient(rc2);
		
		rc1.left = rc1.right - rc1.Height();
		rc1.left = rc1.right - rc1.Height();
		m_btnTest1.MoveWindow(rc1, TRUE);
		m_btnTest2.MoveWindow(rc2, TRUE);
		m_sSize.Format(_T("%i"), rc1.Height());
		m_txtSize.SetWindowText(m_sSize);
	}
	else
	{
		m_btnTest1.ShowWindow(SW_HIDE);
		m_btnTest2.ShowWindow(SW_HIDE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::InitDisplays()
{
	m_Display1.OpenStockDisplay(this, Display2);
	m_Display2.OpenStockDisplay(this, Display2);
	m_Display3.OpenStockDisplay(this, Display2);
	m_Display4.OpenStockDisplay(this, Display2);
	m_Display5.OpenStockDisplay(this, Display2);

//	m_Display1.EnableShadow(TRUE);
	m_Display1.SetDisplayText(_T(""));
	m_Display1.ShowWindow(m_sDisplay1.IsEmpty()?SW_HIDE:SW_SHOW);

//	m_Display2.EnableShadow(TRUE);
	m_Display2.SetDisplayText(_T(""));
	m_Display2.ShowWindow(m_sDisplay2.IsEmpty()?SW_HIDE:SW_SHOW);

//	m_Display3.EnableShadow(TRUE);
	m_Display3.SetDisplayText(_T(""));
	m_Display3.ShowWindow(m_sDisplay3.IsEmpty()?SW_HIDE:SW_SHOW);

//	m_Display4.EnableShadow(TRUE);
	m_Display4.SetDisplayText(_T(""));
	m_Display4.ShowWindow(m_sDisplay4.IsEmpty()?SW_HIDE:SW_SHOW);

//	m_Display5.EnableShadow(TRUE);
	m_Display5.SetDisplayText(_T(""));
	m_Display5.ShowWindow(m_sDisplay5.IsEmpty()?SW_HIDE:SW_SHOW);

	m_Displays.EnableShadow(FALSE);
	m_Displays.EnableTransparency(TRUE);

	DSPCOL DisplayColors;
	DisplayColors.dwFlags = DSP_COL_TEXT;
	DisplayColors.colText = m_colDisplay;

	if (SKIN_COLOR_UNDEFINED != m_colDisplayShadow)
	{
		DisplayColors.dwFlags |= DSP_COL_SHADOW;
		DisplayColors.colShadow = m_colDisplayShadow;
		m_Displays.EnableShadow(TRUE);
	}
	if (SKIN_COLOR_UNDEFINED != m_colDisplayBackGround)
	{
		DisplayColors.dwFlags |= DSP_COL_BACKGROUNND;
		DisplayColors.colBackGround = m_colDisplayBackGround;
		m_Displays.EnableTransparency(FALSE);
	}
	m_Displays.SetColors(DisplayColors);

	m_Displays.SetTextAllignment(DT_TOP|DT_LEFT|DT_WORDBREAK);

	CFont font;
	font.CreatePointFont(m_iDisplayFontHeight*10, m_sDisplayFontName);
	m_Displays.SetFont(font);
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::InitCustomColors()
{
	m_CustomColorRefs[0] = SKIN_COLOR_GOLD_METALLIC;
	m_CustomColorRefs[1] = SKIN_COLOR_GOLD_METALLIC_LIGHT;
	m_CustomColorRefs[2] = SKIN_COLOR_SILVER_METALLIC;
	m_CustomColorRefs[3] = (COLORREF)(RGB(196,196,196));
	m_CustomColorRefs[4] = (COLORREF)(RGB(224,224,224));
	m_CustomColorRefs[5] = SKIN_COLOR_WHITE;
	m_CustomColorRefs[6] = SKIN_COLOR_WHITE;
	m_CustomColorRefs[7] = SKIN_COLOR_KEY;
	m_CustomColorRefs[8] = SKIN_COLOR_VIDETE_CI;
	m_CustomColorRefs[9] = SKIN_COLOR_WHITE;
	m_CustomColorRefs[10] = (COLORREF)(RGB(16,48,96));
	m_CustomColorRefs[11] = (COLORREF)(RGB(24,48,96));
	m_CustomColorRefs[12] = (COLORREF)(RGB(32,56,104));
	m_CustomColorRefs[13] = (COLORREF)(RGB(32,64,128));
	m_CustomColorRefs[14] = SKIN_COLOR_WHITE;
	m_CustomColorRefs[15] = SKIN_COLOR_WHITE;
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::InitWindowSize()
{
	CRect rect;
	GetWindowRect(rect);
	int iWindowWidth	= rect.Width();
	int iWindowHeight	= rect.Height();
	BOOL bResize = FALSE;
	if (m_iWindowWidth != 0)
	{
		iWindowWidth = m_iWindowWidth;
		bResize = TRUE;
	}
	if (m_iWindowHeight != 0)
	{
		iWindowHeight = m_iWindowHeight;
		bResize = TRUE;
	}
	if (bResize)
	{
		SetWindowPos(&wndTop, -1, -1, iWindowWidth, iWindowHeight, SWP_NOMOVE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::Init()
{
	CWK_StopWatch watch;
	CString sMsg;
	sMsg =_T("Init completed");
	watch.StopWatch(sMsg);
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::CreateIniFileName()
{
	//////////////////////////////////////////////////////////////////
	// Pfad der INI-Datei aus dem Applikations-Pfad ableiten
	CString sAppFullPath;
	int nLen = GetModuleFileName(AfxGetApp()->m_hInstance,
									sAppFullPath.GetBuffer(_MAX_PATH),
									_MAX_PATH);
	sAppFullPath.ReleaseBuffer(nLen);

	int iIndex = sAppFullPath.Find(_T(':'));
	if (-1 != iIndex)
	{
		m_sActualDrive = sAppFullPath.Left(iIndex+1);
	}
	
	iIndex = sAppFullPath.ReverseFind(_T('\\'));
	if (-1 != iIndex)
	{
		m_sActualDir = sAppFullPath.Left(iIndex);
	}

	iIndex = sAppFullPath.ReverseFind(_T('.'));
	if (-1 != iIndex)
	{
		m_sIniFile = sAppFullPath.Left(iIndex) + _T(".inf");
		CFile file;
		if (file.Open(m_sIniFile, CFile::modeRead, NULL))
		{
			file.Close();
		}
		else
		{
			m_sIniFile = m_sActualDir + _T("\\Autorun.inf");
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::ShowDisplay(int iDisplay)
{
	if (m_iDisplayShownLast != iDisplay)
	{
		HideLastDisplay();
		switch (iDisplay)
		{
			case 0:
				// nothing to show
				break;
			case 1:
				m_Display1.SetDisplayText(m_sDisplay1);
				break;
			case 2:
				m_Display2.SetDisplayText(m_sDisplay2);
				break;
			case 3:
				m_Display3.SetDisplayText(m_sDisplay3);
				break;
			case 4:
				m_Display4.SetDisplayText(m_sDisplay4);
				break;
			case 5:
				m_Display5.SetDisplayText(m_sDisplay5);
				break;
			default:
				TRACE(_T("HideLastDisplay WRONG Display %i\n"), iDisplay);
				break;
		}
		m_iDisplayShownLast = iDisplay;
	}
	else
	{
		// nothing to do
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::HideLastDisplay()
{
	CRect rect;
	switch (m_iDisplayShownLast)
	{
		case 0:
			// nothing to hide
			break;
		case 1:
			m_Display1.SetDisplayText(_T(""));
			m_Display1.GetWindowRect(rect);
			ScreenToClient(rect);
			InvalidateRect(rect);
			break;
		case 2:
			m_Display2.SetDisplayText(_T(""));
			m_Display2.GetWindowRect(rect);
			ScreenToClient(rect);
			InvalidateRect(rect);
			break;
		case 3:
			m_Display3.SetDisplayText(_T(""));
			m_Display3.GetWindowRect(rect);
			ScreenToClient(rect);
			InvalidateRect(rect);
			break;
		case 4:
			m_Display4.SetDisplayText(_T(""));
			m_Display4.GetWindowRect(rect);
			ScreenToClient(rect);
			InvalidateRect(rect);
			break;
		case 5:
			m_Display5.SetDisplayText(_T(""));
			m_Display5.GetWindowRect(rect);
			ScreenToClient(rect);
			InvalidateRect(rect);
			break;
		default:
			TRACE(_T("HideLastDisplay WRONG Display %i\n"), m_iDisplayShownLast);
			break;
	}
	UpdateWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::ActionButton1()
{
	if (m_bSkinTest)
	{
		DWORD dwFlags = CC_FULLOPEN;
		CColorDialog dlg(m_colButton, dwFlags, this);
		dlg.m_cc.lpCustColors = &m_CustomColorRefs[0];
		if (IDOK == dlg.DoModal())
		{
			COLORREF col = dlg.GetColor();
			TRACE(_T("color %08x\n"), col);
			m_Buttons.SetBaseColor(col);
		}
	}
	else
	{
		if (IsCopy(m_sProgram1))
		{
			BOOL bRecursiv = IsRecursivCopy(m_sProgram1);
			CString sSource, sDest;
			if (SplitSourceAndDest(m_sProgram1, sSource, sDest))
			{
				StartCopy(sSource, sDest, bRecursiv);
			}
		}
		else if (0==m_sProgram1.CompareNoCase(_T("EXIT")))
		{
			OnCancel();
		}
		else 
		{
			StartProgram(GetProgramPathFromEntry(m_sProgram1));
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::ActionButton2()
{
	if (m_bSkinTest)
	{
		DWORD dwFlags = CC_FULLOPEN;
		CColorDialog dlg(m_colButtonText, dwFlags, this);
		dlg.m_cc.lpCustColors = &m_CustomColorRefs[0];
		if (IDOK == dlg.DoModal())
		{
			COLORREF col = dlg.GetColor();
			TRACE(_T("color %08x\n"), col);
			m_Buttons.SetTextColorNormal(col);
		}
	}
	else
	{
		if (IsCopy(m_sProgram2))
		{
			BOOL bRecursiv = IsRecursivCopy(m_sProgram2);
			CString sSource, sDest;
			if (SplitSourceAndDest(m_sProgram2, sSource, sDest))
			{
				StartCopy(sSource, sDest, bRecursiv);
			}
		}
		else if (0==m_sProgram2.CompareNoCase(_T("EXIT")))
		{
			OnCancel();
		}
		else 
		{
			StartProgram(GetProgramPathFromEntry(m_sProgram2));
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::ActionButton3()
{
	if (m_bSkinTest)
	{
		DWORD dwFlags = CC_FULLOPEN;
		CColorDialog dlg(m_colButtonTextHighlight, dwFlags, this);
		dlg.m_cc.lpCustColors = &m_CustomColorRefs[0];
		if (IDOK == dlg.DoModal())
		{
			COLORREF col = dlg.GetColor();
			TRACE(_T("color %08x\n"), col);
			m_Buttons.SetTextColorHighlighted(col);
		}
	}
	else
	{
		if (IsCopy(m_sProgram3))
		{
			BOOL bRecursiv = IsRecursivCopy(m_sProgram3);
			CString sSource, sDest;
			if (SplitSourceAndDest(m_sProgram3, sSource, sDest))
			{
				StartCopy(sSource, sDest, bRecursiv);
			}
		}
		else if (0==m_sProgram3.CompareNoCase(_T("EXIT")))
		{
			OnCancel();
		}
		else 
		{
			StartProgram(GetProgramPathFromEntry(m_sProgram3));
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::ActionButton4()
{
	if (m_bSkinTest)
	{
		DWORD dwFlags = CC_FULLOPEN;
		CColorDialog dlg(m_colDisplay, dwFlags, this);
		dlg.m_cc.lpCustColors = &m_CustomColorRefs[0];
		if (IDOK == dlg.DoModal())
		{
			COLORREF col = dlg.GetColor();
			TRACE(_T("color %08x\n"), col);
			DSPCOL DisplayColors;
			DisplayColors.dwFlags = DSP_COL_TEXT;
			DisplayColors.colText = col;
			m_Displays.SetColors(DisplayColors);
		}
	}
	else
	{
		if (IsCopy(m_sProgram4))
		{
			BOOL bRecursiv = IsRecursivCopy(m_sProgram4);
			CString sSource, sDest;
			if (SplitSourceAndDest(m_sProgram4, sSource, sDest))
			{
				StartCopy(GetProgramPathFromEntry(sSource),
							GetProgramPathFromEntry(sDest),
							bRecursiv);
			}
		}
		else if (0==m_sProgram4.CompareNoCase(_T("EXIT")))
		{
			OnCancel();
		}
		else 
		{
			StartProgram(GetProgramPathFromEntry(m_sProgram4));
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::ActionButton5()
{
	if (m_bSkinTest)
	{
		DWORD dwFlags = CC_FULLOPEN;
		CColorDialog dlg(m_BaseColor, dwFlags, this);
		dlg.m_cc.lpCustColors = &m_CustomColorRefs[0];
		if (IDOK == dlg.DoModal())
		{
			m_BaseColor = dlg.GetColor();
			TRACE(_T("color %08x\n"), m_BaseColor);
			SetBaseColor(m_BaseColor);
/*			DSPCOL DisplayColors;
			DisplayColors.colBackGround = m_BaseColor;
			DisplayColors.dwFlags = DSP_COL_BACKGROUNND;
			m_Displays.SetColors(DisplayColors);
*/		}
	}
	else
	{
		if (IsCopy(m_sProgram5))
		{
			BOOL bRecursiv = IsRecursivCopy(m_sProgram5);
			CString sSource, sDest;
			if (SplitSourceAndDest(m_sProgram5, sSource, sDest))
			{
				StartCopy(sSource, sDest, bRecursiv);
			}
		}
		else if (0==m_sProgram5.CompareNoCase(_T("EXIT")))
		{
			OnCancel();
		}
		else 
		{
			StartProgram(GetProgramPathFromEntry(m_sProgram5));
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::StartCopy(const CString& sSource, const CString& sDest, BOOL bRecursiv)
{
	BOOL bAskForDest = FALSE;
	CString sDestination = sDest;
	if (bAskForDest)
	{
		CString sResultPath, sResultFilename;
		WK_SplitPath(sDestination, sResultPath, sResultFilename);
		if (sResultFilename == _T(""))
		{
			sDestination += _T("*.*");
		}
		CFileDialog dlg(FALSE, NULL, sDestination, OFN_READONLY|OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, this);
//		dlg.HideControl(id);
		if (dlg.DoModal() == IDOK)
		{
			CString sDestPath = dlg.GetPathName();
		}
	}
//	CXCopyDlg dlg(sSource, sDestination, bRecursiv, FALSE, this);
//	dlg.DoModal();
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::AutoRun()
{
	// Starte alle Programme
	// Beginne mit dem Holen aller Schluessel
	CStringArray sAllKeys;
	if (GetAllSectionEntries(szSectionPrograms, sAllKeys))
	{
		CString sKey, sEntry, sProgram;
		// solange noch ein weiterer Schluessel da ist
		for (int i=0 ; i<sAllKeys.GetSize() ;i++)
		{
			sKey = sAllKeys.GetAt(i);
			// hole den Eintrag des Schluessels
			sEntry = GetOneEntry(sKey);
			if(FALSE == sEntry.IsEmpty())
			{
				// pruefe auf Sonderzeichenfolgen (z.B. ..\ und .\) und ersetze sie
				sProgram = GetProgramPathFromEntry(sEntry);
				// starte das Programm
				StartProgram(sProgram);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
CString CCDStartDlgBase::GetAllKeys()
{
	return GetIniString(szSectionPrograms, NULL, _T(""));
}
/////////////////////////////////////////////////////////////////////////////
CString CCDStartDlgBase::GetOneEntry(CString& sKey)
{
	return GetIniString(szSectionPrograms, sKey, _T(""));
}
/////////////////////////////////////////////////////////////////////////////
CString CCDStartDlgBase::GetProgramPathFromEntry(CString& sEntry)
{
	CWK_String sProgram = sEntry;
	if (sProgram.Find(_T("..\\")) != -1)
	{
		sProgram = sProgram.SimpleReplace(_T("..\\"), m_sActualDrive+_T("\\"));
	}
	else if (sProgram.Find(_T(".\\")) != -1)
	{
		sProgram = sProgram.SimpleReplace(_T(".\\"), (m_sActualDir+_T("\\")));
	}
	return sProgram;
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::StartProgram(CString sProgram)
{
	if (sProgram.IsEmpty())
	{
		TRACE(_T("NO Program\n"));
	}
#if 0
	else
	{
		UINT uReturn = WinExec(sProgram, SW_SHOWNORMAL);
		if (31 < uReturn)
		{
			TRACE("started %s\n", sProgram);
		}
		else
		{
			if (0 == uReturn)
			{
				TRACE("NOT started, out of memory %s\n", sProgram);
			}
			else if (ERROR_BAD_FORMAT == uReturn)
			{
				TRACE("NOT started, no EXE %s\n", sProgram);
			}
			else if (ERROR_FILE_NOT_FOUND == uReturn)
			{
				TRACE("NOT started, file not found %s\n", sProgram);
			}
			else if (ERROR_PATH_NOT_FOUND == uReturn)
			{
				TRACE("NOT started, path not found %s\n", sProgram);
			}
			else
			{
				TRACE("NOT started %s, %u\n", sProgram, uReturn);
			}
			CString sMsg;
			sMsg.Format(IDS_PROGRAM_NOT_STARTED, sProgram);
			AfxMessageBox(sMsg);
		}
	}
#else
// gf todo replace WinExec with CreateProcess
	else
	{
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		if (CreateProcess(sProgram,		// LPCTSTR lpApplicationName,
						  NULL,	// LPTSTR lpCommandLine,
                          NULL,		// LPSECURITY_ATTRIBUTES lpProcessAttributes,
						  NULL,		// LPSECURITY_ATTRIBUTES lpThreadAttributes,
						  FALSE,	// BOOL bInheritHandles,
						  0,		// DWORD dwCreationFlags,
						  NULL,		// LPVOID lpEnvironment,
						  NULL,		// LPCTSTR lpCurrentDirectory,
						  &si,		// LPSTARTUPINFO lpStartupInfo,
						  &pi		// LPPROCESS_INFORMATION lpProcessInformation
						  ))
		{
			TRACE(_T("started %s\n"), sProgram);
		}
		else
		{
			DWORD dwError = GetLastError();
			LPVOID lpMsgBuf;
			if (FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				dwError,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) &lpMsgBuf,
				0,
				NULL ))
			{
				// Display the string.
				TRACE(_T("%s NOT started : %s\n"), sProgram, (LPCTSTR)lpMsgBuf);
			}
			else
			{
				TRACE(_T("%s NOT started ; Error code %u\n"), sProgram, dwError);
			}
			CString sMsg;
			sMsg.Format(IDS_PROGRAM_NOT_STARTED, sProgram);
			AfxMessageBox(sMsg);
		}
	}
#endif
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::CollectAllButtons()
{
	m_Buttons.Add(&m_btn1);
	m_Buttons.Add(&m_btn2);
	m_Buttons.Add(&m_btn3);
	m_Buttons.Add(&m_btn4);
	m_Buttons.Add(&m_btn5);
	m_Buttons.Add(&m_btnTest1);
	m_Buttons.Add(&m_btnTest2);
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::CollectAllDisplays()
{
	m_Displays.Add(&m_Display1);
	m_Displays.Add(&m_Display2);
	m_Displays.Add(&m_Display3);
	m_Displays.Add(&m_Display4);
	m_Displays.Add(&m_Display5);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCDStartDlgBase::GetAllSectionEntries(LPCTSTR szSection,
											   CStringArray& sArray)
{
	BOOL bReturn = FALSE;
	sArray.RemoveAll();
	const DWORD dwSize = SHRT_MAX;
	TCHAR szBuffer[dwSize];
	ZeroMemory(szBuffer, dwSize);
	GetPrivateProfileString(szSection, NULL, _T(""), szBuffer, dwSize, m_sIniFile);
	int iIndex = 0;
	CString sSingleEntry = szBuffer;
	while (sSingleEntry != _T(""))
	{
		sArray.Add(sSingleEntry);
		iIndex += sSingleEntry.GetLength() +1;
		sSingleEntry = szBuffer[iIndex];
		bReturn = TRUE;
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
CString CCDStartDlgBase::GetIniString(LPCTSTR szSection,
									  LPCTSTR szEntry,
									  LPCTSTR szDefault)
{
	const DWORD dwSize = SHRT_MAX;
	CString sEntry;
	// ACHTUNG!!!
	// GetPrivateString veraendert den Default-String,
	// wenn es trailing spaces oder Steuerzeichen (\n, \r) findet!!!
	LPCTSTR szDummyDefault = _T("Bloody stupid GetPrivateProfileString");
	GetPrivateProfileString(szSection, szEntry, szDummyDefault,
							sEntry.GetBuffer(dwSize), dwSize, m_sIniFile);
	sEntry.ReleaseBuffer();
	if (szDummyDefault == sEntry)
	{
		sEntry = szDefault;
	}
	return sEntry;
}
/////////////////////////////////////////////////////////////////////////////
int CCDStartDlgBase::GetIniInt(LPCTSTR szSection,
							   LPCTSTR szEntry,
							   int iDefault)
{
	int iReturn = GetPrivateProfileInt(szSection, szEntry, iDefault, m_sIniFile);
	return iReturn;
}
/////////////////////////////////////////////////////////////////////////////
COLORREF CCDStartDlgBase::GetColorRef(const CString& sColor, COLORREF colDefault)
{
	COLORREF colRef = colDefault;
	if (sColor.IsEmpty() == FALSE)
	{
		int iSeperator=0;
		int iRed=0, iGreen=0, iBlue=0;
		// Red
		iRed = max(0, min(255, _ttoi(sColor)));
		iSeperator = sColor.Find(_T(','));
		if (iSeperator != -1)
		{
			// Green
			iGreen = max(0, min(255, _ttoi(sColor.Mid(iSeperator+1))));
			// Blue
			iSeperator = sColor.Find(_T(','), iSeperator+1);
			if (iSeperator != -1)
			{
				iBlue = max(0, min(255, _ttoi(sColor.Mid(iSeperator+1))));
			}
		}
		colRef = RGB(iRed, iGreen, iBlue);
	}
	return colRef;
}
/////////////////////////////////////////////////////////////////////////////
CString CCDStartDlgBase::GetValidFontName(const CString& sFontName, const CString& sDefault)
{
	CString sReturn = sDefault;
	for (int i=0 ; i<m_saFontNames.GetSize() ; i++)
	{
		if (m_saFontNames.GetAt(i) == sFontName)
		{
			sReturn = sFontName;
			break;
		}
	}
	return sReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCDStartDlgBase::IsCopy(const CString& sEntry)
{
	BOOL bReturn = FALSE;
	if (	sEntry.Left(4).CompareNoCase(_T("copy")) == 0
		||	sEntry.Left(5).CompareNoCase(_T("xcopy")) == 0
		)
	{
		bReturn = TRUE;
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCDStartDlgBase::IsRecursivCopy(const CString& sEntry)
{
	BOOL bReturn = FALSE;
	if (sEntry.Left(5).CompareNoCase(_T("xcopy")) == 0)
	{
		bReturn = TRUE;
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCDStartDlgBase::SplitSourceAndDest(const CString& sEntry, CString& sSource, CString& sDest)
{
	BOOL bReturn = FALSE;
	// search first pipe
	int iIndex = sEntry.Find(_T("|"));
	if (-1 != iIndex)
	{
		// skip copy tag
		CString sPathes = sEntry.Mid(iIndex+1);
		// search second pipe
		iIndex = sPathes.Find(_T("|"));
		if (-1 != iIndex)
		{
			// extract source
			sSource = sPathes.Left(iIndex);
			sSource.TrimLeft();
			sSource.TrimRight();
			sSource = GetProgramPathFromEntry(sSource);
			// extract dest
			sDest = sPathes.Mid(iIndex+1);
			sDest.TrimLeft();
			sDest.TrimRight();
			sDest = GetProgramPathFromEntry(sDest);
			bReturn = TRUE;
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
enumSkinButtonShape CCDStartDlgBase::GetButtonShape(int iShape)
{
	enumSkinButtonShape eShape = ShapeRect;
	switch (iShape)
	{
		case ShapeRect:
			eShape = ShapeRect;
			break;
		case ShapeRound:
			eShape = ShapeRound;
			break;
		default:
			eShape = ShapeRect;
			break;
	}
	return eShape;
}
/////////////////////////////////////////////////////////////////////////////
enumSkinButtonSurface CCDStartDlgBase::GetButtonSurface(int iSurface)
{
	enumSkinButtonSurface eSurface = SurfaceColorChangeBrushed;
	switch (iSurface)
	{
		case SurfacePlaneColor:
			eSurface = SurfacePlaneColor;
			break;
		case SurfaceColorChangeSimple:
			eSurface = SurfaceColorChangeSimple;
			break;
		case SurfaceColorChangeBrushed:
			eSurface = SurfaceColorChangeBrushed;
			break;
		case Surface3DPlaneEdge5:
			eSurface = Surface3DPlaneEdge5;
			break;
		case Surface3DPlaneEdge10:
			eSurface = Surface3DPlaneEdge10;
			break;
		case Surface3DPlaneEdge15:
			eSurface = Surface3DPlaneEdge15;
			break;
		default:
			eSurface = SurfaceColorChangeBrushed;
			break;
	}
	return eSurface;
}
/////////////////////////////////////////////////////////////////////////////
STYLE_BACKGROUND CCDStartDlgBase::GetBackgroundStyle(int iStyle)
{
	STYLE_BACKGROUND eStyle = StyleBackGroundColorChangeBrushed;
	switch (iStyle)
	{
		case StyleBackGroundPlaneColor:
			eStyle = StyleBackGroundPlaneColor;
			break;
		case StyleBackGroundColorChangeSimple:
			eStyle = StyleBackGroundColorChangeSimple;
			break;
		case StyleBackGroundColorChangeBrushed:
			eStyle = StyleBackGroundColorChangeBrushed;
			break;
		default:
			eStyle = StyleBackGroundColorChangeBrushed;
			break;
	}
	return eStyle;
}
/////////////////////////////////////////////////////////////////////////////
int CALLBACK CCDStartDlgBase::MyEnumFontFamExProc(
									ENUMLOGFONTEX *lpelfe,    // pointer to logical-font data
									NEWTEXTMETRICEX *lpntme,  // pointer to physical-font data
									int FontType,             // type of font
									LPARAM lParam             // application-defined data
									)
{
	CStringArray*	psaFontNames = (CStringArray*)lParam;
	TRACE(_T("%s\n"), lpelfe->elfFullName);
	psaFontNames->Add(lpelfe->elfFullName);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::DoDataExchange(CDataExchange* pDX)
{
	CTransparentDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCDStartDlgBase)
	DDX_Control(pDX, IDC_TXT_SIZE, m_txtSize);
	DDX_Control(pDX, IDC_DISPLAY_5, m_Display5);
	DDX_Control(pDX, IDC_DISPLAY_4, m_Display4);
	DDX_Control(pDX, IDC_DISPLAY_3, m_Display3);
	DDX_Control(pDX, IDC_DISPLAY_2, m_Display2);
	DDX_Control(pDX, IDC_DISPLAY_1, m_Display1);
	DDX_Control(pDX, IDC_DISPLAY_TITLE, m_DisplayTitle);
	DDX_Control(pDX, IDC_TEST_1, m_btnTest1);
	DDX_Control(pDX, IDC_TEST_2, m_btnTest2);
	DDX_Control(pDX, IDC_BUTTON_1, m_btn1);
	DDX_Control(pDX, IDC_BUTTON_2, m_btn2);
	DDX_Control(pDX, IDC_BUTTON_3, m_btn3);
	DDX_Control(pDX, IDC_BUTTON_4, m_btn4);
	DDX_Control(pDX, IDC_BUTTON_5, m_btn5);
	DDX_Text(pDX, IDC_TXT_SIZE, m_sSize);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CCDStartDlgBase, CTransparentDialog)
	//{{AFX_MSG_MAP(CCDStartDlgBase)
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BUTTON_1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON_2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON_3, OnButton3)
	ON_BN_CLICKED(IDC_BUTTON_4, OnButton4)
	ON_BN_CLICKED(IDC_BUTTON_5, OnButton5)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_TEST_1, OnTest)
	ON_BN_CLICKED(IDC_TEST_2, OnTest)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WKM_INITDLG_READY, OnInitDlgReady)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CCDStartDlgBase message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CCDStartDlgBase::OnInitDialog()
{
	StretchButtonHeight(TRUE);
	CTransparentDialog::OnInitDialog();

	SetWindowText(szAppName);
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	// when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);		// Set big icon
	SetIcon(m_hIcon, FALSE);	// Set small icon

	// TODO: Add extra initialization here
	m_bAutoRun = GetPrivateProfileInt(szSectionControl, szEntryAutoRun, 0, m_sIniFile);
	if (m_bAutoRun)
	{
		ShowWindow(SW_HIDE);
		AutoRun();
		// Zum Schluﬂ wird das Programm wieder beendet
		PostMessage(WM_COMMAND, IDCANCEL, NULL);
	}
	else
	{
		InitDlgControls();
		PostMessage(WKM_INITDLG_READY, NULL, NULL);
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CCDStartDlgBase::OnInitDlgReady(WPARAM wParam, LPARAM lParam)
{
	Init();
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
	}
	else
	{
		CTransparentDialog::OnSysCommand(nID, lParam);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::OnDestroy()
{
	WinHelp(0L, HELP_QUIT);
	CTransparentDialog::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CCDStartDlgBase::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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
		CTransparentDialog::OnPaint();
	}
}
/////////////////////////////////////////////////////////////////////////////
// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCDStartDlgBase::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::OnOK() 
{
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::OnCancel() 
{
	CTransparentDialog::OnCancel();
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::OnTest() 
{
	CRect rc1, rc2;
	m_btnTest1.GetWindowRect(rc1);
	m_btnTest2.GetWindowRect(rc2);
	ScreenToClient(rc1);
	ScreenToClient(rc2);
	
	SHORT w = GetAsyncKeyState(VK_LCONTROL);
	int iDiff = 0;
	if (w & 0x8000) // VK_LCONTROL down
	{
		iDiff = (rc1.Width()==1) ? 0 : -1;
	}
	else
	{
		iDiff++;
	}
	rc1.left += iDiff;
	rc1.bottom -= iDiff;
	rc2.left += iDiff;
	rc2.bottom -= iDiff;
	m_sSize.Format(_T("%i"), rc1.Height());
	m_txtSize.SetWindowText(m_sSize);
	m_btnTest1.MoveWindow(rc1, TRUE);
	m_btnTest2.MoveWindow(rc2, TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::OnButton1() 
{
	ActionButton1();
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::OnButton2() 
{
	ActionButton2();
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::OnButton3() 
{
	ActionButton3();
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::OnButton4() 
{
	ActionButton4();
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::OnButton5() 
{
	ActionButton5();
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::OnMouseMove(UINT nFlags, CPoint point) 
{
	CRect rect;
	int iShowDisplay = 0;

	// point ist relativ zum Fenster, mappe auf Desktop, das vereinfacht den Vergleich
	MapWindowPoints(NULL, &point, 1);
	m_btn1.GetWindowRect(rect);
	if (rect.PtInRect(point))
	{
		if (m_btn1.IsWindowVisible())
		{
			iShowDisplay = 1;
		}
	}
	else
	{
		m_btn2.GetWindowRect(rect);
		if (rect.PtInRect(point))
		{
			if (m_btn2.IsWindowVisible())
			{
				iShowDisplay = 2;
			}
		}
		else
		{
			m_btn3.GetWindowRect(rect);
			if (rect.PtInRect(point))
			{
				if (m_btn3.IsWindowVisible())
				{
					iShowDisplay = 3;
				}
			}
			else
			{
				m_btn4.GetWindowRect(rect);
				if (rect.PtInRect(point))
				{
					if (m_btn4.IsWindowVisible())
					{
						iShowDisplay = 4;
					}
				}
				else
				{
					m_btn5.GetWindowRect(rect);
					if (rect.PtInRect(point))
					{
						if (m_btn5.IsWindowVisible())
						{
							iShowDisplay = 5;
						}
					}
					else
					{
						// for more buttons
						iShowDisplay = 0;
					}
				}
			}
		}
	}
	ShowDisplay(iShowDisplay);
	CTransparentDialog::OnMouseMove(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCDStartDlgBase::OnEraseBkgnd(CDC* pDC) 
{
	BOOL bReturn = FALSE;
	CRect	rect;
	GetClientRect(rect);

	switch (m_eStyleBackground)
	{
		case StyleBackGroundColorChangeSimple:
			DrawBackGroundColorChangeSimple(pDC, rect);
			break;
		case StyleBackGroundColorChangeBrushed:
			DrawBackGroundColorChangeBrushed(pDC, rect);
			bReturn = TRUE;
			break;
		default:
			bReturn = CTransparentDialog::OnEraseBkgnd(pDC);
			break;
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::DrawBackGroundColorChangeSimple(CDC* pDC, CRect rect)
{
	// Farbwerte fuer Farbverlauf bestimmen
	double r,g,b;
	r = min(GetRValue(m_BaseColor),127);
	r /= rect.Height();
	g = min(GetGValue(m_BaseColor),127);
	g /= rect.Height();
	b = min(GetBValue(m_BaseColor),127);
	b /= rect.Height();

	CBrush brush;
	CRect rc = rect;
	COLORREF col;
	for (int i = 0;i<rect.Height();i++)
	{
		col = RGB(GetRValue(m_BaseColor)-r*i,
				  GetGValue(m_BaseColor)-g*i,
				  GetBValue(m_BaseColor)-b*i);

		rc.top = rect.top  + i;
		rc.bottom = rect.top + i + 1;
		brush.CreateSolidBrush(col);
		pDC->FillRect(rc,&brush);
		brush.DeleteObject();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::DrawBackGroundColorChangeBrushed(CDC* pDC, CRect rect)
{
	BOOL m_bChangeOnlyBrightness = TRUE;
	if (m_bChangeOnlyBrightness)
	{
		// Umrechnen in YCbCr-Farbraum
		double dY, dCb, dCr;
		RGB2YCrCb(m_BaseColor, dY, dCb, dCr);

		// Beim Farbverlauf sollte nur die Helligkeit Y wechseln
		// Bestimme die kleinste Spanne bei der Farbe, vergleiche mit vorgegebener Begrenzung
		double dYDiff = min(255-dY, dY);
		dYDiff = (dYDiff < m_dMaxColorChangeRange) ? dYDiff : m_dMaxColorChangeRange;
//		if (m_bDebugTrace)
//		{
//			TRACE(_T("dYDiff total %03.4f\n"), dYDiff);
//		}

		// Verteile die Helligkeitsspanne auf die Abstufungen
		double dSpan = rect.Height()/2;
		dYDiff /= dSpan;
//		if (m_bDebugTrace)
//		{
//			TRACE(_T("dYDiff step %03.4f\n"), dYDiff);
//		}

		// Fange an bei der hellsten Abstufung
		dY = dY + dSpan * dYDiff;
		CBrush brush;
		CRect rc = rect;
		COLORREF col;
		int s = 1;
		BOOL bBright = TRUE;
		for (int i=0;i<rect.Height();i+=s)
		{
			if (bBright)
			{
				col = YCrCb2RGB(dY-i*dYDiff, dCb, dCr);
//				if (m_bDebugTrace)
//				{
//					TRACE(_T("CSkinButton color change simple r=%03i g=%03i b=%03i\n"),
//								GetRValue(col), GetGValue(col), GetBValue(col));
//				}
				bBright = FALSE;
			}
			else
			{
				col = YCrCb2RGB(dY-(i+50)*dYDiff, dCb, dCr);
//				if (m_bDebugTrace)
//				{
//					TRACE(_T("CSkinButton color change simple r=%03i g=%03i b=%03i\n"),
//								GetRValue(col), GetGValue(col), GetBValue(col));
//				}
				bBright = TRUE;
			}
			rc.top = rect.top  + i;
			rc.bottom = rect.top + i + s;
			brush.CreateSolidBrush(col);
			pDC->FillRect(rc,&brush);
			brush.DeleteObject();
		}
	}
	else
	{
		// Farbwerte fuer Farbverlauf bestimmen
		double dRBase = GetRValue(m_BaseColor);
		double dGBase = GetGValue(m_BaseColor);
		double dBBase = GetBValue(m_BaseColor);
		double dSpan = rect.Height()/2;
		double r,g,b;
		r = min(dRBase, 255-dRBase);
		r = (r < m_dMaxColorChangeRange) ? r : m_dMaxColorChangeRange;
		r /= dSpan;
		g = min(dGBase, 255-dGBase);
		g = (g < m_dMaxColorChangeRange) ? g : m_dMaxColorChangeRange;
		g /= dSpan;
		b = min(dBBase, 255-dBBase);
		b = (b < m_dMaxColorChangeRange) ? b : m_dMaxColorChangeRange;
		b /= dSpan;

		// Fange an bei der hellsten Abstufung an
		dRBase += r*dSpan;
		dGBase += g*dSpan;
		dBBase += b*dSpan;

//		if (m_bDebugTrace)
//		{
//			TRACE(_T("DrawBackGroundColorChangeBrushed start R %03.0f   G %03.0f   B %03.0f\n"),
//															dRBase, dGBase, dBBase);
//		}
		CBrush brush;
		CRect rc = rect;
		COLORREF col;
		double dR, dG, dB;
		int s = 1;
		BOOL bBright = TRUE;
		for (int i=0;i<rect.Height();i+=s)
		{
			if (bBright)
			{
				dR = dRBase - r*i;
				dG = dGBase - g*i;
				dB = dBBase - b*i;
				col = RGB(dR, dG, dB);
				bBright = FALSE;
			}
			else
			{
				dR = dRBase - r*(i+5);
				dG = dGBase - r*(i+5);
				dB = dBBase - r*(i+5);
				col = RGB(dR, dG, dB);
				bBright = TRUE;
			}
			rc.top = rect.top  + i;
			rc.bottom = rect.top + i + s;
			brush.CreateSolidBrush(col);
			pDC->FillRect(rc,&brush);
			brush.DeleteObject();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCDStartDlgBase::RGB2YCrCb(COLORREF col, double& dY, double& dCb, double& dCr)
{
	double dR = GetRValue(col);
	double dG = GetGValue(col);
	double dB = GetBValue(col);
	dY	=  0.2990*dR + 0.5870*dG + 0.1140*dB;
	dCb	= -0.1687*dR - 0.3313*dG + 0.5000*dB;
	dCr	=  0.5000*dR - 0.4187*dG - 0.0813*dB;
//	TRACE(_T("dY %03.4f   dCb %03.4f   dCr %03.4f\n"), dY, dCb, dCr);
}
/////////////////////////////////////////////////////////////////////////////
COLORREF CCDStartDlgBase::YCrCb2RGB(const double& dY, const double& dCb, const double& dCr)
{
//	TRACE(_T("\ndY %03.4f   dCb %03.4f   dCr %03.4f\n"), dY, dCb, dCr);
	double dR = (dY +             + 1.40200*dCr);
	double dG = (dY - 0.34414*dCb - 0.71414*dCr);
	double dB = (dY + 1.72700*dCb              );
	// CAVEAT, RGB jeweils im Bereich 0-255
	dR = max(0, min(255,dR));
	dG = max(0, min(255,dG));
	dB = max(0, min(255,dB));
//	TRACE(_T("R %03.0f   G %03.0f   B %03.0f\n"), dR, dG, dB);
	COLORREF col = RGB((int)dR, (int)dG, (int)dB);
	return col;
}
