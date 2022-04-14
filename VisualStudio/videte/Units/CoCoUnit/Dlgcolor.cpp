/////////////////////////////////////////////////////////////////////////////
// PROJECT:		CoCoUnit
// FILE:		$Workfile: Dlgcolor.cpp $
// ARCHIVE:		$Archive: /Project/Units/CoCoUnit/Dlgcolor.cpp $
// CHECKIN:		$Date: 5.08.98 9:43 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 5.08.98 9:43 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "cpydata.h"
#include "ccoco.h"
#include "dlgcolor.h"
#include "message.h"

BOOL CALLBACK DlgColorCallback(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam);

CDlgColor* pDlgColorExt;

///////////////////////////////////////////////////////////////////////////
// CDlgColor::CDlgColor
CDlgColor::CDlgColor(CCoCo* const pCoCo, HWND hWndClient)
{
	m_bOK			= FALSE;
	m_bChanged		= FALSE;
	m_pCoCo			= pCoCo;
	m_hWndDlg		= NULL;
	m_hWndClient	= hWndClient;

#ifdef _DEBUG
	OutputDebugString("CDlgColor::CDlgColor\n");
#endif

	HINSTANCE hThisModule = AfxGetInstanceHandle();
	// Wird in der Fensterfunktion benötigt.
	::pDlgColorExt	= this;

	if (!CreateDialog(hThisModule, MAKEINTRESOURCE(IDD_DLG_COLOR), hWndClient, (DLGPROC)::DlgColorCallback))
	{
#ifdef _DEBUG
		OutputDebugString("Dialoghandle unhültig\n");
#endif
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
#ifdef _DEBUG
	OutputDebugString("CDlgColor::~CDlgColor\n");
#endif

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
	SetScrollRange(GetDlgItem(m_hWndDlg, IDC_SBRIGHTNESS), SB_CTL, COCO_MIN_BRIGHTNESS, COCO_MAX_BRIGHTNESS, FALSE);
	SetScrollRange(GetDlgItem(m_hWndDlg, IDC_SCONTRAST),   SB_CTL, COCO_MIN_CONTRAST,   COCO_MAX_CONTRAST,   FALSE);
	SetScrollRange(GetDlgItem(m_hWndDlg, IDC_SSATURATION), SB_CTL, COCO_MIN_SATURATION, COCO_MAX_SATURATION, FALSE);
	SetScrollRange(GetDlgItem(m_hWndDlg, IDC_SHUE),		   SB_CTL, COCO_MIN_HUE,		COCO_MAX_HUE,		 FALSE);

	UpdateDlg(FALSE);	
	ShowWindow(SW_HIDE);

	// Anfangseinstellungen wegen Cancel merken.	
	m_nSaveBrightness	= m_nBrightness;
	m_nSaveContrast		= m_nContrast;
	m_nSaveSaturation	= m_nSaturation;
	m_nSaveHue			= m_nHue;
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

	ShowWindow(SW_HIDE);
}

///////////////////////////////////////////////////////////////////////////
// CDlgColor::OnCancel
void CDlgColor::OnCancel()
{
	m_pCoCo->SetFeBrightness(m_nSaveBrightness);
	m_pCoCo->SetFeContrast(m_nSaveContrast);
	m_pCoCo->SetFeSaturation(m_nSaveSaturation);

	ShowWindow(SW_HIDE);
}

///////////////////////////////////////////////////////////////////////////
// CDlgColor::OnColorReset
void CDlgColor::OnUebernahme()
{
	// Anfangseinstellungen wegen Cancel merken.	
	m_nSaveBrightness	= m_nBrightness;
	m_nSaveContrast		= m_nContrast;
	m_nSaveSaturation	= m_nSaturation;

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
				m_pCoCo->SetFeBrightness(nValue);
				break;
		
 		case IDC_SCONTRAST:
				m_pCoCo->SetFeContrast(nValue);
	            break;
					
		case IDC_SSATURATION:
				m_pCoCo->SetFeSaturation(nValue);
				break;

		case IDC_SHUE:
				m_pCoCo->SetFeHue(nValue);
				break;
		
		default:
				break;
	} 
	
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

    int nB = m_pCoCo->GetFeBrightness();
    int nC = m_pCoCo->GetFeContrast();
    int nS = m_pCoCo->GetFeSaturation();
    int nH = m_pCoCo->GetFeHue();

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
}

///////////////////////////////////////////////////////////////////////////
// CDlgColor::ShowWindow
void CDlgColor::ShowWindow(BOOL bShow)
{
	m_bChanged	= FALSE;
	::EnableWindow(GetDlgItem(m_hWndDlg, IDC_UEBERNAHME), m_bChanged);

	::ShowWindow(m_hWndDlg, bShow);
}

/****************************************************************************************/
/* Rückruffunktion:DlgColor                                                             */
/****************************************************************************************/
BOOL CALLBACK CDlgColor::DlgColorCallback(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

BOOL CALLBACK DlgColorCallback(HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	if (!pDlgColorExt)
		return FALSE;

	return pDlgColorExt->OnMessage(hDlg, wMsg, wParam, lParam);
}
