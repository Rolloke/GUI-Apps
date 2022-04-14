// PinConnectionCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "GraphCreator.h"
#include "PinConnectionCtrl.h"
#include "FilterButton.h"

#include "GraphCreatorDoc.h"
#include "GraphCreatorView.h"

/////////////////////////////////////////////////////////////////////////////
// CPinConnectionCtrl
IMPLEMENT_DYNCREATE(CPinConnectionCtrl, CBaseCtrl)

CPinConnectionCtrl::CPinConnectionCtrl()
{
}
/////////////////////////////////////////////////////////////////////////////
CPinConnectionCtrl::CPinConnectionCtrl(CFilterCtrl*pF1, int nPin1, CFilterCtrl*pF2, int nPin2, AM_MEDIA_TYPE*pMT/*=NULL*/)
{
	HRESULT hr;
   IEnumMediaTypes *pEnum = NULL;
	m_pOutPin = NULL;
	m_pInPin = NULL;
	m_nOutPin = -1;
	m_nInPin = -1;

	FILTER_INFO fi1, fi2;
	ZERO_INIT(fi1);
	ZERO_INIT(fi2);
   CGraphCreatorView*pView = NULL;
	try
	{
		if (pF1 == NULL) throw HRESULT_ERROR_AT_POS(E_POINTER);
      pView = pF1->GetView();
		hr = pF1->GetFilter()->QueryFilterInfo(&fi1);
		HRESULT_EXCEPTION(hr);
		m_pOutPin = pF1->GetOutPin(nPin1, hr);
		HRESULT_EXCEPTION(hr);
      if (pF1->m_MediatypeOutPin[nPin1] && pMT == 0)
      {
         hr = m_pOutPin->EnumMediaTypes(&pEnum);
         if (SUCCEEDED(hr))
         {
            ULONG uFetched;
            pEnum->Skip(pF1->m_MediatypeOutPin[nPin1]-1);
            hr = pEnum->Next(1, &pMT, &uFetched);
         }
      }

		if (pF2 == NULL) throw HRESULT_ERROR_AT_POS(E_POINTER);
		hr = pF2->GetFilter()->QueryFilterInfo(&fi2);
		HRESULT_EXCEPTION(hr);
		m_pInPin = pF2->GetInPin(nPin2, hr);
		HRESULT_EXCEPTION(hr);

		if (pMT != (void*)INVALID_HANDLE_VALUE)
		{
			if (pMT)
			{
				hr = pView->GetGraph()->ConnectDirect(m_pOutPin, m_pInPin, pMT);
			}
			else
			{
				hr = pView->GetGraph()->Connect(m_pOutPin, m_pInPin);
			}
			HRESULT_EXCEPTION(hr);
		}

		m_nOutPin  = nPin1;
		m_nInPin   = nPin2;

		CRect rc(0,0,1,1);
		CString sF1(fi1.achName), sF2(fi2.achName), sConnection = sF1 + _T(" -> ") + sF2;
		UINT nID = pView->GetFreeConnectionID();
      BOOL bResult = CreateEx(FBTN_STYLE_EX, WC_BUTTON, sConnection, FBTN_STYLE, rc, pView, nID);
		if (bResult == FALSE) throw HRESULT_ERROR_AT_POS(E_FAIL);
		

		AddPrevCtrl(pF1->GetDlgCtrlID());
		AddNextCtrl(pF2->GetDlgCtrlID());
		rc = CalculateRect(pView);
		rc.NormalizeRect();
		rc.InflateRect(1,1);
		MoveWindow(&rc);
		pF1->AddNextCtrl(GetDlgCtrlID());
		pF2->AddPrevCtrl(GetDlgCtrlID());
	}
	catch (ErrorStruct*pError)
	{
		RELEASE_OBJECT(m_pOutPin);
		RELEASE_OBJECT(m_pInPin);
		if (pView)
		{
			pView->PostMessage(WM_HRESULT_ERROR, (WPARAM)pError, 1);
		}
	}
	RELEASE_OBJECT(fi1.pGraph);
	RELEASE_OBJECT(fi2.pGraph);
   RELEASE_OBJECT(pEnum);
}
/////////////////////////////////////////////////////////////////////////////
CRect	CPinConnectionCtrl::CalculateRect(CGraphCreatorView *pView)
{
	CFilterCtrl *pF1 = GetPrevFilterCrtl();
	CFilterCtrl *pF2 = GetNextFilterCrtl();
	if (pF1 && pF2)
	{
		CPoint pt1 = pF1->GetOutPinPos(m_nOutPin);
		CPoint pt2 = pF2->GetInPinPos(m_nInPin);
      if (!pView->IsPrinting())
      {
		   pF1->MapWindowPoints(pView, &pt1, 1);
		   pF2->MapWindowPoints(pView, &pt2, 1);
      }
		return CRect(pt1, pt2);
	}
	return CRect(0,0,1,1);
}
/////////////////////////////////////////////////////////////////////////////
CPinConnectionCtrl::~CPinConnectionCtrl()
{
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CPinConnectionCtrl, CBaseCtrl)
	//{{AFX_MSG_MAP(CPinConnectionCtrl)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
// CPinConnectionCtrl message handlers
/////////////////////////////////////////////////////////////////////////////
void CPinConnectionCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect	rc, rcF1, rcF2;
	CDC*	pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
   LPCSTR sClass = pDC->GetRuntimeClass()->m_lpszClassName;
   CGraphCreatorView *pView = GetView();
   BOOL bPolyLine = FALSE;
	GetWindowRect(&rc);
	rc.DeflateRect(1,1);
   int nTextHeight = 12, nInPins=0, nOutPins=0;
   CFilterCtrl *pF1 = GetPrevFilterCrtl();
   CFilterCtrl *pF2 = GetNextFilterCrtl();
   if (pF1)
   {
      nTextHeight = pF1->GetFilterTextSize().cy;
      nOutPins    = pF1->GetOutPinCount();
   }
   if (pF2)
   {
      nInPins = pF2->GetInPinCount();
   }
	CRect rcCur = CalculateRect(pView);
	CRect rcNorm(rcCur);
	rcNorm.NormalizeRect();
   if (rcCur.right - rcCur.left < nTextHeight*2)
   {
      bPolyLine = TRUE;
      rcNorm.InflateRect(nTextHeight*2+2, 0);
   }

	CRect rcScreen(rcNorm);

   if (!pView->IsPrinting())
   {
	   pView->ClientToScreen(&rcScreen);
   }
	if (  rcScreen == rc
      || pView->IsPrinting())
	{
		pDC->SaveDC();
      if (pView->IsPrinting())
      {
         pView->ScreenToClient(rcCur);
         pDC->SelectObject(pView->GetPrintPen());
      }
      else
      {
		   pView->MapWindowPoints(this, &rcCur);
		   if (m_bFocus)
		   {
			   pDC->SelectStockObject(WHITE_PEN);
		   }
		   else
		   {
			   pDC->SelectStockObject(BLACK_PEN);
		   }
      }

      if (bPolyLine)
      {
		   CPoint pt[6];
         int n1, n2, n4, i2; 
         if (rcCur.top < rcCur.bottom) // F1 top, F2 bottom
         {
            n1 = nTextHeight*(nOutPins-m_nOutPin);
            i2 = 5;
            n2 = -nTextHeight*2*(m_nInPin+1);
            n4 = -nTextHeight*(m_nInPin+1);
         }
         else                          // F1 bottom, F2 top
         {
            n1 = nTextHeight*(m_nOutPin+1);
            i2 = 0;
            n2 = -nTextHeight*2*(m_nOutPin+1);
            n4 = -nTextHeight*(nInPins-m_nInPin);
         }
		   pt[0] = rcCur.TopLeft();      // out pin F1
		   pt[5] = rcCur.BottomRight();  // in  pin F2
         pt[1].x = pt[0].x + n1;
         pt[1].y = pt[0].y;
         pt[2].x = pt[1].x;
         pt[2].y = pt[i2].y + n2;
         pt[4].x = pt[5].x + n4;
         pt[4].y = pt[5].y;
         pt[3].x = pt[4].x;
         pt[3].y = pt[2].y;
         pDC->Polyline((LPPOINT)pt, 6);
      }
      else
      {
		   CPoint pt[4];
         pt[0] = rcCur.TopLeft();
		   pt[2] = pt[1] = rcCur.CenterPoint();
		   pt[3] = rcCur.BottomRight();
         pt[1].y = pt[0].y;
         pt[2].y = pt[3].y;
         pDC->Polyline((LPPOINT)pt, 4);
      }
		pDC->RestoreDC(-1);
	}
	else
	{
		rcNorm.InflateRect(1,1);
		MoveWindow(&rcNorm);
		InvalidateRect(NULL);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPinConnectionCtrl::OnDestroy()
{
	GetView()->GetGraph()->Disconnect(m_pOutPin);
	GetView()->GetGraph()->Disconnect(m_pInPin);
	RELEASE_OBJECT(m_pOutPin);
	RELEASE_OBJECT(m_pInPin);
	CBaseCtrl::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
CFilterCtrl*CPinConnectionCtrl::GetPrevFilterCrtl()
{
	CWnd*pWnd = GetPrevCtrl(0);
#ifdef _DEBUG
	if (pWnd)
	{
		ASSERT_KINDOF(CFilterCtrl, pWnd);
	}
#endif
	return (CFilterCtrl*)pWnd;
}
/////////////////////////////////////////////////////////////////////////////
CFilterCtrl*CPinConnectionCtrl::GetNextFilterCrtl()
{
	CWnd*pWnd = GetNextCtrl(0);
#ifdef _DEBUG
	if (pWnd)
	{
		ASSERT_KINDOF(CFilterCtrl, pWnd);
	}
#endif
	return (CFilterCtrl*)pWnd;
}
/////////////////////////////////////////////////////////////////////////////
