/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ObjectView.cpp $
// ARCHIVE:		$Archive: /Project/Clients/Recherche/ObjectView.cpp $
// CHECKIN:		$Date: 25.02.04 14:25 $
// VERSION:		$Revision: 107 $
// LAST CHANGE:	$Modtime: 25.02.04 14:07 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$

#include "stdafx.h"
#include "Recherche.h"

#include "MainFrm.h"
#include "RechercheDoc.h"
#include "ObjectView.h"
#include "RechercheView.h"

#include "images.h"
#include "BackupDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectView

#define NODE_SERVERID	(WORD)1
#define NODE_ARCHIVEID	(WORD)2
#define NODE_DATEID		(WORD)3
#define NODE_SEQUENCEID	(WORD)4

IMPLEMENT_DYNCREATE(CObjectView, CTreeView)

CObjectView::CObjectView()
{
	m_hItemFirstSel			= NULL;
	m_pQueryDialog			= NULL;
	m_pSearchResultDialog	= NULL;
	m_pArchivToolTip		= NULL;
	m_dwLastRedraw			= 0;
	m_bNeedsRedraw			= FALSE;
	m_bBackupMode			= FALSE;
}

CObjectView::~CObjectView()
{
}


BEGIN_MESSAGE_MAP(CObjectView, CTreeView)
	//{{AFX_MSG_MAP(CObjectView)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_SEQUENCE_OPEN, OnSequenceOpen)
	ON_COMMAND(ID_DELETE, OnDelete)
	ON_COMMAND(ID_DATABASE_INFO, OnDatabaseInfo)
	ON_COMMAND(ID_SEARCH, OnSearch)
	ON_UPDATE_COMMAND_UI(ID_SEARCH, OnUpdateSearch)
	ON_WM_MOUSEMOVE()
	ON_UPDATE_COMMAND_UI(ID_SEQUENCE_OPEN, OnUpdateSequenceOpen)
	ON_UPDATE_COMMAND_UI(ID_DELETE, OnUpdateDelete)
	ON_COMMAND(ID_ARCHIV_EMPTY, OnArchivEmpty)
	ON_UPDATE_COMMAND_UI(ID_ARCHIV_EMPTY, OnUpdateArchivEmpty)
	ON_UPDATE_COMMAND_UI(ID_ARCHIV_BACKUP, OnUpdateArchivBackup)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_COMMAND(ID_ARCHIV_DELETE, OnArchivDelete)
	ON_UPDATE_COMMAND_UI(ID_ARCHIV_DELETE, OnUpdateArchivDelete)
	ON_COMMAND(ID_SEQUENCE_OPEN_SINGLE, OnSequenceOpenSingle)
	ON_COMMAND(ID_DATE_SEQUENCES_DELETE, OnDateSequencesDelete)
	ON_UPDATE_COMMAND_UI(ID_DATE_SEQUENCES_DELETE, OnUpdateDateSequencesDelete)
	ON_UPDATE_COMMAND_UI(ID_SEQUENCE_OPEN_SINGLE, OnUpdateSequenceOpenSingle)
	ON_WM_TIMER()
	ON_COMMAND(ID_DISCONNECT, OnDisconnect)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER,OnUser)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNeedText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectView drawing

/////////////////////////////////////////////////////////////////////////////
// CObjectView diagnostics

#ifdef _DEBUG
void CObjectView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CObjectView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
CRechercheDoc* CObjectView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRechercheDoc)));
	return (CRechercheDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CObjectView message handlers
/////////////////////////////////////////////////////////////////////////////
void CObjectView::DisconnectID(UINT nID) 
{
	CTreeCtrl &theCtrl = GetTreeCtrl();
	WORD wServerID = LOWORD(nID);
	WORD wData;
	HTREEITEM hItem;

	hItem = theCtrl.GetRootItem();
	while (hItem)
	{
		wData = LOWORD(theCtrl.GetItemData(hItem));
		if (wData == wServerID)
		{
			break;
		}
		hItem = theCtrl.GetNextSiblingItem(hItem);
	}

	if (hItem)
	{
		CRechercheDoc* pDoc = GetDocument();
		wServerID = LOWORD(theCtrl.GetItemData(hItem));
		pDoc->Disconnect(wServerID);
		DisconnectDialogs(wServerID);
	}
	AfxGetMainWnd()->PostMessage(WM_USER);
}
/////////////////////////////////////////////////////////////////////////////
void CObjectView::PopupDisconnectMenu(CPoint pt)
{
	CTreeCtrl &theCtrl = GetTreeCtrl();
	CMenu menu;
	HTREEITEM hItem;
	DWORD dwID;
	CString sItemText,sText;
	int i = 1;

	menu.CreatePopupMenu();
	hItem = theCtrl.GetRootItem();
	if (hItem)
	{
		while (hItem)
		{
			dwID = theCtrl.GetItemData(hItem);
			sItemText = theCtrl.GetItemText(hItem);
			sText.Format(_T("&%d %s"),i++,sItemText);
			menu.AppendMenu(MF_STRING,
							dwID+ID_DISCONNECT_LOW,
							sText);
			hItem = theCtrl.GetNextSiblingItem(hItem);
		}
		menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, AfxGetMainWnd());
	}
	else
	{
		AfxGetMainWnd()->PostMessage(WM_USER);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CObjectView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	WORD cause	  = LOWORD(lHint);
	WORD wServerID = HIWORD(lHint);
	CSecID id;

	switch (cause)
	{
	case VDH_DATABASE:
		InsertDatabase((CIPCDatabaseRecherche*)pHint);
		break;
	case VDH_REMOVE_SERVER:
		DeleteServer(LOWORD(pHint));
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
	case VDH_BACKUP:
		if (m_bBackupMode)
		{
			OnArchivBackup();
		}
		break;
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
	}
}
/////////////////////////////////////////////////////////////////////////////
void CObjectView::InsertDatabase(CIPCDatabaseRecherche* pDatabase)
{
	CTreeCtrl &theCtrl = GetTreeCtrl();
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
			InsertArchiv(theCtrl,hServer,pRecord);
		}
		pDatabase->Unlock();
		theCtrl.Expand(hServer,TVE_EXPAND);
		if (WK_IS_WINDOW(m_pQueryDialog))
		{
			m_pQueryDialog->InitServerArchives();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CObjectView::InsertArchiv(CTreeCtrl& ctrl,HTREEITEM hServer, CIPCArchivRecord* pRecord)
{
	HTREEITEM hArchiv = NULL;
	TV_INSERTSTRUCT tvis;
	CString s = pRecord->GetName();

	tvis.hParent = hServer;
	tvis.hInsertAfter = TVI_SORT;
	tvis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

	tvis.item.lParam	 = MAKELONG(pRecord->GetSubID(), NODE_ARCHIVEID);
	tvis.item.cchTextMax = s.GetLength();
	tvis.item.pszText	 = s.GetBuffer(0);

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
		WK_TRACE_ERROR(_T("unknown archive type %s %x\n"),pRecord->GetName(),pRecord->GetFlags());
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
			if (((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode())
			{
				SetCheckRecurse(GetTreeCtrl(),hArchiv,TREE_CONTROL_UNCHECKED,FALSE);
			}
		}
	}
	s.ReleaseBuffer();

	return hArchiv;
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CObjectView::GetServerItem(WORD wServer)
{
	CTreeCtrl &theCtrl = GetTreeCtrl();
	HTREEITEM hServer;

	hServer = theCtrl.GetRootItem();
	while (hServer)
	{
		if (wServer==LOWORD(theCtrl.GetItemData(hServer)))
		{
			break;
		}
		// next server, if any
		hServer = theCtrl.GetNextSiblingItem(hServer);
	}
	return hServer;
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CObjectView::GetArchivItem(WORD wServer, WORD wArchivNr)
{
	CTreeCtrl &theCtrl = GetTreeCtrl();
	HTREEITEM hServer;
	HTREEITEM hArchive;

	hServer = theCtrl.GetRootItem();
	while (hServer)
	{
		if (wServer==LOWORD(theCtrl.GetItemData(hServer)))
		{
			// bingo found server
			// searching archiv
			hArchive = theCtrl.GetChildItem(hServer);
			while (hArchive)
			{
				if (wArchivNr==LOWORD(theCtrl.GetItemData(hArchive)))
				{
					// found archiv
					return hArchive;
				}
				hArchive = theCtrl.GetNextSiblingItem(hArchive);
			}
		}
		// next server, if any
		hServer = theCtrl.GetNextSiblingItem(hServer);
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CObjectView::GetServerItem(HTREEITEM hItem)
{
	CTreeCtrl &theCtrl = GetTreeCtrl();
	switch (HIWORD(theCtrl.GetItemData(hItem)))
	{
		case NODE_SERVERID:	 return hItem;
		case NODE_ARCHIVEID: return theCtrl.GetParentItem(hItem);
		case NODE_DATEID:	 return theCtrl.GetParentItem(theCtrl.GetParentItem(hItem));
		case NODE_SEQUENCEID:return theCtrl.GetParentItem(theCtrl.GetParentItem(theCtrl.GetParentItem(hItem)));
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CObjectView::IsServerItem(HTREEITEM hItem)
{
	return (hItem && HIWORD(GetTreeCtrl().GetItemData(hItem))==NODE_SERVERID) ? TRUE : FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CObjectView::IsArchivItem(HTREEITEM hItem)
{
	return (hItem && HIWORD(GetTreeCtrl().GetItemData(hItem))==NODE_ARCHIVEID) ? TRUE : FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CObjectView::IsDateItem(HTREEITEM hItem)
{
	return (hItem && HIWORD(GetTreeCtrl().GetItemData(hItem))==NODE_DATEID) ? TRUE : FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CObjectView::IsSequenceItem(HTREEITEM hItem)
{
	return (hItem && HIWORD(GetTreeCtrl().GetItemData(hItem))==NODE_SEQUENCEID) ? TRUE : FALSE;
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CObjectView::InsertServer(const CServer* pServer)
{
	TV_INSERTSTRUCT tvis;
	HTREEITEM hServer = NULL;
	CString	s		  = pServer->GetFullName();
	WORD	wServerID = pServer->GetID();

	tvis.hParent = TVI_ROOT;
	tvis.hInsertAfter = pServer->IsLocal() ? TVI_FIRST : TVI_LAST;

	tvis.item.mask= TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvis.item.lParam         = MAKELONG(wServerID, NODE_SERVERID);
	
	tvis.item.cchTextMax     = s.GetLength();
	tvis.item.pszText        = (LPTSTR)(LPCTSTR)s; // only reading!

	tvis.item.iImage         = _IMAGE_HOSTBASE + ((wServerID-1) % _IMAGE_NRHOST);
	tvis.item.iSelectedImage = tvis.item.iImage;

	hServer = GetTreeCtrl().InsertItem(&tvis);

	GetTreeCtrl().SelectItem(hServer);

	return hServer;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CObjectView::DeleteServer(WORD wID)
{
	HTREEITEM hItem;

	DisconnectDialogs(wID);
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
void CObjectView::DisconnectDialogs(WORD wServerID)
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
/////////////////////////////////////////////////////////////////////////////
void CObjectView::OnInitialUpdate() 
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
BOOL CObjectView::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style = WS_CHILD | WS_VISIBLE | WS_BORDER | 
			   TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS;
	return CTreeView::PreCreateWindow(cs);
}
/////////////////////////////////////////////////////////////////////////////
void CObjectView::OnDisconnect() 
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
		CRechercheDoc* pDoc = GetDocument();
		WORD wID = LOWORD(GetTreeCtrl().GetItemData(hParent));
		pDoc->Disconnect(wID);
		DisconnectDialogs(wID);
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CObjectView::OnUser(WPARAM wParam, LPARAM lParam) 
{
	WORD cause	   = LOWORD(wParam);
	WORD wServerID = HIWORD(wParam);

	switch (cause)
	{
		case VDH_DEL_ARCHIV:
			RemoveArchiv(wServerID,LOWORD(lParam));
			GetDocument()->UpdateMyViews(this,wParam,(CObject*)lParam);
			break;
		case VDH_SELECT_SEQUENCE:
		{
			CTreeCtrl &theCtrl = GetTreeCtrl();
			WORD wArchiveID  = LOWORD(lParam);
			WORD wSequenceID = HIWORD(lParam);
			HTREEITEM hTemp, hItem = theCtrl.GetSelectedItem();
			hItem = FindServerItem(wServerID, hItem);
			if (hItem)
			{
				hItem = FindItemByID(wArchiveID, hItem);
				if (hItem)
				{
					ASSERT(IsArchivItem(hItem));
					hTemp = hItem = theCtrl.GetNextItem(hItem, TVGN_CHILD);
					while (hItem)
					{
						ASSERT(IsDateItem(hItem));
						hTemp = FindItemByID(wSequenceID, hItem);
						if (hTemp)
						{
							ASSERT(IsSequenceItem(hTemp));
							theCtrl.SelectItem(hTemp);
						}
						hItem = theCtrl.GetNextItem(hItem, TVGN_NEXT);
					}
				}
			}
		}	break;
/*
		case VDH_SELECT_SEQUENCE:
		{
			CTreeCtrl &theCtrl = GetTreeCtrl();
			WORD wArchiveID  = LOWORD(lParam);
			WORD wSequenceID = HIWORD(lParam);
			HTREEITEM hTemp, hItem = theCtrl.GetSelectedItem();
			hItem = FindServerItem(wServerID, hItem);
			if (hItem)
			{
				theCtrl.SelectItem(hItem);
				hItem = FindItemByID(wArchiveID, hItem);
				if (hItem)
				{
					theCtrl.SelectItem(hItem);
					ASSERT(IsArchivItem(hItem));
					hTemp = hItem = theCtrl.GetNextItem(hItem, TVGN_CHILD);
					while (hItem)
					{
						ASSERT(IsDateItem(hItem));
						theCtrl.SelectItem(hItem);
						if (LOWORD(theCtrl.GetItemData(hItem)) > wSequenceID)
						{
							hItem = FindItemByID(wSequenceID, hTemp);
							if (hItem)
							{
								ASSERT(IsSequenceItem(hItem));
								theCtrl.SelectItem(hItem);
								hTemp = NULL;
								break;
							}
						}
						hTemp = hItem;
						hItem = theCtrl.GetNextItem(hItem, TVGN_NEXT);
					}
					if (hTemp)
					{
						hItem = FindItemByID(wSequenceID, hTemp);
						if (hItem)
						{
							ASSERT(IsSequenceItem(hItem));
							theCtrl.SelectItem(hItem);
						}
					}
				}
			}
		}	break;
*/
	}
	return 0L;
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CObjectView::FindServerItem(WORD wServerID, HTREEITEM hSelected)
{
	CTreeCtrl &theCtrl = GetTreeCtrl();
	HTREEITEM hServer = NULL;
	if (hSelected)
	{
		hServer = GetServerItem(hSelected);
		if (LOWORD(theCtrl.GetItemData(hServer)) == wServerID)
		{
			return hServer;
		}
	}
	
	for (hServer = theCtrl.GetRootItem(); hServer!=NULL; hServer = theCtrl.GetNextSiblingItem(hServer))
	{
		if (LOWORD(theCtrl.GetItemData(hServer)) == wServerID)
		{
			return hServer;
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CObjectView::FindItemByID(WORD wID, HTREEITEM hParent)
{
	CTreeCtrl &theCtrl = GetTreeCtrl();
	ASSERT(hParent != NULL);
	HTREEITEM hItem = NULL;
	for (hItem = theCtrl.GetNextItem(hParent, TVGN_CHILD); hItem!=NULL; hItem = theCtrl.GetNextItem(hItem, TVGN_NEXT))
	{
		if (LOWORD(theCtrl.GetItemData(hItem)) == wID)
		{
			return hItem;
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CObjectView::InsertArchivFile(WORD wServerID,CIPCArchivFileRecord* pFile)
{
	HTREEITEM hArchiv;
	hArchiv = GetArchivItem(wServerID,pFile->GetArchivNr());
	if (hArchiv)
	{
		CTreeCtrl &theCtrl = GetTreeCtrl();
		BOOL bHasChildren = theCtrl.ItemHasChildren(hArchiv);
		HTREEITEM hArchivFile = NULL;
		HTREEITEM hItem       = NULL;
		CTime     t           = pFile->GetFirstTime();
		TV_INSERTSTRUCT tvis;
		CString   sTime, sDate;

		m_bNeedsRedraw = TRUE;//!bHasChildren;
		sTime.Format(_T("%02d:%02d:%02d"), t.GetHour(), t.GetMinute(), t.GetSecond());
		sDate.Format(_T("%02d.%02d.%02d"), t.GetDay() , t.GetMonth() , t.GetYear()%100);
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

		tvis.hInsertAfter    = TVI_LAST;
		tvis.item.mask       = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

		int i = 3;
		theCtrl.GetItemImage(hArchiv,i,i);
		if (pFile->WasSafeRing())
		{
			i = _IMAGE_VORALARM;
		}

		if (hItem == NULL)
		{
			if (i == -1)
			{
				i = 0;
			}
			tvis.hParent = hArchiv;
			tvis.item.iImage = i;
			tvis.item.iSelectedImage = i;
			tvis.item.cchTextMax     = sDate.GetLength();
			tvis.item.pszText        = (LPTSTR)(LPCTSTR)sDate; // only reading!
			tvis.item.lParam         = MAKELONG(pFile->GetDirNr(), NODE_DATEID);
			tvis.hParent             = theCtrl.InsertItem(&tvis);
		}
		else
		{
			tvis.hParent = hItem;
		}

		tvis.item.lParam     = MAKELONG(pFile->GetDirNr(), NODE_SEQUENCEID);
		tvis.item.cchTextMax = sTime.GetLength();
		tvis.item.pszText    = (LPTSTR)(LPCTSTR)sTime; // only reading!

		if (i!=-1)
		{
			tvis.item.iImage = i;
			tvis.item.iSelectedImage = i;
			hArchivFile = theCtrl.InsertItem(&tvis);
		}

		if (WK_IS_WINDOW(m_pSearchResultDialog) && (pFile->GetArchivNr()==255))
		{
			// ist ein Suchergebnis, bitte oeffnen
			CRechercheDoc* pDoc = GetDocument();
			if (pDoc)
			{
				CServer* pServer = pDoc->GetServer(wServerID);
				if (pServer)
				{
					pServer->OpenSequence(pFile->GetArchivNr(),pFile->GetDirNr());
				}
			}
		}
	}
	//Redraw();
}
/////////////////////////////////////////////////////////////////////////////
void CObjectView::InsertSequence(WORD wServerID,CIPCSequenceRecord* pSequence)
{
	HTREEITEM hArchiv;
	hArchiv = GetArchivItem(wServerID,pSequence->GetArchiveNr());
	if (hArchiv)
	{
		int iImage = _IMAGE_INVALID;
		if (pSequence->IsSafeRing())
		{
			iImage = _IMAGE_VORALARM;
		}
		else if (pSequence->IsAlarm())
		{
			iImage = _IMAGE_ALARM;
		}
		else if (pSequence->IsRing())
		{
			iImage = _IMAGE_RING;
		}
		else if (pSequence->IsBackup())
		{
			iImage = _IMAGE_BACKUP;
		}
		else if (pSequence->IsAlarmList())
		{
			iImage = _IMAGE_ALARMLIST;
		}
		else
		{
			WK_TRACE(_T("unknown sequence type %x %s ar=%d\n"),
				pSequence->GetFlags(),pSequence->GetName(),pSequence->GetArchiveNr());
		}
		if (pSequence->IsSearch())
		{
			iImage = _IMAGE_SEARCH;
		}
		if (pSequence->IsSuspect())
		{
			iImage = _IMAGE_SUSPECT;
		}

		// Without image no insert!
		if (iImage != _IMAGE_INVALID)
		{
			CTreeCtrl &theCtrl = GetTreeCtrl();
			// nur neu zeichnen, wenn da nicht
			// schon childs waren
			BOOL bHasChildren = theCtrl.ItemHasChildren(hArchiv);
			m_bNeedsRedraw = TRUE;//!bHasChildren;

			CString sName = pSequence->GetName();
			// if sequence has no name, take the time (of the first image) as name
			CSystemTime tTime = pSequence->GetTime();
			if (sName.IsEmpty())
			{
				sName = tTime.GetTime();
			}

			TV_INSERTSTRUCT tvis;
			tvis.hInsertAfter = TVI_LAST;
			tvis.item.mask    = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

			// Split sequences
			// Normal sequences by date of first image
			// Search sequences by search name
			CString sSplitItem;
// gf todo not yet implemented,
// search sequences lost their names, so it will always be the time... that is senseless
/*			CIPCArchivRecord* pArchiveReord = GetArchiveRecord(hArchiv);
			if (   pArchiveReord
				&& pArchiveReord->IsSearch()
				)
			{
				sSplitItem = sName;
			}
			else
*/			{
				sSplitItem = tTime.GetDate();
			}
			// Does the split item already exist?
			CString sDate = tTime.GetDate();
			HTREEITEM hSplitItem = NULL;
			if (bHasChildren)
			{
				hSplitItem = theCtrl.GetNextItem(hArchiv, TVGN_CHILD);
				while (hSplitItem)
				{
					if (sSplitItem == theCtrl.GetItemText(hSplitItem))
					{
						break;
					}
					hSplitItem = theCtrl.GetNextItem(hSplitItem, TVGN_NEXT);
				}
			}
			if (hSplitItem == NULL)
			{
				// insert split item first
				tvis.hParent			 = hArchiv;
				// take image from sequence
				// but Alarm always alarm image even if pre alarm sequence
				int iArchiveImage;
				theCtrl.GetItemImage(hArchiv, iArchiveImage, iArchiveImage);
				if (iArchiveImage == _IMAGE_ALARM)
				{
					tvis.item.iImage     = iArchiveImage;
				}
				else
				{
					tvis.item.iImage     = iImage;
				}
				tvis.item.iSelectedImage = tvis.item.iImage;
				tvis.item.cchTextMax     = sSplitItem.GetLength();
				tvis.item.pszText        = (LPTSTR)(LPCTSTR)sSplitItem; // only reading!
				tvis.item.lParam         = MAKELONG(pSequence->GetSequenceNr(), NODE_DATEID);
				// Insert the date and take item as parent for sequence
				tvis.hParent             = theCtrl.InsertItem(&tvis);
			}
			else
			{
				tvis.hParent = hSplitItem;
			}

			// now insert the sequence itself
#ifdef _DEBUG
			sName.Format(_T("%s Nr:%04d, %s"),sName,
										  pSequence->GetSequenceNr(),
										  pSequence->GetSize().Format(TRUE));
#endif
			tvis.item.cchTextMax     = sName.GetLength();
			tvis.item.pszText        = (LPTSTR)(LPCTSTR)sName; // only reading!
			tvis.item.lParam         = MAKELONG(pSequence->GetSequenceNr(), NODE_SEQUENCEID);
			tvis.item.iImage         = iImage;
			tvis.item.iSelectedImage = iImage;
			tvis.hInsertAfter = TVI_SORT;

			HTREEITEM hArchivFile = NULL;
			hArchivFile = GetTreeCtrl().InsertItem(&tvis);
		}
		else
		{	// no image fond, sequence not inserted
			ASSERT(iImage != _IMAGE_INVALID);
		}

		// gf todo?
		// shall the sequence really be opened, even if not visible in tree?
		if (   WK_IS_WINDOW(m_pSearchResultDialog) 
			&& (pSequence->IsSearch())//GetArchiveNr()==255)
			&& theApp.GetQueryParameter().ShowPictures()
			&& theApp.GetQueryParameter().CopyPictures())
		{
			// ist ein Suchergebnis, bitte oeffnen
			CRechercheDoc* pDoc = GetDocument();
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
	}
	else
	{	// no archive item found, ignored
		ASSERT(hArchiv != NULL);
	}
	//Redraw();
}
/////////////////////////////////////////////////////////////////////////////
void CObjectView::RemoveSequence(WORD wServerID, WORD wArchivNr, WORD wSequenceNr)
{
	HTREEITEM hArchiv;
	hArchiv = GetArchivItem(wServerID,wArchivNr);
	if (hArchiv)
	{
		CTreeCtrl &theCtrl = GetTreeCtrl();
		HTREEITEM hDate = theCtrl.GetChildItem(hArchiv);
		while (hDate)
		{
			HTREEITEM hSequence = theCtrl.GetChildItem(hDate);
			WORD wDirNr;
			// bestehende Tree Items pruefen
			while (hSequence)
			{
				wDirNr = LOWORD(theCtrl.GetItemData(hSequence));
				if (wDirNr==wSequenceNr)
				{
					theCtrl.DeleteItem(hSequence);
					// check if it was the last sequence of this date, then delete date item also
					if (theCtrl.GetChildItem(hDate) == NULL)
					{
						theCtrl.DeleteItem(hDate);
						hDate = NULL;
					}
					hDate = NULL;
					m_bNeedsRedraw = TRUE;
					break;
				}
				hSequence = theCtrl.GetNextSiblingItem(hSequence);
			}
			if (hDate)
			{
				hDate = theCtrl.GetNextSiblingItem(hDate);
			}
		}
	}
	//Redraw();
}
/////////////////////////////////////////////////////////////////////////////
void CObjectView::RemoveArchiv(WORD wServerID, WORD wArchivNr)
{
	HTREEITEM hArchiv;
	hArchiv = GetArchivItem(wServerID,wArchivNr);
	if (hArchiv)
	{
		GetTreeCtrl().DeleteItem(hArchiv);
		m_bNeedsRedraw = TRUE;
	}
	//Redraw();
}
/////////////////////////////////////////////////////////////////////////////
void CObjectView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_bBackupMode)
	{
		return;
	}

	CMainFrame* pMF = (CMainFrame*)theApp.m_pMainWnd;
	if (WK_IS_WINDOW(pMF) && pMF->IsSyncMode())
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
		HTREEITEM hArchiv = NULL;
		if (IsArchivItem(hItem))
		{
			// do nothing, just expand/close the node
		}
		else if (IsDateItem(hItem))
		{
			// do nothing, just expand/close the node
			// TODO: Öffnen ?
//			hArchiv = theCtrl.GetParentItem(hItem);
		}
		else if (IsSequenceItem(hItem))
		{
			// open this sequence
			hArchiv = theCtrl.GetParentItem(theCtrl.GetParentItem(hItem));
		}
		else
		{
			return;
		}
		if (hArchiv)
		{
			HTREEITEM hServer = theCtrl.GetParentItem(hArchiv);
			if (hServer)
			{
				WORD wServerID = LOWORD(theCtrl.GetItemData(hServer));
				WORD wArchiv   = LOWORD(theCtrl.GetItemData(hArchiv));
				WORD wSequence = LOWORD(theCtrl.GetItemData(hItem));

				CRechercheDoc* pDoc = GetDocument();
				if (pDoc)
				{
					CServer* pServer = pDoc->GetServer(wServerID);
					if (pServer)
					{
						pServer->OpenSequence(wArchiv,wSequence);
					}
				}
			}
		}
	}
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CObjectView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint pt;
	GetCursorPos(&pt);
	HTREEITEM hItem;
	UINT nFlags;
	CTreeCtrl &theCtrl = GetTreeCtrl();

	ScreenToClient(&pt);
	hItem = theCtrl.HitTest(pt,&nFlags);

	// right click on image
	if (hItem && (nFlags & TVHT_ONITEM))
	{
		theCtrl.SelectItem( hItem );
		ClientToScreen(&pt);
		ContextMenu(pt,hItem);
	}
	
	*pResult = 0;
}
///////////////////////////////////////////////////////////////////////
void CObjectView::ContextMenu(const CPoint& pt, HTREEITEM hItem)
{
	CMenu menu;
	CMenu* pPopup = NULL;
	UINT iMenuItemID = 0;
	CTreeCtrl &theCtrl = GetTreeCtrl();

	menu.LoadMenu(IDR_CONTEXT);

	if (IsServerItem(hItem))
	{
		pPopup = menu.GetSubMenu(0);
		theCtrl.SelectItem(hItem);

		CWK_Dongle dongle;
		if (!dongle.RunCDRWriter())
		{
			pPopup->DeleteMenu(ID_ARCHIV_BACKUP,MF_BYCOMMAND);
		}
	}
	else if (IsArchivItem(hItem))
	{
		// nur bei nicht DTS Sendern!
		CServer* pServer = GetServer(theCtrl.GetParentItem(hItem));
		if (pServer && !pServer->IsDV())
		{
			// Es können nur Backuparchive gelöscht werden
			// Alle anderen werden geleert.
			pPopup = menu.GetSubMenu(1);
			CIPCArchivRecord* pArchive = GetArchiveRecord(hItem);
			if (pArchive)
			{
				if (pArchive->IsBackup())
				{
					iMenuItemID = pPopup->GetMenuItemID(1);
					pPopup->RemoveMenu(ID_ARCHIV_EMPTY, MF_BYCOMMAND);
				}
				else // (!pArchive->IsBackup())
				{
					iMenuItemID = pPopup->GetMenuItemID(1);
					pPopup->RemoveMenu(ID_ARCHIV_DELETE, MF_BYCOMMAND);
				}
			}
		}
	}
	else if (IsDateItem(hItem))
	{
		HTREEITEM hArchive = theCtrl.GetParentItem(hItem);
		if (hArchive)
		{
			// nur bei nicht DTS Sendern!
			CServer* pServer = GetServer(theCtrl.GetParentItem(hArchive));
			if (pServer && !pServer->IsDV())
			{
				// Sequenzen eines Datums können nur gelöscht werden
				pPopup = menu.GetSubMenu(7);
			}
		}
	}
	else if (IsSequenceItem(hItem))
	{
		pPopup = menu.GetSubMenu(2);
		HTREEITEM hDate = theCtrl.GetParentItem(hItem);
		if (hDate)
		{
			HTREEITEM hItemArchiv = theCtrl.GetParentItem(hDate);
			if (hItemArchiv)
			{
				HTREEITEM hItemServer = theCtrl.GetParentItem(hItemArchiv);
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
	}

	if (pPopup)
	{
		pPopup->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,AfxGetMainWnd());
	}
}
///////////////////////////////////////////////////////////////////////
void CObjectView::OnSequenceOpenSingle() 
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
///////////////////////////////////////////////////////////////////////
void CObjectView::OnUpdateSequenceOpenSingle(CCmdUI* pCmdUI) 
{
	BOOL bEnable = TRUE;
	if (m_bBackupMode)
	{
		bEnable = FALSE;
	}
	CMainFrame* pMF = (CMainFrame*)theApp.m_pMainWnd;
	if (pMF && pMF->m_hWnd && pMF->IsSyncMode())
	{
		bEnable = FALSE;
	}

	pCmdUI->Enable(bEnable);
}
///////////////////////////////////////////////////////////////////////
void CObjectView::OnSequenceOpen() 
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

	CTreeCtrl &theCtrl = GetTreeCtrl();
	HTREEITEM hArchiv = NULL;
	HTREEITEM hItem = theCtrl.GetSelectedItem();
	if (IsSequenceItem(hItem))
	{
		hArchiv = theCtrl.GetParentItem(theCtrl.GetParentItem(hItem));
	}
	else if (IsDateItem(hItem))
	{
		// TODO: Öffnen ?
		hArchiv = theCtrl.GetParentItem(hItem);
	}
	else
	{
		return;
	}

	if (hArchiv)
	{
		HTREEITEM hServer = theCtrl.GetParentItem(hArchiv);
		if (hServer)
		{
			WORD wServerID = LOWORD(theCtrl.GetItemData(hServer));
			WORD wArchiv   = LOWORD(theCtrl.GetItemData(hArchiv));
			WORD wSequence = LOWORD(theCtrl.GetItemData(hItem));

			CRechercheDoc* pDoc = GetDocument();
			if (pDoc)
			{
				CServer* pServer = pDoc->GetServer(wServerID);
				if (pServer)
				{
					pServer->OpenSequence(wArchiv, wSequence);
				}
			}
		}
	}
}
///////////////////////////////////////////////////////////////////////
void CObjectView::OnDelete() 
{
	CRechercheDoc* pDoc = GetDocument();
	if (!pDoc)
	{
		return;
	}

	int iRet = IDCANCEL;
	CTreeCtrl &theCtrl = GetTreeCtrl();

	// This can be time consuming for very large trees 
	// and is called every time the user does a normal selection
	// If performance is an issue, it may be better to maintain 
	// a list of selected items
	HTREEITEM hItem;
	for ( hItem=theCtrl.GetRootItem(); hItem!=NULL;  hItem=theCtrl.GetNextItem( hItem ,TVGN_NEXTVISIBLE) )
	{
		if (!(theCtrl.GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED ))
		{
			// keine selectierten
			continue;
		}

		if (IsArchivItem(hItem))
		{
			HTREEITEM hArchive = hItem;
			HTREEITEM hServer = theCtrl.GetParentItem(hArchive);
			if (hServer)
			{
				WORD wServerID = LOWORD(theCtrl.GetItemData(hServer));
				CServer* pServer = pDoc->GetServer(wServerID);
				if (   pServer 
					&& pServer->IsConnected()
#ifndef _DEBUG
					&& !pServer->IsDV()
#endif
					)
				{
					CString sMsg;
					CString sArchive = theCtrl.GetItemText(hArchive);
					sMsg.Format(IDP_DELETE_ARCHIV, sArchive, pServer->GetName());
					if (iRet!=IDYESALL)
					{
						iRet = COemGuiApi::MessageBox(sMsg,0,MB_YESNOCANCEL|MB_YESALL|MB_ICONQUESTION);
					}
					if (iRet==IDCANCEL)
					{
						break;
					}
					if ((iRet==IDYES) || (iRet==IDYESALL))
					{
						HTREEITEM hDate = theCtrl.GetChildItem(hArchive);
						while (hDate)
						{
							if ( theCtrl.GetItemState( hDate, TVIS_SELECTED ) & TVIS_SELECTED )
							{
								// selected will be added later
							}
							else
							{
								HTREEITEM hSequence = theCtrl.GetChildItem(hDate);
								while (hSequence)
								{
									if ( theCtrl.GetItemState( hSequence, TVIS_SELECTED ) & TVIS_SELECTED )
									{
										// selected will be added later
									}
									else
									{
										WORD wArchiv   = LOWORD(theCtrl.GetItemData(hArchive));
										WORD wSequence = LOWORD(theCtrl.GetItemData(hSequence));
										pServer->AddDeleteArchivFile(wArchiv, wSequence);
									}
									hSequence = theCtrl.GetNextSiblingItem(hSequence);
								}
							}
							hDate = theCtrl.GetNextSiblingItem(hDate);
						}
					}
				}
			}
		}
		if (IsDateItem(hItem))
		{
			HTREEITEM hDate = hItem;
			HTREEITEM hArchive = theCtrl.GetParentItem(hDate);
			if (hArchive)
			{
				HTREEITEM hServer = theCtrl.GetParentItem(hArchive);
				if (hServer)
				{
					WORD wServerID = LOWORD(theCtrl.GetItemData(hServer));
					CServer* pServer = pDoc->GetServer(wServerID);
					if (   pServer 
						&& pServer->IsConnected()
#ifndef _DEBUG
						&& !pServer->IsDV()
#endif
						)
					{
						CString sMsg;
						CString sDate = theCtrl.GetItemText(hDate);
						CString sArchive = theCtrl.GetItemText(hArchive);
						sMsg.Format(IDP_DELETE_DATE_SEQUENCES, sDate, sArchive, pServer->GetName());
						if (iRet!=IDYESALL)
						{
							iRet = COemGuiApi::MessageBox(sMsg,0,MB_YESNOCANCEL|MB_YESALL|MB_ICONQUESTION);
						}
						if (iRet==IDCANCEL)
						{
							break;
						}
						if ((iRet==IDYES) || (iRet==IDYESALL))
						{
							HTREEITEM hSequence = theCtrl.GetChildItem(hDate);
							if (hSequence)
							{
								while (hSequence)
								{
									if ( theCtrl.GetItemState( hSequence, TVIS_SELECTED ) & TVIS_SELECTED )
									{
										// selected will be added later
									}
									else
									{
										WORD wArchiv   = LOWORD(theCtrl.GetItemData(hArchive));
										WORD wSequence = LOWORD(theCtrl.GetItemData(hSequence));
										pServer->AddDeleteArchivFile(wArchiv, wSequence);
									}
									hSequence = theCtrl.GetNextSiblingItem(hSequence);
								}
							}
						}
					}
				}
			}
		}
		if (IsSequenceItem(hItem))
		{
			HTREEITEM hSequence = hItem;
			HTREEITEM hDate = theCtrl.GetParentItem(hSequence);
			if (hDate)
			{
				HTREEITEM hArchiv = theCtrl.GetParentItem(hDate);
				if (hArchiv)
				{
					HTREEITEM hServer = theCtrl.GetParentItem(hArchiv);
					if (hServer)
					{
						WORD wServerID = LOWORD(theCtrl.GetItemData(hServer));
						CServer* pServer = pDoc->GetServer(wServerID);
						if (pServer 
#ifndef _DEBUG
							&& !pServer->IsDV()
#endif
							)
						{
							CString sMsg;
							CString sSequence = theCtrl.GetItemText(hSequence);
							CString sDate = theCtrl.GetItemText(hDate);
							CString sArchive = theCtrl.GetItemText(hArchiv);
							sMsg.Format(IDP_DELETE_SEQUENCE, sSequence, sDate, sArchive, pServer->GetName());
							if (iRet!=IDYESALL)
							{
								iRet = COemGuiApi::MessageBox(sMsg,0,MB_YESNOCANCEL|MB_YESALL|MB_ICONQUESTION);
							}
							if (iRet==IDCANCEL)
							{
								break;
							}
							if ((iRet==IDYES) || (iRet==IDYESALL))
							{
								if (pServer && pServer->IsConnected())
								{
									WORD wArchiv   = LOWORD(theCtrl.GetItemData(hArchiv));
									WORD wSequence = LOWORD(theCtrl.GetItemData(hSequence));
									pServer->AddDeleteArchivFile(wArchiv, wSequence);
								}
							}
						}
					}
				}
			}
		}
	}
	pDoc->DeleteArchivFiles();
}
///////////////////////////////////////////////////////////////////////
void CObjectView::OnDatabaseInfo() 
{
	CRechercheDoc* pDoc = GetDocument();
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
		WORD wServerID = LOWORD(GetTreeCtrl().GetItemData(hServer));

		CServer* pServer = pDoc->GetServer(wServerID);
		if (pServer && pServer->IsConnected())
		{
			pServer->ShowInfoDialog();
		}
	}
}
///////////////////////////////////////////////////////////////////////
void CObjectView::OnSearch() 
{
	CRechercheDoc* pDoc = GetDocument();
	if (!pDoc)
	{
		return;
	}
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	HTREEITEM hServer = GetServerItem(hItem);

	if (hItem && IsServerItem(hServer))
	{
		WORD wServerID = LOWORD(GetTreeCtrl().GetItemData(hServer));

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
///////////////////////////////////////////////////////////////////////
void CObjectView::OnUpdateSearch(CCmdUI* pCmdUI) 
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
void CObjectView::ShowSearchResultDialog()
{
	m_pSearchResultDialog = new CSearchResultDialog(this);
}
//////////////////////////////////////////////////////////////////////////////////
BOOL CObjectView::PreTranslateMessage(MSG* pMsg) 
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
//////////////////////////////////////////////////////////////////////////////////
BOOL CObjectView::OnToolTipNeedText(UINT id, NMHDR * pNMHDR, LRESULT * pResult)
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
				_tcscpy(pTTT->szText, strTip);
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
////////////////////////////////////////////////////////////////////////////
void CObjectView::OnMouseMove(UINT nFlags, CPoint point) 
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
////////////////////////////////////////////////////////////////////////////
CIPCArchivRecord* CObjectView::HittestArchivToolTip(CPoint pt)
{
   CTreeCtrl &theCtrl = GetTreeCtrl();
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

		hItem = theCtrl.HitTest(pt,&nFlags);

		// camera click on State Image
		if (hItem && (nFlags & TVHT_ONITEM))
		{
			if (IsArchivItem(hItem))
			{
				HTREEITEM hServer = GetServerItem(hItem);
				if (hServer)
				{
					CRechercheDoc* pDoc = GetDocument();
					CServer* pServer = pDoc->GetServer(LOWORD(theCtrl.GetItemData(hServer)));
					if (pServer && pServer->IsConnected())
					{
						CIPCDatabaseRecherche* pDataBase = pServer->GetDatabase();
						pDataBase->Lock();
						pRecord = pDataBase->GetArchiv(LOWORD(theCtrl.GetItemData(hItem)));
						pDataBase->Unlock();
					}
				}
			}
		}
	}
	return pRecord;
}
////////////////////////////////////////////////////////////////////////////
void CObjectView::OnUpdateSequenceOpen(CCmdUI* pCmdUI) 
{
	if (m_bBackupMode)
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	CMainFrame* pMF = (CMainFrame*)theApp.m_pMainWnd;
	if (WK_IS_WINDOW(pMF) && pMF->IsSyncMode())
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	pCmdUI->Enable(IsSequenceItem(GetTreeCtrl().GetSelectedItem()));
}
////////////////////////////////////////////////////////////////////////////
void CObjectView::OnUpdateDelete(CCmdUI* pCmdUI) 
{
	if (!m_bBackupMode)
	{
		HTREEITEM hSelected = GetTreeCtrl().GetSelectedItem();
		if (IsSequenceItem(hSelected))
		{
			CIPCSequenceRecord* pSR = GetSequenceRecord(hSelected);
			if (   pSR
				&& pSR->IsDeleteAllowed()
				)
			{
				pCmdUI->Enable(TRUE);
				return;
			}
		}
	}
	pCmdUI->Enable(FALSE);
}
////////////////////////////////////////////////////////////////////////////
void CObjectView::OnArchivEmpty() 
{
	OnDelete();
}
////////////////////////////////////////////////////////////////////////////
void CObjectView::OnUpdateArchivEmpty(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	if (IsArchivItem(hItem))
	{
		CIPCArchivRecord* pArchive = GetArchiveRecord(hItem);
		if (pArchive)
		{
			HTREEITEM hDateChild = GetTreeCtrl().GetChildItem(hItem);
			if (hDateChild)
			{
				HTREEITEM hSequence = GetTreeCtrl().GetChildItem(hDateChild);
				if (IsSequenceItem(hSequence))
				{
					CIPCSequenceRecord* pSR = GetSequenceRecord(hSequence);
					if (   pSR
						&& pSR->IsDeleteAllowed()
						)
					{
						bEnable = pSR->IsDeleteAllowed();
					}
				}
			}
		}
	}
	pCmdUI->Enable(bEnable);
}
////////////////////////////////////////////////////////////////////////////
void CObjectView::Redraw()
{
	//DWORD dwTick = GetTickCount();
	if (/*(dwTick-m_dwLastRedraw>2000) &&*/ m_bNeedsRedraw)
	{
		// m_dwLastRedraw = dwTick;
		CRect rect;
		GetTreeCtrl().GetClientRect(rect);
		GetTreeCtrl().InvalidateRect(rect);
		m_bNeedsRedraw = FALSE;
	}
}
////////////////////////////////////////////////////////////////////////////
void CObjectView::OnArchivBackup() 
{
	if (!theApp.m_bAllowBackup)
	{
		return;
	}

	HTREEITEM hItem = NULL;
	HTREEITEM hParent = NULL;
	CRechercheDoc* pDoc = GetDocument();
	CServer* pServer = NULL;

	if (m_bBackupMode)
	{
		hItem = GetTreeCtrl().GetRootItem();
		while (hItem)
		{
			pServer = pDoc->GetServer(LOWORD(GetTreeCtrl().GetItemData(hItem)));
			if (pServer && pServer->IsInBackupMode())
			{
				hParent = hItem;
				break;
			}
			else
			{
				pServer = NULL;
				hItem = GetTreeCtrl().GetNextSiblingItem(hItem);
			}
		}
	}
	else
	{
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
		pServer = pDoc->GetServer(LOWORD(GetTreeCtrl().GetItemData(hParent)));
	}

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
		m_bBackupMode = FALSE;
		return;
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
		pServer->SetBackupMode(FALSE);
		// Enable tree again, if disabled
		EnableWindow(TRUE);
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
				SetCheckRecurse(GetTreeCtrl(),hArchiv,TREE_CONTROL_UNCHECKED, (pServer->CanSplittingBackup() == FALSE));
			}
			hArchiv = GetTreeCtrl().GetNextSiblingItem(hArchiv);
		}
		m_bBackupMode = TRUE;
		pServer->SetBackupMode(TRUE);
		GetDocument()->UpdateMyViews(this,VDH_BACKUP,(CObject*)pServer);
	}
}
////////////////////////////////////////////////////////////////////////////
void CObjectView::OnUpdateArchivBackup(CCmdUI* pCmdUI) 
{
	// dongle allows Backup?
	BOOL bEnable = theApp.m_bAllowBackup
					&& (((CMainFrame*)AfxGetMainWnd())->IsSyncMode() == FALSE);
	if (bEnable)
	{
		// user has permission?
		if (theApp.m_pPermission && 
			(theApp.m_pPermission->GetFlags() & WK_ALLOW_BACKUP) == WK_ALLOW_BACKUP
			)
		{
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
				bEnable = FALSE;
			}
			else
			{
				CRechercheDoc* pDoc = GetDocument();
				CServer* pServer;
				pServer = pDoc->GetServer(LOWORD(GetTreeCtrl().GetItemData(hParent)));

				if (pServer==NULL)
				{
					bEnable = FALSE;
				}
				else
				{
					if (pServer->GetDatabase()->GetVersion()<3)
					{
						bEnable = FALSE;
					}
				}
			}
		}
		else
		{
			bEnable = FALSE;
		}
	}
	pCmdUI->Enable(bEnable); 
	pCmdUI->SetCheck(m_bBackupMode);
}
////////////////////////////////////////////////////////////////////////////
void CObjectView::OnArchivDelete() 
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
					CIPCDatabaseRecherche* pDatabase = pServer->GetDatabase();
					if (pDatabase)
					{
						WK_TRACE(_T("delete archiv %d\n"), pArchive->GetSubID());
						pDatabase->DoRequestDeleteArchiv(pArchive->GetSubID());
					}
				}
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////
void CObjectView::OnUpdateArchivDelete(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	if (IsArchivItem(hItem))
	{
		CIPCArchivRecord* pArchive = GetArchiveRecord(hItem);
		if (   pArchive 
			&& pArchive->IsBackup())
		{
			HTREEITEM hDateChild = GetTreeCtrl().GetChildItem(hItem);
			if (hDateChild)
			{
				HTREEITEM hSequence = GetTreeCtrl().GetChildItem(hDateChild);
				if (IsSequenceItem(hSequence))
				{
					CIPCSequenceRecord* pSR = GetSequenceRecord(hSequence);
					if (   pSR
						&& pSR->IsDeleteAllowed()
						)
					{
						bEnable = pSR->IsDeleteAllowed();
					}
				}
			}
			else // no entries left, enable delete
			{
				// empty archives will not be backuped anyway,
				// so the lower entries were deleted before
				bEnable = TRUE;
			}
		}
	}
	pCmdUI->Enable(bEnable);
}
////////////////////////////////////////////////////////////////////////////
void CObjectView::OnDateSequencesDelete() 
{
	OnDelete();
}
////////////////////////////////////////////////////////////////////////////
void CObjectView::OnUpdateDateSequencesDelete(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	if (!theApp.m_bReadOnlyModus)
	{
		HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
		if (IsDateItem(hItem))
		{
			HTREEITEM hSequence = GetTreeCtrl().GetChildItem(hItem);
			if (IsSequenceItem(hSequence))
			{
				CIPCSequenceRecord* pSR = GetSequenceRecord(hSequence);
				if (pSR)
				{
					bEnable = pSR->IsDeleteAllowed();
				}
			}
		}
	}
	pCmdUI->Enable(bEnable);
}
////////////////////////////////////////////////////////////////////////////
void CObjectView::AddAllBackupSequencesExcept(WORD wServerID, CIPCSequenceRecord* pSequence)
{
	HTREEITEM hServer = GetServerItem(wServerID);
	HTREEITEM hArchiv = NULL;

	if (hServer)
	{
		hArchiv = GetTreeCtrl().GetChildItem(hServer);
		while (hArchiv)
		{
			if (pSequence->GetArchiveNr() == LOWORD(GetTreeCtrl().GetItemData(hArchiv)))
			{
				// found archiv
				HTREEITEM hDate = GetTreeCtrl().GetChildItem(hArchiv);

				while (hDate)
				{
					// found date
					HTREEITEM hSequence = GetTreeCtrl().GetChildItem(hDate);

					while (hSequence)
					{
						WORD wSequenceNr = LOWORD(GetTreeCtrl().GetItemData(hSequence));

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
					hDate = GetTreeCtrl().GetNextSiblingItem(hDate);
				}
				break;
			}
			hArchiv = GetTreeCtrl().GetNextSiblingItem(hArchiv);
		}
	}
}
////////////////////////////////////////////////////////////////////////////
void CObjectView::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
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

		// TRACE(_T("<%s>\n"),GetTreeCtrl().GetItemText(hClicked));

		if (hClicked && (nFlags & TVHT_ONITEMSTATEICON))
		{
			// check recursiv only, if in BackupMode, but not, if server can splitting backup
			BOOL bRecursiv = m_bBackupMode;
			if (bRecursiv)
			{
				HTREEITEM hItem;
				HTREEITEM hParent = NULL;

				hItem = hClicked;
				while (hItem)
				{
					hParent = hItem;
					hItem = GetTreeCtrl().GetParentItem(hItem);
				}
				if (hParent)
				{
					CRechercheDoc* pDoc = GetDocument();
					CServer* pServer = pDoc->GetServer(LOWORD(GetTreeCtrl().GetItemData(hParent)));
					if (pServer && pServer->CanSplittingBackup())
					{
						bRecursiv = FALSE;
					}
				}
			}
			BOOL bCheck = GetCheck(GetTreeCtrl(),hClicked);
			if (bCheck)
			{
				// uncheck it
				SetCheckRecurse(GetTreeCtrl(),hClicked,TREE_CONTROL_UNCHECKED, bRecursiv);
			}
			else
			{
				// check it
				// If in BackupMode only if server can no split backup
				SetCheckRecurse(GetTreeCtrl(),hClicked,TREE_CONTROL_CHECKED, bRecursiv);
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
				else if (IsDateItem(hClicked))
				{
					// it's a date
					CRechercheDoc* pDoc = GetDocument();
					HTREEITEM hItem = GetTreeCtrl().GetNextItem(hClicked, TVGN_CHILD);
					while (hItem)
					{
						pDoc->UpdateMyViews(this,
							bCheck ? VDH_BACKUP_ADD_SEQUENCE : VDH_BACKUP_DEL_SEQUENCE,
							(CObject*)GetSequenceRecord(hItem));
						hItem = GetTreeCtrl().GetNextItem(hItem, TVGN_NEXT);
					}
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
					WORD wArchive = LOWORD(GetTreeCtrl().GetItemData(hClicked));
					WORD wServer = LOWORD(GetTreeCtrl().GetItemData(GetServerItem(hClicked)));
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
////////////////////////////////////////////////////////////////////////////
CServer* CObjectView::GetServer(HTREEITEM hServer)
{
	if (IsServerItem(hServer))
	{
		WORD wServerID = LOWORD(GetTreeCtrl().GetItemData(hServer));
		CRechercheDoc* pDoc = GetDocument();
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
CIPCSequenceRecord* CObjectView::GetSequenceRecord(HTREEITEM hSequence)
{
	CTreeCtrl &theCtrl = GetTreeCtrl();
	CIPCSequenceRecord* pSequence = NULL;
	HTREEITEM hArchiv;
	ASSERT(IsSequenceItem(hSequence));
	hArchiv = theCtrl.GetParentItem(theCtrl.GetParentItem(hSequence));
	if (hArchiv)
	{
		HTREEITEM hServer = theCtrl.GetParentItem(hArchiv);
		if (hServer)
		{
			WORD wServerID = LOWORD(theCtrl.GetItemData(hServer));
			WORD wArchiv   = LOWORD(theCtrl.GetItemData(hArchiv));
			WORD wSequence = LOWORD(theCtrl.GetItemData(hSequence));

			CRechercheDoc* pDoc = GetDocument();
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
CIPCArchivRecord* CObjectView::GetArchiveRecord(HTREEITEM hArchiv)
{
	CTreeCtrl &theCtrl = GetTreeCtrl();
	CIPCArchivRecord* pArchiv = NULL;
	if (IsArchivItem(hArchiv))
	{
		HTREEITEM hServer = theCtrl.GetParentItem(hArchiv);
		if (hServer)
		{
			WORD wServerID = LOWORD(theCtrl.GetItemData(hServer));
			WORD wArchiv   = LOWORD(theCtrl.GetItemData(hArchiv));

			CRechercheDoc* pDoc = GetDocument();
			if (pDoc)
			{
				CServer* pServer = pDoc->GetServer(wServerID);
				if (pServer)
				{
					pArchiv = pServer->GetCIPCArchivRecord(wArchiv);
				}
			}
		}
	}
	return pArchiv;
}
////////////////////////////////////////////////////////////////////////////
BOOL CObjectView::ShowSyncCheckBoxes(BOOL bShow /*= TRUE*/)
{
   CTreeCtrl &theCtrl = GetTreeCtrl();
	for (HTREEITEM hServer = theCtrl.GetRootItem();hServer;hServer = theCtrl.GetNextSiblingItem(hServer))
	{
		for (HTREEITEM hArchiv = theCtrl.GetChildItem(hServer);hArchiv;hArchiv = theCtrl.GetNextSiblingItem(hArchiv))
		{
			int i=0,is=0;
			theCtrl.GetItemImage(hArchiv,i,is);
			if (   (i == _IMAGE_RING)
				|| (i == _IMAGE_ALARM)
				|| (i == _IMAGE_BACKUP)
				|| (i == _IMAGE_SEARCH)
				)
			{
				SetCheckRecurse(theCtrl,
					hArchiv,
					bShow?TREE_CONTROL_UNCHECKED:TREE_CONTROL_CHECK_OFF,
					FALSE);
				if (bShow)
				{
					theCtrl.Expand(hArchiv,TVE_COLLAPSE);
				}
			}
		}
	}

	return TRUE;
}


void CObjectView::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == m_dwLastRedraw)
	{
		Redraw();
	}

	
	CTreeView::OnTimer(nIDEvent);
}

int CObjectView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_dwLastRedraw = SetTimer(123, 1000, NULL);
	
	return 0;
}

void CObjectView::OnDestroy() 
{
	if (m_dwLastRedraw)
	{
		KillTimer(m_dwLastRedraw);
		m_dwLastRedraw = 0;
	}
	CTreeView::OnDestroy();
	
	// TODO: Add your message handler code here
	
}


