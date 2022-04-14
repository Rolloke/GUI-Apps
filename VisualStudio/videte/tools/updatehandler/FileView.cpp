// FileView.cpp : implementation file
//

#include "stdafx.h"
#include "UpdateHandler.h"
#include "mainfrm.h"
#include "FileView.h"
#include "DirContent.h"
#include "UpdateHandlerInput.h"
#include "CmdLinParams.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CUpdateHandler theApp;


/////////////////////////////////////////////////////////////////////////////
// TODO! RKE: move to wk_profile
CString NameOfRegType(DWORD dwType)
{
	switch (dwType)
	{
		NAME_OF_ENUM(REG_DWORD);
		NAME_OF_ENUM(REG_SZ);
		NAME_OF_ENUM(REG_BINARY);
		NAME_OF_ENUM(REG_MULTI_SZ);
		NAME_OF_ENUM(REG_EXPAND_SZ);
	}
	return _T("");
}
/////////////////////////////////////////////////////////////////////////////
DWORD RegTypeOfName(CString sType)
{
	RETURN_ENUM_OF_NAME(REG_DWORD, sType);
	RETURN_ENUM_OF_NAME(REG_SZ, sType);
	RETURN_ENUM_OF_NAME(REG_BINARY, sType);
	RETURN_ENUM_OF_NAME(REG_MULTI_SZ, sType);
	RETURN_ENUM_OF_NAME(REG_EXPAND_SZ, sType);
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
// CFileView

IMPLEMENT_DYNCREATE(CFileView, CListView)

CFileView::CFileView()
{
	m_bResizing = FALSE;
	m_bRegistryValues = FALSE;
}

CFileView::~CFileView()
{
}


BEGIN_MESSAGE_MAP(CFileView, CListView)
	//{{AFX_MSG_MAP(CFileView)
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_DELETE, OnFileDelete)
	ON_COMMAND(ID_FILE_EXECUTE, OnFileExecute)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_COMMAND(ID_FILE_RECEIVE, OnFileReceive)
	ON_UPDATE_COMMAND_UI(ID_FILE_DELETE, OnUpdateFileDelete)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXECUTE, OnUpdateFileExecute)
	ON_UPDATE_COMMAND_UI(ID_FILE_RECEIVE, OnUpdateFileReceive)
	ON_COMMAND(ID_REG_EDIT, OnRegEdit)
	ON_UPDATE_COMMAND_UI(ID_REG_EDIT, OnUpdateRegEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileView drawing

void CFileView::OnDraw(CDC* pDC)
{
//	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CFileView diagnostics

#ifdef _DEBUG
void CFileView::AssertValid() const
{
	CListView::AssertValid();
}

void CFileView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
void CFileView::AddFile(const CString& sDir, LPWIN32_FIND_DATA pFFD)
{
/*
    DWORD dwFileAttributes; 
    FILETIME ftCreationTime; 
    FILETIME ftLastAccessTime; 
    FILETIME ftLastWriteTime; 
    DWORD    nFileSizeHigh; 
    DWORD    nFileSizeLow; 
    DWORD    dwReserved0; 
    DWORD    dwReserved1; 
    TCHAR    cFileName[ MAX_PATH ]; 
    TCHAR    cAlternateFileName[ 14 ];
*/
	LV_ITEM lvis;
	SYSTEMTIME sWT;
	FILETIME   fWT;
	CString sName;
	CString sSize;
	CString sTime;
	CString sType;
	int iIndex;
	int iCount;
	//__int64 i64Size = (pFFD->nFileSizeHigh * MAXDWORD) + pFFD->nFileSizeLow;

	sName = (LPCWSTR)pFFD->cFileName;

	sType.Empty();
	sType += (pFFD->dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) ? 'a' : ' ';
	sType += (pFFD->dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED) ? 'c' : ' ';
	sType += (pFFD->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? 'd' : ' ';
	sType += (pFFD->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) ? 'h' : ' ';
	sType += (pFFD->dwFileAttributes & FILE_ATTRIBUTE_NORMAL) ? 'n' : ' ';
	sType += (pFFD->dwFileAttributes & FILE_ATTRIBUTE_OFFLINE) ? 'o' : ' ';
	sType += (pFFD->dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? 'r' : ' ';
	sType += (pFFD->dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) ? 's' : ' ';
	sType += (pFFD->dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY) ? 't' : ' ';

	FileTimeToLocalFileTime(&pFFD->ftLastWriteTime,&fWT);
	FileTimeToSystemTime(&fWT,&sWT);
	sTime.Format(_T("%02d.%02d.%d %02d:%02d:%02d"),
		         sWT.wDay,sWT.wMonth,sWT.wYear, 
				 sWT.wHour,sWT.wMinute,sWT.wSecond);	
	
	if (pFFD->nFileSizeLow > 1048576)
	{
		int nRest = MulDiv(pFFD->nFileSizeLow & 0x000fffff, 1000, 1048576);
		sSize.Format(_T("%ld.%03ld MB"),pFFD->nFileSizeLow >> 20, nRest);
	}
	else if (pFFD->nFileSizeLow>1024)
	{
		int nRest = MulDiv(pFFD->nFileSizeLow & 0x000003ff, 1000, 1024);
		sSize.Format(_T("%ld.%03ld KB"),pFFD->nFileSizeLow >> 10, nRest);
	}
	else 
	{
		sSize.Format(_T("%ld B"),pFFD->nFileSizeLow);
	}

	// always insert at the end, we need the count of existing items
	iCount = GetListCtrl().GetItemCount();

	// insert new item with name of the file
	lvis.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
	lvis.pszText = (LPTSTR)LPCTSTR(sName);
	lvis.cchTextMax = sName.GetLength();
	lvis.lParam = (LPARAM)pFFD;
	lvis.iItem = iCount;
	lvis.iSubItem = 0;
	lvis.iImage=1;
	if (  -1!=sName.Find(_T("mail"))
		&&-1!=sName.Find(_T("lgz")))
	{
		TRACE(_T("insert lgz %s %08lx %d %d\n"),sName,(DWORD)pFFD,pFFD->nFileSizeLow,GetListCtrl().GetItemCount());
	}
	iIndex = GetListCtrl().InsertItem(&lvis);
	if (-1==iIndex)
		return;

	// size of the file
	lvis.mask = LVIF_TEXT;
	lvis.pszText = (LPTSTR)LPCTSTR(sSize);
	lvis.cchTextMax = sSize.GetLength();
	lvis.iItem = iIndex;
	lvis.iSubItem = 1;
	GetListCtrl().SetItem(&lvis);

	// attributes (type) of file	
	lvis.mask = LVIF_TEXT;
	lvis.pszText = (LPTSTR)LPCTSTR(sType);
	lvis.cchTextMax = sType.GetLength();
	lvis.iItem = iIndex;
	lvis.iSubItem = 2;
	GetListCtrl().SetItem(&lvis);

	// time of file
	lvis.mask = LVIF_TEXT;
	lvis.pszText = (LPTSTR)LPCTSTR(sTime);
	lvis.cchTextMax = sTime.GetLength();
	lvis.iItem = iIndex;
	lvis.iSubItem = 3;
	GetListCtrl().SetItem(&lvis);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CFileView::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style = LVS_REPORT | /*LVS_SINGLESEL |*/ LVS_SORTASCENDING | LVS_SHAREIMAGELISTS | 
               LVS_NOLABELWRAP | LVS_ALIGNLEFT | LVS_SHOWSELALWAYS |
			   WS_BORDER | WS_TABSTOP | WS_CHILD | WS_VISIBLE;
	return CListView::PreCreateWindow(cs);
}
/////////////////////////////////////////////////////////////////////////////
void CFileView::OnInitialUpdate() 
{
	CListView::OnInitialUpdate();
	
	const int		iColumns = 4;
	CRect			rect;
	LV_COLUMN		lvcolumn;
	CString s[iColumns];
	int i;
	int w[] = {35,20,15,30};

	// initialize columns of ListCtrl
	GetClientRect(rect);
	s[0].LoadString(IDS_NAME);
	s[1].LoadString(IDS_SIZE);
	s[2].LoadString(IDS_TYP);
	s[3].LoadString(IDS_TIME);

	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;

	for (i = 0 ; i < iColumns ; i++)  // add the columns to the list control
	{
		lvcolumn.cx = MulDiv(rect.Width(), w[i], 100);
		lvcolumn.pszText = s[i].GetBuffer(0);
		lvcolumn.iSubItem = i;
		GetListCtrl().InsertColumn(i, &lvcolumn);  // assumes return value is OK.
		s[i].ReleaseBuffer();
	}

	GetListCtrl().SetImageList(theApp.GetSymbols(),LVSIL_NORMAL);

	LONG dw = ListView_GetExtendedListViewStyle(m_hWnd);
	dw |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyle(m_hWnd,dw);
}
/////////////////////////////////////////////////////////////////////////////
void CFileView::DeleteFile(const CString& sFile)
{
	CString sFileName;
	CString sDirName;
	CString sDirectory(m_Directory);
	int p;
//	CDirContent* pDirContent = NULL;

	if (sDirectory.GetAt(sDirectory.GetLength()-1) != _T('\\'))
	{
		sDirectory += _T("\\");
	}
	p = sFile.ReverseFind(_T('\\'));
	if (p!=-1)
	{
		sDirName = sFile.Left(p+1);
		if (0!=sDirName.CompareNoCase(sDirectory))
		{
			return;
		}
		sFileName = sFile.Mid(p+1);
	}

	int n = GetListCtrl().GetItemCount();
	int i;

	for (i=0;i<n;i++)
	{
		if (0==sFileName.CompareNoCase(GetListCtrl().GetItemText(i,0)))
		{
			GetListCtrl().DeleteItem(i);
			return;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CFileView::Clear()
{
	GetListCtrl().DeleteAllItems();
	m_Directory.Empty();
	m_bRegistryValues = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CFileView::SetDirInfo(const CDirContent* pDirContent)
{
	int i;
	LPWIN32_FIND_DATA lpWFD;

	if (pDirContent==NULL)
	{
		return;
	}
	if (pDirContent->IsEmpty())
	{
		return;
	}

	if (!pDirContent->IsContinued())
	{
		Clear();
	}
	SetColumnText(3, IDS_TIME);
	
	m_Directory = pDirContent->GetDirectory();
	m_bRegistryValues = FALSE;

	for (i=0;i<pDirContent->GetSize();i++)
	{
		lpWFD = pDirContent->GetAt(i);
		if (lpWFD)
		{
			if (!(lpWFD->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				AddFile(pDirContent->GetDirectory(),lpWFD);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CFileView::InsertRegValue(const CString& sReginfo)
{
	CStringArray sa;
	LV_ITEM lvis;
	int     iCount;

	ZERO_INIT(lvis);
	lvis.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
	lvis.lParam = (LPARAM)0;
	lvis.iImage=0;

	iCount = sReginfo.Find(_T("->"));
	if (iCount != -1)
	{
		Clear();
		CString sRegNames;
		m_Directory = sReginfo.Left(iCount);
		m_bRegistryValues = TRUE;
		sRegNames = sReginfo.Mid(iCount+2);
		SplitString(sRegNames, sa, _T(","));
		iCount = sa.GetCount();
		for (lvis.iItem=0; lvis.iItem<iCount; lvis.iItem++)
		{
			lvis.pszText = (LPTSTR)LPCTSTR(sa.GetAt(lvis.iItem));
			lvis.cchTextMax = sa.GetAt(lvis.iItem).GetLength();
			GetListCtrl().InsertItem(&lvis);
		}
		SetColumnText(3, IDS_VALUE);
		return;
	}
	if (m_bRegistryValues)
	{
		iCount = sReginfo.Find(_T("=>"));
		if (   iCount != -1
			&& sReginfo.Find(m_Directory) != -1)
		{
			int i = m_Directory.GetLength()+2;
			CString sRegName, sName, sRegValue;
			sRegName  = sReginfo.Mid(i, iCount-i);
			sRegValue = sReginfo.Mid(iCount+2);
			iCount = GetListCtrl().GetItemCount();
			for (i=0; i<iCount; i++)
			{
				sName = GetListCtrl().GetItemText(i,0);
				if (sRegName == sName)
				{
					DWORD dwType=0, dwSize=0, dwValue=0;
					int n = _stscanf(sRegValue, _T("%d:%d:%d"), &dwType, &dwSize, &dwValue);
					if (n)
					{
						lvis.iItem = i;
						n = sRegValue.Find(_T(":"));
						if (n != -1) n = sRegValue.Find(_T(":"), n+1);
						if (n != -1)
						{
							sName = sRegValue.Mid(n+1);
							GetListCtrl().SetItem(i, 3, LVIF_TEXT, sName, 0, 0, 0, 0);

							if (dwSize == 0)
							{
								switch (dwType)
								{
									case REG_DWORD: dwSize = sizeof(DWORD); break;
									case REG_BINARY: dwSize = sName.GetLength(); break;
									case REG_SZ: case REG_MULTI_SZ:
										dwSize = sName.GetLength()*2;
										break;
								}
							}
							sName.Format(_T("%d"), dwSize);
							GetListCtrl().SetItem(i, 1, LVIF_TEXT, sName, 0, 0, 0, 0);

							sName = NameOfRegType(dwType);
							GetListCtrl().SetItem(i, 2, LVIF_TEXT, sName, 0, 0, 0, 0);
						}
					}
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CFileView::SetColumnText(int nColumn, UINT nID)
{
	CString s;
	if (s.LoadString(nID))
	{
		LVCOLUMN lvc;
		ZERO_INIT(lvc);
		lvc.mask = LVCF_TEXT;
		lvc.cchTextMax = s.GetLength();
		lvc.pszText = (LPTSTR) LPCTSTR(s);
		GetListCtrl().SetColumn(nColumn, &lvc);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CFileView::OnSize(UINT nType, int cx, int cy) 
{
	if (m_bResizing)
	{
		return;
	}
	m_bResizing = TRUE;
	CListView::OnSize(nType, cx, cy);
	
	CMainFrame* pWnd = theApp.GetMainFrame();

	if (pWnd)
	{
		pWnd->ActualizeTitleBarPaneSizes(this);
		int	iColumns = GetListCtrl().GetHeaderCtrl()->GetItemCount();
		int i, nTotal = 0, nWidth, nWidthC, nTotalC;
		int *pnColumnWidth = (int*) _alloca(sizeof(int)*iColumns);
		
		for (i = 0 ; i < iColumns ; i++)  // add the columns to the list control
		{
			pnColumnWidth[i] = GetListCtrl().GetColumnWidth(i);  // assumes return value is OK.
			nTotal += pnColumnWidth[i];
		}

		nTotalC = 0;
		nWidthC = cx;
		// initialize columns of ListCtrl
		for (i = 0 ; i < iColumns ; i++)  // add the columns to the list control
		{
			nWidth = MulDiv(nWidthC, pnColumnWidth[i], nTotal);
			GetListCtrl().SetColumnWidth(i, nWidth);  // assumes return value is OK.
			nTotalC += nWidth;
		}
	}
	m_bResizing = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CFileView::OnFileDelete() 
{
	int n = GetListCtrl().GetItemCount();
	int i;
	CString sFile;

	for (i=0;i<n;i++)
	{
		if (GetListCtrl().GetItemState(i,LVIS_SELECTED) & LVIS_SELECTED)
		{
			sFile = GetListCtrl().GetItemText(i,0);
			if (m_bRegistryValues)
			{
				CString sCmd;
				sCmd.Format(_T("%s %s::%s"), SI_DELETEREGKEY, m_Directory, sFile);
				theApp.GetInput()->DoRequestGetFile(RFU_STRING_INTERFACE, sCmd);
			}
			else
			{
				sFile = m_Directory + sFile;
				theApp.DeleteFile(sFile);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CFileView::OnFileExecute() 
{
	if (GetListCtrl().GetSelectedCount()!=1)
	{
		return;
	}

	int n = GetListCtrl().GetItemCount();
	int i;
	CString sFile;

	for (i=0;i<n;i++)
	{
		if (GetListCtrl().GetItemState(i,LVIS_SELECTED) & LVIS_SELECTED)
		{
			sFile = m_Directory + GetListCtrl().GetItemText(i,0);
			theApp.ExecuteFile(sFile);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CFileView::OnRegEdit()
{
	if (GetListCtrl().GetSelectedCount()!=1)
	{
		return;
	}

	int n = GetListCtrl().GetItemCount();
	int i;

	CString sRegValue, sRegType, sCmd;
	for (i=0;i<n;i++)
	{
		if (GetListCtrl().GetItemState(i,LVIS_SELECTED) & LVIS_SELECTED)
		{
			sRegValue = GetListCtrl().GetItemText(i,0);
			sRegType  = GetListCtrl().GetItemText(i,2);
			CCmdLineParamDlg dlg;
			dlg.m_sHeadline = sRegValue + _T(" (") + sRegType + _T(")");
			dlg.m_sCmdLine  = GetListCtrl().GetItemText(i,3);
			if (dlg.DoModal() == IDOK)
			{
				DWORD dwType = RegTypeOfName(sRegType);
				if (dwType == REG_BINARY)
				{
					sCmd.Format(_T("%s %s::%s::%d:%d:"), SI_SETREGKEY, m_Directory, sRegValue, dwType, dlg.m_sCmdLine.GetLength());
				}
				else
				{
					sCmd.Format(_T("%s %s::%s::%d:"), SI_SETREGKEY, m_Directory, sRegValue, dwType);
				}
				sCmd += dlg.m_sCmdLine;
				theApp.GetInput()->DoRequestGetFile(RFU_STRING_INTERFACE, sCmd);
				GetListCtrl().SetItem(i, 3, 0, dlg.m_sCmdLine, 0, 0, 0, 0);
			}
			return;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CFileView::OnFileReceive() 
{
	int n = GetListCtrl().GetItemCount();
	int i;
	if (m_bRegistryValues)
	{
		CString sRegValue, sCmd;
		for (i=0;i<n;i++)
		{
			if (GetListCtrl().GetItemState(i,LVIS_SELECTED) & LVIS_SELECTED)
			{
				sRegValue = GetListCtrl().GetItemText(i,0);
				sCmd.Format(_T("%s %s::%s"), SI_GETREGKEY, m_Directory, sRegValue);
				theApp.GetInput()->DoRequestGetFile(RFU_STRING_INTERFACE, sCmd);
				return;
			}
		}
	}
	else
	{
		CString sFile;
		DWORD dwSum = 0;
		LPWIN32_FIND_DATA pFFD = NULL;

		for (i=0;i<n;i++)
		{
			if (GetListCtrl().GetItemState(i,LVIS_SELECTED) & LVIS_SELECTED)
			{
				pFFD = (LPWIN32_FIND_DATA)GetListCtrl().GetItemData(i);
				dwSum += pFFD->nFileSizeLow;
			}
		}

		BOOL bFirst = TRUE;

		for (i=0;i<n;i++)
		{
			if (GetListCtrl().GetItemState(i,LVIS_SELECTED) & LVIS_SELECTED)
			{
				sFile = m_Directory + GetListCtrl().GetItemText(i,0);
				if (bFirst)
				{
					theApp.GetFile(sFile,dwSum);
					bFirst = FALSE;
				}
				else
				{
					theApp.GetFile(sFile);
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CFileView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CMenu menu;
	CMenu* pSubMenu;

	if (menu.LoadMenu(IDR_CONTEXT))
	{
		pSubMenu = menu.GetSubMenu(0);
		if (pSubMenu)
		{
			CPoint pt;
			GetCursorPos(&pt);
			COemGuiApi::DoUpdatePopupMenu(this, pSubMenu);
			//ScreenToClient(&pt);
			pSubMenu->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);		
		}	
	}
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CFileView::OnUpdateFileDelete(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetListCtrl().GetItemCount());
}
/////////////////////////////////////////////////////////////////////////////
void CFileView::OnUpdateFileExecute(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetListCtrl().GetItemCount() && m_bRegistryValues == FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CFileView::OnUpdateFileReceive(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetListCtrl().GetItemCount());
}
/////////////////////////////////////////////////////////////////////////////
void CFileView::OnUpdateRegEdit(CCmdUI *pCmdUI)
{
	if (m_bRegistryValues && GetListCtrl().GetItemCount())
	{
		int n = GetListCtrl().GetItemCount();
		int i;

		for (i=0;i<n;i++)
		{
			if (GetListCtrl().GetItemState(i,LVIS_SELECTED) & LVIS_SELECTED)
			{
				if (!GetListCtrl().GetItemText(i,1).IsEmpty())
				{
					pCmdUI->Enable();
					return;
				}
			}
		}
	}

	pCmdUI->Enable(FALSE);
}
