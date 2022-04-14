// SyncCoolBar.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Recherche.h"

#include "SyncCoolBar.h"
#include "mainfrm.h"
#include "RechercheDoc.h"
#include "RechercheView.h"
#include "server.h"
#include "CIPCDatabaseRecherche.h"
#include "DisplayWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CSyncCoolBar, CCoolBar)
/////////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSyncCoolBar, CCoolBar)
	//{{AFX_MSG_MAP(CSyncCoolBar)
	ON_COMMAND(ID_SYNC_BACK, OnSyncBack)
	ON_COMMAND(ID_SYNC_STEP_BACK, OnSyncSingleBack)
	ON_COMMAND(ID_SYNC_STOP, OnSyncStop)
	ON_COMMAND(ID_SYNC_FORWARD, OnSyncForward)
	ON_COMMAND(ID_SYNC_STEP_FORWARD, OnSyncSingleForward)
	ON_UPDATE_COMMAND_UI(ID_SYNC_BACK, OnUpdateSyncBack)
	ON_UPDATE_COMMAND_UI(ID_SYNC_STEP_BACK, OnUpdateSyncSingleBack)
	ON_UPDATE_COMMAND_UI(ID_SYNC_STOP, OnUpdateSyncStop)
	ON_UPDATE_COMMAND_UI(ID_SYNC_FORWARD, OnUpdateSyncForward)
	ON_UPDATE_COMMAND_UI(ID_SYNC_STEP_FORWARD, OnUpdateSyncSingleForward)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER,OnUser)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////////////
CSyncCoolBar::~CSyncCoolBar()
{
	m_QueryResults.SafeDeleteAll();
}
/////////////////////////////////////////////////////////////////////////////////////
int CSyncCoolBar::HitTest()
{
   m_wndToolBar.m_nStateFlags &= ~statusSet;
   return m_wndToolBar.HitButton(true);
}
/////////////////////////////////////////////////////////////////////////////////////
BOOL CSyncCoolBar::OnCreateBands()
{
	m_PlayStatus = PS_STOP;
	m_pMainFrame = (CMainFrame*)AfxGetMainWnd();
	m_dwQueryID = 2;
	m_bTimeValid = TRUE;
	m_iRequests = 0;
	m_dwTick = GetTickCount();
	m_dwTickDiff = 10;

	// Create tool bar
	if (!m_wndToolBar.Create(GetParent(),
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|
			CBRS_TOOLTIPS|CBRS_SIZE_DYNAMIC|CBRS_HIDE_INPLACE))
	{
		TRACE0("Failed to create toolbar\n");
		return FALSE; // failed to create
	}
	
    m_wndToolBar.GetToolBarCtrl().SetButtonSize(CSize(16,17));
	m_wndToolBar.SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);

    m_ilHot.Create(IDB_SYNC_HOT, 16, 0, RGB(255, 0, 255));
	m_wndToolBar.SetImageList(&m_ilHot);
	
	m_wndToolBar.ModifyStyle(0, TBSTYLE_FLAT|TBSTYLE_TRANSPARENT);
	m_wndToolBar.SetButtons(NULL, 11);

	int i,b;
	i = 0;
	b = 0;
    // set up each toolbar button
	m_wndToolBar.SetButtonInfo(i++, IDW_SYNC_DATE, TBBS_SEPARATOR, 180, 2);
	m_wndToolBar.SetButtonInfo(i++, IDW_SYNC_TIME, TBBS_SEPARATOR, 80, 2);
	m_wndToolBar.SetButtonInfo(i++, ID_SEPARATOR, TBBS_SEPARATOR, 12, 2);
	m_wndToolBar.SetButtonInfo(i++, ID_SYNC_BACK, TBSTYLE_BUTTON, b++, 2);
	m_wndToolBar.SetButtonInfo(i++, ID_SYNC_STEP_BACK, TBSTYLE_BUTTON, b++, 2);
	m_wndToolBar.SetButtonInfo(i++, ID_SYNC_STOP, TBSTYLE_BUTTON, b++, 2);
	m_wndToolBar.SetButtonInfo(i++, ID_SYNC_STEP_FORWARD, TBSTYLE_BUTTON, b++, 2);
	m_wndToolBar.SetButtonInfo(i++, ID_SYNC_FORWARD, TBSTYLE_BUTTON, b++, 2);
	m_wndToolBar.SetButtonInfo(i++, ID_SYNC, TBSTYLE_BUTTON, b++, 2);
	m_wndToolBar.SetButtonInfo(i++, ID_SEPARATOR, TBBS_SEPARATOR, 12, 2);
	m_wndToolBar.SetButtonInfo(i++, IDW_SYNC_SLIDER, TBBS_SEPARATOR, 200, 2);

	// Design guide advises 12 pixel gap between combos and buttons
	CSystemTime t;
	t.GetLocalTime();
	CRect rect;
	rect.top = 1;
	rect.left = 1;
	rect.right = 180;
	rect.bottom = 21;
	if (!m_wndDate.Create(WS_VISIBLE|WS_TABSTOP|DTS_LONGDATEFORMAT,
						  rect, 
						  &m_wndToolBar, 
						  IDW_SYNC_DATE))
	{
		TRACE(_T("Failed to create date control\n"));
		return FALSE;
	}
	rect.left = 182;
	rect.right = rect.left + 80;
	if (!m_wndTime.Create(WS_VISIBLE|WS_TABSTOP|DTS_TIMEFORMAT,
						  rect, 
						  &m_wndToolBar, 
						  IDW_SYNC_TIME))
	{
		TRACE(_T("Failed to create time control\n"));
		return FALSE;
	}

	rect.left  = 420;
	rect.right = rect.left+200;
	if (!m_wndSlider.Create(WS_VISIBLE|WS_TABSTOP|TBS_HORZ|TBS_TOP,
							rect,
							&m_wndToolBar,
							IDW_SYNC_SLIDER))
	{
		TRACE(_T("Failed to create slider\n"));
		return FALSE;
	}
	m_wndSlider.SetRange(0,200);

	// Get minimum size of bands
	CSize szHorz = m_wndToolBar.CalcDynamicLayout(-1, 0);	      // get min horz size
	CSize szVert = m_wndToolBar.CalcDynamicLayout(-1, LM_HORZ);	// get min vert size

	CSize sFixed = m_wndToolBar.CalcFixedLayout(FALSE,TRUE);

	// Band 1: Add toolbar band
	CRebarBandInfo rbbi;
	
	rbbi.fMask = RBBIM_STYLE|RBBIM_CHILD|RBBIM_CHILDSIZE|RBBIM_COLORS;
	rbbi.fStyle = 0;
	rbbi.hwndChild = m_wndToolBar;
	rbbi.cx = 0;
	rbbi.cxMinChild = sFixed.cx;//szHorz.cx+4;
	rbbi.cyMinChild = sFixed.cy;

	CWinXPThemes xp;
	if (xp.IsThemeActive())
	{
		rbbi.cyMinChild += 8;
	}

	rbbi.clrFore = GetSysColor(COLOR_BTNTEXT);
	rbbi.clrBack = GetSysColor(COLOR_BTNFACE);
	if (!InsertBand(0, &rbbi))
	{
		WK_TRACE_ERROR(_T("cannot insert toolbar band"));
		return FALSE;
	}

	if (theApp.m_sSyncTime.GetLength()==8+6)
	{
		CSystemTime t;
		t.SetDBDate(theApp.m_sSyncTime.Left(8));
		t.SetDBTime(theApp.m_sSyncTime.Right(6));
		SetDateTime(t);
	}

   m_dwStyle |= CBRS_FLYBY; // Für Tooltips in der Statusbar

	return 0; // OK
}
/////////////////////////////////////////////////////////////////////////////////////
void CSyncCoolBar::AddArchive(WORD wServer, WORD wArchive)
{
	DWORD dwID = MAKELONG(wServer,wArchive);

	BOOL bFound = FALSE;
	for (int i=0;i<m_dwIDs.GetSize();i++)
	{
		if (m_dwIDs[i]==dwID)
		{
			bFound = TRUE;
			break;
		}
	}
	if (!bFound)
	{
		m_dwIDs.Add(dwID);
		// opens the new archive
		if (m_PlayStatus != PS_STOP)
		{
			GotoTime(GetDateTime(),wServer,wArchive);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////
void CSyncCoolBar::DeleteArchive(WORD wServer, WORD wArchive, BOOL bCloseWindow)
{
	DWORD dwID = MAKELONG(wServer,wArchive);
	DWORD dw = 0;

	for (int i=m_dwIDs.GetSize()-1;i>=0;i--)
	{
		dw = m_dwIDs[i];
		if (   dw==dwID
			|| (   LOWORD(m_dwIDs[i]) == wServer
			&& wArchive == 0)
			)
		{
			m_dwIDs.RemoveAt(i);

			if(m_iRequests)
			{
				//es kann passieren, dass die Anzahl der Requests im DisplayWindow schon auf 0 ist
				//(so soll es beim Löschen des Archivfensters auch sein), aber dass der Zähler
				//hier im Sync noch auf 1 steht. Deshalb zurücksetzen,  da ansonsten 
				//beim nächsten zeitsynchronen Abspielen keine weiteren Bilder angefordert werden. 
				//Dies geht erst wieder, wenn der Zähler = 0 ist
				m_iRequests = 0;
			}
			
			if (bCloseWindow)
			{
				CDisplayWindow* pDW = m_pMainFrame->GetRechercheView()->GetDisplayWindow(LOWORD(dw),HIWORD(dw),0);
				if (pDW)
				{
					m_pMainFrame->GetRechercheView()->DeleteSmallWindow(wServer,pDW->GetID());
				}
			}
		}
	}
	if (m_dwIDs.GetSize() == 0)
	{
		SetPlayStatus(PS_STOP);
	}
}
/////////////////////////////////////////////////////////////////////////////////////
int CSyncCoolBar::GetNrOfArchives()
{
	return m_dwIDs.GetSize();
}
/////////////////////////////////////////////////////////////////////////////////////
PlayStatus CSyncCoolBar::GetPlayStatus()
{
	return m_PlayStatus;
}
/////////////////////////////////////////////////////////////////////////////////////
void CSyncCoolBar::SetPlayStatus(PlayStatus ps)
{
	m_PlayStatus = ps;

	m_wndDate.EnableWindow(m_PlayStatus == PS_STOP);
	m_wndTime.EnableWindow(m_PlayStatus == PS_STOP);
}
/////////////////////////////////////////////////////////////////////////////////////
CSystemTime CSyncCoolBar::GetDateTime()
{
	CSystemTime t,d;
	m_wndDate.GetTime(&d);
	m_wndTime.GetTime(&t);
	d.wHour = t.wHour;
	d.wMinute = t.wMinute;
	d.wSecond = t.wSecond;

	return d;
}
/////////////////////////////////////////////////////////////////////////////
void CSyncCoolBar::SetDateTime(const CSystemTime& s)
{
	CSystemTime t,d;
	t = s;
	d = s;
	m_wndDate.SetTime(&d);
	m_wndTime.SetTime(&t);
}
/////////////////////////////////////////////////////////////////////////////
void CSyncCoolBar::OnSyncBack()
{
	if (m_PlayStatus == PS_STOP)
	{
		GotoTime(GetDateTime());
	}
	SetPlayStatus(PS_PLAY_BACK);
}
/////////////////////////////////////////////////////////////////////////////
void CSyncCoolBar::OnSyncSingleBack()
{
	if (m_PlayStatus == PS_STOP)
	{
		GotoTime(GetDateTime());
	}
	else
	{
		Skip(-1);
	}
	SetPlayStatus(PS_SKIP_BACK);
}
/////////////////////////////////////////////////////////////////////////////
void CSyncCoolBar::OnSyncStop()
{
	if (m_PlayStatus == PS_STOP)
	{
		GotoTime(GetDateTime());
	}
	SetPlayStatus(PS_STOP);
	NotifyPlayStatusToDisplayWindows();
}
/////////////////////////////////////////////////////////////////////////////
void CSyncCoolBar::OnSyncForward()
{
	if (m_PlayStatus == PS_STOP)
	{
		GotoTime(GetDateTime());
	}
	SetPlayStatus(PS_PLAY_FORWARD);
	NotifyPlayStatusToDisplayWindows();
}
/////////////////////////////////////////////////////////////////////////////
void CSyncCoolBar::OnSyncSingleForward()
{
	if (m_PlayStatus == PS_STOP)
	{
		GotoTime(GetDateTime());
	}
	else
	{
		Skip(1);
	}
	SetPlayStatus(PS_SKIP_FORWARD);
}
/////////////////////////////////////////////////////////////////////////////
void CSyncCoolBar::OnUpdateSyncBack(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetNrOfArchives()>0);
	pCmdUI->SetCheck(GetPlayStatus()==PS_PLAY_BACK);
}
/////////////////////////////////////////////////////////////////////////////
void CSyncCoolBar::OnUpdateSyncSingleBack(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetNrOfArchives()>0);
}
/////////////////////////////////////////////////////////////////////////////
void CSyncCoolBar::OnUpdateSyncStop(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetNrOfArchives()>0);
}
/////////////////////////////////////////////////////////////////////////////
void CSyncCoolBar::OnUpdateSyncForward(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetNrOfArchives()>0);
	pCmdUI->SetCheck(GetPlayStatus()==PS_PLAY_FORWARD);
}
/////////////////////////////////////////////////////////////////////////////
void CSyncCoolBar::OnUpdateSyncSingleForward(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetNrOfArchives()>0);
}
/////////////////////////////////////////////////////////////////////////////
void CSyncCoolBar::GotoTime(const CSystemTime& t)
{
	TRACE(_T("GotoTime %s\n"),t.GetDateTime());
	for (int i=0;i<m_dwIDs.GetSize();i++)
	{
		WORD wServer = LOWORD(m_dwIDs[i]);
		WORD wArchive = HIWORD(m_dwIDs[i]);
		GotoTime(t,wServer,wArchive);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSyncCoolBar::GotoTime(const CSystemTime& t, WORD wServer, WORD wArchive)
{
	CServer* pServer = m_pMainFrame->GetDocument()->GetServer(wServer);
	if (pServer && pServer->IsConnected())
	{
		CIPCDatabaseRecherche* pDatabase = pServer->GetDatabase();
		CIPCFields fields;
		WORD w[1];
		w[0] = wArchive;
		fields.SafeAdd(new CIPCField(_T("DVR_DATE"),t.GetDBDate(),_T('}')));
		fields.SafeAdd(new CIPCField(_T("DVR_TIME"),t.GetDBTime(),_T('}')));

		DWORD dwID = MAKELONG(wServer,wArchive);

		m_mapQueryIDs.SetAt(m_dwQueryID,dwID);

		TRACE(_T("DoRequestNewQuery %08lx %04x\n"),m_dwQueryID,wArchive);
		pDatabase->DoRequestNewQuery(m_dwQueryID,1,w,fields,1);
		m_dwQueryID += 8;
		m_bTimeValid = FALSE;
	}
	else
	{
		TRACE(_T("no server for GotoTime\n"));
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSyncCoolBar::IsInQuery(DWORD dwQueryID)
{
	DWORD rValue = 0;
	TRACE(_T("IsInQuery QUERY Key %08lx\n"),dwQueryID);
	return m_mapQueryIDs.Lookup(dwQueryID,rValue);
}
/////////////////////////////////////////////////////////////////////////////
void CSyncCoolBar::ConfirmQuery(DWORD dwUserID)
{
	TRACE(_T("Remove QUERY Key %08lx\n"),dwUserID);
	m_mapQueryIDs.RemoveKey(dwUserID);
}
/////////////////////////////////////////////////////////////////////////////
void CSyncCoolBar::ConfirmQuery(DWORD dwUserID,
								WORD wServerNr,
								WORD wArchivNr,
							    WORD wSequenceNr, 
							    DWORD dwRecordNr,
							    int iNumFields,
							    const CIPCField fields[])
{
	m_QueryResults.Lock();
	BOOL bFound = FALSE;
	for (int i=0;i<m_QueryResults.GetSize() && !bFound;i++)
	{
		bFound =    m_QueryResults.GetAt(i)->GetServer() == wServerNr
			     && m_QueryResults.GetAt(i)->GetArchiveNr() == wArchivNr;
	}
	m_QueryResults.Unlock();

	if (!bFound)
	{
		TRACE(_T("Sync Query result S%d A%04hx S%d R%d\n"),wServerNr,wArchivNr,wSequenceNr,dwRecordNr);
		m_QueryResults.SafeAdd(new CQueryResult(dwUserID,wServerNr,
												wArchivNr,
												wSequenceNr,
												dwRecordNr,
												iNumFields,
												fields));
		DWORD dwID = MAKELONG(wServerNr,wArchivNr);
		PostMessage(WM_USER,SCB_QUERY_RESULT,dwID);
	}
	else
	{
		TRACE(_T("Sync Query result S%d A%04hx S%d R%d not shown, already found\n"),wServerNr,wArchivNr,wSequenceNr,dwRecordNr);
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CSyncCoolBar::OnUser(WPARAM wParam, LPARAM lParam) 
{
	switch (wParam)
	{
	case SCB_QUERY_RESULT:
		{
			WORD wServer = LOWORD(lParam);
			WORD wArchive = HIWORD(lParam);
			m_QueryResults.Lock();
			CQueryResult* pQueryResult = NULL;
			for (int i=0;i<m_QueryResults.GetSize() && (pQueryResult==NULL);i++)
			{
				if (   m_QueryResults.GetAt(i)->GetServer() == wServer
					&& m_QueryResults.GetAt(i)->GetArchiveNr() == wArchive
					)
				{
					pQueryResult = m_QueryResults.GetAt(i);
				}
			}
			m_QueryResults.Unlock();

			if (pQueryResult)
			{
				CRechercheView* pRW = m_pMainFrame->GetRechercheView(); 
				// get display window ignoring the sequence id
				CDisplayWindow* pDW = pRW->GetDisplayWindow(pQueryResult->GetServer(),
															pQueryResult->GetArchiveNr(),
															0);
				CServer* pServer = m_pMainFrame->GetDocument()->GetServer(pQueryResult->GetServer());
				if (pDW==NULL)
				{
					pServer->OpenSequence(pQueryResult->GetArchiveNr(),
										  pQueryResult->GetSequenceNr(),
										  pQueryResult->GetRecordNr());
					NotifyPlayStatusToDisplayWindows();
					m_iRequests++;
				}
				else
				{
					if (pDW->GetID().GetSubID() == pQueryResult->GetSequenceNr())
					{
/*
						if (pDW->ContainsAudio() && !pDW->CanNavigate())
						{
							return 0;
						}
*/
						pDW->Navigate(pQueryResult->GetRecordNr());
						m_iRequests++;
					}
					else
					{
						CIPCSequenceRecord* pSequence = pServer->GetCIPCSequenceRecord(pQueryResult->GetArchiveNr(),
										  pQueryResult->GetSequenceNr());

						if (pSequence)
						{
/*
							if (pDW->ContainsAudio() && !pDW->CanNavigate())
							{
								return 0;
							}
*/
							pDW->Navigate(*pSequence,pQueryResult->GetRecordNr());
							m_iRequests++;
						}
					}
				}
			}
		}
		break;
	}
	return 0L;
}
/////////////////////////////////////////////////////////////////////////////
void CSyncCoolBar::OnIdle()
{
	m_QueryResults.Lock();

	DWORD rValue=0;
	for (int i=m_QueryResults.GetSize()-1;i>=0;i--)
	{
		CQueryResult* pQueryResult = m_QueryResults.GetAt(i);
		if (!m_mapQueryIDs.Lookup(pQueryResult->GetUserID(),rValue))
		{
			m_QueryResults.RemoveAt(i);
			WK_DELETE(pQueryResult);
		}
	}
	m_QueryResults.Unlock();

	
	if (//m_mapQueryIDs.GetCount() == 0  //teilweise wurde die zeit nicht gesetzt, da QueryID`s != 0
		/*&&*/ !m_bTimeValid			 //siehe Bug 564
		&& m_PlayStatus!=PS_STOP)
	{
		CSystemTime t;
		if (m_pMainFrame->GetRechercheView()->GetOldestDisplayWindow(t))
		{
			SetDateTime(t);
		}
	}

	if (GetTimeSpan(m_dwTick)>m_dwTickDiff)
	{
		if (m_iRequests==0)
		{
			switch (m_PlayStatus)
			{
			case PS_PLAY_BACK:
				OnPlayBack();
				break;
			case PS_PLAY_FORWARD:
				OnPlayForward();
				break;
			case PS_STOP:
				OnStop();
				break;
			case PS_SKIP_BACK:
				OnSkipBack();
				break;
			case PS_SKIP_FORWARD:
				OnSkipForward();
				break;
			}
		}
		m_dwTick = GetTickCount();
		// range is 0..200 damit 10 bis 1010 ms
		m_dwTickDiff = m_wndSlider.GetPos() * 5 + 10;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSyncCoolBar::OnPlayBack()
{
	Skip(-1);
}
/////////////////////////////////////////////////////////////////////////////
void CSyncCoolBar::OnSkipBack()
{
}
/////////////////////////////////////////////////////////////////////////////
void CSyncCoolBar::OnPlayForward()
{
	Skip();
}
/////////////////////////////////////////////////////////////////////////////
void CSyncCoolBar::OnSkipForward()
{
}
/////////////////////////////////////////////////////////////////////////////
void CSyncCoolBar::OnStop()
{
}
/////////////////////////////////////////////////////////////////////////////
void CSyncCoolBar::Skip(int i/*=1*/)
{
	CSystemTime t;
	CDisplayWindow* pDW = NULL;
	
	if (i>0)
	{
		pDW = m_pMainFrame->GetRechercheView()->GetOldestDisplayWindow(t);
	}
	else
	{
		pDW = m_pMainFrame->GetRechercheView()->GetYoungestDisplayWindow(t);
	}
	if (pDW)
	{
		if (pDW->ContainsAudio() && !pDW->CanNavigate())
		{
			return;
		}
		if (pDW->Skip(TRUE,i))
		{
			m_iRequests++;
		}
		else
		{
			SetPlayStatus(PS_STOP);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSyncCoolBar::ConfirmPicture()
{
	if (m_iRequests>0)
	{
		m_iRequests--;
	}
}

void CSyncCoolBar::OnDestroy() 
{
	CSystemTime t = GetDateTime();

	theApp.m_sSyncTime = t.GetDBDate() + t.GetDBTime();

	CCoolBar::OnDestroy();
}

void CSyncCoolBar::NotifyPlayStatusToDisplayWindows()
{
	DWORD dw;
	for (int i=m_dwIDs.GetSize()-1;i>=0;i--)
	{
		dw = m_dwIDs[i];
		CDisplayWindow* pDW = m_pMainFrame->GetRechercheView()->GetDisplayWindow(LOWORD(dw),HIWORD(dw),0);
		if (pDW)
		{
			pDW->SetSyncPlayStatus(m_PlayStatus);
		}
	}
}
