// ViewPrintPreview.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "mainfrm.h"

#include "ViewPrintPreview.h"
#include "ViewIdipClient.h"


// CViewPrintPreview

IMPLEMENT_DYNCREATE(CViewPrintPreview, CPreviewView)

CViewPrintPreview::CViewPrintPreview()
{
}

CViewPrintPreview::~CViewPrintPreview()
{
}

BEGIN_MESSAGE_MAP(CViewPrintPreview, CPreviewView)
	ON_COMMAND(AFX_ID_PREVIEW_PRINT, OnPreviewPrint)
END_MESSAGE_MAP()


// CViewPrintPreview drawing

void CViewPrintPreview::OnDraw(CDC* pDC)
{
	CPreviewView::OnDraw(pDC);
}

void CViewPrintPreview::OnPreviewPrint()
{
	theApp.GetMainFrame()->GetViewIdipClient()->m_bPreviewPrinting = TRUE;
	CPreviewView::OnPreviewPrint();
}



// CViewPrintPreview diagnostics

#ifdef _DEBUG
void CViewPrintPreview::AssertValid() const
{
	CPreviewView::AssertValid();
}

void CViewPrintPreview::Dump(CDumpContext& dc) const
{
	CPreviewView::Dump(dc);
}
#endif //_DEBUG


// CViewPrintPreview message handlers

void CViewPrintPreview::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
//	CPreviewView::OnUpdate(pSender, lHint, pHint);
}

