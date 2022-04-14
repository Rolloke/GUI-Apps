// ViewCamera.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "MainFrm.h"

#include "ViewCamera.h"
#include "ViewIdipClient.h"

#include "IdipClientDoc.h"
#include "CIPCInputIdipClient.h"
#include "CIPCAudioIdipClient.h"
#include "Server.h"

#include "WndLive.h"
#include "WndHtml.h"

#include "images.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

WORD		CViewControllItems::gm_wLocalServerID = SECID_NO_SUBID;
CPtrList	CViewControllItems::gm_LocalHTMLmappings;

CImageList	CViewControllItems::gm_Images;
CImageList	CViewControllItems::gm_States;


IMPLEMENT_DYNCREATE(CViewControllItems, CViewObjectTree)

CViewControllItems::CViewControllItems()
{
	m_pDragImage         = NULL;
	m_bDraggedInto       = FALSE;
	if (theApp.IsReadOnlyModus())
	{
		m_eVisibility = OTVS_Never;
	}
}

CViewControllItems::~CViewControllItems()
{
	WK_DELETE(m_pDragImage);

	DeleteLocalHTMLmappings();
}

BEGIN_MESSAGE_MAP(CViewControllItems, CViewObjectTree)
	//{{AFX_MSG_MAP(CViewControllItems)
	ON_COMMAND(ID_VIDEO_ALARM_SWITCH, OnVideoAlarmSwitch)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_ALARM_SWITCH, OnUpdateVideoAlarmSwitch)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewControllItems message handlers
void CViewControllItems::OnInitialUpdate() 
{
	CViewObjectTree::OnInitialUpdate();
	CTreeCtrl&  theTree = GetTreeCtrl();
	
	theTree.SetIndent(0);

	if (gm_Images.GetSafeHandle() == NULL)
	{
		gm_Images.Create(IDB_IMAGES,_IMAGE_WIDTH,0,RGB(0,255,255));
	}
	theTree.SetImageList(&gm_Images,TVSIL_NORMAL);

	if (gm_States.GetSafeHandle() == NULL)
	{
		gm_States.Create(IDB_STATES,14,0,RGB(0,255,255));
	}
	theTree.SetImageList(&gm_States,TVSIL_STATE);

}
/////////////////////////////////////////////////////////////////////////////
BOOL CViewControllItems::SetCheck(HTREEITEM hItem,BOOL bFlag)
{
	BOOL bRet = CViewObjectTree::SetCheck(hItem, bFlag, FALSE);

	CSecID id(GetTreeCtrl().GetItemData(hItem));
	if (id.IsOutputID())
	{
		UpdateCameraPage(hItem);
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CViewControllItems::CheckParent(HTREEITEM hItem)
{
	CTreeCtrl&  theTree = GetTreeCtrl();
	HTREEITEM hChild, hParent = theTree.GetParentItem(hItem);
	if (hParent)
	{
		BOOL bAnyOn = FALSE;
		hChild = theTree.GetNextItem(hParent, TVGN_CHILD);
		while (hChild)
		{
			bAnyOn = GetCheck(hChild);
			if (bAnyOn) break;
			hChild = theTree.GetNextItem(hChild, TVGN_NEXT);
		}
		SetCheck(hParent, bAnyOn);
	}
	return hParent;
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CViewControllItems::InsertServer(const CServer* pServer)
{
	CTreeCtrl&  theTree = GetTreeCtrl();
	TV_INSERTSTRUCT tvis;
	HTREEITEM hServer = NULL;
	if (pServer == NULL)
	{
		return hServer;
	}
	CString s = pServer->GetFullName();

	tvis.hParent     = TVI_ROOT;
	tvis.item.lParam = MAKELONG(pServer->GetID(), pServer->GetHostID().GetSubID());
	
	if (pServer->IsLocal())
	{
		tvis.hInsertAfter = TVI_FIRST;
		gm_wLocalServerID = pServer->GetID();
		if (s.IsEmpty())
		{
			s.LoadString(IDS_LOCAL_SERVER);
		}
	}
	else
	{
		tvis.hInsertAfter      = TVI_FIRST;
		HTREEITEM hItem, hLast = TVI_FIRST;
		hItem = theTree.GetNextItem(tvis.hParent, TVGN_CHILD);
		while (hItem)
		{
			WORD wItemData = HIWORD(theTree.GetItemData(hItem));
			if (  (wItemData == SECSUBID_LOCAL_HOST)	// local host always first
				|| (wItemData <= HIWORD(tvis.item.lParam))
				)
			{
				hLast = hItem;
				tvis.hInsertAfter = hItem;
			}
			else // sort before this, so behind last
			{
				tvis.hInsertAfter = hLast;
				break;
			}
			hItem = theTree.GetNextItem(hItem, TVGN_NEXT);
		}
	}

	tvis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	
	tvis.item.cchTextMax = s.GetLength();
	tvis.item.pszText    = (LPTSTR)LPCTSTR(s);
	    
	tvis.item.iImage = _IMAGE_HOSTBASE + ((pServer->GetID()-1) % _IMAGE_NR_HOST_COLORS);
	tvis.item.iSelectedImage = tvis.item.iImage;

	hServer = theTree.InsertItem(&tvis);

//	if (!pServer->IsDTS())
//	{
		// Melder
		// check for SpecialReceiver
//		s.LoadString(IDS_INPUT);
//		tvis.hParent = hServer;
//		tvis.hInsertAfter = TVI_LAST;
	//		tvis.item.mask = TVIF_TEXT | TVIF_PARAM;
	//		tvis.item.iImage = 0;
	//		tvis.item.iSelectedImage = 0;
//		tvis.item.cchTextMax = s.GetLength();
//		tvis.item.pszText    = (LPTSTR)LPCTSTR(s);
//		GetTreeCtrl().InsertItem(&tvis);

		// Kameras
//		s.LoadString(IDS_CAMERAS);
//		tvis.hParent = hServer;
//		tvis.item.cchTextMax = s.GetLength();
//		tvis.item.pszText    = (LPTSTR)LPCTSTR(s);
//		GetTreeCtrl().InsertItem(&tvis);

		// Relais
		// check for SpecialReceiver
//		s.LoadString(IDS_RELAY);
//		tvis.hParent = hServer;
//		tvis.item.cchTextMax = s.GetLength();
//		tvis.item.pszText    = (LPTSTR)LPCTSTR(s);
//		GetTreeCtrl().InsertItem(&tvis);
//	}

	UpdateHostPage(hServer, TRUE);

	return hServer;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CViewControllItems::DeleteServer(DWORD dwID)
{
	BOOL bReturn = FALSE;
	CTreeCtrl&  theTree = GetTreeCtrl();
	HTREEITEM hItem = theTree.GetRootItem();
	while (hItem)
	{
		if (dwID==LOWORD(theTree.GetItemData(hItem)))
		{
			bReturn = TRUE;
			break;
		}
		hItem = theTree.GetNextSiblingItem(hItem);
	}

	CIdipClientDoc* pDoc = GetDocument();
	if (bReturn && pDoc)
	{
		ASSERT(IsServerItem(hItem));
		WORD wServer, wHost;
		wServer = GetSelectedServer(&wHost, hItem);

		UpdateHostPage(hItem, FALSE);
		if (gm_LocalHTMLmappings.GetCount())
		{
			POSITION pos = gm_LocalHTMLmappings.GetHeadPosition();
			while (pos)
			{
				DWORD *pdwMapping = (DWORD*)gm_LocalHTMLmappings.GetNext(pos);
				if (HOSTID_OF(pdwMapping[MAPPING_SERVER]) == wHost)
				{
					CSecID ServerID(wServer, gm_wLocalServerID);
					CString strID;
					DWORD dwUpdate = SEARCH_BY_ID|SEARCH_BY_SERVER|SEARCH_IN_ALL|HTML_ELEMENT_UPDATE_STATE|HTML_ELEMENT_DISCONNECT;
					strID.Format(_T("%08x"), pdwMapping[MAPPING_EL_ID]);
					pDoc->UpdateMapElement(ServerID, strID, dwUpdate);
				}
			}
		}

		theTree.DeleteItem(hItem);
	}
	CheckVisibility();
	if (dwID == gm_wLocalServerID)
	{
		gm_wLocalServerID = SECID_NO_SUBID;
	}
	
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CViewControllItems::InsertOutput(HTREEITEM hServer, const CIPCOutputRecord& rec)
{
	if (hServer==NULL)
	{
		return NULL;
	}
	HTREEITEM hOutput = NULL;
	TV_INSERTSTRUCT tvis;
	CString s = rec.GetName();

	tvis.hParent = hServer;

	tvis.hInsertAfter = TVI_LAST;
	tvis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvis.item.lParam = rec.GetID().GetID();
	tvis.item.cchTextMax = s.GetLength();
	tvis.item.pszText = (LPTSTR)LPCTSTR(s);

	int i = GetImageIndex(rec);

	if (   (i!=_IMAGE_OFF) 
		&& (i!=_IMAGE_ERROR)
		)
	{
		tvis.item.iImage = i;
		tvis.item.iSelectedImage = i;

		hOutput = GetTreeCtrl().InsertItem(&tvis);
		if ( (i==_IMAGE_CAMERA) || (i==_IMAGE_PTZ))
		{
			SetCheck(hOutput,FALSE);
		}
	}

	if (hOutput)
	{
		UpdateCameraPage(hOutput);
	}

	return hOutput;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CViewControllItems::ActualizeOutput(HTREEITEM hOutput, const CIPCOutputRecord& rec)
{
	int i = GetImageIndex(rec);
	GetTreeCtrl().SetItemImage(hOutput,i,i);
	GetTreeCtrl().SetItemText(hOutput,rec.GetName());

	UpdateCameraPage(hOutput);
	
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CViewControllItems::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	WORD cause	  = LOWORD(lHint);
	WORD wServerID = HIWORD(lHint);
	CSecID id; 

	switch (cause)
	{
	case VDH_REMOVE_SERVER:
		DeleteServer(wServerID);
		break;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewControllItems::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	EndDragg();
	CPoint pt;
	GetCursorPos(&pt);
	HTREEITEM hItem;
	UINT nFlags;

	ScreenToClient(&pt);
	hItem = GetTreeCtrl().HitTest(pt,&nFlags);

	if (hItem && ((nFlags & TVHT_ONITEMICON) || (nFlags & TVHT_ONITEMLABEL)))
	{
		// right click on
		GetTreeCtrl().SelectItem(hItem);
		ClientToScreen(&pt);
		ContextMenu(pt,hItem);
	}
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CViewControllItems::ContextMenu(const CPoint& pt, HTREEITEM hItem)
{
	CTreeCtrl&  theTree = GetTreeCtrl();
	CSecID id = (DWORD)theTree.GetItemData(hItem);
	CMenu menu;
	CSkinMenu* pPopup = NULL;

	menu.LoadMenu(IDR_MENU_LIVE);

	if (id.IsInputID())
	{
		pPopup = (CSkinMenu*)menu.GetSubMenu(SUB_MENU_DETECTOR);
		// TODO add MNO menu
		CIdipClientDoc* pDoc = GetDocument();
		HTREEITEM hParent = theTree.GetParentItem(hItem);
		CServer* pServer = pDoc->GetServer(LOWORD(theTree.GetItemData(hParent)));

		if (pServer && pServer->IsConnectedLive())
		{
			CIPCInputIdipClient* pInput = pServer->GetInput();
			if (pInput->HasAlarmOffSpanFeature())
			{
				CString s;
				s.LoadString(IDS_INPUT_DEACTIVATE);
				pPopup->AppendMenu(MF_STRING,ID_INPUT_DEACTIVATE,s);
			}
		}

	}
	else if (id.IsOutputID())
	{
		CIdipClientDoc* pDoc = GetDocument();
		HTREEITEM hParent = theTree.GetParentItem(hItem);
		CServer* pServer = pDoc->GetServer(LOWORD(theTree.GetItemData(hParent)));

		if (pServer && pServer->IsConnectedLive())
		{
			CIPCOutputIdipClient* pOutput = pServer->GetOutput();
			const CIPCOutputRecord rec = pOutput->GetOutputRecordFromSecID(id);

			if (rec.IsCamera())
			{
				CViewIdipClient*pVIC = theApp.GetMainFrame()->GetViewIdipClient();
				pVIC->LockSmallWindows(_T(__FUNCTION__));
				CWndLive* pDW = pVIC->GetWndLive(pServer->GetID(),id);
				if (pDW)
				{
					pVIC->SetWndSmallCmdActive(pDW);
					pVIC->UnlockSmallWindows();
					pDW->PopupMenu(pt);
					return;
				}
				pVIC->UnlockSmallWindows();
				pPopup = (CSkinMenu*)menu.GetSubMenu(SUB_MENU_PICTURES);
			}
			else
			{
				pPopup = (CSkinMenu*)menu.GetSubMenu(SUB_MENU_RELAIS);
			}
		}
	}
	else if (id.IsMediaID())
	{
		int nImage, nSelected;
		theTree.GetItemImage(hItem, nImage, nSelected);
		switch (nImage)
		{
			case _IMAGE_AUDIO_PLAY: case _IMAGE_AUDIO_PLAY_DEF:
			{
				pPopup = (CSkinMenu*)menu.GetSubMenu(SUB_MENU_AUDIO_PLAY);
				pPopup->DeleteMenu(ID_AU_PROPERTIES, MF_BYCOMMAND);
				int nSeparator = pPopup->GetMenuItemCount()-1;
				if (pPopup->GetMenuItemID(nSeparator) == 0)
				{
					pPopup->DeleteMenu(nSeparator, MF_BYPOSITION);
				}
			}break;
			case _IMAGE_AUDIO_REC: case _IMAGE_AUDIO_REC_DEF:
				pPopup = (CSkinMenu*)menu.GetSubMenu(SUB_MENU_AUDIO_REC);
				break;
		}
	}
	else
	{
		HTREEITEM hParent = theTree.GetParentItem(hItem);
		if (hParent == NULL)
		{
			// really on top a server
			pPopup = (CSkinMenu*)menu.GetSubMenu(SUB_MENU_HOST);
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
				sHost.Format(_T("%s (%s)"), str, theTree.GetItemText(hItem));
				pM->ModifyMenu(nID, MF_BYPOSITION, ID_DISCONNECT_LOW+id.GetSubID(), sHost);
			}
#ifdef _DTS
			pPopup->DeleteMenu(ID_VIDEO_ALARM_SWITCH,MF_BYCOMMAND);
			pPopup->DeleteMenu(ID_HOST_CAMERA_MAP,MF_BYCOMMAND);
			pPopup->DeleteMenu(ID_HOST_LOAD_SEQUENCE,MF_BYCOMMAND);
			pPopup->DeleteMenu(ID_HOST_SAVE_SEQUENCE,MF_BYCOMMAND);
			pPopup->DeleteMenu(ID_SEPARATOR,MF_BYCOMMAND);
#endif
		}
	}

	if (pPopup)
	{
		COemGuiApi::DoUpdatePopupMenu(AfxGetMainWnd(), pPopup);
		pPopup->ConvertMenu(TRUE);
		pPopup->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y, AfxGetMainWnd());		
		pPopup->ConvertMenu(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewControllItems::UpdateCameraPage(HTREEITEM hItem,
											  WORD wFlags      //=VDH_FLAGS_UPDATE
											  )
{
	if (hItem)
	{
		CTreeCtrl &theTree = GetTreeCtrl();
		CIdipClientDoc *pDoc = GetDocument();
		HTREEITEM hParent = theTree.GetParentItem(hItem);
		if (hParent == NULL) return;
		CServer* pServer = pDoc->GetServer(LOWORD(theTree.GetItemData(hParent)));
		if (theTree.GetSelectedItem() == hItem) wFlags |= VDH_FLAGS_SELECTED;

		if (pServer)
		{
			WORD wServerID = pServer->GetID();
			CSecID id(theTree.GetItemData(hItem));
			BOOL bIsOn = IsOnLocalHTMLpage(pServer->GetHostID().GetSubID(), id);
#ifdef _DEBUG
			//TODO! RKE: activate at 5.1
			if (bIsOn == TRUE)
			{
				wServerID = gm_wLocalServerID;
			}
			else if (bIsOn)
			{
				wServerID = HIWORD(bIsOn);
			}
#else		
			if (bIsOn)
			{
				wServerID = gm_wLocalServerID;
			}
#endif
			if (id.IsInputID())
			{
				CIPCInputIdipClient * pInput = pServer->GetInput();
				CIPCInputRecord * pRecI  = pInput->GetInputRecordPtrFromSecID(id);
				if (pRecI)
				{
					CHtmlPageHint Hint(pRecI, pServer->GetHostID().GetSubID(), pServer->GetID(), wFlags);
					pDoc->UpdateAllViews(this, MAKELONG(VDH_CAM_MAP, wServerID), &Hint);
				}
			}
			else if (id.IsOutputID())
			{
				CIPCOutputIdipClient* pOutput = pServer->GetOutput();
				CIPCOutputRecord* pRecO   = pOutput->GetOutputRecordPtrFromSecID(id);
				if (pRecO)
				{
					if (pRecO->IsCamera())
					{
						WORD wFlags = pRecO->GetFlags();
						if (GetCheck(hItem))
						{
							wFlags |= OREC_CAMERA_VISIBLE;
						}
						else
						{
							wFlags &= ~OREC_CAMERA_VISIBLE;
						}
						pRecO->SetFlags(wFlags);
					}
					CHtmlPageHint Hint(pRecO, pServer->GetHostID().GetSubID(), pServer->GetID(), wFlags);
					pDoc->UpdateAllViews(this, MAKELONG(VDH_CAM_MAP, wServerID), &Hint);
				}
			}
			else if (id.IsMediaID())
			{
				WORD wServer, wHost;
				CIPCMediaRecord*pRecM = GetSelectedItemsMediaRecord(wHost, wServer, hItem);
				if (pRecM)
				{
					CHtmlPageHint Hint(pRecM, wHost, wServer, wFlags);
					pDoc->UpdateAllViews(this, MAKELONG(VDH_CAM_MAP, wServerID), &Hint);
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewControllItems::UpdateHostPage(HTREEITEM hItem, BOOL bConnect)
{
	if (hItem)
	{
		CIdipClientDoc *pDoc = GetDocument();
		CHtmlPageHint Hint(NULL, HIWORD(GetTreeCtrl().GetItemData(hItem)), LOWORD(GetTreeCtrl().GetItemData(hItem)), (WORD)bConnect);

		pDoc->UpdateAllViews(this, MAKELONG(VDH_HOST_MAP, 0), &Hint);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CViewControllItems::IsServerItem(HTREEITEM hItem)
{
	return NULL == GetTreeCtrl().GetParentItem(hItem);
}
////////////////////////////////////////////////////////////////////////////
CServer* CViewControllItems::GetServer(HTREEITEM hServer)
{
	if (IsServerItem(hServer))
	{
		return GetServer(LOWORD(GetTreeCtrl().GetItemData(hServer)));
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CServer* CViewControllItems::GetServer(WORD wServerID)
{
	CIdipClientDoc* pDoc = GetDocument();
	if (pDoc)
	{
		CServer* pServer = pDoc->GetServer(wServerID);
		if (pServer && pServer->IsConnectedLive())
		{
			return pServer;
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CViewControllItems::Actualize(WORD wServerID, CSecID id)
{
	BOOL bCheckVisibility = FALSE;
	HTREEITEM hServer = GetTreeItem(wServerID);
	HTREEITEM hChild = GetTreeItem(wServerID, id.GetID());

	ASSERT(!id.IsInputID());	// only allowed in CViewAlarmList
	ASSERT(!id.IsMediaID());	// only allowed in CViewAudio
	
	CServer* pServer = GetDocument()->GetServer(wServerID);
	if (hServer == NULL)
	{
		hServer = InsertServer(pServer);
		bCheckVisibility = hServer ? TRUE : FALSE;
	}

	if (id.IsOutputID() && pServer)
	{
		CIPCOutputIdipClient* pOutput = pServer->GetOutput();
		if (pOutput)
		{
			CIPCOutputRecord* pRecord = pOutput->GetOutputRecordPtrFromSecID(id);
			if (pRecord)
			{
				if (hChild)
				{
					// found input or output
					CWndLive* pDW = theApp.GetMainFrame()->GetViewIdipClient()->GetWndLive(wServerID, id);
					if (pDW)
					{
						pDW->SetOutputRecord(*pRecord);
					}
					ActualizeOutput(hChild,*pRecord);
				}
				else
				{
					InsertOutput(hServer,*pRecord);
				}
			}
		}
	}

	if (bCheckVisibility)
	{
		CheckVisibility();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewControllItems::Delete(WORD wServerID, CSecID id)
{
	HTREEITEM hChild = GetTreeItem(wServerID, id.GetID());

	if (hChild)
	{
		HTREEITEM hParent = GetTreeCtrl().GetParentItem(hChild);
		GetTreeCtrl().DeleteItem(hChild);
		if (hParent)
		{
			if (!GetTreeCtrl().ItemHasChildren(hParent))
			{
				GetTreeCtrl().DeleteItem(hParent);
				CheckVisibility();
			}
		}
		if (id.IsOutputID())
		{
			// Camera OFF
			GetDocument()->UpdateAllViews(this,MAKELONG(VDH_DELETE_CAMWINDOW,wServerID),
								(CObject*)id.GetID());
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewControllItems::Add(WORD wServerID, CSecID id)
{
	HTREEITEM hChild = GetTreeItem(wServerID,id.GetID());

	if (hChild == NULL)
	{
		// found input or output
		CServer* pServer = GetDocument()->GetServer(wServerID);
		ASSERT(id.IsOutputID());

		CIPCOutputIdipClient* pOutput = pServer->GetOutput();
		if (pOutput)
		{
			CIPCOutputRecord* pRecord = pOutput->GetOutputRecordPtrFromSecID(id);
			if (pRecord)
			{
				InsertOutput(hChild,*pRecord);
			}
		}
	}
	else
	{
		Actualize(wServerID, id);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewControllItems::Select(WORD wServerID, const CString& s)
{
	HTREEITEM hItem = GetTreeItem(wServerID,s);
	if (hItem)
	{
		DWORD dw = GetTreeCtrl().GetItemData(hItem);
		Select(wServerID,hItem,CSecID(dw));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewControllItems::Select(WORD wServerID, CSecID id, BOOL bOn /*= TRUE*/)
{
	HTREEITEM hItem = GetTreeItem(wServerID,id.GetID());
	if (hItem)
	{
		Select(wServerID,hItem,id,bOn);
	}
}
/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CViewControllItems
 Function   : UpdateMapElement
 Description: Function for update of the elements in a HTML camera or host map

 Parameters:   
  ServerID: (E) : subID: serverID of Map, GroupID: external HostID or ServerID  (CSecID)
  s       : (EA): Name or ID of the Item  (CString&)
  dwUpdate: (EA): Update Flags  (DWORD)
  - SEARCH_BY_ID             : the string "s" contains the ID,
                               otherwise it contains the name of the item.
  - SEARCH_BY_SERVER         : the server id is taken from "ServerID".GetGroupID()
                               otherwise it is determined by the HostID (in "ServerID".GetGroupID())
  - SEARCH_IN_INPUT, SEARCH_IN_CAMERAS, SEARCH_IN_RELAYS, SEARCH_IN_AUDIO, SEARCH_IN_HOSTS, SEARCH_IN_ALL:
										 Search limitations
  - HTML_ELEMENT_SELECT      : selects found item in tree viev
  - HTML_ELEMENT_MENU        : calls context menu of the found item
  - HTML_ELEMENT_DISCONNECT  : sets the item Disconnected
  - HTML_ELEMENT_UPDATE_PAGE : Update the HTMLelement states with redraw
  - HTML_ELEMENT_UPDATE_STATE: Update the HTMLelement states
  - HTML_ELEMENT_CHECK_NAME  : compares the name of the element with the treview items name
  - HTML_ELEMENT_NAME_CHANGED: indicates, that the names are different, the treview items name
                               is returned in "s"

 Result type:  (DWORD)
 created: October, 17 2003
 <TITLE UpdateMapElement>
*********************************************************************************************/
DWORD CViewControllItems::UpdateMapElement(CSecID ServerID, CString &s, DWORD& dwUpdate)
{
	CTreeCtrl &theTree = GetTreeCtrl();
	if (ServerID == SECID_NO_ID)	// Hosts
	{
		HTREEITEM hServer = theTree.GetRootItem();
		DWORD dwID = SECID_NO_ID;
		CHost* pHost = NULL;
		if (dwUpdate & SEARCH_BY_ID)
		{
			_stscanf(s, _T("%08x"), &dwID);
			while (hServer)
			{
				if (HIWORD(theTree.GetItemData(hServer)) == LOWORD(dwID))
				{
					break;
				}
				hServer = theTree.GetNextSiblingItem(hServer);
			}
			WORD wHost = LOWORD(dwID);
			CHostArray& ha = theApp.GetHosts();
			pHost = ha.GetHost(CSecID(SECID_GROUP_HOST, wHost));
		}
		else
		{
			while (hServer)
			{
				if (theTree.GetItemText(hServer).Find(s) != -1)
				{
					break;
				}
				hServer = theTree.GetNextSiblingItem(hServer);
			}
		}
		if (hServer)
		{
			if (dwUpdate & HTML_ELEMENT_SELECT)
			{
				theTree.SelectItem(hServer);
			}
			else if ((dwUpdate & HTML_ELEMENT_MENU) && (hServer != NULL))
			{
				CPoint pt;
				GetCursorPos(&pt);
				theTree.SelectItem(hServer);
				ContextMenu(pt, hServer);
			}
			return theTree.GetItemData(hServer);
		}
		else if (dwID != SECID_NO_ID)
		{
			if (pHost)
			{
				dwUpdate |= HTML_ELEMENT_DISCONNECT;
			}
			else if (dwUpdate & HTML_ELEMENT_CHECK_NAME)
			{
				dwUpdate |= HTML_ELEMENT_DELETED;
			}
		}
		if (dwUpdate & HTML_ELEMENT_CHECK_NAME && pHost != NULL)
		{
			int nFind = s.Find(_T(":"));
			if (nFind != -1)
			{
				CString sFind = s.Mid(nFind+1);
				CString sItem = pHost->GetName();
				if (sFind != sItem)
				{
					s = sItem;
					dwUpdate |= HTML_ELEMENT_NAME_CHANGED;
				}
			}
		}
	}
	else								// Cameras, Detectors, Relays
	{
		HTREEITEM hItem = NULL;
		CSecID id;
		WORD wServer = ServerID.GetSubID();
		if (ServerID.GetGroupID() != SECID_NO_GROUPID)
		{
			if (dwUpdate & SEARCH_BY_SERVER)
			{
				wServer = ServerID.GetGroupID();
			}
			else
			{
				wServer = GetDocument()->GetServerIDbyHostID(ServerID.GetGroupID(), FALSE);
			}
		}
		if (dwUpdate & SEARCH_BY_ID)
		{
			DWORD dwID = 0;
			_stscanf(s, _T("%08x"), &dwID);
			hItem = GetTreeItem(wServer, dwID);
			id.Set(dwID);
		}
		else
		{
			hItem = GetTreeItem(wServer, s);
		}

		if (dwUpdate & HTML_ELEMENT_GET_STATE_FLAG)
		{
			CServer *pServer = GetServer(wServer);
			if (pServer)
			{
				if (id.IsInputID())
				{
					CIPCInputIdipClient*pInput = pServer->GetInput();
					if (pInput)
					{
						const CIPCInputRecord&rec = pInput->GetInputRecordFromSecID(id);
                        if (rec.GetIsActive())  dwUpdate |= HTML_ELEMENT_ACTIVE;
						if (rec.GetIsEnabled()) dwUpdate |= HTML_ELEMENT_ENABLED;
						if (rec.GetIsOkay())    dwUpdate |= HTML_ELEMENT_OK;
					}
				}
			}
		}

		if (dwUpdate & HTML_ELEMENT_SELECT)
		{
			theTree.SelectItem(hItem);
		}
		if (dwUpdate & HTML_ELEMENT_CHECK_NAME && hItem)
		{
			int nFind = s.Find(_T(":"));
			if (nFind != -1)
			{
				CString sFind = s.Mid(nFind+1);
				CString sItem = theTree.GetItemText(hItem);
				nFind = sFind.Find(_T("@"));
				if (nFind != -1)
				{
					sFind = sFind.Left(nFind);
				}
				// RKE: wenn die HTML Seite buchstaben einer anderen Codepage nicht darstellen kann
				// z.B.: deutsche Umlaute im russischen, so wird aus ü -> u und somit ungleich.
				if (CompareStringRelative(sItem, sFind) < 50)
				{
					s = sItem;
					dwUpdate |= HTML_ELEMENT_NAME_CHANGED;
				}
			}
		}
		if (dwUpdate & HTML_ELEMENT_UPDATE_PAGE)
		{
//			theTree.SelectItem(hItem);
			WORD wFlag = VDH_FLAGS_UPDATE;
			if ((dwUpdate & HTML_ELEMENT_UPDATE_PAGE) == HTML_ELEMENT_UPDATE_STATE)
			{
				wFlag = 0;
			}
			if (dwUpdate & HTML_ELEMENT_ACTIVE)
			{
				wFlag |= VDH_FLAGS_UPDATE;
			}
			if (dwUpdate & HTML_ELEMENT_DISCONNECT)
			{
				wFlag |= VDH_FLAGS_DISCONNECT;
			}
			UpdateCameraPage(hItem, wFlag);
		}
		else if ((dwUpdate & HTML_ELEMENT_MENU) && (hItem != NULL))
		{
			CPoint pt;
			GetCursorPos(&pt);
			theTree.SelectItem(hItem);
			ContextMenu(pt, hItem);
		}
		return (hItem != NULL) ? theTree.GetItemData(hItem) : SECID_NO_ID;
	}
	return SECID_NO_ID;
}
/////////////////////////////////////////////////////////////////////////////
void CViewControllItems::Select(WORD wServerID, HTREEITEM hItem, CSecID id, BOOL bpOn /*= TRUE*/)
{
	GetTreeCtrl().Select(hItem, TVGN_CARET);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CViewControllItems::OnUser(WPARAM wParam, LPARAM lParam) 
{
	CIdipClientDoc* pDoc = GetDocument();
	WORD cause	  = LOWORD(wParam);
	WORD wServerID = HIWORD(wParam);
	CServer* pServer = pDoc->GetServer(wServerID);
	CSecID id = (DWORD)(lParam);
	
	switch (cause)
	{
	case WPARAM_ACTUALIZE:
		Actualize(wServerID,id);
		break;
	case WPARAM_DELETE:
		Delete(wServerID,id);
		break;
	case WPARAM_ADD:
		Add(wServerID,id);
		break;
	case WPARAM_SELECT:
		if (id.IsOutputID())
		{
			Select(wServerID,id);
			theApp.AlarmConnection();
			if (theApp.m_b1PlusAlarm)
			{
				// richtige Camera soll 1 Plus werden
				CWndLive* pDW = theApp.GetMainFrame()->GetViewIdipClient()->GetWndLive(wServerID,id);
				if (pDW)
				{
					theApp.GetMainFrame()->GetViewIdipClient()->SetWndSmallBig(pDW);
				}
			}
			// Set fullscreen mode at begin?
			if (   theApp.m_bFullscreenAlarm
				&& pServer->IsAlarm())
			{
				theApp.GetMainFrame()->ChangeFullScreenModus(TRUE);
			}
		}
		else if(id.IsInputID())
		{
			ShowInformation(GetTreeItem(wServerID,id.GetID()));
			theApp.AlarmConnection();
		}
		else if (id == SECID_NO_ID)
		{
			// wenn 1 plus und 1 plus zu Beginn zurück zu quadratisch
			if (   theApp.m_b1PlusAlarm
				&& theApp.GetMainFrame()->GetViewIdipClient()->IsView1plus())
			{
				TRACE(_T("1 plus aus wegen alarm aus\n"));
				theApp.GetMainFrame()->GetViewIdipClient()->SendMessage(WM_COMMAND,ID_VIEW_3_3);
			}
			// wenn das Fenster nach Alarmende wieder geschlossen werden soll
			if (theApp.m_bCloseAlarmWindow)
			{
				Select(wServerID,pServer->GetAlarmID(),FALSE);
			}
		}
		break;
	case WPARAM_UNSELECT:
		if (id.IsOutputID())
		{
			Select(wServerID,id,FALSE);
		}
		break;

	case WPARAM_INFORMATION:
		ShowInformation(GetTreeItem(wServerID,id.GetID()));
		break;

	case WPARAM_CONTROL_CONNECTED:
		if (pServer)
		{
			pServer->OnControlConnected();
		}
		break;
	case WPARAM_CONTROL_DISCONNECTED:
		if (pServer)
		{
			pServer->OnControlDisconnected();
		}
		if (   theApp.m_b1PlusAlarm
			&& theApp.GetMainFrame()->GetViewIdipClient()->IsView1plus())
		{
			TRACE(_T("1 plus aus wegen control disconnect\n"));
			theApp.GetMainFrame()->GetViewIdipClient()->PostMessage(WM_COMMAND,ID_VIEW_3_3);
		}
		break;

	case WPARAM_CONNECT_THREAD_END:
		if (pServer)
		{
			//show the main menu on alarm connection
			if(     theApp.m_bShowAlarmMenu 
				&&  pServer->IsAlarm())
			{
				theApp.GetMainFrame()->ShowMenu(TRUE);
			}
			pServer->OnConnectThreadEnd();
		}
		break;
	case WPARAM_CONNECTION_RECORD:
		GetDocument()->OnConnectionRecord();
		break;
	}

	return 0L;
}
/////////////////////////////////////////////////////////////////////////////
void CViewControllItems::ShowInformation(HTREEITEM hItem)
{
	if (hItem)
	{
		DWORD dwID = GetTreeCtrl().GetItemData(hItem);
		CDocument* pDoc = GetDocument();
		HTREEITEM hServer = GetTreeCtrl().GetParentItem(hItem);
		pDoc->UpdateAllViews(this,MAKELONG(VDH_INFORMATION,
							 LOWORD(GetTreeCtrl().GetItemData(hServer))),(CObject*)dwID);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewControllItems::OnVideoAlarmSwitch() 
{
	WORD wServerID = GetSelectedServer();

	if (wServerID != SERVERID_NOID)
	{
		CIdipClientDoc* pDoc = GetDocument();
		CServer* pServer = pDoc->GetServer(wServerID);
		if (pServer)
		{
			pServer->SetAlarmSwitch(!pServer->GetAlarmSwitch());
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewControllItems::OnUpdateVideoAlarmSwitch(CCmdUI* pCmdUI) 
{
	WORD wServerID = GetSelectedServer();

	if (wServerID != SERVERID_NOID)
	{
		CIdipClientDoc* pDoc = GetDocument();
		CServer* pServer = pDoc->GetServer(wServerID);
		if (pServer && pServer->IsAlarm())
		{
			pCmdUI->SetCheck(pServer->GetAlarmSwitch());
		}
		else
		{
			pCmdUI->SetCheck(FALSE);
			pCmdUI->Enable(FALSE);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CViewControllItems::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	CTreeCtrl &theTree = GetTreeCtrl();
	CSecID id(theTree.GetItemData(pNMTreeView->itemNew.hItem));
	CViewIdipClient *pVV = theApp.GetMainFrame()->GetViewIdipClient();
	int nHtmlType = 0;
	if (pVV)
	{
		CWndSmall *pSW = pVV->GetWndSmallCmdActive();
		if (pSW && pSW->GetType() == WST_HTML)
		{
			ASSERT_KINDOF(CWndHTML, pSW);
			nHtmlType = ((CWndHTML*)pSW)->GetHTMLwndType();
		}
	}
	if (  ((nHtmlType == HTML_TYPE_HOST_MAP)   && (theTree.GetParentItem(pNMTreeView->itemNew.hItem) == NULL))
       || ((nHtmlType == HTML_TYPE_CAMERA_MAP) && (id.IsInputID() || id.IsOutputID() 
#ifdef _DEBUG
		//TODO! RKE: activate at 5.1
		 || id.IsMediaID()
#endif
		 )) )
	{
		CWnd *pWnd = AfxGetMainWnd();
		theTree.SelectItem(pNMTreeView->itemNew.hItem);
		m_pDragImage = theTree.CreateDragImage(pNMTreeView->itemNew.hItem);
		if (m_pDragImage)
		{
			CPoint ptBegin;
			IMAGEINFO ii;
			m_pDragImage->GetImageInfo(0, &ii);
			ptBegin.x = 15;
			ptBegin.y = ((CRect*)&ii.rcImage)->Height()/2 - m_nCaptionHeight;
			if (m_pDragImage->BeginDrag(0, ptBegin))
			{
				CPoint pt;
				GetCursorPos(&pt);
				pWnd->ScreenToClient(&pt);
				pt += GetWindowOffset(pWnd);
				m_pDragImage->DragEnter(pWnd, pt);
				SetCapture();
			}
		}
	}
	*pResult = 0;
}
//////////////////////////////////////////////////////////////////////////
void CViewControllItems::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_pDragImage)
	{
		CWnd *pWnd = AfxGetMainWnd();
		ReleaseCapture();
		m_pDragImage->DragLeave(pWnd);
		m_pDragImage->EndDrag();
		CViewIdipClient *pView = theApp.GetMainFrame()->GetViewIdipClient();
		ASSERT(pView != NULL);
		point += GetWindowOffset(pView);
		CWndSmall* pSmallWnd = pView->GetWndSmallCmdActive();
		if (pSmallWnd && pSmallWnd->GetType() == WST_HTML)
		{
			ASSERT_KINDOF(CWndHTML, pSmallWnd);
			WORD wHost, wServer = GetSelectedServer(&wHost);
			CTreeCtrl &theTree = GetTreeCtrl();
			HTREEITEM hItem = theTree.GetSelectedItem();
			CString sName;
			CSecID id;
			CIPCMediaRecord*pMR = NULL;
			if (theTree.GetParentItem(hItem) == NULL) // Host
			{
				id = CSecID(SECID_GROUP_HOST, wHost);
				CHostArray& ha = theApp.GetHosts();
				CHost* pHost = ha.GetHost(id);
				if (pHost)
				{
					sName = pHost->GetName();
				}
				else
				{
					sName = theTree.GetItemText(hItem);
				}
			}
			else
			{
				id    = theTree.GetItemData(hItem);
				sName = theTree.GetItemText(hItem);
				if (id.IsMediaID())
				{
					wHost, wServer;
					pMR = GetSelectedItemsMediaRecord(wHost, wServer, hItem);
				}
			}
			((CWndHTML*)pSmallWnd)->InsertElement(id, sName, wServer, wHost, pMR);
		}
		m_bDraggedInto =FALSE;
		WK_DELETE(m_pDragImage);
		InvalidateRect(NULL);
	}
	CViewObjectTree::OnLButtonUp(nFlags, point);
}
//////////////////////////////////////////////////////////////////////////
void CViewControllItems::EndDragg()
{
	if (m_pDragImage)
	{
		CWnd *pWnd = AfxGetMainWnd();
		ReleaseCapture();
		m_pDragImage->DragLeave(pWnd);
		m_pDragImage->EndDrag();
		m_bDraggedInto =FALSE;
		WK_DELETE(m_pDragImage);
		InvalidateRect(NULL);
	}
}
//////////////////////////////////////////////////////////////////////////
void CViewControllItems::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_pDragImage)
	{
		point += GetWindowOffset(AfxGetMainWnd());
		m_pDragImage->DragMove(point);
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND)) ? TRUE:FALSE;
		CViewIdipClient *pView = theApp.GetMainFrame()->GetViewIdipClient();
		ASSERT(pView != NULL);
		CWndSmall* pSmallWnd = pView->GetWndSmallCmdActive();
		if (pSmallWnd && pSmallWnd->GetType() == WST_HTML)
		{
			ASSERT_KINDOF(CWndHTML, pSmallWnd);
			CWndHTML*pHW = (CWndHTML*)pSmallWnd;
			CPoint pt;
			GetCursorPos(&pt);
			pHW->ScreenToClient(&pt);
			BOOL bInside = pHW->DraggedInto(pt);
			if (m_bDraggedInto)
			{
				if (!bInside)
				{
					HCURSOR hCursor = pHW->CheckDragScroll(&pt);
					if (hCursor)
					{
						SetCursor(hCursor);
						m_pDragImage->DragShowNolock(FALSE);
					}
					else
					{
						m_pDragImage->DragShowNolock(TRUE);
					}
				}
				else
				{
					m_pDragImage->DragShowNolock(TRUE);
				}
			}
			else
			{
				m_bDraggedInto = bInside;
			}
		}
	}
	CViewObjectTree::OnMouseMove(nFlags, point);
}
//////////////////////////////////////////////////////////////////////////
// HTML
void  CViewControllItems::AddLocalHTMLmapping(DWORD dwID, CSecID elementID)
{
	DWORD *pdwElement = new DWORD[2];
	pdwElement[MAPPING_SERVER] = dwID;
	pdwElement[MAPPING_EL_ID]  = elementID.GetID();
	gm_LocalHTMLmappings.AddTail(pdwElement);
}
//////////////////////////////////////////////////////////////////////////
void CViewControllItems::DeleteLocalHTMLmappings()
{
	while (gm_LocalHTMLmappings.GetCount())
	{
		delete gm_LocalHTMLmappings.RemoveHead();
	}
}
//////////////////////////////////////////////////////////////////////////
BOOL  CViewControllItems::IsOnLocalHTMLpage(WORD wHostID, CSecID elementID)
{
	POSITION pos = gm_LocalHTMLmappings.GetHeadPosition();
	DWORD *pdwElement;
	while (pos)
	{
		pdwElement = (DWORD*) gm_LocalHTMLmappings.GetNext(pos);
		if (HOSTID_OF(pdwElement[MAPPING_SERVER]) == wHostID && pdwElement[MAPPING_EL_ID] == elementID.GetID())
		{
#ifdef _DEBUG
			//TODO! RKE: activate at 5.1
			if (SERVERID_OF(pdwElement[MAPPING_SERVER]) == 0)
			{
				return TRUE;
			}
			else
			{
				return MAKELONG(TRUE, SERVERID_OF(pdwElement[MAPPING_SERVER]));
			}
#else
			return TRUE;
#endif
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////
CIPCMediaRecord* CViewControllItems::GetSelectedItemsMediaRecord(WORD &wHost, WORD &wServer, HTREEITEM hSelected)
{
	wServer = GetSelectedServer(&wHost, hSelected);
	CServer *pServer = GetServer(wServer);
	if (pServer)
	{
		CIPCAudioIdipClient *pAudio = pServer->GetAudio();
		if (pAudio)
		{
			HTREEITEM hItem = hSelected;
			if (hItem == NULL) hItem = GetTreeCtrl().GetSelectedItem();
			if (hItem)
			{
				return pAudio->GetMediaRecordPtrFromSecID(GetTreeCtrl().GetItemData(hItem));
			}
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
int CViewControllItems::GetImageIndex(const CIPCOutputRecord& rec)
{
	int i = -1;

	// _/_ = 0 , ___ = 1 , SW = 2 , SWSN = 3 , COLOR = 4 , COLORSN = 5
	// ERROR = 6 OFF = 7
	if (rec.IsOkay()==FALSE) 
	{
		i = _IMAGE_ERROR;
	}
	else if (rec.IsEnabled() == FALSE)
	{
		i = _IMAGE_OFF;
	}
	else if (rec.IsRelay())
	{
		if (rec.RelayIsClosed())
		{
			i = _IMAGE_CLOSE;
		}
		else
		{
			i = _IMAGE_OPEN;
		}
	}
	else if (rec.IsCamera())
	{
		if (rec.CameraIsFixed())
		{
			i = _IMAGE_CAMERA;
		}
		else
		{
			i = _IMAGE_PTZ;
		}
	}
	return i;
}

