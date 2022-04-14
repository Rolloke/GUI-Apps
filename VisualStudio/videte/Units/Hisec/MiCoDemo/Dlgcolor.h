#ifndef __dlgcolor_h__
#define __dlgcolor_h__

#define GET_WM_COMMAND_CMD(wp, lp) 	HIWORD(wp)
#define GET_WM_COMMAND_ID(wp, lp) 	LOWORD(wp)
#define GET_WM_COMMAND_HWND(wp, lp) ((HWND)lp)
#define GET_THUMBPOSITION(wp, lp)   ((int)(short)HIWORD(wp))

class CMiCo;

class CDlgColor
{
public:

	CDlgColor(CMiCo* const pMiCo, HWND hWndClient);

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

	int			Range(int nVal, int nMin, int nMax);

	BOOL		m_bOK;
	BOOL		m_bChanged;					// FALSE -> Keine Änderung vorgenommen
	HWND		m_hWndDlg;					// Fensterhandle des Dialogs
	HWND		m_hWndClient;				// Fensterhandle des Clients
	CMiCo*		m_pMiCo;

	// Aktuelle Einstellung
	int			m_nBrightness;              // Helligkeit
	int			m_nContrast;                // Kontrast
	int			m_nSaturation;              // Farbsättigung
	int			m_nHue;						// Phase
	
	// Anfangseinstellung
	int			m_nSaveBrightness;          // Helligkeit
	int			m_nSaveContrast;            // Kontrast
	int			m_nSaveSaturation;          // Farbsättigung
	int			m_nSaveHue;					// Phase

	// API-Messageparameter.
	UINT		m_uMsg;
	WPARAM		m_wParam;
	LPARAM		m_lParam;
	FARPROC		m_dlgprc; 
};

#endif // __dlgcolor_h__

