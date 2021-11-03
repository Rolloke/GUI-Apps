// StandardProperties.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "WaveGen.h"
#include "StandardProperties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld StandardProperties 
#define LOWEST_ORDER  4
#define HIGHEST_ORDER 32


StandardProperties::StandardProperties(CWnd* pParent /*=NULL*/)
	: CDialog(StandardProperties::IDD, pParent)
{
	//{{AFX_DATA_INIT(StandardProperties)
	m_nNumVal = 0;
	m_nOrder  = LOWEST_ORDER;
	//}}AFX_DATA_INIT
}


void StandardProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(StandardProperties)
	DDX_Control(pDX, IDC_STD_ORDER_SPIN, m_cOrderSpin);
	DDX_Text(pDX, IDC_STD_NUM_OF_VAL, m_nNumVal);
	DDX_Text(pDX, IDC_STD_ORDER, m_nOrder);
	DDV_MinMaxInt(pDX, m_nOrder, LOWEST_ORDER, HIGHEST_ORDER);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(StandardProperties, CDialog)
	//{{AFX_MSG_MAP(StandardProperties)
	ON_EN_CHANGE(IDC_STD_ORDER, OnChangeOrder)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten StandardProperties 

void StandardProperties::OnOK() 
{
	
	CDialog::OnOK();
}

BOOL StandardProperties::OnInitDialog() 
{
   m_nNumVal = (1 << m_nOrder) + 1;
	CDialog::OnInitDialog();
	m_cOrderSpin.SetBuddy(GetDlgItem(IDC_STD_ORDER));
   m_cOrderSpin.SetRange32(LOWEST_ORDER, HIGHEST_ORDER);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void StandardProperties::OnChangeOrder() 
{
   if (UpdateData(true))
   {
      m_nNumVal = (1 << m_nOrder) +1;
      UpdateData(false);
   }
}
