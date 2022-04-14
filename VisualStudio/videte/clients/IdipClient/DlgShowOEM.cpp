// DlgShowOEM.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "DlgShowOEM.h"

#include "MainFrm.h"
#include ".\dlgshowoem.h"


/////////////////////////////////////////////////////////////////////////////////////////
// CDlgShowOEM dialog
/////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CDlgShowOEM, CDlgInView)
/////////////////////////////////////////////////////////////////////////////////////////
CDlgShowOEM::CDlgShowOEM(CWnd* pParent /*=NULL*/)
	: CDlgInView(CDlgShowOEM::IDD, pParent)
{
	m_nInitToolTips = TOOLTIPS_SIMPLE;
	MoveOnClientAreaClick(false);
	Create(CDlgShowOEM::IDD, pParent);
}
/////////////////////////////////////////////////////////////////////////////////////////
CDlgShowOEM::~CDlgShowOEM()
{
}
/////////////////////////////////////////////////////////////////////////////////////////
void CDlgShowOEM::DoDataExchange(CDataExchange* pDX)
{
	CDlgInView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CLOSE_DLG_BAR, m_btnClose);
	DDX_Control(pDX, IDC_DATE, m_txtDate);
	DDX_Control(pDX, IDC_TIME, m_txtTime);
}
/////////////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDlgShowOEM, CDlgInView)
	ON_BN_CLICKED(IDC_CLOSE_DLG_BAR, OnBnClickedCloseDlgBar)
	ON_MESSAGE(WM_USER, OnUserMsg)
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////////////////
// CDlgShowOEM message handlers
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CDlgShowOEM::OnInitDialog()
{
	CDlgInView::OnInitDialog();
//	m_InitialClientRect.bottom--;
//	m_InitialClientRect.right--;

	AutoCreateSkinButtons();
	AutoCreateSkinStatic();
	CString sTitle;
	sTitle.LoadString(IDS_LOGO);
	SetWindowText(sTitle);
	m_btnClose.SetImageList(GetDownButtonImages());
	//RemoveFromResize(IDC_CLOSE_DLG_BAR);
	KeepProportion(IDC_CLOSE_DLG_BAR);

	CString sPath;
	sPath = theApp.GetApplicationDir() + _T("Logo.bmp");
	if (GetFileAttributes(sPath) != INVALID_FILE_ATTRIBUTES)
	{
		BOOL bLoad = TRUE;
		CDibSection DibSection;
		CFile file(sPath, CFile::modeRead);
		if (DibSection.LoadFromFile(file))
		{
			if (DibSection.GetBitmapInfo()->bmiHeader.biBitCount > 8)
			{
				bLoad = FALSE;
				WK_TRACE(_T("WARNING Logobitmap bit depth too big %d\n"), DibSection.GetBitmapInfo()->bmiHeader.biBitCount);
			}
		}
		file.Close();
		if (bLoad)
		{
			HIMAGELIST hList = ImageList_LoadImage(NULL, sPath, DibSection.GetWidth()/2, 0, DibSection.GetPixel(0, 0), IMAGE_BITMAP, LR_LOADFROMFILE);
			if (hList)
			{
				m_Logo.Attach(hList);
			}
		}
	}
	if (m_Logo.GetSafeHandle() == NULL)
	{
		m_Logo.Create(IDB_LOGOS, 179, 0, SKIN_COLOR_KEY);
	}

	double dY, dCb, dCr;
	CSkinButton::RGB2YCrCb(m_BaseColor, dY, dCb, dCr);
	if (dY > 150)
	{
		m_nImage = 1;
	}
	else
	{
		m_nImage = 0;
	}

	CWnd*pWnd = GetDlgItem(IDC_TXT_PRODUCT);
	ASSERT(pWnd!=NULL);
	m_Font1.CreatePointFont(10*8, _T("Arial"));
	pWnd->SetFont(&m_Font1);
	CString sProductName;

	if (theApp.IsReadOnlyModus())
	{
		sProductName = COemGuiApi::GetApplicationNameOem(WAI_IDIP_CLIENT, theApp.m_OemCode);
		_TCHAR szVersion[32];
		int i, nLen = GetPrivateProfileString(_T("oem"),_T("version"), NULL, szVersion, 32, theApp.m_sOemIni);
		for (i=nLen-1, nLen =0; i>=0; i--)
		{
			if (szVersion[i] == _T('.'))
			{
				if (nLen++ == 1)
				{
					szVersion[i] = 0;
					break;
				}
			}
		}
		sProductName += _T(" ");
		sProductName += szVersion;
	}
	else
	{
		sProductName = COemGuiApi::CalcProductName();
		CRect *pRect;
		if (m_RectMap.Lookup((WORD)IDC_TXT_PRODUCT,(void*&)pRect))
		{
			CWindowDC DC(pWnd);
			DC.SelectObject(&m_Font1);
			CSize sz = DC.GetTextExtent(sProductName);
			if (sz.cx > pRect->Width())
			{
				pRect->top -= (pRect->Height() >> 1);
				int nFind = sProductName.Find(_T("/"));
				if (nFind != -1)
				{
					nFind = sProductName.Find(_T(" "), nFind+2);
					if (nFind != -1)
					{
						sProductName.SetAt(nFind, _T('\n'));
					}
				}
			}
		}
	}

	pWnd->SetWindowText(sProductName);
	m_Font2.CreatePointFont(10*10, _T("Arial"));
	m_txtDate.CWnd::SetFont(&m_Font2); 
	m_txtTime.CWnd::SetFont(&m_Font2);
	
	CTime time;
	time = CTime::GetCurrentTime();
	CString str = time.Format(IDS_DATE_FORMAT);
//	CString	str = time.Format(_T("%x"));
	m_txtDate.SetWindowText(str);
	str = time.Format(_T("%H:%M:%S"));
//	str = time.Format(_T("%X"));
	m_txtTime.SetWindowText(str);

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////////////////
void CDlgShowOEM::OnBnClickedCloseDlgBar()
{
	theApp.GetMainFrame()->PostMessage(WM_USER, WM_CLOSE, (LPARAM)this);
}
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CDlgShowOEM::StretchElements()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////////////////
LRESULT CDlgShowOEM::OnUserMsg(WPARAM wParam, LPARAM)
{
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////
void CDlgShowOEM::OnBnClickedAppAbout()
{
	AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_APP_ABOUT);
}
/////////////////////////////////////////////////////////////////////////////////////////
LRESULT CDlgShowOEM::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return CDlgInView::WindowProc(message, wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CDlgShowOEM::OnEraseBkgnd(CDC* pDC)
{
	CDlgInView::OnEraseBkgnd(pDC);
	CRect	rc;
	if (GetLogoRect(&rc))
	{
		m_Logo.Draw(pDC, m_nImage, rc.TopLeft(), ILD_NORMAL);
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////////////////
int CDlgShowOEM::GetLogoRect(CRect*prc)
{
	int nRet = 0;
	CPoint	pt;
	CRect	rc;
	GetClientRect(&rc);
	pt.y = 0; //rc.bottom - sz.cy;

	IMAGEINFO ii;
	m_Logo.GetImageInfo(0, &ii);
	CSize sz = ((CRect*)&ii.rcImage)->Size();
	nRet = 1;

	int iCloseButtonSize = 20;
	if(rc.Width() > sz.cx + iCloseButtonSize)
	{
		//center logo if enough space
		//pt.x = (rc.right - sz.cx) / 2; origin code
		pt.x = (rc.right - (sz.cx - iCloseButtonSize)) / 2;
	}
	else
	{	
		//dont center logo, not enough space
		pt.x = (rc.right - sz.cx) / 2;
		pt.y = iCloseButtonSize;
	}

	//
	if (pt.x < 0) pt.x = 0;
	prc->SetRect(pt, pt + sz);
	return nRet;
}
/////////////////////////////////////////////////////////////////////////////////////////
void CDlgShowOEM::OnTimer(UINT nIDEvent)
{
	if (   nIDEvent == theApp.GetMainFrame()->GetOneSecTimerID()
		&& IsWindowVisible())
	{
		CTime time;
		time = CTime::GetCurrentTime();
		CString str = time.Format(_T("%H:%M:%S"));
		m_txtTime.SetWindowText(str);
		if (time.GetSecond() == 0)
		{
			str = time.Format(IDS_DATE_FORMAT);
			m_txtDate.SetWindowText(str);
		}
	}
	CDlgInView::OnTimer(nIDEvent);
}
/////////////////////////////////////////////////////////////////////////////////////////
void CDlgShowOEM::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CRect rc;
	GetLogoRect(&rc);
	if (rc.PtInRect(point))
	{
		AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_APP_ABOUT);
	}
	else
	{
		bool bCallDateTimeDlg = false;
		m_txtTime.GetWindowRect(&rc);
		ScreenToClient(&rc);
		if (rc.PtInRect(point))
		{
			bCallDateTimeDlg = true;
		}
		else
		{
			m_txtDate.GetWindowRect(&rc);
			ScreenToClient(&rc);
			if (rc.PtInRect(point))
			{
				bCallDateTimeDlg = true;
			}
		}
		if (bCallDateTimeDlg)
		{
			theApp.StartApplication(WAI_LAUNCHER, LAUNCHER_DATE_TIME_DLG);
		}
	}
	CDlgInView::OnLButtonDblClk(nFlags, point);
}
