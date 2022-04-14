// GraphCreatorView.cpp : implementation of the CGraphCreatorView class
//

#include "stdafx.h"
#include "GraphCreator.h"

#include "GraphCreatorDoc.h"
#include "GraphCreatorView.h"
#include "..\FindFilter\FindFilterDlg.h"

#include "FilterButton.h"
#include "PinConnectionCtrl.h"
#include ".\graphcreatorview.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

struct SPrintParam
{
	CDC* pDC;
	CPrintInfo* pInfo;
};


/////////////////////////////////////////////////////////////////////////////
// CGraphCreatorView
IMPLEMENT_DYNCREATE(CGraphCreatorView, CFormView)
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CGraphCreatorView, CFormView)
	//{{AFX_MSG_MAP(CGraphCreatorView)
	ON_UPDATE_COMMAND_UI(ID_INSERT_FILTER, OnUpdateInsertFilter)
	ON_COMMAND(ID_INSERT_FILTER, OnInsertFilter)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
	ON_MESSAGE(WM_USER, OnUser)
	ON_MESSAGE(WM_HRESULT_ERROR, OnHresultError)
	ON_MESSAGE(WM_GRAPHNOTIFY, OnGraphNotify)
	ON_COMMAND(ID_GRAPH_START, OnGraphStart)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_START, OnUpdateGraphStart)
	ON_COMMAND(ID_GRAPH_STOP, OnGraphStop)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_STOP, OnUpdateGraphStop)
	ON_COMMAND(ID_GRAPH_PAUSE, OnGraphPause)
	ON_UPDATE_COMMAND_UI(ID_GRAPH_PAUSE, OnUpdateGraphPause)
	ON_UPDATE_COMMAND_UI(ID_FILE_RENDER_MEDIA, OnUpdateFileRenderMedia)
	ON_COMMAND(ID_VIEW_REARRANGE_FILTERS, OnViewRearrangeFilters)
	ON_UPDATE_COMMAND_UI(ID_VIEW_REARRANGE_FILTERS, OnUpdateViewRearrangeFilters)
	//}}AFX_MSG_MAP
	// Standard printing commands
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CGraphCreatorView construction/destruction
CGraphCreatorView::CGraphCreatorView()
	: CFormView(CGraphCreatorView::IDD)
{
	//{{AFX_DATA_INIT(CGraphCreatorView)
	//}}AFX_DATA_INIT
   //m_pPrintPen = NULL;
}
/////////////////////////////////////////////////////////////////////////////
CGraphCreatorView::~CGraphCreatorView()
{
	WK_DELETE(m_pDlgFilters);
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGraphCreatorView)
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BOOL CGraphCreatorView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CFormView::PreCreateWindow(cs);
}
/////////////////////////////////////////////////////////////////////////////
// CGraphCreatorView diagnostics
#ifdef _DEBUG
void CGraphCreatorView::AssertValid() const
{
	CFormView::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
/////////////////////////////////////////////////////////////////////////////
CGraphCreatorDoc* CGraphCreatorView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGraphCreatorDoc)));
	return (CGraphCreatorDoc*)m_pDocument;
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
IGraphBuilder* CGraphCreatorView::GetGraph()
{
	return GetDocument()->m_pGraph;
}
/////////////////////////////////////////////////////////////////////////////
// CGraphCreatorView implementation
void CGraphCreatorView::UpdateGraphFilters()
{
	if (GetDocument()->m_pGraph)
	{
		HRESULT       hr;
		IEnumFilters *pEnum = NULL;
		IBaseFilter  *pFilter = NULL;
		ULONG cFetched;

		if (GetDocument()->m_pGraph)
		{
			hr = GetDocument()->m_pGraph->EnumFilters(&pEnum);
			if (SUCCEEDED(hr))
			{
				while(pEnum->Next(1, &pFilter, &cFetched) == S_OK)
				{
					FILTER_INFO FilterInfo;
					hr = pFilter->QueryFilterInfo(&FilterInfo);
					if (SUCCEEDED(hr))
					{
						SFindFilterCtrl ffc(pFilter);
						EnumChildWindows(m_hWnd, FindFilterCtrls, (LPARAM) &ffc);
						if (ffc.nID == 0)
						{
							CFilterCtrl *pBtn = new CFilterCtrl(this, pFilter, CString(FilterInfo.achName));
						}
					}
					RELEASE_OBJECT(pFilter);
					RELEASE_OBJECT(FilterInfo.pGraph);
				}
				pEnum->Release();
			}
		}
		EnumChildWindows(m_hWnd, FindPinConnections, (LPARAM) this);
		if (theApp.m_bAutoRearrangeFilters)
		{
			ReArrangeFilters();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::ReArrangeFilters()
{
	CPtrList list;
	CWnd*pWnd;
	CRect	rcBounding(0,0,0,0);
	EnumChildWindows(m_hWnd, EnumSourceFilters, (LPARAM)&list);
	POSITION pos = list.GetHeadPosition();
	while (pos)
	{
		pWnd = (CWnd*)list.GetNext(pos);
		if (pWnd)
		{
			CBaseCtrl*pCtrl = (CBaseCtrl*)pWnd;
			m_rcBounding.SetRectEmpty();
			m_rcBounding.OffsetRect(0, rcBounding.bottom+5);
			// width is 0
			ReArrangeFilters(pCtrl, &m_rcBounding);
			rcBounding.UnionRect(&rcBounding, m_rcBounding);
		}
	};

	rcBounding.InflateRect(10,10);
	m_totalLog = rcBounding.Size();
/*
	CWnd *pMW = AfxGetMainWnd();
	if ((pMW->GetStyle() & WS_MAXIMIZE) == 0)
	{
		MoveWindow(&rcBounding);
		GetParentFrame()->RecalcLayout();
		ResizeParentToFit(FALSE);
	}
*/
}
/////////////////////////////////////////////////////////////////////////////
CRect CGraphCreatorView::ReArrangeFilters(CBaseCtrl*pCtrl, CRect *prcPrev)
{
	ASSERT(pCtrl != NULL);
	if (pCtrl->IsKindOf(RUNTIME_CLASS(CFilterCtrl)))
	{
		CRect rcCtrl, rcTemp, rcNext;
		CBaseCtrl *pCtrlI = NULL;
		pCtrl->GetWindowRect(&rcCtrl);
		ScreenToClient(rcCtrl);
		CPoint ptTL = -rcCtrl.TopLeft();
		if (prcPrev->Width() > 0)
		{
			CPoint ptTL = -rcCtrl.TopLeft();
			rcCtrl.OffsetRect(ptTL);
			rcCtrl.OffsetRect(prcPrev->right + CBaseCtrl::gm_nFilterDistance, prcPrev->top);
		}
		else
		{
			ptTL += CPoint(5,prcPrev->bottom+5);
			rcCtrl.OffsetRect(ptTL);
		}
		pCtrl->MoveWindow(&rcCtrl);
		m_rcBounding.UnionRect(&m_rcBounding, &rcCtrl);
		int i, n = pCtrl->GetNoOfNext(), nOffsetX = 0;
		rcTemp = rcCtrl;
		if (n>2)
		{
			rcTemp.OffsetRect((n-2)*CBaseCtrl::gm_nFilterDistance, 0);
			nOffsetX = -CBaseCtrl::gm_nFilterDistance;
		}
		for (i=0; i<n; i++)
		{
			pCtrlI = pCtrl->GetNextCtrl(i);
			rcNext = ReArrangeFilters(pCtrlI, &rcTemp);
			rcTemp.OffsetRect(nOffsetX, rcNext.Height() + CBaseCtrl::gm_nFilterDistance);
		}
		return rcCtrl;
	}
	else
	{
		return ReArrangeFilters(pCtrl->GetNextCtrl(0), prcPrev);
	}
}
/////////////////////////////////////////////////////////////////////////////
UINT CGraphCreatorView::GetFreeFilterID()
{
	UINT i;
	for (i=FIRST_FILTER_ID; i<FIRST_CONNECTION_ID; i++)
	{
		if (GetDlgItem(i) == 0)
		{
			return i;
		}
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
UINT CGraphCreatorView::GetFreeConnectionID()
{
	UINT i;
	for (i=FIRST_CONNECTION_ID; i<LAST_CONNECTION_ID; i++)
	{
		if (GetDlgItem(i) == 0)
		{
			return i;
		}
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CGraphCreatorView::GetCtrlRect(UINT nID, CRect*prcCtrl)
{
	CWnd*pWnd = GetDlgItem(nID);
	if (pWnd)
	{
		pWnd->GetClientRect(prcCtrl);
		pWnd->MapWindowPoints(this, prcCtrl);
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK CGraphCreatorView::FindPinConnections(HWND hwnd, LPARAM lParam)
{
	CGraphCreatorView *pView = (CGraphCreatorView*)lParam;
	CWnd *pWnd = CWnd::FromHandle(hwnd);
	if (pWnd->IsKindOf(RUNTIME_CLASS(CFilterCtrl)))
	{
		CFilterCtrl*pF = (CFilterCtrl*)pWnd;
		pF->CheckConnections();
	}
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK CGraphCreatorView::FindFilterCtrls(HWND hwnd, LPARAM lParam)
{
	SFindFilterCtrl *pffc = (SFindFilterCtrl*)lParam;;
	CWnd *pWnd = CWnd::FromHandle(hwnd);
	if (pWnd->IsKindOf(RUNTIME_CLASS(CFilterCtrl)))
	{
		if (pffc == NULL)
		{
			pWnd->DestroyWindow();
		}
		else if (((CFilterCtrl*)pWnd)->GetFilter() == pffc->pFilter)
		{
			pffc->nID = pWnd->GetDlgCtrlID();
			return 0;
		}
	}
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK CGraphCreatorView::RemoveCtrlID(HWND hwnd, LPARAM lParam)
{
	CWnd *pWnd = CWnd::FromHandle(hwnd);
	if (pWnd->IsKindOf(RUNTIME_CLASS(CFilterCtrl)))
	{
		((CBaseCtrl*)pWnd)->RemoveCtrlID(lParam);
	}
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK CGraphCreatorView::DrawCtrls(HWND hwnd, LPARAM lParam)
{
	SPrintParam*pPP = (SPrintParam*)lParam;
	CWnd *pWnd = CWnd::FromHandle(hwnd);
	if (pWnd->IsKindOf(RUNTIME_CLASS(CBaseCtrl)))
	{
		DRAWITEMSTRUCT dis;
		ZERO_INIT(dis);
		dis.CtlID = pWnd->GetDlgCtrlID();
		dis.CtlType = ODT_BUTTON;
		dis.hDC = pPP->pDC->m_hDC;
		dis.hwndItem = pWnd->m_hWnd;
		dis.itemAction = ODA_DRAWENTIRE;
		((CBaseCtrl*)pWnd)->DrawItem(&dis);
	}
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK CGraphCreatorView::EnumSourceFilters(HWND hwnd, LPARAM lParam)
{
	CPtrList *pPtrList = (CPtrList*)lParam;
	CWnd *pWnd = CWnd::FromHandle(hwnd);
	if (   pWnd->IsKindOf(RUNTIME_CLASS(CFilterCtrl)) 
		&& ((CFilterCtrl*)pWnd)->GetNoOfPrev() == 0)
	{
		pPtrList->AddTail(pWnd);
	}
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
// CGraphCreatorView virtual fucntions
void CGraphCreatorView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	if (GetDocument()->m_pMediaEvent)
	{
		GetDocument()->m_pMediaEvent->SetNotifyWindow((OAHWND)m_hWnd, WM_GRAPHNOTIFY, 0);
	}
}
/////////////////////////////////////////////////////////////////////////////
// CGraphCreatorView printing
BOOL CGraphCreatorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	pInfo->m_pPD->m_pd.Flags |= PD_RETURNDC;
	BOOL bOK = DoPreparePrinting(pInfo);
	if (bOK)
	{
		CDC* pDC = CDC::FromHandle(pInfo->m_pPD->GetPrinterDC());
		CRect rcPage (0, 0, pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));
		CRect rcM(100, 100, 100, 100);// = theApp.GetPrintMargin(pDC);
		rcPage.DeflateRect(rcM.left, rcM.top, rcM.right, rcM.bottom);
		m_totalDev = rcPage.Size();
		m_nMapMode = -1;
		pInfo->SetMaxPage(1);
	}
	return bOK;
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	if (pDC->IsPrinting())
	{
		pDC->SetMapMode(MM_ISOTROPIC);
		pDC->SetWindowExt(m_totalLog);  // window is in logical coordinates
		pDC->SetViewportExt(m_totalDev);
	}
	else
	{
		CFormView::OnPrepareDC(pDC, pInfo);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
   m_PrintPen.CreatePen(PS_SOLID, 2, RGB(0,0,0));
	SPrintParam pp = {pDC, pInfo};
	EnumChildWindows(m_hWnd, DrawCtrls, (LPARAM) &pp);
   m_PrintPen.DeleteObject();
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	m_nMapMode = MM_TEXT;
}
/////////////////////////////////////////////////////////////////////////////
// CGraphCreatorView message handlers
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::OnUpdateInsertFilter(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::OnInsertFilter() 
{
	if (m_pDlgFilters->m_hWnd)
	{
		m_pDlgFilters->ShowWindow(SW_RESTORE);
	}
	else
	{
		m_pDlgFilters->Create(CFindFilterDlg::IDD, this);
	}
}
/////////////////////////////////////////////////////////////////////////////
int CGraphCreatorView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	CDC *pDC = GetDC();
	int nSize = theApp.GetProfileInt(SETTINGS_SECTION, CTRL_FONT_SIZE, 8*10);
	CString sFaceName = theApp.GetProfileString(SETTINGS_SECTION, CTRL_FONT, _T("Arial"));
	m_Font.CreatePointFont(nSize, sFaceName, pDC);
	ReleaseDC(pDC);
	SetFont(&m_Font);

	m_pDlgFilters = new CFindFilterDlg(this);
	
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::OnDestroy() 
{
	TRACE("CGraphCreatorView::OnDestroy()\n");
	EnumChildWindows(m_hWnd, FindFilterCtrls, (LPARAM) 0);
	GetDocument()->ReleaseBasicFilters();
	CFormView::OnDestroy();
}
//////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::ReportError(ErrorStruct*ps, bool bMsgBox)
{
	ASSERT(ps != NULL);
	if (FAILED(ps->m_hr))
	{
		CString str, sError, sDescription;
		GetErrorStrings(ps->m_hr, sError, sDescription);
		CString sFile(ps->m_sFile);
		str.Format(_T("Error initialising Filters\n%s(%d) :\nError: %s (%x):\n %s\n"), sFile, ps->m_nLine, sError, ps->m_hr, sDescription);
		TRACE(str);
		if (bMsgBox)
		{
			AfxMessageBox(str, MB_ICONERROR|MB_OK);
		}
	}
	delete ps;
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::OnEditClear() 
{
	CWnd *pWnd = GetFocus();
	if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CBaseCtrl)))
	{
		pWnd->DestroyWindow();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::OnUpdateEditClear(CCmdUI* pCmdUI) 
{
	CWnd *pWnd = GetFocus();
	if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CBaseCtrl)))
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::OnEditCopy() 
{
	
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::OnEditCut() 
{
	
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::OnEditPaste() 
{
	
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::OnEditUndo() 
{
	
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::OnDraw(CDC* pDC) 
{
	// do nothing the control draw itself
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (nFlags & MK_FILTERPIN)
	{
		CLineTracker tracker;
		CDC *pDC = GetDC();
		CWnd *pWnd1 = GetFocus();
		tracker.TrackRubberBand(this, point);
		tracker.DrawTrackerRect(NULL, NULL, pDC, this);
		ReleaseDC(pDC);
		CPoint pt, ptView, ptCtrl;
		GetCursorPos(&pt);
		ptView = pt;
		ScreenToClient(&ptView);
		OnLButtonUp(0, ptView);

		CWnd *pWnd2 = ChildWindowFromPoint(ptView);
		if (pWnd1 && pWnd1->IsKindOf(RUNTIME_CLASS(CFilterCtrl)))
		{
			MapWindowPoints(pWnd1, &point, 1);
			pWnd1->SendMessage(WM_LBUTTONUP, 0, MAKELONG(point.x, point.y));
			if (pWnd2 && pWnd2->IsKindOf(RUNTIME_CLASS(CFilterCtrl)))
			{
				bool bInPin;
				CFilterCtrl *pF1 = (CFilterCtrl*)pWnd1;
				CFilterCtrl *pF2 = (CFilterCtrl*)pWnd2;
				int nPin1 = pF1->GetCurrentPinIndex(bInPin);
				if (nPin1 != -1 && bInPin == false)
				{
					ptCtrl = pt;
					pF2->ScreenToClient(&ptCtrl);
					int nPin2 = pF2->PinIndexFromPosition(ptCtrl, bInPin);
					if (nPin2 != -1 && bInPin == true)
					{
						CPinConnectionCtrl *pPC = new CPinConnectionCtrl(pF1, nPin1, pF2, nPin2);
						if (pPC->m_hWnd == NULL)
						{
							delete pPC;
						}
						else
						{
							GetDocument()->SetModifiedFlag();
						}
					}
				}
			}
		}
	}
	else
	{
		CRectTracker tracker;
		tracker.TrackRubberBand(this, point);
	}
	CFormView::OnLButtonDown(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CFormView::OnLButtonUp(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::OnMouseMove(UINT nFlags, CPoint point) 
{
	CFormView::OnMouseMove(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CGraphCreatorView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	return CFormView::OnSetCursor(pWnd, nHitTest, message);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CGraphCreatorView::OnHresultError(WPARAM wParam, LPARAM lParam)
{
	ReportError((ErrorStruct*)wParam, lParam ? true:false);
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CGraphCreatorView::OnUser(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
		case WPARAM_CTRL_REMOVED:
			EnumChildWindows(m_hWnd, RemoveCtrlID, lParam);
			break;
		case WPARAM_CLOSE_DOC:
			EnumChildWindows(m_hWnd, FindFilterCtrls, (LPARAM) 0);
			break;
		case WPARAM_UPDATE_GRAPH:
			UpdateGraphFilters();
			break;
		case WPARAM_INSERT_FILTER:
		{
			CFilterCtrl *pBtn = new CFilterCtrl(this, m_pDlgFilters->m_pMoniker, m_pDlgFilters->m_sFilterName);
			if (pBtn->m_hWnd == NULL)
			{
				delete pBtn;
			}
			else
			{
				GetDocument()->SetModifiedFlag();
			}
		}
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::OnGraphStart()
{
	if (GetDocument()->m_pMediaControl)
	{
		GetDocument()->m_pMediaControl->Run();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::OnUpdateGraphStart(CCmdUI *pCmdUI)
{
	BOOL bEnable = FALSE;
	if (GetDocument()->m_pMediaControl)
	{
		OAFilterState oafs;
		GetDocument()->m_pMediaControl->GetState(0, &oafs);
		bEnable = oafs != State_Running;
	}
	pCmdUI->Enable(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::OnGraphStop()
{
	if (GetDocument()->m_pMediaControl)
	{
		GetDocument()->m_pMediaControl->Stop();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::OnUpdateGraphStop(CCmdUI *pCmdUI)
{
	BOOL bEnable = FALSE;
	if (GetDocument()->m_pMediaControl)
	{
		OAFilterState oafs;
		GetDocument()->m_pMediaControl->GetState(0, &oafs);
		bEnable = oafs != State_Stopped;
	}
	pCmdUI->Enable(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::OnGraphPause()
{
	if (GetDocument()->m_pMediaControl)
	{
		GetDocument()->m_pMediaControl->Pause();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::OnUpdateGraphPause(CCmdUI *pCmdUI)
{
	BOOL bEnable = FALSE;
	if (GetDocument()->m_pMediaControl)
	{
		OAFilterState oafs;
		GetDocument()->m_pMediaControl->GetState(0, &oafs);
		bEnable = oafs != State_Paused;
	}
	pCmdUI->Enable(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CGraphCreatorView::OnGraphNotify(WPARAM wParam,  LPARAM lParam)
{
	LONG evCode, evParam1, evParam2;
	HRESULT hr=S_OK;
	if (GetDocument()->m_pMediaEvent)
	{
		while(SUCCEEDED(GetDocument()->m_pMediaEvent->GetEvent(&evCode, (LONG_PTR *) &evParam1, (LONG_PTR *) &evParam2, 0)))
		{
			CString str;
			switch (evCode)
			{
				case EC_DEVICE_LOST: break;
				default:
					str.Format(_T("Event(%d, %x), %x, %x\r\n"), evCode, evCode, evParam1, evParam2);
					break;
			}
			TRACE(_T("%s"), str);
//			m_cReports.SetSel(-1, -1);
//			m_cReports.ReplaceSel(str);
			// Free event parameters to prevent memory leaks
			hr = GetDocument()->m_pMediaEvent->FreeEventParams(evCode, evParam1, evParam2);
		}
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::OnUpdateFileRenderMedia(CCmdUI *pCmdUI)
{
	// TODO! RKE: check filter and graph states
	pCmdUI->Enable(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::OnViewRearrangeFilters()
{
	ReArrangeFilters();
}
/////////////////////////////////////////////////////////////////////////////
void CGraphCreatorView::OnUpdateViewRearrangeFilters(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}
