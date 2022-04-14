/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: Dlgcolor.h $
// ARCHIVE:		$Archive: /Project/Units/Savic/SavicDll/Dlgcolor.h $
// CHECKIN:		$Date: 2.07.02 13:35 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 2.07.02 9:17 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __DLGCOLOR_H__
#define __DLGCOLOR_H__

#define GET_WM_COMMAND_CMD(wp, lp) 	HIWORD(wp)
#define GET_WM_COMMAND_ID(wp, lp) 	LOWORD(wp)
#define GET_WM_COMMAND_HWND(wp, lp) ((HWND)lp)
#define GET_THUMBPOSITION(wp, lp)   ((int)(short)HIWORD(wp))

class CCodec;

class CDlgColor
{
public:

	CDlgColor(CCodec* const pCodec);

	~CDlgColor();
	
	BOOL		CALLBACK DlgColorCallback(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam);
	BOOL		IsValid();

	BOOL		OnMessage(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void		UpdateDlg(BOOL bShow);
	void		ShowWindow(BOOL bShow);

private:
  	BOOL		OnInitDialog();
	BOOL		OnColorHScroll();
	BOOL		OnWMCommand();

	void		OnOk();
	void		OnCancel();
	void		OnUebernahme();
	void		OnMDConfiguration();

	void		SetHScale(WORD wHScale);
	WORD		GetHScale();

	void		SetVScale(WORD wVScaleRatio);
	WORD		GetVScale();

	int			Range(int nVal, int nMin, int nMax);

	BOOL		m_bOK;
	BOOL		m_bChanged;					// FALSE -> Keine Änderung vorgenommen
	HWND		m_hWndDlg;					// Fensterhandle des Dialogs
	CCodec*		m_pCodec;

	// Aktuelle Einstellung
	int			m_nBrightness;              // Helligkeit
	int			m_nContrast;                // Kontrast
	int			m_nSaturation;              // Farbsättigung
	int			m_nHue;						// Phase
	int			m_nHScale;					// Horizontale Scalierung	
	int			m_nVScale;					// Vertikale Scalierung	

	// Anfangseinstellung
	int			m_nSaveBrightness;          // Helligkeit
	int			m_nSaveContrast;            // Kontrast
	int			m_nSaveSaturation;          // Farbsättigung
	int			m_nSaveHue;					// Phase
	int			m_nSaveHScale;				// Horizontale Skalierung	
	int			m_nSaveVScale;				// Vertiklae Skalierung

	// API-Messageparameter.
	UINT		m_uMsg;
	WPARAM		m_wParam;
	LPARAM		m_lParam;
	FARPROC		m_dlgprc; 
};

#endif // __DLGCOLOR_H__

