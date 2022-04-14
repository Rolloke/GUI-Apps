// DlgUserInterface.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "MainFrm.h"

#include "DlgUserInterface.h"

#include "ObjectTree.h"
#include "ViewDialogBar.h"
#include "ViewAlarmList.h"
#include "ViewDlg.h"
#include "DlgBarIdipClient.h"
#include "SplitterObjectView.h"

#define MINIMIZED_FLAG	0x1000
#define AS_LIST_FLAG	0x2000

/////////////////////////////////////////////////////////////////////////////
// CDlgUserInterface dialog
IMPLEMENT_DYNAMIC(CDlgUserInterface, CSkinDialog)
CDlgUserInterface::CDlgUserInterface(CWnd* pParent /*=NULL*/)
	: CSkinDialog(CDlgUserInterface::IDD, pParent)
	, m_nDivisionMode(0)
	, m_bAsList(FALSE)
	, m_bMinimized(FALSE)
{
	m_nInitToolTips = TOOLTIPS_SIMPLE;
	m_nVisibility	= -1;
	m_nDivisionMode = -1;
	m_bAsList		= -1;
	m_bMinimized	= -1;
	m_hSelected		= NULL;
}
/////////////////////////////////////////////////////////////////////////////
CDlgUserInterface::~CDlgUserInterface()
{
}
/////////////////////////////////////////////////////////////////////////////
void CDlgUserInterface::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_UI_TREE, m_UI_Tree);
	DDX_Radio(pDX, IDC_UI_VISIBILITY0, m_nVisibility);
	DDX_Radio(pDX, IDC_UI_DIVMODE0, m_nDivisionMode);
	DDX_Check(pDX, IDC_UI_AS_LIST, m_bAsList);
	DDX_Check(pDX, IDC_UI_MINIMIZED, m_bMinimized);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDlgUserInterface, CSkinDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_UI_TREE, OnTvnSelchangedUiTree)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CDlgUserInterface message handlers
BOOL CDlgUserInterface::OnInitDialog()
{
	HTREEITEM	hParent;
	CMainFrame*	pMF = theApp.GetMainFrame();
	CString		sName;
	UINT		uMask = TVIF_IMAGE|TVIF_PARAM|TVIF_TEXT|TVIF_SELECTEDIMAGE;
	eVisibilityStates eVisibility;

	CSkinDialog::OnInitDialog();
	AutoCreateSkinButtons();
	AutoCreateSkinStatic();

	m_UI_Tree.ModifyStyle(0, TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS, 0);
	sName.LoadString(IDS_TREE_VIEWS);
	hParent = m_UI_Tree.InsertItem(sName);
	
	POSITION pos = pMF->GetActiveDocument()->GetFirstViewPosition();
	while (pos)
	{
		CView *pView = pMF->GetActiveDocument()->GetNextView(pos);
		if (pView->IsKindOf(RUNTIME_CLASS(CViewObjectTree)))
		{
			CViewObjectTree *pVOT = (CViewObjectTree*)pView;
			pVOT->GetWindowText(sName);
			int nImage = pVOT->GetVisibilityState();
			if (pVOT->IsMinimized()) nImage |= MINIMIZED_FLAG;
			m_UI_Tree.InsertItem(uMask, sName, nImage, 0, 0, 0, (LPARAM)pVOT, hParent, TVI_LAST);
		}
		else if (pView->IsKindOf(RUNTIME_CLASS(CViewDialogBar)))
		{
			CViewDialogBar *pVDB = (CViewDialogBar*)pView;
			CDlgInView*pDlg = pVDB->GetDlgInView();
			if (pDlg->IsKindOf(RUNTIME_CLASS(CDlgBarIdipClient)))
			{
				pDlg->GetWindowText(sName);
				eVisibility = pDlg->GetVisibilityState();
				m_UI_Tree.InsertItem(uMask, sName, eVisibility, 0, 0, 0, (LPARAM)pDlg, TVI_ROOT, TVI_LAST);
			}
		}
	}

	CSplitterObjectView *pSOV = pMF->GetSplitterObjectView();
	pSOV->GetWindowText(sName);
	eVisibility = pSOV->GetVisibilityState();
	eObjViewDivMode eDM = pSOV->GetDivisionMode();
	m_UI_Tree.InsertItem(uMask, sName, eVisibility, eDM, 0, 0, (LPARAM)pSOV, TVI_ROOT, TVI_LAST);

	m_UI_Tree.Expand(hParent, TVE_EXPAND);
	m_UI_Tree.SelectItem(m_UI_Tree.GetChildItem(hParent));

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgUserInterface::OnOK()
{
	m_UI_Tree.SelectItem(m_UI_Tree.GetRootItem());
	m_nVisibility = 0;

	SaveTreeData(m_UI_Tree.GetRootItem(), TRUE);
	
	if (m_nVisibility == 0)
	{
		AfxMessageBox(IDS_SHOW_AT_LEAST_ONE_WINDOW, MB_ICONERROR);
	}
	else
	{
		SaveTreeData(m_UI_Tree.GetRootItem(), FALSE);
		CSkinDialog::OnOK();
	}
}
void CDlgUserInterface::SaveTreeData(HTREEITEM hItem, BOOL bTest)
{
	if (hItem)
	{
		CWnd *pWnd = (CWnd*)m_UI_Tree.GetItemData(hItem);
		CString str = m_UI_Tree.GetItemText(hItem);
		if (pWnd)
		{
			int nImage, nSelected;
			eVisibilityStates eVisibility;
			m_UI_Tree.GetItemImage(hItem, nImage, nSelected);
			eVisibility = (eVisibilityStates) LOBYTE(nImage);
			if (pWnd->IsKindOf(RUNTIME_CLASS(CViewObjectTree)))
			{
				if (!bTest)
				{
					((CViewObjectTree*)pWnd)->SetMinimized((nImage & MINIMIZED_FLAG) ? TRUE : FALSE);
					((CViewObjectTree*)pWnd)->SetVisibility(eVisibility);
				}
				switch (eVisibility)
				{
					case OTVS_Always: case OTVS_InitiallyYes:
						m_nVisibility++;
					break;
				}
			}
			else if (!bTest && pWnd->IsKindOf(RUNTIME_CLASS(CDlgInView)))
			{
				((CDlgInView*)pWnd)->SetVisibility(eVisibility);		
			}
			else if (!bTest && pWnd->IsKindOf(RUNTIME_CLASS(CSplitterObjectView)))
			{
				((CSplitterObjectView*)pWnd)->SetVisibility(eVisibility);
				((CSplitterObjectView*)pWnd)->SetDivisionMode((eObjViewDivMode)nSelected);
				((CSplitterObjectView*)pWnd)->SaveSettings();
			}
		}

		SaveTreeData(m_UI_Tree.GetChildItem(hItem), bTest);
		SaveTreeData(m_UI_Tree.GetNextItem(hItem, TVGN_NEXT), bTest);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgUserInterface::OnTvnSelchangedUiTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM hItem = m_UI_Tree.GetSelectedItem();
	if (hItem && hItem != m_hSelected)
	{
		DWORD dwFlag=0, dwMask;
		int nImage = 0;
		CWnd*pWnd = (CWnd*)m_UI_Tree.GetItemData(hItem);
		if (m_hSelected)
		{
			UpdateData(TRUE);
			nImage = m_nVisibility + 1;
			if (m_bMinimized) nImage |= MINIMIZED_FLAG;
			if (m_bAsList)	  nImage |= AS_LIST_FLAG;
			m_UI_Tree.SetItemImage(m_hSelected, nImage, m_nDivisionMode+1);
		}
		if (pWnd)
		{
			m_UI_Tree.GetItemImage(hItem, nImage, m_nDivisionMode);
			m_bMinimized = (nImage & MINIMIZED_FLAG) ? TRUE : FALSE;
			m_bAsList    = (nImage & AS_LIST_FLAG)   ? TRUE : FALSE;
			m_nVisibility = LOBYTE(nImage)-1;
			m_nDivisionMode--;
			UpdateData(FALSE);
			m_hSelected = hItem;
			if      (pWnd->IsKindOf(RUNTIME_CLASS(CViewAlarmList)))
			{
				dwFlag = 0x003f003f;
//				dwFlag = 0x007f007f;	// TODO! RKE: Enable 'As List' when ready
			}
			else if (pWnd->IsKindOf(RUNTIME_CLASS(CViewObjectTree)))
			{
				dwFlag = 0x003f003f;
			}
			else if (pWnd->IsKindOf(RUNTIME_CLASS(CSplitterObjectView)))
			{
				dwFlag = 0x00180018;
			}
			else
			{
				dwFlag = 0x001f0018;
			}

			int nID;
			for (nID=IDC_UI_VISIBILITY0,dwMask=0x00010001; nID<=IDC_UI_AS_LIST; nID++,dwMask<<=1)
			{
				pWnd = GetDlgItem(nID);
				pWnd->EnableWindow(LOWORD(dwFlag&dwMask) ? 1 : 0);
				pWnd->ShowWindow(HIWORD(dwFlag&dwMask) ? SW_SHOW : SW_HIDE);
			}

			for (nID=IDC_UI_DIVMODE0,dwMask=0x00010001; nID<=IDC_UI_DIVMODE2; nID++,dwMask<<=1)
			{
				pWnd = GetDlgItem(nID);
				pWnd->ShowWindow(HIWORD(dwFlag&dwMask) ? SW_HIDE : SW_SHOW);
			}
		}
		else
		{
			m_hSelected = NULL;
			int nID;
			for (nID=IDC_UI_VISIBILITY0; nID<=IDC_UI_AS_LIST; nID++)
			{
				pWnd = GetDlgItem(nID);
				pWnd->ShowWindow(SW_HIDE);
			}

			for (nID=IDC_UI_DIVMODE0; nID<=IDC_UI_DIVMODE2; nID++)
			{
				pWnd = GetDlgItem(nID);
				pWnd->ShowWindow(SW_HIDE);
			}
		}

	}
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////

void CDlgUserInterface::OnCancel()
{
	CSkinDialog::OnCancel();
}
