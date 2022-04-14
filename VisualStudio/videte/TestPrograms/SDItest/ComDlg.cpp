// ComDlg.cpp : implementation file
//

#include "stdafx.h"
#include "sditest.h"
#include "ComDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComDlg dialog


CComDlg::CComDlg(int iCom, CWnd* pParent /*=NULL*/)
	: CDialog(CComDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CComDlg)
	m_iCom = iCom;
	//}}AFX_DATA_INIT
}


void CComDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CComDlg)
	DDX_Control(pDX, IDC_RADIO_COM_1, m_btnCom1);
	DDX_Radio(pDX, IDC_RADIO_COM_1, m_iCom);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CComDlg, CDialog)
	//{{AFX_MSG_MAP(CComDlg)
	ON_BN_CLICKED(IDC_RADIO_COM_1, OnRadioComChanged)
	ON_BN_CLICKED(IDC_RADIO_COM_2, OnRadioComChanged)
	ON_BN_CLICKED(IDC_RADIO_COM_3, OnRadioComChanged)
	ON_BN_CLICKED(IDC_RADIO_COM_4, OnRadioComChanged)
	ON_BN_CLICKED(IDC_RADIO_COM_5, OnRadioComChanged)
	ON_BN_CLICKED(IDC_RADIO_COM_6, OnRadioComChanged)
	ON_BN_CLICKED(IDC_RADIO_COM_7, OnRadioComChanged)
	ON_BN_CLICKED(IDC_RADIO_COM_8, OnRadioComChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComDlg message handlers

void CComDlg::OnRadioComChanged() 
{
	UpdateData();	
}
