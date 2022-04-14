// MCLTPpg.cpp : Implementation of the CMCLTPropPage property page class.

#include "stdafx.h"
#include "MCLT.h"
#include "MCLTPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CMCLTPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CMCLTPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CMCLTPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CMCLTPropPage, "MCLT.MCLTPropPage.1",
	0x4a14026a, 0xbcc8, 0x4c11, 0xbc, 0xff, 0xd7, 0xbd, 0x12, 0xa9, 0xe, 0xbe)


/////////////////////////////////////////////////////////////////////////////
// CMCLTPropPage::CMCLTPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CMCLTPropPage

BOOL CMCLTPropPage::CMCLTPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_MCLT_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CMCLTPropPage::CMCLTPropPage - Constructor

CMCLTPropPage::CMCLTPropPage() :
	COlePropertyPage(IDD, IDS_MCLT_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CMCLTPropPage)
	m_nShape = -1;
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CMCLTPropPage::DoDataExchange - Moves data between page and properties

void CMCLTPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CMCLTPropPage)
	DDP_CBIndex(pDX, IDC_CBSHAPE, m_nShape, _T("Shape") );
	DDX_CBIndex(pDX, IDC_CBSHAPE, m_nShape);
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CMCLTPropPage message handlers
