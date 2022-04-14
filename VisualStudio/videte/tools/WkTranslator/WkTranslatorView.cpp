// WkTranslatorView.cpp : implementation of the CWkTranslatorView class
//

#include "stdafx.h"
#include "WkTranslator.h"

#include "WkTranslatorDoc.h"
#include "WkTranslatorView.h"
#include "FindDialog.h"
#include "EditReplaceDialog.h"
#include ".\wktranslatorview.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CWkTranslatorApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CWkTranslatorView
IMPLEMENT_DYNCREATE(CWkTranslatorView, CListView)
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CWkTranslatorView, CListView)
	//{{AFX_MSG_MAP(CWkTranslatorView)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_EDIT, OnEdit)
	ON_COMMAND(ID_NEXT_TODO, OnNextTodo)
	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_COMMAND(ID_EDIT_FIND_NEXT, OnEditFindNext)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND_NEXT, OnUpdateEditFindNext)
	ON_COMMAND(ID_CHECK_TRANSLATION, OnCheckTranslation)
	ON_COMMAND(ID_CHECK_TRANSLATION_ALL, OnCheckTranslationAll)
	ON_COMMAND(ID_EDIT_REPLACE, OnEditReplace)
	ON_COMMAND(ID_VIEW_SHOW_ONLY_TODO, OnViewShowOnlyTodo)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOW_ONLY_TODO, OnUpdateViewShowOnlyTodo)
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	ON_COMMAND(ID_VIEW_SHOW_ONLY_SUGGESTIONS, OnViewShowOnlySuggestions)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOW_ONLY_SUGGESTIONS, OnUpdateViewShowOnlySuggestions)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_COMMAND(ID_EDIT_REMOVE_ALL_TODOS, OnEditRemoveAllTodos)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REMOVE_ALL_TODOS, OnUpdateEditRemoveAllTodos)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CWkTranslatorView construction/destruction
CWkTranslatorView::CWkTranslatorView()
{
	m_iCurSel = 0;
	m_dwCurData = 0;
	m_iLastFound = -1;
	m_iI = 0;
	m_iJ = 0;
	m_bCaseSensitive = FALSE;
	m_bShowOnlyTODOs = TRUE;
	m_bShowOnlySuggestions = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
CWkTranslatorView::~CWkTranslatorView()
{
}

BOOL CWkTranslatorView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style = WS_CHILD | WS_VISIBLE | WS_BORDER
				| LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS;

	return CListView::PreCreateWindow(cs);
}
/////////////////////////////////////////////////////////////////////////////
// CWkTranslatorView drawing
void CWkTranslatorView::OnDraw(CDC* /*pDC*/)
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWkTranslatorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	CWkTranslatorDoc* pDoc = GetDocument();

	ASSERT_VALID(pDoc);

	const CTSArrayArray& TSAA = pDoc->GetTSArrayArray();
	int i,p;
	p = 0;
	for (i=0;i<TSAA.GetSize();i++)
	{
		p += TSAA.GetAt(i)->GetSize();
	}
	pInfo->SetMinPage(1);
	pInfo->SetMaxPage((p/20)+1);
    pInfo->m_nNumPreviewPages = 1;

	return DoPreparePrinting(pInfo);
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	m_iI = 0;
	m_iJ = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	m_iI = 0;
	m_iJ = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	// TODO: You may populate your ListView with items by directly accessing
	//  its list control through a call to GetListCtrl().
	GetListCtrl().SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
}
/////////////////////////////////////////////////////////////////////////////
// CWkTranslatorView diagnostics
#ifdef _DEBUG
void CWkTranslatorView::AssertValid() const
{
	CListView::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
/////////////////////////////////////////////////////////////////////////////
CWkTranslatorDoc* CWkTranslatorView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWkTranslatorDoc)));
	return (CWkTranslatorDoc*)m_pDocument;
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// CWkTranslatorView message handlers
void CWkTranslatorView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* pHint) 
{
	if (lHint == UPDATE_NEW_EMPTY_DOC)
	{
		// new Project file
		DeleteItemsAndColumns();
	}
	else if (lHint == UPDATE_OPENED_DOC)
	{
		// pHint contains a CTSArray
		OnTSArrayArray((CTSArrayArray*)pHint);
	}
	else if (lHint >= UPDATE_TRANSLATION)
	{
		CTranslateString* pTS = (CTranslateString*)pHint;
		// Token setzen

		LV_ITEM lvis;
		CString s;
		DWORD dwParam = lHint-10;
		int i;

		TRACE(_T("OnUpdate(%d,%d,%s)\n"),LOWORD(dwParam),HIWORD(dwParam),pTS->GetText().Left(450));
		for (i=0;i<GetListCtrl().GetItemCount();i++)
		{
			if (dwParam==GetListCtrl().GetItemData(i))
			{
				// Max Length
				s = _T("");
				UINT uLen = pTS->GetMaxLength();
				if (uLen > 0)
				{
					s.Format(_T("%u"), uLen);
				}
				lvis.mask = LVIF_TEXT;
				lvis.pszText = s.GetBuffer(0);
				lvis.cchTextMax = s.GetLength();
				lvis.iItem = i;
				lvis.iSubItem = 0;
				GetListCtrl().SetItem(&lvis);
				s.ReleaseBuffer();
				// Translation
				s = pTS->GetTranslate();
				lvis.mask = LVIF_TEXT;
				lvis.pszText = s.GetBuffer(0);
				lvis.cchTextMax = s.GetLength();
				lvis.iItem = i;
				lvis.iSubItem = 2;
				GetListCtrl().SetItem(&lvis);
				s.ReleaseBuffer();
				break;
			}
		}

	}
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorView::DeleteItemsAndColumns()
{
	int i;
	GetListCtrl().DeleteAllItems();
	int iColumnCount = GetListCtrl().GetHeaderCtrl()->GetItemCount();
	for (i=0 ; i<iColumnCount ; i++)
	{
		GetListCtrl().DeleteColumn(0);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorView::OnTSArrayArray(const CTSArrayArray* pTSArrayArray)
{
	// insert the columns
	const int		iColumns = 4;
	CRect			rect;
	LV_COLUMN		lvcolumn;
	CString s[iColumns];
	int i,j;
	int w[] = {1,5,5,5};
	CTSArray* pTSArray = NULL;

	DeleteItemsAndColumns();

	GetListCtrl().GetClientRect(rect);
	s[0].LoadString(IDS_MAX_LENGTH);
	s[1].LoadString(IDS_ORIGINAL);
	s[2].LoadString(IDS_TRANSLATE);
	s[3].LoadString(IDS_SUGGESTION);

	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;
	int nTotal = 0;
	for (i = 0 ; i < iColumns ; i++) nTotal += w[i];

	for (i = 0 ; i < iColumns ; i++)  // add the columns to the list control
	{
		lvcolumn.cx = (rect.Width() * w[i])/nTotal;
		lvcolumn.pszText = s[i].GetBuffer(0);
		lvcolumn.iSubItem = i;
		GetListCtrl().InsertColumn(i, &lvcolumn);  // assumes return value is OK.
		s[i].ReleaseBuffer();
	}
	GetListCtrl().UpdateWindow();

	for (i=0;i<pTSArrayArray->GetSize();i++)
	{
		pTSArray = pTSArrayArray->GetAt(i);
		for (j=0;j<pTSArray->GetSize();j++)
		{
			InsertTranslateString(pTSArray->GetAt(j),i,j);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorView::InsertTranslateString(CTranslateString* pTranslateString, int i, int j)
{
	// show only TODOs?
	if (   m_bShowOnlyTODOs == FALSE					// show all
		|| pTranslateString->CheckTODO() == FALSE	// or show only TODOs
		)
	{
		if (m_bShowOnlySuggestions)
		{
			if (pTranslateString->GetSuggestion().IsEmpty())
			{
				return;
			}
		}
		// Token setzen
		LV_ITEM lvis;
		CString s, sText      = pTranslateString->GetText(),
			        sTranslate = pTranslateString->GetTranslate();
		int iIndex;
		int iCount;
		if (sText.IsEmpty() && sTranslate.IsEmpty())
		{
			return;
		}

		// Am Ende einfuegen, daher ListControl-Count holen
		iCount = GetListCtrl().GetItemCount();

		// Item einfuegen
		// Max. length
		UINT uLen = pTranslateString->GetMaxLength();
		s = _T("");
		if (uLen > 0)
		{
			s.Format(_T("%u"), uLen);
		}
		lvis.mask = LVIF_TEXT | LVIF_PARAM;
		lvis.pszText = (LPTSTR)LPCTSTR(s);
		lvis.cchTextMax = s.GetLength();
		lvis.iItem = iCount;
		lvis.iSubItem = 0;
		lvis.lParam = MAKELONG((WORD)i,(WORD)j); // low, high
		iIndex = GetListCtrl().InsertItem(&lvis);
		
		// Original text
//		s = pTranslateString->GetText();
		lvis.mask = LVIF_TEXT;
		lvis.pszText = (LPTSTR)LPCTSTR(sText);
		lvis.cchTextMax = sText.GetLength();
		lvis.iItem = iIndex;
		lvis.iSubItem = 1;
		GetListCtrl().SetItem(&lvis);

		
		// Translated text
//		s = pTranslateString->GetTranslate();
		lvis.mask = LVIF_TEXT;
		lvis.pszText = (LPTSTR)LPCTSTR(sTranslate);
		lvis.cchTextMax = sTranslate.GetLength();
		lvis.iItem = iIndex;
		lvis.iSubItem = 2;
		GetListCtrl().SetItem(&lvis);

		// Suggestion
		s = pTranslateString->GetSuggestion();
		lvis.mask = LVIF_TEXT;
		lvis.pszText = (LPTSTR)LPCTSTR(s);
		lvis.cchTextMax = s.GetLength();
		lvis.iItem = iIndex;
		lvis.iSubItem = 3;
		GetListCtrl().SetItem(&lvis);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorView::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->uNewState & LVIS_SELECTED)
	{
		m_iCurSel = pNMListView->iItem;	
		m_dwCurData = pNMListView->lParam;	
		TRACE(_T("m_iCurSel=%d, i=%d,j=%d\n"),m_iCurSel,LOWORD(m_dwCurData),HIWORD(m_dwCurData));
	}
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CWkTranslatorDoc* pDoc = GetDocument();
	pDoc->EditTranslateString(LOWORD(m_dwCurData),HIWORD(m_dwCurData), FALSE);
	CListView::OnLButtonDblClk(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorView::OnEdit() 
{
	CWkTranslatorDoc* pDoc = GetDocument();
	pDoc->EditTranslateString(LOWORD(m_dwCurData),HIWORD(m_dwCurData), FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/) 
{
    ASSERT_VALID(pDC);
   
	CWkTranslatorDoc* pDoc = GetDocument();

	ASSERT_VALID(pDoc);

	const CTSArrayArray& pTSAA = pDoc->GetTSArrayArray();
	CTSArray* pTSA;
	CTranslateString* pTS;
	int i,j,d;
	int x,y;
	CSize s;
	int pagey = pDC->GetDeviceCaps(VERTRES);

	x = 300;
	y = 10;
	for (i=m_iI;i<pTSAA.GetSize();i++,m_iI++)
	{
		pTSA = pTSAA.GetAt(i);
		d = pTSA->GetSize();
		for (j=m_iJ;j<d;j++,m_iJ++)
		{
			if (y>pagey-100)
			{
				if (m_iI>0)
				{
					m_iI--;
				}
				if (m_iJ>0)
				{
					m_iJ--;
				}
				return;
			}
			pTS = pTSA->GetAt(j);		
			pDC->TextOut(x,y,pTS->GetText());
			s = pDC->GetOutputTextExtent(pTS->GetText());
			y += s.cy+10;
			pDC->TextOut(x,y,pTS->GetTranslate());
			s = pDC->GetOutputTextExtent(pTS->GetTranslate());
			y += s.cy+40;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorView::OnNextTodo() 
{
	CString s;
	int i;
	BOOL bFound = FALSE;

	BOOL bLoop = TRUE;
	int iCurIndex = m_iCurSel;
	while (!bFound && bLoop)
	{
		for (i=iCurIndex;i<GetListCtrl().GetItemCount();i++)
		{
			// Find TODO! at begin of translated text
			s = GetListCtrl().GetItemText(i,2);
			if (0==s.Find(szTODO))
			{
				if ((i==m_iCurSel) && (i==m_iLastFound)) {
					// already found previous but not changed, skip it
				}
				else {
					bFound = TRUE;
					m_iLastFound = i;
					break;
				}
			}
		}
		
		if (bFound)
		{
			m_iCurSel = i;
			m_dwCurData = GetListCtrl().GetItemData(i);
			GetListCtrl().SetItemState(i, LVIS_FOCUSED |LVIS_SELECTED ,LVIS_FOCUSED |LVIS_SELECTED );
			GetListCtrl().EnsureVisible(i,FALSE);
			UpdateWindow();
			CWkTranslatorDoc* pDoc = GetDocument();
			pDoc->EditTranslateString(LOWORD(m_dwCurData),HIWORD(m_dwCurData), FALSE);
			bLoop = FALSE;
		}
		else
		{
			if (iCurIndex != 0)
			{
				if (AfxMessageBox(IDP_SEARCH_END_OF_DOC, MB_YESNO) == IDNO)
				{
					bLoop = FALSE;
				}
				else
				{
					iCurIndex = 0;
				}
			}
			else
			{
				AfxMessageBox(IDP_NOTHING_TODO);
				bLoop = FALSE;
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorView::OnEditFind() 
{
	CFindDialog dlg(m_sSearchFor);
	if (IDOK == dlg.DoModal())
	{
		m_sSearchFor = dlg.SearchFor();
		m_bCaseSensitive = dlg.CaseSensitive();
		FindText();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorView::OnEditFindNext() 
{
	FindText();
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorView::FindText() 
{
	BOOL bFound = FALSE;
	int iFound;
	BOOL bLoop = TRUE;

	CString sText;
	int iCurIndex = m_iCurSel;
	while (!bFound && bLoop)
	{
		for (int i=iCurIndex ; !bFound && i<GetListCtrl().GetItemCount() ; i++)
		{
			for (int j=1 ; !bFound && j<=2 ; j++)
			{
				sText = GetListCtrl().GetItemText(i,j);
				if(m_bCaseSensitive)
				{
					iFound = sText.Find(m_sSearchFor);
				}
				else
				{
					CString sTemp = m_sSearchFor;
					sTemp.MakeLower();
					sText.MakeLower();
					iFound = sText.Find(sTemp);
				}

				if (-1 != iFound)
				{
					if ((i==m_iCurSel) && (i==m_iLastFound)) 
					{
						// already found previous, skip it
					}
					else
					{
						bFound = TRUE;
						m_iLastFound = i;
						break;
					}
				}
			}
			if (bFound)
			{
				break;
			}
		}
		
		if (bFound)
		{
			m_iCurSel = i;
			GetListCtrl().SetItemState(i, LVIS_FOCUSED |LVIS_SELECTED ,LVIS_FOCUSED |LVIS_SELECTED );
			GetListCtrl().EnsureVisible(i,FALSE);
			UpdateWindow();
			bLoop = FALSE;
		}
		else
		{
			if (iCurIndex != 0)
			{
				if (AfxMessageBox(IDP_SEARCH_END_OF_DOC, MB_YESNO) == IDNO)
				{
					bLoop = FALSE;
				}
				else
				{
					iCurIndex = 0;
				}
			}
			else
			{
				AfxMessageBox(IDP_NO_MORE_FOUND);
				bLoop = FALSE;
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorView::OnUpdateEditFindNext(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_sSearchFor.IsEmpty());
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorView::OnCheckTranslationAll() 
{
	// First(!) ensure, that all texts are visible in the view
	if (m_bShowOnlyTODOs)
	{
		m_bShowOnlyTODOs = FALSE;
		OnTSArrayArray(&(GetDocument()->GetTSArrayArray()));
	}

	// Then(!) set the selection at start
	m_iCurSel = 0;
	GetListCtrl().SetItemState(m_iCurSel, LVIS_FOCUSED |LVIS_SELECTED ,LVIS_FOCUSED |LVIS_SELECTED );
	GetListCtrl().EnsureVisible(m_iCurSel,FALSE);
	UpdateWindow();
	CheckTranslation();
//	WK_TRACE(_T("CheckTranslationAll finished\n"));
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorView::OnCheckTranslation() 
{
	// Ensure, that all texts are visible in the view
	if (m_bShowOnlyTODOs)
	{
		m_bShowOnlyTODOs = FALSE;
		OnTSArrayArray(&(GetDocument()->GetTSArrayArray()));
		// Original selection is removed, so start at beginning
		m_iCurSel = 0;
	}
	CheckTranslation();
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorView::CheckTranslation() 
{
	BOOL bLoop = TRUE;

	CString sOriginal, sTranslation;
	UINT uMaxLength;

	BOOL bHasTodo = FALSE;
	int iStartIndex = m_iCurSel;
	while (bLoop)
	{
		for (int i=iStartIndex ; i<GetListCtrl().GetItemCount() ; i++)
		{
			sOriginal    = GetListCtrl().GetItemText(i,1);
			sTranslation = GetListCtrl().GetItemText(i,2);
			uMaxLength   = _ttoi(GetListCtrl().GetItemText(i,0));
			if(!CTranslateString::CheckTODO(sTranslation))
			{
				bHasTodo = TRUE;
				m_iCurSel = i;
				GetListCtrl().SetItemState(i, LVIS_FOCUSED |LVIS_SELECTED ,LVIS_FOCUSED |LVIS_SELECTED );
				GetListCtrl().EnsureVisible(i,FALSE);
				UpdateWindow();
				CWkTranslatorDoc* pDoc = GetDocument();
				bLoop = pDoc->EditTranslateString(LOWORD(m_dwCurData),HIWORD(m_dwCurData), FALSE);
				if(CTranslateString::CheckTODO(sTranslation))
				{
					bHasTodo = FALSE;
				}
				if (!bLoop) return;
			}
			if(!bHasTodo)
			{
				if (!CTranslateString::CheckDataAll(sOriginal, sTranslation, uMaxLength))
				{
					m_iCurSel = i;
					GetListCtrl().SetItemState(i, LVIS_FOCUSED |LVIS_SELECTED ,LVIS_FOCUSED |LVIS_SELECTED );
					GetListCtrl().EnsureVisible(i,FALSE);
					UpdateWindow();
					CWkTranslatorDoc* pDoc = GetDocument();
					pDoc->EditTranslateString(LOWORD(m_dwCurData),HIWORD(m_dwCurData), TRUE);
				}
			}
		}
		
		if (iStartIndex != 0)
		{
			if (AfxMessageBox(IDP_SEARCH_END_OF_DOC, MB_YESNO) == IDNO)
			{
				bLoop = FALSE;
			}
			else
			{
				iStartIndex = 0;
			}
		}
		else
		{
			AfxMessageBox(IDP_NO_MORE_FOUND);
			bLoop = FALSE;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorView::OnEditReplace() 
{
	CEditReplaceDialog dlg;

	dlg.m_sFind = m_sEdit;
	dlg.m_sReplace = m_sReplace;

	if (IDOK==dlg.DoModal())
	{
		m_sEdit = dlg.m_sFind;
		m_sReplace = dlg.m_sReplace;
	}
	
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorView::EditReplaceText()
{
	BOOL bFound = FALSE;
	int iFound;
	BOOL bLoop = TRUE;

	CString sText;
	int iCurIndex = m_iCurSel;
	while (!bFound && bLoop)
	{
		for (int i=iCurIndex ; !bFound && i<GetListCtrl().GetItemCount() ; i++)
		{
			sText = GetListCtrl().GetItemText(i,1);
			iFound = sText.Find(m_sEdit);

			if (-1 != iFound)
			{
				if ((i==m_iCurSel) && (i==m_iLastFound)) 
				{
					// already found previous, skip it
				}
				else 
				{
					bFound = TRUE;
					m_iLastFound = i;
				}
			}
		}
		
		if (bFound)
		{
			m_iCurSel = i;
			GetListCtrl().SetItemState(i, LVIS_FOCUSED |LVIS_SELECTED ,LVIS_FOCUSED |LVIS_SELECTED );
			GetListCtrl().EnsureVisible(i,FALSE);
			UpdateWindow();

			sText.Replace(m_sEdit,m_sReplace);
			DWORD dwCurData = GetListCtrl().GetItemData(m_iCurSel);
			CWkTranslatorDoc* pDoc = GetDocument();
			pDoc->EditTranslateString(LOWORD(dwCurData),HIWORD(dwCurData), sText);
		}
		else
		{
			if (iCurIndex != 0)
			{
				if (AfxMessageBox(IDP_SEARCH_END_OF_DOC, MB_YESNO) == IDNO)
				{
					bLoop = FALSE;
				}
				else
				{
					iCurIndex = 0;
				}
			}
			else
			{
				AfxMessageBox(IDP_NO_MORE_FOUND);
				bLoop = FALSE;
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorView::OnViewShowOnlyTodo() 
{
	m_bShowOnlyTODOs = !m_bShowOnlyTODOs;
	OnTSArrayArray(&(GetDocument()->GetTSArrayArray()));
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorView::OnUpdateViewShowOnlyTodo(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bShowOnlyTODOs);
}
/////////////////////////////////////////////////////////////////////////////
int CWkTranslatorView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorView::OnViewShowOnlySuggestions() 
{
	m_bShowOnlySuggestions = !m_bShowOnlySuggestions;
	OnTSArrayArray(&(GetDocument()->GetTSArrayArray()));
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorView::OnUpdateViewShowOnlySuggestions(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bShowOnlySuggestions);
}
/////////////////////////////////////////////////////////////////////////////
void CWkTranslatorView::OnEditRemoveAllTodos()
{
	if (AfxMessageBox(IDS_REQUES_TREMOVE_ALL_TODOS, MB_YESNO|MB_ICONWARNING)==IDYES)
	{
		const CTSArrayArray &tsaa = GetDocument()->GetTSArrayArray();
		CTSArray* pTSArray = NULL;
		CTranslateString* pTranslateString;
		int i, j, nCount = tsaa.GetSize();
		for (i=0; i<nCount; i++)
		{
			pTSArray = tsaa.GetAt(i);
			for (j=0;j<pTSArray->GetSize();j++)
			{
				pTranslateString = pTSArray->GetAt(j);
				pTranslateString->SetTranslate(pTranslateString->GetTranslate(TRUE));
			}
		}
		OnTSArrayArray(&tsaa);
	}
}

void CWkTranslatorView::OnUpdateEditRemoveAllTodos(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->GetTSArrayArray().GetCount()> 0);
}
