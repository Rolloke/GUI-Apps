// ViewCamera.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "MainFrm.h"

#include "ViewAudio.h"
#include "IdipClientDoc.h"

#include "DlgAudio.h"
#include "CIPCAudioIdipClient.h"

#include "images.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CViewAudio, CViewControllItems)

CViewAudio::CViewAudio()
{
	m_bDuplex            = FALSE;
	if (theApp.IsReadOnlyModus())
	{
		m_eVisibility = OTVS_Never;
	}
	m_eVisibility = OTVS_IfNotEmpty;
}

CViewAudio::~CViewAudio()
{
}

BEGIN_MESSAGE_MAP(CViewAudio, CViewControllItems)
	//{{AFX_MSG_MAP(CViewAudio)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_MESSAGE(WM_NOTIFY_WINDOW, OnNotifyWindow)
	ON_MESSAGE(WM_USER, OnUser)
	ON_COMMAND(ID_AU_START_DUPLEX_PLAY, OnAuStartDuplexPlay)
	ON_UPDATE_COMMAND_UI(ID_AU_START_DUPLEX_PLAY, OnUpdateAuStartDuplexPlay)
	ON_COMMAND(ID_AU_STOP_ANY, OnAuStopAny)
	ON_UPDATE_COMMAND_UI(ID_AU_STOP_ANY, OnUpdateAuStopAny)
	ON_COMMAND(ID_AU_START_PLAY, OnAuStartPlay)
	ON_UPDATE_COMMAND_UI(ID_AU_START_PLAY, OnUpdateAuStartPlay)
	ON_COMMAND(ID_AU_STOP_PLAY, OnAuStopPlay)
	ON_UPDATE_COMMAND_UI(ID_AU_STOP_PLAY, OnUpdateAuStopPlay)
	ON_COMMAND(ID_AU_START_REC, OnAuStartRec)
	ON_UPDATE_COMMAND_UI(ID_AU_START_REC, OnUpdateAuStartRec)
	ON_COMMAND(ID_AU_STOP_REC, OnAuStopRec)
	ON_UPDATE_COMMAND_UI(ID_AU_STOP_REC, OnUpdateAuStopRec)
	ON_COMMAND(ID_AU_PROPERTIES, OnAuProperties)
	ON_UPDATE_COMMAND_UI(ID_AU_PROPERTIES, OnUpdateAuProperties)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
void CViewAudio::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	WORD cause	  = LOWORD(lHint);
//	WORD wServerID = HIWORD(lHint);
	CSecID id;
//	HTREEITEM hChild;

	switch (cause)
	{
	case VDH_AUDIO:
		ASSERT_KINDOF(CIPCAudioIdipClient, pHint);
		InsertAudios((CIPCAudioIdipClient*)pHint);
		break;
	case VDH_SET_AUDIO_INPUT_STATE:
		{
			ASSERT_KINDOF(CAudioUpdateHint, pHint);
			CIPCAudioIdipClient *pLocalAudio = GetDocument()->GetLocalAudio();
			CIPCAudioIdipClient *pSendingAudio = ((CAudioUpdateHint*)pHint)->GetAudio();
			if (pSendingAudio == pLocalAudio)
			{
				CIPCMediaRecord *pDefOut = pLocalAudio->GetMediaRecordPtrFromSecID(pLocalAudio->GetDefaultOutputID());
				if (pDefOut)
				{
					CIdipClientDoc* pDoc = GetDocument();
					int nServer=0;
					CServer* pServer = pDoc->GetServerFromIndex(nServer++);

					while (pServer)
					{
						CIPCAudioIdipClient*pAudio = pServer->GetAudio();
						if (   pAudio 
							&& (   pAudio != pLocalAudio 
								|| pSender != this))
						{
							if (pAudio->GetDestination(0) != pLocalAudio)
							{
								int i, n = pAudio->GetNumberOfMedia();
								for (i=0; i<n; i++)
								{
									const CIPCMediaRecord &recS = pAudio->GetMediaRecordFromIndex(i);
									if (recS.IsInput())
									{
										((CIPCMediaRecord*)&recS)->SetEnabled(!pDefOut->IsRendering());
										pAudio->OnUpdateRecord(recS);
									}
								}
							}
						}
						pServer = pDoc->GetServerFromIndex(nServer++);
					}
				}
			}
		} break;
	default:
		CViewControllItems::OnUpdate(pSender, lHint, pHint);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
int CViewAudio::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CViewControllItems::OnCreate(lpCreateStruct) == -1)
		return -1;

	CString sText;
	sText.LoadString(IDS_AUDIO);
	SetWindowText(sText);
	m_btnTitle.SetWindowText(sText);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CViewAudio::InsertAudios(CIPCAudioIdipClient* pAudio)
{
	HTREEITEM hServer;
	if (GetDocument()->GetLocalAudio() == NULL)
	{
		return;		// it is disabled anyway
	}

	WORD wServerID = pAudio->GetServerID();
	CServer*  pServer = NULL;
	if (wServerID == SECID_NO_SUBID)
	{
		pServer = GetDocument()->GetLocalServer();
	}
	else
	{
		pServer = GetDocument()->GetServer(wServerID);
	}
	if (pServer == NULL)
	{
		return;		// 
	}
	wServerID = pServer->GetID();

	CTreeCtrl& theTree = GetTreeCtrl();

	// insert server if not in tree
	hServer = GetTreeItem(wServerID);
	if (hServer==NULL)
	{
		hServer = InsertServer(pServer);
	}

	// insert Audio input and output channels if not in tree
	if (hServer)
	{
		CSecID           idAudio;
		HTREEITEM        hChild;
		HTREEITEM        hDelete = NULL;
		HTREEITEM        hAudio  = hServer; // GetAudioTreeItem(hServer);
		int              nAudio  = pAudio->GetNumberOfMedia();
		CIPCMediaRecord *pRecord = NULL;
		CAutoCritSec cs(pAudio->GetCSAudio());
		if (hAudio)
		{
			// check existing tree items for Audio
			hChild = theTree.GetChildItem(hAudio);
			while (hChild)
			{
				idAudio = (DWORD)theTree.GetItemData(hChild);
				hDelete = NULL;
				if (idAudio.IsMediaID())
				{
					pRecord = pAudio->GetMediaRecordPtrFromSecID(idAudio);
					if (pRecord)
					{
						if (!pRecord->IsOkay())
						{
							hDelete = hChild;
						}
						else
						{
							
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
					// Audio OFF
					GetDocument()->UpdateAllViews(this,MAKELONG(VDH_DELETE_CAMWINDOW,wServerID), (CObject*)id.GetID());
				}
			}
		}
		// insert Items
		for (int i=0; i<nAudio; i++)
		{
			const CIPCMediaRecord &rec = pAudio->GetMediaRecordFromIndex(i);
			if (rec.IsEnabled())
			{
				hChild = GetTreeItem(wServerID, rec.GetID().GetID());
				if (hChild)
				{
					ActualizeAudio(hChild, rec);
				}
				else
				{
					pAudio->DoRequestHardware(rec.GetID(), 0);
					CIPCMediaRecord*pRec = (CIPCMediaRecord*) &rec;
					InsertAudio(hServer, *pRec);
				}
			}
		}

		if (theTree.GetChildItem(hServer) == NULL)
		{
			theTree.DeleteItem(hServer);
		}
		else
		{
			theTree.Expand(hAudio, TVE_EXPAND);
			theTree.SelectItem(hServer);
			if (m_idSelectMedia != SECID_NO_ID)
			{
				CViewControllItems::Select(wServerID, m_idSelectMedia, TRUE);
				m_idSelectMedia = SECID_NO_ID;
			}
			CheckVisibility();
		}
	}
}
//////////////////////////////////////////////////////////////////////////
BOOL CViewAudio::ActualizeAudio(HTREEITEM hAudio, const CIPCMediaRecord& rec)
{
	//	WK_TRACE(_T("ActualizeAudio:%08x\n"), rec.GetID().GetID());
	if (hAudio)
	{
		int nImage = GetImageIndex(rec);
		GetTreeCtrl().SetItemImage(hAudio, nImage, nImage);
	}
	BOOL bCheck = FALSE;
	if (rec.IsInput())
	{
		bCheck = rec.IsCapturing();
	}
	else if (rec.IsOutput())
	{
		CIdipClientDoc* pDoc = GetDocument();
		int nServer=0;
		CServer* pServer = pDoc->GetServerFromIndex(nServer++);
		while (pServer)
		{
			if (!pServer->IsLocal())
			{
				CIPCAudioIdipClient*pAudio = pServer->GetAudio();
				if (pAudio)
				{
					int i, n = pAudio->GetNumberOfMedia();
					for (i=0; i<n; i++)
					{
						const CIPCMediaRecord &recS = pAudio->GetMediaRecordFromIndex(i);
						if (recS.IsInput())
						{
							pAudio->OnUpdateRecord(recS);
						}
					}
				}
			}
			pServer = pDoc->GetServerFromIndex(nServer++);
		}
		
		bCheck = rec.IsRendering();
	}

	if (hAudio)
	{
		SetCheck(hAudio, bCheck);
		UpdateCameraPage(hAudio);
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CViewAudio::Actualize(WORD wServerID, CSecID id)
{
	HTREEITEM hChild = GetTreeItem(wServerID, id.GetID());

	ASSERT(id.IsMediaID());

	if (hChild)
	{
		CServer* pServer = GetDocument()->GetServer(wServerID);
		if (pServer)
		{
			CIPCAudioIdipClient* pAudio = pServer->GetAudio();
			if (pAudio)
			{
				CAutoCritSec cs(pAudio->GetCSAudio());
				CIPCMediaRecord* pRecord = pAudio->GetMediaRecordPtrFromSecID(id);
				if (pRecord)
				{
					ActualizeAudio(hChild,*pRecord);
				}
			}
		}
	}
	else
	{
		CIPCAudioIdipClient* pAudio = GetDocument()->GetLocalAudio();
		if (pAudio)
		{
			CAutoCritSec cs(pAudio->GetCSAudio());
			CIPCMediaRecord* pRecord = pAudio->GetMediaRecordPtrFromSecID(id);
			if (pRecord)
			{
				ActualizeAudio(hChild, *pRecord);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
HTREEITEM CViewAudio::InsertAudio(HTREEITEM hServer, CIPCMediaRecord&rec)
{
	HTREEITEM hItem, hAudio = hServer; // GetAudioTreeItem(hServer, true);
	CTreeCtrl& theTree = GetTreeCtrl();
	if (hAudio)
	{
		WORD wHost, wServer = GetServerIDs(hServer, &wHost);
		CServer *pServer = GetServer(wServer);
		CIPCAudioIdipClient*pAudio = NULL;
		if (pServer)
		{
			pAudio = pServer->GetAudio();
			if (pAudio)
			{	// Initialisierung der ReplyID
				pAudio->DoRequestValues(rec.GetID(), MEDIA_GET_VALUE|MEDIA_STATE, 0); 
			}
		}

		TV_INSERTSTRUCT tvis;
		int nImage = GetImageIndex(rec);
		CString s  = rec.GetName();
		tvis.item.mask    = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		tvis.hInsertAfter = TVI_LAST;
		tvis.hParent      = hAudio;
		tvis.item.lParam  = rec.GetID().GetID();
		tvis.item.cchTextMax = s.GetLength();
		tvis.item.pszText = (LPTSTR)LPCTSTR(s);
		tvis.item.iImage = nImage;
		tvis.item.iSelectedImage = nImage;

		hItem = theTree.InsertItem(&tvis);
		if (hItem)
		{
			SetCheck(hItem, FALSE);
			UpdateCameraPage(hItem);
		}

		return hItem;
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CViewAudio::Add(WORD wServerID, CSecID id)
{
	HTREEITEM hChild = GetTreeItem(wServerID,id.GetID());

	if (hChild == NULL)
	{
		// found input or output
		CServer* pServer = GetDocument()->GetServer(wServerID);
		ASSERT(id.IsMediaID()); // ausgelagert
		if (pServer)		
		{
			CIPCAudioIdipClient* pAudio = pServer->GetAudio();
			if (pAudio)
			{
				CIPCMediaRecord* pRecord = pAudio->GetMediaRecordPtrFromSecID(id);
				if (pRecord)
				{
					InsertAudio(hChild, *pRecord);
				}
			}
		}
	}
	else
	{
		Actualize(wServerID, id);
	}
}
/////////////////////////////////////////////////////////////////////////////
int CViewAudio::GetImageIndex(const CIPCMediaRecord& rec)
{
	int i = _IMAGE_OK;

	// 0=ALARM, 1=OK, 2=ERROR , 3=OFF
	if (rec.IsOkay()==FALSE) 
	{
		i = _IMAGE_ERROR;
	}
	else if (rec.IsEnabled())
	{
		if (rec.IsAudio())
		{
			if (rec.IsInput())
			{
				i = _IMAGE_AUDIO_REC_OFF;
				CIPCMediaRecord *pRec = GetDefaultOutputRecord(GetDocument()->GetLocalAudio());
				if (pRec)
				{
					if (pRec->IsEnabled())
					{
						i = rec.IsDefault() ? _IMAGE_AUDIO_REC_DEF  : _IMAGE_AUDIO_REC;
					}
				}
			}
			if (rec.IsOutput())	
			{
				i = rec.IsDefault() ? _IMAGE_AUDIO_PLAY_DEF : _IMAGE_AUDIO_PLAY;
			}
		}
	}
	else 
	{
		if (rec.IsInput())
		{
			i = _IMAGE_AUDIO_REC_OFF;
		}
		if (rec.IsOutput())	
		{
			i = _IMAGE_AUDIO_PLAY_OFF;	// not enabled
		}
	}
	return i;
}
//////////////////////////////////////////////////////////////////////////
void CViewAudio::SwitchAudio(HTREEITEM hItemAudio, BOOL bOn)
{
	CTreeCtrl&  theTree = GetTreeCtrl();
	WORD wHost, wServer;
	if (hItemAudio != theTree.GetSelectedItem())
	{
		theTree.SelectItem(hItemAudio);
	}
	CIPCMediaRecord*pMR = GetSelectedItemsMediaRecord(wHost, wServer, hItemAudio);
	if (pMR)
	{
		if (pMR->IsInput())
		{
			CStateCmdUI start, stop;
			OnUpdateAuStartRec(&start);
			OnUpdateAuStopRec(&stop);
			if (start.m_bEnabled)
			{
				OnAuStartRec(hItemAudio);
			}
			if (stop.m_bEnabled)
			{
				OnAuStopRec(hItemAudio);
			}
		}
		else if (pMR->IsOutput())
		{
			CStateCmdUI start, stop;
			OnUpdateAuStartPlay(&start);
			OnUpdateAuStopPlay(&stop);
			if (start.m_bEnabled)
			{
				OnAuStartPlay(hItemAudio);
			}
			if (stop.m_bEnabled)
			{
				OnAuStopPlay(hItemAudio);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
CIPCMediaRecord* CViewAudio::GetDefaultInputRecord(CIPCAudioIdipClient *pAudio)
{
	if (pAudio && pAudio->GetDefaultInputID() != SECID_NO_ID)
	{
		const CIPCMediaRecord &rec = pAudio->GetMediaRecordFromSecID(pAudio->GetDefaultInputID());
		return (CIPCMediaRecord*)&rec;
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
CIPCMediaRecord* CViewAudio::GetDefaultOutputRecord(CIPCAudioIdipClient *pAudio)
{
	if (pAudio)
	{
		if (pAudio && pAudio->GetDefaultOutputID() != SECID_NO_ID)
		{
			const CIPCMediaRecord &rec = pAudio->GetMediaRecordFromSecID(pAudio->GetDefaultOutputID());
			return (CIPCMediaRecord*)&rec;
		}
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
CIPCMediaRecord* CViewAudio::GetActiveRecord(CIPCAudioIdipClient *pAudio, DWORD dwFlags)
{
	if (pAudio)
	{
		int i, nCount = pAudio->GetNumberOfMedia();
		for (i=0; i<nCount; i++)
		{
			const CIPCMediaRecord &rec = pAudio->GetMediaRecordFromIndex(i);
			if (  (dwFlags & MEDIA_INPUT  && rec.IsCapturing())
				|| (dwFlags & MEDIA_OUTPUT && rec.IsRendering()))
			{
				return (CIPCMediaRecord*)&rec;
			}
		}
	}
	return NULL;
}
/*********************************************************************************************
 Class      : CViewAudio
 Function   : OnAuStartPlay
 Description: Starts the announcement to an external hosts audio destination.

 Parameters: None 

 Result type:  (void)
 created: October, 09 2003
 <TITLE OnAuStartPlay>
*********************************************************************************************/
void CViewAudio::OnAuStartPlay()
{
	OnAuStartPlay(GetTreeCtrl().GetSelectedItem());
}
void CViewAudio::OnAuStartPlay(HTREEITEM hSelected) 
{
	CIPCAudioIdipClient *pLocalAudio = GetDocument()->GetLocalAudio();
	if (pLocalAudio && pLocalAudio->GetDefaultOutputID() != SECID_NO_ID)															// get local audio device
	{
		WORD wHost, wServer = GetSelectedServer(&wHost);
		CServer *pServer = GetServer(wServer);							// get external Server
		if (pServer)
		{
			CIPCAudioIdipClient *pAudio = pServer->GetAudio();	// get external audio device
			if (pAudio)
			{
				bool bLocal = false;
				// add the external audio device to the loca audio device with it´s SecID
				CIPCMediaRecord*pMRout = pAudio->GetMediaRecordPtrFromSecID(GetTreeCtrl().GetItemData(hSelected));
				if (pMRout == NULL || pMRout->IsInput())
				{
					pMRout = GetDefaultOutputRecord(pAudio);
					if (pMRout == NULL) return;
				}
				if (pAudio != pLocalAudio)
				{
					pLocalAudio->AddDestination(pAudio, pMRout->GetID());
				}
				else
				{
					bLocal = true;
					m_bDuplex = FALSE;
				}
				// start recording from the local audio device and force a new segment
				// to initialize the destinations audio format.
				pLocalAudio->StartRecord(pLocalAudio->GetDefaultInputID(), bLocal, 0);

				if (m_bDuplex)													// duplex connection
				{																	// set the default output device
					pAudio->AddDestination(pLocalAudio, pLocalAudio->GetDefaultOutputID());// as destination
					CIPCMediaRecord *pRec = NULL;
					int i, n = pAudio->GetNumberOfMedia();
					for (i=0; i<n; i++)										// find the default input
					{																// of the external audio device
						const CIPCMediaRecord &rec = pAudio->GetMediaRecordFromIndex(i);
						if (rec.IsInput() && rec.IsDefault())
						{
							pRec = (CIPCMediaRecord*)&rec;
							break;
						}
					}
					if (!pRec)													// if no default
					{
						for (i=0; i<n; i++)									// take the first input as default
						{															// of the external audio device
							const CIPCMediaRecord &rec = pAudio->GetMediaRecordFromIndex(i);
							if (rec.IsInput())
							{
								pRec = (CIPCMediaRecord*)&rec;
								break;
							}
						}
					}
					if (pRec)
					{
						// start recording from the local audio device and force a new segment
						// to initialize the destinations audio format.
						pAudio->StartRecord(pRec->GetID(), false, 0);
					}

					m_bDuplex = 2;
				}
			}
		}
	}
}
/*********************************************************************************************
 Class      : CViewAudio
 Function   : OnAuStopPlay
 Description: Stops an announcement to a host audio destination.

 Parameters: None 

 Result type:  (void)
 created: December, 12 2003
 <TITLE OnAuStopPlay>
*********************************************************************************************/
void CViewAudio::OnAuStopPlay()
{
	OnAuStopPlay(GetTreeCtrl().GetSelectedItem());
}
void CViewAudio::OnAuStopPlay(HTREEITEM hSelected) 
{
	WORD wHost, wServer = GetSelectedServer(&wHost, hSelected);
	CServer *pServer = GetServer(wServer);
	if (pServer)
	{
		CIPCAudioIdipClient *pAudio = pServer->GetAudio();
		if (pAudio)
		{
			CTreeCtrl &theCtrl =GetTreeCtrl();								// stop the external device
			pAudio->StopPlay(theCtrl.GetItemData(hSelected));
		}
	}
	
	if (m_bDuplex == 2)
	{
		CIPCAudioIdipClient *pLocalAudio = GetDocument()->GetLocalAudio();
		if (pLocalAudio)														// get local audio device
		{
			pLocalAudio->StopPlay(pLocalAudio->GetDefaultOutputID());
		}
		m_bDuplex = FALSE;
	}
}
/*********************************************************************************************
 Class      : CViewAudio
 Function   : OnAuStartRec
 Description: Starts to listen in an external hosts audio source.

 Parameters: None 

 Result type:  (void)
 created: December, 12 2003
 <TITLE OnAuStartRec>
*********************************************************************************************/
void CViewAudio::OnAuStartRec()
{
	OnAuStartRec(GetTreeCtrl().GetSelectedItem());
}
void CViewAudio::OnAuStartRec(HTREEITEM hSelected) 
{
	WORD wHost, wServer = GetSelectedServer(&wHost, hSelected);
	CServer *pServer = GetServer(wServer);
	if (pServer)																			// get the source server
	{
		CIPCAudioIdipClient *pAudio = pServer->GetAudio();
		if (pAudio)																			// and its audio connection
		{
			bool bLocal;
			if (wHost == SECSUBID_LOCAL_HOST)										// is it local?
			{
				bLocal = true;																// just start it
			}
			else																				// is it external?
			{
				bLocal = false;
				CIPCAudioIdipClient *pLocalAudio = GetDocument()->GetLocalAudio();
				if (pLocalAudio)															// get local audio device
				{	// and set it as destination to the external audio connection
					pAudio->AddDestination(pLocalAudio, pLocalAudio->GetDefaultOutputID());
				}
			}
			
			CIPCMediaRecord*pMRin = GetSelectedItemsMediaRecord(wHost, wServer, hSelected);
			if (pMRin == NULL || pMRin->IsOutput())
			{
				pMRin = GetDefaultInputRecord(pAudio);
				if (pMRin == NULL) return;
			}
			pAudio->StartRecord(pMRin->GetID(), bLocal, 0);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CViewAudio::OnUpdateAuStartRec(CCmdUI* pCmdUI) 
{
	WORD wHost, wServer;
	CIPCMediaRecord*pRecIn  = GetSelectedItemsMediaRecord(wHost, wServer);
	CIPCMediaRecord*pRecOut = GetDefaultOutputRecord(GetDocument()->GetLocalAudio());
	CServer *pServer = GetServer(wServer);
	if (pServer && (pRecIn == NULL || pRecIn->IsOutput()))
	{
		pRecIn = GetDefaultInputRecord(pServer->GetAudio());
	}

	if (pRecIn && pRecOut)
	{
		pCmdUI->Enable(!pRecIn->IsCapturing() && !pRecOut->IsRendering() && pRecOut->IsEnabled());
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
/*********************************************************************************************
 Class      : CViewAudio
 Function   : OnAuStopRec
 Description: Stops to listen in an external hosts audio source.

 Parameters: None 

 Result type:  (void)
 created: December, 12 2003
 <TITLE OnAuStopRec>
*********************************************************************************************/
void CViewAudio::OnAuStopRec()
{
	OnAuStopRec(GetTreeCtrl().GetSelectedItem());
}
void CViewAudio::OnAuStopRec(HTREEITEM hSelected) 
{
	WORD wHost;
	WORD wServer = GetSelectedServer(&wHost, hSelected);
	CServer *pServer = GetServer(wServer);
	if (pServer)
	{
		CIPCAudioIdipClient *pAudio = pServer->GetAudio();
		CIPCAudioIdipClient *pDest  = NULL;
		if (pAudio)
		{
			CTreeCtrl &theCtrl = GetTreeCtrl();
			pDest = (CIPCAudioIdipClient*) pAudio->GetDestination(0);
			CSecID secID(theCtrl.GetItemData(hSelected));
			pAudio->StopRecord(secID);
			Actualize(wServer, secID);
		}
		
		if (pDest && m_bDuplex == 2)
		{
			pDest->StopRecord(SECID_NO_ID);
			m_bDuplex = 0;
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CViewAudio::OnUpdateAuStopRec(CCmdUI* pCmdUI)
{
	WORD wHost, wServer;
	CIPCMediaRecord*pRec = GetSelectedItemsMediaRecord(wHost, wServer);
	if (pRec)
	{
		ASSERT(pRec->IsInput());
		pCmdUI->Enable(pRec->IsCapturing());
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
//////////////////////////////////////////////////////////////////////////
void CViewAudio::OnAuStartDuplexPlay() 
{
	m_bDuplex = TRUE;
	OnAuStartPlay(GetTreeCtrl().GetSelectedItem());
}
//////////////////////////////////////////////////////////////////////////
void CViewAudio::OnUpdateAuStartDuplexPlay(CCmdUI* pCmdUI) 
{
	OnUpdateAuStartPlay(pCmdUI);
}
/*********************************************************************************************
 Class      : CViewAudio
 Function   : OnAuStopAny
 Description: Stops any existing audio connection

 Parameters: None 

 Result type:  (void)
 created: December, 12 2003
 <TITLE OnAuStopAny>
*********************************************************************************************/
void CViewAudio::OnAuStopAny() 
{
	CIPCAudioIdipClient *pLocalAudio = GetDocument()->GetLocalAudio();
	CIPCMediaRecord*pAMR = GetActiveRecord(pLocalAudio, MEDIA_INPUT);
	if (pAMR)
	{
		pLocalAudio->StopRecord(pLocalAudio->GetDefaultInputID());
	}
	
	pAMR = GetActiveRecord(pLocalAudio, MEDIA_OUTPUT);
	if (pAMR)
	{
		pLocalAudio->StopPlay(pLocalAudio->GetDefaultOutputID());
	}
	m_bDuplex = FALSE;
}
//////////////////////////////////////////////////////////////////////////
void CViewAudio::OnUpdateAuStopAny(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetActiveRecord(GetDocument()->GetLocalAudio()) != NULL);
}
//////////////////////////////////////////////////////////////////////////
LRESULT CViewAudio::OnNotifyWindow(WPARAM dwCmd, LPARAM nID)
{
	if (MEDIA_COMMAND_VALUE(dwCmd) == MEDIA_STATE)
	{
		
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////
void CViewAudio::OnUpdateAuStartPlay(CCmdUI* pCmdUI) 
{
	WORD wHost, wServer;
	CIPCMediaRecord* pRecOut = GetSelectedItemsMediaRecord(wHost, wServer);

	if (pRecOut == NULL)
	{
		CTreeCtrl& theTree = GetTreeCtrl();
		HTREEITEM hSelectedItem = theTree.GetSelectedItem();
		if (hSelectedItem)
		{
			CSecID id(theTree.GetItemData(hSelectedItem));
			if (!id.IsMediaID())
			{
				CServer *pServer = GetServer(wServer);
				if (pServer)
				{
					pRecOut = GetDefaultOutputRecord(pServer->GetAudio());
				}
			}
		}
	}

	CIPCMediaRecord* pRecIn  = GetDefaultInputRecord(GetDocument()->GetLocalAudio());

	BOOL bEnable = FALSE;
	if (pRecOut && pRecIn && !pRecOut->IsRendering() && pRecOut->IsEnabled())
	{
		bEnable = TRUE;
	}
	pCmdUI->Enable(bEnable);
}
//////////////////////////////////////////////////////////////////////
void CViewAudio::OnAuProperties() 
{
	WORD wHost, wServer;
	CIPCMediaRecord*pMR = GetSelectedItemsMediaRecord(wHost, wServer);
	CServer *pServer = GetServer(wServer);
	if (pServer)
	{
		CIPCAudioIdipClient *pAudio = pServer->GetAudio();
		if (pMR == NULL)
		{
			pMR = GetDefaultInputRecord(pAudio);
		}
		CIPCAudioIdipClient *pLocalAudio = GetDocument()->GetLocalAudio();
		if (pMR && pAudio && pLocalAudio)
		{
			CWK_Profile wkp;
			if (wkp.GetInt(_T("Debug"), _T("TestAudioConnection"), 0) & EXTENDED_PROPERTIES)
			{
				CDlgAudio dlg(pAudio, pLocalAudio, this);
				dlg.SetSecID(pMR->GetID());
				dlg.m_bReduced = false;
				dlg.DoModal();
			}
			else
			{
				CDlgAudio dlg(pAudio, pLocalAudio, this);
				dlg.SetSecID(pMR->GetID());
				dlg.m_sInput = pServer->GetFullName();
				dlg.DoModal();
			}

			pServer = GetServer(wServer);	// Server erneut holen
			if (pServer)					// kann von aussen disconnected sein!
			{
				CIPCAudioIdipClient *pAudio = pServer->GetAudio();
				if (pAudio)
				{
					pAudio->SetNotifyWindow(this);
				}
			}
			pLocalAudio = GetDocument()->GetLocalAudio();
			if (pLocalAudio)
			{
				pLocalAudio->SetNotifyWindow(this);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CViewAudio::OnUpdateAuProperties(CCmdUI* pCmdUI) 
{
	WORD wHost, wServer;
	CIPCMediaRecord*pMR = GetSelectedItemsMediaRecord(wHost, wServer);
	CServer *pServer = GetServer(wServer);
	CWK_Profile wkp;
	if (pServer && pMR == NULL)
	{
		pMR = GetDefaultInputRecord(pServer->GetAudio());
	}
	if (wkp.GetInt(_T("Debug"), _T("TestAudioConnection"), 0) & EXTENDED_PROPERTIES)
	{
		pCmdUI->Enable(pServer != NULL && pServer->GetAudio());
	}
	else if (pMR && pServer != NULL && pServer->GetAudio())
	{
		pCmdUI->Enable(pMR->IsInput() && pMR->IsCaptureInitialized());
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewAudio::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint pt;
	GetCursorPos(&pt);
	HTREEITEM hItem;
	UINT nFlags;
	CTreeCtrl &theTree = GetTreeCtrl();

	ScreenToClient(&pt);
	hItem = theTree.HitTest(pt,&nFlags);

	// camera click on State Image
	if (hItem && (nFlags & TVHT_ONITEMSTATEICON))
	{
		CSecID id(theTree.GetItemData(hItem));
		ASSERT(id.IsMediaID());
		BOOL bOn = GetCheck(hItem);
		SwitchAudio(hItem, !bOn);
	}

	UpdateCameraPage(hItem, VDH_FLAGS_UPDATE|VDH_FLAGS_SELECTED);
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CViewAudio::OnUser(WPARAM wParam, LPARAM lParam) 
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
		case WPARAM_AUDIO_STOP_AUDIORECORD:
		{
			CIPCAudioIdipClient *pLocalAudio =pDoc->GetLocalAudio();
			if (pLocalAudio)
			{
				pLocalAudio->StopRecord(pLocalAudio->GetDefaultInputID());
			}
		} break;
		case WPARAM_AUDIO_FETCHRESULT:
			if (pServer)
			{
				pServer->OnAudioFetchResult();
			} 
			break;
		case WPARAM_AUDIO_CONNECTED:
			if (pServer)
			{
				pServer->OnAudioConnected();
			} 
			break;
		default:
			return CViewControllItems::OnUser(wParam, lParam);
	}
	return 0; 
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CViewAudio::GetTreeItem(WORD wServer, const CString& s)
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
			HTREEITEM hAudio;
			hAudio = hServer; // GetAudioTreeItem(hServer);
			hChild = theTree.GetChildItem(hAudio);
			while (hChild)
			{
				if (0==s.CompareNoCase(theTree.GetItemText(hChild)))
				{
					return hChild;// found Audio
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
HTREEITEM CViewAudio::GetTreeItem(WORD wServer, DWORD dwID /*=0*/)
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
			HTREEITEM hAudio;
			hAudio = hServer; // GetAudioTreeItem(hServer);
			hChild = theTree.GetChildItem(hAudio);
			while (hChild)
			{
				if (dwID==theTree.GetItemData(hChild))
				{
					return hChild;// found Audio
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
void CViewAudio::Select(WORD wServerID, HTREEITEM hItem, CSecID id, BOOL bpOn /*= TRUE*/)
{
	if (id.IsMediaID())
	{
		SwitchAudio(hItem, bpOn);
	}
	else
	{
		GetTreeCtrl().Select(hItem, TVGN_CARET);
	}
}
//////////////////////////////////////////////////////////////////////
void CViewAudio::OnUpdateAuStopPlay(CCmdUI* pCmdUI) 
{
	WORD wHost, wServer;
	CIPCMediaRecord *pRecOut = GetSelectedItemsMediaRecord(wHost, wServer);
	if (pRecOut)
	{
		ASSERT(pRecOut->IsOutput());
		pCmdUI->Enable(pRecOut->IsRendering() && pRecOut->IsEnabled());
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
