// OptionsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "jpegtest.h"
#include "OptionsDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsDialog dialog


COptionsDialog::COptionsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(COptionsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsDialog)
	m_bDecodeToHDD = FALSE;
	m_bDecodeToScreen = FALSE;
	m_bBrightness = FALSE;
	m_bNoiseReduction = FALSE;
	m_bH263 = FALSE;
	m_iScale = 0;
	m_iHeight = 0;
	m_iWidth = 0;
	//}}AFX_DATA_INIT
	m_iFPS = 6;
}


void COptionsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsDialog)
	DDX_Control(pDX, IDC_COMBO_FPS, m_comboFPS);
	DDX_Control(pDX, IDC_EDIT_WIDTH, m_editWidth);
	DDX_Control(pDX, IDC_EDIT_HEIGHT, m_editHeight);
	DDX_Check(pDX, IDC_CHECK_DECODE_HDD, m_bDecodeToHDD);
	DDX_Check(pDX, IDC_CHECK_DECODE_SCREEN, m_bDecodeToScreen);
	DDX_Check(pDX, IDC_CHECK_BRIGHTNESS, m_bBrightness);
	DDX_Check(pDX, IDC_CHECK_NOISE_REDUCTION, m_bNoiseReduction);
	DDX_Check(pDX, IDC_CHECK_H263, m_bH263);
	DDX_Radio(pDX, IDC_RADIO_NO_SCALE, m_iScale);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_iHeight);
	DDX_Text(pDX, IDC_EDIT_WIDTH, m_iWidth);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
		// control to data
		int s = m_comboFPS.GetCurSel();
		m_iFPS = m_comboFPS.GetItemData(s);
	}
	else
	{
		// data to control
		for (int i=0;i<m_comboFPS.GetCount();i++)
		{
			if (m_iFPS == m_comboFPS.GetItemData(i))
			{
				m_comboFPS.SetCurSel(i);
				break;
			}
		}
	}
}


BEGIN_MESSAGE_MAP(COptionsDialog, CDialog)
	//{{AFX_MSG_MAP(COptionsDialog)
	ON_BN_CLICKED(IDC_CHECK_DECODE_SCREEN, OnCheckDecodeScreen)
	ON_BN_CLICKED(IDC_CHECK_H263, OnCheckH263)
	ON_BN_CLICKED(IDC_CHECK_DECODE_HDD, OnCheckDecodeHdd)
	ON_BN_CLICKED(IDC_RADIO_NO_SCALE, OnRadioNoScale)
	ON_BN_CLICKED(IDC_RADIO_SCALE_CIF, OnRadioScaleCif)
	ON_BN_CLICKED(IDC_RADIO_SCALE_FREE, OnRadioScaleFree)
	ON_BN_CLICKED(IDC_RADIO_SCALE_QCIF, OnRadioScaleQcif)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsDialog message handlers

void COptionsDialog::OnCheckDecodeScreen() 
{
/*
	UpdateData();
	if (m_bDecodeToScreen)
	{
		m_bH263 = FALSE;
	}
	UpdateData(FALSE);
*/
}
/////////////////////////////////////////////////////////////////////////////
void COptionsDialog::OnCheckH263() 
{
/*
	UpdateData();
	if (m_bH263)
	{
		m_bDecodeToScreen = FALSE;
	}
	UpdateData(FALSE);
*/
}
/////////////////////////////////////////////////////////////////////////////
void COptionsDialog::OnCheckDecodeHdd() 
{
/*
	UpdateData();
	if (m_bDecodeToHDD)
	{
		m_bH263 = FALSE;
	}
	UpdateData(FALSE);
*/
}

void COptionsDialog::OnRadioNoScale() 
{
	// TODO: Add your control notification handler code here
	m_editWidth.EnableWindow(FALSE);
	m_editHeight.EnableWindow(FALSE);
}

void COptionsDialog::OnRadioScaleCif() 
{
	UpdateData();

	// TODO: Add your control notification handler code here
	m_editWidth.EnableWindow(FALSE);
	m_editHeight.EnableWindow(FALSE);

	m_iWidth = 352;
	m_iHeight = 288;

	UpdateData(FALSE);
	
}

void COptionsDialog::OnRadioScaleFree() 
{
	// TODO: Add your control notification handler code here
	m_editWidth.EnableWindow(TRUE);
	m_editHeight.EnableWindow(TRUE);
	
}

void COptionsDialog::OnRadioScaleQcif() 
{
	UpdateData();

	// TODO: Add your control notification handler code here
	m_editWidth.EnableWindow(FALSE);
	m_editHeight.EnableWindow(FALSE);
	
	m_iWidth = 176;
	m_iHeight = 144;

	UpdateData(FALSE);
}

BOOL COptionsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	int i;
	i = m_comboFPS.AddString("fast");
	m_comboFPS.SetItemData(i,0);
	i = m_comboFPS.AddString("1 fps");
	m_comboFPS.SetItemData(i,1);
	i = m_comboFPS.AddString("2 fps");
	m_comboFPS.SetItemData(i,2);
	i = m_comboFPS.AddString("3 fps");
	m_comboFPS.SetItemData(i,3);
	i = m_comboFPS.AddString("4 fps");
	m_comboFPS.SetItemData(i,4);
	i = m_comboFPS.AddString("5 fps");
	m_comboFPS.SetItemData(i,5);
	i = m_comboFPS.AddString("6 fps");
	m_comboFPS.SetItemData(i,6);
	i = m_comboFPS.AddString("8 fps");
	m_comboFPS.SetItemData(i,8);
	i = m_comboFPS.AddString("12 fps");
	m_comboFPS.SetItemData(i,12);
	i = m_comboFPS.AddString("16 fps");
	m_comboFPS.SetItemData(i,16);
	i = m_comboFPS.AddString("25 fps");
	m_comboFPS.SetItemData(i,25);

	UpdateData(FALSE);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
