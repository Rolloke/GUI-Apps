// VisionCoolBar.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Vision.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////
// CVisionCoolBar
//
IMPLEMENT_DYNAMIC(CVisionCoolBar, CCoolBar)

////////////////
// This is the virtual function you have to override to add bands
//
BOOL CVisionCoolBar::OnCreateBands()
{
	// Create tool bar
	if (!m_wndToolBar.Create(GetParent(),
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|
			CBRS_TOOLTIPS|CBRS_SIZE_DYNAMIC|CBRS_HIDE_INPLACE))
	{
		TRACE("Failed to create toolbar\n");
		return FALSE; // failed to create
	}
	
   m_wndToolBar.GetToolBarCtrl().SetButtonSize(CSize(22,22));
	m_wndToolBar.SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);

    m_ilHot.Create(IDB_HOT, 20, 0, RGB(255, 0, 255));
	m_wndToolBar.SetHotImageList(&m_ilHot);

	m_ilCold.Create(IDB_COLD, 20, 0, RGB(255, 0, 255));
	m_wndToolBar.SetImageList(&m_ilCold);

	m_wndToolBar.ModifyStyle(0, TBSTYLE_FLAT|TBSTYLE_TRANSPARENT);

	m_wndToolBar.SetButtons(NULL, 16);

	int i,b;
	i = 0;
	b = 0;
    // set up each toolbar button
	m_wndToolBar.SetButtonInfo(i++, ID_CONNECT, TBSTYLE_BUTTON|TBSTYLE_DROPDOWN, b++, 1);
	m_wndToolBar.SetButtonInfo(i++, ID_CONNECT_MAP, TBSTYLE_BUTTON, b++, 1);
	m_wndToolBar.SetButtonInfo(i++, ID_DISCONNECT, TBSTYLE_BUTTON|TBSTYLE_DROPDOWN, b++, 1);
	m_wndToolBar.SetButtonInfo(i++, ID_SEPARATOR, TBBS_SEPARATOR, 10/*Pixel Width*/, 1);
	
	m_wndToolBar.SetButtonInfo(i++, ID_VIEW_1_PLUS,TBSTYLE_BUTTON,b++,1);
	m_wndToolBar.SetButtonInfo(i++, ID_VIEW_1, TBSTYLE_BUTTON, b++, 1);
	m_wndToolBar.SetButtonInfo(i++, ID_VIEW_2_2, TBSTYLE_BUTTON, b++, 1);
	m_wndToolBar.SetButtonInfo(i++, ID_VIEW_3_3, TBSTYLE_BUTTON, b++, 1);
	m_wndToolBar.SetButtonInfo(i++, ID_VIEW_4_4, TBSTYLE_BUTTON, b++, 1);
	m_wndToolBar.SetButtonInfo(i++, ID_VIEW_FULLSCREEN, TBSTYLE_BUTTON, b++, 1);
	m_wndToolBar.SetButtonInfo(i++, ID_VIEW_ORIGINAL, TBSTYLE_BUTTON, b++, 1);
	m_wndToolBar.SetButtonInfo(i++, ID_SEPARATOR, TBBS_SEPARATOR, 10/*Pixel Width*/, 1);
	
	m_iSaveIndex = i;
	m_wndToolBar.SetButtonInfo(i++, ID_VIDEO_SAVE_ALL, TBSTYLE_BUTTON, b++, 1);
	
	m_wndToolBar.SetButtonInfo(i++, ID_SEQUENZE, TBSTYLE_BUTTON|TBSTYLE_DROPDOWN, b++, 1);
	m_wndToolBar.SetButtonInfo(i++, ID_SEPARATOR, TBBS_SEPARATOR, 10/*Pixel Width*/, 1);
	m_wndToolBar.SetButtonInfo(i++, ID_APP_ABOUT, TBSTYLE_BUTTON, b++, 1);

	// Get minimum size of bands
	CSize szHorz = m_wndToolBar.CalcDynamicLayout(-1, 0);	      // get min horz size
	CSize szVert = m_wndToolBar.CalcDynamicLayout(-1, LM_HORZ);	// get min vert size

	CSize sFixed = m_wndToolBar.CalcFixedLayout(FALSE,TRUE);

	// Band 1: Add toolbar band
	CRebarBandInfo rbbi;

	
	rbbi.fMask = RBBIM_STYLE|RBBIM_CHILD|RBBIM_CHILDSIZE|RBBIM_COLORS;
	rbbi.fStyle = 0;
	rbbi.hwndChild = m_wndToolBar;
	rbbi.cx = 0;
	rbbi.cxMinChild = sFixed.cx;//szHorz.cx+4;
	rbbi.cyMinChild = 40;
//#ifdef _DEBUG

	CWinXPThemes xp;
	if (xp.IsThemeActive())
	{
		rbbi.cyMinChild += 8;	       // +8 wegen XP-Look
	}

//#endif

	rbbi.clrFore = GetSysColor(COLOR_BTNTEXT);
	rbbi.clrBack = GetSysColor(COLOR_BTNFACE);
	if (!InsertBand(0, &rbbi))
	{
		return FALSE;
	}


	// Create the Logo
	CBitmap bm;
	bm.LoadBitmap(COemGuiApi::GetOEMLogoId());
	CSize bitmapSize;
	BITMAP bmInfo;
	VERIFY(bm.GetObject(sizeof(bmInfo), &bmInfo) == sizeof(bmInfo));
	CRect logorect(0,0,bmInfo.bmWidth,bmInfo.bmHeight);
	m_wndButton.Create(_T("Dummy"),WS_VISIBLE|WS_DISABLED|WS_CHILD|BS_OWNERDRAW,
						logorect,GetParent(),AFX_IDW_TOOLBAR+2);
	m_wndButton.LoadBitmaps(COemGuiApi::GetOEMLogoId());
	m_wndButton.SizeToContent();

	rbbi.fMask = RBBIM_STYLE|RBBIM_CHILD|RBBIM_CHILDSIZE;
	rbbi.fStyle = RBBS_FIXEDSIZE;
	rbbi.hwndChild = m_wndButton;
	rbbi.cxMinChild = logorect.Width();
	rbbi.cyMinChild = logorect.Height();


	if (!InsertBand(1, &rbbi))
	{
		return FALSE;
	}

   m_dwStyle |= CBRS_FLYBY; // Für Tooltips in der Statusbar
	return 0; // OK
}
void CVisionCoolBar::SetStore(BOOL bOn)
{
	UINT nID;
	UINT nStyle; 
	int iImage;

	m_wndToolBar.GetButtonInfo(m_iSaveIndex, nID, nStyle, iImage);
	iImage = bOn ? 13 : 10; 
	nID = bOn ? ID_VIDEO_SAVE_ALL_STOP : ID_VIDEO_SAVE_ALL;
	m_wndToolBar.SetButtonInfo(m_iSaveIndex, nID, nStyle, iImage, 1);
}


int CVisionCoolBar::HitTest()
{
   m_wndToolBar.m_nStateFlags &= ~statusSet;
   return m_wndToolBar.HitButton(true);
}
