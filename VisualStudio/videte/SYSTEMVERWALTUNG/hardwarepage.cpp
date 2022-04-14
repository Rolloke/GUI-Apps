/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: hardwarepage.cpp $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/hardwarepage.cpp $
// CHECKIN:		$Date: 24.11.04 11:07 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 24.11.04 10:59 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"
#include "systemverwaltung.h"

#include "InputList.h"
#include "OutputList.h"
#include "HardwarePage.h"

#include "SVDoc.h"
#include "SVView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CHardwarePage property page
IMPLEMENT_DYNAMIC(CHardwarePage, CSVPage)
/////////////////////////////////////////////////////////////////////////////
CHardwarePage::CHardwarePage(CSVView* pParent) : CSVPage(CHardwarePage::IDD)
{
	m_pParent = pParent;
	//{{AFX_DATA_INIT(CHardwarePage)
	//}}AFX_DATA_INIT
	Create(IDD,m_pParent);
}

/////////////////////////////////////////////////////////////////////////////
CHardwarePage::~CHardwarePage()
{
}
/////////////////////////////////////////////////////////////////////////////
void CHardwarePage::CancelChanges()
{
}
/////////////////////////////////////////////////////////////////////////////
void CHardwarePage::SaveChanges()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHardwarePage::IsDataValid()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CHardwarePage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHardwarePage)
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CHardwarePage, CSVPage)
	//{{AFX_MSG_MAP(CHardwarePage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHardwarePage message handlers
BOOL CHardwarePage::OnInitDialog() 
{
	CSVPage::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
