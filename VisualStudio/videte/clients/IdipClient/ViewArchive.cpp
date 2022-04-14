/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ViewArchive.cpp $
// ARCHIVE:		$Archive: /Project/Clients/IdipClient/ViewArchive.cpp $
// CHECKIN:		$Date: 21.04.06 14:38 $
// VERSION:		$Revision: 82 $
// LAST CHANGE:	$Modtime: 21.04.06 14:29 $
// BY AUTHOR:	$Author: Tomas.krogh $
// $Nokeywords:$

#include "stdafx.h"
#include "IdipClient.h"
#include "ViewArchive.h"

#include "MainFrm.h"
#include "IdipClientDoc.h"
#include "ViewIdipClient.h"

#include "images.h"
#include "CIPCDataBaseIdipClient.h"
#include "DlgBackup.h"
#include "DlgQuery.h"

#include "ViewControllItems.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewArchive

#define NODE_SERVERID	(WORD)1
#define NODE_ARCHIVEID	(WORD)2
#define NODE_DATEID		(WORD)3
#define NODE_SEQUENCEID	(WORD)4

IMPLEMENT_DYNCREATE(CViewArchive, CViewObjectTree)

CViewArchive::CViewArchive()
{
	m_hItemFirstSel			= NULL;
	m_pDlgQuery			= NULL;
	m_pDlgSearchResult	= NULL;
	m_pArchivToolTip		= NULL;
	m_bNeedsRedraw			= FALSE;
	m_bBackupMode			= FALSE;
	m_bInsertion			= FALSE;
	if (theApp.IsReadOnlyModus())
	{
		m_eVisibility = OTVS_Always;
	}
}

CViewArchive::~CViewArchive()
{
}


BEGIN_MESSAGE_MAP(CViewArchive, CViewObjectTree)
	//{{AFX_MSG_MAP(CViewArchive)
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
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_USER,OnUser)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNeedText)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_DATABASE_INFO, OnUpdateDatabaseInfo)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewArchive drawing



/////////////////////////////////////////////////////////////////////////////
// CViewArchive message handlers
/////////////////////////////////////////////////////////////////////////////
void CViewArchive::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	WORD cause	  = LOWORD(lHint);
	WORD wServerID = HIWORD(lHint);
	CSecID id;

	switch (cause)
	{
	case NULL:	// called by CView::OnInitialUpdate()
	case VDH_REMOVE_SERVER:
		DeleteServer(wServerID);
		break;
	case VDH_DATABASE:
		ASSERT_KINDOF(CIPCDatabaseIdipClient, pHint);
		InsertDatabase((CIPCDatabaseIdipClient*)pHint);
		break;
	case VDH_DEL_SEQUENCE:
		{
			DWORD dw = (DWORD)pHint;
			WORD wArchivNr = LOWORD(dw);
			WORD wSequenceNr = HIWORD(dw);
			RemoveSequence(wServerID,wArchivNr,wSequenceNr);
		}
		break;
	case VDH_ADD_ARCHIV:
		ASSERT_KINDOF(CIPCArchivRecord, pHint);
		InsertArchiv(GetTreeCtrl(),GetServerItem(wServerID),(CIPCArchivRecord*)pHint);
		break;
	case VDH_ADD_SEQUENCE:
	{
		ASSERT_KINDOF(CIPCSequenceRecord, pHint);
		HTREEITEM hArchive = NULL, hInsert = NULL;
		InsertSequence(wServerID,(CIPCSequenceRecord*)pHint, hArchive, hInsert);
	}	break;
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
				SetCheckRecurse(hArchiv,TREE_CONTROL_UNCHECKED,FALSE);
			}
		}
		break;
#ifdef _DEBUG
	case VDH_BACKUP_ADD_SEQUENCE:
	case VDH_BACKUP_DEL_SEQUENCE:
	case VDH_BACKUP_ADD_ARCHIVE:
	case VDH_BACKUP_DEL_ARCHIVE:
	case VDH_SELECT_SEQUENCE:
	case VDH_SET_CMD_ACTIVE_WND:
	case VDH_INPUT:
	case VDH_OUTPUT:
	case VDH_ADD_CAMWINDOW:
	case VDH_REMOVE_CAMWINDOW:
	case VDH_ACTIVATE_REQUEST_CAM:
	case VDH_INFORMATION:
	case VDH_INPUT_CONFIRM:
	case VDH_DELETE_CAMWINDOW:
	case VDH_INPUT_URL:
	case VDH_DEACTIVATE_CAMERAS:
	case VDH_NOTIFICATION_MESSAGE:
	case VDH_SINGLE_CAM:
	case VDH_CAMERA_MAP:
	case VDH_CAM_MAP:
	case VDH_HOST_MAP:
	case VDH_AUDIO:
	case VDH_SET_AUDIO_INPUT_STATE:
	case VDH_OPEN_SEQUENCE:
	case VDH_OPEN_FILE:
	case VDH_DEL_ARCHIV:
		break;
#endif
	default:
		TRACE(_T("unknown cause in CViewArchive::OnUpdate %d\n"), cause);
		break;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewArchive::InsertDatabase(CIPCDatabaseIdipClient* pDatabase)
{
	m_bInsertion = TRUE;
	CTreeCtrl &theCtrl = GetTreeCtrl();
	HTREEITEM hServer;
	CServer*  pServer = pDatabase->GetServer();
	WORD wServerID = pServer->GetID();

	// insert server, input/camera/relais if not in tree
	hServer = GetServerItem(wServerID);

	if (hServer)
	{
		CIPCArchivRecord* pRecord;
		int i,c;
		CIPCArchivRecordArray& a = pDatabase->GetRecords();

		a.Lock(_T(__FUNCTION__));
		c = a.GetSize();
		for (i=0;i<c;i++)
		{
			pRecord = a.GetAtFast(i);
			InsertArchiv(theCtrl,hServer,pRecord);
			m_bInsertion = TRUE;
		}
		a.Unlock();

		theCtrl.Expand(hServer,TVE_EXPAND);
		if (WK_IS_WINDOW(m_pDlgQuery))
		{
			m_pDlgQuery->InitServerArchives();
		}
	}
	m_bInsertion = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CViewArchive::InsertArchiv(CTreeCtrl& ctrl,HTREEITEM hServer, CIPCArchivRecord* pRecord)
{
	WORD wServer = LOWORD(ctrl.GetItemData(hServer));
	HTREEITEM hArchiv = NULL;

	m_bInsertion = FALSE;
	hArchiv = GetArchivItem(wServer,pRecord->GetID());
	if (hArchiv)
	{
		TRACE(_T("archive already inserted %s\n"),pRecord->GetName());
	}
	else
	{
		m_bInsertion = TRUE;
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
			i = _IMAGE_ARCH_RING;
		}
		else if (pRecord->IsSafeRing())
		{
			i = _IMAGE_ARCH_PRE_ALARM;
		}
		else if (pRecord->IsAlarm())
		{
			i = _IMAGE_ARCH_ALARM;
		}
		else if (pRecord->IsBackup())
		{
			i = _IMAGE_ARCH_BACKUP;
			tvis.hInsertAfter = TVI_LAST;
		}
		else if (pRecord->IsAlarmList())
		{
			i = _IMAGE_ARCH_ALARMLIST;
		}
		else
		{
			WK_TRACE_ERROR(_T("unknown archive type %s %x\n"),pRecord->GetName(),pRecord->GetFlags());
		}
		if (pRecord->IsSearch())
		{
			i = _IMAGE_ARCH_SEARCH;
		}

		if (i!=-1)
		{
			tvis.item.iImage = i;
			tvis.item.iSelectedImage = i;
			//TRACE(_T("insert archive %s\n"),pRecord->GetName());
			hArchiv = ctrl.InsertItem(&tvis);

			if (tvis.item.iImage == _IMAGE_ARCH_RING)
			{
				if (((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode())
				{
					SetCheckRecurse(hArchiv, TREE_CONTROL_UNCHECKED, FALSE);
				}
			}
		}
		s.ReleaseBuffer();
		m_bInsertion = FALSE;
	}
	return hArchiv;
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CViewArchive::GetServerItem(WORD wServer)
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

	if (m_bInsertion && hServer==NULL)
	{
		CServer *pServer = GetDocument()->GetServer(wServer);
		if (pServer)
		{
			hServer = InsertServer(pServer);
		}
	}
	return hServer;
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CViewArchive::GetArchivItem(WORD wServer, WORD wArchivNr)
{
	CTreeCtrl &theCtrl = GetTreeCtrl();
	HTREEITEM hServer = GetServerItem(wServer);
	HTREEITEM hArchive = NULL;

	if (hServer)
	{
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
	if (m_bInsertion && hArchive == NULL)
	{
		CServer *pServer = GetDocument()->GetServer(wServer);
		if (pServer)
		{
			CIPCArchivRecord* pArchiv = pServer->GetCIPCArchivRecord(wArchivNr);
			if (pArchiv)
			{
				hArchive = InsertArchiv(theCtrl, hServer, pArchiv);
			}
		}
	}
	return hArchive;
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CViewArchive::GetServerItem(HTREEITEM hItem)
{
	CTreeCtrl &theCtrl = GetTreeCtrl();
	while (HIWORD(theCtrl.GetItemData(hItem)) != NODE_SERVERID)
	{
		hItem = theCtrl.GetParentItem(hItem);
	}
	return hItem;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CViewArchive::IsServerItem(HTREEITEM hItem)
{
	return (hItem && HIWORD(GetTreeCtrl().GetItemData(hItem))==NODE_SERVERID) ? TRUE : FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CViewArchive::IsArchivItem(HTREEITEM hItem)
{
	return (hItem && HIWORD(GetTreeCtrl().GetItemData(hItem))==NODE_ARCHIVEID) ? TRUE : FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CViewArchive::IsDateItem(HTREEITEM hItem)
{
	return (hItem && HIWORD(GetTreeCtrl().GetItemData(hItem))==NODE_DATEID) ? TRUE : FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CViewArchive::IsSequenceItem(HTREEITEM hItem)
{
	return (hItem && HIWORD(GetTreeCtrl().GetItemData(hItem))==NODE_SEQUENCEID) ? TRUE : FALSE;
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CViewArchive::InsertServer(const CServer* pServer)
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

	tvis.item.iImage         = _IMAGE_HOSTBASE + ((wServerID-1) % _IMAGE_NR_HOST_COLORS);
	tvis.item.iSelectedImage = tvis.item.iImage;

	hServer = GetTreeCtrl().InsertItem(&tvis);

	GetTreeCtrl().SelectItem(hServer);

	CheckVisibility();
	return hServer;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CViewArchive::DeleteServer(WORD wID)
{
	HTREEITEM hItem;

	DisconnectDialogs(wID);
	hItem = GetTreeCtrl().GetRootItem();
	while (hItem)
	{
		if (wID==LOWORD(GetTreeCtrl().GetItemData(hItem)))
		{
			GetTreeCtrl().DeleteItem(hItem);
			CheckVisibility();
			return TRUE;
		}
		hItem = GetTreeCtrl().GetNextSiblingItem(hItem);
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CViewArchive::DisconnectDialogs(WORD wServerID)
{
	if (m_pDlgQuery)
	{
		m_pDlgQuery->Disconnect(wServerID);
	}
	if (m_pDlgSearchResult)
	{
		m_pDlgSearchResult->Disconnect(wServerID);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewArchive::OnInitialUpdate() 
{
	CViewObjectTree::OnInitialUpdate();
	
	GetTreeCtrl().SetIndent(0);

	m_Images.Create(IDB_IMAGES,_IMAGE_WIDTH,0,RGB(0,255,255));
	m_States.Create(IDB_STATES,14,0,RGB(0,255,255));

	GetTreeCtrl().SetImageList(&m_Images,TVSIL_NORMAL);
	GetTreeCtrl().SetImageList(&m_States,TVSIL_STATE);

//	EnableToolTips(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CViewArchive::OnUser(WPARAM wParam, LPARAM lParam) 
{
	CIdipClientDoc* pDoc = GetDocument();
	WORD cause	   = LOWORD(wParam);
	WORD wServerID = HIWORD(wParam);
	CServer* pServer = pDoc->GetServer(wServerID);

	switch (cause)
	{
		case VDH_DEL_ARCHIV:
			RemoveArchiv(wServerID,LOWORD(lParam));
			GetDocument()->UpdateAllViews(this, wParam, (CObject*)lParam);
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
		case WPARAM_DATABASE_CONNECTED:
			if (wServerID == 0xFFFF)
			{
				GetDocument()->OnLocalDatabaseConnected();
			}
			else if (pServer && pServer->GetDatabase())
			{
				InsertDatabase(pServer->GetDatabase());
			}
			break;
		case WPARAM_DATABASE_DISCONNECTED:
			if (wServerID == 0xFFFF)
			{
				GetDocument()->OnLocalDatabaseDisconnected();
			}
			else
			{
				DeleteServer(wServerID);
			}
			if (pServer)
			{
				// close BackupDialog if nessesary
				GetDocument()->UpdateAllViews(this, MAKELONG(VDH_BACKUP, pServer->GetID()), NULL);
				pServer->DeleteDataBaseObject();
			}
			break;
		case WPARAM_DATABASE_FETCHRESULT:
			if (pServer)
			{
				pServer->OnDatabaseFetchResult();
			} 
			break;
		case WPARAM_ACTUALIZE:
			Actualize(wServerID);
			break;
		case WPARAM_ADD_SEQUENCE:
			InsertSequences(pServer);
			break;
		case WPARAM_DELETE_SEQUENCE:
			break;
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
void CViewArchive::Actualize(WORD wServerID)
{
	HTREEITEM hServer = GetServerItem(wServerID);

	if (hServer)
	{
		CServer* pServer = GetServer(hServer);
		
		if (   pServer
			&& pServer->IsConnectedDatabase())
		{
			HTREEITEM hArchive = GetTreeCtrl().GetChildItem(hServer);

			while (hArchive)
			{
				WORD wArchive = LOWORD(GetTreeCtrl().GetItemData(hArchive));
				CIPCArchivRecord* pRecord = pServer->GetDatabase()->GetArchive(wArchive);

				if (pRecord)
				{
					CString sName;
					sName = GetTreeCtrl().GetItemText(hArchive);
					if (sName != pRecord->GetName())
					{
						GetTreeCtrl().SetItemText(hArchive,pRecord->GetName());
					}
				}

				hArchive = GetTreeCtrl().GetNextSiblingItem(hArchive);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
HTREEITEM CViewArchive::FindServerItem(WORD wServerID, HTREEITEM hSelected)
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
HTREEITEM CViewArchive::FindItemByID(WORD wID, HTREEITEM hParent)
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
void CViewArchive::InsertSequences(CServer* pServer)
{
	m_bInsertion = TRUE;
	if (pServer==NULL)
	{
		return;
	}
	CIPCSequenceRecords& records = pServer->GetNewSequences();

	records.Lock(_T(__FUNCTION__));
	if (records.GetSize())
	{
		WORD wArchive = 0xffff, wServer = pServer->GetID();
		HTREEITEM hArchive = NULL;
		HTREEITEM hSplitItem = NULL;
		CIPCArchivRecord* pArchiveReord = NULL;
		CIPCSequenceRecord* pSequence = NULL;
		for (int i=0;i<records.GetSize(); i++)
		{
			pSequence = records.GetAtFast(i);
			if (wArchive != pSequence->GetArchiveNr())
			{
				wArchive = pSequence->GetArchiveNr();
				hArchive = GetArchivItem(wServer, wArchive);
				if (hArchive)
				{
					pArchiveReord = GetArchiveRecord(hArchive);
				}
				hSplitItem = NULL;
//				TRACE(_T("New Archive %d:%x\n"), wArchive, hArchive);
			}
// To test old performance enable this
//			hArchive = hSplitItem = NULL;
			InsertSequence(wServer, pSequence, hArchive, hSplitItem, pArchiveReord);
			m_bInsertion = TRUE;
		}
		records.DeleteAll();
	}
	records.Unlock();

	Redraw();
	m_bInsertion = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CViewArchive::InsertSequence(WORD wServerID, CIPCSequenceRecord* pSequence, HTREEITEM &hArchiv, HTREEITEM &hSplitItem, CIPCArchivRecord* pArchiveReord)
{
	m_bInsertion = TRUE;
//	TRACE(_T("InsertSequence:%d:%d:%d\n"), wServerID, pSequence->GetArchiveNr(), pSequence->GetSequenceNr());
	if (hArchiv == NULL)
	{
		hArchiv = GetArchivItem(wServerID, pSequence->GetArchiveNr());
		if (hArchiv)
		{
			pArchiveReord = GetArchiveRecord(hArchiv);
		}
	}
	if (hArchiv)
	{
		int iImage = _IMAGE_INVALID;
		if (pSequence->IsSafeRing())
		{
			iImage = _IMAGE_ARCH_PRE_ALARM;
		}
		else if (pSequence->IsAlarm())
		{
			iImage = _IMAGE_ARCH_ALARM;
		}
		else if (pSequence->IsRing())
		{
			iImage = _IMAGE_ARCH_RING;
		}
		else if (pSequence->IsAlarmList())
		{
			iImage = _IMAGE_ARCH_ALARMLIST;
		}
		else if (pSequence->IsBackup())
		{
			iImage = _IMAGE_ARCH_BACKUP;
		}
		else
		{
			WK_TRACE(_T("unknown sequence type %x %s ar=%d\n"),
				pSequence->GetFlags(),pSequence->GetName(),pSequence->GetArchiveNr());
		}
		if (pSequence->IsSearch())
		{
			iImage = _IMAGE_ARCH_SEARCH;
		}
		if (pSequence->IsSuspect())
		{
			iImage = _IMAGE_ARCH_SUSPECT;
		}

		// Without image no insert!
		if (iImage != _IMAGE_INVALID)
		{
			CTreeCtrl &theCtrl = GetTreeCtrl();
			// nur neu zeichnen, wenn da nicht
			// schon childs waren
			BOOL bHasChildren = theCtrl.ItemHasChildren(hArchiv);
			m_bNeedsRedraw = TRUE;//!bHasChildren;

			CString sName(pSequence->GetName());
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
			CString sSplitItem(tTime.GetDate());


			// Does the split item already exist?

			if (bHasChildren)
			{
				if (   hSplitItem == NULL
					|| sSplitItem != theCtrl.GetItemText(hSplitItem))
				{
//					TRACE(_T("Change to DateItem %s, Previous was:%s\n"), sSplitItem, hSplitItem != NULL ? theCtrl.GetItemText(hSplitItem) : _T("0"));
					hSplitItem = theCtrl.GetNextItem(hArchiv, TVGN_CHILD);
				}
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
				if (iArchiveImage == _IMAGE_ARCH_ALARM)
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
//			CString sTemp = sName;
//			sName.Format(_T("%s Nr:%04d, %s"),sTemp,
//										  pSequence->GetSequenceNr(),
//										  pSequence->GetSize().Format(TRUE));
#endif
			sName = tTime.GetTime();
			tvis.item.cchTextMax     = sName.GetLength();
			tvis.item.pszText        = (LPTSTR)(LPCTSTR)sName; // only reading!
			tvis.item.lParam         = MAKELONG(pSequence->GetSequenceNr(), NODE_SEQUENCEID);
			tvis.item.iImage         = iImage;
			tvis.item.iSelectedImage = iImage;
			tvis.hInsertAfter = TVI_SORT;
//			tvis.hInsertAfter = TVI_;

			HTREEITEM hArchivFile = NULL;
			hArchivFile = GetTreeCtrl().InsertItem(&tvis);
		}
		else
		{	// no image fond, sequence not inserted
			ASSERT(iImage != _IMAGE_INVALID);
		}

		// gf todo?
		// shall the sequence really be opened, even if not visible in tree?
		if (m_pDlgSearchResult != NULL
			&& (pSequence->IsSearch())//GetArchiveNr()==255)
			&& theApp.GetQueryParameter().ShowPictures()
			&& theApp.GetQueryParameter().CopyPictures())
		{
			// ist ein Suchergebnis, bitte oeffnen
			CIdipClientDoc* pDoc = GetDocument();
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
//		ASSERT(FALSE);
	}
	//Redraw();
	m_bInsertion = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CViewArchive::RemoveSequence(WORD wServerID, WORD wArchivNr, WORD wSequenceNr)
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
void CViewArchive::RemoveArchiv(WORD wServerID, WORD wArchivNr)
{
	HTREEITEM hArchiv;
	hArchiv = GetArchivItem(wServerID,wArchivNr);
	if (hArchiv)
	{
		GetTreeCtrl().DeleteItem(hArchiv);
		CServer* pServer = GetDocument()->GetServer(wServerID);
		if (pServer)
		{
			pServer->RemoveArchiveSequences(wArchivNr);
		}
		m_bNeedsRedraw = TRUE;
	}
	//Redraw();
}
/////////////////////////////////////////////////////////////////////////////
void CViewArchive::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_bBackupMode)
	{
		return;
	}

	if (((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode())
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

				CIdipClientDoc* pDoc = GetDocument();
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
void CViewArchive::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
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
void CViewArchive::ContextMenu(const CPoint& pt, HTREEITEM hItem)
{
	CMenu menu;
	CSkinMenu* pPopup = NULL;
	UINT iMenuItemID = 0;
	CTreeCtrl &theCtrl = GetTreeCtrl();

	menu.LoadMenu(IDR_MENU_ARCHIVE);

	if (IsServerItem(hItem))
	{
		pPopup = (CSkinMenu*)menu.GetSubMenu(SUB_MENU_SERVER);
		if (pPopup)
		{
			theCtrl.SelectItem(hItem);

			CWK_Dongle dongle;
			if (!dongle.RunCDRWriter())
			{
				pPopup->DeleteMenu(ID_ARCHIV_BACKUP,MF_BYCOMMAND);
			}
			CString str;
			UINT nID = ID_DISCONNECT;
			CMenu*pM = pPopup->FindMenuString(nID, str);
			if (pM)
			{
				CString sHost;
				int  nFind = str.Find(_T("\t"));
				if (nFind != -1)
				{
					str = str.Left(nFind);
				}
				sHost.Format(_T("%s (%s)"), str, theCtrl.GetItemText(hItem));
				pM->ModifyMenu(nID, MF_BYPOSITION, ID_DISCONNECT_LOW+LOWORD(theCtrl.GetItemData(hItem)), sHost);
			}
		}
	}
	else if (IsArchivItem(hItem))
	{
		// nur bei nicht DTS Sendern!
		CServer* pServer = GetServer(theCtrl.GetParentItem(hItem));
		if (pServer && !pServer->IsDTS())
		{
			// Es können nur Backuparchive gelöscht werden
			// Alle anderen werden geleert.
			pPopup = (CSkinMenu*)menu.GetSubMenu(SUB_MENU_ARCHIV);
			CIPCArchivRecord* pArchive = GetArchiveRecord(hItem);
			if (pArchive)
			{
				if (pArchive->IsBackup())
				{
					iMenuItemID = pPopup->GetMenuItemID(SUB_MENU_ARCHIV);
					pPopup->RemoveMenu(ID_ARCHIV_EMPTY, MF_BYCOMMAND);
				}
				else // (!pArchive->IsBackup())
				{
					iMenuItemID = pPopup->GetMenuItemID(SUB_MENU_ARCHIV);
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
			if (pServer && !pServer->IsDTS())
			{
				// Sequenzen eines Datums können nur gelöscht werden
				pPopup = (CSkinMenu*)menu.GetSubMenu(SUB_MENU_DATE_TIME);
			}
		}
	}
	else if (IsSequenceItem(hItem))
	{
		pPopup = (CSkinMenu*)menu.GetSubMenu(SUB_MENU_SEQUENCE);
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
					if (pServer && pServer->IsDTS())
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
		COemGuiApi::DoUpdatePopupMenu(AfxGetMainWnd(), pPopup);
		pPopup->ConvertMenu(TRUE);
		pPopup->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,AfxGetMainWnd());
		pPopup->ConvertMenu(FALSE);
	}
}
///////////////////////////////////////////////////////////////////////
void CViewArchive::OnSequenceOpenSingle() 
{
	if (m_bBackupMode)
	{
		return;
	}
	CMainFrame* pMF = (CMainFrame*)theApp.m_pMainWnd;
	if (   pMF
		&& pMF->IsSyncMode())
	{
		return;
	}

	pMF->GetViewIdipClient()->CloseWndPlayAll();

	OnSequenceOpen();
}
///////////////////////////////////////////////////////////////////////
void CViewArchive::OnUpdateSequenceOpenSingle(CCmdUI* pCmdUI) 
{
	BOOL bEnable = TRUE;
	if (m_bBackupMode)
	{
		bEnable = FALSE;
	}
	if (((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode())
	{
		bEnable = FALSE;
	}

	pCmdUI->Enable(bEnable);
}
///////////////////////////////////////////////////////////////////////
void CViewArchive::OnSequenceOpen() 
{
	if (m_bBackupMode)
	{
		return;
	}
	if (((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode())
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

			CServer* pServer = GetDocument()->GetServer(wServerID);
			if (pServer)
			{
				pServer->OpenSequence(wArchiv, wSequence);
			}
		}
	}
}
///////////////////////////////////////////////////////////////////////
void CViewArchive::OnDelete() 
{
	CIdipClientDoc* pDoc = GetDocument();
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
					&& pServer->IsConnectedPlay()
#ifndef _DEBUG
					&& !pServer->IsDTS()
#endif
					)
				{
					CString sMsg;
					CString sArchive = theCtrl.GetItemText(hArchive);
					sMsg.Format(IDP_DELETE_ARCHIV, sArchive, pServer->GetName());
//					sMsg.FormatMessage(IDP_DELETE_ARCHIV, sArchive, pServer->GetName());
					// TODO! RKE: FormatMessage uses permutable %1, %2,... format specifiers
					if (iRet!=IDYESALL)
					{
						iRet = AfxMessageBox(sMsg, MB_YESNOCANCEL|MB_YESALL|MB_ICONQUESTION);
//						iRet = COemGuiApi::MessageBox(sMsg,0,MB_YESNOCANCEL|MB_YESALL|MB_ICONQUESTION);
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
						&& pServer->IsConnectedPlay()
#ifndef _DEBUG
						&& !pServer->IsDTS()
#endif
						)
					{
						CString sMsg;
						CString sDate = theCtrl.GetItemText(hDate);
						CString sArchive = theCtrl.GetItemText(hArchive);
						sMsg.Format(IDP_DELETE_DATE_SEQUENCES, sArchive, sDate, pServer->GetName());
						// TODO! RKE: FormatMessage uses permutable %1, %2,... format specifiers
//						sMsg.FormatMessage(IDP_DELETE_DATE_SEQUENCES, sArchive, sDate, pServer->GetName());
						if (iRet!=IDYESALL)
						{
//							iRet = COemGuiApi::MessageBox(sMsg,0,MB_YESNOCANCEL|MB_YESALL|MB_ICONQUESTION);
							iRet = AfxMessageBox(sMsg,MB_YESNOCANCEL|MB_YESALL|MB_ICONQUESTION);
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
							&& !pServer->IsDTS()
#endif
							)
						{
							CString sMsg;
							CString sSequence = theCtrl.GetItemText(hSequence);
							CString sDate = theCtrl.GetItemText(hDate);
							CString sArchive = theCtrl.GetItemText(hArchiv);
							// TODO! RKE: FormatMessage uses permutable %1, %2,... format specifiers
//							sMsg.FormatMessage(IDP_DELETE_SEQUENCE, sSequence, sArchive, sDate, pServer->GetName());
							sMsg.Format(IDP_DELETE_SEQUENCE, sSequence, sArchive, sDate, pServer->GetName());
							if (iRet!=IDYESALL)
							{
//								iRet = COemGuiApi::MessageBox(sMsg,0,MB_YESNOCANCEL|MB_YESALL|MB_ICONQUESTION);
								iRet = AfxMessageBox(sMsg, MB_YESNOCANCEL|MB_YESALL|MB_ICONQUESTION);
							}
							if (iRet==IDCANCEL)
							{
								break;
							}
							if ((iRet==IDYES) || (iRet==IDYESALL))
							{
								if (pServer && pServer->IsConnectedPlay())
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
////////////////////////////////////////////////////////////////////////////
void CViewArchive::OnUpdateDatabaseInfo(CCmdUI *pCmdUI)
{
	BOOL bEnable = FALSE;
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	if (hItem)
	{
		HTREEITEM hServer = NULL;
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
			CServer* pServer = GetDocument()->GetServer(wServerID);
			if (pServer && pServer->IsConnectedPlay())
			{
				bEnable = TRUE;
			}
		}
	}
	pCmdUI->Enable(bEnable);
}
///////////////////////////////////////////////////////////////////////
void CViewArchive::OnDatabaseInfo() 
{
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	if (hItem)
	{
		HTREEITEM hServer = NULL;

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

			CServer* pServer = GetDocument()->GetServer(wServerID);
			if (pServer && pServer->IsConnectedPlay())
			{
				pServer->ShowInfoDialog();
			}
		}
	}
}
///////////////////////////////////////////////////////////////////////
void CViewArchive::OnSearch() 
{
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	HTREEITEM hServer = GetServerItem(hItem);

	if (hItem && IsServerItem(hServer))
	{
		WORD wServerID = LOWORD(GetTreeCtrl().GetItemData(hServer));

		CServer* pServer = GetDocument()->GetServer(wServerID);
		if (pServer && pServer->IsConnectedPlay())
		{
			if (m_pDlgQuery)
			{
			}
			else
			{
				m_pDlgQuery = new CDlgQuery(this);
				m_pDlgQuery->ShowWindow(SW_SHOW);
			}
		}
	}
}
///////////////////////////////////////////////////////////////////////
void CViewArchive::OnUpdateSearch(CCmdUI* pCmdUI) 
{
	if (m_bBackupMode)
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(   GetTreeCtrl().GetRootItem()!=NULL 
					   && m_pDlgSearchResult==NULL
					   && !((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode());
	}
}
///////////////////////////////////////////////////////////////////////
void CViewArchive::ShowDlgSearchResult()
{
	m_pDlgSearchResult = new CDlgSearchResult(this);
}
//////////////////////////////////////////////////////////////////////////////////
BOOL CViewArchive::PreTranslateMessage(MSG* pMsg) 
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
	return CViewObjectTree::PreTranslateMessage(pMsg);
}
//////////////////////////////////////////////////////////////////////////////////
BOOL CViewArchive::OnToolTipNeedText(UINT id, NMHDR * pNMHDR, LRESULT * pResult)
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
			CString sF;
			sF.LoadString(IDS_TOOLTIP_ARCHIV);
			m_sTipText.Format(sF,
				m_pArchivToolTip->GetName(),
				(DWORD)(m_pArchivToolTip->BenutzterPlatz()/(1024*1024)),
				m_pArchivToolTip->SizeInMB()
				);
			pTTT->lpszText = (LPTSTR)LPCTSTR(m_sTipText);
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
	if (m_ToolTip.m_hWnd == NULL)			// do not create it
	{
		m_ToolTip.m_hWnd = pNMHDR->hwndFrom;// set it only

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
		if (CSkinDialog::UseGlobalFonts())
		{
			m_ToolTip.SetFont(CSkinDialog::GetDialogItemGlobalFont());
		}
	}
	return bHandledNotify;
}
////////////////////////////////////////////////////////////////////////////
void CViewArchive::OnMouseMove(UINT nFlags, CPoint point) 
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
	CViewObjectTree::OnMouseMove(nFlags, point);
}
////////////////////////////////////////////////////////////////////////////
CIPCArchivRecord* CViewArchive::HittestArchivToolTip(CPoint pt)
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
					CServer* pServer = GetDocument()->GetServer(LOWORD(theCtrl.GetItemData(hServer)));
					if (pServer && pServer->IsConnectedPlay())
					{
						CIPCDatabaseIdipClient* pDataBase = pServer->GetDatabase();
						if(pDataBase)
						{
							pRecord = pDataBase->GetArchive(LOWORD(theCtrl.GetItemData(hItem)));
						}
					}
				}
			}
		}
	}
	return pRecord;
}
////////////////////////////////////////////////////////////////////////////
void CViewArchive::OnUpdateSequenceOpen(CCmdUI* pCmdUI) 
{
	if (m_bBackupMode)
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	if (((CMainFrame*)theApp.m_pMainWnd)->IsSyncMode())
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	pCmdUI->Enable(IsSequenceItem(GetTreeCtrl().GetSelectedItem()));
}
////////////////////////////////////////////////////////////////////////////
void CViewArchive::OnUpdateDelete(CCmdUI* pCmdUI) 
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
void CViewArchive::OnArchivEmpty() 
{
	OnDelete();
}
////////////////////////////////////////////////////////////////////////////
void CViewArchive::OnUpdateArchivEmpty(CCmdUI* pCmdUI) 
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
void CViewArchive::Redraw()
{
	if (m_bNeedsRedraw)
	{
		CRect rect;
		GetTreeCtrl().GetClientRect(rect);
		GetTreeCtrl().InvalidateRect(rect);
		m_bNeedsRedraw = FALSE;
	}
}
////////////////////////////////////////////////////////////////////////////
void CViewArchive::OnArchivBackup() 
{
	if (!theApp.AllowBackup())
	{
		return;
	}

	HTREEITEM hItemLocal = NULL;
	HTREEITEM hParentLocal = NULL;
	CIdipClientDoc* pDoc = GetDocument();
	CServer* pServer = NULL;
	
	
	//only allow backup if localhost is connected
	CServer* pLocalServer = NULL;
	pLocalServer = pDoc->GetLocalServer();


	if (m_bBackupMode)
	{
		hItemLocal = GetTreeCtrl().GetRootItem();
		while (hItemLocal)
		{
			pServer = pDoc->GetServer(LOWORD(GetTreeCtrl().GetItemData(hItemLocal)));
			if (pServer && pServer->IsInBackupMode())
			{
				hParentLocal = hItemLocal;
				break;
			}
			else
			{
				pServer = NULL;
				hItemLocal = GetTreeCtrl().GetNextSiblingItem(hItemLocal);
			}
		}
	}
	else
	{
		//for backup only get the local server if connected
		//no backup on other connected servers possible anymore
		hItemLocal = GetTreeCtrl().GetRootItem();
		
		//TKR absolete nur noch backup auf lokeler Station möglich
		//hItem = GetTreeCtrl().GetSelectedItem();

		while (hItemLocal)
		{
			hParentLocal = hItemLocal;
			hItemLocal = GetTreeCtrl().GetParentItem(hItemLocal);
		}
		if (hParentLocal==NULL)
		{
			m_bBackupMode = FALSE;
			return;
		}
		pServer = pDoc->GetServer(LOWORD(GetTreeCtrl().GetItemData(hParentLocal)));

		if(pServer != pLocalServer)
		{
			m_bBackupMode = FALSE;
			return;
		}
	}

	if (pServer==NULL)
	{
		m_bBackupMode = FALSE;
		return;
	}
	if (   pServer->GetDatabase()
		 && pServer->GetDatabase()->GetVersion()<3)
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
		HTREEITEM hArchiv = GetTreeCtrl().GetChildItem(hParentLocal);
		while (hArchiv)
		{
			SetCheckRecurse(hArchiv,TREE_CONTROL_CHECK_OFF);
			hArchiv = GetTreeCtrl().GetNextSiblingItem(hArchiv);
		}
#ifdef _ROOT_CHECK_BOXES
		SetCheckRecurse(hParent,TREE_CONTROL_CHECK_OFF, FALSE);
#endif
		m_bBackupMode = FALSE;
		pServer->SetBackupMode(FALSE);
		// Enable tree again, if disabled
		EnableWindow(TRUE);
		pDoc->UpdateAllViews(this,VDH_BACKUP,(CObject*)NULL);
	}
	else
	{
		HTREEITEM hArchiv = GetTreeCtrl().GetChildItem(hParentLocal);
		while (hArchiv)
		{
			int i=0,is=0;
			GetTreeCtrl().GetItemImage(hArchiv,i,is);
			if (i != _IMAGE_ARCH_BACKUP)
			{
				SetCheckRecurse(hArchiv,TREE_CONTROL_UNCHECKED, (pServer->CanSplittingBackup() == FALSE));
			}
			hArchiv = GetTreeCtrl().GetNextSiblingItem(hArchiv);
		}
#ifdef _ROOT_CHECK_BOXES
		SetCheckRecurse(hParentLocal, TREE_CONTROL_UNCHECKED, FALSE);
#endif
		m_bBackupMode = TRUE;
		pServer->SetBackupMode(TRUE);

		//now switch the localhost in archiv tree to the selected tree (jump to localhost)
		HTREEITEM hItem = NULL;
		hItem = GetTreeCtrl().GetRootItem();
		GetTreeCtrl().Select(hItem, TVGN_FIRSTVISIBLE);
		GetTreeCtrl().Expand(hItem, TVE_EXPAND);

		pDoc->UpdateAllViews(this,VDH_BACKUP,(CObject*)pServer);
	}
}
////////////////////////////////////////////////////////////////////////////
void CViewArchive::OnUpdateArchivBackup(CCmdUI* pCmdUI) 
{
	BOOL bEnable = (   theApp.AllowBackup()
					&& theApp.IsValidUserLoggedIn()
					&& (((CMainFrame*)AfxGetMainWnd())->IsSyncMode() == FALSE)
					);
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
				CIdipClientDoc* pDoc = GetDocument();
				CServer* pServer = pDoc->GetServer(LOWORD(GetTreeCtrl().GetItemData(hParent)));

				if (pServer==NULL)
				{
					bEnable = FALSE;
				}
				else
				{
					if (   pServer->IsConnectedDatabase()
						&& pServer->GetDatabase()->GetVersion()<3)
					{
						bEnable = FALSE;
					}
					else
					{
						//only allow backup only if localhost is connected
						CServer* pLocalServer = NULL;
						pLocalServer = pDoc->GetLocalServer();
						if(!pLocalServer)
						{
							bEnable = FALSE;
						}

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
void CViewArchive::OnArchivDelete() 
{
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();

	if (IsArchivItem(hItem))
	{
		HTREEITEM hServer = GetServerItem(hItem);
		CServer* pServer = GetServer(hServer);
		if (pServer && !pServer->IsDTS())
		{
			CIPCArchivRecord* pArchive = GetArchiveRecord(hItem);
			if (pArchive && pArchive->IsBackup())
			{
				CString sM;
				sM.Format(IDP_ASK_DELETE_ARCHIV,pArchive->GetName());
				if (IDYES==AfxMessageBox(sM,MB_ICONQUESTION|MB_YESNO))
				{
					CIPCDatabaseIdipClient* pDatabase = pServer->GetDatabase();
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
void CViewArchive::OnUpdateArchivDelete(CCmdUI* pCmdUI) 
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
void CViewArchive::OnDateSequencesDelete() 
{
	OnDelete();
}
////////////////////////////////////////////////////////////////////////////
void CViewArchive::OnUpdateDateSequencesDelete(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	if (theApp.IsReadOnlyModus() == FALSE)
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
void CViewArchive::AddAllBackupSequencesExcept(WORD wServerID, CIPCSequenceRecord* pSequence)
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
							if (GetCheck(hSequence))
							{
								GetDocument()->UpdateAllViews(this, VDH_BACKUP_ADD_SEQUENCE, (CObject*)GetSequenceRecord(hSequence));
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
void CViewArchive::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CMainFrame* pMF = (CMainFrame*)AfxGetMainWnd();

	if (m_bBackupMode || pMF->IsSyncMode())
	{
		CPoint pt;
		GetCursorPos(&pt);
		HTREEITEM hClicked;
		UINT nFlags;

		ScreenToClient(&pt);
		CTreeCtrl &theTree = GetTreeCtrl();

		hClicked = theTree.HitTest(pt,&nFlags);

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
					hItem = theTree.GetParentItem(hItem);
				}
				if (hParent)
				{
					CServer* pServer = GetDocument()->GetServer(LOWORD(theTree.GetItemData(hParent)));
					if (pServer && pServer->CanSplittingBackup())
					{
						bRecursiv = FALSE;
					}
				}
			}
#ifdef _ROOT_CHECK_BOXES
			if (IsServerItem(hClicked))
			{
				if (bRecursiv)
				{
					bRecursiv = -2;
				}
				else
				{
					bRecursiv = -1;
				}
			}
#endif
			BOOL bCheck = GetCheck(hClicked);
			if (bCheck)
			{
				// uncheck it
				SetCheckRecurse(hClicked,TREE_CONTROL_UNCHECKED, bRecursiv);
			}
			else
			{
				// check it
				// If in BackupMode only if server can no split backup
				SetCheckRecurse(hClicked,TREE_CONTROL_CHECKED, bRecursiv);
			}
			bCheck = !bCheck;

#ifdef _ROOT_CHECK_BOXES
			if (!IsServerItem(hClicked))
			{
				HTREEITEM hParent = theTree.GetParentItem(hClicked);
				while (hParent)
				{
					HTREEITEM hItem = theTree.GetNextItem(hParent, TVGN_CHILD);
					BOOL bAny = FALSE;
					while (hItem)
					{
						if (GetCheck(hItem))
						{
							bAny = TRUE;
							break;
						}
						hItem = theTree.GetNextItem(hItem, TVGN_NEXT);
					}
					SetCheck(hParent, bAny);
					hParent = theTree.GetParentItem(hParent);
				}
			}
#endif

			if (m_bBackupMode)
			{
				CIdipClientDoc* pDoc = GetDocument();
				if (IsSequenceItem(hClicked))
				{
					// it's a sequence
					pDoc->UpdateAllViews(this,
						bCheck ? VDH_BACKUP_ADD_SEQUENCE : VDH_BACKUP_DEL_SEQUENCE,
						(CObject*)GetSequenceRecord(hClicked));
				}
				else if (IsDateItem(hClicked))
				{
					// it's a date
					HTREEITEM hItem = theTree.GetNextItem(hClicked, TVGN_CHILD);
					while (hItem)
					{
						pDoc->UpdateAllViews(this,
							bCheck ? VDH_BACKUP_ADD_SEQUENCE : VDH_BACKUP_DEL_SEQUENCE,
							(CObject*)GetSequenceRecord(hItem));
						hItem = GetTreeCtrl().GetNextItem(hItem, TVGN_NEXT);
					}
				}
				else if (IsArchivItem(hClicked))
				{
					// it's an archive
					CObject* pHint = (CObject*)GetArchiveRecord(hClicked);
					pDoc->UpdateAllViews(this,
						bCheck ? VDH_BACKUP_ADD_ARCHIVE : VDH_BACKUP_DEL_ARCHIVE,
						pHint);
				}
				else if (IsServerItem(hClicked))
				{
#ifdef _ROOT_CHECK_BOXES
					HTREEITEM hItem = theTree.GetNextItem(hClicked, TVGN_CHILD);
					while (hItem)
					{
						bCheck = GetCheck(hItem);
						pDoc->UpdateAllViews(this,
							bCheck ? VDH_BACKUP_ADD_ARCHIVE : VDH_BACKUP_DEL_ARCHIVE,
							(CObject*)GetArchiveRecord(hItem));
						hItem = theTree.GetNextItem(hItem, TVGN_NEXT);
					}
#endif
				}
			}
			if (pMF->IsSyncMode())
			{
				if (IsArchivItem(hClicked))
				{
					WORD wArchive = LOWORD(theTree.GetItemData(hClicked));
					WORD wServer = LOWORD(theTree.GetItemData(GetServerItem(hClicked)));
					if (bCheck)
						pMF->GetSyncCoolBar()->AddArchive(wServer,wArchive);
					else
						pMF->GetSyncCoolBar()->DeleteArchive(wServer,wArchive,TRUE);
				}
#ifdef _ROOT_CHECK_BOXES
				else if (IsServerItem(hClicked))
				{
					WORD wServer = LOWORD(theTree.GetItemData(hClicked));
					HTREEITEM hItem = theTree.GetNextItem(hClicked, TVGN_CHILD);
					BOOL bAny = FALSE;
					CSyncCoolBar*pSCB = pMF->GetSyncCoolBar();
					while (hItem)
					{
						bCheck = GetCheck(hItem);
						if (bCheck) bAny = TRUE;
						WORD wArchive = LOWORD(theTree.GetItemData(hItem));
						if (bCheck)
							pSCB->AddArchive(wServer,wArchive);
						else
							pSCB->DeleteArchive(wServer,wArchive,TRUE);

						hItem = theTree.GetNextItem(hItem, TVGN_NEXT);
					}
				}
#endif
			}
		}
	}
	
	*pResult = 0;
}
////////////////////////////////////////////////////////////////////////////
CServer* CViewArchive::GetServer(HTREEITEM hServer)
{
	if (IsServerItem(hServer))
	{
		WORD wServerID = LOWORD(GetTreeCtrl().GetItemData(hServer));
		CServer* pServer = GetDocument()->GetServer(wServerID);
		if (pServer && pServer->IsConnectedPlay())
		{
			return pServer;
		}
	}
	return NULL;
}
////////////////////////////////////////////////////////////////////////////
CIPCSequenceRecord* CViewArchive::GetSequenceRecord(HTREEITEM hSequence)
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
			CServer* pServer = GetDocument()->GetServer(wServerID);
			if (pServer)
			{
				pSequence = pServer->GetCIPCSequenceRecord(wArchiv, wSequence);
			}
		}
	}
	return pSequence;
}
////////////////////////////////////////////////////////////////////////////
CIPCArchivRecord* CViewArchive::GetArchiveRecord(HTREEITEM hArchiv)
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

			CServer* pServer = GetDocument()->GetServer(wServerID);
			if (pServer)
			{
				pArchiv = pServer->GetCIPCArchivRecord(wArchiv);
			}
		}
	}
	return pArchiv;
}
////////////////////////////////////////////////////////////////////////////
BOOL CViewArchive::ShowSyncCheckBoxes(BOOL bShow /*= TRUE*/)
{
    CTreeCtrl &theCtrl = GetTreeCtrl();
	for (HTREEITEM hServer = theCtrl.GetRootItem();hServer;hServer = theCtrl.GetNextSiblingItem(hServer))
	{
#ifdef _ROOT_CHECK_BOXES
		BOOL bAny = FALSE;
#endif
		for (HTREEITEM hArchiv = theCtrl.GetChildItem(hServer);hArchiv;hArchiv = theCtrl.GetNextSiblingItem(hArchiv))
		{
			int i=0,is=0;
			theCtrl.GetItemImage(hArchiv,i,is);
			if (   (i == _IMAGE_ARCH_RING)
				|| (i == _IMAGE_ARCH_ALARM)
				|| (i == _IMAGE_ARCH_BACKUP)
				|| (i == _IMAGE_ARCH_SEARCH)
				)
			{
#ifdef _ROOT_CHECK_BOXES
				bAny = TRUE;
#endif
				SetCheckRecurse(hArchiv,
					bShow?TREE_CONTROL_UNCHECKED:TREE_CONTROL_CHECK_OFF,
					FALSE);
				if (bShow)
				{
					theCtrl.Expand(hArchiv,TVE_COLLAPSE);

					//bei SyncPlay schon geöffnete Archive im ArchivTree anhaken
					WORD wServerID = LOWORD(theCtrl.GetItemData(hServer));
					WORD wArchiv   = LOWORD(theCtrl.GetItemData(hArchiv));

					CServer* pServer = GetDocument()->GetServer(wServerID);
					if (pServer)
					{
						CViewIdipClient* pViewIdip = theApp.GetMainFrame()->GetViewIdipClient();
						if(pViewIdip)
						{
							CWndPlay*pDW = pViewIdip->GetWndPlay(wServerID,wArchiv, 0);
							if(pDW)
							{
								SetCheck(hArchiv,TRUE);
								theCtrl.SelectItem(hArchiv);
								if (theApp.GetMainFrame()->GetSyncCoolBar())
								{
									theApp.GetMainFrame()->GetSyncCoolBar()->AddArchive(wServerID,wArchiv);
								}
							}
						}
					}
				}
			}
		}
#ifdef _ROOT_CHECK_BOXES
		if (bAny)
		{
			SetCheckRecurse(hServer,
				bShow?TREE_CONTROL_UNCHECKED:TREE_CONTROL_CHECK_OFF,
				FALSE);
		}
#endif
	}

	return TRUE;
}
////////////////////////////////////////////////////////////////////////////
void CViewArchive::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == theApp.GetMainFrame()->GetOneSecTimerID())
	{
		Redraw();
	}
	
	CViewObjectTree::OnTimer(nIDEvent);
}
////////////////////////////////////////////////////////////////////////////
int CViewArchive::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CViewObjectTree::OnCreate(lpCreateStruct) == -1)
		return -1;
							   
	CString sText;
	sText.LoadString(IDS_ARCHIVE);
	SetWindowText(sText);
	m_btnTitle.SetWindowText(sText);

	EnableToolTips(TRUE);
	
	return 0;
}
////////////////////////////////////////////////////////////////////////////
void CViewArchive::OnDestroy() 
{
	m_ToolTip.m_hWnd = NULL;// it was not created here, so set to NULL
							// otherwise it is destroyed here
	CViewObjectTree::OnDestroy();
}
