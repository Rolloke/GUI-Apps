// ObjectView.cpp : implementation file
//

#include "stdafx.h"
#include "Vision.h"
#include "MainFrm.h"

#include "ObjectView.h"

#include "images.h"

#include "VisionDoc.h"
#include "CIPCInputVision.h"
#include "Server.h"

#include "LoadSaveSequencerDialog.h"

#include "VisionView.h"
#include "DisplayWindow.h"

#include "AudioPropertiesDlg.h"
#include "AudioInputDlg.h"
#include "AudioOutputDlg.h"
#include "CIPCControlAudioUnit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAPPING_SERVER 0
#define MAPPING_EL_ID  1
/////////////////////////////////////////////////////////////////////////////
// CObjectView

#define STATIC_DEFAULT_DEV 1

#define TREE_CONTROL_CHECK_OFF	0
#define TREE_CONTROL_UNCHECKED	2
#define TREE_CONTROL_CHECKED	   1

IMPLEMENT_DYNCREATE(CObjectView, CTreeView)

CObjectView::CObjectView()
{
	m_pInputDeactivationDialog=NULL;

	m_bDuplex            = FALSE;
	m_pDragImage         = NULL;
	m_bDraggedInto       = FALSE;
	m_wLocalServerID     = SECID_NO_SUBID;
}

CObjectView::~CObjectView()
{
	WK_DELETE(m_pDragImage);
#ifdef _HTML_ELEMENTS
	DeleteLocalHTMLmappings();
#endif
}

BEGIN_MESSAGE_MAP(CObjectView, CTreeView)
	//{{AFX_MSG_MAP(CObjectView)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_COMMAND(ID_DISCONNECT, OnDisconnect)
	ON_COMMAND(ID_RELAIS_OPEN, OnRelaisOpen)
	ON_UPDATE_COMMAND_UI(ID_RELAIS_OPEN, OnUpdateRelaisOpen)
	ON_COMMAND(ID_RELAIS_CLOSE, OnRelaisClose)
	ON_UPDATE_COMMAND_UI(ID_RELAIS_CLOSE, OnUpdateRelaisClose)
	ON_COMMAND(ID_VIDEO_START, OnVideoStart)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_COMMAND(ID_VIDEO_ALARM_SWITCH, OnVideoAlarmSwitch)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_ALARM_SWITCH, OnUpdateVideoAlarmSwitch)
	ON_COMMAND(ID_HOST_SAVE_SEQUENCE, OnHostSaveSequence)
	ON_COMMAND(ID_HOST_LOAD_SEQUENCE, OnHostLoadSequence)
	ON_COMMAND(ID_INPUT_CONFIRM, OnInputConfirm)
	ON_UPDATE_COMMAND_UI(ID_INPUT_CONFIRM, OnUpdateInputConfirm)
	ON_COMMAND(ID_HOST_DATETIME, OnHostDatetime)
	ON_UPDATE_COMMAND_UI(ID_HOST_DATETIME, OnUpdateHostDatetime)
	ON_COMMAND(ID_HOST_CAMERA_MAP, OnHostCameraMap)
	ON_COMMAND(ID_INPUT_INFORMATION, OnInputInformation)
	ON_UPDATE_COMMAND_UI(ID_INPUT_INFORMATION, OnUpdateInputInformation)
	ON_COMMAND(ID_INPUT_DEACTIVATE, OnInputDeactivate) /*RM*/
	ON_UPDATE_COMMAND_UI(ID_INPUT_DEACTIVATE,OnUpdateInputDeactivate) /*RM*/
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_COMMAND(ID_AU_START_DUPLEX_PLAY, OnAuStartDuplexPlay)
	ON_UPDATE_COMMAND_UI(ID_AU_START_DUPLEX_PLAY, OnUpdateAuStartDuplexPlay)
	ON_COMMAND(ID_AU_STOP_ANY, OnAuStopAny)
	ON_UPDATE_COMMAND_UI(ID_AU_STOP_ANY, OnUpdateAuStopAny)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_HOST_CAMERA_MAP, OnUpdateHostCameraMap)
	ON_MESSAGE(WM_NOTIFY_WINDOW, OnNotifyWindow)
	ON_MESSAGE(WM_USER,OnUser)
	ON_UPDATE_COMMAND_UI(ID_AU_START_PLAY, OnUpdateAuStartPlay)
	ON_UPDATE_COMMAND_UI(ID_AU_STOP_PLAY, OnUpdateAuStopPlay)
	ON_UPDATE_COMMAND_UI(ID_AU_START_REC, OnUpdateAuStartRec)
	ON_UPDATE_COMMAND_UI(ID_AU_STOP_REC, OnUpdateAuStopRec)
	ON_UPDATE_COMMAND_UI(ID_AU_PROPERTIES, OnUpdateAuProperties)
	ON_COMMAND(ID_AU_START_PLAY, OnAuStartPlay)
	ON_COMMAND(ID_AU_STOP_PLAY, OnAuStopPlay)
	ON_COMMAND(ID_AU_START_REC, OnAuStartRec)
	ON_COMMAND(ID_AU_STOP_REC, OnAuStopRec)
	ON_COMMAND(ID_AU_PROPERTIES, OnAuProperties)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectView drawing
void CObjectView::OnDraw(CDC* pDC)
{
}
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
CVisionDoc* CObjectView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVisionDoc)));
	return (CVisionDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
BOOL CObjectView::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style = WS_CHILD | WS_VISIBLE | WS_BORDER | 
			   TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_DISABLEDRAGDROP;

	return CTreeView::PreCreateWindow(cs);
}
/////////////////////////////////////////////////////////////////////////////
// CObjectView message handlers
void CObjectView::OnInitialUpdate() 
{
	CTreeView::OnInitialUpdate();
	
	GetTreeCtrl().SetIndent(0);

	m_Images.Create(IDB_IMAGES,_IMAGE_WIDTH,0,RGB(0,255,255));
	GetTreeCtrl().SetImageList(&m_Images,TVSIL_NORMAL);

	m_States.Create(IDB_STATES,14,0,RGB(0,255,255));
	GetTreeCtrl().SetImageList(&m_States,TVSIL_STATE);

}
/////////////////////////////////////////////////////////////////////////////
BOOL CObjectView::SetCheck(HTREEITEM hItem,BOOL bFlag)
{
	TVITEM tvItem;

	tvItem.mask = TVIF_HANDLE | TVIF_STATE;
	tvItem.hItem = hItem;
	tvItem.stateMask = TVIS_STATEIMAGEMASK;

	CString s;
	s = GetTreeCtrl().GetItemText(hItem);
//	TRACE(_T("set %s to %s\n"),s,bFlag ? _T("ON") : _T("OFF"));
	/*
	Since state images are one-based, 1 in this macro turns the check off, and 
	2 turns it on.
	*/
	tvItem.state = INDEXTOSTATEIMAGEMASK((bFlag ? 1 : 2));


	BOOL bRet = GetTreeCtrl().SetItem(&tvItem);
#ifdef _HTML_ELEMENTS
	CSecID id(GetTreeCtrl().GetItemData(hItem));
	if (id.IsOutputID())
	{
		UpdateCameraPage(hItem);
	}
#endif
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CObjectView::GetCheck(HTREEITEM hItem)
{
	TVITEM tvItem;

	tvItem.mask = TVIF_HANDLE | TVIF_STATE;
	tvItem.hItem = hItem;
	tvItem.stateMask = TVIS_STATEIMAGEMASK;

	GetTreeCtrl().GetItem(&tvItem);

	int i = tvItem.state>>12;

	if (i==2)
	{
		return FALSE;
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CObjectView::InsertServer(const CServer* pServer)
{
	TV_INSERTSTRUCT tvis;
	HTREEITEM hServer = NULL;
	CString s = pServer->GetFullName();

	tvis.hParent     = TVI_ROOT;
   tvis.item.lParam = MAKELONG(pServer->GetID(), pServer->GetHostID().GetSubID());
	
	if (pServer->IsLocal())
	{
		tvis.hInsertAfter = TVI_FIRST;
		m_wLocalServerID = pServer->GetID();
	}
	else
	{
		tvis.hInsertAfter      = TVI_FIRST;
		HTREEITEM hItem, hLast = TVI_FIRST;
		hItem = GetTreeCtrl().GetNextItem(tvis.hParent, TVGN_CHILD);
		while (hItem)
		{
			WORD wItemData = HIWORD(GetTreeCtrl().GetItemData(hItem));
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
			hItem = GetTreeCtrl().GetNextItem(hItem, TVGN_NEXT);
		}
	}

   tvis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	
	tvis.item.cchTextMax = s.GetLength();
   tvis.item.pszText    = (LPTSTR)LPCTSTR(s);
	    
	tvis.item.iImage = _IMAGE_HOSTBASE + ((pServer->GetID()-1) % _IMAGE_NRHOST);
	tvis.item.iSelectedImage = tvis.item.iImage;

	hServer = GetTreeCtrl().InsertItem(&tvis);

	if (!pServer->IsDTS())
	{
		// Melder
		// check for SpecialReceiver
		s.LoadString(IDS_INPUT);
		tvis.hParent = hServer;
		tvis.hInsertAfter = TVI_LAST;
	//		tvis.item.mask = TVIF_TEXT | TVIF_PARAM;
	//		tvis.item.iImage = 0;
	//		tvis.item.iSelectedImage = 0;
		tvis.item.cchTextMax = s.GetLength();
		tvis.item.pszText    = (LPTSTR)LPCTSTR(s);
		GetTreeCtrl().InsertItem(&tvis);

		// Kameras
		s.LoadString(IDS_CAMERAS);
		tvis.hParent = hServer;
		tvis.item.cchTextMax = s.GetLength();
		tvis.item.pszText    = (LPTSTR)LPCTSTR(s);
		GetTreeCtrl().InsertItem(&tvis);

		// Relais
		// check for SpecialReceiver
		s.LoadString(IDS_RELAY);
		tvis.hParent = hServer;
		tvis.item.cchTextMax = s.GetLength();
		tvis.item.pszText    = (LPTSTR)LPCTSTR(s);
		GetTreeCtrl().InsertItem(&tvis);
	}
#ifdef _HTML_ELEMENTS
	UpdateHostPage(hServer, TRUE);
#endif


/*
	// TODO! RKE: Audio im lokalen Host einfügen
	if (pServer->IsLocal())
	{
		CVisionDoc *pDoc =GetDocument();
		CIPCAudioVision *pAudio = pDoc->GetLocalAudio();
		if (pAudio)
		{
			((CServer*)pServer)->SetCIPCAudio(pAudio);
			pAudio->SetServer(pServer);
			pDoc->ActualizeAudio(pAudio);
		}
	}
*/


	return hServer;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CObjectView::DeleteServer(DWORD dwID)
{
	BOOL bReturn = FALSE;
	HTREEITEM hItem;
	if (dwID == m_wLocalServerID)
	{
		m_wLocalServerID = SECID_NO_SUBID;
	}
	if (m_pInputDeactivationDialog
		&& m_pInputDeactivationDialog->GetServerID() == dwID)
	{
		m_pInputDeactivationDialog->DestroyWindow();
		// ATTENTION set's m_pInputDeactivationDialog = NULL !!!
	}
	
	hItem = GetTreeCtrl().GetRootItem();
	while (hItem)
	{
		if (dwID==LOWORD(GetTreeCtrl().GetItemData(hItem)))
		{
			GetTreeCtrl().DeleteItem(hItem);
			bReturn = TRUE;
			break;
		}
		hItem = GetTreeCtrl().GetNextSiblingItem(hItem);
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
int CObjectView::GetImageIndex(const CIPCInputRecord& rec)
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
int CObjectView::GetImageIndex(const CIPCOutputRecord& rec)
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
/////////////////////////////////////////////////////////////////////////////
int CObjectView::GetImageIndex(const CIPCMediaRecord& rec)
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
HTREEITEM CObjectView::InsertInput(HTREEITEM hServer, const CIPCInputRecord& rec)
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

	tvis.hParent = GetInputTreeItem(hServer);
	if (tvis.hParent==NULL)
	{
		return NULL;
	}
	tvis.hInsertAfter = TVI_SORT;

   tvis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

   tvis.item.lParam = rec.GetID().GetID();
	tvis.item.cchTextMax = s.GetLength();
   tvis.item.pszText = s.GetBuffer(0);

	int i = GetImageIndex(rec);

	if (i!=_IMAGE_OFF)
	{
		tvis.item.iImage = i;
		tvis.item.iSelectedImage = i;
		hInput = GetTreeCtrl().InsertItem(&tvis);
	}
	s.ReleaseBuffer();

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

#ifdef _HTML_ELEMENTS
//	UpdateCameraPage(hInput);
#endif

	return hInput;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CObjectView::ActualizeInput(HTREEITEM hInput, const CIPCInputRecord& rec)
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
#ifdef _HTML_ELEMENTS
	UpdateCameraPage(hInput);
#endif
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CObjectView::InsertOutput(HTREEITEM hServer, const CIPCOutputRecord& rec)
{
	HTREEITEM hOutput = NULL;
	TV_INSERTSTRUCT tvis;
	CString s = rec.GetName();

	tvis.hParent = NULL;
	if (rec.IsCamera())
	{
		tvis.hParent = GetCameraTreeItem(hServer);
	}
	else
	{
		tvis.hParent = GetRelayTreeItem(hServer);
#ifdef _HTML_ELEMENTS
//		UpdateCameraPage(hOutput);
#endif
	}

	if (tvis.hParent==NULL)
	{
		return NULL;
	}

	tvis.hInsertAfter = TVI_LAST;

   tvis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

   tvis.item.lParam = rec.GetID().GetID();
	tvis.item.cchTextMax = s.GetLength();
   tvis.item.pszText = (LPTSTR)LPCTSTR(s);

	int i = GetImageIndex(rec);

	if (  (i!=_IMAGE_OFF) 
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
	s.ReleaseBuffer();

	return hOutput;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CObjectView::ActualizeOutput(HTREEITEM hOutput, const CIPCOutputRecord& rec)
{
	int i = GetImageIndex(rec);
	GetTreeCtrl().SetItemImage(hOutput,i,i);
	GetTreeCtrl().SetItemText(hOutput,rec.GetName());
#ifdef _HTML_ELEMENTS
	UpdateCameraPage(hOutput);
#endif
	
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CObjectView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	WORD cause	  = LOWORD(lHint);
	WORD wServerID = HIWORD(lHint);
	CSecID id;
	HTREEITEM hChild;

	switch (cause)
	{
	case VDH_INPUT:
		InsertInputs((CIPCInputVision*)pHint);
		break;
	case VDH_OUTPUT:
		InsertOutputs((CIPCOutputVision*)pHint);
		break;
	case VDH_AUDIO:
		InsertAudios((CIPCAudioVision*)pHint);
		break;
	case VDH_REMOVE_SERVER:
		DeleteServer((DWORD)pHint);
		break;
	case VDH_REMOVE_CAMWINDOW:
		id = (DWORD)pHint;
		if (id.IsOutputID())
		{
			hChild = GetTreeItem(wServerID,id.GetID());
			if (hChild)
			{
				SetCheck(hChild,FALSE);
			}
		}
		break;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CObjectView::InsertInputs(CIPCInputVision* pInput)
{
	HTREEITEM hServer;
	CServer*  pServer = pInput->GetServer();
	WORD wServerID = pServer->GetID();

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
	
		hChild = GetTreeCtrl().GetChildItem(GetInputTreeItem(hServer));
		// bestehende Tree Items pruefen
		while (hChild)
		{
			idInput = (DWORD)GetTreeCtrl().GetItemData(hChild);
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
			hChild = GetTreeCtrl().GetNextSiblingItem(hChild);
			if (hDelete)
			{
				GetTreeCtrl().DeleteItem(hDelete);
			}
		}

		// Inputs einfügen
		for (int i=0;i<pInput->GetNumberOfInputs();i++)
		{
			const CIPCInputRecord rec = pInput->GetInputRecordFromIndex(i);

			// Input already in Tree ?
			hChild = GetTreeCtrl().GetChildItem(GetInputTreeItem(hServer));
			while (hChild)
			{
				if (rec.GetID().GetID()==GetTreeCtrl().GetItemData(hChild))
				{
					// found
					break;
				}
				hChild = GetTreeCtrl().GetNextSiblingItem(hChild);
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
			GetTreeCtrl().Expand(GetInputTreeItem(hServer), TVE_EXPAND);

			CString sMessage = pServer->GetNotificationMessage();

			if (!sMessage.IsEmpty())
			{
				CDocument* pDoc = GetDocument();
				pDoc->UpdateAllViews(this,MAKELONG(VDH_NOTIFICATION_MESSAGE,
									 LOWORD(GetTreeCtrl().GetItemData(hServer))),
									 (CObject*)&sMessage);
			}
			else
			{
				CString sURL = pServer->GetNotificationURL();
				if (!sURL.IsEmpty())
				{
					CDocument* pDoc = GetDocument();
					pDoc->UpdateAllViews(this,MAKELONG(VDH_INPUT_URL,
										 LOWORD(GetTreeCtrl().GetItemData(hServer))),
										 (CObject*)&sURL);
				}
			}
		}

		GetTreeCtrl().Expand(hServer, TVE_EXPAND);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CObjectView::InsertOutputs(CIPCOutputVision* pOutput)
{
	HTREEITEM hServer;
	CServer*  pServer = pOutput->GetServer();
	WORD wServerID = pServer->GetID();
	BOOL bFirstCam = theApp.m_bFirstCam;	// Wert zwischenspeichern
	CTreeCtrl &theTree = GetTreeCtrl();

	if (m_idFirstCam != SECID_NO_ID)			// shall a destinct camera be the first camera
	{
		bFirstCam = TRUE;							// switch of the 'all cameras to begin mode'
	}
	
	// insert server if not in tree
	hServer = GetTreeItem(wServerID);
	if (hServer==NULL)
	{
		hServer = InsertServer(pServer);
	}

	// insert camera/relais if not in tree
	if (hServer)
	{
		HTREEITEM hChild;
		HTREEITEM hDelete = NULL;
		CSecID idOutput;
		CIPCOutputRecord* pRecord = NULL;

		// bestehende Tree Items pruefen Camera
		hChild = theTree.GetChildItem(GetCameraTreeItem(hServer));
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
				GetDocument()->UpdateAllViews(this,MAKELONG(VDH_DELETE_CAMWINDOW,wServerID),
									(CObject*)id.GetID());
			}
		}

		// bestehende Tree Items pruefen Relay
		hChild = theTree.GetChildItem(GetRelayTreeItem(hServer));
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
		CSecID idFirstCam = m_idFirstCam;
		m_idFirstCam = SECID_NO_ID;
		HTREEITEM hFirstCam = NULL;
		c = pOutput->GetNumberOfOutputs();

		for (i=0;i<c;i++)
		{
			const CIPCOutputRecord rec = pOutput->GetOutputRecordFromIndex(i);
			TRACE(_T("output %s %s %d, %08lx\n"),pServer->GetName(),rec.GetName(),i,rec.GetID().GetID());
			if (rec.IsCamera())
			{
				hChild = theTree.GetChildItem(GetCameraTreeItem(hServer));
			}
			else
			{
				hChild = theTree.GetChildItem(GetRelayTreeItem(hServer));
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
				ActualizeOutput(hChild,rec);
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
				// add it
				// check for SpecialReceiver
				if(   pServer->IsDTS()
					&& rec.IsRelay()
					)
				{
					// do not add for DTS
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
		CVisionDoc* pDoc = GetDocument();
		CVisionView* pView = pDoc->GetVisionView();
		// Set view mode 1plus at begin?
		if (   theApp.m_b1PlusAlarm
			&& (   pServer->IsAlarm()
			    || pServer->IsControl()))
		{
			if (!pView->IsView1plus())
			{
				pView->SendMessage(WM_COMMAND, ID_VIEW_1_PLUS);
			}
		}

		// Set fullscreen mode at begin?
		if (   theApp.m_bFullscreenAlarm
			&& pServer->IsAlarm())
		{
			CMainFrame* pMF = (CMainFrame*)theApp.GetMainWnd();
			if (   WK_IS_WINDOW(pMF) 
				&& !pMF->IsFullScreen())
			{
				pMF->ChangeFullScreenModus();
			}
		}

		if (  bFirstCam
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
		}
		else
		{
			// switch to all cams
			if (!pServer->IsDTS())
			{
				if (!pServer->IsMultiCamera())
				{
					GetDocument()->SetSequence(TRUE);
				}
			}
			HTREEITEM hCam = theTree.GetChildItem(hServer);

			if (   (   pServer->IsAlarm()
				    || pServer->IsControl()
					)
				&& idFirstCam.IsOutputID())
			{
				HTREEITEM hItem = GetTreeItem(wServerID,pServer->GetAlarmID().GetID());
				if (hItem)
				{
					SwitchCamera(hItem,TRUE);
					theTree.SelectItem(hItem);
				}
			}

			hCam = theTree.GetChildItem(GetCameraTreeItem(hServer));
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
		}

		// show camera map if any and if server is local
#ifdef _HTML_ELEMENTS
		if (pServer->IsLocal())
		{
			GetDocument()->UpdateAllViews(this,MAKELONG(VDH_CAMERA_MAP,wServerID),NULL);
		}

		if (m_LocalHTMLmappings.GetCount())
		{
			POSITION pos = m_LocalHTMLmappings.GetHeadPosition();
			while (pos)
			{
				DWORD *pdwMapping = (DWORD*)m_LocalHTMLmappings.GetNext(pos);
				if (SERVERID_OF(pdwMapping[MAPPING_SERVER]) == wServerID)
				{
					CSecID ServerID(wServerID, m_wLocalServerID);
					CString strID;
					DWORD dwUpdate = SEARCH_BY_ID|SEARCH_BY_SERVER|SEARCH_IN_ALL|HTML_ELEMENT_UPDATE_STATE;
					strID.Format(_T("%08x"), pdwMapping[MAPPING_EL_ID]);
					UpdateMapElement(ServerID, strID, dwUpdate);
				}
			}
		}
#endif
		
		hChild = GetCameraTreeItem(hServer);
		if (hChild)
		{
			theTree.Expand(hChild, TVE_EXPAND);
		}

		hChild = GetRelayTreeItem(hServer);
		if (hChild)
		{
			theTree.Invalidate();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CObjectView::InsertAudios(CIPCAudioVision* pAudio)
{
	HTREEITEM hServer;
	CServer*  pServer = pAudio->GetServer();
	if (pServer == NULL)
	{
		pServer = GetDocument()->GetLocalServer();
	}
	WORD wServerID = pServer->GetID();
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
		HTREEITEM        hAudio  = GetAudioTreeItem(hServer);
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
				hChild = GetTreeItem(wServerID, rec.GetID().GetID(), SEARCH_IN_MEDIA);
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

		hAudio = GetAudioTreeItem(hServer, false);
		if (hAudio)
		{
			theTree.Expand(hAudio, TVE_EXPAND);
		}
		if (m_idSelectMedia != SECID_NO_ID)
		{
			Select(wServerID, m_idSelectMedia, TRUE);
			m_idSelectMedia = SECID_NO_ID;
		}
	}
}
//////////////////////////////////////////////////////////////////////////
HTREEITEM CObjectView::InsertAudio(HTREEITEM hServer, CIPCMediaRecord&rec)
{
	HTREEITEM hItem, hAudio = GetAudioTreeItem(hServer, true);
	CTreeCtrl& theTree = GetTreeCtrl();
	if (hAudio)
	{
		WORD wHost, wServer = GetServerIDs(hServer, &wHost);
		CServer *pServer = GetServer(wServer);
		CIPCAudioVision*pAudio = NULL;
		if (pServer)
		{
			pAudio = pServer->GetAudio();
			if (pAudio)
			{																		// Initialisierung der ReplyID
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
		if (hItem)// && rec.IsOutput())
		{
			SetCheck(hItem, FALSE);
		}
		return hItem;
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
BOOL CObjectView::ActualizeAudio(HTREEITEM hAudio, const CIPCMediaRecord& rec)
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
		CVisionDoc* pDoc = GetDocument();
		int nServer=0;
		CServer* pServer = pDoc->GetServerFromIndex(nServer++);
		while (pServer)
		{
			if (!pServer->IsLocal())
			{
				CIPCAudioVision*pAudio = pServer->GetAudio();
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
	if (hAudio) SetCheck(hAudio, bCheck);
#ifdef _HTML_ELEMENTS
	if (hAudio) UpdateCameraPage(hAudio);
#endif
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CObjectView::ActivateCamera(HTREEITEM hItem)
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

	CVisionDoc* pDoc = GetDocument();
	CSecID id = (DWORD)GetTreeCtrl().GetItemData(hItem);
	hParent = GetTreeCtrl().GetParentItem(hItem);
	CServer* pServer = pDoc->GetServer(LOWORD(GetTreeCtrl().GetItemData(hParent)));

	if (pServer)
	{
		CIPCOutputVision* pOutput = pServer->GetOutput();
		CIPCOutputRecord* pRec = pOutput->GetOutputRecordPtrFromSecID(id);

		if (pRec && pRec->IsCamera())
		{
			pDoc->UpdateAllViews(this,MAKELONG(VDH_SINGLE_CAM,pServer->GetID()),
								(CObject*)pRec);
			GetTreeCtrl().SelectItem(hItem);
			SetCheck(hItem,TRUE);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CObjectView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
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
void CObjectView::ContextMenu(const CPoint& pt, HTREEITEM hItem)
{
	CTreeCtrl&  theTree = GetTreeCtrl();
	CSecID id = (DWORD)theTree.GetItemData(hItem);
	CMenu menu;
	CMenu* pPopup = NULL;

	menu.LoadMenu(IDR_CONTEXT);

	if (id.IsInputID())
	{
		pPopup = menu.GetSubMenu(SUB_MENU_DETECTOR);
		// TODO add MNO menu
		CVisionDoc* pDoc = GetDocument();
		HTREEITEM hParent = theTree.GetParentItem(hItem);
		CServer* pServer = pDoc->GetServer(LOWORD(theTree.GetItemData(hParent)));

		if (pServer && pServer->IsConnected())
		{
			CIPCInputVision* pInput = pServer->GetInput();
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
		CVisionDoc* pDoc = GetDocument();
		HTREEITEM hParent = theTree.GetParentItem(hItem);
		CServer* pServer = pDoc->GetServer(LOWORD(theTree.GetItemData(hParent)));

		if (pServer && pServer->IsConnected())
		{
			CIPCOutputVision* pOutput = pServer->GetOutput();
			const CIPCOutputRecord rec = pOutput->GetOutputRecordFromSecID(id);

			if (rec.IsCamera())
			{
				// GF todo m_SmallWindows.Lock;
				CDisplayWindow* pDW = pDoc->GetVisionView()->GetDisplayWindow(pServer->GetID(),id);
				if (pDW)
				{
					// gf Context menu has to work on the active camera
					pDoc->GetVisionView()->SetCmdActiveSmallWindow(pDW);
					pDW->PopupMenu(pt);
					return;
				}
				pPopup = menu.GetSubMenu(SUB_MENU_PICTURES);
			}
			else
			{
				pPopup = menu.GetSubMenu(SUB_MENU_RELAIS);
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
				pPopup = menu.GetSubMenu(SUB_MENU_AUDIO_PLAY);
				pPopup->DeleteMenu(ID_AU_PROPERTIES, MF_BYCOMMAND);
				int nSeparator = pPopup->GetMenuItemCount()-1;
				if (pPopup->GetMenuItemID(nSeparator) == 0)
				{
					pPopup->DeleteMenu(nSeparator, MF_BYPOSITION);
				}
			}break;
			case _IMAGE_AUDIO_REC: case _IMAGE_AUDIO_REC_DEF:
				pPopup = menu.GetSubMenu(SUB_MENU_AUDIO_REC);
				break;
		}
	}
	else
	{
		HTREEITEM hParent = theTree.GetParentItem(hItem);
		if (hParent == NULL)
		{
			// really on top a server
			pPopup = menu.GetSubMenu(SUB_MENU_HOST);
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
		pPopup->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y, AfxGetMainWnd());		
	}
}
/////////////////////////////////////////////////////////////////////////////
void CObjectView::SwitchCamera(HTREEITEM hItemCamera, BOOL bOn)
{
	CVisionDoc* pDoc    = GetDocument();
	CTreeCtrl&  theTree = GetTreeCtrl();
	CSecID      id      = (DWORD)theTree.GetItemData(hItemCamera);
	HTREEITEM   hParent = theTree.GetParentItem(hItemCamera);
	CServer*    pServer = pDoc->GetServer(LOWORD(theTree.GetItemData(hParent)));

	if (pServer)
	{
		CIPCOutputVision* pOutput = pServer->GetOutput();
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
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CObjectView::SwitchRelay(HTREEITEM hItemRelais, BOOL bOn)
{
	CTreeCtrl&  theTree = GetTreeCtrl();
	CVisionDoc* pDoc = GetDocument();
	CSecID id = (DWORD)theTree.GetItemData(hItemRelais);
	HTREEITEM hParent = theTree.GetParentItem(hItemRelais);

	if (hParent)
	{
		CServer* pServer = pDoc->GetServer(LOWORD(theTree.GetItemData(hParent)));

		if (pServer)
		{
			CIPCOutputVision* pOutput = pServer->GetOutput();
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
#ifdef _HTML_ELEMENTS
//				UpdateCameraPage(hItemRelais);
#endif
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CObjectView::SwitchAudio(HTREEITEM hItemAudio, BOOL bOn)
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
/////////////////////////////////////////////////////////////////////////////
void CObjectView::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
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
		if (id.IsOutputID())
		{
			BOOL bOn = GetCheck(hItem);
			SwitchCamera(hItem, !bOn);
		}
		else if (id.IsMediaID())
		{
			BOOL bOn = GetCheck(hItem);
			SwitchAudio(hItem, !bOn);
		}
	}
	else if (hItem && (nFlags & TVHT_ONITEMICON))
	{
		SwitchRelay(hItem,-1);
	}
#ifdef _HTML_ELEMENTS
	UpdateCameraPage(hItem, VDH_FLAGS_UPDATE|VDH_FLAGS_SELECTED);
#endif
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
#ifdef _HTML_ELEMENTS
void CObjectView::UpdateCameraPage(HTREEITEM hItem,
											  WORD wFlags      //=VDH_FLAGS_UPDATE
											  )
{
	if (hItem)
	{
		CTreeCtrl &theTree = GetTreeCtrl();
		CVisionDoc *pDoc = GetDocument();
		HTREEITEM hParent = theTree.GetParentItem(hItem);
		if (hParent == NULL) return;
		CServer* pServer = pDoc->GetServer(LOWORD(theTree.GetItemData(hParent)));
		if (theTree.GetSelectedItem() == hItem) wFlags |= VDH_FLAGS_SELECTED;

		if (pServer)
		{
			WORD wServerID = pServer->GetID();
			CSecID id(theTree.GetItemData(hItem));
			if (IsOnLocalHTMLpage(pServer->GetHostID().GetSubID(), id))
			{
				wServerID = m_wLocalServerID;
			}

			if (id.IsInputID())
			{
				CIPCInputVision * pInput = pServer->GetInput();
				CIPCInputRecord * pRecI  = pInput->GetInputRecordPtrFromSecID(id);
				if (pRecI)
				{
					CHtmlPageHint Hint(pRecI, pServer->GetHostID().GetSubID(), pServer->GetID(), wFlags);
					pDoc->UpdateAllViews(this, MAKELONG(VDH_CAM_MAP, wServerID), &Hint);
				}
			}
			else if (id.IsOutputID())
			{
				CIPCOutputVision* pOutput = pServer->GetOutput();
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
void CObjectView::UpdateHostPage(HTREEITEM hItem, BOOL bConnect)
{
	if (hItem)
	{
		CVisionDoc *pDoc = GetDocument();
		CHtmlPageHint Hint(NULL, HIWORD(GetTreeCtrl().GetItemData(hItem)), LOWORD(GetTreeCtrl().GetItemData(hItem)), (WORD)bConnect);

		pDoc->UpdateAllViews(this, MAKELONG(VDH_HOST_MAP, 0), &Hint);
	}
}
#endif
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
	CTreeCtrl &theTree = GetTreeCtrl();
	WORD wServer, wHost;
	HTREEITEM hItem = theTree.GetSelectedItem();

	wServer = GetSelectedServer(&wHost, hItem);
		
	if (wServer != SECID_NO_SUBID)
	{
		CVisionDoc* pDoc = GetDocument();
#ifdef _HTML_ELEMENTS
		UpdateHostPage(hItem, FALSE);
		
		if (m_LocalHTMLmappings.GetCount())
		{
			POSITION pos = m_LocalHTMLmappings.GetHeadPosition();
			while (pos)
			{
				DWORD *pdwMapping = (DWORD*)m_LocalHTMLmappings.GetNext(pos);
				if (HOSTID_OF(pdwMapping[MAPPING_SERVER]) == wHost)
				{
					CSecID ServerID(wServer, m_wLocalServerID);
					CString strID;
					DWORD dwUpdate = SEARCH_BY_ID|SEARCH_BY_SERVER|SEARCH_IN_ALL|HTML_ELEMENT_UPDATE_STATE|HTML_ELEMENT_DISCONNECT;
					strID.Format(_T("%08x"), pdwMapping[MAPPING_EL_ID]);
					UpdateMapElement(ServerID, strID, dwUpdate);
				}
			}
		}
#endif

		pDoc->Disconnect(wServer);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CObjectView::DisconnectID(UINT nID) 
{
	GetDocument()->Disconnect((WORD)nID);
}
/////////////////////////////////////////////////////////////////////////////
void CObjectView::PopupDisconnectMenu(CPoint pt)
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
			sText.Format(_T("&%d %s"),i++,sItemText);
			menu.AppendMenu(MF_STRING,
							dwID+ID_DISCONNECT_LOW,
							sText);
			hItem = GetTreeCtrl().GetNextSiblingItem(hItem);
		}
        menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, AfxGetMainWnd());
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CObjectView::IsServerItem(HTREEITEM hItem)
{
	return NULL == GetTreeCtrl().GetParentItem(hItem);
}
////////////////////////////////////////////////////////////////////////////
CServer* CObjectView::GetServer(HTREEITEM hServer)
{
	if (IsServerItem(hServer))
	{
		return GetServer(LOWORD(GetTreeCtrl().GetItemData(hServer)));
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CServer* CObjectView::GetServer(WORD wServerID)
{
	CVisionDoc* pDoc = GetDocument();
	if (pDoc)
	{
		CServer* pServer = pDoc->GetServer(wServerID);
		if (pServer && pServer->IsConnected())
		{
			return pServer;
		}
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
HTREEITEM CObjectView::GetInputTreeItem(HTREEITEM hServer)
{
	CServer* pServer = GetServer(hServer);
	if (pServer && pServer->IsDTS())
	{
		return NULL;
	}
	else
	{
		return GetTreeCtrl().GetChildItem(hServer);
	}
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CObjectView::GetCameraTreeItem(HTREEITEM hServer)
{
	CServer* pServer = GetServer(hServer);
	if (pServer && pServer->IsDTS())
	{
		return hServer;
	}
	else
	{
		HTREEITEM hInput = GetTreeCtrl().GetChildItem(hServer);
		return GetTreeCtrl().GetNextSiblingItem(hInput);
	}
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CObjectView::GetRelayTreeItem(HTREEITEM hServer)
{
	CServer* pServer = GetServer(hServer);
	if (pServer && pServer->IsDTS())
	{
		return NULL;
	}
	else
	{
		HTREEITEM hInput = GetTreeCtrl().GetChildItem(hServer);
		HTREEITEM hOutput = GetTreeCtrl().GetNextSiblingItem(hInput);
		return GetTreeCtrl().GetNextSiblingItem(hOutput);
	}
}
//////////////////////////////////////////////////////////////////////////
HTREEITEM CObjectView::GetAudioTreeItem(HTREEITEM hServer, bool bCreate)
{
	CServer* pServer = GetServer(hServer);
	if (pServer && pServer->IsDTS())
	{
		return hServer;
	}
	else if (pServer)
	{
		HTREEITEM hItem = GetTreeCtrl().GetChildItem(hServer);
		while (hItem)
		{
			CSecID id(GetTreeCtrl().GetItemData(hItem));
			if (id.IsMediaID() && (id.GetSubID() == SUBID_MEDIA_AUDIO))
			{
				return hItem;
			}
			hItem = GetTreeCtrl().GetNextSiblingItem(hItem);
		}
		if (bCreate)
		{
			CString s;
			s.LoadString(IDS_AUDIO);
			int nImage = _IMAGE_HOSTBASE + ((pServer->GetID()-1) % _IMAGE_NRHOST);
			hItem = GetTreeCtrl().InsertItem(s, nImage, nImage, hServer);
			if (hItem)
			{
				CSecID id(SECID_GROUP_MEDIA, SUBID_MEDIA_AUDIO);
				GetTreeCtrl().SetItemData(hItem, id.GetID());
			}
			return hItem;
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CObjectView::GetTreeItem(WORD wServer, const CString& s, DWORD dwSearch /*=SEARCH_IN_ALL*/)
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

			if (dwSearch & SEARCH_IN_INPUT)	// searching input
			{
				HTREEITEM hInput;
				hInput = GetInputTreeItem(hServer);
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
		
			if (dwSearch & SEARCH_IN_CAMERAS)	// searching camera
			{
				HTREEITEM hCamera;
				hCamera = GetCameraTreeItem(hServer);
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

			if (dwSearch & SEARCH_IN_RELAYS)		// searching relay
			{
				HTREEITEM hRelay;
				hRelay = GetRelayTreeItem(hServer);
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

			if (dwSearch & SEARCH_IN_MEDIA)		// searching audio
			{
				HTREEITEM hAudio;
				hAudio = GetAudioTreeItem(hServer);
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
		}
		// next server, if any
		hServer = theTree.GetNextSiblingItem(hServer);
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
HTREEITEM CObjectView::GetTreeItem(WORD wServer, DWORD dwID /*=0*/, DWORD dwSearch /*=SEARCH_IN_ALL*/)
{
	HTREEITEM hServer;
	HTREEITEM hChild;
	HTREEITEM hCamera;
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

			if (dwSearch & SEARCH_IN_INPUT)	// searching input
			{
				HTREEITEM hInput;
				hInput = GetInputTreeItem(hServer);
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
		
			if (dwSearch & SEARCH_IN_CAMERAS)	// searching camera
			{
				hCamera = GetCameraTreeItem(hServer);
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

			if (dwSearch & SEARCH_IN_RELAYS)		// searching relay
			{
				hRelay = GetRelayTreeItem(hServer);
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

			if (dwSearch & SEARCH_IN_MEDIA)		// searching audio
			{
				HTREEITEM hAudio;
				hAudio = GetAudioTreeItem(hServer);
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
		}
		// next server, if any
		hServer = theTree.GetNextSiblingItem(hServer);
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CObjectView::Actualize(WORD wServerID, CSecID id)
{
	HTREEITEM hChild = GetTreeItem(wServerID, id.GetID());

	if (hChild)
	{
		// found input or output
		CServer* pServer = GetDocument()->GetServer(wServerID);
		if (id.IsInputID())
		{
			if (!pServer->IsDTS())
			{
				CIPCInputVision* pInput = pServer->GetInput();
				if (pInput)
				{
					pInput->Lock();
					CIPCInputRecord* pRecord = pInput->GetInputRecordPtrFromSecID(id);
					if (pRecord)
					{
						ActualizeInput(hChild,*pRecord);
					}
					pInput->Unlock();
				}
			}
		}
		else if (id.IsOutputID())
		{
			CIPCOutputVision* pOutput = pServer->GetOutput();
			if (pOutput)
			{
				CIPCOutputRecord* pRecord = pOutput->GetOutputRecordPtrFromSecID(id);
				if (pRecord)
				{
					CDisplayWindow* pDW = GetDocument()->GetVisionView()->GetDisplayWindow(wServerID, id);
					if (pDW)
					{
						pDW->SetOutputRecord(*pRecord);
					}
					ActualizeOutput(hChild,*pRecord);
				}
			}
		}
		else if (id.IsMediaID())
		{
			CIPCAudioVision* pAudio = pServer->GetAudio();
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
	else if (id.IsMediaID())
	{
		CIPCAudioVision* pAudio = GetDocument()->GetLocalAudio();
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
/////////////////////////////////////////////////////////////////////////////
void CObjectView::Delete(WORD wServerID, CSecID id)
{
	HTREEITEM hChild = GetTreeItem(wServerID, id.GetID());

	if (hChild)
	{
		if (id.IsInputID())
		{
			GetTreeCtrl().DeleteItem(hChild);
		}
		else if (id.IsOutputID())
		{
			GetTreeCtrl().DeleteItem(hChild);
			// Camera OFF
			GetDocument()->UpdateAllViews(this,MAKELONG(VDH_DELETE_CAMWINDOW,wServerID),
								(CObject*)id.GetID());
		}
		else if (id.IsMediaID())
		{
			GetTreeCtrl().DeleteItem(hChild);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CObjectView::Add(WORD wServerID, CSecID id)
{
	HTREEITEM hChild = GetTreeItem(wServerID,id.GetID());

	if (hChild == NULL)
	{
		// found input or output
		CServer* pServer = GetDocument()->GetServer(wServerID);
		if (id.IsInputID())
		{
			if (!pServer->IsDTS())
			{
				CIPCInputVision* pInput = pServer->GetInput();
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
		else if (id.IsOutputID())
		{
			CIPCOutputVision* pOutput = pServer->GetOutput();
			if (pOutput)
			{
				CIPCOutputRecord* pRecord = pOutput->GetOutputRecordPtrFromSecID(id);
				if (pRecord)
				{
					InsertOutput(hChild,*pRecord);
				}
			}
		}
		else if (id.IsMediaID())
		{
			CIPCAudioVision* pAudio = pServer->GetAudio();
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
void CObjectView::Select(WORD wServerID, const CString& s)
{
	HTREEITEM hItem = GetTreeItem(wServerID,s);
	if (hItem)
	{
		DWORD dw = GetTreeCtrl().GetItemData(hItem);
		Select(wServerID,hItem,CSecID(dw));
	}
}
/////////////////////////////////////////////////////////////////////////////
#ifdef _HTML_ELEMENTS
/*********************************************************************************************
 Class      : CObjectView
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
DWORD CObjectView::UpdateMapElement(CSecID ServerID, CString &s, DWORD& dwUpdate)
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
	else								// Cameras, Detectors, Relais
	{
		HTREEITEM hItem = NULL;
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
			hItem = GetTreeItem(wServer, dwID, dwUpdate);
		}
		else
		{
			hItem = GetTreeItem(wServer, s, dwUpdate);
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
				if (sFind != sItem)
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
#endif 
/////////////////////////////////////////////////////////////////////////////
void CObjectView::Select(WORD wServerID, CSecID id, BOOL bOn /*= TRUE*/)
{
	HTREEITEM hItem = GetTreeItem(wServerID,id.GetID());
	if (hItem)
	{
		Select(wServerID,hItem,id,bOn);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CObjectView::Select(WORD wServerID, HTREEITEM hItem, CSecID id, BOOL bpOn /*= TRUE*/)
{
	if (id.IsOutputID())
	{
		if (bpOn)
		{
			BOOL blOn = GetCheck(hItem);
			if (blOn)
			{
				ReactivateCamera(hItem);
			}
			else
			{
				CVisionDoc* pDoc = GetDocument();
				HTREEITEM hParent = GetTreeCtrl().GetParentItem(hItem);
				CServer* pServer = pDoc->GetServer(LOWORD(GetTreeCtrl().GetItemData(hParent)));

				if (pServer)
				{
					CIPCOutputVision* pOutput = pServer->GetOutput();
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
	else if (id.IsMediaID())
	{
		SwitchAudio(hItem, bpOn);
	}
	else
	{
		GetTreeCtrl().Select(hItem, TVGN_CARET);
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CObjectView::OnUser(WPARAM wParam, LPARAM lParam) 
{
	CDocument* pDoc = GetDocument();
	WORD cause	  = LOWORD(wParam);
	WORD wServerID = HIWORD(wParam);
	CServer* pServer = GetDocument()->GetServer(wServerID);
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
				CDisplayWindow* pDW = GetDocument()->GetVisionView()->GetDisplayWindow(wServerID,id);
				if (WK_IS_WINDOW(pDW))
				{
					GetDocument()->GetVisionView()->SetBigSmallWindow(pDW);
				}
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
				&& GetDocument()->GetVisionView()->IsView1plus())
			{
				TRACE(_T("1 plus aus wegen alarm aus\n"));
				GetDocument()->GetVisionView()->SendMessage(WM_COMMAND,ID_VIEW_1_PLUS);
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
	//ML 24.9.99 Begin Insertation{
	case WPARAM_CONTROL_SELECT_CAM:
		if (id.IsOutputID())
		{
			CServer* pServer = GetDocument()->GetServer(wServerID);
			if (   pServer
				&& pServer->IsControl())
			{
				if (pServer->IsControlExclusive())
				{
					ActivateCamera(GetTreeItem(wServerID,id.GetID()));
				}
				else
				{
					Select(wServerID,id);
					if (theApp.m_b1PlusAlarm)
					{
						// richtige Camera soll 1 Plus werden
						CDisplayWindow* pDW = GetDocument()->GetVisionView()->GetDisplayWindow(wServerID,id);
						if (WK_IS_WINDOW(pDW))
						{
							GetDocument()->GetVisionView()->SetBigSmallWindow(pDW);
						}
					}
				}
			}
		}
		else if(id.IsInputID())
		{
			ShowInformation(GetTreeItem(wServerID,id.GetID()));
		}
		break;
	case WPARAM_CONTROL_UNSELECT_CAM:
		if (id.IsOutputID())
		{
			if (theApp.m_bCloseAlarmWindow)
			{
				Select(wServerID,id,FALSE);
			}
			if (   theApp.m_b1PlusAlarm
				&& GetDocument()->GetVisionView()->IsView1plus())
			{
				if (id == GetDocument()->GetVisionView()->Get1plusWindowID())
				{
					GetDocument()->GetVisionView()->SendMessage(WM_COMMAND,ID_VIEW_1_PLUS);
				}
			}
			CVisionView* pVV = GetDocument()->GetVisionView();
			if (WK_IS_WINDOW(pVV))
			{
				int iNrDisplays = pVV->GetNrOfDisplayWindows(wServerID);
				if (iNrDisplays == 0)
				{
					GetDocument()->Disconnect(wServerID);
				}
			}
		}
		break;
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
	// End Insertation}

	case WPARAM_INFORMATION:
		ShowInformation(GetTreeItem(wServerID,id.GetID()));
		break;
	case WPARAM_MESSAGE:
		{
			char *pBuffer = (char*)lParam;
			CString sMessage(pBuffer);
			delete pBuffer;
			if (!sMessage.IsEmpty())
			{
				pDoc->UpdateAllViews(this,MAKELONG(VDH_NOTIFICATION_MESSAGE,
									 wServerID),(CObject*)&sMessage);
			}
		}
		break;
	case WPARAM_URL:
		{
			char *pBuffer = (char*)lParam;
			CString sURL(pBuffer);
			delete pBuffer;
			if (!sURL.IsEmpty())
			{
				pDoc->UpdateAllViews(this,MAKELONG(VDH_INPUT_URL,
									 wServerID),(CObject*)&sURL);
			}
		}
		break;
	case WPARAM_CAMERA_MAP:
		pDoc->UpdateAllViews(this,MAKELONG(VDH_CAMERA_MAP,wServerID),NULL);
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
	case WPARAM_DATABASE_CONNECTED:
		GetDocument()->OnDatabaseConnected();
		break;
	case WPARAM_DATABASE_DISCONNECTED:
		GetDocument()->OnDatabaseDisconnected();
		break;
	case WPARAM_INPUT_CONNECTED:
		if (pServer)
		{
			pServer->OnInputConnected();
		}
		break;
	case WPARAM_INPUT_DISCONNECTED:
		if (pServer)
		{
			pServer->OnInputDisconnected();
		}
		break;
	case WPARAM_OUTPUT_CONNECTED:
		if (pServer)
		{
			pServer->OnOutputConnected();
		}
		break;
	case WPARAM_OUTPUT_DISCONNECTED:
		if (pServer)
		{
			pServer->OnOutputDisconnected();
		}
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
			&& GetDocument()->GetVisionView()->IsView1plus())
		{
			TRACE(_T("1 plus aus wegen control disconnect\n"));
			GetDocument()->GetVisionView()->SendMessage(WM_COMMAND,ID_VIEW_1_PLUS);
		}
		break;
	case WPARAM_AUDIO_CONNECTED:
		if (pServer)
		{
			pServer->OnAudioConnected();
		}
		break;

	case WPARAM_INPUT_FETCHRESULT:
		if (pServer)
		{
			pServer->OnInputFetchResult();
		}
		break;
	case WPARAM_OUTPUT_FETCHRESULT:
		if (pServer)
		{
			pServer->OnOutputFetchResult();
		}
		break;
	case WPARAM_AUDIO_FETCHRESULT:
		if (pServer)
		{
			pServer->OnAudioFetchResult();
		}
		break;
	case WPARAM_CONNECT_THREAD_END:
		if (pServer)
		{
			pServer->OnConnectThreadEnd();
		}
		break;
	case WPARAM_CONNECTION_RECORD:
		GetDocument()->OnConnectionRecord();
		break;
	case WPARAM_ERROR_MESSAGE:
		theApp.OnErrorMessage();
		break;
	case WPARAM_AUDIO_STOP_AUDIORECORD:
		{
			CIPCAudioVision *pLocalAudio = GetDocument()->GetLocalAudio();
			if (pLocalAudio)
			{
				pLocalAudio->StopRecord(pLocalAudio->GetDefaultInputID());
			}
		} break;
	case WPARAM_AUDIO_SET_INPUT_STATE:
		{
			CIPCAudioVision *pLocalAudio = GetDocument()->GetLocalAudio();
			CIPCAudioVision *pSender = (CIPCAudioVision*)lParam;
			if (pSender == pLocalAudio)
			{
				CIPCMediaRecord *pDefOut = pLocalAudio->GetMediaRecordPtrFromSecID(pLocalAudio->GetDefaultOutputID());
				if (pDefOut)
				{
					CVisionDoc* pDoc = GetDocument();
					int nServer=0;
					CServer* pServer = pDoc->GetServerFromIndex(nServer++);

					while (pServer)
					{
						CIPCAudioVision*pAudio = pServer->GetAudio();
						if (pAudio && pAudio != pLocalAudio)
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
	}

	return 0L;
}
/////////////////////////////////////////////////////////////////////////////
void CObjectView::ShowInformation(HTREEITEM hItem)
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
void CObjectView::OnRelaisOpen() 
{
	HTREEITEM hSelectedItem = GetTreeCtrl().GetSelectedItem();
	if (hSelectedItem)
	{
		SwitchRelay(hSelectedItem,TRUE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CObjectView::OnUpdateRelaisOpen(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	HTREEITEM hSelectedItem = GetTreeCtrl().GetSelectedItem();
	WORD wHost, wServer = GetSelectedServer(&wHost, hSelectedItem);
	CServer *pServer = GetServer(wServer);
	if (pServer)
	{
		CIPCOutputVision* pOutput = pServer->GetOutput();
		if (pOutput)
		{
			CIPCOutputRecord* pRecord = pOutput->GetOutputRecordPtrFromSecID(GetTreeCtrl().GetItemData(hSelectedItem));
			if (pRecord && pRecord->IsRelay())
			{
				bEnable = pRecord->RelayIsClosed() && !pRecord->RelayIsEdgeControlled();
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
void CObjectView::OnRelaisClose() 
{
	HTREEITEM hSelectedItem = GetTreeCtrl().GetSelectedItem();
	if (hSelectedItem)
	{
		SwitchRelay(hSelectedItem,FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CObjectView::OnUpdateRelaisClose(CCmdUI* pCmdUI) 
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
void CObjectView::OnVideoStart() 
{
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	
	if (hItem)
	{
		CSecID id = (DWORD)GetTreeCtrl().GetItemData(hItem);
		if (id.IsOutputID())
		{
			CVisionDoc* pDoc = GetDocument();
			HTREEITEM hParent = GetTreeCtrl().GetParentItem(hItem);
			CServer* pServer = pDoc->GetServer(LOWORD(GetTreeCtrl().GetItemData(hParent)));

			if (pServer)
			{
				CIPCOutputVision* pOutput = pServer->GetOutput();
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
void CObjectView::OnSpace() 
{
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	
	if (hItem)
	{
		CSecID id = (DWORD)GetTreeCtrl().GetItemData(hItem);
		if (id.IsOutputID())
		{
			CVisionDoc* pDoc = GetDocument();
			HTREEITEM hParent = GetTreeCtrl().GetParentItem(hItem);
			CServer* pServer = pDoc->GetServer(LOWORD(GetTreeCtrl().GetItemData(hParent)));

			if (pServer)
			{
				CIPCOutputVision* pOutput = pServer->GetOutput();
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
/////////////////////////////////////////////////////////////////////////////
void CObjectView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
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
void CObjectView::ReactivateCamera(HTREEITEM hItem)
{
	CSecID id = (DWORD)GetTreeCtrl().GetItemData(hItem);
	if (id.IsOutputID())
	{
		CVisionDoc* pDoc = GetDocument();
		HTREEITEM hParent = GetTreeCtrl().GetParentItem(hItem);
		CServer* pServer = pDoc->GetServer(LOWORD(GetTreeCtrl().GetItemData(hParent)));

		if (pServer)
		{
			CIPCOutputVision* pOutput = pServer->GetOutput();
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
void CObjectView::OnVideoAlarmSwitch() 
{
	WORD wServerID = GetSelectedServer();

	if (wServerID != SERVERID_NOID)
	{
		CVisionDoc* pDoc = GetDocument();
		CServer* pServer = pDoc->GetServer(wServerID);
		if (pServer)
		{
			pServer->SetAlarmSwitch(!pServer->GetAlarmSwitch());
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
WORD CObjectView::GetSelectedServer(WORD *pwHostId, HTREEITEM hSelected)
{
	HTREEITEM hItem = NULL;
	HTREEITEM hParent;
	if (hSelected)
	{
		hItem = hSelected;
	}
	else
	{
		hItem = GetTreeCtrl().GetSelectedItem();
	}
	while (hItem)
	{
		hParent = GetTreeCtrl().GetParentItem(hItem);
		if (hParent == NULL) break;
		hItem = hParent;
	}
	if (hItem==NULL)
	{
		hItem = GetTreeCtrl().GetRootItem();
	}
	return GetServerIDs(hItem, pwHostId);
}
//////////////////////////////////////////////////////////////////////////
WORD CObjectView::GetServerIDs(HTREEITEM hServer, WORD *pwHost)
{
	DWORD dwID = SECID_NO_ID;
	if (hServer) dwID =  GetTreeCtrl().GetItemData(hServer);
	if (pwHost) *pwHost = HIWORD(dwID);
	return  LOWORD(dwID);
}
//////////////////////////////////////////////////////////////////////////
CIPCMediaRecord* CObjectView::GetSelectedItemsMediaRecord(WORD &wHost, WORD &wServer, HTREEITEM hSelected)
{
	wServer = GetSelectedServer(&wHost, hSelected);
	CServer *pServer = GetServer(wServer);
	if (pServer)
	{
		CIPCAudioVision *pAudio = pServer->GetAudio();
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
void CObjectView::OnUpdateVideoAlarmSwitch(CCmdUI* pCmdUI) 
{
	WORD wServerID = GetSelectedServer();

	if (wServerID != SERVERID_NOID)
	{
		CVisionDoc* pDoc = GetDocument();
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
/////////////////////////////////////////////////////////////////////////////
void CObjectView::OnHostSaveSequence() 
{
	WORD wServerID = GetSelectedServer();

	if (wServerID != SERVERID_NOID)
	{
		CVisionDoc* pDoc = GetDocument();
		CServer* pServer = pDoc->GetServer(wServerID);
		if (pServer)
		{
			CString sHost = pServer->GetFullName();
			CLoadSaveSequencerDialog dlg(TRUE,sHost,this);

			if (IDOK==dlg.DoModal())
			{
				CString sSequenceName = dlg.GetSequenceName();
				CString sCameraID;
				CString sCameraIDs;
				CString sSection;
				HTREEITEM hServer = GetTreeItem(wServerID);
				HTREEITEM hCam	  = GetCameraTreeItem(hServer);
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
				sSection = _T("Vision\\Connections\\");
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
void CObjectView::OnHostLoadSequence() 
{
	WORD wServerID = GetSelectedServer();

	if (wServerID != SERVERID_NOID)
	{
		CVisionDoc* pDoc = GetDocument();
		CServer* pServer = pDoc->GetServer(wServerID);
		if (pServer)
		{
			CString sHost = pServer->GetFullName();
			CLoadSaveSequencerDialog dlg(FALSE,sHost,this);

			if (IDOK==dlg.DoModal())
			{
				CString sSequenceName = dlg.GetSequenceName();
				CString sCameraID;
				CString sCameraIDs;
				CString sSection;
				DWORD   dwID;
				CWK_Profile wkp;
				CSecID id; 

				sSection = _T("Vision\\Connections\\");
				sSection += sHost;
				sCameraIDs = wkp.GetString(sSection, sSequenceName, _T(""));
				sSequenceName += _T("Time");
				pDoc->SetSequenceTime(wkp.GetInt(sSection, sSequenceName, pDoc->GetSequenceIntervall()));

				HTREEITEM hServer = GetTreeItem(wServerID);
				HTREEITEM hCam	  = GetCameraTreeItem(hServer);
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
							CIPCOutputVision* pOutput = pServer->GetOutput();
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
							CIPCOutputVision* pOutput = pServer->GetOutput();
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
void CObjectView::OnInputConfirm() 
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
			if (pServer && pServer->IsConnected())
			{
				CIPCInputVision* pInput = pServer->GetInput();
				pInput->DoConfirmAlarm(id);
			}

		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CObjectView::OnUpdateInputConfirm(CCmdUI* pCmdUI) 
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
////////////////////////////////////////////////////////////////////////////
void CObjectView::OnHostDatetime() 
{
	WORD wServer = GetSelectedServer();
	CServer* pServer = GetDocument()->GetServer(wServer);
	if (pServer && pServer->IsConnected())
	{
		COEMDateTimeDialog dlg(this);

		if (IDOK==dlg.DoModal())
		{
			WORD wServer = GetSelectedServer();
			CServer* pServer = GetDocument()->GetServer(wServer);
			if (pServer && pServer->IsConnected())
			{
				CIPCInputVision* pInput = pServer->GetInput();
				pInput->DoRequestSetSystemTime(dlg.GetSystemTime());
			}
		}
	}
	
}
////////////////////////////////////////////////////////////////////////////
void CObjectView::OnUpdateHostDatetime(CCmdUI* pCmdUI) 
{
	WORD wServer = GetSelectedServer();
	CServer* pServer = GetDocument()->GetServer(wServer);
	pCmdUI->Enable(   pServer 
				   && theApp.m_pPermission
				   && (theApp.m_pPermission->GetFlags() & WK_ALLOW_DATE_TIME));
}
////////////////////////////////////////////////////////////////////////////
void CObjectView::OnHostCameraMap() 
{
	WORD wServerID = GetSelectedServer();
	GetDocument()->UpdateAllViews(this,MAKELONG(VDH_CAMERA_MAP,wServerID),NULL);
}
////////////////////////////////////////////////////////////////////////////
void CObjectView::OnUpdateHostCameraMap(CCmdUI* pCmdUI) 
{
	BOOL bVersion = theApp.m_sVersion>=_T("4.1");
	if (bVersion)
	{
		WORD wServerID = GetSelectedServer();
		CSecID secID (SECID_LOCAL_HOST); // Local Host
		CServer* pServer = GetDocument()->GetServer(wServerID);
		if (pServer)
		{
			secID = pServer->GetHostID();
		}
		CString sFile;
		sFile.Format(_T("%s\\%08lx\\camera.htm"),CNotificationMessage::GetWWWRoot(), secID);
#ifdef _HTML_ELEMENTS
		pCmdUI->Enable(DoesFileExist(sFile));
#else
		pCmdUI->Enable(FALSE);
#endif
	}
	else 
	{
		pCmdUI->Enable(FALSE);
	}
}
//////////////////////////////////////////////////////////////////////
void CObjectView::OnInputInformation() 
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
void CObjectView::OnUpdateInputInformation(CCmdUI* pCmdUI) 
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
				&& pServer->IsConnected()
				&& !pServer->IsB3())
			{
				pCmdUI->Enable(TRUE);
				return;
			}
		}
	}
	pCmdUI->Enable(FALSE);
}
//////////////////////////////////////////////////////////////////////
void CObjectView::OnInputDeactivate()  /*RM*/
{
	// nur ein Dialog zur Zeit erlaubt, daher keine Anfrage, wenn offen
	if (m_pInputDeactivationDialog == FALSE)
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
				if (pServer && pServer->IsConnected())
				{
					CIPCInputVision* pInput = pServer->GetInput();
					pInput->DoRequestGetAlarmOffSpans(id);
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CObjectView::OnUpdateInputDeactivate(CCmdUI* pCmdUI) /*RM*/
{
	BOOL bEnable = FALSE;
	HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
	if (hItem)
	{
		CSecID id(GetTreeCtrl().GetItemData(hItem));
		if (   id.IsInputID()
			&& (m_pInputDeactivationDialog == NULL)
			)
		{
				WORD wServerID = GetSelectedServer();
				CServer* pServer = GetDocument()->GetServer(wServerID);
				if (pServer && pServer->IsConnected())
				{
					CIPCInputVision* pInput = pServer->GetInput();
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

//ML 24.9.99 Begin Insertation{
//////////////////////////////////////////////////////////////////////
void CObjectView::SwitchRelay(WORD wServerID, CSecID id, BOOL bOn)
{
	HTREEITEM hItem = GetTreeItem(wServerID, id.GetID());
	if (hItem)
	{
		// Die Logik von 'SwitchRelay(hItem, !bOn)' ist genau umgekehrt: bOn=1 ->Relay auf
		SwitchRelay(hItem, !bOn); 
	}
}
//ML 24.9.99 End Insertation}
//////////////////////////////////////////////////////////////////////
void CObjectView::OnAlarmOffSpanData(WORD wServerID, CSecID id)
{
	// 
	if (!m_pInputDeactivationDialog)
	{											 
		CServer* pServer = GetDocument()->GetServer(wServerID);
		if (pServer)
		{
			m_pInputDeactivationDialog = new CInputDeactivationDialog(this,pServer, id);
		}
		else
		{
//			WK_TRACE_ERROR(_T("OnAlarmOffSpanData NO server for id %u\n"), wServerID);
		}
	}
	else
	{
//		WK_TRACE_WARNING(_T("OnAlarmOffSpanData while dialog open; server id %u\n"), wServerID);
	}
}
//////////////////////////////////////////////////////////////////////
void CObjectView::OnAlarmOffSpan(WORD wServerID,CSecID id, BOOL bOn)
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
void CObjectView::OnUpdateAuStartPlay(CCmdUI* pCmdUI) 
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
//////////////////////////////////////////////////////////////////////////
void CObjectView::OnUpdateAuStartDuplexPlay(CCmdUI* pCmdUI) 
{
	OnUpdateAuStartPlay(pCmdUI);
}
//////////////////////////////////////////////////////////////////////
void CObjectView::OnUpdateAuStopPlay(CCmdUI* pCmdUI) 
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
//////////////////////////////////////////////////////////////////////
void CObjectView::OnUpdateAuStartRec(CCmdUI* pCmdUI) 
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
//////////////////////////////////////////////////////////////////////
void CObjectView::OnUpdateAuStopRec(CCmdUI* pCmdUI)
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
void CObjectView::OnUpdateAuStopAny(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetActiveRecord(GetDocument()->GetLocalAudio()) != NULL);
}
//////////////////////////////////////////////////////////////////////
void CObjectView::OnUpdateAuProperties(CCmdUI* pCmdUI) 
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
/*********************************************************************************************
 Class      : CObjectView
 Function   : OnAuStartPlay
 Description: Starts the announcement to an external hosts audio destination.

 Parameters: None 

 Result type:  (void)
 created: October, 09 2003
 <TITLE OnAuStartPlay>
*********************************************************************************************/
void CObjectView::OnAuStartPlay()
{
	OnAuStartPlay(GetTreeCtrl().GetSelectedItem());
}
void CObjectView::OnAuStartPlay(HTREEITEM hSelected) 
{
	CIPCAudioVision *pLocalAudio = GetDocument()->GetLocalAudio();
	if (pLocalAudio && pLocalAudio->GetDefaultOutputID() != SECID_NO_ID)															// get local audio device
	{
		WORD wHost, wServer = GetSelectedServer(&wHost);
		CServer *pServer = GetServer(wServer);							// get external Server
		if (pServer)
		{
			CIPCAudioVision *pAudio = pServer->GetAudio();	// get external audio device
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
//////////////////////////////////////////////////////////////////////////
void CObjectView::OnAuStartDuplexPlay() 
{
	m_bDuplex = TRUE;
	OnAuStartPlay(GetTreeCtrl().GetSelectedItem());
}
/*********************************************************************************************
 Class      : CObjectView
 Function   : OnAuStopPlay
 Description: Stops an announcement to a host audio destination.

 Parameters: None 

 Result type:  (void)
 created: December, 12 2003
 <TITLE OnAuStopPlay>
*********************************************************************************************/
void CObjectView::OnAuStopPlay()
{
	OnAuStopPlay(GetTreeCtrl().GetSelectedItem());
}
void CObjectView::OnAuStopPlay(HTREEITEM hSelected) 
{
	WORD wHost, wServer = GetSelectedServer(&wHost, hSelected);
	CServer *pServer = GetServer(wServer);
	if (pServer)
	{
		CIPCAudioVision *pAudio = pServer->GetAudio();
		if (pAudio)
		{
			CTreeCtrl &theCtrl =GetTreeCtrl();								// stop the external device
			pAudio->StopPlay(theCtrl.GetItemData(hSelected));
		}
	}
	
	if (m_bDuplex == 2)
	{
		CIPCAudioVision *pLocalAudio = GetDocument()->GetLocalAudio();
		if (pLocalAudio)														// get local audio device
		{
			pLocalAudio->StopPlay(pLocalAudio->GetDefaultOutputID());
		}
		m_bDuplex = FALSE;
	}
}
/*********************************************************************************************
 Class      : CObjectView
 Function   : OnAuStartRec
 Description: Starts to listen in an external hosts audio source.

 Parameters: None 

 Result type:  (void)
 created: December, 12 2003
 <TITLE OnAuStartRec>
*********************************************************************************************/
void CObjectView::OnAuStartRec()
{
	OnAuStartRec(GetTreeCtrl().GetSelectedItem());
}
void CObjectView::OnAuStartRec(HTREEITEM hSelected) 
{
	WORD wHost, wServer = GetSelectedServer(&wHost, hSelected);
	CServer *pServer = GetServer(wServer);
	if (pServer)																			// get the source server
	{
		CIPCAudioVision *pAudio = pServer->GetAudio();
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
				CIPCAudioVision *pLocalAudio = GetDocument()->GetLocalAudio();
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
/*********************************************************************************************
 Class      : CObjectView
 Function   : OnAuStopRec
 Description: Stops to listen in an external hosts audio source.

 Parameters: None 

 Result type:  (void)
 created: December, 12 2003
 <TITLE OnAuStopRec>
*********************************************************************************************/
void CObjectView::OnAuStopRec()
{
	OnAuStopRec(GetTreeCtrl().GetSelectedItem());
}
void CObjectView::OnAuStopRec(HTREEITEM hSelected) 
{
	WORD wHost;
	WORD wServer = GetSelectedServer(&wHost, hSelected);
	CServer *pServer = GetServer(wServer);
	if (pServer)
	{
		CIPCAudioVision *pAudio = pServer->GetAudio();
		CIPCAudioVision *pDest  = NULL;
		if (pAudio)
		{
			CTreeCtrl &theCtrl = GetTreeCtrl();
			pDest = (CIPCAudioVision*) pAudio->GetDestination(0);
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
void CObjectView::OnAuProperties() 
{
	WORD wHost, wServer;
	CIPCMediaRecord*pMR = GetSelectedItemsMediaRecord(wHost, wServer);
	CServer *pServer = GetServer(wServer);
	if (pServer)
	{
		CIPCAudioVision *pAudio = pServer->GetAudio();
		if (pMR == NULL)
		{
			pMR = GetDefaultInputRecord(pAudio);
		}
		CIPCAudioVision *pLocalAudio = GetDocument()->GetLocalAudio();
		if (pMR && pAudio && pLocalAudio)
		{
			CWK_Profile wkp;
			if (wkp.GetInt(_T("Debug"), _T("TestAudioConnection"), 0) & EXTENDED_PROPERTIES)
			{
				CAudioPropertiesDlg dlg(pAudio, pLocalAudio, this);
				dlg.SetSecID(pMR->GetID());
				dlg.m_bReduced = false;
				dlg.DoModal();
			}
			else
			{
				CAudioPropertiesDlg dlg(pAudio, pLocalAudio, this);
				dlg.SetSecID(pMR->GetID());
				dlg.m_sInput = pServer->GetFullName();
				dlg.DoModal();
			}
			pAudio->SetNotifyWindow(this);
			pLocalAudio->SetNotifyWindow(this);
		}
	}
}
/*********************************************************************************************
 Class      : CObjectView
 Function   : OnAuStopAny
 Description: Stops any existing audio connection

 Parameters: None 

 Result type:  (void)
 created: December, 12 2003
 <TITLE OnAuStopAny>
*********************************************************************************************/
void CObjectView::OnAuStopAny() 
{
	CIPCAudioVision *pLocalAudio = GetDocument()->GetLocalAudio();
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
LRESULT CObjectView::OnNotifyWindow(WPARAM dwCmd, LPARAM nID)
{
	if (MEDIA_COMMAND_VALUE(dwCmd) == MEDIA_STATE)
	{
		
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
void CObjectView::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
#ifdef _HTML_ELEMENTS
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	CTreeCtrl &theTree = GetTreeCtrl();
	CSecID id(theTree.GetItemData(pNMTreeView->itemNew.hItem));
	CVisionView *pVV = GetDocument()->GetVisionView();
	int nHtmlType = 0;
	if (pVV)
	{
		CSmallWindow *pSW = pVV->GetCmdActiveSmallWindow();
		if (pSW && pSW->IsHtmlWindow())
		{
			nHtmlType = ((CHtmlWindow*)pSW)->GetHTMLwndType();
		}
	}
	if (   ((nHtmlType == HTML_TYPE_HOST_MAP)   && (theTree.GetParentItem(pNMTreeView->itemNew.hItem) == NULL))
       || ((nHtmlType == HTML_TYPE_CAMERA_MAP) && (id.IsInputID() || id.IsOutputID() 
#ifdef _DEBUG
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
			ptBegin.y = ((CRect*)&ii.rcImage)->Height()/2;
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
#endif
	*pResult = 0;
}
//////////////////////////////////////////////////////////////////////////
void CObjectView::OnLButtonUp(UINT nFlags, CPoint point) 
{
#ifdef _HTML_ELEMENTS
	if (m_pDragImage)
	{
		CWnd *pWnd = AfxGetMainWnd();
		ReleaseCapture();
		m_pDragImage->DragLeave(pWnd);
		m_pDragImage->EndDrag();
		CVisionView *pView = GetDocument()->GetVisionView();
		ASSERT(pView != NULL);
		point += GetWindowOffset(pView);
		CSmallWindow* pSmallWnd = pView->GetCmdActiveSmallWindow();
		if (pSmallWnd && pSmallWnd->IsHtmlWindow())
		{
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
			((CHtmlWindow*)pSmallWnd)->InsertElement(id, sName, wServer, wHost, pMR);
		}
		m_bDraggedInto =FALSE;
		WK_DELETE(m_pDragImage);
		InvalidateRect(NULL);
	}
#endif
	CTreeView::OnLButtonUp(nFlags, point);
}
//////////////////////////////////////////////////////////////////////////
void CObjectView::OnMouseMove(UINT nFlags, CPoint point) 
{
#ifdef _HTML_ELEMENTS
	if (m_pDragImage)
	{
		point += GetWindowOffset(AfxGetMainWnd());
		m_pDragImage->DragMove(point);
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND)) ? TRUE:FALSE;
		CVisionView *pView = GetDocument()->GetVisionView();
		ASSERT(pView != NULL);
		CSmallWindow* pSmallWnd = pView->GetCmdActiveSmallWindow();
		if (pSmallWnd && pSmallWnd->IsHtmlWindow())
		{
			CHtmlWindow*pHW = (CHtmlWindow*)pSmallWnd;
			CPoint pt;
			GetCursorPos(&pt);
			pHW->ScreenToClient(&pt);
			BOOL bInside = pHW->DraggedInto(pt);
			if (m_bDraggedInto)
			{
				if (!bInside)
				{
					pHW->CheckDragScroll(&pt);
				}
			}
			else
			{
				m_bDraggedInto = bInside;
			}
		}
	}
#endif
	CTreeView::OnMouseMove(nFlags, point);
}
//////////////////////////////////////////////////////////////////////////
CPoint CObjectView::GetWindowOffset(CWnd *pWnd)
{
	ASSERT(pWnd != NULL);
	CRect rc1, rc2;
	pWnd->GetWindowRect(&rc1);
	GetWindowRect(&rc2);
	return rc2.TopLeft() - rc1.TopLeft();
}
//////////////////////////////////////////////////////////////////////////
CIPCMediaRecord* CObjectView::GetDefaultInputRecord(CIPCAudioVision *pAudio)
{
	if (pAudio && pAudio->GetDefaultInputID() != SECID_NO_ID)
	{
		const CIPCMediaRecord &rec = pAudio->GetMediaRecordFromSecID(pAudio->GetDefaultInputID());
		return (CIPCMediaRecord*)&rec;
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
CIPCMediaRecord* CObjectView::GetDefaultOutputRecord(CIPCAudioVision *pAudio)
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
CIPCMediaRecord* CObjectView::GetActiveRecord(CIPCAudioVision *pAudio, DWORD dwFlags)
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
//////////////////////////////////////////////////////////////////////////
#ifdef _HTML_ELEMENTS
void  CObjectView::AddLocalHTMLmapping(DWORD dwID, CSecID elementID)
{
	DWORD *pdwElement = new DWORD[2];
	pdwElement[MAPPING_SERVER] = dwID;
	pdwElement[MAPPING_EL_ID]  = elementID.GetID();
	m_LocalHTMLmappings.AddTail(pdwElement);
}
//////////////////////////////////////////////////////////////////////////
void CObjectView::DeleteLocalHTMLmappings()
{
	while (m_LocalHTMLmappings.GetCount())
	{
		delete m_LocalHTMLmappings.RemoveHead();
	}
}
//////////////////////////////////////////////////////////////////////////
BOOL  CObjectView::IsOnLocalHTMLpage(WORD wHostID, CSecID elementID)
{
	POSITION pos = m_LocalHTMLmappings.GetHeadPosition();
	DWORD *pdwElement;
	while (pos)
	{
		pdwElement = (DWORD*) m_LocalHTMLmappings.GetNext(pos);
		if (HOSTID_OF(pdwElement[MAPPING_SERVER]) == wHostID && pdwElement[MAPPING_EL_ID] == elementID.GetID())
		{
			return TRUE;
		}
	}
	return FALSE;
}
#endif
//////////////////////////////////////////////////////////////////////////
void CObjectView::OnTimer(UINT nIDEvent) 
{
	CTreeView::OnTimer(nIDEvent);
}
