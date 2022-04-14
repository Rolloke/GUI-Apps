// SplitterObjectView.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "SplitterObjectView.h"

#include "MainFrm.h"

#include "ViewDlg.h"

#include "ObjectTree.h"

#include "ViewCamera.h"
#include "ViewRelais.h"
#include "ViewAlarmList.h"
#include "ViewArchive.h"
#include "ViewAudio.h"
#include "ViewDialogBar.h"

#define MK_INCAPTION	0x00010000
/////////////////////////////////////////////////////////////////////////////////////////
// CSplitterObjectView


IMPLEMENT_DYNAMIC(CSplitterObjectView, CSplitterWndCommon)
/////////////////////////////////////////////////////////////////////////////////////////
CSplitterObjectView::CSplitterObjectView()
{
	m_nActiveID			= 0;
	m_nTrackingRow		= -1;
	m_cxBorder			= 1;
	m_cyBorder			= afxData.cyBorder2;
	m_bOEMDialogVisible = TRUE;
	m_bObjectViewVisible = TRUE;

	m_eVisibility		= OTVS_InitiallyYes;

	m_szUpdate.cx = 0;
	m_szUpdate.cy = 0;
	m_pViewCamera		= NULL;
	m_pViewArchive		= NULL;
	m_pViewRelais		= NULL;
	m_pViewAlarmList	= NULL;
	m_pViewAudio		= NULL;
	m_pViewOEMDlg		= NULL;
	m_pViewSelected		= NULL;

//	SetDivisionMode(OVDM_VARIABLE_ARANGEMENT);
	SetDivisionMode(OVDM_EQUAL_ARANGEMENT);
//	SetDivisionMode(OVDM_SHOW_ONE_VIEW);

}
/////////////////////////////////////////////////////////////////////////////////////////
CSplitterObjectView::~CSplitterObjectView()
{
}
/////////////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSplitterObjectView, CSplitterWndCommon)
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_USER, OnUserMsg)
	ON_WM_CREATE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_OBJECTVIEW_EQUAL, OnObjectviewEqual)
	ON_UPDATE_COMMAND_UI(ID_OBJECTVIEW_EQUAL, OnUpdateObjectviewEqual)
	ON_COMMAND(ID_OBJECTVIEW_ONEVIEW, OnObjectviewOneview)
	ON_UPDATE_COMMAND_UI(ID_OBJECTVIEW_ONEVIEW, OnUpdateObjectviewOneview)
	ON_COMMAND(ID_OBJECTVIEW_VARIABLE, OnObjectviewVariable)
	ON_UPDATE_COMMAND_UI(ID_OBJECTVIEW_VARIABLE, OnUpdateObjectviewVariable)
	ON_WM_DESTROY()
	ON_UPDATE_COMMAND_UI_RANGE(AFX_IDW_PANE_FIRST, AFX_IDW_PANE_FIRST+256, OnUpdateCmdShowObjectPane)
	ON_COMMAND_RANGE(AFX_IDW_PANE_FIRST, AFX_IDW_PANE_FIRST+256, OnShowObjectPane)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnToolTipNotify)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////////////////
// CSplitterObjectView message handlers
void CSplitterObjectView::OnSize(UINT nType, int cx, int cy)
{
	if (theApp.GetMainFrame() 
		&& theApp.GetMainFrame()->IsSplitterWndCreated() 
		&& nType == SIZE_UPDATE
		&& cx > 0 && cy > 0
		&& theApp.m_dwClosing == 0)
	{
		int nRow			= 0,
			nRows			= m_nRows,
			nBarHeight		= 0,	
			nHeight			= cy - m_cySplitterGap * (nRows - 1),
			nRemainHeight	= cy - 1,
			nCaptionHeight	= 0,
			nDivisionMode = m_nDivisionMode;

		if (m_pViewCamera)
		{
			nCaptionHeight = m_pViewCamera->GetCaptionHeight();
		}
		m_szUpdate.cx = cx;
		m_szUpdate.cy = cy;

		if (m_pViewOEMDlg)
		{	
			nRows--;										// height of the OEM DlgBar is fixed
			if (m_bObjectViewVisible)						// is the ObjectView visible
			{
				nBarHeight = m_pViewOEMDlg->GetBarHeight();	// Last View has the height of the DlgBar
				nHeight -= nBarHeight;
				CSplitterWnd::SetRowInfo(m_nRows-1, nBarHeight,  nBarHeight-5);
			}
			else											// Show SubstituteDlg in full height
			{
				CSplitterWnd::SetRowInfo(m_nRows-1, cy, 1);
				for (nRow=0; nRow<nRows; nRow++)			// all Views have zero height
				{
					CSplitterWnd::SetRowInfo(nRow, 0, 0);
				}
				nDivisionMode = OVDM_MINIMIZED;
			}
		}

		if (nDivisionMode == OVDM_EQUAL_ARANGEMENT)
		{
			CWnd*pWnd;
			if (nHeight < 0 || nRows <= 0)
			{
				nHeight = 0;
			}
			else
			{
				int nFullRows = nRows;
				for (nRow=0; nRow<nRows; nRow++)
				{
					pWnd = GetPane(nRow, 0);
					ASSERT_KINDOF(CViewObjectTree, pWnd);
					if (((CViewObjectTree*)pWnd)->m_bMinimized)
					{
						nHeight -= nCaptionHeight;
						nFullRows--;
					}
				}
				ASSERT(nFullRows > 0);
				nHeight /= nFullRows;		// calculate equal height
			}
			for (nRow=0; nRow<nRows; nRow++)
			{
				pWnd = GetPane(nRow, 0);
				ASSERT_KINDOF(CViewObjectTree, pWnd);
				if (((CViewObjectTree*)pWnd)->m_bMinimized)
				{
					SetRowInfo(nRow, nCaptionHeight, nCaptionHeight, nRemainHeight);
				}
				else
				{
					SetRowInfo(nRow, nHeight, nCaptionHeight, nRemainHeight);
				}
			}
			if (nRemainHeight != nBarHeight)				// to prevent rounding errors
			{	// the last view (OEM Dialog) gets the rest, so add it to first visible
				CSplitterWnd::SetRowInfo(0, nHeight + (nRemainHeight - nBarHeight), nCaptionHeight);
			}
		}
		else if (nDivisionMode == OVDM_SHOW_ONE_VIEW)
		{
			int nID, nActiveRow = 0;
			for (nRow=0; nRow<nRows; nRow++)
			{
				nID = IdFromRowCol(nRow, 0);
				if (m_nActiveID == 0)
				{
					m_nActiveID = nID;
				}
				if (nID == m_nActiveID)	// the active view is set to the remaining height
				{
					nActiveRow = nRow;	
				}
				else					// the inactive show only the caption
				{
					SetRowInfo(nRow, nCaptionHeight, nCaptionHeight, nRemainHeight);
				}
			}
			nRemainHeight -= (nBarHeight + m_cySplitterGap);
			CSplitterWnd::SetRowInfo(nActiveRow, nRemainHeight, nCaptionHeight);
		}
		else if (nDivisionMode == OVDM_VARIABLE_ARANGEMENT)
		{
			int nCurrent;
			// set adjustable, only if there is enough space to do
			if (   nHeight < nRows * nCaptionHeight * 4 
				&& m_cySplitterGap > 1)
			{
				m_cySplitterGap = 1;
				nHeight	= cy - m_cySplitterGap * (nRows - 1);
			}
			else if (m_cySplitterGap == 1)
			{
				nHeight	= cy - m_cySplitterGap * (nRows - 1);
			}

			CWnd *pWnd = GetPane(0, 0);
			ASSERT_KINDOF(CViewObjectTree, pWnd);
			if (   ((CViewObjectTree*)pWnd)->m_nHeightInPromille == 0 
				&& nRows > 0
				&& nHeight > nCaptionHeight*nRows)
			{
				nCurrent = 1000 / nRows;		// initialize with equal arrangement
				for (nRow=0; nRow<nRows; nRow++)
				{
					pWnd = GetPane(nRow, 0);
					ASSERT_KINDOF(CViewObjectTree, pWnd);
					((CViewObjectTree*)pWnd)->m_nHeightInPromille = nCurrent;
				}
			}

			for (nRow=0; nRow<nRows; nRow++)
			{
				pWnd = GetPane(nRow, 0);
				ASSERT_KINDOF(CViewObjectTree, pWnd);
				nCurrent = MulDiv(((CViewObjectTree*)pWnd)->m_nHeightInPromille, nHeight, 1000);
				SetRowInfo(nRow, nCurrent, nCaptionHeight, nRemainHeight);
			}

			if (nRemainHeight != nBarHeight)				// to prevent rounding errors
			{	// the last view (OEM Dialog) gets the rest, so add it to first visible
				pWnd = GetPane(m_nRows-2, 0);
				nCurrent = MulDiv(((CViewObjectTree*)pWnd)->m_nHeightInPromille, nHeight, 1000);
				CSplitterWnd::SetRowInfo(m_nRows-2, nCurrent + (nRemainHeight - nBarHeight), nCaptionHeight);
			}
		}

		CSplitterWndCommon::OnSize(nType, cx, cy);
		for (nRow=0; nRow<m_nRows; nRow++)
		{
			CWnd *pWnd =GetPane(nRow, 0);
			ASSERT_KINDOF(CView, pWnd);
			((CView*)pWnd)->Invalidate(TRUE);
		}
		return;
	}

	CSplitterWndCommon::OnSize(nType, cx, cy);

}
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CSplitterObjectView::CreateView(int row, int col, CRuntimeClass* pViewClass, SIZE sizeInit, CCreateContext* pContext)
{
	BOOL bReturn = CSplitterWndCommon::CreateView(row, col, pViewClass, sizeInit, pContext);
	if (bReturn)
	{
		CWnd *pWnd =  GetPane(row, col);
		m_ViewList.AddTail(pWnd);
		if (pWnd->IsKindOf(RUNTIME_CLASS(CViewCamera)))
		{
			m_pViewCamera = (CViewCamera*)pWnd;
		}
		else if (pWnd->IsKindOf(RUNTIME_CLASS(CViewRelais)))
		{
			m_pViewRelais = (CViewRelais*)pWnd;
		}
		else if (pWnd->IsKindOf(RUNTIME_CLASS(CViewAlarmList)))
		{
			m_pViewAlarmList = (CViewAlarmList*)pWnd;
		}
		else if (pWnd->IsKindOf(RUNTIME_CLASS(CViewAudio)))
		{
			m_pViewAudio = (CViewAudio*)pWnd;
		}
		else if (pWnd->IsKindOf(RUNTIME_CLASS(CViewArchive)))
		{
			m_pViewArchive = (CViewArchive*)pWnd;
		}
		else if (pWnd->IsKindOf(RUNTIME_CLASS(CViewDialogBar)))
		{
			m_pViewOEMDlg = (CViewDialogBar*)pWnd;
		}
		else
		{
			ASSERT(FALSE);
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////////////////
void CSplitterObjectView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_nDivisionMode == OVDM_VARIABLE_ARANGEMENT)
	{
		CRect rc;
		GetClientRect(&rc);
		rc.bottom -= (m_pRowInfo[m_nRows-1].nCurSize + m_cySplitterGap*2);
		if (rc.PtInRect(point))
		{
			CSplitterWndCommon::OnLButtonDown(nFlags, point);
			m_nTrackingRow = m_htTrack - 101; // enum HitTestValue::vSplitterBar1 = 101
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
void CSplitterObjectView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_nDivisionMode == OVDM_VARIABLE_ARANGEMENT)
	{
		CRect rc;
		GetClientRect(&rc);
		rc.bottom -= (m_pRowInfo[m_nRows-1].nCurSize + m_cySplitterGap*2);
		if (rc.PtInRect(point))
		{
			CSplitterWndCommon::OnMouseMove(nFlags, point);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
void CSplitterObjectView::OnLButtonUp(UINT nFlags, CPoint point)
{
	int nCaptionHeight = GetSystemMetrics(SM_CYSMCAPTION);
	if (m_nTrackingRow != -1 && m_bObjectViewVisible)
	{
		int nRow, nRowOffset, nHeight;
		CWnd *pWnd;
		CViewObjectTree *pView1, *pView2;
		CRect rcClient;
		GetClientRect(&rcClient);
		nHeight = m_szUpdate.cy - m_cySplitterGap * (m_nRows-1);
		nHeight -= m_pViewOEMDlg->GetBarHeight();

		nCaptionHeight		= MulDiv(nCaptionHeight, 1000, nHeight);
		int nSplitterGap	= MulDiv(m_cySplitterGap, 1000, nHeight);
		int nMousePos		= MulDiv(point.y, 1000, nHeight);
		nRowOffset			= nSplitterGap;
		nCaptionHeight	   += nSplitterGap / 2;

		for (nRow=0; nRow<m_nRows-1; nRow++)
		{
			pWnd = GetPane(nRow, 0);
			ASSERT_KINDOF(CViewObjectTree, pWnd);
			pView1 = (CViewObjectTree*)pWnd;
			if (nRow == m_nTrackingRow)
			{
				pWnd = GetPane(++nRow, 0);
				ASSERT_KINDOF(CViewObjectTree, pWnd);
				pView2 = (CViewObjectTree*)pWnd;
				int nDiff,
					nOld1		= pView1->m_nHeightInPromille,
					nOld2		= pView2->m_nHeightInPromille;

				pView1->m_nHeightInPromille = max(nMousePos - nRowOffset, nCaptionHeight);
				nDiff = pView1->m_nHeightInPromille - nOld1;
				pView2->m_nHeightInPromille = nOld2 - nDiff;
				if (pView2->m_nHeightInPromille < nCaptionHeight)
				{
					pView1->m_nHeightInPromille = nOld1 + nOld2 - nCaptionHeight;
					pView2->m_nHeightInPromille = nCaptionHeight;
				}
				break;
			}
			nRowOffset += pView1->m_nHeightInPromille + nSplitterGap;
		}
#ifdef _DEBUG
		nHeight = 0;
		for (nRow=0; nRow<m_nRows-1; nRow++)
		{
			pWnd = GetPane(nRow, 0);
			ASSERT_KINDOF(CViewObjectTree, pWnd);
			pView1 = (CViewObjectTree*)pWnd;
			nHeight += pView1->m_nHeightInPromille;
		}
		ASSERT(IsBetween(nHeight, 999, 1001)); // 1 Promill Abweichung
#endif
		StopTracking(FALSE);
		Update();
	}
	m_nTrackingRow = -1;
//	CSplitterWndCommon::OnLButtonUp(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////////////////
int	CSplitterObjectView::GetMinWidth()
{
	if (m_pViewOEMDlg && m_bOEMDialogVisible)
	{
		return m_pViewOEMDlg->GetDlgInView()->GetWidth();
	}
	return 1;
}
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CSplitterObjectView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
//	return CSplitterWndCommon::OnEraseBkgnd(pDC);
}
/////////////////////////////////////////////////////////////////////////////////////////
LRESULT CSplitterObjectView::OnUserMsg(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
		case WM_UPDATE_ALL_VIEWS:	// this is only called in CMainFrame::OnCreateClient()
			UpdateVisibility();		// after creation of the last view
			if (m_eVisibility == OTVS_InitiallyNo)
			{
				AfxGetMainWnd()->PostMessage(WM_USER, WM_CLOSE, (LPARAM)this);
			}
			if (lParam)
			{
				Update();
			}
			break;
		case WM_ACTIVATE:
		{
			m_pViewSelected = (CViewObjectTree*)lParam;
			POSITION pos = m_ViewList.GetHeadPosition();
			while (pos)
			{
				CWnd*pWnd = m_ViewList.GetNext(pos);
				if (pWnd->IsKindOf(RUNTIME_CLASS(CViewObjectTree)))
				{
					CViewObjectTree*pView = (CViewObjectTree*)pWnd;
					if (pView != m_pViewSelected)
					{
						pView->SetSelected(FALSE);
					}
				}
			}
		}	break;
		case WM_DESTROY:
		{
			CWnd *pWnd = (CWnd*)lParam;
			if (pWnd)
			{
				POSITION posOld, pos = m_ViewList.GetHeadPosition();
				while (pos)
				{
					posOld  = pos;
					if (pWnd == m_ViewList.GetNext(pos))
					{
						m_ViewList.RemoveAt(posOld);
						break;
					}
				}
				if (pWnd->IsKindOf(RUNTIME_CLASS(CViewCamera)))
				{
					m_pViewCamera = NULL;
				}
				else if (pWnd->IsKindOf(RUNTIME_CLASS(CViewRelais)))
				{
					m_pViewRelais = NULL;
				}
				else if (pWnd->IsKindOf(RUNTIME_CLASS(CViewAlarmList)))
				{
					m_pViewAlarmList = NULL;
				}
				else if (pWnd->IsKindOf(RUNTIME_CLASS(CViewAudio)))
				{
					m_pViewAudio = NULL;
				}
				else if (pWnd->IsKindOf(RUNTIME_CLASS(CViewArchive)))
				{
					m_pViewArchive = NULL;
				}
				else if (pWnd->IsKindOf(RUNTIME_CLASS(CViewDialogBar)))
				{
					m_pViewOEMDlg = NULL;
				}
			}

		} break;
	}
	return 1;
}
/////////////////////////////////////////////////////////////////////////////////////////
int CSplitterObjectView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CSplitterWndCommon::OnCreate(lpCreateStruct) == -1)
		return -1;

	CString sTitle;
	sTitle.LoadString(IDS_OBJECT_VIEW);
	SetWindowText(sTitle);

	ASSERT(m_nRows > 1 && m_nCols == 1);
	
	if (!theApp.IsReadOnlyModus())
	{
		CRuntimeClass*pRTC = GetRuntimeClass();
		if (pRTC)
		{
			CWK_Profile wkp;
			CString sSection(&pRTC->m_lpszClassName[1]);
			m_eVisibility = (eVisibilityStates)wkp.GetInt(IDIP_CLIENT_VISIBILITY, sSection, m_eVisibility);
			sSection += _T("DM");
			int nDM = wkp.GetInt(IDIP_CLIENT_VISIBILITY, sSection, m_nDivisionMode);
			SetDivisionMode((eObjViewDivMode)nDM);
		}
	}
	EnableToolTips(TRUE);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////
void CSplitterObjectView::SetDivisionMode(eObjViewDivMode nDM)
{
	if (m_nDivisionMode != nDM)
	{
		m_nDivisionMode		= nDM;
		if (m_nDivisionMode == OVDM_VARIABLE_ARANGEMENT)
		{
			m_cySplitterGap		= 7;
			m_cySplitter		= 7;
		}
		else
		{
			m_cySplitterGap		= 1;
			m_cySplitter		= 1;
		}
		if (m_nDivisionMode == OVDM_EQUAL_ARANGEMENT)
		{
			UpdateVisibility();
		}
		Update();
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
void CSplitterObjectView::Update()
{
	if (m_hWnd && m_szUpdate.cx && m_szUpdate.cy && theApp.m_dwClosing == 0)
	{
		PostMessage(WM_SIZE, SIZE_UPDATE, MAKELPARAM(m_szUpdate.cx, m_szUpdate.cy));
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
void CSplitterObjectView::OnObjectviewEqual()
{
	SetDivisionMode(OVDM_EQUAL_ARANGEMENT);
}
/////////////////////////////////////////////////////////////////////////////////////////
void CSplitterObjectView::OnUpdateObjectviewEqual(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	pCmdUI->SetRadio(OVDM_EQUAL_ARANGEMENT == m_nDivisionMode);
}
/////////////////////////////////////////////////////////////////////////////////////////
void CSplitterObjectView::OnObjectviewOneview()
{
	SetDivisionMode(OVDM_SHOW_ONE_VIEW);
}
/////////////////////////////////////////////////////////////////////////////////////////
void CSplitterObjectView::OnUpdateObjectviewOneview(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	pCmdUI->SetRadio(OVDM_SHOW_ONE_VIEW == m_nDivisionMode);
}
/////////////////////////////////////////////////////////////////////////////////////////
void CSplitterObjectView::OnObjectviewVariable()
{
	SetDivisionMode(OVDM_VARIABLE_ARANGEMENT);
}
/////////////////////////////////////////////////////////////////////////////////////////
void CSplitterObjectView::OnUpdateObjectviewVariable(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	pCmdUI->SetRadio(OVDM_VARIABLE_ARANGEMENT == m_nDivisionMode);
}
/////////////////////////////////////////////////////////////////////////////////////////
void CSplitterObjectView::OnDestroy()
{
	CSplitterWndCommon::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////////////////
void CSplitterObjectView::SetActiveID(int nID)
{
	m_nActiveID = nID;
	Update();
}
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CSplitterObjectView::SetActiveView(CRuntimeClass* pRTC)
{
	int nRow;
	for (nRow=0; nRow<m_nRows; nRow++)
	{
		int nID = IdFromRowCol(nRow, 0);
		CWnd *pWnd = GetDlgItem(nID);
		if (pWnd && pWnd->IsKindOf(pRTC))
		{
			m_nActiveID = nID;
			return TRUE;
		}
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////////////////
void CSplitterObjectView::ShowObjectView(BOOL bShow)
{
	m_bObjectViewVisible = bShow;
	m_pViewOEMDlg->ShowDialog(m_bObjectViewVisible && m_bOEMDialogVisible);
	int nShowCmd = 0;
	if (!bShow)
	{
		m_cySplitterGap = 1;
		nShowCmd = SW_HIDE;
	}
	else
	{
		SetDivisionMode(m_nDivisionMode);
		nShowCmd = SW_SHOW;
	}
	POSITION pos = m_ViewList.GetHeadPosition();
	while (pos)
	{
		CWnd*pWnd = m_ViewList.GetNext(pos);
		if (pWnd->IsKindOf(RUNTIME_CLASS(CViewObjectTree)))
		{
			CViewObjectTree*pView = (CViewObjectTree*)pWnd;
			if (pView->IsWindowVisible())
			{
				pView->m_btnMiniMize.ShowWindow(nShowCmd);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
void CSplitterObjectView::ShowOEMDialog(BOOL bShow)
{
	m_bOEMDialogVisible = bShow;
	m_pViewOEMDlg->ShowDialog(m_bObjectViewVisible && m_bOEMDialogVisible);
}
/////////////////////////////////////////////////////////////////////////////////////////
UINT CSplitterObjectView::GetSubstCmdID()
{
	if (!m_bObjectViewVisible)
	{
		return ID_VIEW_OBJECT_VIEW;
	}
	else
	{
		return ID_VIEW_OEMDIALOG;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
CImageList* CSplitterObjectView::GetSubstImageList(int nID)
{
	if (!m_bObjectViewVisible)
	{
		return (nID == IdFromRowCol(0, 0)) ? CSkinDialog::GetRightButtonImages(): CSkinDialog::GetLeftButtonImages();
	}
	else
	{
		return CSkinDialog::GetUpButtonImages();
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CSplitterObjectView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	CWnd *pWndOwner = NULL,
		 *pWnd = CWnd::FromHandlePermanent((HWND)lParam);
	
	if (pWnd)
	{
		pWndOwner = pWnd->GetOwner();	// get the owner view
	}

	WORD wCmd = LOWORD(wParam);
	switch (wCmd)
	{
		/////////////////////////////////////////////////////////////////////////////////////////
		case IDC_MINIMIZE_SPLITTER:		// minimize the object splitter
		theApp.GetMainFrame()->PostMessage(WM_USER, WM_CLOSE, (LPARAM)this);
		break;
		/////////////////////////////////////////////////////////////////////////////////////////
		case IDC_MENU_BTN:		// show context menu
		{
			CMenu menu;
			menu.LoadMenu(IDR_MENU_LIVE);
			CSkinMenu *pPopup = (CSkinMenu*) menu.GetSubMenu(SUB_MENU_OBJECT_VIEWS);
			ASSERT(pPopup != NULL);
			CPoint point;
			if (pWnd)			// calculate coordinates
			{
				CRect rc;
				pWnd->GetWindowRect(&rc);
				point.x = rc.left;
				point.y = rc.bottom;
			}
			else
			{
				GetCursorPos(&point);
			}
			UINT nID = ID_VIEW_OEMDIALOG;
			CString str;		// Find menu entry
			if (((CSkinMenu*)pPopup)->FindMenuString(nID, str))
			{
				int nIDWnd;
				POSITION pos = m_ViewList.GetHeadPosition();
				while (pos)
				{
					CWnd*pWnd = m_ViewList.GetNext(pos);
					if (pWnd->IsKindOf(RUNTIME_CLASS(CViewObjectTree)))
					{	// and insert the views that may be shown or hidden
						switch(((CViewObjectTree*)pWnd)->m_eVisibility)
						{
							case OTVS_Always:
								continue;
								break;
// Never still means never, but some users where confused, because they
// did not find the views they had hidden permanently
							case OTVS_Never:
								if (theApp.IsReadOnlyModus())
								{
									continue;
								}break;
							default:
								break;
						}
						nIDWnd = pWnd->GetDlgCtrlID();
						pWnd->GetWindowText(str);
						pPopup->InsertMenu(nID++, MF_BYPOSITION, nIDWnd, str);
					}
				}
			}

			if (pWndOwner && pWndOwner->IsKindOf(RUNTIME_CLASS(CViewObjectTree)))
			{	// Modify the context menu in view dependency
				((CViewObjectTree*)pWndOwner)->ModifyContextMenu(pPopup);
			}

			COemGuiApi::DoUpdatePopupMenu(AfxGetMainWnd(), pPopup);

			pPopup->ConvertMenu(TRUE);
			nID = pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RETURNCMD, point.x, point.y, AfxGetMainWnd());
			pPopup->ConvertMenu(FALSE);
			if (nID)
			{
				if (nID == ID_OBJECTVIEW_ONEVIEW && pWndOwner)
				{	// set the owner of the menu button as active view
					int nOld = m_nActiveID;	
					m_nActiveID = pWndOwner->GetDlgCtrlID();
					if (   m_nDivisionMode == OVDM_SHOW_ONE_VIEW // already this mode
						&& m_nActiveID != nOld)					// but other ID
					{
						theApp.GetMainFrame()->PostMessage(WM_USER, MAKELONG(WM_ACTIVATE, 2), (LPARAM)pWndOwner);
						Update();								// update
					}
				}
				theApp.GetMainFrame()->PostMessage(WM_COMMAND, nID, lParam);
			}

		}break;
		/////////////////////////////////////////////////////////////////////////////////////////
		case IDC_CLOSE_VIEW:	// show or hide the owned pane
		if (pWndOwner)
		{
			OnShowObjectPane((UINT)pWndOwner->GetDlgCtrlID());
			theApp.GetMainFrame()->PostMessage(WM_USER, MAKELONG(WM_ACTIVATE, 0), (LPARAM)pWndOwner);
		}break;
		/////////////////////////////////////////////////////////////////////////////////////////
		case IDC_TITLE_BTN:	// minimize or maximize the owned pane
		if (pWndOwner && pWndOwner->IsKindOf(RUNTIME_CLASS(CViewObjectTree)))
		{
			if (m_nDivisionMode == OVDM_SHOW_ONE_VIEW)
			{				// maximize the active pane
				if (pWndOwner->GetDlgCtrlID() != m_nActiveID)
				{
					m_nActiveID = pWndOwner->GetDlgCtrlID();
					theApp.GetMainFrame()->PostMessage(WM_USER, MAKELONG(WM_ACTIVATE, 2), (LPARAM)pWndOwner);
					Update();
				}
			}
			else if (m_nDivisionMode == OVDM_EQUAL_ARANGEMENT)
			{
				CViewObjectTree *pView = (CViewObjectTree*)pWndOwner;
				if (   pView->m_eVisibility != OTVS_Always 
					|| (theApp.IsReadOnlyModus() && pView->m_eVisibility != OTVS_Never))
				{	// when allowed: change minimize state
					pView->m_bMinimized = !pView->m_bMinimized;
					theApp.GetMainFrame()->PostMessage(WM_USER, MAKELONG(WM_ACTIVATE, pView->m_bMinimized ? 0 : 2), (LPARAM)pView);
					UpdateVisibility();
					Update();
				}
			}
		}break;
	}
	return CSplitterWndCommon::OnCommand(wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////
void CSplitterObjectView::OnShowObjectPane(UINT nID)
{
	CWnd *pWnd = GetDlgItem(nID);
	if (   pWnd												// valid
		&& pWnd->IsKindOf(RUNTIME_CLASS(CViewObjectTree))	// only Views
		&& (   m_nRows > 2									// there must be at least one View
		|| !pWnd->IsWindowVisible()))						// or increase visible views
	{
		CViewObjectTree *pView = (CViewObjectTree*)pWnd;
		if (pView->OnShowObjectPane())						// show/hide
		{
			if (  !pView->IsWindowVisible()					// a hidden view
				&& m_nActiveID == (int)nID)					// cannot be active
			{
				m_nActiveID = 0;							// set active ID to 0
			}

			UpdateVisibility();								// sort the dlg ids
			Update();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSplitterObjectView::OnUpdateCmdShowObjectPane(CCmdUI *pCmdUI)
{
	CWnd *pWnd = GetDlgItem(pCmdUI->m_nID);
	if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CViewObjectTree)))
	{	// distribute to child panes
		((CViewObjectTree*)pWnd)->OnUpdateCmdShowObjectPane(pCmdUI);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSplitterObjectView::SetRowInfo(int nRow, int nIdealHeight, int nMinHeight, int&nRemainHeight)
{
	CSplitterWnd::SetRowInfo(nRow, nIdealHeight, nMinHeight);
	nRemainHeight -= nIdealHeight;
	nRemainHeight -= m_cySplitterGap;
}
/////////////////////////////////////////////////////////////////////////////
void CSplitterObjectView::UpdateVisibility()
{
	if (m_hWnd && theApp.m_dwClosing == 0)
	{
		m_nRows = m_ViewList.GetCount();
		POSITION pos = m_ViewList.GetHeadPosition();
		int i, nVisible = 0, nNonVisible = m_nRows-1;
		int nFullHeight = 0;
		while (pos)	// sort the Panes by visibility
		{
			CWnd*pWnd = m_ViewList.GetNext(pos);	// original order is in ViewList
			if (pWnd->IsWindowVisible())			// the visible before
			{
				pWnd->SetDlgCtrlID(IdFromRowCol(nVisible++, 0));
				if (pWnd->IsKindOf(RUNTIME_CLASS(CViewObjectTree)))
				{
					CViewObjectTree *pView = (CViewObjectTree*)pWnd;
					pView->m_nHeightInPromille = 0;
					if (!pView->m_bMinimized)
					{
						nFullHeight++;
					}
				}
			}
			else									// the non visible
			{
				if (m_pViewSelected == pWnd)
				{
					m_pViewSelected = NULL;
				}
				pWnd->SetDlgCtrlID(IdFromRowCol(nNonVisible--, 0));
			}
		}

		if (   m_nDivisionMode == OVDM_EQUAL_ARANGEMENT
			&& nFullHeight < nVisible)				// sort the visible by minimized
		{
			pos = m_ViewList.GetHeadPosition();
			i = 0;
			while (pos)
			{
				CWnd*pWnd = m_ViewList.GetNext(pos);
				if (!pWnd->IsWindowVisible()) continue;// forget the non visible
				if (pWnd->IsKindOf(RUNTIME_CLASS(CViewObjectTree)))
				{									// sort only views
					CViewObjectTree *pView = (CViewObjectTree*)pWnd;
					pView->EnableCloseButton(nVisible > 2 ? TRUE : FALSE);
					if (nFullHeight == 0 && pView->m_bMinimized)
					{								// at least on must have full height
						pView->m_bMinimized = FALSE;
						nFullHeight++;
					}
					if (pView->m_bMinimized)		// minimized after
					{
						pWnd->SetDlgCtrlID(IdFromRowCol(nFullHeight++, 0));
						if (m_pViewSelected == pWnd)
						{
							m_pViewSelected = NULL;
						}
					}
					else							// non minimized windows
					{
						pWnd->SetDlgCtrlID(IdFromRowCol(i++, 0));
					}
				}
			}
		}

		if (m_pViewSelected == NULL)
		{
			CWnd *pWnd = GetPane(0, 0);
			if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CViewObjectTree)))
			{
				m_pViewSelected = (CViewObjectTree*)pWnd;
				m_pViewSelected->SetSelected(TRUE);
			}
		}

		if (m_pViewSelected)
		{
			if (m_nActiveID == 0)
			{
				m_nActiveID = m_pViewSelected->GetDlgCtrlID();
			}
			else
			{
				CWnd *pWnd = GetDlgItem(m_nActiveID);
				if (pWnd)
				{
					if (!pWnd->IsWindowVisible())
					{
						m_nActiveID = m_pViewSelected->GetDlgCtrlID();
					}
					else if (pWnd->IsKindOf(RUNTIME_CLASS(CViewObjectTree)))
					{
						CViewObjectTree *pView = (CViewObjectTree*)pWnd;
						if (pView->m_bMinimized)
						{
							m_nActiveID = m_pViewSelected->GetDlgCtrlID();
						}
					}
				}
			}
		}
		else
		{
			ASSERT(FALSE);
		}
		m_nRows = nVisible;
/*
#ifdef _DEBUG
	TRACE(_T("UpdateVisibility():\n"));
	for (i=0; i<m_ViewList.GetCount(); i++)
	{
		CWnd *pWnd = GetPane(i, 0);
		if (pWnd)
		{
			CString str(pWnd->GetRuntimeClass()->m_lpszClassName);
			TRACE(_T("%d:%s\n"), i, str); 
		}
	}
#endif
*/
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSplitterObjectView::OnToolTipNotify(UINT /*id*/, NMHDR *pNMHDR, LRESULT *pResult)
{
	// need to handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*)pNMHDR;
	CString strTipText;
	UINT nID = (UINT)pNMHDR->idFrom;
	if (pNMHDR->code == TTN_NEEDTEXT && (pTTT->uFlags & TTF_IDISHWND))
	{
		// idFrom is actually the HWND of the tool
		nID = ::GetDlgCtrlID(reinterpret_cast<HWND>(nID));
	}

	if (nID != 0)
	{
		if (strTipText.LoadString(nID))
		{
			int nFind = strTipText.Find(_T("\n"));
			if (nFind != -1)
			{
				m_strTipText = strTipText.Left(nFind);
			}
			else
			{
				m_strTipText = strTipText;
			}
		}
		else
		{
			m_strTipText.Empty();
			return FALSE;
		}
	}
	pTTT->lpszText = m_strTipText.GetBuffer();
	m_strTipText.ReleaseBuffer();
	if (CSkinDialog::UseGlobalFonts() && pNMHDR->hwndFrom)
	{
		::SendMessage(pNMHDR->hwndFrom, WM_SETFONT, (WPARAM)CSkinDialog::GetDialogItemGlobalFont()->GetSafeHandle(), 0);
	}

	*pResult = 0;

	return TRUE;    // message was handled
}
/////////////////////////////////////////////////////////////////////////////
void CSplitterObjectView::SetVisibility(eVisibilityStates eVisibility)
{
	m_eVisibility = eVisibility;
}
/////////////////////////////////////////////////////////////////////////////
void CSplitterObjectView::SaveSettings()
{
	CRuntimeClass*pRTC = GetRuntimeClass();
	if (pRTC)
	{
		CWK_Profile wkp;
		CString sSection(&pRTC->m_lpszClassName[1]);
		wkp.WriteInt(IDIP_CLIENT_VISIBILITY, sSection, m_eVisibility);
		sSection += _T("DM");
		wkp.WriteInt(IDIP_CLIENT_VISIBILITY, sSection, m_nDivisionMode);
	}
}
/////////////////////////////////////////////////////////////////////////////
CViewObjectTree* CSplitterObjectView::GetSelectedServerAndView(WORD &wServer)
{
	if (m_bObjectViewVisible && IsWindowVisible())
	{
		POSITION pos = m_ViewList.GetHeadPosition();
		while (pos)
		{
			CWnd*pWnd = m_ViewList.GetNext(pos);
			if (pWnd->IsKindOf(RUNTIME_CLASS(CViewObjectTree)))
			{
				CViewObjectTree*pView = (CViewObjectTree*)pWnd;
				if (pView->IsWindowVisible() && !pView->IsMinimized())
				{
					wServer = pView->GetSelectedServer();
					if (wServer != SECID_NO_SUBID)
					{
						return pView;
					}
				}
			}
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSplitterObjectView::OnTab(BOOL bNext, BOOL bSelect)
{
	CViewObjectTree *pSelected = GetViewSelected();
	if (pSelected)
	{
		if (bSelect)
		{
			CTreeCtrl&theTree = pSelected->GetTreeCtrl();
			HTREEITEM hParent, hItem = theTree.GetSelectedItem();
			while (hItem)
			{
				hParent = theTree.GetParentItem(hItem);
				if (hParent == NULL) break;
				hItem = hParent;
			}
			if (hItem)
			{
				hItem = theTree.GetNextItem(hItem, TVGN_NEXT);
			}

			if (hItem==NULL)
			{
				hItem = theTree.GetRootItem();
			}
			theTree.SelectItem(hItem);

			return TRUE;
		}
		else
		{
			BOOL bFound = FALSE;
			if (bNext < 0)
			{
				bFound = TRUE;
				if (bNext == -2) bNext = FALSE;
			}
			POSITION pos = bNext ? m_ViewList.GetHeadPosition() : m_ViewList.GetTailPosition();
			while (pos)
			{
				CWnd*pWnd = bNext ? m_ViewList.GetNext(pos) : m_ViewList.GetPrev(pos);
				if (bFound)
				{
					if (pWnd && pWnd->IsWindowVisible() && pWnd->IsKindOf(RUNTIME_CLASS(CViewObjectTree)))
					{
						((CViewObjectTree*)pWnd)->SetSelected(TRUE);
						return TRUE;
					}
				}
				else if (pWnd == pSelected)
				{
					bFound = TRUE;
				}
			}
			pSelected->SetSelected(FALSE);
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
CViewObjectTree*CSplitterObjectView::GetViewSelected()	const
{
	if (m_bObjectViewVisible && IsWindowVisible())
	{
		return m_pViewSelected;
	}
	return NULL;
}
