/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: Dlgcolor.cpp $
// ARCHIVE:		$Archive: /Project/Units/SaVic/SavicDll/Dlgcolor.cpp $
// CHECKIN:		$Date: 20.06.03 12:01 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 20.06.03 11:35 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "CCodec.h"
#include "DlgColor.h"

extern HINSTANCE g_hInstance;

BOOL CALLBACK DlgColorCallback(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam);

CDlgColor* pDlgColorExt;

///////////////////////////////////////////////////////////////////////////
// CDlgColor::CDlgColor
CDlgColor::CDlgColor(CCodec* const pCodec)
{    
	m_bOK			= FALSE;
	m_bChanged		= FALSE;
	m_pCodec		= pCodec;
	m_hWndDlg		= NULL;

	// Wird in der Fensterfunktion benötigt.
	::pDlgColorExt	= this;

	if (!CreateDialog(g_hInstance, MAKEINTRESOURCE(IDD_DLG_COLOR),
		AfxGetMainWnd()->m_hWnd,
		(DLGPROC)::DlgColorCallback))
	{
		ML_TRACE_ERROR(_T("CDlgColor::CDlgColor\tDialoghandle ungültig\n"));
		return;
	}

	m_bOK = TRUE;

	return;
}

///////////////////////////////////////////////////////////////////////////
// CDlgColor::IsValid
BOOL CDlgColor::IsValid()
{
	return m_bOK;
}

///////////////////////////////////////////////////////////////////////////
// CDlgColor::~CDlgColor
CDlgColor::~CDlgColor()
{
	DestroyWindow(m_hWndDlg);

	m_bOK = FALSE;
}

///////////////////////////////////////////////////////////////////////////
// CDlgColor::OnMessage
BOOL CDlgColor::OnMessage(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_hWndDlg		= hDlg;
	m_wParam	= wParam;
	m_lParam	= lParam;
	m_uMsg		= uMsg;
	
	switch (uMsg)
	{
		case WM_INITDIALOG:
				OnInitDialog();
				break;
		case WM_HSCROLL:
				OnColorHScroll();
				break;
		case WM_COMMAND:
				OnWMCommand();
				break;
	}
	return 0;
}				

///////////////////////////////////////////////////////////////////////////
// CDlgColor::OnInitDialog
BOOL CDlgColor::OnInitDialog()
{
	/*** Wertebereich der Schieber setzen ***/
	SetScrollRange(GetDlgItem(m_hWndDlg, IDC_SBRIGHTNESS), SB_CTL, MIN_BRIGHTNESS,	MAX_BRIGHTNESS, FALSE);
	SetScrollRange(GetDlgItem(m_hWndDlg, IDC_SCONTRAST),   SB_CTL, MIN_CONTRAST,	MAX_CONTRAST,   FALSE);
	SetScrollRange(GetDlgItem(m_hWndDlg, IDC_SSATURATION), SB_CTL, MIN_SATURATION,	MAX_SATURATION,	FALSE);
	SetScrollRange(GetDlgItem(m_hWndDlg, IDC_SHUE),		   SB_CTL, MIN_HUE,			MAX_HUE,		FALSE);
	SetScrollRange(GetDlgItem(m_hWndDlg, IDC_SHSCALE),	   SB_CTL, 0,				768,			FALSE);
	SetScrollRange(GetDlgItem(m_hWndDlg, IDC_SVSCALE),	   SB_CTL, 0,				576,			FALSE);

	UpdateDlg(FALSE);	
	ShowWindow(SW_HIDE);

	// Anfangseinstellungen wegen Cancel merken.	
	m_nSaveBrightness	= m_nBrightness;
	m_nSaveContrast		= m_nContrast;
	m_nSaveSaturation	= m_nSaturation;
	m_nSaveHue			= m_nHue;
	m_nSaveHScale		= m_nHScale;
	m_nSaveVScale		= m_nVScale;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////
// CDlgColor::OnWMCommand
BOOL CDlgColor::OnWMCommand()
{
	switch (LOWORD(m_wParam))
	{
		case IDOK:
				OnOk();
		    	break;
        
        case IDCANCEL:
				OnCancel();
				break;
            
        case IDC_UEBERNAHME:
				OnUebernahme();
                break;	
	}
	
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////
// CDlgColor::OnOk
void CDlgColor::OnOk()
{
	// Anfangseinstellungen wegen Cancel merken.	
	m_nSaveBrightness	= m_nBrightness;
	m_nSaveContrast		= m_nContrast;
	m_nSaveSaturation	= m_nSaturation;
	m_nSaveHue			= m_nHue;
	m_nSaveHScale		= m_nHScale;
	m_nSaveVScale		= m_nVScale;

	ShowWindow(SW_HIDE);
}

///////////////////////////////////////////////////////////////////////////
// CDlgColor::OnCancel
void CDlgColor::OnCancel()
{
	if (m_pCodec)
	{
		m_pCodec->SetBrightness(VIDEO_SOURCE_CURRENT,	m_nSaveBrightness, TRUE);
		m_pCodec->SetContrast(VIDEO_SOURCE_CURRENT,		m_nSaveContrast, TRUE);
		m_pCodec->SetSaturation(VIDEO_SOURCE_CURRENT,	m_nSaveSaturation, TRUE);
		m_pCodec->SetHue(VIDEO_SOURCE_CURRENT,			m_nSaveHue, TRUE);
		SetHScale(m_nSaveHScale);
		SetVScale(m_nSaveVScale);
	}

	ShowWindow(SW_HIDE);
	UpdateDlg(FALSE);
}

///////////////////////////////////////////////////////////////////////////
// CDlgColor::OnColorReset
void CDlgColor::OnUebernahme()
{
	// Anfangseinstellungen wegen Cancel merken.	
	m_nSaveBrightness	= m_nBrightness;
	m_nSaveContrast		= m_nContrast;
	m_nSaveSaturation	= m_nSaturation;
	m_nSaveHScale		= m_nHScale;
	m_nSaveVScale		= m_nVScale;

	ShowWindow(SW_SHOW);
}

///////////////////////////////////////////////////////////////////////////
// CDlgColor::OnColorHScroll
BOOL CDlgColor::OnColorHScroll()
{
	int           nCtrlID;
	int           nValue;
    int           nMinRange, nMaxRange;
    HWND          hCtrl;

	m_bChanged	= TRUE;
	::EnableWindow(GetDlgItem(m_hWndDlg, IDC_UEBERNAHME), m_bChanged);

    /*** Schieber-Handle holen ***/
	hCtrl = GET_WM_COMMAND_HWND(m_wParam, m_lParam);

	/*** Schieber-ID holen ***/
    nCtrlID = GetDlgCtrlID(hCtrl);
    
    /*** Momentane Schieberposition holen ***/
	nValue = GetScrollPos(hCtrl, SB_CTL);
    
    /*** Minimale und Maximale Position holen ***/
    GetScrollRange(hCtrl, SB_CTL, &nMinRange, &nMaxRange);
    
	/*** Neue Schieberposition bestimmen ***/	
    switch(GET_WM_COMMAND_ID(m_wParam, m_lParam))
    {
        case SB_PAGERIGHT:
                nValue += 8;
                break;
                
        case SB_LINERIGHT:
                nValue += 1;
                break;
	        
        case SB_PAGELEFT:
                nValue -= 8;
        		break;
        		
        case SB_LINELEFT:
                nValue -= 1;
                break;
       
        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:
				nValue = GET_THUMBPOSITION(m_wParam, m_lParam);
				break;
	
		default:
				return	FALSE;
	}
	
	/*** Schieberwert innerhalb der vorgegebenen Grenzen ? ***/    
    nValue = Range(nValue, nMinRange, nMaxRange);

	/*** Schieber setzen, Zahlenwerte ausgeben und Parameter an DLL senden ***/ 	
	switch (nCtrlID)
	{
		case IDC_SBRIGHTNESS:
				if (m_pCodec)
					m_pCodec->SetBrightness(VIDEO_SOURCE_CURRENT, nValue, TRUE);
				break;
		
 		case IDC_SCONTRAST:
				if (m_pCodec)
					m_pCodec->SetContrast(VIDEO_SOURCE_CURRENT, nValue, TRUE);
	            break;
					
		case IDC_SSATURATION:
				if (m_pCodec)
					m_pCodec->SetSaturation(VIDEO_SOURCE_CURRENT, nValue, TRUE);
				break;

		case IDC_SHUE:
				if (m_pCodec)
					m_pCodec->SetHue(VIDEO_SOURCE_CURRENT, nValue, TRUE);
				break;

		case IDC_SHSCALE:
				SetHScale(nValue);
				break;
		
		case IDC_SVSCALE:
				SetVScale(nValue);
				break;

		default:
				break;
	} 

	UpdateDlg(TRUE);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////
// CDlgColor::Range
int CDlgColor::Range(int nVal, int nMin, int nMax)
{
	int nRet = max(nVal, nMin);
	return min(nRet, nMax); 
}
               
///////////////////////////////////////////////////////////////////////////
// CDlgColor::UpdateDlg
void CDlgColor::UpdateDlg(BOOL bShow)
{  

    int nB		= (MAX_BRIGHTNESS	- MIN_BRIGHTNESS)	/ 2;
    int nC		= (MAX_CONTRAST		- MIN_CONTRAST)		/ 2;
    int nS		= (MAX_SATURATION	- MIN_SATURATION)	/ 2;
    int nH		= (MAX_HUE			- MIN_HUE)			/ 2;
    int nHSc	= (MAX_HSCALE		- MIN_HSCALE)		/ 2;
    int nVSc	= (MAX_VSCALE		- MIN_VSCALE)		/ 2;
 
	if (m_pCodec)
	{
		nB	= m_pCodec->GetBrightness(VIDEO_SOURCE_CURRENT);
		nC	= m_pCodec->GetContrast(VIDEO_SOURCE_CURRENT);
		nS	= m_pCodec->GetSaturation(VIDEO_SOURCE_CURRENT);
		nH	= m_pCodec->GetHue(VIDEO_SOURCE_CURRENT);
		nHSc= GetHScale();
		nVSc= GetVScale();
	}	
	
	if (nB != m_nBrightness)
	{
		m_nBrightness = nB;
	    SetScrollPos(GetDlgItem(m_hWndDlg, IDC_SBRIGHTNESS), SB_CTL, m_nBrightness, bShow);
		SetDlgItemInt(m_hWndDlg, IDC_VBRIGHTNESS,	m_nBrightness, TRUE);
	}
	if (nC != m_nContrast)
	{
		m_nContrast = nC;
	    SetScrollPos(GetDlgItem(m_hWndDlg, IDC_SCONTRAST),   SB_CTL, m_nContrast,   bShow);
		SetDlgItemInt(m_hWndDlg, IDC_VCONTRAST,	m_nContrast,   TRUE);
	}
	if (nS != m_nSaturation)
	{
		m_nSaturation = nS;
	    SetScrollPos(GetDlgItem(m_hWndDlg, IDC_SSATURATION), SB_CTL, m_nSaturation, bShow);
		SetDlgItemInt(m_hWndDlg, IDC_VSATURATION,	m_nSaturation, TRUE);  
	}
	if (nH != m_nHue)
	{
		m_nHue = nH;
	    SetScrollPos(GetDlgItem(m_hWndDlg, IDC_SHUE),		 SB_CTL, m_nHue, bShow);
		SetDlgItemInt(m_hWndDlg, IDC_VHUE,	m_nHue, TRUE);  
	}
	if (nHSc != m_nHScale)
	{
		m_nHScale = nHSc;
	    SetScrollPos(GetDlgItem(m_hWndDlg,	IDC_SHSCALE),		 SB_CTL, m_nHScale, bShow);
		SetDlgItemInt(m_hWndDlg, IDC_VHSCALE,	m_nHScale, TRUE);  
	}
	if (nVSc != m_nVScale)
	{
		m_nVScale = nVSc;
	    SetScrollPos(GetDlgItem(m_hWndDlg,	IDC_SVSCALE),		 SB_CTL, m_nVScale, bShow);
		SetDlgItemInt(m_hWndDlg, IDC_VVSCALE,	m_nVScale, TRUE);  
	}
}

///////////////////////////////////////////////////////////////////////////
// CDlgColor::ShowWindow
void CDlgColor::ShowWindow(BOOL bShow)
{
	m_bChanged	= FALSE;
	::EnableWindow(GetDlgItem(m_hWndDlg, IDC_UEBERNAHME), m_bChanged);
	::ShowWindow(m_hWndDlg, bShow);
}

///////////////////////////////////////////////////////////////////////////
void CDlgColor::SetHScale(WORD wHScale)
{
	if (m_pCodec)
	{
		m_pCodec->SetHActive(wHScale);

		if (m_pCodec->GetVideoFormat() == VFMT_PAL_CCIR)
			m_pCodec->SetFeHScale((int)(((922.0/(float)wHScale) - 1.0) * 4096));
		else if (m_pCodec->GetVideoFormat() == VFMT_NTSC_CCIR)
			m_pCodec->SetFeHScale((int)(((754.0/(float)wHScale) - 1.0) * 4096));
		else
			ML_TRACE_ERROR(_T("SetHScale: Unknown Videoformat\n"));
	}
}

///////////////////////////////////////////////////////////////////////////
WORD CDlgColor::GetHScale()
{
	if (m_pCodec)
	{
		WORD wHScale = 0;
		if (m_pCodec->GetFeHScale(wHScale))
		{

			TRACE(_T("GetFeHScale=0x%04x\n"), wHScale);
			if (m_pCodec->GetVideoFormat() == VFMT_PAL_CCIR)
				return (WORD)(922.0/((float)wHScale/4096.0 + 1.0));
			else if (m_pCodec->GetVideoFormat() == VFMT_NTSC_CCIR)
				return (WORD)(754.0/((float)wHScale/4096.0 + 1.0));
			else
				ML_TRACE_ERROR(_T("GetHScale: Unknown Videoformat\n"));
		}
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////
void CDlgColor::SetVScale(WORD wVScale)
{
	if (m_pCodec)
	{
		float fVScaleRatio = 576.0/(float)wVScale;	// For both fields PAL Square
	//	float fVScaleRatio = 288.0/(float)wVScale;	// For one field   PAL Square
		m_pCodec->SetFeVScale((WORD)((0x10000 - (fVScaleRatio-1.0)*512.0)) & 0x1fff);
		TRACE(_T("SetVScale=0x%04x\n"), (WORD)((0x10000 - (fVScaleRatio-1.0)*512.0)) & 0x1fff);
	}
}

///////////////////////////////////////////////////////////////////////////
WORD CDlgColor::GetVScale()
{
	if (m_pCodec)
	{
		WORD wVScale = 0;
		if (m_pCodec->GetFeVScale(wVScale))
		{
			float fVScaleRatio = ((0x10000 - wVScale)&0x1fff)/(float)512.0+1.0; 
			TRACE(_T("GetVScale=0x%04x (VRatio=%f/1)\n"), wVScale, fVScaleRatio);
			return (WORD)576/(fVScaleRatio);  // For both fields PAL Square
//			return (WORD)288/(fVScaleRatio);  // For one field   PAL Square

		}
	}
	return 0;
}


/****************************************************************************************/
/* Rückruffunktion:DlgColor                                                             */
/****************************************************************************************/
BOOL CALLBACK CDlgColor::DlgColorCallback(HWND /*hDlg*/, UINT /*wMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	return TRUE;
}

BOOL CALLBACK DlgColorCallback(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	if (!pDlgColorExt)
		return FALSE;

	return pDlgColorExt->OnMessage(hDlg, wMsg, wParam, lParam);
}
