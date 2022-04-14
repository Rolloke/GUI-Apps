/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaUnit
// FILE:		$Workfile: AnalogVideoDlg.cpp $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaUnit/AnalogVideoDlg.cpp $
// CHECKIN:		$Date: 12.07.04 11:51 $
// VERSION:		$Revision: 15 $
// LAST CHANGE:	$Modtime: 12.07.04 10:26 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tashaunit.h"
#include "AnalogVideoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_UPDATE WM_USER+10

/////////////////////////////////////////////////////////////////////////////
// CAnalogVideoDlg dialog


CAnalogVideoDlg::CAnalogVideoDlg(CWnd* pParent /*=NULL*/, CCodec* pCodec/*=NULL*/)
	: CDialog(CAnalogVideoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAnalogVideoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pCodec			= pCodec;
	m_dwCrossPointMask	= 0x01020304;
	m_byTermMask		= 0xff;
	m_bVideoEnable		= TRUE;
	m_bCrosspointEnable	= TRUE;
	m_bVisible			= FALSE;

	CDialog::Create(IDD, pParent);
}


void CAnalogVideoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAnalogVideoDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAnalogVideoDlg, CDialog)
	//{{AFX_MSG_MAP(CAnalogVideoDlg)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CROSSPOINT_0_0, OnCrosspoint00)
	ON_BN_CLICKED(IDC_CROSSPOINT_1_0, OnCrosspoint10)
	ON_BN_CLICKED(IDC_CROSSPOINT_2_0, OnCrosspoint20)
	ON_BN_CLICKED(IDC_CROSSPOINT_3_0, OnCrosspoint30)
	ON_BN_CLICKED(IDC_CROSSPOINT_4_0, OnCrosspoint40)
	ON_BN_CLICKED(IDC_CROSSPOINT_5_0, OnCrosspoint50)
	ON_BN_CLICKED(IDC_CROSSPOINT_6_0, OnCrosspoint60)
	ON_BN_CLICKED(IDC_CROSSPOINT_7_0, OnCrosspoint70)
	ON_BN_CLICKED(IDC_CROSSPOINT_0_1, OnCrosspoint01)
	ON_BN_CLICKED(IDC_CROSSPOINT_1_1, OnCrosspoint11)
	ON_BN_CLICKED(IDC_CROSSPOINT_2_1, OnCrosspoint21)
	ON_BN_CLICKED(IDC_CROSSPOINT_3_1, OnCrosspoint31)
	ON_BN_CLICKED(IDC_CROSSPOINT_4_1, OnCrosspoint41)
	ON_BN_CLICKED(IDC_CROSSPOINT_5_1, OnCrosspoint51)
	ON_BN_CLICKED(IDC_CROSSPOINT_6_1, OnCrosspoint61)
	ON_BN_CLICKED(IDC_CROSSPOINT_7_1, OnCrosspoint71)
	ON_BN_CLICKED(IDC_CROSSPOINT_0_2, OnCrosspoint02)
	ON_BN_CLICKED(IDC_CROSSPOINT_1_2, OnCrosspoint12)
	ON_BN_CLICKED(IDC_CROSSPOINT_2_2, OnCrosspoint22)
	ON_BN_CLICKED(IDC_CROSSPOINT_3_2, OnCrosspoint32)
	ON_BN_CLICKED(IDC_CROSSPOINT_4_2, OnCrosspoint42)
	ON_BN_CLICKED(IDC_CROSSPOINT_5_2, OnCrosspoint52)
	ON_BN_CLICKED(IDC_CROSSPOINT_6_2, OnCrosspoint62)
	ON_BN_CLICKED(IDC_CROSSPOINT_7_2, OnCrosspoint72)
	ON_BN_CLICKED(IDC_CROSSPOINT_0_3, OnCrosspoint03)
	ON_BN_CLICKED(IDC_CROSSPOINT_1_3, OnCrosspoint13)
	ON_BN_CLICKED(IDC_CROSSPOINT_2_3, OnCrosspoint23)
	ON_BN_CLICKED(IDC_CROSSPOINT_3_3, OnCrosspoint33)
	ON_BN_CLICKED(IDC_CROSSPOINT_4_3, OnCrosspoint43)
	ON_BN_CLICKED(IDC_CROSSPOINT_5_3, OnCrosspoint53)
	ON_BN_CLICKED(IDC_CROSSPOINT_6_3, OnCrosspoint63)
	ON_BN_CLICKED(IDC_CROSSPOINT_7_3, OnCrosspoint73)
	ON_BN_CLICKED(IDC_TERM_0, OnTermination0)
	ON_BN_CLICKED(IDC_TERM_1, OnTermination1)
	ON_BN_CLICKED(IDC_TERM_2, OnTermination2)
	ON_BN_CLICKED(IDC_TERM_3, OnTermination3)
	ON_BN_CLICKED(IDC_TERM_4, OnTermination4)
	ON_BN_CLICKED(IDC_TERM_5, OnTermination5)
	ON_BN_CLICKED(IDC_TERM_6, OnTermination6)
	ON_BN_CLICKED(IDC_TERM_7, OnTermination7)
	ON_BN_CLICKED(IDC_VID_ENABLE, OnVideoEnable)
	ON_BN_CLICKED(IDC_CROSSPOINT_ENABLE, OnCrosspointEnable)
	ON_MESSAGE(WM_UPDATE, OnUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnalogVideoDlg message handlers

/////////////////////////////////////////////////////////////////////////////

long CAnalogVideoDlg::OnUpdate(WPARAM wParam, LPARAM lParam)
{
	UpdateDlg();

	return 0;
}

BOOL CAnalogVideoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Aktuellen Einstellungen abfragen
	if (m_pCodec)
	{
		m_pCodec->DoRequestGetAnalogOut();
		m_pCodec->DoRequestGetTerminationState();
		m_pCodec->DoRequestGetVideoEnableState();
		m_pCodec->DoRequestGetCrosspointEnableState();
		UpdateDlg();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnDestroy() 
{
	CDialog::OnDestroy();
}


/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::ShowConfigDlg(HWND hWndParent) 
{
	UpdateDlg();

	m_bVisible = TRUE;

	SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);

	if (hWndParent != NULL)
		CenterWindow(FromHandle(hWndParent));

	ShowWindow(SW_SHOW);
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::HideConfigDlg() 
{
	m_bVisible = FALSE;
	ShowWindow(SW_HIDE);
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnOK() 
{
	m_bVisible = FALSE;
	
	HideConfigDlg();
}

//////////////////////////////////////////////////////////////////////
BOOL CAnalogVideoDlg::OnConfirmSetAnalogOut(DWORD dwCrossPointMask)
{
	m_dwCrossPointMask = dwCrossPointMask;
	PostMessage(WM_UPDATE);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CAnalogVideoDlg::OnConfirmGetAnalogOut(DWORD dwCrossPointMask)
{
	m_dwCrossPointMask = dwCrossPointMask;
	PostMessage(WM_UPDATE);
	return TRUE;
}	

//////////////////////////////////////////////////////////////////////
BOOL CAnalogVideoDlg::OnConfirmSetTerminationState(BYTE byTermMask)
{
	m_byTermMask = byTermMask;
	PostMessage(WM_UPDATE);
	return TRUE;
}	

//////////////////////////////////////////////////////////////////////
BOOL CAnalogVideoDlg::OnConfirmGetTerminationState(BYTE byTermMask)
{
	m_byTermMask = byTermMask;
	PostMessage(WM_UPDATE);
	return TRUE;
}	

//////////////////////////////////////////////////////////////////////
BOOL CAnalogVideoDlg::OnConfirmSetVideoEnableState(BOOL bState)
{
	m_bVideoEnable = bState;
	PostMessage(WM_UPDATE);
	return TRUE;
}	

//////////////////////////////////////////////////////////////////////
BOOL CAnalogVideoDlg::OnConfirmGetVideoEnableState(BOOL bState)
{
	m_bVideoEnable = bState;
	PostMessage(WM_UPDATE);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CAnalogVideoDlg::OnConfirmSetCrosspointEnableState(BOOL bState)
{
	m_bCrosspointEnable = bState;
	PostMessage(WM_UPDATE);
	return TRUE;
}	

//////////////////////////////////////////////////////////////////////
BOOL CAnalogVideoDlg::OnConfirmGetCrosspointEnableState(BOOL bState)
{
	m_bCrosspointEnable = bState;
	PostMessage(WM_UPDATE);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::DoRequestSetAnalogOut(int nIn, int nOut)
{
	m_dwCrossPointMask = m_dwCrossPointMask &~ (0xff<<(8*nOut));
	m_dwCrossPointMask |= nIn<<(8*nOut);

	if (m_pCodec)
		m_pCodec->DoRequestSetAnalogOut(m_dwCrossPointMask);
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint00() 
{
	DoRequestSetAnalogOut(0, 0);
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint10() 
{
	DoRequestSetAnalogOut(1, 0);
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint20() 
{
	DoRequestSetAnalogOut(2, 0);
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint30() 
{
	DoRequestSetAnalogOut(3, 0);
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint40() 
{
	DoRequestSetAnalogOut(4, 0);
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint50() 
{
	DoRequestSetAnalogOut(5, 0);
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint60() 
{
	DoRequestSetAnalogOut(6, 0);
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint70() 
{
	DoRequestSetAnalogOut(7, 0);
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint01() 
{
	DoRequestSetAnalogOut(0, 1);
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint11() 
{
	DoRequestSetAnalogOut(1, 1);
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint21() 
{
	DoRequestSetAnalogOut(2, 2);
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint31() 
{
	DoRequestSetAnalogOut(3, 1);
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint41() 
{
	DoRequestSetAnalogOut(4, 1);
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint51() 
{
	DoRequestSetAnalogOut(5, 1);
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint61() 
{
	DoRequestSetAnalogOut(6, 1);
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint71() 
{
	DoRequestSetAnalogOut(7, 1);
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint02() 
{
	DoRequestSetAnalogOut(0, 2);
}
/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint12() 
{
	DoRequestSetAnalogOut(1, 2);
}
/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint22() 
{
	DoRequestSetAnalogOut(2, 2);
}
/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint32() 
{
	DoRequestSetAnalogOut(3, 2);
}
/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint42() 
{
	DoRequestSetAnalogOut(4, 2);
}
/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint52() 
{
	DoRequestSetAnalogOut(5, 2);
}
/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint62() 
{
	DoRequestSetAnalogOut(6, 2);
}
/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint72() 
{
	DoRequestSetAnalogOut(7, 2);
}
/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint03() 
{
	DoRequestSetAnalogOut(0, 3);
}
/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint13() 
{
	DoRequestSetAnalogOut(1, 3);
}
/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint23() 
{
	DoRequestSetAnalogOut(2, 3);
}
/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint33() 
{
	DoRequestSetAnalogOut(3, 3);
}
/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint43() 
{
	DoRequestSetAnalogOut(4, 3);
}
/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint53() 
{
	DoRequestSetAnalogOut(5, 3);
}
/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint63() 
{
	DoRequestSetAnalogOut(6, 3);
}
/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspoint73() 
{
	DoRequestSetAnalogOut(7, 3);
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::DoRequestSetTerminationState(int nPort)
{
	if (!TSTBIT(m_byTermMask, nPort))
		m_byTermMask = SETBIT(m_byTermMask, nPort);
	else
		m_byTermMask = CLRBIT(m_byTermMask, nPort);

	if (m_pCodec)
		m_pCodec->DoRequestSetTerminationState(m_byTermMask);
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnTermination0() 
{
	DoRequestSetTerminationState(0);
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnTermination1() 
{
	DoRequestSetTerminationState(1);
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnTermination2() 
{
	DoRequestSetTerminationState(2);
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnTermination3() 
{
	DoRequestSetTerminationState(3);
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnTermination4() 
{
	DoRequestSetTerminationState(4);
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnTermination5() 
{
	DoRequestSetTerminationState(5);
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnTermination6() 
{
	DoRequestSetTerminationState(6);
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnTermination7() 
{
	DoRequestSetTerminationState(7);
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnVideoEnable() 
{
	m_bVideoEnable = !m_bVideoEnable;
	if (m_pCodec)
		m_pCodec->DoRequestSetVideoEnableState(m_bVideoEnable);
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::OnCrosspointEnable() 
{
	m_bCrosspointEnable = !m_bCrosspointEnable;
	if (m_pCodec)
		m_pCodec->DoRequestSetCrosspointEnableState(m_bCrosspointEnable);
}

/////////////////////////////////////////////////////////////////////////////
void CAnalogVideoDlg::UpdateDlg()
{
	// Videooutmatrix korrekt anzeigen
	BYTE byOut = (BYTE)(m_dwCrossPointMask>>0);
	CheckRadioButton(IDC_CROSSPOINT_0_0, IDC_CROSSPOINT_7_0, IDC_CROSSPOINT_0_0+byOut);
	
	byOut = (BYTE)(m_dwCrossPointMask>>8);
	CheckRadioButton(IDC_CROSSPOINT_0_1, IDC_CROSSPOINT_7_1, IDC_CROSSPOINT_0_1+byOut);

	byOut = (BYTE)(m_dwCrossPointMask>>16);
	CheckRadioButton(IDC_CROSSPOINT_0_2, IDC_CROSSPOINT_7_2, IDC_CROSSPOINT_0_2+byOut);

	byOut = (BYTE)(m_dwCrossPointMask>>24);
	CheckRadioButton(IDC_CROSSPOINT_0_3, IDC_CROSSPOINT_7_3, IDC_CROSSPOINT_0_3+byOut);


	// Videoterminierung anzeigen.
	for (int nI = 0; nI < CHANNEL_COUNT; nI++)
		CheckDlgButton(IDC_TERM_0+nI, TSTBIT(m_byTermMask, nI));

	// Video Enable/Disable
	CheckDlgButton(IDC_VID_ENABLE, m_bVideoEnable);

	// Crosspoint Enable/Disable
	CheckDlgButton(IDC_CROSSPOINT_ENABLE, m_bCrosspointEnable);
}

