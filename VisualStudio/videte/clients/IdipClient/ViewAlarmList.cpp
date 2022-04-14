// ViewCamera.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "MainFrm.h"

#include "ViewAlarmList.h"
#include "IdipClientDoc.h"

#include "images.h"


#include "DlgInputDeactivation.h"
#include ".\viewalarmlist.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CViewAlarmList, CViewControllItems)

CViewAlarmList::CViewAlarmList()
{
	m_pDlgInputDeactivation=NULL;
}

CViewAlarmList::~CViewAlarmList()
{
}

BEGIN_MESSAGE_MAP(CViewAlarmList, CViewControllItems)
	//{{AFX_MSG_MAP(CViewAlarmList)
	ON_WM_CREATE()
	ON_MESSAGE(WM_USER,OnUser)
	ON_COMMAND(ID_INPUT_DEACTIVATE, OnInputDeactivate) /*RM*/
	ON_UPDATE_COMMAND_UI(ID_INPUT_DEACTIVATE,OnUpdateInputDeactivate) /*RM*/
	ON_COMMAND(ID_INPUT_CONFIRM, OnInputConfirm)
	ON_UPDATE_COMMAND_UI(ID_INPUT_CONFIRM, OnUpdateInputConfirm)
	ON_COMMAND(ID_INPUT_INFORMATION, OnInputInformation)
	ON_UPDATE_COMMAND_UI(ID_INPUT_INFORMATION, OnUpdateInputInformation)
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
void CViewAlarmList::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	WORD cause	  = LOWORD(lHint);
	WORD wServerID = HIWORD(lHint);
//	CSecID id;
//	HTREEITEM hChild;

	switch (cause)
	{
	case VDH_INPUT:
		ASSERT_KINDOF(CIPCInputIdipClient, pHint);
		InsertInputs((CIPCInputIdipClient*)pHint);
		break;
	case VDH_REMOVE_SERVER:
		CViewControllItems::OnUpdate(pSender, lHint, pHint);
		if (m_pDlgInputDeactivation && m_pDlgInputDeactivation->GetServerID() == wServerID)
		{
			m_pDlgInputDeactivation->DestroyWindow(); // ATTENTION set's m_pDlgInputDeactivation = NULL !!!
		}
		break;
	default:
		CViewControllItems::OnUpdate(pSender, lHint, pHint);
		break;
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CViewAlarmList::OnUser(WPARAM wParam, LPARAM lParam) 
{
//	CDocument* pDoc = GetDocument();
	WORD cause	  = LOWORD(wParam);
	WORD wServerID = HIWORD(wParam);
	CServer* pServer = GetDocument()->GetServer(wServerID);
	CSecID id = (DWORD)(lParam);
	
	switch (cause)
	{
	case WPARAM_ACTUALIZE:
		Actualize(wServerID,id);
		break;
	case WPARAM_ALARM_OFF_SPAN:
		// got Alarm Off Span data
		OnAlarmOffSpanData(wServerID,id);
		break;
	case WPARAM_ALARM_OFF_SPAN_ON:
		OnAlarmOffSpan(wServerID,id,TRUE);
		break;
	case WPARAM_ALARM_OFF_SPAN_OFF:
		OnAlarmOffSpan(wServerID,id,FALSE);
		break;
	case WPARAM_INPUT_CONNECTED:
		if (pServer)
		{
			pServer->OnInputConnected();
		} break;
	case WPARAM_INPUT_DISCONNECTED:
		if (pServer)
		{
			pServer->OnInputDisconnected();
		}break;
	case WPARAM_INPUT_FETCHRESULT:
		if (pServer)
		{
			pServer->OnInputFetchResult();
		} 
		else
		{
			TRACE(_T("no server object for OnInputFetchResult %d\n"),wServerID);
		}
		break;
	default:
		return CViewControllItems::OnUser(wParam, lParam);
		break;
	}

	return 0L;
}
//////////////////////////////////////////////////////////////////////////
int CViewAlarmList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CViewControllItems::OnCreate(lpCreateStruct) == -1)
		return -1;

	CString sText;
	sText.LoadString(IDS_ALARM);
	SetWindowText(sText);
	m_btnTitle.SetWindowText(sText);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CViewAlarmList::InsertInputs(CIPCInputIdipClient* pInput)
{
	HTREEITEM hServer;
	CServer*  pServer = pInput->GetServer();
	WORD wServerID = pServer->GetID();
	CTreeCtrl &theTree = GetTreeCtrl();
	// insert server if not in tree
	hServer = GetTreeItem(wServerID);
	if (hServer==NULL)
	{
		hServer = InsertServer(pServer);
	}

	// check for SpecialReceiver
	if (pServer->IsDTS())
	{
		// nothing to add
		return;
	}

	// insert input if not in tree
	if (hServer)
	{
		pInput->Lock();
		HTREEITEM hChild;
		HTREEITEM hDelete = NULL;
		CSecID idInput;
		CIPCInputRecord* pRecord = NULL;
	
		hChild = theTree.GetChildItem(hServer);
		// bestehende Tree Items pruefen
		while (hChild)
		{
			idInput = (DWORD)theTree.GetItemData(hChild);
			hDelete = NULL;
			if (idInput.IsInputID())
			{
				pRecord = pInput->GetInputRecordPtrFromSecID(idInput);
				if (pRecord)
				{
					if (!pRecord->GetIsEnabled())
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

		// Inputs einfügen
		for (int i=0;i<pInput->GetNumberOfInputs();i++)
		{
			const CIPCInputRecord rec = pInput->GetInputRecordFromIndex(i);

			// Input already in Tree ?
			hChild = theTree.GetChildItem(hServer);
			while (hChild)
			{
				if (rec.GetID().GetID()==theTree.GetItemData(hChild))
				{
					// found
					break;
				}
				hChild = theTree.GetNextSiblingItem(hChild);
			}
			if (hChild)
			{
				// actualize it
				ActualizeInput(hChild,rec);
			}
			else
			{
				// add it
				InsertInput(hServer,rec);
				if (rec.GetIsEnabled())
				{
					pInput->DoRequestGetValue(rec.GetID(),CSD_ALARM_SPAN_OFF_STATUS,0);
				}
			}
		}
		pInput->Unlock();

		if (pServer->IsAlarm())
		{
			theTree.Expand(hServer, TVE_EXPAND);

			CString sMessage = pServer->GetNotificationMessage();

			if (!sMessage.IsEmpty())
			{
				CDocument* pDoc = GetDocument();
				CStringHint NMhint(sMessage);
				pDoc->UpdateAllViews(this,MAKELONG(VDH_NOTIFICATION_MESSAGE,
									 LOWORD(theTree.GetItemData(hServer))),
									 &NMhint);
			}
			else
			{
				CString sURL = pServer->GetNotificationURL();
				if (!sURL.IsEmpty())
				{
					CDocument* pDoc = GetDocument();
					pDoc->UpdateAllViews(this,MAKELONG(VDH_INPUT_URL,
										 LOWORD(theTree.GetItemData(hServer))),
										 (CObject*)&sURL);
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
//////////////////////////////////////////////////////////////////////////
HTREEITEM CViewAlarmList::InsertInput(HTREEITEM hServer, const CIPCInputRecord& rec)
{
	if (rec.IsCommPort())
	{
		return NULL;
	}

	HTREEITEM hInput = NULL;
	TV_INSERTSTRUCT tvis;
	CString s = rec.GetName();

#ifdef _DEBUG
	CString sID;
	sID.Format(_T(" %08lx"),rec.GetID().GetID());
//	s+=sID;
#endif

	tvis.hParent = hServer;
	if (tvis.hParent==NULL)
	{
		return NULL;
	}
	tvis.hInsertAfter = TVI_SORT;

	tvis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

	tvis.item.lParam = rec.GetID().GetID();
	tvis.item.cchTextMax = s.GetLength();
	tvis.item.pszText = (LPTSTR)LPCTSTR(s);

	int i = GetImageIndex(rec);

	if (i!=_IMAGE_OFF)
	{
		tvis.item.iImage = i;
		tvis.item.iSelectedImage = i;
		hInput = GetTreeCtrl().InsertItem(&tvis);
	}
	if (hInput)
	{
		UpdateCameraPage(hInput);
	}
	// NOT YET TODO
	if (i==_IMAGE_ALARM) // ALARM
	{
		GetTreeCtrl().EnsureVisible(hInput);
		/*
		CDocument* pDoc = GetDocument();
		HTREEITEM hServer = GetTreeCtrl().GetParentItem(hInput);
		pDoc->UpdateAllViews(this,MAKELONG(VDH_INPUT_DATA,
							 LOWORD(GetTreeCtrl().GetItemData(hServer))),(CObject*)(DWORD)rec.GetID().GetID());
							 */
	}

//	UpdateCameraPage(hInput);

	return hInput;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CViewAlarmList::ActualizeInput(HTREEITEM hInput, const CIPCInputRecord& rec)
{
	int i = GetImageIndex(rec);
	int nImage,nSelImage;
	nImage = 0;
	nSelImage = 0;
	GetTreeCtrl().GetItemImage(hInput,nImage,nSelImage);
	if (   (nImage==_IMAGE_OK_GRAY) 
		|| (nImage==_IMAGE_ALARM_GRAY)
		)
	{
		if (i == _IMAGE_OK)
		{
			i = _IMAGE_OK_GRAY;
		}
		else if (i == _IMAGE_ALARM)
		{
			i = _IMAGE_ALARM_GRAY;
		}
	}

	GetTreeCtrl().SetItemImage(hInput,i,i);
	GetTreeCtrl().SetItemText(hInput,rec.GetName());

	UpdateCameraPage(hInput);

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CViewAlarmList::Actualize(WORD wServerID, CSecID id)
{
	HTREEITEM hServer = GetTreeItem(wServerID);
	HTREEITEM hChild = GetTreeItem(wServerID, id.GetID());

	// found input or output
	CServer* pServer = GetDocument()->GetServer(wServerID);
	ASSERT(id.IsInputID());
	if (pServer && !pServer->IsDTS())
	{
		CIPCInputIdipClient* pInput = pServer->GetInput();
		if (pInput)
		{
			pInput->Lock();
			CIPCInputRecord* pRecord = pInput->GetInputRecordPtrFromSecID(id);
			if (pRecord)
			{
				if (hChild)
				{
					ActualizeInput(hChild,*pRecord);
				}
				else
				{
					if (hServer == NULL)
					{
						hServer = InsertServer(pServer);
					}
					if (hServer)
					{
						InsertInput(hServer,*pRecord);
					}
				}
			}
			pInput->Unlock();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewAlarmList::Add(WORD wServerID, CSecID id)
{
	HTREEITEM hChild = GetTreeItem(wServerID, id.GetID());
	if (hChild == NULL)
	{
		// found input or output
		CServer* pServer = GetDocument()->GetServer(wServerID);
		ASSERT(id.IsInputID());
		if (!pServer->IsDTS())
		{
			CIPCInputIdipClient* pInput = pServer->GetInput();
			if (pInput)
			{
				pInput->Lock();
				CIPCInputRecord* pRecord = pInput->GetInputRecordPtrFromSecID(id);
				if (pRecord)
				{
					InsertInput(hChild,*pRecord);
				}
				pInput->Unlock();
			}
		}
	}
	else
	{
		Actualize(wServerID, id);
	}
}
//ML 24.9.99 End Insertation}
//////////////////////////////////////////////////////////////////////
void CViewAlarmList::OnAlarmOffSpanData(WORD wServerID, CSecID id)
{
	if (!m_pDlgInputDeactivation)
	{											 
		CServer* pServer = GetDocument()->GetServer(wServerID);
		if (pServer)
		{
			m_pDlgInputDeactivation = new CDlgInputDeactivation(this, pServer, id);
		}
		else
		{
			WK_TRACE_ERROR(_T("OnAlarmOffSpanData NO server for id %u\n"), wServerID);
		}
	}
	else
	{
		WK_TRACE_WARNING(_T("OnAlarmOffSpanData while dialog open; server id %u\n"), wServerID);
	}
}
//////////////////////////////////////////////////////////////////////
void CViewAlarmList::OnAlarmOffSpan(WORD wServerID,CSecID id, BOOL bOn)
{
	HTREEITEM hItem = GetTreeItem(wServerID, id.GetID());
	if (hItem)
	{
		int nImage,nSelImage;
		nImage = 0;
		nSelImage = 0; 
		GetTreeCtrl().GetItemImage(hItem,nImage,nSelImage);
		if (   (nImage == _IMAGE_OK)
			|| (nImage == _IMAGE_OK_GRAY)
			)
		{
			nImage = bOn ? _IMAGE_OK_GRAY : _IMAGE_OK;
		}
		else if ( (nImage == _IMAGE_ALARM)
				 || (nImage == _IMAGE_ALARM_GRAY)
				 )
		{
			nImage = bOn ? _IMAGE_ALARM_GRAY : _IMAGE_ALARM;
		}
		GetTreeCtrl().SetItemImage(hItem,nImage,nImage);
	}
	else
	{
//		WK_TRACE(_T("cannot find Item for Alarm Off Span\n"));
	}
}
//////////////////////////////////////////////////////////////////////
void CViewAlarmList::OnInputDeactivate()  /*RM*/
{
	// nur ein Dialog zur Zeit erlaubt, daher keine Anfrage, wenn offen
	if (m_pDlgInputDeactivation == FALSE)
	{
		HTREEITEM hSelectedItem = GetTreeCtrl().GetSelectedItem();
		
		if (hSelectedItem)
		{
			DWORD dwID = GetTreeCtrl().GetItemData(hSelectedItem);
			CSecID id = dwID;

			if (id.IsInputID())
			{
				WORD wServerID = GetSelectedServer();
				CServer* pServer = GetDocument()->GetServer(wServerID);
				if (pServer && pServer->IsConnectedLive())
				{
					CIPCInputIdipClient* pInput = pServer->GetInput();
					pInput->DoRequestGetAlarmOffSpans(id);
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CViewAlarmList::OnUpdateInputDeactivate(CCmdUI* pCmdUI) /*RM*/
{
	BOOL bEnable = FALSE;
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	if (hItem)
	{
		CSecID id(GetTreeCtrl().GetItemData(hItem));
		if (   id.IsInputID()
			&& (m_pDlgInputDeactivation == NULL)
			)
		{
				WORD wServerID = GetSelectedServer();
				CServer* pServer = GetDocument()->GetServer(wServerID);
				if (pServer && pServer->IsConnectedLive())
				{
					CIPCInputIdipClient* pInput = pServer->GetInput();
					if (pInput)
					{
						pInput->Lock();
						CIPCInputRecord* pRecord = pInput->GetInputRecordPtrFromSecID(id);
						if (pRecord)
						{
							bEnable = pRecord->GetIsTempDeactivateAllowed();
						}
						pInput->Unlock();
					}
				}
		}
	}
	pCmdUI->Enable(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CViewAlarmList::OnInputConfirm() 
{
	HTREEITEM hSelectedItem = GetTreeCtrl().GetSelectedItem();
	
	if (hSelectedItem)
	{
		DWORD dwID = GetTreeCtrl().GetItemData(hSelectedItem);
		CSecID id = dwID;
		if (id.IsInputID())
		{
			WORD wServerID = GetSelectedServer();
			CServer* pServer = GetDocument()->GetServer(wServerID);
			if (pServer && pServer->IsConnectedLive())
			{
				CIPCInputIdipClient* pInput = pServer->GetInput();
				pInput->DoConfirmAlarm(id);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewAlarmList::OnUpdateInputConfirm(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	HTREEITEM hSelectedItem = GetTreeCtrl().GetSelectedItem();
	if (hSelectedItem)
	{
		int i=0,is=0;
		GetTreeCtrl().GetItemImage(hSelectedItem,i,is);
		bEnable = (i==_IMAGE_ALARM);
	}

	pCmdUI->Enable(bEnable);
}
//////////////////////////////////////////////////////////////////////
void CViewAlarmList::OnInputInformation() 
{
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	if (hItem)
	{
		CSecID id(GetTreeCtrl().GetItemData(hItem));
		if (id.IsInputID())
		{
			ShowInformation(hItem);
		}
	}

}
//////////////////////////////////////////////////////////////////////
void CViewAlarmList::OnUpdateInputInformation(CCmdUI* pCmdUI) 
{
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	if (hItem)
	{
		CSecID id(GetTreeCtrl().GetItemData(hItem));
		if (id.IsInputID())
		{						
			WORD wServerID = GetSelectedServer();
			CServer* pServer = GetDocument()->GetServer(wServerID);
			if (   pServer 
				&& pServer->IsConnectedLive()
				&& !pServer->IsB3())
			{
				pCmdUI->Enable(TRUE);
				return;
			}
		}
	}
	pCmdUI->Enable(FALSE);
}
//////////////////////////////////////////////////////////////////////////
HTREEITEM CViewAlarmList::GetTreeItem(WORD wServer, const CString& s)
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
			HTREEITEM hInput;
			hInput = hServer;
			hChild = theTree.GetChildItem(hInput);
			while (hChild)
			{
				if (0  == s.CompareNoCase(theTree.GetItemText(hChild)))
				{
					return hChild;// found input
				}
				hChild = theTree.GetNextSiblingItem(hChild);
			}
		}
		// next server, if any
		hServer = theTree.GetNextSiblingItem(hServer);
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
HTREEITEM CViewAlarmList::GetTreeItem(WORD wServer, DWORD dwID)
{
	HTREEITEM hServer;
	HTREEITEM hChild;
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

			HTREEITEM hInput;
			hInput = hServer;
			hChild = theTree.GetChildItem(hInput);
			while (hChild)
			{
				if (dwID==theTree.GetItemData(hChild))
				{
					return hChild;// found input
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
int CViewAlarmList::GetImageIndex(const CIPCInputRecord& rec)
{
	int i = _IMAGE_OK;

	// 0=ALARM, 1=OK, 2=ERROR , 3=OFF
	if (rec.GetIsOkay()==FALSE) 
	{
		i = _IMAGE_ERROR;
	}
	else if (rec.GetIsEnabled())
	{
		if (rec.GetIsActive())
		{
			i = _IMAGE_ALARM;
		}
		else	
		{
			i = _IMAGE_OK;
		}
	}
	else 
	{
		i = _IMAGE_OFF;	// not enabled
	}
	return i;
}
/////////////////////////////////////////////////////////////////////////////
void CViewAlarmList::OnSize(UINT nType, int cx, int cy)
{
	CViewControllItems::OnSize(nType, cx, cy);
}
/////////////////////////////////////////////////////////////////////////////
void CViewAlarmList::ModifyContextMenu(CMenu*pMenu)
{
	// TODO! RKE: remove CViewObjectTree::ModifyContextMenu, when AlarmList is ready
	// it removes the menu item 'As List'
	CViewObjectTree::ModifyContextMenu(pMenu);
	// nix tun
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CViewAlarmList::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{

	return CViewControllItems::WindowProc(message, wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////
void CViewAlarmList::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CViewControllItems::OnShowWindow(bShow, nStatus);
}
