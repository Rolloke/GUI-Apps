/////////////////////////////////////////////////////////////////////////////
// PROJECT:		SaVicDll
// FILE:		$Workfile: CMDConfigDlg.cpp $
// ARCHIVE:		$Archive: /Project/Units/SaVic/SavicDll/CMDConfigDlg.cpp $
// CHECKIN:		$Date: 20.06.03 12:01 $
// VERSION:		$Revision: 6 $
// LAST CHANGE:	$Modtime: 20.06.03 11:35 $
// BY AUTHOR:	$Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CMDConfigDlg.h"
#include "CMotionDetection.h"
#include "CCodec.h"
#include "PictureDef.h"


/////////////////////////////////////////////////////////////////////////////
// CMDConfigDlg dialog


CMDConfigDlg::CMDConfigDlg(CMotionDetection* pMD, CCodec* pCodec, CWnd* pParent /*=NULL*/)
	: CMDConfigDlgBase(pMD, pCodec, pParent, CMDConfigDlg::IDD)
{
	//{{AFX_DATA_INIT(CMDConfigDlg)
	//}}AFX_DATA_INIT

	CDialog::Create(IDD, pParent);

	CWnd* pWndDlg	= this;
	CWnd* pWndInlay = GetDlgItem(IDC_INLAY_WND);
	CWnd* pWndMask	= GetDlgItem(IDC_DIFF_WND);

	pWndDlg->GetClientRect(m_rcDlg); 
	pWndDlg->ClientToScreen(m_rcDlg);
	pWndInlay->GetWindowRect(m_rcInlay);
	pWndMask->GetWindowRect(m_rcMask);
	
	m_nXInlayOffset = m_rcInlay.left - m_rcDlg.left;
	m_nYInlayOffset = m_rcInlay.top  - m_rcDlg.top;

	m_nXMaskOffset = m_rcMask.left - m_rcDlg.left;
	m_nYMaskOffset = m_rcMask.top  - m_rcDlg.top;
}

/////////////////////////////////////////////////////////////////////////////
CMDConfigDlg::~CMDConfigDlg()
{
	DestroyWindow();
}

void CMDConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CMDConfigDlgBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMDConfigDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMDConfigDlg, CMDConfigDlgBase)
	//{{AFX_MSG_MAP(CMDConfigDlg)
	ON_BN_CLICKED(IDC_CLEAR_MASK, OnClearMask)
	ON_BN_CLICKED(IDC_INVERT_PERMANENT_MASK, OnInvertPermanentMask)
	ON_BN_CLICKED(IDC_INVERT_ADAPTIVE_MASK, OnInvertAdaptiveMask)
	ON_BN_CLICKED(IDC_SAVE_MASK, OnSaveMask)
	ON_CBN_SELENDOK(IDC_COMBO1, OnSelectCamera)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_LOAD_MASK, OnLoadMask)
	ON_BN_CLICKED(IDC_HIDE_MASK, OnHideMask)
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_HISTOGRAMM, OnHistogramm)
	ON_CBN_SELENDOK(IDC_SELECT_MASK_SENSITIVITY, OnSelectMaskSensitivity)
	ON_CBN_SELENDOK(IDC_SELECT_ALARM_SENSITIVITY, OnSelectAlarmSensitivity)
	ON_BN_CLICKED(IDC_FILTER, OnFilter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMDConfigDlg message handlers
/////////////////////////////////////////////////////////////////////////////
CWnd* CMDConfigDlg::GetInlayWnd()
{
	return GetDlgItem(IDC_INLAY_WND);
}

/////////////////////////////////////////////////////////////////////////////
CWnd* CMDConfigDlg::GetDiffWnd()
{
	return GetDlgItem(IDC_DIFF_WND);
}

/////////////////////////////////////////////////////////////////////////////
void CMDConfigDlg::OnClearMask() 
{
	GetDlgItem(IDC_CLEAR_MASK)->EnableWindow(FALSE);
	if (m_pMD)
		m_pMD->ClearMask(m_wSubID);
	GetDlgItem(IDC_CLEAR_MASK)->EnableWindow(TRUE);
	GetDlgItem(IDC_SAVE_MASK)->EnableWindow(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
void CMDConfigDlg::OnInvertPermanentMask() 
{
	GetDlgItem(IDC_INVERT_PERMANENT_MASK)->EnableWindow(FALSE);
	if (m_pMD)
		m_pMD->InvertPermanentMask(m_wSubID);
	GetDlgItem(IDC_INVERT_PERMANENT_MASK)->EnableWindow(TRUE);
	GetDlgItem(IDC_SAVE_MASK)->EnableWindow(TRUE);
}


/////////////////////////////////////////////////////////////////////////////
void CMDConfigDlg::OnInvertAdaptiveMask() 
{
	GetDlgItem(IDC_INVERT_ADAPTIVE_MASK)->EnableWindow(FALSE);
	if (m_pMD)
		m_pMD->InvertAdaptiveMask(m_wSubID);
	GetDlgItem(IDC_INVERT_ADAPTIVE_MASK)->EnableWindow(TRUE);
	GetDlgItem(IDC_SAVE_MASK)->EnableWindow(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
void CMDConfigDlg::OnSaveMask() 
{
	if (m_pMD)
	{
		m_pMD->SaveMask(m_wSubID,AM_PERMANENT);
		m_pMD->SaveMask(m_wSubID,AM_ADAPTIVE);
	}
	GetDlgItem(IDC_SAVE_MASK)->EnableWindow(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
void CMDConfigDlg::OnOK() 
{
	// Alle Masken speichern.
	for (int nI = 0; nI < MAX_CAMERAS; nI++)
	{
		if (m_pMD)
		{
			m_pMD->SaveMask(nI, AM_PERMANENT);
			m_pMD->SaveMask(nI, AM_ADAPTIVE);
		}
	}
	HideConfigDlg();
}

/////////////////////////////////////////////////////////////////////////////
void CMDConfigDlg::OnCancel() 
{
	// Alle Masken laden.
	for (int nI = 0; nI < MAX_CAMERAS; nI++)
	{
		if (m_pMD)
			m_pMD->LoadMask(nI);
	}
	HideConfigDlg();
}

/////////////////////////////////////////////////////////////////////////////
void CMDConfigDlg::OnSelectCamera() 
{
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_SELECT_CAMERA);

	int nSel = pCombo->GetCurSel();	
	if (nSel != CB_ERR )
	{
		m_wSubID	= nSel;
		SOURCE_SELECT_STRUCT SrcSel;
		SrcSel.wSource			= m_wSubID;
		SrcSel.wFormat			= ENCODER_RES_HIGH;
		SrcSel.dwBPF			= 24000;
		SrcSel.dwProzessID		= 0;
		SrcSel.dwPics			= (DWORD)-1;
		SrcSel.wCompressionType	= COMPRESSION_YUV_422;
		if (m_pCodec)
			m_pCodec->SetInputSource(SrcSel);
	
		EnableDlgCtrl();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMDConfigDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	int		nX		= point.x;
	int		nY		= point.y;
	int		nX1		= 0;
	int		nY1		= 0;
	BOOL	bInlay	= FALSE;
	BOOL	bMask	= FALSE;

	nX1 = nX - m_nXInlayOffset;
	nY1 = nY - m_nYInlayOffset;

	if ((nX1 >= 0) &&
		(nX1 <= m_rcInlay.Width()) &&
		(nY1 >= 0) &&
		(nY1 <= m_rcInlay.Height()))
		bInlay = TRUE;

	nX1 = nX - m_nXMaskOffset;
	nY1 = nY - m_nYMaskOffset;

	if ((nX1 >= 0) &&
		(nX1 <= m_rcMask.Width()) &&
		(nY1 >= 0) &&
		(nY1 <= m_rcMask.Height()))
		bMask = TRUE;

	
	if (bInlay || bMask)
	{
		if (nFlags & MK_LBUTTON)
		{
			if (bInlay)
				m_pMD->SetPixelInMask(m_wSubID, nX - m_nXInlayOffset, nY - m_nYInlayOffset, TRUE);
			if (bMask)
				m_pMD->SetPixelInMask(m_wSubID, nX - m_nXMaskOffset, nY - m_nYMaskOffset, TRUE);
			GetDlgItem(IDC_SAVE_MASK)->EnableWindow(TRUE);
		}

		if (nFlags & MK_RBUTTON)
		{
			if (bInlay)		
				m_pMD->SetPixelInMask(m_wSubID, nX - m_nXInlayOffset, nY - m_nYInlayOffset, FALSE);
			if (bMask)
				m_pMD->SetPixelInMask(m_wSubID, nX - m_nXMaskOffset, nY - m_nYMaskOffset, FALSE);
			GetDlgItem(IDC_SAVE_MASK)->EnableWindow(TRUE);
		}
	}

	// CMDConfigDlgBase::OnMouseMove(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMDConfigDlg::OnInitDialog() 
{
	CMDConfigDlgBase::OnInitDialog();
	CString sName;

	GetDlgItem(IDC_SAVE_MASK)->EnableWindow(FALSE);

	// Defaulteinträge der Kameraauswahlcombox setzen
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_SELECT_CAMERA);
	sName.LoadString(IDS_CAMERA1);
	pCombo->InsertString(0, sName);
	sName.LoadString(IDS_CAMERA2);
	pCombo->InsertString(1, sName);
	sName.LoadString(IDS_CAMERA3);
	pCombo->InsertString(2, sName);
	sName.LoadString(IDS_CAMERA4);
	pCombo->InsertString(3, sName);
	sName.LoadString(IDS_CAMERA5);
	pCombo->InsertString(4, sName);
	sName.LoadString(IDS_CAMERA6);
	pCombo->InsertString(5, sName);
	sName.LoadString(IDS_CAMERA7);
	pCombo->InsertString(6, sName);
	sName.LoadString(IDS_CAMERA8);
	pCombo->InsertString(7, sName);
	sName.LoadString(IDS_CAMERA9);
	pCombo->InsertString(8, sName);
	sName.LoadString(IDS_CAMERA10);
	pCombo->InsertString(9, sName);
	sName.LoadString(IDS_CAMERA11);
	pCombo->InsertString(10, sName);
	sName.LoadString(IDS_CAMERA12);
	pCombo->InsertString(11, sName);
	sName.LoadString(IDS_CAMERA13);
	pCombo->InsertString(12, sName);
	sName.LoadString(IDS_CAMERA14);
	pCombo->InsertString(13, sName);
	sName.LoadString(IDS_CAMERA15);
	pCombo->InsertString(14, sName);
	sName.LoadString(IDS_CAMERA16);
	pCombo->InsertString(15, sName);
	pCombo->SetCurSel(m_wSubID);

	// Defaulteinträge der Maskenempfindlichkeit-combobox setzen
	pCombo = (CComboBox*)GetDlgItem(IDC_SELECT_MASK_SENSITIVITY);
	sName.LoadString(IDS_HIGH);
	pCombo->InsertString(0, sName);
	sName.LoadString(IDS_NORMAL);
	pCombo->InsertString(1, sName);
	sName.LoadString(IDS_LOW);
	pCombo->InsertString(2, sName);
	sName.LoadString(IDS_OFF);
	pCombo->InsertString(3, sName);

	// Defaulteinträge der Alarmempfindlichkeit-combobox setzen
	pCombo = (CComboBox*)GetDlgItem(IDC_SELECT_ALARM_SENSITIVITY);
	sName.LoadString(IDS_HIGH);
	pCombo->InsertString(0, sName);
	sName.LoadString(IDS_NORMAL);
	pCombo->InsertString(1, sName);
	sName.LoadString(IDS_LOW);
	pCombo->InsertString(2, sName);
	sName.LoadString(IDS_OFF);
	pCombo->InsertString(3, sName);


	EnableDlgCtrl();

	CRect rc;

	// Position des Inlayfensters
	GetDlgItem(IDC_INLAY_WND)->GetClientRect(rc);
	GetDlgItem(IDC_INLAY_WND)->ClientToScreen(rc);
	m_rcInlayScreen = rc;

	// Position des Maskenfensters
	GetDlgItem(IDC_DIFF_WND)->GetClientRect(rc);
	GetDlgItem(IDC_DIFF_WND)->ClientToScreen(rc);
	m_rcMaskScreen = rc;

	InitToolTips();

	if (!EnableToolTips(TRUE))
		ML_TRACE(_T("Konnte Tooltips nicht aktivieren\n"));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
extern HINSTANCE g_hInstance;

/////////////////////////////////////////////////////////////////////////////
/*BOOL CMDConfigDlg::OnToolTipNotify( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
    TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
    UINT nID =pNMHDR->idFrom;

    if (pTTT->uFlags & TTF_IDISHWND)
    {
        // idFrom is actually the HWND of the tool
        nID = ::GetDlgCtrlID((HWND)nID);
        if(nID)
        {
			LoadString(g_hInstance, nID, pTTT->lpszText, 80);
//			pTTT->lpszText = MAKEINTRESOURCE(nID);            
			pTTT->hinst = AfxGetResourceHandle();
            return(TRUE);
        }
    }
    return(FALSE);
}
*/
/////////////////////////////////////////////////////////////////////////////
void CMDConfigDlg::InitToolTips()
{
	// Alle wollen sie ToolTips haben
	if (m_ToolTip.Create(this) ) {
		m_ToolTip.Activate(TRUE);
		// Lasse den ToolTip die Zeilenumbrueche beachten
		m_ToolTip.SendMessage(TTM_SETMAXTIPWIDTH, 0, SHRT_MAX);

		// Wenn man will, kann man auch noch die Zeiten veraendern
		// Verweildauer des ToolTips (Maximale Zeit=SHRT_MAX)
		m_ToolTip.SendMessage(TTM_SETDELAYTIME, TTDT_AUTOPOP, SHRT_MAX);
		// Wie schnell soll er erscheinen, wenn der Cursor ueber einem Tool steht
		m_ToolTip.SendMessage(TTM_SETDELAYTIME, TTDT_INITIAL, 200);
		// Mindest-Verzoegerungszeit bis zum naechsten Erscheinen
		m_ToolTip.SendMessage(TTM_SETDELAYTIME, TTDT_RESHOW, 200);

		// Alle Child-Windows durchklappern, um deren ToolTips einzulesen
		UINT uID = 0;
		CString sTip;
		int iIndex = 0;
		CWnd* pItem = GetWindow(GW_CHILD);
		// Wir wollen keine Endlosschleife
		while ( pItem && IsChild(pItem) ) {
			// die ToolTip-Texte aus der Resource laden,
			uID = pItem->GetDlgCtrlID();
			// falls Text vorhanden ins Array schreiben und das Tool anhaengen
			if ( sTip.LoadString(uID) && !sTip.IsEmpty() ) {
				iIndex = m_sTTArray.Add(sTip);
				m_ToolTip.AddTool( pItem, m_sTTArray.GetAt(iIndex) );
			}
			// Haben wir noch ein Child?
			pItem = pItem->GetNextWindow();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMDConfigDlg::ShowConfigDlg(WORD /*wSubID*/) 
{
	SOURCE_SELECT_STRUCT SrcSel;
	SrcSel.wSource				= m_wSubID;
	SrcSel.wFormat				= ENCODER_RES_HIGH;
	SrcSel.dwBPF				= 24000;
	SrcSel.dwProzessID			= 0;
	SrcSel.dwPics				= (DWORD)-1;
	SrcSel.wCompressionType		= COMPRESSION_YUV_422;
	if (m_pCodec)
		m_pCodec->SetInputSource(SrcSel);
	
	m_bVisible = TRUE;

	SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
	ShowWindow(SW_SHOW);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CMDConfigDlg::HideConfigDlg() 
{
	m_bVisible = FALSE;
	ShowWindow(SW_HIDE);
}

/////////////////////////////////////////////////////////////////////////////
void CMDConfigDlg::OnLoadMask() 
{
	if (m_pMD)
		m_pMD->LoadMask(m_wSubID);

	GetDlgItem(IDC_SAVE_MASK)->EnableWindow(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
void CMDConfigDlg::OnHideMask() 
{
	if (m_pMD)
		m_pMD->ToggleShowMask();
}

/////////////////////////////////////////////////////////////////////////////
void CMDConfigDlg::OnHistogramm() 
{
	if (m_pMD)
		m_pMD->ToggleHistogramm(m_wSubID);
	GetDlgItem(IDC_SAVE_MASK)->EnableWindow(TRUE);
	EnableDlgCtrl();
}

/////////////////////////////////////////////////////////////////////////////
void CMDConfigDlg::OnFilter() 
{
	if (m_pMD)
		m_pMD->ToggleFilter(m_wSubID);
	GetDlgItem(IDC_SAVE_MASK)->EnableWindow(TRUE);
	EnableDlgCtrl();
}

/////////////////////////////////////////////////////////////////////////////
void CMDConfigDlg::EnableDlgCtrl()
{
	if (m_pMD)		
	{	
		BOOL bMD = m_pMD->GetActivationMDStatus(m_wSubID);
		CheckDlgButton(IDC_ACTIVATE_MD, bMD);   
		GetDlgItem(IDC_INLAY_WND)->EnableWindow(bMD);
		GetDlgItem(IDC_DIFF_WND)->EnableWindow(bMD);
		GetDlgItem(IDC_INVERT_PERMANENT_MASK)->EnableWindow(bMD);
		GetDlgItem(IDC_INVERT_ADAPTIVE_MASK)->EnableWindow(bMD);
		GetDlgItem(IDC_CLEAR_MASK)->EnableWindow(bMD);
		GetDlgItem(IDC_SAVE_MASK)->EnableWindow(bMD);
		GetDlgItem(IDC_LOAD_MASK)->EnableWindow(bMD);
		//GetDlgItem(IDC_USE_ADAPTIVE_MASK)->EnableWindow(bMD);
		GetDlgItem(IDC_HIDE_MASK)->EnableWindow(bMD);
		GetDlgItem(IDC_HISTOGRAMM)->EnableWindow(bMD);
		GetDlgItem(IDC_FILTER)->EnableWindow(bMD);
		CheckDlgButton(IDC_HISTOGRAMM,	m_pMD->GetHistogrammStatus(m_wSubID));
		CheckDlgButton(IDC_FILTER,		m_pMD->GetFilterStatus(m_wSubID));
	
		// Richtigen Masksensitivity Eintrag in der Combobox vorwählen
		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_SELECT_MASK_SENSITIVITY);
		CString sSel = m_pMD->GetMDMaskSensitivity(m_wSubID);
		if (sSel == CSD_HIGH)
			pCombo->SetCurSel(0);
		else if (sSel == CSD_NORMAL)
			pCombo->SetCurSel(1);
		else if (sSel == CSD_LOW)
			pCombo->SetCurSel(2);
		else if (sSel == CSD_OFF)
			pCombo->SetCurSel(3);

		// Richtigen Alarmsensitivity Eintrag in der Combobox vorwählen
		pCombo = (CComboBox*)GetDlgItem(IDC_SELECT_ALARM_SENSITIVITY);
		sSel = m_pMD->GetMDAlarmSensitivity(m_wSubID);
		if (sSel == CSD_HIGH)
			pCombo->SetCurSel(0);
		else if (sSel == CSD_NORMAL)
			pCombo->SetCurSel(1);
		else if (sSel == CSD_LOW)
			pCombo->SetCurSel(2);
		else if (sSel == CSD_OFF)
			pCombo->SetCurSel(3);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMDConfigDlg::OnMove(int x, int y) 
{
	CMDConfigDlgBase::OnMove(x, y);
	
	CRect	rc(0,0,0,0);
	CWnd	*pWnd = NULL;

	// Position des Inlayfensters
	pWnd = GetDlgItem(IDC_INLAY_WND);
	if (pWnd)
	{
		pWnd->GetClientRect(rc);
		pWnd->ClientToScreen(rc);
		m_rcInlayScreen = rc;
	}

	// Position des Maskenfensters
	pWnd = GetDlgItem(IDC_DIFF_WND);
	if (pWnd)
	{
		pWnd->GetClientRect(rc);
		pWnd->ClientToScreen(rc);
		m_rcMaskScreen = rc;
	}
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMDConfigDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	// Let the ToolTip process this message.
	if ( m_ToolTip.GetSafeHwnd() ) {
		m_ToolTip.RelayEvent(pMsg);	
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
void CMDConfigDlg::OnSelectMaskSensitivity() 
{
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_SELECT_MASK_SENSITIVITY);

	int nSel = pCombo->GetCurSel();	
	if (nSel != CB_ERR )
	{
		switch (nSel)
		{
			case 0:	// High
				m_pMD->SetMDMaskSensitivity(m_wSubID, CSD_HIGH);
				break;
			case 1:	// Mid
				m_pMD->SetMDMaskSensitivity(m_wSubID, CSD_NORMAL);
				break;
			case 2:	// Low
				m_pMD->SetMDMaskSensitivity(m_wSubID, CSD_LOW);
				break;
			case 3:	// Off
				m_pMD->SetMDMaskSensitivity(m_wSubID, CSD_OFF);
				break;
		}
		EnableDlgCtrl();
	}	
}

/////////////////////////////////////////////////////////////////////////////
void CMDConfigDlg::OnSelectAlarmSensitivity() 
{
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_SELECT_ALARM_SENSITIVITY);
	int nSel = pCombo->GetCurSel();	
	if (nSel != CB_ERR )
	{
		switch (nSel)
		{
			case 0:	// High
				m_pMD->SetMDAlarmSensitivity(m_wSubID, CSD_HIGH);
				break;
			case 1:	// Mid
				m_pMD->SetMDAlarmSensitivity(m_wSubID, CSD_NORMAL);
				break;
			case 2:	// Low
				m_pMD->SetMDAlarmSensitivity(m_wSubID, CSD_LOW);
				break;
			case 3:	// Off
				m_pMD->SetMDAlarmSensitivity(m_wSubID, CSD_OFF);
				break;
		}
		EnableDlgCtrl();
	}	
}

