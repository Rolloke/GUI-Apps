// SVView.cpp : implementation of the CSVView class
//


#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "Wk_Profile.h"
#include "SVDoc.h"
#include "SVView.h"

#include "SVTree.h"
#include "SVPage.h"

#include "ISDNPage.h"
#include "SocketPage.h"
#include "AppPage.h"
#include "HostPage.h"
#include "DrivePage.h"
#include "ArchivPage.h"
#include "globals.h"

#include "CIPCServerControlClientSide.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSVApp theApp;

//#define SV_TREE_WIDTH	200

/////////////////////////////////////////////////////////////////////////////
// CSVView

IMPLEMENT_DYNCREATE(CSVView, CView)

BEGIN_MESSAGE_MAP(CSVView, CView)
	//{{AFX_MSG_MAP(CSVView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_EDIT_NEW, OnEditNew)
	ON_UPDATE_COMMAND_UI(ID_EDIT_NEW, OnUpdateEditNew)
	ON_COMMAND(ID_EDIT_CANCEL, OnEditCancel)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CANCEL, OnUpdateEditCancel)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
	ON_COMMAND(ID_EDIT_SERVERINIT, OnEditServerinit)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SERVERINIT, OnUpdateEditServerinit)
	ON_COMMAND(ID_VIEW_TOOLTIPS, OnViewTooltips)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLTIPS, OnUpdateViewTooltips)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSVView construction/destruction

CSVView::CSVView()
{
	m_pSVTree = new CSVTree(this);
	m_pSVPage = NULL;
	m_bResizing = FALSE;
	m_bClearing = FALSE;
	m_bShowToolTips = TRUE;
	m_lTreeWidth = GetSystemMetrics(SM_CXFULLSCREEN) / 4;

	ClearHTREEITEMs();

	m_bServerInit = FALSE;
	m_bInitApps = FALSE;
}

CSVView::~CSVView()
{
	delete m_pSVTree;
}

void CSVView::ClearHTREEITEMs()
{
	m_SelectedItem = NULL;
	m_DeletedItem  = NULL;

	m_hISDNPage = NULL;
	m_hSocketPage = NULL;
	m_hAutoLogoutPage = NULL;
	m_hHostPage = NULL;
	m_hDrives = NULL;
	m_hArchivs = NULL;
}

BOOL CSVView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
//	cs.style |= DS_CONTROL | WS_CHILD;	// gf dass reicht nicht

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSVView drawing

void CSVView::OnDraw(CDC* pDC)
{
//	CSVDoc* pDoc = GetDocument();
//	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CSVView printing

BOOL CSVView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSVView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSVView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CSVView diagnostics

#ifdef _DEBUG
void CSVView::AssertValid() const
{
	CView::AssertValid();
}

void CSVView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSVDoc* CSVView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSVDoc)));
	return (CSVDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSVView message handlers
int CSVView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	CRect rect(0,0,200,200);
	if (!m_pSVTree->Create(WS_CHILD|WS_VISIBLE|WS_BORDER|
						   TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS|TVS_SHOWSELALWAYS,
						   rect,this,1))
	{
		return -1;
	}
	
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::InitSystem()
{
	CString s;
	COsVersionInfo osinfo;


	// Linkunits
	if (   GetDocument()->GetDongle()->RunAnyLinkUnit() 
		|| GetDocument()->GetDongle()->RunAnyBoxUnit()
		)
	{
		if (GetDocument()->GetDongle()->RunISDNUnit())
		{
			s.LoadString(IDS_ISDN1);
			m_hISDNPage = m_pSVTree->InsertItem(s,IMAGE_ISDN,IMAGE_ISDN);
		}
		if (   GetDocument()->GetDongle()->RunSocketUnit()
			&& !GetDocument()->GetDongle()->IsDemo())
		{
			s.LoadString(IDS_SOCKET);
			m_hSocketPage = m_pSVTree->InsertItem(s,IMAGE_SOCKET,IMAGE_SOCKET);
		}
	}

	if (!GetDocument()->GetDongle()->IsDemo())
	{
		s.LoadString(IDS_AUTOLOGOUT);
		m_hAutoLogoutPage = m_pSVTree->InsertItem(s,IMAGE_AUTOLOGOUT,IMAGE_AUTOLOGOUT);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CSVView::InitHost()
{
	// HOSTS
	if (GetDocument()->GetFlags() & WK_ALLOW_HOST)
	{
		CString s;
		s.LoadString(IDS_HOSTS);
		m_hHostPage = m_pSVTree->InsertItem(s,IMAGE_HOST,IMAGE_HOST);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::InitDatabase()
{
	if (GetDocument()->GetFlags() & WK_ALLOW_ARCHIV)
	{
		CString s;
		s.LoadString(IDS_DRIVES);
		m_hDrives = m_pSVTree->InsertItem(s,IMAGE_DRIVES,IMAGE_DRIVES);
		s.LoadString(IDS_ARCHIVES);
		m_hArchivs = m_pSVTree->InsertItem(s,IMAGE_ARCHIVS,IMAGE_ARCHIVS);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::InitTree()
{
	m_bClearing = TRUE;
	m_pSVTree->DeleteAllItems();

	ClearHTREEITEMs();

	InitSystem();
	InitDatabase();
	if (!GetDocument()->GetDongle()->IsDemo())
	{
		InitHost();
	}

	m_bServerInit = FALSE;
	m_bClearing = FALSE;

	// select the first item in the tree
	m_pSVTree->SelectItem(m_pSVTree->GetRootItem());
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::Clear()
{
	m_bClearing = TRUE;
	if (WK_IS_WINDOW(m_pSVPage))
	{
		m_pSVPage->DestroyWindow();
		m_pSVPage = NULL;
	}
	m_pSVTree->DeleteAllItems();
	ClearHTREEITEMs();
	m_bClearing = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();

	m_pSVTree->SetImageList(theApp.GetSmallImage(),LVSIL_NORMAL);
	
	InitTree();

}
/////////////////////////////////////////////////////////////////////////////
void CSVView::TreeSelchanged(HTREEITEM hNewSelItem)
{
	if (m_bClearing)
	{
		return;
	}

	if (hNewSelItem == m_SelectedItem)
	{
		// nothing todo if nothing changed
		return;
	}

	// first close old page if exist
	if (m_pSVPage)
	{
		BOOL bReturn = TRUE;
		if (m_pSVPage->IsModified())
		{
			if (m_pSVPage->IsDataValid()) 
			{
				int iReturn = AfxMessageBox(IDP_SAVE_CHANGES, MB_YESNOCANCEL);
				switch (iReturn)
				{
					case IDYES:
						m_pSVPage->OnOK();
						if ((m_DeletedItem!=NULL) && (hNewSelItem == m_DeletedItem))
						{
							m_DeletedItem = NULL;
							return;
						}
						bReturn = FALSE;
						break;
					case IDNO:
						m_pSVPage->OnCancel();
						if ((m_DeletedItem!=NULL) && (hNewSelItem == m_DeletedItem))
						{
							m_DeletedItem = NULL;
							return;
						}
						bReturn = FALSE;
						break;
					case IDCANCEL:
						bReturn = TRUE;
					default:
						break;
				}
			}
		}
		else
		{
			// not modified
			bReturn = FALSE;
		}
		if (bReturn)
		{
			m_pSVTree->SelectItem(m_SelectedItem);
			return;
		}
		else
		{
			m_pSVPage->DestroyWindow();
			m_pSVPage = NULL;
		}
	}

	m_SelectedItem = hNewSelItem;

	GetDocument()->Load();

	if (hNewSelItem == m_hHostPage)	{
		m_pSVPage = new CHostPage(this);
	}
	else if (hNewSelItem == m_hISDNPage)	{
		m_pSVPage = new CISDNPage(this);
	}
	else if (hNewSelItem == m_hSocketPage)	{
		m_pSVPage = new CSocketPage(this);
	}
	else if (hNewSelItem == m_hAutoLogoutPage)	{
		m_pSVPage = new CAppPage(this);
	}
	else if (hNewSelItem == m_hDrives) {
		m_pSVPage = new CDrivePage(this);
	}
	else if (hNewSelItem == m_hArchivs) {
		m_pSVPage = new CArchivPage(this);
	}
	else
	{
		HTREEITEM hToSelect = NULL;
		if (m_pSVTree->ItemHasChildren(hNewSelItem))
		{
			m_pSVTree->Expand(hNewSelItem,TVE_EXPAND);
		}
		hToSelect = m_pSVTree->GetNextItem(hNewSelItem,TVGN_NEXTVISIBLE);
		if (hToSelect==NULL)
		{
			hToSelect = m_pSVTree->GetRootItem();
		}
		// this call causes a recursion of TreSelChanged
		m_pSVTree->SelectItem(hToSelect);
		// so return here to prevent from
		// executing further code
		return;
	}

	if (m_pSVPage && m_pSVPage->m_hWnd && IsWindow(m_pSVPage->m_hWnd))
	{
		m_pSVPage->GetClientRect(m_PageRect);
	}

	Resize();
	
	if (m_pSVPage)
	{
		m_pSVPage->ShowWindow(SW_SHOW);
		m_pSVPage->SetFocus();
		m_pSVPage->ActivateTooltips(m_bShowToolTips);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	Resize();
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::Resize()
{
	CRect pr;

	if (m_bResizing)
	{
		// never call this twice
		// prevent us from recursive calls
		return;
	}

	m_bResizing = TRUE;
	GetClientRect(pr);

	// TREE
	if (m_pSVTree && m_pSVTree->m_hWnd && IsWindow(m_pSVTree->m_hWnd))
	{
//		pr.right = SV_TREE_WIDTH;
		pr.right = m_lTreeWidth;
		m_pSVTree->MoveWindow(pr);
	}

	// PAGE
	if (m_pSVPage && m_pSVPage->m_hWnd && IsWindow(m_pSVPage->m_hWnd))
	{
		int xdiff,ydiff;

		GetClientRect(pr);
//		pr.left = SV_TREE_WIDTH+1;
		pr.left = m_lTreeWidth+1;
		xdiff = m_PageRect.Width() - pr.Width();
		ydiff = m_PageRect.Height() - pr.Height();
		if ((xdiff>0) || (ydiff>0))
		{
			CRect mr,vr,sr,tr;
			CMainFrame* pMF = (CMainFrame*)AfxGetMainWnd();

			if (xdiff<0)
			{
				xdiff = 0;
			}
			if (ydiff<0)
			{
				ydiff = 0;
			}
			
			// get rects
			pMF->GetWindowRect(mr);
			pMF->GetStatusBar()->GetWindowRect(sr);
			pMF->GetToolBar()->GetWindowRect(tr);
			GetWindowRect(vr);
			pMF->ScreenToClient(vr);
			pMF->ScreenToClient(tr);
			pMF->ScreenToClient(sr);

			// calc new rects
			mr.right += xdiff;
			pr.right += xdiff;
			vr.right += xdiff;
			tr.right += xdiff;
			sr.right += xdiff;
			mr.bottom += ydiff;
			pr.bottom += ydiff;
			vr.bottom += ydiff;
			tr.bottom += ydiff;
			sr.bottom += ydiff;
			
			// set rects
			m_pSVPage->MoveWindow(pr);
			MoveWindow(vr);
			pMF->GetStatusBar()->MoveWindow(sr);
			pMF->GetToolBar()->MoveWindow(tr);
			pMF->MoveWindow(mr);
			pMF->UpdateWindow();

			if (ydiff>0)
			{
				if (WK_IS_WINDOW(m_pSVTree))
				{
					GetClientRect(pr);
//					pr.right = SV_TREE_WIDTH;
					pr.right = m_lTreeWidth;
					m_pSVTree->MoveWindow(pr);
				}
			}
		}
		else
		{
			m_pSVPage->MoveWindow(pr);
		}
	}
	m_bResizing = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::OnFileSave() 
{
	if (m_pSVPage && m_pSVPage->m_hWnd && IsWindow(m_pSVPage->m_hWnd))
	{
		m_pSVPage->OnOK();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	if (m_pSVPage && m_pSVPage->m_hWnd && IsWindow(m_pSVPage->m_hWnd))
	{
		pCmdUI->Enable(m_pSVPage->IsModified());
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::OnEditCancel() 
{
	if (m_pSVPage && m_pSVPage->m_hWnd && IsWindow(m_pSVPage->m_hWnd))
	{
		m_pSVPage->OnCancel();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::OnUpdateEditCancel(CCmdUI* pCmdUI) 
{
	if (m_pSVPage && m_pSVPage->m_hWnd && IsWindow(m_pSVPage->m_hWnd))
	{
		pCmdUI->Enable(m_pSVPage->IsModified());
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::OnEditNew() 
{
	if (m_pSVPage && m_pSVPage->m_hWnd && IsWindow(m_pSVPage->m_hWnd))
	{
		if (m_pSVPage->CanNew())
		{
			m_pSVPage->OnNew();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::OnUpdateEditNew(CCmdUI* pCmdUI) 
{
	if (m_pSVPage && m_pSVPage->m_hWnd && IsWindow(m_pSVPage->m_hWnd))
	{
		pCmdUI->Enable(m_pSVPage->CanNew());
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::OnEditDelete() 
{
	if (m_pSVPage && m_pSVPage->m_hWnd && IsWindow(m_pSVPage->m_hWnd))
	{
		if (m_pSVPage->CanDelete())
		{
			m_pSVPage->OnDelete();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVView::OnUpdateEditDelete(CCmdUI* pCmdUI) 
{
	if (m_pSVPage && m_pSVPage->m_hWnd && IsWindow(m_pSVPage->m_hWnd))
	{
		pCmdUI->Enable(m_pSVPage->CanDelete());
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVView::CanCloseFrame(BOOL bInit /*= TRUE*/)
{
	if (m_pSVPage)
	{
		if (m_pSVPage->IsModified())
		{
			int r = AfxMessageBox(IDP_SAVE_CHANGES, MB_YESNOCANCEL);
			switch (r)
			{
				case IDYES:
					m_pSVPage->OnOK();
					break;
				case IDNO:
					m_pSVPage->OnCancel();
					m_bServerInit = FALSE;
					break;
				case IDCANCEL:
				default:
					return FALSE;
					break;
			}
		}
		if (m_pSVPage->IsModified())
		{
			return FALSE;
		}
		if (bInit)
		{
			if (WK_IS_RUNNING(WK_APP_NAME_SERVER) && m_bServerInit)
			{
				GetDocument()->InitServer();
			}
			GetDocument()->InitApps();
		}
		m_bInitApps = FALSE;
	}
	return TRUE;
}
///////////////////////////////////////////////////////////////
void CSVView::OnEditServerinit() 
{
	CanCloseFrame();
}
///////////////////////////////////////////////////////////////
void CSVView::OnUpdateEditServerinit(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   (m_bServerInit || m_bInitApps) 
		           && (m_pSVPage!=NULL) 
				   && (WK_IS_RUNNING(WK_APP_NAME_SERVER) || WK_IS_RUNNING(WK_APP_NAME_DB_SERVER))
				   );
}
///////////////////////////////////////////////////////////////
void CSVView::AddInitApp(const CString& sAppname)
{
	m_bInitApps = TRUE;
	GetDocument()->AddInitApp(sAppname);
}
///////////////////////////////////////////////////////////////
void CSVView::OnViewTooltips() 
{
	m_bShowToolTips = !m_bShowToolTips;
	if (WK_IS_WINDOW(m_pSVPage))
	{
		m_pSVPage->ActivateTooltips(m_bShowToolTips);
	}
}
///////////////////////////////////////////////////////////////
void CSVView::OnUpdateViewTooltips(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bShowToolTips);
}
