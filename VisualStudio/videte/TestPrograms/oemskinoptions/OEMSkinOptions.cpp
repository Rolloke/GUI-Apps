// OEMSkinOptions.cpp : Definiert das Klassenverhalten für die Anwendung.
//

#include "stdafx.h"
#include "OEMSkinOptions.h"
#include "OEMSkinOptionsDlg.h"
#include ".\oemskinoptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define WINDOW_SYS_COLORS _T("WINDOW_SYS_COLORS")
#define SYS_COLORS_SAVED _T("Saved")
// COEMSkinOptionsApp

BEGIN_MESSAGE_MAP(COEMSkinOptionsApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
	ON_COMMAND(ID_FILE_RESTORE_SYS_COLORS, OnFileRestoreSysColors)
	ON_UPDATE_COMMAND_UI(ID_FILE_RESTORE_SYS_COLORS, OnUpdateFileRestoreSysColors)
END_MESSAGE_MAP()

extern AFX_DATA_IMPORT AUX_DATA afxData;

// COEMSkinOptionsApp-Erstellung

COEMSkinOptionsApp::COEMSkinOptionsApp()
{
	// TODO: Hier Code zur Konstruktion einfügen
	// Alle wichtigen Initialisierungen in InitInstance positionieren
}


// Das einzige COEMSkinOptionsApp-Objekt

COEMSkinOptionsApp theApp;


// COEMSkinOptionsApp Initialisierung

BOOL COEMSkinOptionsApp::InitInstance()
{
	CWinApp::InitInstance();

	SetRegistryKey(_T("Videte"));

	int nSaved = GetProfileInt(WINDOW_SYS_COLORS, SYS_COLORS_SAVED, 0);
	if (!nSaved)
	{
		CString str;
		for (int i=0; i<=COLOR_INFOBK; i++)
		{
			str.Format(_T("Color%d"), i);
			WriteProfileInt(WINDOW_SYS_COLORS, str, GetSysColor(i));
		}
		WriteProfileInt(WINDOW_SYS_COLORS, SYS_COLORS_SAVED, 1);
		nSaved = TRUE;
	}
	m_bSysColorsSaved = nSaved;

	CSkinDialog::SetButtonImages(SC_CLOSE    , (UINT)-1, 0, 0);
	CSkinDialog::SetButtonImages(SC_MAXIMIZE , (UINT)-1, 0, 0);
	CSkinDialog::SetButtonImages(SC_MINIMIZE , (UINT)-1, 0, 0);
	CSkinDialog::SetButtonImages(SC_MOUSEMENU, (UINT)-1, 0, 0, LoadIcon(IDR_MAINFRAME));
	CSkinDialog::LoadButtonImages();


	COLORREF cTextColor     = SKIN_COLOR_GREY_MEDIUM_LIGHT,
		     cBaseColor		= SKIN_COLOR_VIDETE_CI,
			 cTextHighlited = SKIN_COLOR_WHITE,
			 cTextDisabled  = SKIN_COLOR_GREY_MEDIUM_DARK,
			 cLineLight     = CSkinButton::ChangeBrightness(cBaseColor,  30, TRUE),
			 cLineDark      = CSkinButton::ChangeBrightness(cBaseColor, -30, TRUE),
			 cTextShaddow   = SKIN_COLOR_GREY_MEDIUM_DARK;

	CString sIniFile = m_pszHelpFilePath;
	int nFind = sIniFile.ReverseFind(_T('\\'));
	if (nFind != -1)
	{
		sIniFile = sIniFile.Left(nFind+1) + OEM_FILE;
	}

	CSkinButton::SetOEMProperties(GetPrivateProfileInt(OEM_DIALOG, OEM_CHANGE_ONLY_BRIGHTNESS, 0, sIniFile),
								  GetPrivateProfileInt(OEM_DIALOG, OEM_MAX_COLOR_RANGE, 64, sIniFile),
								  GetPrivateProfileInt(OEM_DIALOG, OEM_BRUSH_EFFECT_FACTOR, 5, sIniFile));

	CSkinDialog::SetOEMSettings(
		(STYLE_BACKGROUND)GetPrivateProfileInt(OEM_DIALOG, OEM_BACKGROUND_STYLE, StyleBackGroundColorChangeSimple, sIniFile),
		GetPrivateProfileInt(OEM_DIALOG, OEM_BASE_COLOR, cBaseColor, sIniFile),
		GetPrivateProfileInt(OEM_DIALOG, OEM_TEXT_COLOR, cTextColor, sIniFile), 
		GetPrivateProfileInt(OEM_DIALOG, OEM_USE_OEM_SKIN, 1, sIniFile));

	CSkinDialog::SetOEMColors(
		GetPrivateProfileInt(OEM_DIALOG, OEM_HILIGHT_COLOR, cLineLight, sIniFile),
		GetPrivateProfileInt(OEM_DIALOG, OEM_SHADOW_COLOR, cLineDark, sIniFile));
/*
	afxData.clrBtnFace     = CSkinButton::GetBaseColorG();
	afxData.clrBtnText     = CSkinButton::GetTextColorNormalG();
	afxData.clrBtnHilite   = CSkinDialog::GetHilightColor();
	afxData.clrBtnShadow   = CSkinDialog::GetShadowColor();
	afxData.clrWindowFrame = CSkinDialog::GetBaseColor();
*/
	CSkinButton::SetOEMColors(
		GetPrivateProfileInt(OEM_BUTTON, OEM_BASE_COLOR, cBaseColor, sIniFile),
		GetPrivateProfileInt(OEM_BUTTON, OEM_TEXT_COLOR, cTextColor, sIniFile),
		GetPrivateProfileInt(OEM_BUTTON, OEM_TEXT_COLOR_HILIGHTED, cTextHighlited, sIniFile),
		GetPrivateProfileInt(OEM_BUTTON, OEM_TEXT_COLOR_DISABLED, cTextDisabled, sIniFile), SKIN_COLOR_KEY);
	CSkinButton::SetOEMShape((enumSkinButtonShape)GetPrivateProfileInt(OEM_BUTTON, OEM_SHAPE, ShapeRect, sIniFile));
	CSkinButton::SetOEMSurface((enumSkinButtonSurface)GetPrivateProfileInt(OEM_BUTTON, OEM_SURFACE, SurfaceColorChangeSimple, sIniFile));

	CSkinStatic::SetOEMColors(
		GetPrivateProfileInt(OEM_STATIC, OEM_TEXT_COLOR, cTextColor, sIniFile),
		GetPrivateProfileInt(OEM_STATIC, OEM_BACKGROUND_COLOR, cBaseColor, sIniFile), 
		GetPrivateProfileInt(OEM_STATIC, OEM_HILIGHT_COLOR, cLineLight, sIniFile),
		GetPrivateProfileInt(OEM_STATIC, OEM_SHADOW_COLOR, cLineDark, sIniFile));

	CSkinSlider::SetOEMColors(
		GetPrivateProfileInt(OEM_SLIDER, OEM_BACKGROUND_COLOR, cBaseColor, sIniFile),
		GetPrivateProfileInt(OEM_SLIDER, OEM_TEXT_COLOR, cTextColor, sIniFile),
		GetPrivateProfileInt(OEM_SLIDER, OEM_HILIGHT_COLOR, cLineLight, sIniFile),
		GetPrivateProfileInt(OEM_SLIDER, OEM_SHADOW_COLOR, cLineDark, sIniFile));
	CSkinSlider::SetOEMShape((enumSkinButtonShape)GetPrivateProfileInt(OEM_SLIDER, OEM_SHAPE, ShapeRect, sIniFile));
	CSkinSlider::SetOEMSurface((enumSkinButtonSurface)GetPrivateProfileInt(OEM_SLIDER, OEM_SURFACE, SurfaceColorChangeSimple, sIniFile));

	CSkinGroupBox::SetOEMColors(
		GetPrivateProfileInt(OEM_GROUP_BOX, OEM_TEXT_COLOR, cTextColor, sIniFile),
		GetPrivateProfileInt(OEM_GROUP_BOX, OEM_TEXT_COLOR_DISABLED, cTextDisabled, sIniFile),
		GetPrivateProfileInt(OEM_GROUP_BOX, OEM_SHADOW_COLOR, cTextShaddow, sIniFile),
		GetPrivateProfileInt(OEM_GROUP_BOX, OEM_HILIGHT_COLOR, cLineLight, sIniFile),
		GetPrivateProfileInt(OEM_GROUP_BOX, OEM_SHADOW_COLOR, cLineDark, sIniFile));

	CSkinEdit::SetOEMColors(
		GetPrivateProfileInt(OEM_EDIT, OEM_BACKGROUND_COLOR, cBaseColor, sIniFile),
		GetPrivateProfileInt(OEM_EDIT, OEM_TEXT_COLOR, cTextColor, sIniFile));
	
	CSkinProgress::SetOEMColors(
		GetPrivateProfileInt(OEM_PROGRESS, OEM_BACKGROUND_COLOR, cBaseColor, sIniFile),
		GetPrivateProfileInt(OEM_PROGRESS, OEM_PROGRESS_COLOR, SKIN_COLOR_GOLD_METALLIC, sIniFile));

	CSkinTree::SetOEMColors(
		GetPrivateProfileInt(OEM_TREE, OEM_BACKGROUND_COLOR, cBaseColor, sIniFile),
		GetPrivateProfileInt(OEM_TREE, OEM_TEXT_COLOR, cTextColor, sIniFile), 
		GetPrivateProfileInt(OEM_TREE, OEM_LINE_COLOR, cTextColor, sIniFile));

	CSkinListCtrlX::SetOEMColors(
		GetPrivateProfileInt(OEM_LIST_CONTROL, OEM_BASE_COLOR, cBaseColor, sIniFile),
		GetPrivateProfileInt(OEM_LIST_CONTROL, OEM_TEXT_COLOR, cTextColor, sIniFile),
		GetPrivateProfileInt(OEM_LIST_CONTROL, OEM_TEXT_BACKGROUND_COLOR, cBaseColor, sIniFile),
		GetPrivateProfileInt(OEM_LIST_CONTROL, OEM_HEADER_BACKGROUND_COLOR, cBaseColor, sIniFile),
		GetPrivateProfileInt(OEM_LIST_CONTROL, OEM_HEADER_TEXT_COLOR, cTextColor, sIniFile));
	CSkinListCtrlX::SetOEMShape((enumSkinButtonShape)GetPrivateProfileInt(OEM_LIST_CONTROL, OEM_SHAPE, ShapeRect, sIniFile));
	CSkinListCtrlX::SetOEMSurface((enumSkinButtonSurface)GetPrivateProfileInt(OEM_LIST_CONTROL, OEM_SURFACE, SurfaceColorChangeSimple, sIniFile));

	CSkinComboBox::SetOEMColors(
		GetPrivateProfileInt(OEM_COMBO_BOX, OEM_BACKGROUND_COLOR, cBaseColor, sIniFile),
		GetPrivateProfileInt(OEM_COMBO_BOX, OEM_TEXT_COLOR, cTextColor, sIniFile));
	CSkinComboBox::SetOEMShape((enumSkinButtonShape)GetPrivateProfileInt(OEM_COMBO_BOX, OEM_SHAPE, ShapeRect, sIniFile));
	CSkinComboBox::SetOEMSurface((enumSkinButtonSurface)GetPrivateProfileInt(OEM_COMBO_BOX, OEM_SURFACE, SurfaceColorChangeSimple, sIniFile));

	CSkinScrollBar::SetOEMColors(
		GetPrivateProfileInt(OEM_SCROLLBAR, OEM_BACKGROUND_COLOR, cBaseColor, sIniFile),
		GetPrivateProfileInt(OEM_SCROLLBAR, OEM_BUTTON_FACE, cBaseColor, sIniFile));
	CSkinScrollBar::SetOEMShape((enumSkinButtonShape)GetPrivateProfileInt(OEM_SCROLLBAR, OEM_SHAPE, ShapeRect, sIniFile));
	CSkinScrollBar::SetOEMSurface((enumSkinButtonSurface)GetPrivateProfileInt(OEM_SCROLLBAR, OEM_SURFACE, SurfaceColorChangeSimple, sIniFile));

	CSkinMenu::SetOEMColors(
		GetPrivateProfileInt(OEM_MENU, OEM_BACKGROUND_COLOR, cBaseColor, sIniFile),
		GetPrivateProfileInt(OEM_MENU, OEM_TEXT_COLOR, cTextColor, sIniFile));

	afxData.clrBtnFace		= GetPrivateProfileInt(OEM_SYSTEM_COLORS, OEM_BUTTON_FACE, GetSysColor(COLOR_BTNFACE), sIniFile);
	afxData.clrBtnHilite	= GetPrivateProfileInt(OEM_SYSTEM_COLORS, OEM_HILIGHT, GetSysColor(COLOR_BTNHIGHLIGHT), sIniFile);
	afxData.clrBtnShadow	= GetPrivateProfileInt(OEM_SYSTEM_COLORS, OEM_SHADOW, GetSysColor(COLOR_BTNSHADOW), sIniFile);
	afxData.clrBtnText		= GetPrivateProfileInt(OEM_SYSTEM_COLORS, OEM_BUTTON_TEXT, GetSysColor(COLOR_BTNTEXT), sIniFile);
	afxData.clrWindowFrame	= GetPrivateProfileInt(OEM_SYSTEM_COLORS, OEM_WINDOW_FRAME, GetSysColor(COLOR_WINDOWFRAME), sIniFile);

	if (   GetSysColor(COLOR_BTNSHADOW) != cLineDark
		&& GetSysColor(COLOR_BTNHIGHLIGHT) != cLineLight)
	{
		if (CSkinDialog::UseOEMSkin() && GetPrivateProfileInt(OEM_DIALOG, OEM_CHANGE_SYS_COLORS, 0, sIniFile))
		{
		}
	}

	COEMSkinOptionsDlg dlg;
	dlg.m_sInifile = sIniFile;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Fügen Sie hier Code ein, um das Schließen des
		//  Dialogfelds über OK zu steuern
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Fügen Sie hier Code ein, um das Schließen des
		//  Dialogfelds über "Abbrechen" zu steuern
	}

	// Da das Dialogfeld geschlossen wurde, FALSE zurückliefern, so dass wir die
	//  Anwendung verlassen, anstatt das Nachrichtensystem der Anwendung zu starten.
	return FALSE;
}

void COEMSkinOptionsApp::OnFileRestoreSysColors()
{
	if (m_bSysColorsSaved)
	{
		CString str;
		COLORREF cColors[COLOR_INFOBK+1];
		int      nColors[COLOR_INFOBK+1];
		for (int i=0; i<=COLOR_INFOBK; i++)
		{
			str.Format(_T("Color%d"), i);
			cColors[i] = GetProfileInt(WINDOW_SYS_COLORS, str, -1);
			if (cColors[i] != -1)
			{
				nColors[i] = i;
			}
			else
			{
				nColors[i] = -1;
				i--;
				break;
			}
		}
		SetSysColors(i, nColors, cColors);
	}
}

void COEMSkinOptionsApp::OnUpdateFileRestoreSysColors(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_bSysColorsSaved);
}
