// FilterButton.cpp : implementation file
//

#include "stdafx.h"

#include "GraphCreator.h"
#include "GraphCreatorView.h"
#include "GraphCreatorDoc.h"

#include "FilterButton.h"
#include "PinConnectionCtrl.h"
#include "OutPinProperties.h"
#include "InPinProperties.h"

#include <initguid.h>    // DEFINE_GUID to declare an EXTERN_C const.
#include "Filters\Dump\idump.h"
#include "graphcreatorpropertypage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CFilterCtrl
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CFilterCtrl, CBaseCtrl)
CFilterCtrl::CFilterCtrl(CGraphCreatorView*pParent, IMoniker*pMoniker, CString& sFilterName)
{
	m_pFilter = NULL;
	m_bInPin  = false;
	m_bSingleLine = true;
	m_wOnPin  = 0xffff;
	m_wOffset1 = gm_nTextDistance;
	m_wOffset2 = gm_nTextDistance;
	BSTR sName = sFilterName.AllocSysString();
	HRESULT hr = AddFilterToGraph(pParent->GetGraph(), &m_pFilter, pMoniker, sName);
	SysFreeString(sName);
	if (SUCCEEDED(hr))
	{
		CRect rc (0,0,200,100);
		int nID = pParent->GetFreeFilterID();
		if (nID > FIRST_FILTER_ID)
		{
			CRect rcCtrl;
			pParent->GetCtrlRect(nID-1, &rcCtrl);
			rc.OffsetRect(rcCtrl.right + gm_nFilterDistance, 0);
		}
		if (sFilterName.CompareNoCase(_T("Dump")) == 0)
		{
			InitDumpFilter(m_pFilter);
		}
		CreateEx(FBTN_STYLE_EX, WC_BUTTON, sFilterName, FBTN_STYLE, rc, pParent, nID);
	}
	else
	{
		pParent->PostMessage(WM_HRESULT_ERROR, (WPARAM)HRESULT_ERROR_AT_POS(hr), 1);
		RELEASE_OBJECT(m_pFilter);
	}
}
/////////////////////////////////////////////////////////////////////////////
CFilterCtrl::CFilterCtrl(CGraphCreatorView*pParent, IBaseFilter *pFilter, CString& sFilterName)
{
	m_bInPin  = false;
	m_bSingleLine = true;
	m_wOnPin  = 0xffff;
	m_wOffset1 = gm_nTextDistance;
	m_wOffset2 = gm_nTextDistance;

	m_pFilter = pFilter;
	if (m_pFilter)
	{
		m_pFilter->AddRef();
		CRect rc (0,0,200,100);
		int nID = pParent->GetFreeFilterID();
		if (nID > FIRST_FILTER_ID)
		{
			CRect rcCtrl;
			pParent->GetCtrlRect(nID-1, &rcCtrl);
			rc.OffsetRect(rcCtrl.right + gm_nFilterDistance, 0);
		}
		CreateEx(FBTN_STYLE_EX, WC_BUTTON, sFilterName, FBTN_STYLE, rc, pParent, nID);
	}
}
/////////////////////////////////////////////////////////////////////////////
CFilterCtrl::~CFilterCtrl()
{
//	TRACE("~CFilterCtrl()\n");
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CFilterCtrl, CBaseCtrl)
	//{{AFX_MSG_MAP(CFilterCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CONTEXTMENU()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_PIN_RENDER, OnPinRender)
	ON_COMMAND(ID_PIN_SHOWMEDIATYPES, OnPinShowmediatypes)
	ON_COMMAND(ID_FILTER_PROPERTIES, OnFilterProperties)
	ON_UPDATE_COMMAND_UI(ID_FILTER_PROPERTIES, OnUpdateFilterProperties)
	ON_COMMAND(ID_FILTER_INTERFACES, OnFilterInterfaces)
	ON_UPDATE_COMMAND_UI(ID_FILTER_INTERFACES, OnUpdateFilterInterfaces)
	ON_COMMAND(ID_PIN_INTERFACES, OnPinInterfaces)
	ON_UPDATE_COMMAND_UI(ID_PIN_INTERFACES, OnUpdatePinInterfaces)
	ON_UPDATE_COMMAND_UI(ID_FILTER_REFRESH, OnUpdateFilterRefresh)
	ON_COMMAND(ID_FILTER_REFRESH, OnFilterRefresh)
	ON_UPDATE_COMMAND_UI(ID_FILTER_FILENAME, OnUpdateFilterFilename)
	ON_COMMAND(ID_FILTER_FILENAME, OnFilterFilename)
   ON_COMMAND(ID_FILTERS_PIN_PROPERTIES, OnFiltersPinProperties)
   ON_UPDATE_COMMAND_UI(ID_FILTERS_PIN_PROPERTIES, OnUpdateFiltersPinProperties)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
IPin*CFilterCtrl::GetInPin(int i, HRESULT &hr)
{
	IPin* pPin = NULL;
	hr = GetPin(m_pFilter, PINDIR_INPUT, i, &pPin);
	if (SUCCEEDED(hr))
	{
		return pPin;
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
IPin*CFilterCtrl::GetOutPin(int i, HRESULT &hr)
{
	IPin* pPin = NULL;
	hr = GetPin(m_pFilter, PINDIR_OUTPUT, i, &pPin);
	if (SUCCEEDED(hr))
	{
		return pPin;
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CPoint	CFilterCtrl::GetInPinPos(int i)
{
	CPoint pt;
   CGraphCreatorView *pView = GetView();
	pt.x = m_szFilterText.cy>>1;
	pt.y = gm_nPinOffsetY + (m_szFilterText.cy + gm_nPinDistanceY)*i + pt.x;
   if (pView->IsPrinting())
   {
      CRect rc;
      GetWindowRect(&rc);
      pt.Offset(rc.TopLeft());
   }
	return pt;
}
/////////////////////////////////////////////////////////////////////////////
CPoint	CFilterCtrl::GetOutPinPos(int i)
{
	CPoint pt;
	CRect rc;
   CGraphCreatorView *pView = GetView();
   if (pView->IsPrinting())
   {
      GetWindowRect(&rc);
   }
   else
   {
	   GetClientRect(&rc);
   }
	pt.x = m_szFilterText.cy>>1;
   pt.y = rc.top + gm_nPinOffsetY + (m_szFilterText.cy + gm_nPinDistanceY)*i + pt.x;
	pt.x = rc.right - pt.x;
	return pt;
}
/////////////////////////////////////////////////////////////////////////////
// CFilterCtrl message handlers
void CFilterCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_wOnPin != 0xffff)
	{
		CGraphCreatorView*pView = GetView();
		CPoint ptView(point);
		MapWindowPoints(pView, &ptView, 1);
		SetFocus();
		pView->OnLButtonDown(MK_FILTERPIN|nFlags, ptView);
		return;
	}
	else
	{
		m_ptMove	= point;
	}
	CBaseCtrl::OnLButtonDown(nFlags, point);
	InvalidateRect(NULL);
}
/////////////////////////////////////////////////////////////////////////////
void CFilterCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_ptMove.x = -1;
	m_ptMove.y = -1;
	
	CBaseCtrl::OnLButtonUp(nFlags, point);
	InvalidateRect(NULL);
	InvalidateConnections();
}
/////////////////////////////////////////////////////////////////////////////
void CFilterCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_ptMove != CPoint(-1, -1) && (m_wOnPin == 0xffff))	
	{
		CRect rc;
		GetWindowRect(&rc);
		GetParent()->ScreenToClient(&rc);
		rc.OffsetRect(point - m_ptMove);
		MoveWindow(&rc, TRUE);
		InvalidateConnections();		
	}
	CBaseCtrl::OnMouseMove(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
void CFilterCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu menu, *pSubMenu;
	int nItem = 1;
	menu.LoadMenu(IDR_CONTEXT);
	if (m_wOnPin != 0xffff)
	{
		nItem = 0;
	}
	else
	{
	}
	pSubMenu = menu.GetSubMenu(nItem);	
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	if (pSubMenu)
	{
		CGraphCreatorApp::DoUpdateMenuPopup(pSubMenu, this);
		pSubMenu->TrackPopupMenu(0, point.x, point.y, this);
	}
}
/////////////////////////////////////////////////////////////////////////////
int CFilterCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBaseCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	OnFilterRefresh();

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CFilterCtrl::OnDestroy() 
{
	CWnd *pConnected;
	int i;
	
	for (i=0; ((pConnected = GetPrevCtrl(i)) != NULL); i++)
	{
		ASSERT_KINDOF(CPinConnectionCtrl, pConnected);
		pConnected->DestroyWindow();
	}
	for (i=0; ((pConnected = GetNextCtrl(i)) != NULL); i++)
	{
		ASSERT_KINDOF(CPinConnectionCtrl, pConnected);
		pConnected->DestroyWindow();
	}

	if (m_pFilter)
	{
		GetView()->GetGraph()->RemoveFilter(m_pFilter);
		RELEASE_OBJECT(m_pFilter);
	}
	CBaseCtrl::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
CSize CFilterCtrl::EnumPins()
{
	IPin *pPin = NULL;
	int i, nMaxIn=0, nMaxOut=0;
	CString sName;
	CSize   szName, szBtn;
	PIN_INFO pi;
	HRESULT hr;
	CClientDC dc(this);

	dc.SaveDC();
	dc.SelectObject(GetView()->GetFont());

	m_sInPins.RemoveAll();
	m_sOutPins.RemoveAll();

	for (i=0; ; i++)
	{
		pPin = GetInPin(i, hr);
		if (SUCCEEDED(hr))
		{
			hr = pPin->QueryPinInfo(&pi);
			if (SUCCEEDED(hr))
			{
				sName = CString(pi.achName);
				m_sInPins.Add(sName);
				szName = dc.GetOutputTextExtent(sName);
				nMaxIn = max(nMaxIn, szName.cx);
				pi.pFilter->Release();
			}
			pPin->Release();
		}
		else
		{
			break;
		}
	}
	
	for (i=0; ; i++)
	{
		pPin = GetOutPin(i, hr);
		if (SUCCEEDED(hr))
		{
			hr = pPin->QueryPinInfo(&pi);
			if (SUCCEEDED(hr))
			{
				pi.pFilter->Release();
				sName = CString(pi.achName);
				m_sOutPins.Add(sName);
				szName = dc.GetOutputTextExtent(sName);
				nMaxOut = max(nMaxOut, szName.cx);
			}
			pPin->Release();
		}
		else
		{
			break;
		}
	}

	dc.RestoreDC(-1);
	if (nMaxIn)
	{
		m_wOffset1 = m_szFilterText.cy + gm_nTextDistance + nMaxIn + gm_nTextDistance;
	}
	if (nMaxOut)
	{
		m_wOffset2 = gm_nTextDistance + nMaxOut+ gm_nTextDistance + m_szFilterText.cy;
	}
	szBtn.cx = m_wOffset1 + m_szFilterText.cx + m_wOffset2;
	szBtn.cy = max(gm_nMinHeight, max(m_sInPins.GetSize(), m_sOutPins.GetSize()) * (m_szFilterText.cy + gm_nPinDistanceY) + gm_nPinOffsetY * 2);
   i = m_sInPins.GetSize();
   if (i > 0)
   {
      m_MediatypeInPin.SetSize(i);
   }
   i = m_sOutPins.GetSize();
   if (i > 0)
   {
      m_MediatypeOutPin.SetSize(i);
   }
	return szBtn;
}
/////////////////////////////////////////////////////////////////////////////
void CFilterCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CString strWnd;
	CRect	rc, rcBtn, rcWnd, rcItem(lpDrawItemStruct->rcItem);
	CDC*	pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	pDC->SaveDC();
	GetWindowText(strWnd);

	pDC->SelectObject(GetView()->GetFont());

	GetWindowRect(&rcWnd);
	if (rcItem.IsRectEmpty())
	{
		GetView()->ScreenToClient(&rcWnd);
	}
	else
	{
		ScreenToClient(&rcWnd);
	}
	rc = rcWnd;
	int nDeflateX = rcWnd.Width() - m_szFilterText.cx;
	nDeflateX >>= 1;

	if (lpDrawItemStruct->itemState & ODS_FOCUS)
	{
		pDC->SelectStockObject(WHITE_BRUSH);
	}
	else
	{
		pDC->SelectStockObject(LTGRAY_BRUSH);
	}

	pDC->Rectangle(&rc);

	pDC->SelectObject(GetFont());
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextAlign(TA_LEFT | TA_TOP);
	pDC->SelectStockObject(NULL_PEN);

	CRect rcText(rc);
	DWORD dwFlags = DT_CENTER|DT_VCENTER |(m_bSingleLine ? DT_SINGLELINE : DT_WORDBREAK);
	rcText.DeflateRect(m_wOffset1, 0, m_wOffset2, 0);
	pDC->DrawText(strWnd, &rcText, dwFlags);

	pDC->SelectStockObject(BLACK_BRUSH);
	int i, nSize, nY = rc.top + gm_nPinOffsetY;
	nSize = m_sInPins.GetSize();
	for (i=0; i<nSize; i++)
	{
		rcBtn.SetRect(rcWnd.left, nY, rcWnd.left+m_szFilterText.cy, nY+m_szFilterText.cy);
		DrawFrameControl(pDC->m_hDC, &rcBtn, DFC_BUTTON, DFCS_BUTTONPUSH);
		pDC->TextOut(rcBtn.right+gm_nTextDistance, nY, m_sInPins.GetAt(i));
		rcBtn.DeflateRect(4,4);
		pDC->Ellipse(rcBtn);
		nY += m_szFilterText.cy + gm_nPinDistanceY;
	}

	pDC->SetTextAlign(TA_RIGHT | TA_TOP);

	nY = rc.top + gm_nPinOffsetY;
	nSize = m_sOutPins.GetSize();
	for (i=0; i<nSize; i++)
	{
		rcBtn.SetRect(rcWnd.right-m_szFilterText.cy, nY, rcWnd.right, nY+m_szFilterText.cy);
		DrawFrameControl(pDC->m_hDC, &rcBtn, DFC_BUTTON, DFCS_BUTTONPUSH);
		pDC->TextOut(rcBtn.left-gm_nTextDistance, nY, m_sOutPins.GetAt(i));
		rcBtn.DeflateRect(4,4);
		pDC->Ellipse(rcBtn);
		nY += m_szFilterText.cy + gm_nPinDistanceY;
	}

	pDC->RestoreDC(-1);
}
//////////////////////////////////////////////////////////////////////////
int	CFilterCtrl::PinIndexFromPosition(CPoint pt, bool& bInPin)
{
	CRect  rcWnd;
	GetWindowRect(&rcWnd);
	ScreenToClient(&rcWnd);

	int nPin = -1;
	pt.y    -= gm_nPinOffsetY;
	nPin = (WORD)(pt.y / (m_szFilterText.cy + gm_nPinDistanceY));
	pt.y    %= (m_szFilterText.cy + gm_nPinDistanceY);
	
	bInPin = IsBetween(pt.x, rcWnd.left, rcWnd.left+m_szFilterText.cy);
	if (    IsBetween(pt.y, 0, m_szFilterText.cy)
		 && (    bInPin
		     ||  IsBetween(pt.x, rcWnd.right-m_szFilterText.cy, rcWnd.right)))
	{
		if (   ( bInPin && nPin >= m_sInPins.GetCount())
			|| (!bInPin && nPin >= m_sOutPins.GetCount()))
		{
			nPin = -1;
		}
		else
		{
			return nPin;
		}
	}
	return -1;
}
//////////////////////////////////////////////////////////////////////////
int	CFilterCtrl::GetCurrentPinIndex(bool& bInPin)
{
	if (m_wOnPin != 0xffff)
	{
		bInPin = m_bInPin ? true : false;
		return m_wOnPin;
	}
	return -1;
}
//////////////////////////////////////////////////////////////////////////
void CFilterCtrl::InvalidateConnections()
{
	CWnd*pWnd;
	int i = 0;
	while ((pWnd = GetNextCtrl(i++)) != NULL)
	{
		pWnd->InvalidateRect(NULL);
	}
	i=0;
	while ((pWnd = GetPrevCtrl(i++)) != NULL)
	{
		pWnd->InvalidateRect(NULL);
	}
}
//////////////////////////////////////////////////////////////////////////
void CFilterCtrl::CheckConnections()
{
	HRESULT hr;
	IPin *pOutPin = NULL, *pInPin = NULL;
	int i, n = m_sOutPins.GetCount();
	for (i=0; i<n; i++)
	{
		pOutPin = GetOutPin(i, hr);
		if (SUCCEEDED(hr))
		{
			hr = pOutPin->ConnectedTo(&pInPin);
			if (SUCCEEDED(hr))
			{
				PIN_INFO pi;
				pInPin->QueryPinInfo(&pi);
				if (SUCCEEDED(hr))
				{
					BOOL bFound = FALSE;
					for (int j=0; ; j++)
					{
						CWnd*pWnd = GetNextCtrl(j);
						if (pWnd)
						{
							ASSERT_KINDOF(CPinConnectionCtrl, pWnd);
							if (((CPinConnectionCtrl*)pWnd)->GetOutPin() == pOutPin)
							{
								bFound = TRUE;
								break;
							}
						}
						else
						{
							break;
						}
					}
					if (!bFound)
					{
						SFindFilterCtrl ffc(pi.pFilter);
						EnumChildWindows(GetView()->m_hWnd, CGraphCreatorView::FindFilterCtrls, (LPARAM) &ffc);
						CWnd *pWnd = GetView()->GetDlgItem(ffc.nID);
						if (pWnd)
						{
							ASSERT_KINDOF(CFilterCtrl, pWnd);
							CFilterCtrl*pF2 = (CFilterCtrl*)pWnd;
							CPinConnectionCtrl *pPC = new CPinConnectionCtrl(this, i, pF2, pF2->GetPinNo(pInPin, true, hr), (AM_MEDIA_TYPE*)INVALID_HANDLE_VALUE);
							if (pPC->m_hWnd == NULL)
							{
								delete pPC;
							}
						}
					}
				}
				RELEASE_OBJECT(pi.pFilter);
			}
			RELEASE_OBJECT(pInPin);
		}
		RELEASE_OBJECT(pOutPin);
	}
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CFilterCtrl::InitDumpFilter(IBaseFilter*pDump)
{
	HRESULT hr = E_POINTER;
	if (pDump)
	{
		IPin* pPinIn = NULL;
		IDump *pIDump = NULL;
		HRESULT hr = GetPin(pDump  , PINDIR_INPUT , 0, &pPinIn);
		if (SUCCEEDED(hr))
		{
			hr = pPinIn->QueryInterface(IID_IDump, (void **)&pIDump);
			if (SUCCEEDED(hr))
			{
				hr = pIDump->SetReceiveFunction(ReceiveMediaSample, (long)this);
			}
		}
		RELEASE_OBJECT(pPinIn);
		RELEASE_OBJECT(pIDump);
	}
	return hr;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT WINAPI CFilterCtrl::ReceiveMediaSample(IMediaSample *pSample, long nPackageNo, long lParam)
{
	CFilterCtrl *pThis = (CFilterCtrl*)(LONG_PTR)lParam;
	return pThis->ReceiveMediaSample(pSample, nPackageNo);
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CFilterCtrl::ReceiveMediaSample(IMediaSample *pSample, long nPackageNo)
{
	if (nPackageNo == 0)
	{
		AM_MEDIA_TYPE *pMT = NULL;
		HRESULT hr = pSample->GetMediaType(&pMT);
		if (pMT)
		{
			DeleteMediaType(pMT);
		}
	}

	const char sContentLength[] = "Content-Length: ";
	char*pBuffer;
	int nSampleLength = pSample->GetActualDataLength();
	pSample->GetPointer((BYTE**)&pBuffer);
	char *ptr = strstr(pBuffer, sContentLength);
	if (ptr)
	{
		ptr = &ptr[sizeof(sContentLength)-1];
		int nLen = atoi(ptr);
		ptr = strstr(ptr, "\r\n\r\n");
		if (ptr)
		{
			ptr = &ptr[4];
			int nHead = ptr - pBuffer;
			nSampleLength -= nHead;

		}
	}
	
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
BOOL CFilterCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	bool bInPin;
	int nPin = PinIndexFromPosition(pt, bInPin);
	if (nPin != -1)
	{
		m_wOnPin = (WORD)nPin;
		m_bInPin = bInPin;
		HCURSOR hCur = AfxGetApp()->LoadStandardCursor(IDC_CROSS);
		::SetCursor(hCur);
	}
	else
	{
		m_wOnPin = 0xffff;
		HCURSOR hCur = AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);
		::SetCursor(hCur);
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CFilterCtrl::OnPinRender() 
{
	CGraphCreatorView *pView = GetView();
	IPin *pPin = NULL;
	HRESULT hr = GetPin(m_pFilter, m_bInPin ? PINDIR_INPUT : PINDIR_OUTPUT, m_wOnPin, &pPin);
	if (SUCCEEDED(hr))
	{
		hr = pView->GetGraph()->Render(pPin);
		if (SUCCEEDED(hr))
		{
			pView->GetDocument()->SetModifiedFlag();
			pView->UpdateGraphFilters();
		}
		else
		{
			pView->ReportError(HRESULT_ERROR_AT_POS(hr));
		}
	}
	RELEASE_OBJECT(pPin);
}
/////////////////////////////////////////////////////////////////////////////
int CFilterCtrl::GetPinNo(IPin*pPinSearch, bool bInPin, HRESULT &hr)
{
	PIN_DIRECTION dirrequired = bInPin ? PINDIR_INPUT : PINDIR_OUTPUT;
	CComPtr< IEnumPins > pEnum;
	hr = m_pFilter->EnumPins(&pEnum);
    if(FAILED(hr)) 
        return hr;

    ULONG ulFound;
    IPin *pPin = NULL;
	int iPin = 0;
    hr = E_FAIL;

    while(S_OK == pEnum->Next(1, &pPin, &ulFound))
    {
        PIN_DIRECTION pindir = (PIN_DIRECTION)3;
        pPin->QueryDirection(&pindir);
        if(pindir == dirrequired)
        {
			if (pPin == pPinSearch)
			{
                hr = S_OK;
				break;
			}
			iPin++;
        }
		RELEASE_OBJECT(pPin);
    } 
	RELEASE_OBJECT(pPin);

	return (hr == S_OK) ? iPin : -1;
}
/////////////////////////////////////////////////////////////////////////////
void CFilterCtrl::OnUpdatePinShowmediatypes(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}
/////////////////////////////////////////////////////////////////////////////
void CFilterCtrl::OnPinShowmediatypes()
{
	IPin *pPin = NULL;
	HRESULT hr = GetPin(m_pFilter, m_bInPin ? PINDIR_INPUT : PINDIR_OUTPUT, m_wOnPin, &pPin);
	if (SUCCEEDED(hr))
	{
		IEnumMediaTypes *pEMT = NULL;
		CString sMajorType, sSubType, sFormatType, sMsg;
		hr = pPin->EnumMediaTypes(&pEMT);
		if (SUCCEEDED(hr))
		{
			AM_MEDIA_TYPE *pMT = NULL;
			ULONG uFetched;
			do
			{
				hr = pEMT->Next(1, &pMT, &uFetched);
				if (pMT)
				{
					GetGUIDString(sMajorType, &pMT->majortype, TRUE);
					GetGUIDString(sSubType, &pMT->subtype, TRUE);
					GetGUIDString(sFormatType, &pMT->formattype, TRUE);
					sMsg += sMajorType + _T(", ") + sSubType + _T(", ") + sFormatType + _T("\n");
					DeleteMediaType(pMT);
					pMT = NULL;
				}
			}while (hr==S_OK);
			if (!sMsg.IsEmpty())
			{
				AfxMessageBox(sMsg);
			}
		}
		RELEASE_OBJECT(pEMT);
	}
	RELEASE_OBJECT(pPin);
}
/////////////////////////////////////////////////////////////////////////////
void CFilterCtrl::OnFilterProperties()
{
#if GRAPH_CREATOR_PROPERTY_TEST
	HRESULT hr = CGraphCreatorApp::ShowFilterPropertyPage(m_pFilter, m_hWnd);
#else
	HRESULT hr = ShowFilterPropertyPage(m_pFilter, m_hWnd);
#endif
	if (FAILED(hr))
	{
		GetView()->PostMessage(WM_HRESULT_ERROR, (WPARAM)HRESULT_ERROR_AT_POS(hr), 1);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CFilterCtrl::OnUpdateFilterProperties(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(SupportsPropertyPage(m_pFilter));
}
/////////////////////////////////////////////////////////////////////////////
void CFilterCtrl::OnFilterInterfaces()
{
	IUnknown *pUnk = NULL;
	CString str;
	CStringArray sa;
	HRESULT hr = m_pFilter->QueryInterface(IID_IUnknown, (void**)&pUnk);
	if (SUCCEEDED(hr))
	{
		theApp.GetInterfaces(pUnk, sa);
		ConcatenateStrings(sa, str, _T('\n'));
		AfxMessageBox(str);
	}
	RELEASE_OBJECT(pUnk);
}
/////////////////////////////////////////////////////////////////////////////
void CFilterCtrl::OnUpdateFilterInterfaces(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CFilterCtrl::OnPinInterfaces()
{
	IUnknown *pUnk = NULL;
	IPin *pPin = NULL;
	HRESULT hr = GetPin(m_pFilter, m_bInPin ? PINDIR_INPUT : PINDIR_OUTPUT, m_wOnPin, &pPin);
	if (SUCCEEDED(hr))
	{
		CString str;
		CStringArray sa;
		HRESULT hr = pPin->QueryInterface(IID_IUnknown, (void**)&pUnk);
		if (SUCCEEDED(hr))
		{
			theApp.GetInterfaces(pUnk, sa);
			ConcatenateStrings(sa, str, _T('\n'));
			AfxMessageBox(str);
		}
	}
	RELEASE_OBJECT(pPin);
	RELEASE_OBJECT(pUnk);
}
/////////////////////////////////////////////////////////////////////////////
void CFilterCtrl::OnUpdatePinInterfaces(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CFilterCtrl::OnFilterRefresh()
{
	CRect rc;
	CString sName;
	CSize   szBtnSize;
	CSize szOldSize;
	CClientDC dc(this);

	GetWindowRect(&rc);
	szOldSize.cx = rc.Width();
	szOldSize.cy = rc.Height();

	dc.SaveDC();
	dc.SelectObject(GetView()->GetFont());
	GetWindowText(sName);
	m_szFilterText = dc.GetOutputTextExtent(sName);
	int nMaxSize = m_szFilterText.cx / sName.GetLength(); // avg char size
	nMaxSize *= 30;		// max 30 characters
	if (m_szFilterText.cx > nMaxSize)
	{
		m_szFilterText.cx = nMaxSize;
		m_bSingleLine = false;
	}
	dc.RestoreDC(-1);
	
	szBtnSize = EnumPins();

	rc.right  = rc.left + szBtnSize.cx;
	rc.bottom = rc.top  + szBtnSize.cy;
	if (szBtnSize != szOldSize)
	{
		GetParent()->ScreenToClient(&rc);
		rc.right  = rc.left + szBtnSize.cx;
		rc.bottom = rc.top  + szBtnSize.cy;
		MoveWindow(&rc, TRUE);
	}
	else
	{
		InvalidateRect(NULL);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CFilterCtrl::OnUpdateFilterRefresh(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CFilterCtrl::OnFilterFilename()
{
	IFileSourceFilter *pFSoF = NULL;
	IFileSinkFilter   *pFSiF = NULL;
	IFileSinkFilter2  *pFSiF2 = NULL;
	HRESULT hr;
	hr = m_pFilter->QueryInterface(IID_IFileSourceFilter, (void**)&pFSoF);
	hr = m_pFilter->QueryInterface(IID_IFileSinkFilter, (void**)&pFSiF);
	hr = m_pFilter->QueryInterface(IID_IFileSinkFilter2, (void**)&pFSiF2);

	if (pFSoF || pFSiF || pFSiF2)
	{
		BOOL bOpen = pFSoF != NULL;
		CFileDialog dlg(bOpen, NULL);
		if (dlg.DoModal() == IDOK)
		{
			CString sFN = dlg.GetPathName();
			if (pFSoF)
			{
				hr = pFSoF->Load(sFN, NULL);
			}
			else if (pFSiF)
			{
				hr = pFSiF->SetFileName(sFN, NULL);
			}
			else if (pFSiF2)
			{
				hr = pFSiF2->SetFileName(sFN, NULL);
			}
			if (SUCCEEDED(hr))
			{
				CString sTxt;
				GetWindowText(sTxt);
				int nFind = sTxt.Find(_T(":\n"));
				if (nFind != -1)
				{
					sTxt = sTxt.Left(nFind);
				}
				sTxt = sTxt + _T(":\n") + sFN;
				SetWindowText(sTxt);
				OnFilterRefresh();
			}
			else
			{
				GetView()->PostMessage(WM_HRESULT_ERROR, (WPARAM)HRESULT_ERROR_AT_POS(hr), 1);
			}
		}
	}
	RELEASE_OBJECT(pFSoF);
	RELEASE_OBJECT(pFSiF);
	RELEASE_OBJECT(pFSiF2);
}
/////////////////////////////////////////////////////////////////////////////
void CFilterCtrl::OnUpdateFilterFilename(CCmdUI *pCmdUI)
{
	BOOL bEnable = FALSE;
	if (m_pFilter)
	{
		IUnknown*pTest = NULL;
		m_pFilter->QueryInterface(IID_IFileSourceFilter, (void**)&pTest);
		if (pTest)
		{
			bEnable = TRUE;
		}
		else
		{
			m_pFilter->QueryInterface(IID_IFileSinkFilter, (void**)&pTest);
			if (pTest)
			{
				bEnable = TRUE;
			}
			else
			{
				m_pFilter->QueryInterface(IID_IFileSinkFilter2, (void**)&pTest);
				if (pTest)
				{
					bEnable = TRUE;
				}
			}
		}
		RELEASE_OBJECT(pTest);
	}
	pCmdUI->Enable(bEnable);
}

void CFilterCtrl::OnFiltersPinProperties()
{
	CString strWnd;
   GetWindowText(strWnd);
   CPropertySheet sheet(strWnd, this);
   int i;
	int nInPis   = GetInPinCount();
	int nOutPins = GetOutPinCount();
   CInPinProperties*pInProps = NULL;
   COutPinProperties*pOutProps = NULL;
   HRESULT hr;

   if (nInPis)
   {
      pInProps = new CInPinProperties[nInPis];
      for (i=0; i<nInPis; i++)
      {
         pInProps[i].SetName(m_sInPins[i]);
         pInProps[i].SetPin(GetInPin(i, hr), i);
         sheet.AddPage(&pInProps[i]);
      }
   }
   if (nOutPins)
   {
      pOutProps = new COutPinProperties[nOutPins];
      for (i=0; i<nOutPins; i++)
      {
         pOutProps[i].SetName(m_sOutPins[i]);
         pOutProps[i].SetPin(GetOutPin(i, hr), i, this);
         sheet.AddPage(&pOutProps[i]);
      }
   }
   sheet.DoModal();

   WK_DELETE_ARRAY(pInProps);
   WK_DELETE_ARRAY(pOutProps);
}

void CFilterCtrl::OnUpdateFiltersPinProperties(CCmdUI *pCmdUI)
{
   pCmdUI->Enable();
}

/////////////////////////////////////////////////////////////////////////////
/*
			IAMVfwCaptureDialogs *pDlg;
			hr = m_pCapture[nCam]->QueryInterface(IID_IAMVfwCaptureDialogs, (void**)&pDlg);
			if (SUCCEEDED(hr))
			{
				hr = pDlg->HasDialog(VfwCaptureDialog_Source);
				hr = pDlg->HasDialog(VfwCaptureDialog_Format);
				hr = pDlg->HasDialog(VfwCaptureDialog_Display);
				pDlg->Release();
			}

			{	// Not Available
				IAMCameraControl *pCC = NULL;
				hr = m_pCapture[nCam]->QueryInterface(IID_IAMCameraControl, (void**)&pCC);
				if (SUCCEEDED(hr))
				{
					RELEASE_OBJECT(pCC);
				}
			}
			{	// Available
				IAMAnalogVideoDecoder *pCC = NULL;
				hr = m_pCapture[nCam]->QueryInterface(IID_IAMAnalogVideoDecoder, (void**)&pCC);
				if (SUCCEEDED(hr))
				{
					RELEASE_OBJECT(pCC);
				}
			}
			{	// Available
				IAMDroppedFrames *pCC = NULL;
				hr = m_pCapture[nCam]->QueryInterface(IID_IAMDroppedFrames, (void**)&pCC);
				if (SUCCEEDED(hr))
				{
					RELEASE_OBJECT(pCC);
				}
			}
			{	// Not Available
				IAMExtDevice *pCC = NULL;
				hr = m_pCapture[nCam]->QueryInterface(IID_IAMExtDevice, (void**)&pCC);
				if (SUCCEEDED(hr))
				{
					RELEASE_OBJECT(pCC);
				}
			}
			{	// Available
				IAMExtTransport *pCC = NULL;
				hr = m_pCapture[nCam]->QueryInterface(IID_IAMExtTransport, (void**)&pCC);
				if (SUCCEEDED(hr))
				{
					RELEASE_OBJECT(pCC);
				}
			}
			{	// Available
				IAMFilterMiscFlags *pCC = NULL;
				hr = m_pCapture[nCam]->QueryInterface(IID_IAMFilterMiscFlags, (void**)&pCC);
				if (SUCCEEDED(hr))
				{
					RELEASE_OBJECT(pCC);
				}
			}
			{	// Not Available
				IAMTimecodeReader *pCC = NULL;
				hr = m_pCapture[nCam]->QueryInterface(IID_IAMTimecodeReader, (void**)&pCC);
				if (SUCCEEDED(hr))
				{
					RELEASE_OBJECT(pCC);
				}
			}
			{	// Available
				IAMVideoCompression *pCC = NULL;
				hr = m_pCapture[nCam]->QueryInterface(IID_IAMVideoCompression, (void**)&pCC);
				if (SUCCEEDED(hr))
				{
					RELEASE_OBJECT(pCC);
				}
			}
			{	// Available
				IAMVideoProcAmp *pCC = NULL;
				hr = m_pCapture[nCam]->QueryInterface(IID_IAMVideoProcAmp, (void**)&pCC);
				if (SUCCEEDED(hr))
				{
					RELEASE_OBJECT(pCC);
				}
			}
			{	// Available
				IKsPropertySet *pCC = NULL;
				hr = m_pCapture[nCam]->QueryInterface(IID_IKsPropertySet, (void**)&pCC);
				if (SUCCEEDED(hr))
				{
					RELEASE_OBJECT(pCC);
				}
			}
			{	// Not Available
				IReferenceClock *pCC = NULL;
				hr = m_pCapture[nCam]->QueryInterface(IID_IReferenceClock, (void**)&pCC);
				if (SUCCEEDED(hr))
				{
					RELEASE_OBJECT(pCC);
				}
			}
*/

