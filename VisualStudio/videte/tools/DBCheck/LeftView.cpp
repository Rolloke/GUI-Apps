// LeftView.cpp : implementation of the CLeftView class
//

#include "stdafx.h"
#include "DBCheck.h"

#include "DBCheckDoc.h"
#include "LeftView.h"
#include "images.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLeftView

IMPLEMENT_DYNCREATE(CLeftView, CTreeView)

BEGIN_MESSAGE_MAP(CLeftView, CTreeView)
	//{{AFX_MSG_MAP(CLeftView)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CTreeView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CTreeView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CTreeView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLeftView construction/destruction

#define NODE_SERVERID   (WORD)1
#define NODE_ARCHIVEID  (WORD)2
#define NODE_DATEID     (WORD)3
#define NODE_SEQUENCEID (WORD)4

CLeftView::CLeftView()
{
	m_hItemFirstSel		  = NULL;
   m_hItemIterateSequence = NULL;
   m_hItemIterateDate     = NULL;
//	m_pQueryDialog		     = NULL;
//	m_pSearchResultDialog  = NULL;
	m_pArchivToolTip	     = NULL;
	m_dwLastRedraw         = 0;
	m_bNeedsRedraw         = FALSE;
	m_bBackupMode          = FALSE;

}

CLeftView::~CLeftView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CLeftView drawing

void CLeftView::OnDraw(CDC* pDC)
{
	CDBCheckDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}


/////////////////////////////////////////////////////////////////////////////
// CLeftView printing

BOOL CLeftView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CLeftView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CLeftView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CLeftView diagnostics

#ifdef _DEBUG
void CLeftView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CLeftView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CDBCheckDoc* CLeftView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDBCheckDoc)));
	return (CDBCheckDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLeftView message handlers
/*
void CLeftView::DisconnectID(UINT nID) 
{
	DWORD dwD = nID;
	DWORD dwData;
	HTREEITEM hItem;

	hItem = GetTreeCtrl().GetRootItem();
	while (hItem)
	{
		dwData = GetTreeCtrl().GetItemData(hItem);
		if (dwData == dwD)
		{
			break;
		}
		hItem = GetTreeCtrl().GetNextSiblingItem(hItem);
	}

	if (hItem)
	{
		CDBCheckDoc* pDoc = GetDocument();
		DWORD dwID = GetTreeCtrl().GetItemData(hItem);
		pDoc->Disconnect((WORD)dwID);
		DisconnectDialogs((WORD)dwID);
	}
	AfxGetMainWnd()->PostMessage(WM_USER);
}
/////////////////////////////////////////////////////////////////////////////
void CLeftView::PopupDisconnectMenu(CPoint pt)
{
	CMenu menu;
	HTREEITEM hItem;
	DWORD dwID;
	CString sItemText,sText;
	int i = 1;

	menu.CreatePopupMenu();
	hItem = GetTreeCtrl().GetRootItem();
	if (hItem)
	{
		while (hItem)
		{
			dwID = GetTreeCtrl().GetItemData(hItem);
			sItemText = GetTreeCtrl().GetItemText(hItem);
			sText.Format("&%d %s",i++,sItemText);
			menu.AppendMenu(MF_STRING,
							dwID+ID_DISCONNECT_LOW,
							sText);
			hItem = GetTreeCtrl().GetNextSiblingItem(hItem);
		}
        menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, AfxGetMainWnd());
	}
	else
	{
		AfxGetMainWnd()->PostMessage(WM_USER);
	}
}
*/
/////////////////////////////////////////////////////////////////////////////
void CLeftView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	WORD cause	  = LOWORD(lHint);
	WORD wServerID = HIWORD(lHint);
	CSecID id;

	switch (cause)
	{
	case VDH_DATABASE:
		InsertDatabase((CIPCDatabaseDBCheck*)pHint);
		break;
	case VDH_REMOVE_SERVER:
		DeleteServer((WORD)pHint);
		break;
	case VDH_ADD_ARCHIV:
		InsertArchiv(GetTreeCtrl(),GetServerItem(wServerID),(CIPCArchivRecord*)pHint);
		break;
	case VDH_ADD_FILE:
		InsertArchivFile(wServerID,(CIPCArchivFileRecord*)pHint);
		break;
	case VDH_ADD_SEQUENCE:
		InsertSequence(wServerID,(CIPCSequenceRecord*)pHint);
		break;
	case VDH_REMOVE_FILE:
		{
			DWORD dw = (DWORD)pHint;
			WORD wArchivNr = LOWORD(dw);
			WORD wSequenceNr = HIWORD(dw);
			RemoveSequence(wServerID,wArchivNr,wSequenceNr);
		}
		break;
/*
   case VDH_BACKUP:
		if (m_bBackupMode)
		{
			OnArchivBackup();
		}
		break;
*/
/*
   case VDH_SYNC_DEL_ARCHIVE:
		{
			DWORD dw = (DWORD)pHint;
			WORD wServerNr = LOWORD(dw);
			WORD wArchivNr = HIWORD(dw);
			HTREEITEM hArchiv = GetArchivItem(wServerNr,wArchivNr);
			if (hArchiv)
			{
				SetCheckRecurse(GetTreeCtrl(),hArchiv,TREE_CONTROL_UNCHECKED,FALSE);
			}
		}
		break;
*/
	}
}
/////////////////////////////////////////////////////////////////////////////
void CLeftView::InsertDatabase(CIPCDatabaseDBCheck* pDatabase)
{
	HTREEITEM hServer;
	CServer*  pServer = pDatabase->GetServer();
	WORD wServerID = pServer->GetID();

	// insert server, input/camera/relais if not in tree
	hServer = GetServerItem(wServerID);
	if (hServer==NULL)
	{
		hServer = InsertServer(pServer);
	}

	if (hServer)
	{
		pDatabase->Lock();
		const CIPCArchivRecordArray& a = pDatabase->GetRecords();
		CIPCArchivRecord* pRecord;
		int i,c;
		c = a.GetSize();
		for (i=0;i<c;i++)
		{
			pRecord = a.GetAt(i);
			InsertArchiv(GetTreeCtrl(),hServer,pRecord);
		}
		pDatabase->Unlock();
		GetTreeCtrl().Expand(hServer,TVE_EXPAND);
/*
      if (WK_IS_WINDOW(m_pQueryDialog))
		{
			m_pQueryDialog->InitServerArchives();
		}
*/
	}
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CLeftView::InsertArchiv(CTreeCtrl& ctrl,HTREEITEM hServer, CIPCArchivRecord* pRecord)
{
	HTREEITEM hArchiv = NULL;
	TV_INSERTSTRUCT tvis;
	CString s = pRecord->GetName();

   tvis.hParent = hServer;
   tvis.hInsertAfter = TVI_SORT;
   tvis.item.mask    = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

   tvis.item.lParam     = MAKELONG(pRecord->GetSubID(), NODE_ARCHIVEID);
   tvis.item.cchTextMax = s.GetLength();
   tvis.item.pszText    = (char*)LPCTSTR(s);

	int i = -1;

	if (pRecord->IsRing())
	{
		i = _IMAGE_RING;
	}
	else if (pRecord->IsSafeRing())
	{
		i = _IMAGE_VORALARM;
	}
	else if (pRecord->IsAlarm())
	{
		i = _IMAGE_ALARM;
	}
	else if (pRecord->IsBackup())
	{
		i = _IMAGE_BACKUP;
		tvis.hInsertAfter = TVI_LAST;
	}
	else if (pRecord->IsAlarmList())
	{
		i = _IMAGE_ALARMLIST;
	}
	else
	{
		WK_TRACE_ERROR("unknown archive type %s %x\n",pRecord->GetName(),pRecord->GetFlags());
	}
	if (pRecord->IsSearch())
	{
		i = _IMAGE_SEARCH;
	}

	if (i!=-1)
	{
		tvis.item.iImage = i;
		tvis.item.iSelectedImage = i;
		hArchiv = ctrl.InsertItem(&tvis);

		if (tvis.item.iImage == _IMAGE_RING)
		{
/*
			if (((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode())
			{
				SetCheckRecurse(GetTreeCtrl(),hArchiv,TREE_CONTROL_UNCHECKED,FALSE);
			}
*/
		}
	}

	return hArchiv;
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CLeftView::GetServerItem(WORD wServer)
{
	HTREEITEM hServer;

	hServer = GetTreeCtrl().GetRootItem();
	while (hServer)
	{
		if (wServer==LOWORD(GetTreeCtrl().GetItemData(hServer)))
		{
			break;
		}
		// next server, if any
		hServer = GetTreeCtrl().GetNextSiblingItem(hServer);
	}
	return hServer;
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CLeftView::GetArchivItem(WORD wServer, WORD wArchivNr)
{
	HTREEITEM hServer;
	HTREEITEM hChild;

	hServer = GetTreeCtrl().GetRootItem();
	while (hServer)
	{
		if (wServer==LOWORD(GetTreeCtrl().GetItemData(hServer)))
		{
			// bingo found server
			// searching archiv
			hChild = GetTreeCtrl().GetChildItem(hServer);
			while (hChild)
			{
				if (wArchivNr==LOWORD(GetTreeCtrl().GetItemData(hChild)))
				{
					// found archiv
					return hChild;
				}
				hChild = GetTreeCtrl().GetNextSiblingItem(hChild);
			}
		}
		// next server, if any
		hServer = GetTreeCtrl().GetNextSiblingItem(hServer);
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CLeftView::GetServerItem(HTREEITEM hItem)
{
	if (IsServerItem(hItem))
	{
		return hItem;
	}
	else if (IsArchivItem(hItem))
	{
		return GetTreeCtrl().GetParentItem(hItem);
	}
	else if (IsSequenceItem(hItem))
	{
		return GetTreeCtrl().GetParentItem(GetTreeCtrl().GetParentItem(hItem));
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLeftView::IsServerItem(HTREEITEM hItem)
{
   return (HIWORD(GetTreeCtrl().GetItemData(hItem))==NODE_SERVERID) ? TRUE : FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLeftView::IsArchivItem(HTREEITEM hItem)
{
/*
	HTREEITEM hParent = GetTreeCtrl().GetParentItem(hItem);
	if (hParent==NULL)
	{
		return FALSE;
	}
	hParent = GetTreeCtrl().GetParentItem(hParent);
	return hParent == NULL;
*/
   return (HIWORD(GetTreeCtrl().GetItemData(hItem))==NODE_ARCHIVEID) ? TRUE : FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLeftView::IsSequenceItem(HTREEITEM hItem)
{
/*
	HTREEITEM hParent = GetTreeCtrl().GetParentItem(hItem);
	if (hParent==NULL)
	{
		return FALSE;
	}
	hParent = GetTreeCtrl().GetParentItem(hParent);
	return hParent != NULL;
*/
   return (HIWORD(GetTreeCtrl().GetItemData(hItem))==NODE_SEQUENCEID) ? TRUE : FALSE;
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CLeftView::InsertServer(const CServer* pServer)
{
	TV_INSERTSTRUCT tvis;
	HTREEITEM hServer = NULL;
	CString s         = pServer->GetFullName();
   WORD    wServerID = pServer->GetID();

   tvis.hParent = TVI_ROOT;
   tvis.hInsertAfter = pServer->IsLocal() ? TVI_FIRST : TVI_LAST;

   tvis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
   tvis.item.lParam = MAKELONG(wServerID, NODE_SERVERID);
	
   tvis.item.cchTextMax = s.GetLength();
   tvis.item.pszText    = (char*)LPCTSTR(s);
	    
   tvis.item.iImage = _IMAGE_HOSTBASE + ((wServerID-1) % _IMAGE_NRHOST);
   tvis.item.iSelectedImage = tvis.item.iImage;

	hServer = GetTreeCtrl().InsertItem(&tvis);

	GetTreeCtrl().SelectItem(hServer);

	return hServer;
}
/////////////////////////////////////////////////////////////////////////////

BOOL CLeftView::DeleteServer(WORD wID)
{
	HTREEITEM hItem;

//	DisconnectDialogs(wID);
	hItem = GetTreeCtrl().GetRootItem();
	while (hItem)
	{
		if (wID==LOWORD(GetTreeCtrl().GetItemData(hItem)))
		{
			GetTreeCtrl().DeleteItem(hItem);
			return TRUE;
		}
		hItem = GetTreeCtrl().GetNextSiblingItem(hItem);
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
/*
void CLeftView::DisconnectDialogs(WORD wServerID)
{
	if (WK_IS_WINDOW(m_pQueryDialog))
	{
		m_pQueryDialog->Disconnect(wServerID);
	}
	if (WK_IS_WINDOW(m_pSearchResultDialog))
	{
		m_pSearchResultDialog->Disconnect(wServerID);
	}
}
*/
/////////////////////////////////////////////////////////////////////////////
void CLeftView::OnInitialUpdate() 
{
	CTreeView::OnInitialUpdate();
	
	GetTreeCtrl().SetIndent(0);

	m_Images.Create(IDB_IMAGES,_IMAGE_WIDTH,0,RGB(0,255,255));
	m_States.Create(IDB_STATES,14,0,RGB(0,255,255));

	GetTreeCtrl().SetImageList(&m_Images,TVSIL_NORMAL);
	GetTreeCtrl().SetImageList(&m_States,TVSIL_STATE);

	EnableToolTips(TRUE);
	// Alle wollen sie ToolTips haben
	if (m_ToolTip.Create(this, TTS_ALWAYSTIP|TTS_NOPREFIX) && m_ToolTip.AddTool(this))
	{
		m_ToolTip.Activate(TRUE);
		// Lasse den ToolTip die Zeilenumbrueche beachten
		m_ToolTip.SendMessage(TTM_SETMAXTIPWIDTH, 0, SHRT_MAX);

		// Wenn man will, kann man auch noch die Zeiten veraendern
		// Verweildauer des ToolTips (Maximale Zeit=SHRT_MAX)
		m_ToolTip.SendMessage(TTM_SETDELAYTIME, TTDT_AUTOPOP, SHRT_MAX);
		// Wie schnell soll er erscheinen, wenn der Cursor ueber einem Tool steht
		m_ToolTip.SendMessage(TTM_SETDELAYTIME, TTDT_INITIAL, 100);
		// Mindest-Verzoegerungszeit bis zum naechsten Erscheinen
		m_ToolTip.SendMessage(TTM_SETDELAYTIME, TTDT_RESHOW, 200);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLeftView::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style = WS_CHILD | WS_VISIBLE | WS_BORDER | 
			   TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS;
	return CTreeView::PreCreateWindow(cs);
}
/////////////////////////////////////////////////////////////////////////////
/*
void CLeftView::OnDisconnect() 
{
	// beim ersten mal ESC nur den Fullscreen Modus
	// zurückschalten !
	CMainFrame* pMF = (CMainFrame*)AfxGetMainWnd();
	if (pMF && pMF->IsFullScreen())
	{
		pMF->ChangeFullScreenModus();
		return;
	}

	// Kein Fullscreen also richtig trennen.

	HTREEITEM hItem;
	HTREEITEM hParent=NULL;

	hItem = GetTreeCtrl().GetSelectedItem();
	while (hItem)
	{
		hParent = hItem;
		hItem = GetTreeCtrl().GetParentItem(hItem);
	}

	if (hParent)
	{
		CDBCheckDoc* pDoc = GetDocument();
		DWORD dwID = GetTreeCtrl().GetItemData(hParent);
		pDoc->Disconnect((WORD)dwID);
		DisconnectDialogs((WORD)dwID);
	}
}
*/
/////////////////////////////////////////////////////////////////////////////
/*
LRESULT CLeftView::OnUser(WPARAM wParam, LPARAM lParam) 
{
	WORD cause	  = LOWORD(wParam);
	WORD wServerID = HIWORD(wParam);

	switch (cause)
	{
	case VDH_DEL_ARCHIV:
		RemoveArchiv(wServerID,(WORD)lParam);
		GetDocument()->UpdateMyViews(this,wParam,(CObject*)lParam);
		break;
	}
	return 0L;
}
*/
/////////////////////////////////////////////////////////////////////////////
void CLeftView::InsertArchivFile(WORD wServerID,CIPCArchivFileRecord* pFile)
{
	HTREEITEM hArchiv;
	hArchiv = GetArchivItem(wServerID,pFile->GetArchivNr());
	if (hArchiv)
	{
		m_bNeedsRedraw = !GetTreeCtrl().ItemHasChildren(hArchiv);
		HTREEITEM hArchivFile = NULL;
		TV_INSERTSTRUCT tvis;
		CTime t = pFile->GetFirstTime();
		CString s;

		s.Format("%02d.%02d.%02d %02d:%02d:%02d",
				 t.GetDay(),t.GetMonth(),t.GetYear()%100,
				 t.GetHour(),t.GetMinute(),t.GetSecond());

		tvis.hParent = hArchiv;
		tvis.hInsertAfter = TVI_LAST;
		tvis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

		tvis.item.lParam = MAKELONG(pFile->GetDirNr(), NODE_SEQUENCEID);
		tvis.item.cchTextMax = s.GetLength();
		tvis.item.pszText = s.GetBuffer(0);

		int i = 3;
		GetTreeCtrl().GetItemImage(hArchiv,i,i);
		if (pFile->WasSafeRing())
		{
			i = _IMAGE_VORALARM;
		}

		if (i!=-1)
		{
			tvis.item.iImage = i;
			tvis.item.iSelectedImage = i;
			hArchivFile = GetTreeCtrl().InsertItem(&tvis);
		}
		s.ReleaseBuffer();
/*
		if (   WK_IS_WINDOW(m_pSearchResultDialog) 
			&& (pFile->GetArchivNr()==255))
		{
			// ist ein Suchergebnis, bitte oeffnen
			CDBCheckDoc* pDoc = GetDocument();
			if (pDoc)
			{
				CServer* pServer = pDoc->GetServer(wServerID);
				if (pServer)
				{
					pServer->OpenSequence(pFile->GetArchivNr(),pFile->GetDirNr());
				}
			}
		}
*/
	}
	Redraw();
}
/////////////////////////////////////////////////////////////////////////////
void CLeftView::InsertSequence(WORD wServerID,CIPCSequenceRecord* pSequence)
{
	HTREEITEM hArchiv;
	hArchiv = GetArchivItem(wServerID,pSequence->GetArchiveNr());
	if (hArchiv)
	{
      CTreeCtrl &theCtrl = GetTreeCtrl();
		// nur neu zeichnen, wenn da nicht
		// schon childs waren
      BOOL bHasChildren = theCtrl.ItemHasChildren(hArchiv);
		m_bNeedsRedraw = !bHasChildren;
		HTREEITEM hArchivFile = NULL;
      HTREEITEM hItem       = NULL;
		TV_INSERTSTRUCT tvis;
		CString     s     = pSequence->GetName();
      CSystemTime tTime = pSequence->GetTime();
      CString     sDate = tTime.GetDate();
      if (bHasChildren)
      {
         hItem = theCtrl.GetNextItem(hArchiv, TVGN_CHILD);
         while (hItem)
         {
            if (sDate == theCtrl.GetItemText(hItem))
            {
               break;
            }
            hItem = theCtrl.GetNextItem(hItem, TVGN_NEXT);
         }
      }

#ifdef _DEBUG
		s.Format("%s %04d, %s",pSequence->GetName(),
							   pSequence->GetSequenceNr(),
							   pSequence->GetSize().Format(TRUE));
#endif
//		tvis.hParent = hArchiv;
		tvis.hInsertAfter = TVI_LAST;
		tvis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

		int i = -1;
		if (pSequence->IsSafeRing())
		{
			i = _IMAGE_VORALARM;
		}
		else if (pSequence->IsAlarm())
		{
			i = _IMAGE_ALARM;
		}
		else if (pSequence->IsRing())
		{
			i = _IMAGE_RING;
		}
		else if (pSequence->IsBackup())
		{
			i = _IMAGE_BACKUP;
		}
		else if (pSequence->IsAlarmList())
		{
			i = _IMAGE_ALARMLIST;
		}
		else
		{
			WK_TRACE("unknown sequence type %x %s ar=%d\n",
				pSequence->GetFlags(),pSequence->GetName(),pSequence->GetArchiveNr());
		}
		if (pSequence->IsSearch())
		{
			i = _IMAGE_SEARCH;
		}
		if (pSequence->IsSuspect())
		{
			i = _IMAGE_SUSPECT;
		}

      if (hItem == NULL)
      {
         tvis.hParent = hArchiv;
			tvis.item.iImage = i;
			tvis.item.iSelectedImage = i;
		   tvis.item.cchTextMax = sDate.GetLength();
		   tvis.item.pszText    = (char*)LPCTSTR(sDate);
   		tvis.item.lParam     = MAKELONG(pSequence->GetSequenceNr(), NODE_DATEID);
         tvis.hParent = theCtrl.InsertItem(&tvis);
      }
      else
      {
         tvis.hParent = hItem;
      }

      tvis.item.cchTextMax = s.GetLength();
		tvis.item.pszText    = (char*)LPCTSTR(s);
		tvis.item.lParam     = MAKELONG(pSequence->GetSequenceNr(), NODE_SEQUENCEID);

		if (i!=-1)
		{
			tvis.item.iImage = i;
			tvis.item.iSelectedImage = i;
			hArchivFile = theCtrl.InsertItem(&tvis);
		}

      /*
		if (   WK_IS_WINDOW(m_pSearchResultDialog) 
			&& (pSequence->IsSearch())//GetArchiveNr()==255)
			&& theApp.GetQueryParameter().ShowPictures()
			&& theApp.GetQueryParameter().CopyPictures())
		{
			// ist ein Suchergebnis, bitte oeffnen
			CDBCheckDoc* pDoc = GetDocument();
			if (pDoc)
			{
				CServer* pServer = pDoc->GetServer(wServerID);
				if (pServer)
				{
					pServer->OpenSequence(pSequence->GetArchiveNr(),
										  pSequence->GetSequenceNr());
				}
			}
		}
*/
	}
	Redraw();
}
/////////////////////////////////////////////////////////////////////////////
void CLeftView::RemoveSequence(WORD wServerID, WORD wArchivNr, WORD wSequenceNr)
{
	HTREEITEM hArchiv;
	hArchiv = GetArchivItem(wServerID,wArchivNr);
	if (hArchiv)
	{
		HTREEITEM hChild;
		WORD wDirNr;
		hChild = GetTreeCtrl().GetChildItem(hArchiv);
		// bestehende Tree Items pruefen
		while (hChild)
		{
			wDirNr = LOWORD(GetTreeCtrl().GetItemData(hChild));
			if (wDirNr==wSequenceNr)
			{
				GetTreeCtrl().DeleteItem(hChild);
				break;
			}
			hChild = GetTreeCtrl().GetNextSiblingItem(hChild);
		}
	}
	Redraw();
}
/////////////////////////////////////////////////////////////////////////////
void CLeftView::RemoveArchiv(WORD wServerID, WORD wArchivNr)
{
	HTREEITEM hArchiv;
	hArchiv = GetArchivItem(wServerID,wArchivNr);
	if (hArchiv)
	{
		GetTreeCtrl().DeleteItem(hArchiv);
	}
	Redraw();
}
/////////////////////////////////////////////////////////////////////////////
void CLeftView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_bBackupMode)
	{
		return;
	}
	CPoint pt;
	GetCursorPos(&pt);
	HTREEITEM hItem;
	UINT nFlags;
   CTreeCtrl &theCtrl = GetTreeCtrl();

	ScreenToClient(&pt);
	hItem = theCtrl.HitTest(pt,&nFlags);

	if (hItem && (nFlags & TVHT_ONITEM))
	{
		HTREEITEM hArchiv;
      DWORD dwID = theCtrl.GetItemData(hItem);
      WORD  wDir = 0;
      switch (HIWORD(dwID))
      {
         case NODE_SEQUENCEID:
            wDir    = LOWORD(dwID);
            hItem   = theCtrl.GetParentItem(hItem);
       		hArchiv = theCtrl.GetParentItem(hItem);
            break;
         case NODE_DATEID:
            wDir    = LOWORD(dwID);
      		hArchiv = theCtrl.GetParentItem(hItem);
            m_hItemIterateSequence = theCtrl.GetNextItem(hItem, TVGN_CHILD);
            break;
         case NODE_ARCHIVEID:
            hArchiv = hItem;
            hItem   = theCtrl.GetNextItem(hArchiv, TVGN_CHILD);
            dwID    = theCtrl.GetItemData(hItem);
            wDir    = LOWORD(dwID);
            m_hItemIterateDate     = hItem;
            m_hItemIterateSequence = theCtrl.GetNextItem(m_hItemIterateDate, TVGN_CHILD);
            break;
      }
		if (hArchiv)
		{
			HTREEITEM hServer = theCtrl.GetParentItem(hArchiv);
			if (hServer)
			{
				WORD wServerID = LOWORD(theCtrl.GetItemData(hServer));
				WORD wArchiv   = LOWORD(theCtrl.GetItemData(hArchiv));

				CDBCheckDoc* pDoc = GetDocument();
				if (pDoc)
				{
					CServer* pServer = pDoc->GetServer(wServerID);
					if (pServer)
					{
						pServer->OpenSequence(wArchiv,wDir);
					}
				}
			}
		}
	}
	
	*pResult = 0;
}

bool CLeftView::IterateSequences()
{
   CTreeCtrl &theCtrl = GetTreeCtrl();
   if (m_hItemIterateSequence)
   {
      m_hItemIterateSequence = theCtrl.GetNextItem(m_hItemIterateSequence, TVGN_NEXT);
      if (m_hItemIterateSequence)
      {
         ASSERT(HIWORD(theCtrl.GetItemData(m_hItemIterateSequence)) == NODE_SEQUENCEID);
         HTREEITEM hDate   = theCtrl.GetParentItem(m_hItemIterateSequence);
   		HTREEITEM hArchiv = theCtrl.GetParentItem(hDate);
		   if (hArchiv)
		   {
			   HTREEITEM hServer = theCtrl.GetParentItem(hArchiv);
			   if (hServer)
			   {
				   WORD wServerID = LOWORD(theCtrl.GetItemData(hServer));
				   WORD wArchiv   = LOWORD(theCtrl.GetItemData(hArchiv));
				   WORD wDir      = LOWORD(theCtrl.GetItemData(m_hItemIterateSequence));

				   CDBCheckDoc* pDoc = GetDocument();
				   if (pDoc)
				   {
					   CServer* pServer = pDoc->GetServer(wServerID);
					   if (pServer)
					   {
						   pServer->OpenSequence(wArchiv,wDir);
                     return true;
					   }
				   }
			   }
         }
      }
      else
      {
         if (m_hItemIterateDate)
         {
            m_hItemIterateDate = theCtrl.GetNextItem(m_hItemIterateDate, TVGN_NEXT);
            if (m_hItemIterateDate)
            {
               m_hItemIterateSequence = theCtrl.GetNextItem(m_hItemIterateDate, TVGN_CHILD);
               IterateSequences();
            }
         }
      }
   }
   return false;
}

bool CLeftView::CanIterateSequences()
{
   return (m_hItemIterateSequence != NULL) ? true : false;
}

/////////////////////////////////////////////////////////////////////////////
/*
void CLeftView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint pt;
	GetCursorPos(&pt);
	HTREEITEM hItem;
	UINT nFlags;

	ScreenToClient(&pt);
	hItem = GetTreeCtrl().HitTest(pt,&nFlags);

	// right click on image
	if (hItem && (nFlags & TVHT_ONITEM))
	{
		GetTreeCtrl().SelectItem( hItem );
		ClientToScreen(&pt);
		ContextMenu(pt,hItem);
	}
	
	*pResult = 0;
}
*/
///////////////////////////////////////////////////////////////////////
/*
void CLeftView::ContextMenu(const CPoint& pt, HTREEITEM hItem)
{
	CMenu menu;
	CMenu* pPopup = NULL;
	UINT iMenuItemID = 0;

	menu.LoadMenu(IDR_CONTEXT);

	if (IsServerItem(hItem))
	{
		pPopup = menu.GetSubMenu(0);
		GetTreeCtrl().SelectItem(hItem);

		CWK_Dongle dongle;
		if (!dongle.RunCDRWriter())
		{
			pPopup->DeleteMenu(ID_ARCHIV_BACKUP,MF_BYCOMMAND);
		}
	}
	else if (IsArchivItem(hItem))
	{
		// nur bei nicht DTS Sendern!
		CServer* pServer = GetServer(GetTreeCtrl().GetParentItem(hItem));
		if (   pServer 
			&& !pServer->IsDV()
			)
		{
			// Es können nur Backuparchive gelöscht werden
			// Alle anderen werden geleert.
			pPopup = menu.GetSubMenu(1);
			CIPCArchivRecord* pArchive = GetArchiveRecord(hItem);
			if (   pArchive 
				&& !pArchive->IsBackup())
			{
				iMenuItemID = pPopup->GetMenuItemID(1);
				pPopup->RemoveMenu(ID_ARCHIV_DELETE, MF_BYCOMMAND);
			}
		}
	}
	else if (IsSequenceItem(hItem))
	{
		pPopup = menu.GetSubMenu(2);
		HTREEITEM hItemArchiv = GetTreeCtrl().GetParentItem(hItem);
		if (hItemArchiv)
		{
			HTREEITEM hItemServer = GetTreeCtrl().GetParentItem(hItemArchiv);
			if (hItemServer)
			{
				CServer* pServer = GetServer(hItemServer);
				if (pServer && pServer->IsDV())
				{
#ifndef _DEBUG
					pPopup->DeleteMenu(ID_DELETE,MF_BYCOMMAND);
#endif
				}
			}
		}
	}

	if (pPopup)
	{
		pPopup->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,AfxGetMainWnd());
	}
}
*/
///////////////////////////////////////////////////////////////////////
/*
void CLeftView::OnSequenceOpenSingle() 
{
	if (m_bBackupMode)
	{
		return;
	}
	CMainFrame* pMF = (CMainFrame*)theApp.m_pMainWnd;
	if (   WK_IS_WINDOW(pMF)
		&& pMF->IsSyncMode())
	{
		return;
	}

	GetDocument()->GetRechercheView()->CloseAllWindows();

	OnSequenceOpen();
}
*/
///////////////////////////////////////////////////////////////////////
/*
void CLeftView::OnSequenceOpen() 
{
	if (m_bBackupMode)
	{
		return;
	}
	CMainFrame* pMF = (CMainFrame*)theApp.m_pMainWnd;
	if (   WK_IS_WINDOW(pMF)
		&& pMF->IsSyncMode())
	{
		return;
	}

	HTREEITEM hArchiv;
	HTREEITEM hItem;
	hItem = GetTreeCtrl().GetSelectedItem();
	hArchiv = GetTreeCtrl().GetParentItem(hItem);
	if (hArchiv)
	{
		HTREEITEM hServer = GetTreeCtrl().GetParentItem(hArchiv);
		if (hServer)
		{
			WORD wServerID = (WORD)GetTreeCtrl().GetItemData(hServer);
			WORD wArchiv  = (WORD)GetTreeCtrl().GetItemData(hArchiv);
			WORD wDir = (WORD)GetTreeCtrl().GetItemData(hItem);

			CDBCheckDoc* pDoc = GetDocument();
			if (pDoc)
			{
				CServer* pServer = pDoc->GetServer(wServerID);
				if (pServer)
				{
					pServer->OpenSequence(wArchiv,wDir);
				}
			}
		}
	}
}
*/
///////////////////////////////////////////////////////////////////////
/*
void CLeftView::OnDelete() 
{
	CDBCheckDoc* pDoc = GetDocument();
	if (!pDoc)
	{
		return;
	}

	HTREEITEM hItem;
	int r;

	r = IDCANCEL;

	// This can be time consuming for very large trees 
	// and is called every time the user does a normal selection
	// If performance is an issue, it may be better to maintain 
	// a list of selected items
	for ( hItem=GetTreeCtrl().GetRootItem(); hItem!=NULL;  hItem=GetTreeCtrl().GetNextItem( hItem ,TVGN_NEXTVISIBLE) )
	{
		if (!(GetTreeCtrl().GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED ))
		{
			// keine selectierten
			continue;
		}

		if (IsArchivItem(hItem))
		{
			HTREEITEM hServer = GetTreeCtrl().GetParentItem(hItem);
			if (hServer)
			{
				WORD wServerID = (WORD)GetTreeCtrl().GetItemData(hServer);
				CServer* pServer = pDoc->GetServer(wServerID);
				if (   pServer 
					&& pServer->IsConnected()
#ifndef _DEBUG
					&& !pServer->IsDV()
#endif
					)
				{
					HTREEITEM hSequence;
					hSequence = GetTreeCtrl().GetChildItem(hItem);
					WORD wArchiv  = (WORD)GetTreeCtrl().GetItemData(hItem);
					CString m;
					CString s = GetTreeCtrl().GetItemText(hItem);
					m.Format(IDP_DELETE_ARCHIV,s,pServer->GetName());

					if (hSequence)
					{
						if (r!=IDYESALL)
						{
							r = COemGuiApi::MessageBox(m,0,MB_YESNOCANCEL|MB_YESALL|MB_ICONQUESTION);
						}
						if (r==IDCANCEL)
						{
							break;
						}
						if ((r==IDYES) || (r==IDYESALL))
						{
							while (hSequence)
							{
								if ( GetTreeCtrl().GetItemState( hSequence, TVIS_SELECTED ) & TVIS_SELECTED )
								{
									// selected will be added later
								}
								else
								{
									WORD wDir = (WORD)GetTreeCtrl().GetItemData(hSequence);
									pServer->AddDeleteArchivFile(wArchiv,wDir);
								}
								hSequence = GetTreeCtrl().GetNextSiblingItem(hSequence);
							}
						}
					}
				}
			}
		}
		if (IsSequenceItem(hItem))
		{
			HTREEITEM hArchiv;
			hArchiv = GetTreeCtrl().GetParentItem(hItem);
			if (hArchiv)
			{
				HTREEITEM hServer = GetTreeCtrl().GetParentItem(hArchiv);
				if (hServer)
				{
					WORD wServerID = (WORD)GetTreeCtrl().GetItemData(hServer);
					WORD wArchiv  = (WORD)GetTreeCtrl().GetItemData(hArchiv);
					WORD wDir = (WORD)GetTreeCtrl().GetItemData(hItem);

					CServer* pServer = pDoc->GetServer(wServerID);
					if (pServer 
#ifndef _DEBUG
						&& !pServer->IsDV()
#endif
						)
					{
						CString m;
						CString s = GetTreeCtrl().GetItemText(hItem);
						m.Format(IDP_DELETE_SEQUENCE,s,pServer->GetName());
						if (r!=IDYESALL)
						{
							r = COemGuiApi::MessageBox(m,0,MB_YESNOCANCEL|MB_YESALL|MB_ICONQUESTION);
						}
						if (r==IDCANCEL)
						{
							break;
						}
						if ((r==IDYES) || (r==IDYESALL))
						{
							if (pServer && pServer->IsConnected())
							{
								pServer->AddDeleteArchivFile(wArchiv,wDir);
							}
						}
					}
				}
			}
		}
	}
	pDoc->DeleteArchivFiles();
}
*/
///////////////////////////////////////////////////////////////////////
/*
void CLeftView::OnDatabaseInfo() 
{
	CDBCheckDoc* pDoc = GetDocument();
	if (!pDoc)
	{
		return;
	}
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	HTREEITEM hServer;

	if (IsServerItem(hItem))
	{
		hServer = hItem;
	}
	else
	{
		hServer = GetServerItem(hItem);
	}

	if (hServer && IsServerItem(hServer))
	{
		WORD wServerID = (WORD)GetTreeCtrl().GetItemData(hServer);

		CServer* pServer = pDoc->GetServer(wServerID);
		if (pServer && pServer->IsConnected())
		{
			pServer->ShowInfoDialog();
		}
	}
}
*/
///////////////////////////////////////////////////////////////////////
/*
void CLeftView::OnSearch() 
{
	CDBCheckDoc* pDoc = GetDocument();
	if (!pDoc)
	{
		return;
	}
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	HTREEITEM hServer = GetServerItem(hItem);

	if (hItem && IsServerItem(hServer))
	{
		WORD wServerID = (WORD)GetTreeCtrl().GetItemData(hServer);

		CServer* pServer = pDoc->GetServer(wServerID);
		if (pServer && pServer->IsConnected())
		{
			if (m_pQueryDialog)
			{
			}
			else
			{
				m_pQueryDialog = new CQueryDialog(this);
				m_pQueryDialog->ShowWindow(SW_SHOW);
			}
		}
	}
}
*/
///////////////////////////////////////////////////////////////////////
/*
void CLeftView::OnUpdateSearch(CCmdUI* pCmdUI) 
{
	if (m_bBackupMode)
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(   GetTreeCtrl().GetRootItem()!=NULL 
					   && m_pSearchResultDialog==NULL
					   && !((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode());
	}
}
///////////////////////////////////////////////////////////////////////
void CLeftView::ShowSearchResultDialog()
{
	m_pSearchResultDialog = new CSearchResultDialog(this);
}
*/
//////////////////////////////////////////////////////////////////////////////////
/*
BOOL CLeftView::PreTranslateMessage(MSG* pMsg) 
{
	if (::IsWindow(m_ToolTip.m_hWnd) && pMsg->hwnd == m_hWnd)
	{
		switch(pMsg->message)
		{
		case WM_LBUTTONDOWN:	
		case WM_MOUSEMOVE:
		case WM_LBUTTONUP:	
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:	
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
			m_ToolTip.RelayEvent(pMsg);
			break;
		}
	}
	return CTreeView::PreTranslateMessage(pMsg);
}
*/
//////////////////////////////////////////////////////////////////////////////////
/*
BOOL CLeftView::OnToolTipNeedText(UINT id, NMHDR * pNMHDR, LRESULT * pResult)
{
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
	BOOL bHandledNotify = FALSE;

	CPoint pt;
	VERIFY(::GetCursorPos(&pt));
	ScreenToClient(&pt);

	CRect rect;
	GetClientRect(rect);

	// Make certain that the cursor is in the client rect, because the
	// mainframe also wants these messages to provide tooltips for the
	// toolbar.
	if (rect.PtInRect(pt))
	{
		m_pArchivToolTip = HittestArchivToolTip(pt);

		if (m_pArchivToolTip)
		{
			// Adjust the text by filling in TOOLTIPTEXT
			CString strTip;
			CString sF;
			sF.LoadString(IDS_TOOLTIP_ARCHIV);
			strTip.Format(sF,
				m_pArchivToolTip->GetName(),
				(DWORD)(m_pArchivToolTip->BenutzterPlatz()/(1024*1024)),
				m_pArchivToolTip->SizeInMB()
				);
			if (strTip.GetLength() <= sizeof(pTTT->szText))
			{
				::strcpy(pTTT->szText, strTip);
			}
			else
			{
				pTTT->szText[0] = 0;
			}
		}
		else
		{
			pTTT->szText[0] = 0;
		}
		bHandledNotify = TRUE;
	}
	else
	{
		pTTT->szText[0] = 0;
		bHandledNotify = TRUE;
	}
	return bHandledNotify;
}
*/
////////////////////////////////////////////////////////////////////////////
/*
void CLeftView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (::IsWindow(m_ToolTip.m_hWnd))
	{
		CIPCArchivRecord* pHit = HittestArchivToolTip(point);

		if (!pHit || pHit != m_pArchivToolTip)
		{
			// Use Activate() to hide the tooltip.
			m_ToolTip.Activate(FALSE);		
		}

		if (pHit)
		{
			m_ToolTip.Activate(TRUE);
			m_pArchivToolTip = pHit;
		}
	}
	CTreeView::OnMouseMove(nFlags, point);
}
*/
////////////////////////////////////////////////////////////////////////////
/*
CIPCArchivRecord* CLeftView::HittestArchivToolTip(CPoint pt)
{
	CIPCArchivRecord* pRecord = NULL;
	
	CRect rect;
	GetClientRect(rect);

	// Make certain that the cursor is in the client rect, because the
	// mainframe also wants these messages to provide tooltips for the
	// toolbar.
	if (rect.PtInRect(pt))
	{
		HTREEITEM hItem;
		UINT nFlags;

		hItem = GetTreeCtrl().HitTest(pt,&nFlags);

		// camera click on State Image
		if (hItem && (nFlags & TVHT_ONITEM))
		{
			if (IsArchivItem(hItem))
			{
				HTREEITEM hServer = GetServerItem(hItem);
				if (hServer)
				{
					CDBCheckDoc* pDoc = GetDocument();
					CServer* pServer = pDoc->GetServer((WORD)GetTreeCtrl().GetItemData(hServer));
					if (pServer && pServer->IsConnected())
					{
						CIPCDatabaseDBCheck* pDataBase = pServer->GetDatabase();
						pDataBase->Lock();
						pRecord = pDataBase->GetArchiv((WORD)GetTreeCtrl().GetItemData(hItem));
						pDataBase->Unlock();
					}
				}
			}
		}
	}
	return pRecord;
}
*/
////////////////////////////////////////////////////////////////////////////
/*
void CLeftView::OnUpdateSequenceOpen(CCmdUI* pCmdUI) 
{
	if (m_bBackupMode)
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	CMainFrame* pMF = (CMainFrame*)theApp.m_pMainWnd;
	if (   WK_IS_WINDOW(pMF)
		&& pMF->IsSyncMode())
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	pCmdUI->Enable(IsSequenceItem(GetTreeCtrl().GetSelectedItem()));
}
*/
////////////////////////////////////////////////////////////////////////////
/*
void CLeftView::OnUpdateDelete(CCmdUI* pCmdUI) 
{
	if (m_bBackupMode)
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(IsSequenceItem(GetTreeCtrl().GetSelectedItem()));
	}
}
*/
////////////////////////////////////////////////////////////////////////////
/*
void CLeftView::OnArchivEmpty() 
{
	OnDelete();
}
*/
////////////////////////////////////////////////////////////////////////////
/*
void CLeftView::OnUpdateArchivEmpty(CCmdUI* pCmdUI) 
{
	if (m_bBackupMode)
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
		if (IsArchivItem(hItem))
		{
			pCmdUI->Enable(GetTreeCtrl().ItemHasChildren(hItem));
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
}
*/
////////////////////////////////////////////////////////////////////////////
void CLeftView::Redraw()
{
	DWORD dwTick = GetTickCount();
	if ((dwTick-m_dwLastRedraw>2000) && m_bNeedsRedraw)
	{
		m_dwLastRedraw = dwTick;
		CRect rect;
		GetTreeCtrl().GetClientRect(rect);
		GetTreeCtrl().InvalidateRect(rect);
		m_bNeedsRedraw = FALSE;
	}
}
////////////////////////////////////////////////////////////////////////////
/*
void CLeftView::OnArchivBackup() 
{
	if (!theApp.m_bAllowBackup)
	{
		return;
	}

	HTREEITEM hItem;
	HTREEITEM hParent = NULL;

	hItem = GetTreeCtrl().GetSelectedItem();
	while (hItem)
	{
		hParent = hItem;
		hItem = GetTreeCtrl().GetParentItem(hItem);
	}
	if (hParent==NULL)
	{
		m_bBackupMode = FALSE;
		return;
	}

	CDBCheckDoc* pDoc = GetDocument();
	CServer* pServer;
	pServer = pDoc->GetServer((WORD)GetTreeCtrl().GetItemData(hParent));

	if (pServer==NULL)
	{
		m_bBackupMode = FALSE;
		return;
	}
	if (pServer->GetDatabase()->GetVersion()<3)
	{
		CString sL,sM;
		sL.LoadString(IDS_NO_BACKUP_3X);
		sM.Format(sL,pServer->GetName());
		COemGuiApi::MessageBox(sM,20,MB_OK|MB_ICONSTOP);
	}

	if (m_bBackupMode)
	{
		HTREEITEM hArchiv = GetTreeCtrl().GetChildItem(hParent);
		while (hArchiv)
		{
			SetCheckRecurse(GetTreeCtrl(),hArchiv,TREE_CONTROL_CHECK_OFF);
			hArchiv = GetTreeCtrl().GetNextSiblingItem(hArchiv);
		}
		m_bBackupMode = FALSE;
		GetDocument()->UpdateMyViews(this,VDH_BACKUP,(CObject*)NULL);
	}
	else
	{
		HTREEITEM hArchiv = GetTreeCtrl().GetChildItem(hParent);
		while (hArchiv)
		{
			int i=0,is=0;
			GetTreeCtrl().GetItemImage(hArchiv,i,is);
			if (i != _IMAGE_BACKUP)
			{
				SetCheckRecurse(GetTreeCtrl(),hArchiv,TREE_CONTROL_UNCHECKED);
			}
			hArchiv = GetTreeCtrl().GetNextSiblingItem(hArchiv);
		}
		m_bBackupMode = TRUE;
		GetDocument()->UpdateMyViews(this,VDH_BACKUP,(CObject*)pServer);
	}
}

////////////////////////////////////////////////////////////////////////////
void CLeftView::OnUpdateArchivBackup(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(theApp.m_bAllowBackup); 
	pCmdUI->SetCheck(m_bBackupMode);
}
*/
////////////////////////////////////////////////////////////////////////////
/*
void CLeftView::OnArchivDelete() 
{
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();

	if (IsArchivItem(hItem))
	{
		HTREEITEM hServer = GetServerItem(hItem);
		CServer* pServer = GetServer(hServer);
		if (pServer && !pServer->IsDV())
		{
			CIPCArchivRecord* pArchive = GetArchiveRecord(hItem);
			if (pArchive && pArchive->IsBackup())
			{
				CString sM;
				sM.Format(IDP_ASK_DELETE_ARCHIV,pArchive->GetName());
				if (IDYES==AfxMessageBox(sM,MB_ICONQUESTION|MB_YESNO))
				{
					CIPCDatabaseDBCheck* pDatabase = pServer->GetDatabase();
					if (pDatabase)
					{
						pDatabase->DoRequestDeleteArchiv(pArchive->GetSubID());
					}
				}
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////
void CLeftView::OnUpdateArchivDelete(CCmdUI* pCmdUI) 
{
	if (!theApp.m_bReadOnlyModus)
	{
		HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
		CIPCArchivRecord* pArchive = GetArchiveRecord(hItem);
		if (pArchive && pArchive->IsBackup())
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}
	pCmdUI->Enable(FALSE);
}
*/
/*
////////////////////////////////////////////////////////////////////////////
void CLeftView::AddAllBackupSequencesExcept(WORD wServerID, CIPCSequenceRecord* pSequence)
{
	HTREEITEM hServer = GetServerItem(wServerID);
	HTREEITEM hArchiv = NULL;

	if (hServer)
	{
		hArchiv = GetTreeCtrl().GetChildItem(hServer);
		while (hArchiv)
		{
			if (pSequence->GetArchiveNr()==GetTreeCtrl().GetItemData(hArchiv))
			{
				// found archiv
				HTREEITEM hSequence = GetTreeCtrl().GetChildItem(hArchiv);

				while (hSequence)
				{
					WORD wSequenceNr = (WORD)GetTreeCtrl().GetItemData(hSequence);

					if (wSequenceNr!=pSequence->GetSequenceNr())
					{
						if (GetCheck(GetTreeCtrl(),hSequence))
						{
							GetDocument()->UpdateMyViews(this,
								VDH_BACKUP_ADD_SEQUENCE,
								(CObject*)GetSequenceRecord(hSequence));
						}
					}

					hSequence = GetTreeCtrl().GetNextSiblingItem(hSequence);
				}

				break;
			}
			hArchiv = GetTreeCtrl().GetNextSiblingItem(hArchiv);
		}
	}
}
*/
////////////////////////////////////////////////////////////////////////////
/*
void CLeftView::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CMainFrame* pMF = (CMainFrame*)AfxGetMainWnd();
	if (m_bBackupMode || pMF->IsSyncMode())
	{
		CPoint pt;
		GetCursorPos(&pt);
		HTREEITEM hClicked;
		UINT nFlags;

		ScreenToClient(&pt);
		hClicked = GetTreeCtrl().HitTest(pt,&nFlags);

		// TRACE("<%s>\n",GetTreeCtrl().GetItemText(hClicked));

		if (hClicked && (nFlags & TVHT_ONITEMSTATEICON))
		{
			BOOL bCheck = GetCheck(GetTreeCtrl(),hClicked);
			if (bCheck)
			{
				// uncheck it
				SetCheckRecurse(GetTreeCtrl(),hClicked,TREE_CONTROL_UNCHECKED,m_bBackupMode);
			}
			else
			{
				// check it
				SetCheckRecurse(GetTreeCtrl(),hClicked,TREE_CONTROL_CHECKED,m_bBackupMode);
			}
			bCheck = !bCheck;

			if (m_bBackupMode)
			{
				if (IsSequenceItem(hClicked))
				{
					// it's a sequence
					GetDocument()->UpdateMyViews(this,
						bCheck ? VDH_BACKUP_ADD_SEQUENCE : VDH_BACKUP_DEL_SEQUENCE,
						(CObject*)GetSequenceRecord(hClicked));
				}
				else if (IsArchivItem(hClicked))
				{
					// it's an archive
					GetDocument()->UpdateMyViews(this,
						bCheck ? VDH_BACKUP_ADD_ARCHIVE : VDH_BACKUP_DEL_ARCHIVE,
						(CObject*)GetArchiveRecord(hClicked));
				}
				else if (IsServerItem(hClicked))
				{
					// it's a server
				}
			}
			if (pMF->IsSyncMode())
			{
				if (IsArchivItem(hClicked))
				{
					WORD wArchive = (WORD)GetTreeCtrl().GetItemData(hClicked);
					WORD wServer = (WORD)GetTreeCtrl().GetItemData(GetServerItem(hClicked));
					if (bCheck)
						pMF->GetSyncCoolBar()->AddArchive(wServer,wArchive);
					else
						pMF->GetSyncCoolBar()->DeleteArchive(wServer,wArchive,TRUE);
				}
			}


		}
	}
	
	*pResult = 0;
}
 */
////////////////////////////////////////////////////////////////////////////
CServer* CLeftView::GetServer(HTREEITEM hServer)
{
	if (IsServerItem(hServer))
	{
		WORD wServerID = LOWORD(GetTreeCtrl().GetItemData(hServer));
		CDBCheckDoc* pDoc = GetDocument();
		if (pDoc)
		{
			CServer* pServer = pDoc->GetServer(wServerID);
			if (pServer && pServer->IsConnected())
			{
				return pServer;
			}
		}
	}
	return NULL;
}
////////////////////////////////////////////////////////////////////////////
CIPCSequenceRecord* CLeftView::GetSequenceRecord(HTREEITEM hSequence)
{
	CIPCSequenceRecord* pSequence = NULL;
	HTREEITEM hArchiv;
	hArchiv = GetTreeCtrl().GetParentItem(hSequence);
	if (hArchiv)
	{
		HTREEITEM hServer = GetTreeCtrl().GetParentItem(hArchiv);
		if (hServer)
		{
			WORD wServerID = LOWORD(GetTreeCtrl().GetItemData(hServer));
			WORD wArchiv   = LOWORD(GetTreeCtrl().GetItemData(hArchiv));
			WORD wSequence = LOWORD(GetTreeCtrl().GetItemData(hSequence));

			CDBCheckDoc* pDoc = GetDocument();
			if (pDoc)
			{
				CServer* pServer = pDoc->GetServer(wServerID);
				if (pServer)
				{
					pSequence = pServer->GetCIPCSequenceRecord(wArchiv,wSequence);
				}
			}
		}
	}
	return pSequence;
}
////////////////////////////////////////////////////////////////////////////
CIPCArchivRecord* CLeftView::GetArchiveRecord(HTREEITEM hArchiv)
{
	CIPCArchivRecord* pArchiv = NULL;
	HTREEITEM hServer = GetTreeCtrl().GetParentItem(hArchiv);
	if (hServer)
	{
		WORD wServerID = LOWORD(GetTreeCtrl().GetItemData(hServer));
		WORD wArchiv   = LOWORD(GetTreeCtrl().GetItemData(hArchiv));

		CDBCheckDoc* pDoc = GetDocument();
		if (pDoc)
		{
			CServer* pServer = pDoc->GetServer(wServerID);
			if (pServer)
			{
				pArchiv = pServer->GetCIPCArchivRecord(wArchiv);
			}
		}
	}
	return pArchiv;
}
////////////////////////////////////////////////////////////////////////////

//BOOL CLeftView::ShowSyncCheckBoxes(BOOL bShow /*= TRUE*/)
/*
{
	for (HTREEITEM hServer = GetTreeCtrl().GetRootItem();hServer;hServer = GetTreeCtrl().GetNextSiblingItem(hServer))
	{
		for (HTREEITEM hArchiv = GetTreeCtrl().GetChildItem(hServer);hArchiv;hArchiv = GetTreeCtrl().GetNextSiblingItem(hArchiv))
		{
			int i=0,is=0;
			GetTreeCtrl().GetItemImage(hArchiv,i,is);
			if (i == _IMAGE_RING)
			{
				SetCheckRecurse(GetTreeCtrl(),
					hArchiv,
					bShow?TREE_CONTROL_UNCHECKED:TREE_CONTROL_CHECK_OFF,
					FALSE);
				if (bShow)
				{
					GetTreeCtrl().Expand(hArchiv,TVE_COLLAPSE);
				}
			}
		}
	}

	return TRUE;
}
*/
void CLeftView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// Set focus to control if key strokes are needed.
	// Focus is not automatically given to control on lbuttondown

	if(nFlags & MK_CONTROL ) 
	{
		// Control key is down
		UINT flag;
		HTREEITEM hItem = GetTreeCtrl().HitTest( point, &flag );
		if( hItem )
		{
			// Toggle selection state
			UINT uNewSelState = 
				GetTreeCtrl().GetItemState(hItem, TVIS_SELECTED) & TVIS_SELECTED ? 
							0 : TVIS_SELECTED;
            
			// Get old selected (focus) item and state
			HTREEITEM hItemOld = GetTreeCtrl().GetSelectedItem();
			UINT uOldSelState  = hItemOld ? 
					GetTreeCtrl().GetItemState(hItemOld, TVIS_SELECTED) : 0;
            
			// Select new item
			if( GetTreeCtrl().GetSelectedItem() == hItem )
				GetTreeCtrl().SelectItem( NULL );		// to prevent edit
			CTreeView::OnLButtonDown(nFlags, point);

			// Set proper selection (highlight) state for new item
			GetTreeCtrl().SetItemState(hItem, uNewSelState,  TVIS_SELECTED);

			// Restore state of old selected item
			if (hItemOld && hItemOld != hItem)
				GetTreeCtrl().SetItemState(hItemOld, uOldSelState, TVIS_SELECTED);

			m_hItemFirstSel = NULL;

			return;
		}
	}
/*   
	else if(nFlags & MK_SHIFT)
	{
		// Shift key is down
		UINT flag;
		HTREEITEM hItem = GetTreeCtrl().HitTest( point, &flag );

		// Initialize the reference item if this is the first shift selection
		if( !m_hItemFirstSel )
			m_hItemFirstSel = GetTreeCtrl().GetSelectedItem();

		// Select new item
		if( GetTreeCtrl().GetSelectedItem() == hItem )
			GetTreeCtrl().SelectItem( NULL );			// to prevent edit
		CTreeView::OnLButtonDown(nFlags, point);

		if( m_hItemFirstSel )
		{
			SelectItems( m_hItemFirstSel, hItem );
			return;
		}
	}
	else
	{
		// Normal - remove all selection and let default 
		// handler do the rest
		ClearSelection();
		m_hItemFirstSel = NULL;
	}
*/
   CTreeView::OnLButtonDown(nFlags, point);
}
///////////////////////////////////////////////////////////////////////
/*
BOOL CLeftView::SelectItems(HTREEITEM hItemFrom, HTREEITEM hItemTo)
{
	HTREEITEM hItem = GetTreeCtrl().GetRootItem();

	// Clear selection upto the first item
	while ( hItem && hItem!=hItemFrom && hItem!=hItemTo )
	{
		hItem = GetTreeCtrl().GetNextVisibleItem( hItem );
		GetTreeCtrl().SetItemState( hItem, 0, TVIS_SELECTED );
	}

	if ( !hItem )
		return FALSE;	// Item is not visible

	GetTreeCtrl().SelectItem( hItemTo );

	// Rearrange hItemFrom and hItemTo so that hItemFirst is at top
	if( hItem == hItemTo )
	{
		hItemTo = hItemFrom;
		hItemFrom = hItem;
	}


	// Go through remaining visible items
	BOOL bSelect = TRUE;
	while ( hItem )
	{
		// Select or remove selection depending on whether item
		// is still within the range.
		GetTreeCtrl().SetItemState( hItem, bSelect ? TVIS_SELECTED : 0, TVIS_SELECTED );

		// Do we need to start removing items from selection
		if( hItem == hItemTo ) 
			bSelect = FALSE;

		hItem = GetTreeCtrl().GetNextVisibleItem( hItem );
	}

	return TRUE;
}
*/
///////////////////////////////////////////////////////////////////////
/*
HTREEITEM CLeftView::GetFirstSelectedItem()
{
	for ( HTREEITEM hItem = GetTreeCtrl().GetRootItem(); 
		  hItem!=NULL; 
		  hItem = GetTreeCtrl().GetNextItem( hItem ,TVGN_NEXTVISIBLE) )
		if ( GetTreeCtrl().GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )
			return hItem;

	return NULL;
}
///////////////////////////////////////////////////////////////////////
HTREEITEM CLeftView::GetNextSelectedItem( HTREEITEM hItem )
{
	for ( hItem = GetTreeCtrl().GetNextItem( hItem ,TVGN_NEXTVISIBLE); 
		  hItem!=NULL; 
		  hItem = GetTreeCtrl().GetNextItem( hItem ,TVGN_NEXTVISIBLE) )
		if ( GetTreeCtrl().GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )
			return hItem;

	return NULL;
}
///////////////////////////////////////////////////////////////////////
HTREEITEM CLeftView::GetPrevSelectedItem( HTREEITEM hItem )
{
	for ( hItem = GetTreeCtrl().GetNextItem( hItem,TVGN_PREVIOUSVISIBLE); 
	      hItem!=NULL; 
		  hItem = GetTreeCtrl().GetNextItem( hItem,TVGN_PREVIOUSVISIBLE) )
		if ( GetTreeCtrl().GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )
			return hItem;

	return NULL;
}

///////////////////////////////////////////////////////////////////////
void CLeftView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if ( (nChar==VK_UP || nChar==VK_DOWN) && GetKeyState( VK_SHIFT )&0x8000)
	{
		// Initialize the reference item if this is the first shift selection
		if( !m_hItemFirstSel )
		{
			m_hItemFirstSel = GetTreeCtrl().GetSelectedItem();
			ClearSelection();
		}

		// Find which item is currently selected
		HTREEITEM hItemPrevSel = GetTreeCtrl().GetSelectedItem();

		HTREEITEM hItemNext;
		if ( nChar==VK_UP )
			hItemNext = GetTreeCtrl().GetPrevVisibleItem( hItemPrevSel );
		else
			hItemNext = GetTreeCtrl().GetNextVisibleItem( hItemPrevSel );

		if ( hItemNext )
		{
			// Determine if we need to reselect previously selected item
			BOOL bReselect = 
				!( GetTreeCtrl().GetItemState( hItemNext, TVIS_SELECTED ) & TVIS_SELECTED );

			// Select the next item - this will also deselect the previous item
			GetTreeCtrl().SelectItem( hItemNext );

			// Reselect the previously selected item
			if ( bReselect )
				GetTreeCtrl().SetItemState( hItemPrevSel, TVIS_SELECTED, TVIS_SELECTED );
		}
		return;
	}
	else if( nChar >= VK_SPACE )
	{
		m_hItemFirstSel = NULL;
		ClearSelection();
	}
	CTreeView::OnKeyDown(nChar, nRepCnt, nFlags);
}
/////////////////////////////////////////////////////////////////////////////////////
void CLeftView::ClearSelection()
{
	// This can be time consuming for very large trees 
	// and is called every time the user does a normal selection
	// If performance is an issue, it may be better to maintain 
	// a list of selected items
	for ( HTREEITEM hItem=GetTreeCtrl().GetRootItem(); hItem!=NULL; hItem=GetTreeCtrl().GetNextItem( hItem ,TVGN_NEXTVISIBLE) )
		if ( GetTreeCtrl().GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )
			GetTreeCtrl().SetItemState( hItem, 0, TVIS_SELECTED );
}
*/

/////////////////////////////////////////////////////////////////////////////
BOOL GetCheck(CTreeCtrl& ctrl, HTREEITEM hItem)
{
	TVITEM tvItem;

	tvItem.mask = TVIF_HANDLE | TVIF_STATE;
	tvItem.hItem = hItem;
	tvItem.stateMask = TVIS_STATEIMAGEMASK;

	ctrl.GetItem(&tvItem);

	int i = tvItem.state>>12;

	if (i==2)
	{
		return FALSE;
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL SetCheck(CTreeCtrl& ctrl, HTREEITEM hItem,BOOL bFlag,BOOL bDisable /* = FALSE */)
{
	TVITEM tvItem;

	tvItem.mask = TVIF_HANDLE | TVIF_STATE;
	tvItem.hItem = hItem;
	tvItem.stateMask = TVIS_STATEIMAGEMASK;

	/*
	Since state images are one-based, 1 in this macro turns the check off, and 
	2 turns it on. 
	If bDisable == TRUE, the check is turned on but not editable
	*/
	if(bDisable)
	{
		tvItem.state = INDEXTOSTATEIMAGEMASK(3);
	}
	else
	{
		tvItem.state = INDEXTOSTATEIMAGEMASK((bFlag ? 1 : 2));
	}


	return ctrl.SetItem(&tvItem);
}
/////////////////////////////////////////////////////////////////////////////
void SetCheckRecurse(CTreeCtrl& ctrl, HTREEITEM hParent, int iState, BOOL bRecurse/*=TRUE*/)
{

	TVITEM tvItem;

	tvItem.mask = TVIF_HANDLE | TVIF_STATE;
	tvItem.stateMask = TVIS_STATEIMAGEMASK;
	tvItem.state = INDEXTOSTATEIMAGEMASK(iState);

	tvItem.hItem = hParent;
	ctrl.SetItem(&tvItem);

	if (bRecurse)
	{
		HTREEITEM hChild = ctrl.GetChildItem(hParent);
		while (hChild)
		{
			tvItem.hItem = hChild;
			ctrl.SetItem(&tvItem);
			SetCheckRecurse(ctrl,hChild,iState,bRecurse);
			hChild = ctrl.GetNextSiblingItem(hChild);
		}
	}

}

