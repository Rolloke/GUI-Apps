// InfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dvclient.h"
#include "InfoDlg.h"
#include "CPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInfoDlg dialog


CInfoDlg::CInfoDlg(CString sInfo, CWnd* pParent /*=NULL*/)
	: CTransparentDialog(CInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInfoDlg)
	//}}AFX_DATA_INIT

	m_hCursor	= NULL;
	m_pPanel	= pParent;
	m_BaseColor	= CPanel::m_BaseColorBackGrounds;
	m_sInfoTxt	= sInfo;
}

/////////////////////////////////////////////////////////////////////////////
CInfoDlg::~CInfoDlg()
{

}

/////////////////////////////////////////////////////////////////////////////
void CInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CTransparentDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CInfoDlg)
	DDX_Control(pDX, IDC_FEATURE_NAME, m_ctrlDisplayFeatureName);
	DDX_Control(pDX, ID_INFO_OK, m_ctrlOK);
	DDX_Control(pDX, IDC_BORDER, m_ctrlBorder);
	DDX_Control(pDX, IDC_FEATURE, m_ctrlDisplayFeature);
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CInfoDlg, CTransparentDialog)
	//{{AFX_MSG_MAP(CInfoDlg)
	ON_WM_SETCURSOR()
	ON_BN_CLICKED(ID_INFO_OK, OnInfoOk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CInfoDlg message handlers
BOOL CInfoDlg::OnInitDialog() 
{
	CTransparentDialog::OnInitDialog();
	
	m_hCursor = theApp.m_hArrow;

	CreateTransparent(StyleBackGroundColorChangeBrushed, CPanel::m_BaseColorBackGrounds);
	
	m_ctrlDisplayFeature.OpenStockDisplay(this, Display2);
	m_ctrlDisplayFeatureName.OpenStockDisplay(this, Display2);

	// Kontrast erhöhen
	DSPCOL DisplayColors;
	DisplayColors.colText		= SKIN_COLOR_BLACK;
	DisplayColors.colBackGround = CPanel::m_BaseColorDisplays; // Background Green Color

	// Alarmcom bekommt eine andere Displayfarbe
	// soll doch in grün, da im hauptmenü
/*	if ((m_pPanel && ((CPanel*)m_pPanel)->GetOem() == OemAlarmCom) ||
		(m_pPanel && ((CPanel*)m_pPanel)->GetOem() == OemPOne))
		DisplayColors.colBackGround	= SKIN_COLOR_BLUE; // Background Blue Color
*/
	
	DisplayColors.colBorder		= DisplayColors.colBackGround;
	DisplayColors.dwFlags = DSP_COL_TEXT | DSP_COL_BACKGROUNND | DSP_COL_BORDER;
	m_ctrlDisplayFeature.SetDisplayColors(DisplayColors);
	m_ctrlDisplayFeatureName.SetDisplayColors(DisplayColors);


	m_ctrlBorder.CreateFrame(this);
	m_ctrlBorder.EnableShadow(TRUE);
		
	FRAMECOL FrameColors;
	FrameColors.dwFlags = FRAME_COL_BACKGROUNND;
	FrameColors.colBackGround = DisplayColors.colBackGround;
	m_ctrlBorder.SetFrameColors(FrameColors);
 
	m_ctrlDisplayFeature.SetTextAllignment(DT_LEFT|DT_WORDBREAK|DT_EXPANDTABS);
	m_ctrlDisplayFeature.SetTextAllignment(DT_LEFT|DT_WORDBREAK|DT_EXPANDTABS);

	m_ctrlBorder.CreateFrame(this);
	m_ctrlBorder.EnableShadow(TRUE);

	m_ilOK.Create(IDB_ENTER, 16, 0, SKIN_COLOR_KEY);
	m_ctrlOK.SetImageList(&m_ilOK);
	m_ctrlOK.SetBaseColor(m_BaseColor);
	
	UpdateDisplayFeatures();

	CRect rcPanel;
	if (m_pPanel)
	{
		m_pPanel->GetWindowRect(rcPanel);

		CRect rcInfoDlg;
		GetWindowRect(rcInfoDlg);
		
		// InfoDialog oberhalb des Panels positionieren
		int nX	= rcPanel.left;
		int	nY	= rcPanel.top - rcInfoDlg.Height();

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
void CInfoDlg::UpdateDisplayFeatures() 
{

	CStringArray saFeatures;
	CString sFeature, sFeatureName;
	int iMaxTxtLenght = 0;
	int iNewTxtLength = 0;

	//Display Text formatieren
	SplitString(m_sInfoTxt,saFeatures,';');

	for(int i=0; i<saFeatures.GetSize(); i++)
	{
		CString sTxt = saFeatures.GetAt(i);
		int iSeparator = sTxt.Find(':');
		int iLength	   = sTxt.GetLength();
		if(iSeparator != -1 && iLength > iSeparator)
		{
			CString sNewTxt;
			sNewTxt		 = sTxt.Left(iSeparator) + _T(":\n");
			sFeature	 = sFeature     + sNewTxt;

			sNewTxt		 = sTxt.Right(iLength-iSeparator-1) + _T("\n");
			sFeatureName = sFeatureName + sNewTxt;

			iNewTxtLength = sNewTxt.GetLength();
			if(iMaxTxtLenght < iNewTxtLength)
			{
				iMaxTxtLenght = iNewTxtLength;
			}
		}
	}

	CRect rcFeatureName, rcBorder, rcOk, rcDlg;
	GetWindowRect(rcDlg);

	// Größe des benötigten Displayfensters anhand der Zeilenlänge ermitteln.
	m_ctrlDisplayFeatureName.GetWindowRect(rcFeatureName);
	int iBigger = 0;
	int iDisplayWidth = rcFeatureName.Width();
	if(iDisplayWidth < iMaxTxtLenght*10)
	{
		iBigger = iMaxTxtLenght*10 - iDisplayWidth;
		rcFeatureName.right = rcFeatureName.left + iDisplayWidth + iBigger;
	}

	//muss der Dialog vergrößert werden ?
	if(iBigger > 0)
	{
		// Display horizontal vergrößern
		m_ctrlDisplayFeatureName.SetWindowPos(&wndTop, -1, -1,
					rcFeatureName.Width(), rcFeatureName.Height(), SWP_NOMOVE);

		// Border horizontal vergrößern
		m_ctrlBorder.GetWindowRect(rcBorder);
 		m_ctrlBorder.SetWindowPos(&wndTop, -1, -1, rcBorder.Width() + iBigger, rcBorder.Height(), SWP_NOMOVE);

		// Ok-Button neu positionieren
		m_ctrlOK.GetWindowRect(rcOk);
		m_ctrlOK.SetWindowPos(&wndTop, rcDlg.Width() - rcOk.Width() + iBigger - 10, 
				rcDlg.Height() - rcOk.Height() - 10, -1, -1, SWP_NOSIZE);

		// Dialog horizontal vergrößern
		SetWindowPos(&wndTopMost, -1, -1, rcDlg.Width() + iBigger, rcDlg.Height(), SWP_NOMOVE);
	}



	//Ausgabe im Display formatieren
	CFont font;
	font.CreatePointFont(12*10, GetFixFontFaceName());
	
	m_ctrlDisplayFeature.SetFont(font);
	m_ctrlDisplayFeatureName.SetFont(font);
	m_ctrlDisplayFeature.SetTextAllignment(DT_RIGHT);
	m_ctrlDisplayFeature.SetDisplayText(sFeature);
	m_ctrlDisplayFeatureName.SetDisplayText(sFeatureName);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CInfoDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_hCursor)
	{
		if (SetCursor(m_hCursor) != NULL)
			return TRUE;		
	}	
	
	return CTransparentDialog::OnSetCursor(pWnd, nHitTest, message);
}

/////////////////////////////////////////////////////////////////////////////
void CInfoDlg::OnInfoOk() 
{
	EndDialog(IDOK);
}
