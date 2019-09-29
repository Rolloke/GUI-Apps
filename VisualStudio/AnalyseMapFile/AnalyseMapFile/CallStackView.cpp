// CallStackView.cpp : implementation file
//

#include "stdafx.h"
#include "AnalyseMapFile.h"
#include "CallStackView.h"
#include ".\callstackview.h"


/////////////////////////////////////////////////////////////////////////////
// CCallStackView
IMPLEMENT_DYNCREATE(CCallStackView, CListView)

/////////////////////////////////////////////////////////////////////////////
CCallStackView::CCallStackView()
{
}
/////////////////////////////////////////////////////////////////////////////
CCallStackView::~CCallStackView()
{
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CCallStackView, CListView)
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCallStackView diagnostics
#ifdef _DEBUG
void CCallStackView::AssertValid() const
{
	CListView::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CCallStackView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCallStackView message handlers
int CCallStackView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;
	GetListCtrl().SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	GetListCtrl().InsertColumn(0, _T("Adresse"));
	GetListCtrl().InsertColumn(1, _T("Funktion"));

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
// CCallStackView message handlers
void CCallStackView::InsertEntry(CString &sAddr, CString& sFncName)
{
	int n = GetListCtrl().GetItemCount();
	n = GetListCtrl().InsertItem(n, sAddr, 0);
	GetListCtrl().SetItemText(n, 1, sFncName);
	GetListCtrl().SetColumnWidth(0, LVSCW_AUTOSIZE);
	GetListCtrl().SetColumnWidth(1, LVSCW_AUTOSIZE);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCallStackView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= LVS_SINGLESEL | LVS_REPORT | LVS_SHOWSELALWAYS;

	return CListView::PreCreateWindow(cs);
}
/////////////////////////////////////////////////////////////////////////////

void CCallStackView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	SHORT sLControl = GetKeyState(VK_LCONTROL);

	int nSel = GetListCtrl().GetSelectionMark();
	switch (nChar)
	{
		case VK_DELETE:
			GetListCtrl().DeleteItem(nSel);
			break;
		case VK_UP:
			if (sLControl & 0x8000)
			{
				SwapItems(nSel, nSel-1);
			}break;
		case VK_DOWN:
			if (sLControl & 0x8000)
			{
				SwapItems(nSel, nSel+1);
			}break;
	}
	CListView::OnKeyDown(nChar, nRepCnt, nFlags);
}
/////////////////////////////////////////////////////////////////////////////
void CCallStackView::SwapItems(int n1, int n2)
{
	int nIC = GetListCtrl().GetItemCount()-1;
	if (   n1 != n2 
		&& IsBetween(n1, 0, nIC)
		&& IsBetween(n2, 0, nIC))
	{
		int nC, nCC = GetListCtrl().GetHeaderCtrl()->GetItemCount();
		CString s;
		for (nC=0; nC<nCC; nC++)
		{
			s = GetListCtrl().GetItemText(n1, nC);
			GetListCtrl().SetItemText(n1, nC, GetListCtrl().GetItemText(n2, nC));
			GetListCtrl().SetItemText(n2, nC, s);
		}
	}
}

void CCallStackView::OnEditCopy()
{
	BOOL bReturn;
	try
	{
        bReturn = OpenClipboard();
		if (!bReturn) throw 1;
		bReturn = EmptyClipboard();
		if (!bReturn) throw 2;
		int nI, nIC = GetListCtrl().GetItemCount();
		int nC, nCC = GetListCtrl().GetHeaderCtrl()->GetItemCount();
		CString s;
		for (nI=0; nI<nIC; nI++)
		{
			for (nC=0; nC<nCC; nC++)
			{
				s += GetListCtrl().GetItemText(nI, nC);
				if (nC == nCC-1) s += _T("\r\n");
				else if (nC >= 0) s += _T("\t");
			}
		}

		HGLOBAL hMem = GlobalAlloc(GHND, (s.GetLength()+1)*sizeof(_TCHAR));
		if (hMem)
		{
			LPTSTR sMem = (LPTSTR)GlobalLock(hMem); 
			if (sMem)
			{
				_tcscpy_s(sMem, s.GetLength(), s);
				GlobalUnlock(hMem);
			}
			else throw 4;
		}
		else throw 3;
#ifdef _UNICODE
		bReturn = SetClipboardData(CF_UNICODETEXT, hMem) != NULL;
#else
		bReturn = SetClipboardData(CF_TEXT, hMem) != NULL;
#endif
		if (!bReturn) throw 6;

		bReturn = CloseClipboard();
		if (!bReturn) throw 7;
	}
	catch(int nError)
	{
		bReturn = CloseClipboard();
		nError = 0;
	}
}

void CCallStackView::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetListCtrl().GetItemCount() > 0);
}
