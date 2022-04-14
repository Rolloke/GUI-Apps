// CStatisticDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dvclient.h"
#include "CStatisticDlg.h"
#include "MainFrame.h"
#include "Server.h"
#include "CPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CDVClientApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CStatisticDlg dialog


/////////////////////////////////////////////////////////////////////////////
CStatisticDlg::CStatisticDlg(CWnd* pParent /*=NULL*/)
	: CTransparentDialog(CStatisticDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStatisticDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hCursor	= NULL;
	m_pPanel	= pParent;
	m_BaseColor	= CPanel::m_BaseColorBackGrounds;
}

/////////////////////////////////////////////////////////////////////////////
CStatisticDlg::~CStatisticDlg()
{
	ClearStatistics();
}

/////////////////////////////////////////////////////////////////////////////
void CStatisticDlg::DoDataExchange(CDataExchange* pDX)
{
	CTransparentDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStatisticDlg)
	DDX_Control(pDX, IDC_STATISTIC_OK, m_ctrlOK);
	DDX_Control(pDX, IDC_STATISTIC_DISPLAY, m_ctrlDisplay);
	DDX_Control(pDX, IDC_STATISTIC_BORDER, m_ctrlBorder);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStatisticDlg, CTransparentDialog)
	//{{AFX_MSG_MAP(CStatisticDlg)
	ON_WM_SETCURSOR()
	ON_BN_CLICKED(IDC_STATISTIC_OK, OnStatisticOk)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
BOOL CStatisticDlg::OnInitDialog() 
{
	CTransparentDialog::OnInitDialog();
	
	m_hCursor = theApp.m_hArrow;

	CreateTransparent(StyleBackGroundColorChangeBrushed, CPanel::m_BaseColorBackGrounds);
	
	m_ctrlDisplay.OpenStockDisplay(this, Display2);

	// Kontrast erhöhen
	DSPCOL DisplayColors;
	DisplayColors.colText		= SKIN_COLOR_BLACK;
	DisplayColors.colBackGround = CPanel::m_BaseColorDisplays; // Background Green Color

	//TKR kann entfallen, da der Statistic Dialog sich nun im Hauptmenü befindet
	// Alarmcom bekommt eine andere Displayfarbe
/*	if ((m_pPanel && ((CPanel*)m_pPanel)->GetOem() == OemAlarmCom) ||
		(m_pPanel && ((CPanel*)m_pPanel)->GetOem() == OemPOne))
		DisplayColors.colBackGround	= SKIN_COLOR_BLUE; // Background Blue Color
*/
	DisplayColors.colBorder	= DisplayColors.colBackGround;
	DisplayColors.dwFlags = DSP_COL_TEXT | DSP_COL_BACKGROUNND | DSP_COL_BORDER;
	m_ctrlDisplay.SetDisplayColors(DisplayColors);

	m_ctrlBorder.CreateFrame(this);
	m_ctrlBorder.EnableShadow(TRUE);
		
	FRAMECOL FrameColors;
	FrameColors.dwFlags = FRAME_COL_BACKGROUNND;
	FrameColors.colBackGround = DisplayColors.colBackGround;
	m_ctrlBorder.SetFrameColors(FrameColors);
 
	m_ctrlDisplay.SetTextAllignment(DT_LEFT|DT_WORDBREAK|DT_EXPANDTABS);

	m_ctrlBorder.CreateFrame(this);
	m_ctrlBorder.EnableShadow(TRUE);

	m_ilOK.Create(IDB_ENTER, 16, 0, SKIN_COLOR_KEY);
	m_ctrlOK.SetImageList(&m_ilOK);
	m_ctrlOK.SetBaseColor(m_BaseColor);
	
	UpdateStatistics();

	CRect rcPanel;
	if (m_pPanel)
	{
		m_pPanel->GetWindowRect(rcPanel);

		CRect rcStatisticDlg;
		GetWindowRect(rcStatisticDlg);
		
		// StatisticDialog oberhalb des Panels positionieren
		int nX	= rcPanel.left;
		int	nY	= rcPanel.top - rcStatisticDlg.Height();

		if (nX < 0 || nY < 0)
		{
			// RTEDialog unterhalb des Panels positionieren
			nX	= rcPanel.left;
			nY	= rcPanel.bottom;
		}
		SetWindowPos(&wndTopMost, nX, nY, -1, -1, SWP_NOSIZE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
BOOL CStatisticDlg::UpdateStatistics()
{
	BOOL bResult = FALSE;
	__int64 i64TotalNrOfPictures = 0;
	__int64 i64NrOfPictures = 0;

	CMainFrame* pMF = theApp.GetMainFrame();
	if (pMF)
	{
		ClearStatistics();

		for (int nCam = 0; nCam < MAX_CAM; nCam++)
		{
			CPlayWindow* pPlay = pMF->GetPlayWindow((WORD)nCam);

			if (pPlay)
			{
				i64NrOfPictures	= pPlay->GetNrOfPictures();
				i64TotalNrOfPictures += i64NrOfPictures;
				CStatisticRecord* pStatistic = new CStatisticRecord();
				pStatistic->sCamName = pPlay->GetName();
				pStatistic->StartTime = pPlay->GetStartTime();

				CSystemTime EndTime;
				CSystemTime Duration;
				if (!pPlay->GetServer()->GetClock(EndTime))
				{
					EndTime.GetLocalTime();
				}
				pStatistic->Duration = EndTime - pStatistic->StartTime;
				pStatistic->i64Using = i64NrOfPictures;
				m_Statistics.Add(pStatistic);
				bResult = TRUE;
			}
		}
	}

	// Zeilenbreite
	int nColumnWidth1 = 18;
	int nColumnWidth2 = 13;
	int nColumnWidth3 = 7;
	int nColumnWidth4 = 10;

	// niemals feste Zeichenbreite annehmen, immer vom Font ermitteln
	// bisher können wir nur anhand des DllInfo den chinesischen Zeichensatz unterscheiden
	// Ein chin. Zeichen ist zwei ANSI- Zeichen breit!
	BOOL bChina = (((CPanel*)m_pPanel)->GetLangDllInfo())->GetCodePageBits();
	int iChars = 0;

	// 1. Überschrift formatieren
	CString sColumnTitle1 = _T("");
	sColumnTitle1.LoadString(IDS_CAMNAME);
	iChars = sColumnTitle1.GetLength();
	if (bChina)
	{
		iChars = 2 * iChars - 1;
	}
	sColumnTitle1 += CString(_T(' '), nColumnWidth1-iChars);
	
	// 2. Überschrift formatieren
	CString sColumnTitle2 = _T("");
	sColumnTitle2.LoadString(IDS_STATISTIC_STARTTIME);
	iChars = sColumnTitle2.GetLength();
	if (bChina)
	{
		iChars = 2 * iChars - 1;
	}
	sColumnTitle2 += CString(_T(' '), nColumnWidth2-iChars);

	// 3. Überschrift formatieren
	CString sColumnTitle3 = _T("");
	sColumnTitle3.LoadString(IDS_STATISTIC_DURATION);
	iChars = sColumnTitle3.GetLength();
	if (bChina)
	{
		iChars = 2 * iChars - 1;
	}
	sColumnTitle3 += CString(_T(' '), nColumnWidth3-iChars);

	// 4. Überschrift formatieren
	CString sColumnTitle4 = _T("");
	sColumnTitle4.LoadString(IDS_STATISTIC_PICCOUNTER);
	iChars = sColumnTitle4.GetLength();
	if (bChina)
	{
		iChars = 2 * iChars - 1;
	}
	sColumnTitle4 += CString(_T(' '), nColumnWidth4-iChars);

	CString sText;
	CString sTemp;

	// Gesamtüberschrift.
	sText.Format(_T("%s%s%s%s\n"),	sColumnTitle1, sColumnTitle2, sColumnTitle3, sColumnTitle4);
	
	for (int nI = 0; nI < m_Statistics.GetSize(); nI++)
	{
		CStatisticRecord* pStatistic = m_Statistics[nI];
		CString sCamName	= _T("");
		CString sStartTime	= _T("");
		CString	sDuration	= _T("");
		CString sPicCounter = _T("");

		// Kameraname formatieren
		sCamName = pStatistic->sCamName;
		iChars = sCamName.GetLength();
		sCamName += CString(_T(' '), nColumnWidth1-iChars);

		// Startzeit formatieren
		sStartTime.Format(_T("%02d.%02d %02d:%02d"),	pStatistic->StartTime.GetDay(),
													pStatistic->StartTime.GetMonth(),
													pStatistic->StartTime.GetHour(),
													pStatistic->StartTime.GetMinute());

		iChars = sStartTime.GetLength();
		sStartTime += CString(_T(' '), nColumnWidth2-iChars);
		
		// Aufnahmedauer in Tagen berechnen
		LARGE_INTEGER liDuration;
		FILETIME ftResult = {0,0};
		if (SystemTimeToFileTime(&pStatistic->Duration,&ftResult))
		{
			liDuration.LowPart  = ftResult.dwLowDateTime;
			liDuration.HighPart = ftResult.dwHighDateTime;
			DWORD dwDuration = (DWORD)(liDuration.QuadPart / 10000000);

			sDuration.Format(_T("%.1f"), (double)dwDuration / (24*3600));
		}

		// Aufnahmedauer formatieren
		iChars = sDuration.GetLength();
		sDuration += CString(_T(' '), nColumnWidth3-iChars);

		// Bildzähler formatieren.
		sPicCounter.Format(_T("%I64u"), pStatistic->i64Using);
		iChars = sPicCounter.GetLength();
		sPicCounter += CString(_T(' '), nColumnWidth4-iChars);

		// Gesamttext erstellen
		sTemp.Format(_T("%s%s%s%s\n"),	sCamName, sStartTime, sDuration, sPicCounter);
		sText += sTemp;
	}

	CRect rcText, rcOk;

	// Größe des benötigten Displayfensters anhand der Zeilenzahl ermitteln.
	int iLineHeight = 20;
#ifdef _UNICODE
	// niemals feste Zeilenhöhe annehmen, immer vom Font ermitteln
	// aber wie?
/*	pFont = m_ctrlDisplay.GetDisplayFont();
	if (pFont->GetLogFont(&lf))
	{
		TRACE(_T("%s lfHeight %li\n"), lf.lfFaceName, lf.lfHeight);
		lfHeight = -MulDiv(PointSize, GetDeviceCaps(hDC, LOGPIXELSY), 72);
		iLineHeight = MulDiv(abs(lf.lfHeight), GetDeviceCaps(hDC, LOGPIXELSY), 72);
		iLineHeight = ;
	}
*/
#endif
	GetWindowRect(rcText);
	rcText.bottom = rcText.top + (m_Statistics.GetSize()+1) * iLineHeight;

	// Display vertikal vergrößern
	m_ctrlDisplay.SetWindowPos(&wndTop, 4, 4, rcText.Width()-16, rcText.Height()-4, 0);

	// Border vertikal vergrößern
 	m_ctrlBorder.SetWindowPos(&wndTop, 4, 4, rcText.Width()-16, rcText.Height(), 0);

	// Ok-Button neu positionieren
	m_ctrlOK.GetWindowRect(rcOk);
	m_ctrlOK.SetWindowPos(&wndTop, rcText.Width() - rcOk.Width()-8, rcText.Height()+8, -1, -1, SWP_NOSIZE);
	
	// Dialog vertikal vergrößern
	SetWindowPos(&wndTopMost, -1, -1, rcText.Width(), rcText.Height()+40, SWP_NOMOVE);

	// Displaytet ausgeben.
	m_ctrlDisplay.SetDisplayText(sText);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
void CStatisticDlg::ClearStatistics()
{
	m_Statistics.DeleteAll();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CStatisticDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_hCursor)
	{
		if (SetCursor(m_hCursor) != NULL)
			return TRUE;		
	}	
	
	return CTransparentDialog::OnSetCursor(pWnd, nHitTest, message);
}

//////////////////////////////////////////////////////////////////////
void CStatisticDlg::OnStatisticOk() 
{
	EndDialog(IDOK);
}
