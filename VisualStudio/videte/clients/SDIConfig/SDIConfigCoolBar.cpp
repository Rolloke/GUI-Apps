// SDIConfigCoolBar.cpp: implementation of the CSDIConfigCoolBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SDIConfig.h"
#include "SDIConfigCoolBar.h"

////////////////////////////////////////////////////////////////
// CSDIConfigCoolBar
//
IMPLEMENT_DYNAMIC(CSDIConfigCoolBar, CCoolBar)

////////////////
// This is the virtual function you have to override to add bands
//
BOOL CSDIConfigCoolBar::OnCreateBands()
{
	// Create tool bar
	if (!m_wndToolBar.Create(GetParent(),
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|
			CBRS_TOOLTIPS|CBRS_SIZE_DYNAMIC|CBRS_HIDE_INPLACE))
	{
		TRACE0("Failed to create toolbar\n");
		return FALSE; // failed to create
	}
	
    m_wndToolBar.GetToolBarCtrl().SetButtonSize(CSize(22,22));
	m_wndToolBar.SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);

    m_ilHot.Create(IDB_HOT, 20, 0, RGB(255, 0, 255));
	m_wndToolBar.SetHotImageList(&m_ilHot);

	m_ilCold.Create(IDB_COLD, 20, 0, RGB(255, 0, 255));
	m_wndToolBar.SetImageList(&m_ilCold);

	m_wndToolBar.ModifyStyle(0, TBSTYLE_FLAT|TBSTYLE_TRANSPARENT);
	m_wndToolBar.SetButtons(NULL, 10);

	int i,b;
	i = 0;
	b = 0;
    // set up each toolbar button
	m_wndToolBar.SetButtonInfo(i++, ID_FILE_NEW, TBSTYLE_BUTTON, b++);
	m_wndToolBar.SetButtonInfo(i++, ID_SEPARATOR, TBBS_SEPARATOR, 10/*Pixel Width*/);
	m_wndToolBar.SetButtonInfo(i++, ID_CONNECT, TBSTYLE_BUTTON, b++);
	m_wndToolBar.SetButtonInfo(i++, ID_DISCONNECT, TBSTYLE_BUTTON, b++);
	m_wndToolBar.SetButtonInfo(i++, ID_SEPARATOR, TBBS_SEPARATOR, 10/*Pixel Width*/);
	m_wndToolBar.SetButtonInfo(i++, ID_EXTERN_PROGRAMM , TBSTYLE_BUTTON, b++);
	m_wndToolBar.SetButtonInfo(i++, ID_SEPARATOR, TBBS_SEPARATOR, 10/*Pixel Width*/);
	m_wndToolBar.SetButtonInfo(i++, ID_CONFIGURE , TBSTYLE_BUTTON, b++);
	m_wndToolBar.SetButtonInfo(i++, ID_SEPARATOR, TBBS_SEPARATOR, 10/*Pixel Width*/);
	m_wndToolBar.SetButtonInfo(i++, ID_APP_ABOUT , TBSTYLE_BUTTON, b++);
	
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
	rbbi.clrFore = GetSysColor(COLOR_BTNTEXT);
	rbbi.clrBack = GetSysColor(COLOR_BTNFACE);
	if (!InsertBand(0, &rbbi))
	{
		WK_TRACE_ERROR("Cannot insert toolbar band\n");
		return FALSE;
	}


	// Create the Logo
	CBitmap bm;
	bm.LoadBitmap(COemGuiApi::GetOEMLogoId());
	CSize bitmapSize;
	BITMAP bmInfo;
	VERIFY(bm.GetObject(sizeof(bmInfo), &bmInfo) == sizeof(bmInfo));
	CRect logorect(0,0,bmInfo.bmWidth,bmInfo.bmHeight);
	m_wndButton.Create("Dummy",WS_VISIBLE|WS_DISABLED|WS_CHILD|BS_OWNERDRAW,
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
		WK_TRACE_ERROR("Cannot insert logo band\n");
		return FALSE;
	}
	return 0; // OK
}

