// ViewCamera.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "MainFrm.h"
#include "IdipClientDoc.h"

#include "ViewCamera.h"
#include "ViewIdipClient.h"

#include "WndLive.h"
#include "WndHtml.h"

#include "CIPCAudioIdipClient.h"

#include "DlgImpExpCamSequencer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define _ROOT_CHECK_BOXES 1

IMPLEMENT_DYNCREATE(CViewCamera, CViewControllItems)

CViewCamera::CViewCamera()
{
	m_bCamNrFirstDigit	 = FALSE;
	m_wCamNrFirstDigit	 = 0;
}

CViewCamera::~CViewCamera()
{
}

BEGIN_MESSAGE_MAP(CViewCamera, CViewControllItems)
	//{{AFX_MSG_MAP(CViewCamera)
	ON_WM_CREATE()
	ON_MESSAGE(WM_USER, OnUser)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_COMMAND(ID_VIDEO_START, OnVideoStart)
	ON_COMMAND(ID_SPACE, OnSpace)
	ON_COMMAND_RANGE(ID_BUTTON_NUM1, ID_BUTTON_NUMPLUS , OnButtonNumBlock)
	ON_UPDATE_COMMAND_UI_RANGE(ID_BUTTON_NUM1, ID_BUTTON_NUMPLUS , OnUpdateButtonNumBlock)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
LRESULT CViewCamera::OnUser(WPARAM wParam, LPARAM lParam) 
{
	CIdipClientDoc* pDoc = GetDocument();
	WORD cause	  = LOWORD(wParam);
	WORD wServerID = HIWORD(wParam);
	CServer* pServer = pDoc->GetServer(wServerID);
	CSecID id = (DWORD)(lParam);
	
	switch (cause)
	{
		case WPARAM_CONTROL_SELECT_CAM:
		if (id.IsOutputID())
		{
			CServer* pServer = GetDocument()->GetServer(wServerID);
			if (   pServer
				&& pServer->IsControl())
			{
				if (pServer->IsControlExclusive())
				{
					ActivateCamera(GetTreeItem(wServerID, id.GetID()));
				}
				else
				{
					CViewControllItems::Select(wServerID,id);
					if (theApp.m_b1PlusAlarm)
					{
						// richtige Camera soll 1 Plus werden
						CWndLive* pDW = theApp.GetMainFrame()->GetViewIdipClient()->GetWndLive(wServerID,id);
						if (pDW)
						{
							theApp.GetMainFrame()->GetViewIdipClient()->SetWndSmallCmdActive(pDW);
							theApp.GetMainFrame()->GetViewIdipClient()->SetWndSmallBig(pDW);
						}
					}
				}
			}
		}
		else if (id.IsInputID())
		{
			ShowInformation(GetTreeItem(wServerID,id.GetID()));
		} break;

		case WPARAM_CONTROL_UNSELECT_CAM:
		ASSERT(id.IsOutputID());
		{
			if (theApp.m_bCloseAlarmWindow)
			{
				CViewControllItems::Select(wServerID,id,FALSE);
			}
			CViewIdipClient* pVV = theApp.GetMainFrame()->GetViewIdipClient();
			if (pVV)
			{
				if (   theApp.m_b1PlusAlarm
					&& pVV->IsView1plus())
				{
					if (id == pVV->Get1plusWindowID())
					{
						pVV->SendMessage(WM_COMMAND,ID_VIEW_3_3);
					}
				}
				int iNrDisplays = pVV->GetNrOfLiveWindows(wServerID);
				if (iNrDisplays == 0)
				{
					GetDocument()->Disconnect(wServerID, FALSE);
				}
			}
		} break;

		case WPARAM_OUTPUT_CONNECTED:
		if (pServer)
		{
			pServer->OnOutputConnected();
		} break;

		case WPARAM_OUTPUT_DISCONNECTED:
		if (pServer)
		{
			pServer->OnOutputDisconnected();
		} 
		break;

		case WPARAM_OUTPUT_FETCHRESULT:
		if (pServer)
		{
			pServer->OnOutputFetchResult();
		}	break;

		case WPARAM_MESSAGE:
		{
			TCHAR *pBuffer = (TCHAR*)lParam;
			CString sMessage(pBuffer);
			delete pBuffer;
			if (!sMessage.IsEmpty())
			{
				CStringHint NMhint(sMessage);
				pDoc->UpdateAllViews(this,MAKELONG(VDH_NOTIFICATION_MESSAGE, wServerID), &NMhint);
			}
		} break;

		case WPARAM_URL:
		{
			TCHAR *pBuffer = (TCHAR*)lParam;
			CString sURL(pBuffer);
			delete pBuffer;
			if (!sURL.IsEmpty())
			{
				pDoc->UpdateAllViews(this,MAKELONG(VDH_INPUT_URL, wServerID),(CObject*)&sURL);
			}
		} break;
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
void CViewCamera::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	WORD cause	  = LOWORD(lHint);
	WORD wServerID = HIWORD(lHint);
	CSecID id;
	HTREEITEM hChild;

	switch (cause)
	{
	case VDH_OUTPUT:
		ASSERT_KINDOF(CIPCOutputIdipClient, pHint);
		InsertOutputs((CIPCOutputIdipClient*)pHint);
		break;
	case VDH_REMOVE_CAMWINDOW:
		id = (DWORD)pHint;
		if (id.IsOutputID())
		{
			hChild = GetTreeItem(wServerID,id.GetID());
			if (hChild)
			{
				SetCheck(hChild,FALSE);
				CheckParent(hChild);
			}
		}break;
	default:
		CViewControllItems::OnUpdate(pSender, lHint, pHint);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
int CViewCamera::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CViewControllItems::OnCreate(lpCreateStruct) == -1)
		return -1;

	CString sText;
	sText.LoadString(IDS_CAMERAS);
	SetWindowText(sText);
	m_btnTitle.SetWindowText(sText);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CViewCamera::SwitchCamera(HTREEITEM hItemCamera, BOOL bOn, BOOL bForce/*=TRUE*/)
{
	BOOL bRealyOn = GetCheck(hItemCamera);
	if (bForce || bOn != bRealyOn)
	{
		CIdipClientDoc* pDoc    = GetDocument();
		CTreeCtrl&  theTree = GetTreeCtrl();
		CSecID      id      = (DWORD)theTree.GetItemData(hItemCamera);
		HTREEITEM   hParent = theTree.GetParentItem(hItemCamera);
		CServer*    pServer = pDoc->GetServer(LOWORD(theTree.GetItemData(hParent)));

		if (pServer)
		{
			CIPCOutputIdipClient* pOutput = pServer->GetOutput();
			CIPCOutputRecord* pRec = pOutput->GetOutputRecordPtrFromSecID(id);

			if (pRec && pRec->IsCamera())
			{
				if (bOn)
				{
					// Camera ON
					pDoc->UpdateAllViews(this,MAKELONG(VDH_ADD_CAMWINDOW,pServer->GetID()),
										(CObject*)pRec);
					if (theApp.m_pUser)
					{
						WK_TRACE_USER(_T("%s schaltet Kamera %s %08lx an\n"),
							theApp.m_pUser->GetName(),
							pRec->GetName(),pRec->GetID().GetID());
					}
				}
				else
				{
					// Camera OFF
					pDoc->UpdateAllViews(this,MAKELONG(VDH_REMOVE_CAMWINDOW,pServer->GetID()),
										(CObject*)pRec);
					if (theApp.m_pUser)
					{
						WK_TRACE_USER(_T("%s schaltet Kamera %s %08lx aus\n"),
							theApp.m_pUser->GetName(),
							pRec->GetName(),pRec->GetID().GetID());
					}
				}
				SetCheck(hItemCamera,bOn);
	#ifdef _ROOT_CHECK_BOXES
				CheckParent(hItemCamera);
	#endif
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewCamera::ReactivateCamera(HTREEITEM hItem)
{
	CSecID id = (DWORD)GetTreeCtrl().GetItemData(hItem);
	if (id.IsOutputID())
	{
		CIdipClientDoc* pDoc = GetDocument();
		HTREEITEM hParent = GetTreeCtrl().GetParentItem(hItem);
		CServer* pServer = pDoc->GetServer(LOWORD(GetTreeCtrl().GetItemData(hParent)));

		if (pServer)
		{
			CIPCOutputIdipClient* pOutput = pServer->GetOutput();
			CIPCOutputRecord* pRec = pOutput->GetOutputRecordPtrFromSecID(id);

			if (pRec && pRec->IsCamera())
			{
				// Camera ON
				pDoc->UpdateAllViews(this,MAKELONG(VDH_ACTIVATE_REQUEST_CAM,pServer->GetID()),(CObject*)pRec);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewCamera::ActivateCamera(HTREEITEM hItem)
{
	CTreeCtrl &theTree = GetTreeCtrl();
	HTREEITEM hParent = theTree.GetParentItem(hItem);
	HTREEITEM hCamera = theTree.GetChildItem(hParent);
	BOOL bCheck;

	while (hCamera)
	{
		bCheck = GetCheck(hCamera);

#ifdef _DTS	// hier sind die inputs, outputs und medias nicht gruppiert
		CSecID id(theTree.GetItemData(hCamera));
		if (!id.IsOutputID())
		{
			bCheck = FALSE;
		}
#endif
		
		if (   (hCamera != hItem)
			&& (bCheck)
			)
		{
			// deactivate this one
			SetCheck(hCamera,FALSE);
		}
		hCamera = theTree.GetNextSiblingItem(hCamera);
	}

	CIdipClientDoc* pDoc = GetDocument();
	CSecID id = (DWORD)GetTreeCtrl().GetItemData(hItem);
	hParent = GetTreeCtrl().GetParentItem(hItem);
	CServer* pServer = pDoc->GetServer(LOWORD(GetTreeCtrl().GetItemData(hParent)));

	if (pServer)
	{
		CIPCOutputIdipClient* pOutput = pServer->GetOutput();
		CIPCOutputRecord* pRec = pOutput->GetOutputRecordPtrFromSecID(id);

		if (pRec && pRec->IsCamera())
		{
			pDoc->UpdateAllViews(this,MAKELONG(VDH_SINGLE_CAM,pServer->GetID()),
								(CObject*)pRec);
			GetTreeCtrl().SelectItem(hItem);
			SetCheck(hItem,TRUE);
			CheckParent(hItem);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
int CViewCamera::GetNrOfCamerasOfSelectedServer()
{
	//get all cameras IDs from current camera tree
	int iRet = 0;
	CIPCOutputIdipClient* pOutput = NULL;
	WORD wIDSelectedServer = GetSelectedServer();
	CServer*  pServer = GetServer(wIDSelectedServer);

	if (   pServer
		&& pServer->IsConnectedOutput())
	{
		pOutput = pServer->GetOutput();
		iRet = pOutput->GetNrOfActiveCameras();
	}

	return iRet;
}
//////////////////////////////////////////////////////////////////////////
BOOL CViewCamera::FindCamInTreeAndActivate(WORD wCurrentCamID)
{
	CIPCOutputIdipClient* pOutput = NULL;
	WORD wIDSelectedServer = GetSelectedServer();
	CServer*  pServer = GetServer(wIDSelectedServer);
	HTREEITEM hServer = NULL;
	HTREEITEM hCamera = NULL;
	HTREEITEM hChild = NULL;
	CTreeCtrl &theTree = GetTreeCtrl();
	BOOL bRet = FALSE;

	if(pServer)
	{
		pOutput = pServer->GetOutput();
	}

	if (   pOutput 
		&& wIDSelectedServer != SECID_NO_SUBID)
	{
		hServer = GetTreeItem(wIDSelectedServer); 
		hCamera = hServer;
		hChild = GetTreeCtrl().GetChildItem(hCamera);

		WORD wCounter = 0;

		// einfach die wCurrentCamID.te Kamera suchen
		while (hChild)
		{
			CIPCOutputRecord *pRecord = NULL;
			CSecID idOutput(theTree.GetItemData(hChild));
			if(idOutput.IsOutputID())
			{
				pRecord = pOutput->GetOutputRecordPtrFromSecID(idOutput);
				if (pRecord && pRecord->IsCamera())
				{
					if (wCounter == wCurrentCamID)
					{
						SetCheck(hChild,TRUE);
						SwitchCamera(hChild,TRUE);
						theTree.SelectItem(hChild);
						CIdipClientDoc *pDoc = GetDocument();
						if(pDoc)
						{
							pDoc->UpdateAllViews(this,
								MAKELONG(VDH_SET_CMD_ACTIVE_WND,pServer->GetID()),
								(CObject*)pRecord);
							bRet= TRUE;
						}
	
						break;
					}
					wCounter++;
				}
			}
			hChild = theTree.GetNextSiblingItem(hChild);
		}
	}
	return bRet;
}
#ifdef _ROOT_CHECK_BOXES
/////////////////////////////////////////////////////////////////////////////
void CViewCamera::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint pt;
	GetCursorPos(&pt);
	HTREEITEM hItem;
	UINT nFlags;
	CTreeCtrl &theTree = GetTreeCtrl();

	ScreenToClient(&pt);
	hItem = theTree.HitTest(pt,&nFlags);

	// camera click on State Image
	if (hItem)
	{
		if (nFlags & TVHT_ONITEMSTATEICON)
		{
			CSecID id(theTree.GetItemData(hItem));
			ASSERT(!id.IsMediaID());
			BOOL bOn = GetCheck(hItem);
			if (id.IsOutputID())							// switch one camera
			{
				SwitchCamera(hItem, !bOn);
			}
			else if (theTree.GetParentItem(hItem) == NULL)	// switch all cameras of this host
			{
				CViewIdipClient* pVV = theApp.GetMainFrame()->GetViewIdipClient();
				pVV->LockSmallWindowResizing(TRUE);
				HTREEITEM hChild = theTree.GetNextItem(hItem, TVGN_CHILD);
				HTREEITEM hActive = NULL;
				if (bOn)
				{
					CSecID idActive(SECID_NO_ID);			// search active window
					CViewIdipClient*pView = theApp.GetMainFrame()->GetViewIdipClient();
					pView->LockSmallWindows(_T(__FUNCTION__));
					CWndLive*pActive= pView->GetWndLiveCmdActive();
					if (pActive)
					{
						idActive = pActive->GetID();
					}
					pView->UnlockSmallWindows();

					while (hChild)
					{
						id.Set((DWORD)theTree.GetItemData(hChild));
						if (idActive == id)
						{
							hActive = hChild;				// found active window in this branch
							break;
						}
						hChild = theTree.GetNextItem(hChild, TVGN_NEXT);
					}
					hChild = theTree.GetNextItem(hItem, TVGN_CHILD);
				}
				while (hChild)
				{
					if (hActive != hChild)					// switch all except the active
					{
						SwitchCamera(hChild, !bOn, FALSE);
					}
					hChild = theTree.GetNextItem(hChild, TVGN_NEXT);
				}
				if (hActive)								// switch of active window at last
				{											// to avoid flicker
					SwitchCamera(hActive, !bOn, FALSE);
				}
				pVV->LockSmallWindowResizing(FALSE);
			}

			if(!bOn)
			{
				WORD wIDSelectedServer = GetSelectedServer();
				CServer*  pServer = GetServer(wIDSelectedServer);
				CIPCOutputRecord *pRecord = NULL;
				CIPCOutputIdipClient* pOutput = NULL;
				if(pServer)
				{
					pOutput = pServer->GetOutput();
				}
				if(pOutput)
				{
					pRecord = pOutput->GetOutputRecordPtrFromSecID(id);
				}
				if(pRecord)
				{
					CIdipClientDoc *pDoc = GetDocument();	
					if(pDoc)
					{
						pDoc->UpdateAllViews(this,
							MAKELONG(VDH_SET_CMD_ACTIVE_WND,pServer->GetID()),
							(CObject*)pRecord);
					}
				}
			}
		}
		UpdateCameraPage(hItem, VDH_FLAGS_UPDATE|VDH_FLAGS_SELECTED);
	}
	
	*pResult = 0;
}
#else
void CViewCamera::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint pt;
	GetCursorPos(&pt);
	HTREEITEM hItem;
	UINT nFlags;
	CTreeCtrl &theTree = GetTreeCtrl();

	ScreenToClient(&pt);
	hItem = theTree.HitTest(pt,&nFlags);

	// camera click on State Image
	if (hItem)
	{
		if (nFlags & TVHT_ONITEMSTATEICON)
		{
			CSecID id(theTree.GetItemData(hItem));
			ASSERT(!id.IsMediaID());
			if (id.IsOutputID())

			{
				BOOL bOn = GetCheck(hItem);
				SwitchCamera(hItem, !bOn);
				if(!bOn)
				{
					WORD wIDSelectedServer = GetSelectedServer();
					CServer*  pServer = GetServer(wIDSelectedServer);
					CIPCOutputRecord *pRecord = NULL;
					CIPCOutputIdipClient* pOutput = NULL;
					if(pServer)
					{
						pOutput = pServer->GetOutput();
					}
					if(pOutput)
					{
						pRecord = pOutput->GetOutputRecordPtrFromSecID(id);
					}
					if(pRecord)
					{
						CIdipClientDoc *pDoc = GetDocument();	
						if(pDoc)
						{
							pDoc->UpdateAllViews(this,
								MAKELONG(VDH_SET_CMD_ACTIVE_WND,pServer->GetID()),
								(CObject*)pRecord);
						}
					}
				}
			}
		}
		UpdateCameraPage(hItem, VDH_FLAGS_UPDATE|VDH_FLAGS_SELECTED);
	}
	
	*pResult = 0;
}
#endif
/////////////////////////////////////////////////////////////////////////////
void CViewCamera::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	EndDragg();
	CPoint pt;
	GetCursorPos(&pt);
	HTREEITEM hItem;
	UINT nFlags;

	ScreenToClient(&pt);
	hItem = GetTreeCtrl().HitTest(pt,&nFlags);

	// camera right click on State Image
	if (hItem && (nFlags & TVHT_ONITEMSTATEICON))
	{
		// deactivate all cams
		// activate current one
		// it must be a camera item, because all
		// other items have no state icon
		CSecID id = (DWORD)GetTreeCtrl().GetItemData(hItem);
		if (id.IsOutputID())
		{
			ActivateCamera(hItem);
		}
	}
	else if (hItem && ((nFlags & TVHT_ONITEMICON) || (nFlags & TVHT_ONITEMLABEL)))
	{
		// right click on
		GetTreeCtrl().SelectItem(hItem);
		ClientToScreen(&pt);
		ContextMenu(pt,hItem);
	}
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CViewCamera::OnVideoStart() 
{
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	
	if (hItem)
	{
		CSecID id = (DWORD)GetTreeCtrl().GetItemData(hItem);
		if (id.IsOutputID())
		{
			CIdipClientDoc* pDoc = GetDocument();
			HTREEITEM hParent = GetTreeCtrl().GetParentItem(hItem);
			CServer* pServer = pDoc->GetServer(LOWORD(GetTreeCtrl().GetItemData(hParent)));

			if (pServer)
			{
				CIPCOutputIdipClient* pOutput = pServer->GetOutput();
				CIPCOutputRecord* pRec = pOutput->GetOutputRecordPtrFromSecID(id);

				if (pRec && pRec->IsCamera())
				{
					BOOL bOn = GetCheck(hItem);
					if (!bOn)
					{
						// Camera ON
						pDoc->UpdateAllViews(this,MAKELONG(VDH_ADD_CAMWINDOW,pServer->GetID()),(CObject*)pRec);
						bOn = TRUE;
					}
					SetCheck(hItem,bOn);
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewCamera::OnHostSaveSequence(WORD wServerID) 
{
	// TODO! RKE: new Sequencer modus to be implemented
	if (wServerID != SERVERID_NOID)
	{
		CIdipClientDoc* pDoc = GetDocument();
		CServer* pServer = pDoc->GetServer(wServerID);
		if (pServer)
		{
			CString sHost = pServer->GetFullName();
			CDlgImpExpCamSequencer dlg(TRUE,this);
			dlg.m_sHost = sHost;

			if (IDOK==dlg.DoModal())
			{
				CString sSequenceName = dlg.GetSequenceName();
				CString sCameraID;
				CString sCameraIDs;
				CString sSection;
				HTREEITEM hServer = GetTreeItem(wServerID);
				HTREEITEM hCam	  = hServer;
				HTREEITEM hChild;

				hChild = GetTreeCtrl().GetChildItem(hCam);
				while (hChild)
				{
					if (GetCheck(hChild))
					{
						sCameraID.Format(_T("%08lx"),(DWORD)GetTreeCtrl().GetItemData(hChild));
						sCameraIDs += sCameraID;
					}
					hChild = GetTreeCtrl().GetNextSiblingItem(hChild);
				}

				// now we have a name, cameras and a station
				sSection = _T("IdipClient\\Connections\\");
				sSection += sHost;
				CWK_Profile wkp;
				wkp.WriteString(sSection, sSequenceName, sCameraIDs);
				sSequenceName += _T("Time");
				wkp.WriteInt(sSection, sSequenceName, pDoc->GetSequenceIntervall());
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewCamera::OnHostLoadSequence(WORD wServerID) 
{
	// TODO! RKE: new Sequencer modus to be implemented
	if (wServerID != SERVERID_NOID)
	{
		CIdipClientDoc* pDoc = GetDocument();
		CServer* pServer = pDoc->GetServer(wServerID);
		if (pServer)
		{
			CString sHost = pServer->GetFullName();

			CDlgImpExpCamSequencer dlg(FALSE,this);
			dlg.m_sHost = sHost;

			if (IDOK==dlg.DoModal())
			{
				CString sSequenceName = dlg.GetSequenceName();
				CString sCameraID;
				CString sCameraIDs;
				CString sSection;
				DWORD   dwID;
				CWK_Profile wkp;
				CSecID id; 

				sSection = _T("IdipClient\\Connections\\");
				sSection += sHost;
				sCameraIDs = wkp.GetString(sSection, sSequenceName, _T(""));
				sSequenceName += _T("Time");
				pDoc->SetSequenceTime(wkp.GetInt(sSection, sSequenceName, pDoc->GetSequenceIntervall()));

				HTREEITEM hServer = GetTreeItem(wServerID);
				HTREEITEM hCam	  = hServer;
				HTREEITEM hChild;

				hChild = GetTreeCtrl().GetChildItem(hCam);
				while (hChild)
				{
					dwID = (DWORD)GetTreeCtrl().GetItemData(hChild);
					id = dwID;
					sCameraID.Format(_T("%08lx"),dwID);

					if (-1!=sCameraIDs.Find(sCameraID))
					{
						// turn on
						if (GetCheck(hChild))
						{
							// ok nothing to do
						}
						else
						{
							// turn on
							CIPCOutputIdipClient* pOutput = pServer->GetOutput();
							CIPCOutputRecord* pRec = pOutput->GetOutputRecordPtrFromSecID(id);
							if (pRec && pRec->IsCamera())
							{
								// Camera ON
								pDoc->UpdateAllViews(this,MAKELONG(VDH_ADD_CAMWINDOW,pServer->GetID()),(CObject*)pRec);
								SetCheck(hChild,TRUE);
							}
						}
					}
					else
					{
						if (GetCheck(hChild))
						{
							// turn off
							CIPCOutputIdipClient* pOutput = pServer->GetOutput();
							CIPCOutputRecord* pRec = pOutput->GetOutputRecordPtrFromSecID(id);
							if (pRec && pRec->IsCamera())
							{
								// Camera OFF
								pDoc->UpdateAllViews(this,MAKELONG(VDH_REMOVE_CAMWINDOW,pServer->GetID()),
													(CObject*)pRec);
								pOutput->DeactivateCamera(id);
								SetCheck(hChild,FALSE);
							}
						}
						else
						{
							// ok nothing to do
						}
					}

					hChild = GetTreeCtrl().GetNextSiblingItem(hChild);
				}  // while
				pDoc->SetSequence(TRUE);
			} // IDOK
		} //pServer
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewCamera::OnSpace() 
{
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	
	if (hItem)
	{
		CSecID id = (DWORD)GetTreeCtrl().GetItemData(hItem);
		if (id.IsOutputID())
		{
			CIdipClientDoc* pDoc = GetDocument();
			HTREEITEM hParent = GetTreeCtrl().GetParentItem(hItem);
			CServer* pServer = pDoc->GetServer(LOWORD(GetTreeCtrl().GetItemData(hParent)));

			if (pServer)
			{
				CIPCOutputIdipClient* pOutput = pServer->GetOutput();
				CIPCOutputRecord* pRec = pOutput->GetOutputRecordPtrFromSecID(id);

				if (pRec && pRec->IsCamera())
				{
					BOOL bOn = GetCheck(hItem);
					if (!bOn)
					{
						// Camera ON
						pDoc->UpdateAllViews(this,MAKELONG(VDH_ADD_CAMWINDOW,pServer->GetID()),(CObject*)pRec);
						bOn = TRUE;
					}
					else
					{
						// Camera OFF
						pDoc->UpdateAllViews(this,MAKELONG(VDH_REMOVE_CAMWINDOW,pServer->GetID()),
											(CObject*)pRec);
						bOn = FALSE;
					}
					SetCheck(hItem,bOn);
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CViewCamera::OnButtonNumBlock(UINT nID)
{
	WORD wCurrentCamFromButton = 0;
	BOOL bCamActivated = FALSE;

	//open camera (nID), set active and show
	//or switch to camera (nID)
	if(nID == ID_BUTTON_NUMPLUS)
	{
		if(m_bCamNrFirstDigit) //first of two digits already entered
		{
			m_bCamNrFirstDigit = !m_bCamNrFirstDigit;
		}
		else
		{
			//check if there are cameras with 2 digits (10..64)
			if (GetNrOfCamerasOfSelectedServer()>9) 
			{
				//camera excists with 2 digits (10..64)
				m_bCamNrFirstDigit = !m_bCamNrFirstDigit;
				bCamActivated = TRUE;
			}
		}
		//return; //do nothing, wait for second digit
	}
	else if(    nID >= ID_BUTTON_NUM1 
		&& nID <= ID_BUTTON_NUM0)
	{
		wCurrentCamFromButton = (WORD)nID - ID_BUTTON_NUM1;
		if(m_bCamNrFirstDigit)
		{
			if(m_wCamNrFirstDigit > 0)
			{
				//first digit is already  set
				if(nID == ID_BUTTON_NUM0)
				{
					wCurrentCamFromButton = m_wCamNrFirstDigit*10 - 1;
				}
				else
				{
					wCurrentCamFromButton = m_wCamNrFirstDigit*10 + wCurrentCamFromButton;
				}
				m_wCamNrFirstDigit = 0;
				m_bCamNrFirstDigit = FALSE;
				bCamActivated = FindCamInTreeAndActivate(wCurrentCamFromButton);
			}
			else
			{
				//save first digit of two digits for cameras (10..64)
				if(nID != ID_BUTTON_NUM0)
				{
					m_wCamNrFirstDigit = wCurrentCamFromButton+1;
					bCamActivated = TRUE;
				}
			}
		}
		else //only one digit camera (1..9)
		{
			if(nID != ID_BUTTON_NUM0)
			{
				bCamActivated = FindCamInTreeAndActivate(wCurrentCamFromButton);
			}
		}
	}

	if(bCamActivated)
	{
		CViewIdipClient* pVV = theApp.GetMainFrame()->GetViewIdipClient();
		if(pVV && pVV->Is1x1())
		{
			//if view 1-mode is active, switch new active cam into 1-mode
			pVV->SetView1();
		}
	}
	else
	{			
		if(!MessageBeep(MB_ICONEXCLAMATION))//for soundcards
		{
			Beep( 440, 100); //no soundcard
		} 
	}
}
//////////////////////////////////////////////////////////////////////////
void CViewCamera::OnUpdateButtonNumBlock(CCmdUI* pCmdUI)
{	
	if(pCmdUI->m_nID == ID_BUTTON_NUMPLUS)
	{
		pCmdUI->SetCheck(m_bCamNrFirstDigit);
	}
	pCmdUI->Enable(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CViewCamera::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM hItem;

	if (pNMTreeView->action==TVC_BYKEYBOARD)
	{
		hItem = pNMTreeView->itemNew.hItem;
		// camera reactivate on text
		if (hItem)
		{
			BOOL bOn = GetCheck(hItem);
			TRACE(_T("select %s \n"),GetTreeCtrl().GetItemText(hItem));
			if (bOn)
			{
				ReactivateCamera(hItem);
			}
			else
			{
				ActivateCamera(hItem);
			}
		}
	}
// never do this, special function handling for mouse OnClick/OnRClick
/*	else if (pNMTreeView->action==TVC_BYMOUSE)
	{
		hItem = pNMTreeView->itemNew.hItem;
		// camera reactivate on mouse click
		if (hItem)
		{
			BOOL bOn = GetCheck(hItem);
			TRACE(_T("select %s \n"),GetTreeCtrl().GetItemText(hItem));
			if (bOn)
			{
				ReactivateCamera(hItem);
			}
			else
			{
				ActivateCamera(hItem);
			}
		}
	}
*/	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CViewCamera::Select(WORD wServerID, HTREEITEM hItem, CSecID id, BOOL bpOn /*= TRUE*/)
{
	ASSERT(id.IsOutputID());
	if (bpOn)
	{
		BOOL blOn = GetCheck(hItem);
		if (blOn)
		{
			ReactivateCamera(hItem);
		}
		else
		{
			CIdipClientDoc* pDoc = GetDocument();
			HTREEITEM hParent = GetTreeCtrl().GetParentItem(hItem);
			CServer* pServer = pDoc->GetServer(LOWORD(GetTreeCtrl().GetItemData(hParent)));

			if (pServer)
			{
				CIPCOutputIdipClient* pOutput = pServer->GetOutput();
				CIPCOutputRecord* pRec = pOutput->GetOutputRecordPtrFromSecID(id);

				if (pRec)
				{
					// Camera ON
					SetCheck(hItem,TRUE);
					pDoc->UpdateAllViews(this,MAKELONG(VDH_ADD_CAMWINDOW,pServer->GetID()),(CObject*)pRec);
				}
			}
		}
	}
	else
	{
		SwitchCamera(hItem,bpOn);
	}
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM	CViewCamera::GetTreeItem(WORD wServer, const CString& s)
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
			HTREEITEM hCamera;
			hCamera = hServer;
			hChild = theTree.GetChildItem(hCamera);
			while (hChild)
			{
				if (0==s.CompareNoCase(theTree.GetItemText(hChild)))
				{
					return hChild;// found camera
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
HTREEITEM	CViewCamera::GetTreeItem(WORD wServer, DWORD dwID)
{
	HTREEITEM hServer;
	HTREEITEM hChild;
	HTREEITEM hCamera;
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

			hCamera = hServer;
			hChild = theTree.GetChildItem(hCamera);
			while (hChild)
			{
				if (dwID==theTree.GetItemData(hChild))
				{
					return hChild;// found camera
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
void CViewCamera::InsertOutputs(CIPCOutputIdipClient* pOutput)
{
	HTREEITEM hServer;
	CServer*  pServer = pOutput->GetServer();
	WORD wServerID = pServer->GetID();
	int bFirstCam = theApp.m_bFirstCam;				// use a copy it may be modified
	CTreeCtrl &theTree = GetTreeCtrl();
	CIdipClientDoc* pDoc = GetDocument();
	CSecID idFirstCam(SECID_NO_ID);

	// insert server if not in tree
	hServer = GetTreeItem(wServerID);
	if (hServer==NULL)								// no Server there
	{
		hServer = InsertServer(pServer);			// insert it
		CSecID idAlarm = pServer->GetAlarmID();
		if (idAlarm.IsOutputID())					// Alarm Call
		{
			idFirstCam = idAlarm;					// select alarm camera
		}
		else if (m_idFirstCam != SECID_NO_ID)		// shall a destinct camera be the first camera
		{
			bFirstCam = TRUE;						// switch of the 'all cameras to begin mode'
			idFirstCam = m_idFirstCam;				// set the first cam to the destinct value
		}
	}
	else											// Server already there
	{
		bFirstCam  = TRUE;								// do not set first cam or all cams to begin
		idFirstCam = SECID_NO_ID-1;					// by this combination of parameters
	}
	// this overwrites the camera switching before: used in special cases (SetFirstCamID(..);) !
	
	m_idFirstCam = SECID_NO_ID;						// reset the destinct Cam ID

	// insert camera if not in tree
	if (hServer)
	{
		HTREEITEM hChild;
		HTREEITEM hDelete = NULL;
		CSecID idOutput;
		CIPCOutputRecord* pRecord = NULL;

		// bestehende Tree Items pruefen Camera
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
					else
					{ // Flags (e.g. _T("Reference image") may be updated, rec has to be updated!
						
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
				CSecID id = (DWORD)theTree.GetItemData(hDelete);
				theTree.DeleteItem(hDelete);
				// Camera OFF
				pDoc->UpdateAllViews(this,MAKELONG(VDH_DELETE_CAMWINDOW,wServerID),
									(CObject*)id.GetID());
			}
		}

		// Outputs einfügen
		int i,c;
		HTREEITEM hFirstCam = NULL;
		c = pOutput->GetNumberOfOutputs();

		for (i=0;i<c;i++)
		{
			const CIPCOutputRecord rec = pOutput->GetOutputRecordFromIndex(i);
			TRACE(_T("output %s %s %d, %08lx\n"),pServer->GetName(),rec.GetName(),i,rec.GetID().GetID());
			if (rec.IsCamera())
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
			if (hChild)
			{
				// actualize it
				ActualizeOutput(hChild, rec);
				if (   (idFirstCam==SECID_NO_ID)
					&& rec.IsCamera()
					&& rec.IsEnabled()
					&& rec.IsOkay()
					)
				{
					idFirstCam = rec.GetID();
				}
			}
			else
			{
				HTREEITEM hCam = InsertOutput(hServer,rec);
				if (   (idFirstCam==SECID_NO_ID)
					&& rec.IsCamera()
					&& rec.IsEnabled()
					&& rec.IsOkay()
					)
				{
					idFirstCam = rec.GetID();
					hFirstCam = hCam;
				}
			}
		}
		
		if (pServer->IsAlarm())
		{
			CSecID id = pServer->GetAlarmID();
			if (id.IsOutputID())
			{
				idFirstCam = id;
			}
			else
			{
				idFirstCam = SECID_NO_ID;
			}
		}
		else if (pServer->IsControl())
		{
			CSecID id = pServer->GetOutput()->GetCamID(pServer->GetAlarmID().GetSubID());
			if (id.IsOutputID())
			{
				idFirstCam = id;
			}
			else
			{
				idFirstCam = SECID_NO_ID;
			}
		}

		// switch view mode and cameras
		CViewIdipClient* pView = theApp.GetMainFrame()->GetViewIdipClient();
		// Set view mode 1plus at begin?
		if (   theApp.m_b1PlusAlarm
			&& (   pServer->IsAlarm()
			    || pServer->IsControl()))
		{
			if (!pView->IsView1plus())
			{
				pView->PostMessage(WM_COMMAND, ID_VIEW_1_PLUS);
			}
		}

		// Set fullscreen mode at begin?
		if (   theApp.m_bFullscreenAlarm
			&& pServer->IsAlarm())
		{
			theApp.GetMainFrame()->ChangeFullScreenModus(TRUE|FULLSCREEN_FORCE|FULLSCREEN_POST);
		}

		if (  (bFirstCam==TRUE)
			|| (   pServer->IsControl()
			    && pServer->IsControlExclusive())
			)
		{
			// switch to first camera
			if (idFirstCam!=SECID_NO_ID)
			{
				HTREEITEM hItem = GetTreeItem(wServerID,idFirstCam.GetID());
				if (hItem)
				{
					SwitchCamera(hItem,TRUE);
					theTree.SelectItem(hItem);
				}
			}
			else
			{
				HTREEITEM hCam = theTree.GetChildItem(hServer);
				if(hCam)
				{
					idOutput = (DWORD)theTree.GetItemData(hCam);
					if (idOutput.IsOutputID())
					{
						pRecord = pOutput->GetOutputRecordPtrFromSecID(idOutput);
						if (   pRecord 
							&& pRecord->IsEnabled()
							&& pRecord->IsCamera())
						{
							SwitchCamera(hCam,TRUE);
							theTree.SelectItem(hCam);
						}
					}
				}
			}
		}
		else if (bFirstCam==ALL_CAMS)
		{
			HTREEITEM hCam = theTree.GetChildItem(hServer);
			CViewIdipClient* pVV = theApp.GetMainFrame()->GetViewIdipClient();

			// switch to all cams
			if (!pServer->IsDTS())
			{
				if (!pServer->IsMultiCamera())
				{
					pDoc->SetSequence(TRUE);
				}
			}

			pVV->LockSmallWindowResizing(TRUE);
			hCam = theTree.GetChildItem(hServer);
			while (hCam)
			{
				idOutput = (DWORD)theTree.GetItemData(hCam);
				if (idOutput.IsOutputID())
				{
					pRecord = pOutput->GetOutputRecordPtrFromSecID(idOutput);
					if (   pRecord 
						&& pRecord->IsEnabled()
						&& pRecord->IsCamera())
					{
						SwitchCamera(hCam,TRUE);
					}
				}
				hCam = theTree.GetNextSiblingItem(hCam);
			}

			if (   (   pServer->IsAlarm()
				    || pServer->IsControl()
					)
				&& idFirstCam.IsOutputID())
			{
				HTREEITEM hItem = GetTreeItem(wServerID, idFirstCam.GetID());
				if (hItem)
				{
					SwitchCamera(hItem,TRUE, TRUE);
					theTree.SelectItem(hItem);
				}
			}
			pVV->LockSmallWindowResizing(FALSE);
		}
		else if (bFirstCam == NO_CAM)
		{
			// do nothing
		}


		// show camera map if any and if server is local

		if (pServer->IsLocal() && theApp.m_bShowCamereaMapAtConnection)
		{
			pDoc->UpdateAllViews(this, MAKELONG(VDH_CAMERA_MAP, wServerID), NULL);
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
					pDoc->UpdateMapElement(ServerID, strID, dwUpdate);
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
/////////////////////////////////////////////////////////////////////////////
BOOL CViewCamera::InsertKnownHosts(BOOL bInsert)
{
	BOOL bChanged = FALSE;
	if (bInsert)
	{
		CHostArray	&ha = theApp.GetHosts();
		CHost		*ph;
		int			i, nSize = ha.GetCount();
		WORD		wHostID, wTempHost, wServerID = 0;
		HTREEITEM	hServer;
		CTreeCtrl	&theTree = GetTreeCtrl();
		CString		sName;
		sName.LoadString(IDS_LOCAL_SERVER);
		CHost		LocalHost(sName, _T("0815"));

		for (i=-1; i<nSize; i++)
		{
			if (i==-1)
			{
				ph = &LocalHost;
				CSecID id = SECID_LOCAL_HOST;
				wHostID = id.GetSubID();
			}
			else
			{
				ph = ha.GetAtFast(i);
				wHostID = ph->GetID().GetSubID();
			}
			if (!ph->IsISDN() && !ph->IsTCPIP() && !ph->IsRouting())
			{
				continue;
			}

			hServer = theTree.GetRootItem();
			while (hServer)
			{
				wServerID = GetServerIDs(hServer, &wTempHost);
				if (wTempHost == wHostID)
				{
					break;
				}
				hServer = theTree.GetNextSiblingItem(hServer);
			}
			if (hServer==NULL)								// insert it
			{
				wServerID++;
				CString s = ph->GetName();
				TV_INSERTSTRUCT tvis;
				tvis.hParent     = TVI_ROOT;
				tvis.item.lParam = MAKELONG(wServerID, wHostID);
				tvis.hInsertAfter      = TVI_LAST;
				tvis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
				tvis.item.cchTextMax = s.GetLength();
				tvis.item.pszText    = (LPTSTR)LPCTSTR(s);
				tvis.item.iImage = _IMAGE_HOSTBASE + ((wServerID-1) % _IMAGE_NR_HOST_COLORS);
				tvis.item.iSelectedImage = tvis.item.iImage;
				hServer = GetTreeCtrl().InsertItem(&tvis);
				bChanged = TRUE;
			}
		}
	}
	else
	{
		WORD		wTempHost, wServerID = 0;
		HTREEITEM	hServer;
		CTreeCtrl	&theTree = GetTreeCtrl();
		CPtrList	list;
		hServer = theTree.GetRootItem();
		while (hServer)
		{
			wServerID = GetServerIDs(hServer, &wTempHost);
			wServerID = GetDocument()->GetServerIDbyHostID(wTempHost, FALSE);

			if (wServerID == SECID_NO_SUBID)
			{
				list.AddHead((void*)hServer);
				bChanged = TRUE;
			}
			hServer = theTree.GetNextSiblingItem(hServer);
		}
		while (list.GetCount())
		{
			hServer = (HTREEITEM)list.RemoveHead();
			theTree.DeleteItem(hServer);
		}
	}
	return bChanged;
}
