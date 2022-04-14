// ViewCamera.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "MainFrm.h"

#include "images.h"

#include "WndHtml.h"

#include "ViewRelais.h"
#include "IdipClientDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CViewRelais, CViewControllItems)

CViewRelais::CViewRelais()
{
}

CViewRelais::~CViewRelais()
{
}

BEGIN_MESSAGE_MAP(CViewRelais, CViewControllItems)
	//{{AFX_MSG_MAP(CViewRelais)
	ON_WM_CREATE()
	ON_MESSAGE(WM_USER, OnUser)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_COMMAND(ID_RELAIS_OPEN, OnRelayOpen)
	ON_UPDATE_COMMAND_UI(ID_RELAIS_OPEN, OnUpdateRelayOpen)
	ON_COMMAND(ID_RELAIS_CLOSE, OnRelayClose)
	ON_UPDATE_COMMAND_UI(ID_RELAIS_CLOSE, OnUpdateRelayClose)
	ON_COMMAND(ID_RELAIS_TOGGLE, OnRelayToggle)
	ON_UPDATE_COMMAND_UI(ID_RELAIS_TOGGLE, OnUpdateRelayToggle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
void CViewRelais::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	WORD cause	  = LOWORD(lHint);
//	WORD wServerID = HIWORD(lHint);
	CSecID id;
//	HTREEITEM hChild;

	switch (cause)
	{
	case VDH_OUTPUT:
		ASSERT_KINDOF(CIPCOutputIdipClient, pHint);
		InsertOutputs((CIPCOutputIdipClient*)pHint);
		break;
	default:
		CViewControllItems::OnUpdate(pSender, lHint, pHint);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
int CViewRelais::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CViewControllItems::OnCreate(lpCreateStruct) == -1)
		return -1;

	CString sText;
	sText.LoadString(IDS_RELAY);
	SetWindowText(sText);
	m_btnTitle.SetWindowText(sText);

	return 0;
}
//////////////////////////////////////////////////////////////////////
void CViewRelais::SwitchRelay(WORD wServerID, CSecID id, BOOL bOn)
{
	HTREEITEM hItem = GetTreeItem(wServerID, id.GetID());
	if (hItem)
	{
		// Die Logik von 'SwitchRelay(hItem, !bOn)' ist genau umgekehrt: bOn=1 ->Relay auf
		SwitchRelay(hItem, !bOn); 
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewRelais::SwitchRelay(HTREEITEM hItemRelay, BOOL bOn)
{
	CTreeCtrl&  theTree = GetTreeCtrl();
	CIdipClientDoc* pDoc = GetDocument();
	CSecID id = (DWORD)theTree.GetItemData(hItemRelay);
	HTREEITEM hParent = theTree.GetParentItem(hItemRelay);

	if (hParent)
	{
		CServer* pServer = pDoc->GetServer(LOWORD(theTree.GetItemData(hParent)));

		if (pServer)
		{
			CIPCOutputIdipClient* pOutput = pServer->GetOutput();
			CIPCOutputRecord* pRec = pOutput->GetOutputRecordPtrFromSecID(id);
			if (pRec && (!pRec->IsCamera()))
			{
				if (!pOutput->IsRelayPTZ(id))
				{
					// it's relay
					if (   pRec->RelayIsOpen()
						&& (bOn!=1)
						)
					{
						pOutput->DoRequestSetRelay(id,TRUE);
						if (theApp.m_pUser)
						{
							WK_TRACE_USER(_T("%s schließt Relais %s\n"),
								theApp.m_pUser->GetName(),
								pRec->GetName());
						}
					}
					else if (   pRec->RelayIsClosed()
							 && (bOn!=0)
							)
					{
						pOutput->DoRequestSetRelay(id,FALSE);
						if (theApp.m_pUser)
						{
							WK_TRACE_USER(_T("%s öffnet Relais %s\n"),
								theApp.m_pUser->GetName(),
								pRec->GetName());
						}
					}
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewRelais::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint pt;
	GetCursorPos(&pt);
	HTREEITEM hItem;
	UINT nFlags;
	CTreeCtrl &theTree = GetTreeCtrl();

	ScreenToClient(&pt);
	hItem = theTree.HitTest(pt,&nFlags);

	// camera click on State Image
	if (hItem && (nFlags & TVHT_ONITEMICON))
	{
		SwitchRelay(hItem,-1);
	}

	UpdateCameraPage(hItem, VDH_FLAGS_UPDATE|VDH_FLAGS_SELECTED);
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CViewRelais::OnRelayOpen() 
{
	HTREEITEM hSelectedItem = GetTreeCtrl().GetSelectedItem();
	if (hSelectedItem)
	{
		SwitchRelay(hSelectedItem,TRUE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewRelais::OnUpdateRelayOpen(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	HTREEITEM hSelectedItem = GetTreeCtrl().GetSelectedItem();
	if (hSelectedItem)
	{
		WORD wHost, wServer = GetSelectedServer(&wHost, hSelectedItem);
		CServer *pServer = GetServer(wServer);
		if (pServer)
		{
			CIPCOutputIdipClient* pOutput = pServer->GetOutput();
			if (pOutput)
			{
				CIPCOutputRecord* pRecord = pOutput->GetOutputRecordPtrFromSecID(GetTreeCtrl().GetItemData(hSelectedItem));
				if (pRecord && pRecord->IsRelay())
				{
					bEnable = pRecord->RelayIsClosed() && !pRecord->RelayIsEdgeControlled();
				}
			}
		}
	}
/*
	if (hSelectedItem)
	{
		int i=0,is=0;
		GetTreeCtrl().GetItemImage(hSelectedItem,i,is);
		bEnable = (i==_IMAGE_CLOSE);
	}
*/
	pCmdUI->Enable(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CViewRelais::OnRelayClose() 
{
	HTREEITEM hSelectedItem = GetTreeCtrl().GetSelectedItem();
	if (hSelectedItem)
	{
		SwitchRelay(hSelectedItem,FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewRelais::OnUpdateRelayClose(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	HTREEITEM hSelectedItem = GetTreeCtrl().GetSelectedItem();
	
	if (hSelectedItem)
	{
		int i=0,is=0;
		GetTreeCtrl().GetItemImage(hSelectedItem,i,is);
		bEnable = (i==_IMAGE_OPEN);
	}
	pCmdUI->Enable(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CViewRelais::OnRelayToggle()
{
	HTREEITEM hSelectedItem = GetTreeCtrl().GetSelectedItem();
	if (hSelectedItem)
	{
		SwitchRelay(hSelectedItem,-1);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewRelais::OnUpdateRelayToggle(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CViewRelais::OnUser(WPARAM wParam, LPARAM lParam) 
{
//	CIdipClientDoc* pDoc = GetDocument();
	WORD cause	  = LOWORD(wParam);
	WORD wServerID = HIWORD(wParam);
//	CServer* pServer = pDoc->GetServer(wServerID);
	CSecID id = (DWORD)(lParam);
	
	switch (cause)
	{
		case WPARAM_CONTROL_SELECT_RELAY:
			if (id.IsOutputID())
			{
				SwitchRelay(wServerID, id, TRUE);
			}
			else if(id.IsInputID())
			{
				ShowInformation(GetTreeItem(wServerID,id.GetID()));
			} 
			break;

		case WPARAM_CONTROL_UNSELECT_RELAY:
			if (id.IsOutputID())
			{
				SwitchRelay(wServerID, id, FALSE);
			} 
			break;
		case WPARAM_ACTUALIZE:
			Actualize(wServerID,id);
			break;

		default:
			CViewControllItems::OnUser(wParam, lParam);
		break;
	}

	return 0L;
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CViewRelais::GetTreeItem(WORD wServer, const CString& s)
{
	HTREEITEM hServer;
	HTREEITEM hChild;
	CSecID    id;
	CTreeCtrl &theTree = GetTreeCtrl();
	hServer = theTree.GetRootItem();
	while (hServer)
	{
		if (wServer==LOWORD(theTree.GetItemData(hServer)))
		{
			HTREEITEM hRelay;
			hRelay = hServer;
			hChild = theTree.GetChildItem(hRelay);
			while (hChild)
			{
				if (0==s.CompareNoCase(theTree.GetItemText(hChild)))
				{
					return hChild;// found relay
				}
				hChild = theTree.GetNextSiblingItem(hChild);
			}
		}
		// next server, if any
		hServer = theTree.GetNextSiblingItem(hServer);
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CViewRelais::GetTreeItem(WORD wServer, DWORD dwID)
{
	HTREEITEM hServer;
	HTREEITEM hChild;
	HTREEITEM hRelay;
	CTreeCtrl &theTree = GetTreeCtrl();

	hServer = theTree.GetRootItem();
	while (hServer)
	{
		if (wServer==LOWORD(theTree.GetItemData(hServer)))
		{
			// bingo found server
			if (dwID==0)
			{
				return hServer;
			}

			hRelay = hServer;
			hChild = theTree.GetChildItem(hRelay);
			while (hChild)
			{
				if (dwID==theTree.GetItemData(hChild))
				{
					return hChild;// found relay
				}
				hChild = theTree.GetNextSiblingItem(hChild);
			}
		}
		// next server, if any
		hServer = theTree.GetNextSiblingItem(hServer);
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CViewRelais::InsertOutputs(CIPCOutputIdipClient* pOutput)
{
	HTREEITEM hServer;
	CServer*  pServer = pOutput->GetServer();
	WORD wServerID = pServer->GetID();
	CTreeCtrl &theTree = GetTreeCtrl();

	// insert server if not in tree
	hServer = GetTreeItem(wServerID);
	if (hServer==NULL)
	{
		hServer = InsertServer(pServer);
	}

	// insert camera/relay if not in tree
	if (hServer)
	{
		HTREEITEM hChild;
		HTREEITEM hDelete = NULL;
		CSecID idOutput;
		CIPCOutputRecord* pRecord = NULL;

		// bestehende Tree Items pruefen Relay
		hChild = theTree.GetChildItem(hServer);
		while (hChild)
		{
			idOutput = (DWORD)theTree.GetItemData(hChild);
			hDelete = NULL;
			if (idOutput.IsOutputID())
			{
				pRecord = pOutput->GetOutputRecordPtrFromSecID(idOutput);
				if (pRecord)
				{
					if (!pRecord->IsEnabled())
					{
						hDelete = hChild;
					}
				}
				else
				{
					hDelete = hChild;
				}
			}
			hChild = theTree.GetNextSiblingItem(hChild);
			if (hDelete)
			{
				theTree.DeleteItem(hDelete);
			}
		}

		// Outputs einfügen
		int i,c;
		c = pOutput->GetNumberOfOutputs();

		for (i=0;i<c;i++)
		{
			const CIPCOutputRecord rec = pOutput->GetOutputRecordFromIndex(i);
			TRACE(_T("output %s %s %d, %08lx\n"),pServer->GetName(),rec.GetName(),i,rec.GetID().GetID());
			if (rec.IsRelay())
			{
				hChild = theTree.GetChildItem(hServer);
			}
			else
			{
				continue;
			}
			while (hChild)
			{
				if (rec.GetID().GetID()==theTree.GetItemData(hChild))
				{
					// found
					break;
				}
				hChild = theTree.GetNextSiblingItem(hChild);
			}
			if (hChild == NULL)
			{
				// check for SpecialReceiver
				if(!pServer->IsDTS())	// do not add for DTS
				{
					InsertOutput(hServer,rec);
				}
			}
		}

		if (gm_LocalHTMLmappings.GetCount())
		{
			POSITION pos = gm_LocalHTMLmappings.GetHeadPosition();
			while (pos)
			{
				DWORD *pdwMapping = (DWORD*)gm_LocalHTMLmappings.GetNext(pos);
				if (SERVERID_OF(pdwMapping[MAPPING_SERVER]) == wServerID)
				{
					CSecID ServerID(wServerID, gm_wLocalServerID);
					CString strID;
					DWORD dwUpdate = SEARCH_BY_ID|SEARCH_BY_SERVER|SEARCH_IN_ALL|HTML_ELEMENT_UPDATE_STATE;
					strID.Format(_T("%08x"), pdwMapping[MAPPING_EL_ID]);
					UpdateMapElement(ServerID, strID, dwUpdate);
				}
			}
		}

		if (theTree.GetChildItem(hServer) == NULL)
		{
			theTree.DeleteItem(hServer);
		}
		else
		{
			theTree.Expand(hServer, TVE_EXPAND);
			CheckVisibility();
		}
	}
}
