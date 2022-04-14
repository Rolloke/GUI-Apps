// AnalyzerBar.cpp : implementation file
//

#include "stdafx.h"
#include "SecAnalyzer.h"
#include "SecAnalyzerDoc.h"
#include "AnalyzerBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnalyzerBar dialog


CAnalyzerBar::CAnalyzerBar(CWnd* pParent /*=NULL*/)
	: CInitDialogBar()//(CAnalyzerBar::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAnalyzerBar)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAnalyzerBar::DoDataExchange(CDataExchange* pDX)
{
	TRACE("DDE\n");
	CInitDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAnalyzerBar)
	DDX_Control(pDX, IDC_KNOW_GROUPS, m_ctlKnownGroups);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAnalyzerBar, CInitDialogBar)
	//{{AFX_MSG_MAP(CAnalyzerBar)
	ON_BN_CLICKED(IDC_LAST_RESET, OnLastReset)
	ON_LBN_SELCHANGE(IDC_KNOW_GROUPS, OnSelchangeKnownGroups)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




void CAnalyzerBar::OnLastReset() 
{
	TRACE("last reset");
}

BOOL CAnalyzerBar::OnInitDialog() 
{
	CInitDialogBar::OnInitDialogBar();

	UpdateGroups();

	return TRUE;  // return TRUE unless you set the focus to a conrol
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CAnalyzerBar::UpdateGroups() 
{
	m_ctlKnownGroups.UpdateData();
	m_ctlKnownGroups.ResetContent();
	
	CString sGroup;
	for (int g=0;g < CSecAnalyzerDoc::m_knownGroups.GetSize();g++) {
		sGroup = *CSecAnalyzerDoc::m_knownGroups[g];
		sGroup = sGroup.Left(sGroup.GetLength()-3);
		int ix = m_ctlKnownGroups.AddString(sGroup);
		m_ctlKnownGroups.SetCheck(ix,TRUE);
	}

}

// scan the CheckListBox and transfer the selected group to m_usedGroups
void CAnalyzerBar::UpdateUsedGroups() 
{
	CString sGroup;
	CSecAnalyzerDoc::m_usedGroups.DeleteAll();

	for (int g=0;g < m_ctlKnownGroups.GetCount();g++) {
		if (m_ctlKnownGroups.GetCheck(g)) {
			CString sTmp;
			m_ctlKnownGroups.GetText(g,sTmp);
			sTmp += "Log";
			CSecAnalyzerDoc::m_usedGroups.Add(new CWK_String(sTmp));
		}
	}

	CSecAnalyzerDoc::m_usedGroups.UpdateMap();
			
}


void CAnalyzerBar::OnSelchangeKnownGroups() 
{
	UpdateUsedGroups();
}
