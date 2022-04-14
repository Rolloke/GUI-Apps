// PlayWindow.cpp : implementation file
//

#include "stdafx.h"
#include "dvclient.h"
#include "PlayWindow.h"
#include "LiveWindow.h"

#include "Server.h"
#include "IPCDatabaseDVClient.h"
#include "MainFrame.h"
#include "CPanel.h"
#include "CopyReadOnlyVersion.h"
#include "InitPacketCD.h"
#include "PlayRealTime.h"
#include "CIPCAudioDVClient.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#ifdef _DEBUG
//  #define BUFERED_PICTURE_IF_AUDIO
//TKR war mit RKE zum Testen eines Zwischenspeicherns und späteren Abspielens von Bilddaten 
//parallel zu Audiodaten gedacht. Test hat aber bei Mpeg nicht geklappt, da ein Bild in den Puffer
//gelegt wird (z.B. Bild Nr. 5) dann wird Bild 6 angefordert, dann erkannt, das zunächst
//Bild 5 angezeigt (dekodiert) werden muss, dann wird das nächste Bild angefordert. Das
//nun aber angeforderte Bild hat die Nr. 7 weil zu es dem sich im Puffer befindliche Bild keine
//Angabe über dessen Sequenz und RecordNr. gab. Entsprechend gab es geisterbilder.
//Diese ganze Problematik wird später mittels einer weiteren, komplett neuen Bild und Audio Pufferklasse
//gelöst werden. 
//#endif

////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CPlayWindow,CDisplayWindow)
/////////////////////////////////////////////////////////////////////////////
// CPlayWindow

CPlayWindow::CPlayWindow(CMainFrame* pParent, CServer* pServer, const CIPCArchivRecord& rec):
	CDisplayWindow(pParent, pServer),
	m_ArchivRecord(rec)
{
	m_PlayStatus               = PS_EJECT;
	m_RectPlayStatus           = PS_RECT_NO_RECT;
	m_iFFStepWidth             = 2;
	m_iFFPicts                 = 0;
	m_dwLastRequest            = GetTickCount() - REQUEST_TIMEOUT_PLAY;
	m_bCanMDTrack              = FALSE;
	m_bSuspect                 = FALSE;
	m_bManipulated             = FALSE;
	m_bBackupSymbol            = SYMBOL_BACKUP1;
	m_wCurrentSequence         = 0;
	m_dwCurrentRecord          = 1;
	m_bNavigateToLastRecord    = FALSE;
	m_dwNavigateFailedStart    = 0;
	m_bEjectAfterBackupRemoved = FALSE;
	m_dwStartDeleting          = 0;
	m_dwStartDeleted           = 0;
	m_bGetPicture				= FALSE;

	m_wLastSequenceRequest     = 0;
	m_dwLastRecordRequest      = 0;
	m_dwLastNewRecordRequest   = 0;
	m_dwLastCamRequest         = 0;
	m_LastPlayStatus           = m_PlayStatus;
	m_dwStartQueryFailed       = 0;

	m_bCanMilliSeconds         = m_pServer->CanMilliSeconds();

	m_bGetSequenceEndtime      = FALSE;

	m_pPlayRealTime			   = NULL;
//	m_pPlayRealTime			   = new CPlayRealTime();

	m_pAudioRecord             = NULL;
	m_bSetSyncPoint            = true;
	m_bDoPrefetch              = false;
	m_bContainsAudio           = false;
	m_bReInitAudio             = true;
	m_dwRequestedRecord        = 1;
	m_wRequestedSequence	   = 1;

//TODO TKR für Sycn Abspielen
/*
	m_bCanMakeSyncBigOne	= TRUE;
*/
	InitRectQueryMembers();
}

CPlayWindow::~CPlayWindow()
{
	m_Sequences.SafeDeleteAll();
	WK_DELETE(m_pPlayRealTime);
	WK_DELETE(m_pAudioRecord);
	CIPCAudioDVClient* pAudio = theApp.GetLocalAudio();
	if (pAudio && pAudio->GetActiveDisplayWindow() == this)
	{
		pAudio->DoRequestStopMediaDecoding();
		pAudio->SetActiveDisplayWindow(NULL);
	}
}


BEGIN_MESSAGE_MAP(CPlayWindow, CDisplayWindow)
	//{{AFX_MSG_MAP(CPlayWindow)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_PLAY_REALTIME,OnPlayRealTime)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
CSecID CPlayWindow::GetID() const
{
	return CSecID(SECID_GROUP_ARCHIVE,m_ArchivRecord.GetSubID());
}
/////////////////////////////////////////////////////////////////////////////
WORD CPlayWindow::GetNr() const
{
	return m_ArchivRecord.GetSubID();
}
/////////////////////////////////////////////////////////////////////////////
CString CPlayWindow::GetName() const
{
	return m_ArchivRecord.GetName();
}
/////////////////////////////////////////////////////////////////////////////
CSystemTime	CPlayWindow::GetStartTime()
{
	CSystemTime st;
	st.GetLocalTime();

	m_Sequences.Lock(_T(__FUNCTION__));
	if (m_Sequences.GetSize())
	{
		st = m_Sequences.GetAtFast(0)->GetTime();
	}
	m_Sequences.Unlock();

	return st;
}
/////////////////////////////////////////////////////////////////////////////
__int64	CPlayWindow::GetNrOfPictures()
{
	RequestSequenceList(TRUE);

	__int64 r = 0;
	m_Sequences.Lock(_T(__FUNCTION__));
	for (int i=0;i<m_Sequences.GetSize();i++)
	{
		r = r + (__int64)m_Sequences.GetAtFast(i)->GetNrOfPictures();
	}
	m_Sequences.Unlock();

	return r;
}
/////////////////////////////////////////////////////////////////////////////
CIPCSequenceRecord*	CPlayWindow::GetFirstNotBackupSequence()
{
	CIPCSequenceRecord* pRet = NULL;
	m_Sequences.Lock(_T(__FUNCTION__));
	for (int i=0;i<m_Sequences.GetSize() && (pRet==NULL);i++)
	{
		if (!m_Sequences.GetAtFast(i)->IsBackup())
		{
			pRet = m_Sequences.GetAtFast(i);
		}
	}
	m_Sequences.Unlock();

	return pRet;

}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::SetMDValues(WORD wX, WORD wY)
{
	if (wX>0)
	{
		m_bCanMDTrack = TRUE;
	}
	return CDisplayWindow::SetMDValues(wX,wY);
}
/////////////////////////////////////////////////////////////////////////////
//wird nur bei m_bCanRectSearch == TRUE verwendet
BOOL CPlayWindow::SetAllMDValues(WORD wX, WORD wY)
{
	m_csPictureRecord.Lock();

	m_waAllMD_X.Add(wX);
	m_waAllMD_Y.Add(wY);

	m_csPictureRecord.Unlock();

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
//wird nur bei m_bCanRectSearch == TRUE verwendet
//wird nur bei aktivierter Rechtecksuche verwendet
BOOL CPlayWindow::RemoveAllMDValues()
{
	m_csPictureRecord.Lock();

	m_waAllMD_X.RemoveAll();
	m_waAllMD_Y.RemoveAll();

	m_csPictureRecord.Unlock();

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
CString CPlayWindow::GetTitle(CDC* pDC)
{		    
	CString sName, sID;
	int nID = (GetID().GetSubID() & (MAX_CAM-1)) + 1;
	sID.Format(_T("%02d"), nID);
	if (sID != GetName())
	{
		sName = GetName() + _T(" ");
	}

	sID.Format(_T("(%02d) "), nID);
	CString sDate;
	sDate = GetDateString() + _T(" ");
	CString sTime = GetTimeString();
	CString sTitle;
	sTitle = sID + sName + sDate + sTime;

	{	// Critical Section
		CAutoCritSec cs(&m_csPictureRecord);
		if (m_PlayStatus == PS_PLAY_FORWARD && m_pAudioRecord)
		{
			sTitle += _T(", ") + GetAudioCompressionType() + _T(": ");
			CSystemTime st;
			m_pAudioRecord->GetMediaTime(st);
			sTitle += st.GetTime();
			return sTitle;
		}
	}

	// Fenstertitel ggf. anpassen, falls pDC vorhanden
	if (pDC)
	{
		CRect rect;
		GetClientRect(rect);
		// Ist der Titel zu lang
		CSize size = pDC->GetOutputTextExtent(sTitle);
		if (size.cx > rect.Width())
		{
			// Zeit ohne Sekunden anzeigen?
			CString sShortTime = sTime.Left(sTime.GetLength()-3);
			sTitle = sID + sName + sDate + sShortTime;
			// Ist er noch zu lang
			size = pDC->GetOutputTextExtent(sTitle);
			if (size.cx > rect.Width())
			{
				// Namen weglassen (Zuordnung ist über ID gegeben)
				sTitle = sID + sDate + sTime;
				size = pDC->GetOutputTextExtent(sTitle);
				if (size.cx > rect.Width())
				{
					// immer noch zu lang, also Jahreszahl
					// weglassen
					CString sShortDate = GetShortDateString() + _T(" ");
					sTitle = sID + sShortDate + sTime;
					size = pDC->GetOutputTextExtent(sTitle);
					if (size.cx > rect.Width())
					{
						// immer noch zu lang
						// also auch die Sekunden wieder weglassen
						sTitle = sID + sShortDate + sShortTime;
					}
				}
			}
			else
			{
				// Wenn nur die Sekunden zu viel sind,
				// können sie aber auch abgeschnitten werden
				sTitle = sID + sName + sDate + sTime;
			}
		}
	}
	if (theApp.ShowSequenceInfo())
	{
		CAutoCritSec acs(&m_Sequences.m_cs);
		CIPCSequenceRecord* pSR = m_Sequences.GetSequence(GetID().GetSubID(),m_wCurrentSequence);
		if (pSR)
		{
			CString sRecordNr;
			CString sTyp;
			if (pSR->IsAlarm())
			{
				sTyp = _T("ALARM");
			}
			else if (pSR->IsSafeRing())
			{
				sTyp = _T("PRE-ALARM");
			}
			else if (pSR->IsSuspect())
			{
				sTyp = _T("SUSPECT");
			}
			else
			{
				sTyp = _T("RING");
			}
			sRecordNr.Format(_T(" Rec(%d/%d), Seq %d, Typ %s"),m_dwCurrentRecord,
				pSR->GetNrOfPictures(),
				m_wCurrentSequence,
				sTyp);
			sTitle += sRecordNr;
		}
	}
	return sTitle;
}
/////////////////////////////////////////////////////////////////////////////
void CPlayWindow::GetFooter(CByteArray& Array)
{
	// Ist gezoomt?
	if (!m_rcZoom.IsRectEmpty())
	{
		Array.Add(SYMBOL_ZOOM);
	}

	if (m_bSuspect)
	{
		Array.Add(SYMBOL_SUSPECT);
	}

	if (m_wCurrentSequence!=0)
	{
		CAutoCritSec acs(&m_Sequences.m_cs);
		CIPCSequenceRecord* pSR = m_Sequences.GetSequence(m_ArchivRecord.GetID(), m_wCurrentSequence);
		if (pSR)
		{
			if (pSR->IsBackup())
			{
				Array.Add(m_bBackupSymbol++);
				if (m_bBackupSymbol>SYMBOL_BACKUP_LAST)
				{
					m_bBackupSymbol = SYMBOL_BACKUP1;
				}
			}
			// can be backup and alarm !
			if (pSR->IsAlarm())
			{
				Array.Add(SYMBOL_UVV);
				Array.Add(SYMBOL_ALARM);
			}
			else if (pSR->IsSafeRing())
			{
				Array.Add(SYMBOL_UVV);
				Array.Add(SYMBOL_PREALARM);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
CString CPlayWindow::GetDefaultText()
{
	CString s;

	switch(m_PlayStatus)
	{
	case PS_QUERY:
		s.LoadString(IDS_QUERY);
		s += m_sQuery;
		break;

	case PS_QUERY_FAILED:
		s.LoadString(IDS_QUERY_FAILED);
		s += m_sQuery;
		break;

	case PS_DELETING:
		s.LoadString(IDS_DELETE_ALARM);
		break;

	case PS_DELETED:
		if (m_waSequencesToDelete.GetSize()>0)
		{
			s.LoadString(IDS_DELETED_ALARM2);
		}
		else
		{
			s.LoadString(IDS_DELETED_ALARM);
		}
		break;
	case PS_NAVIGATE_FAILED:
		s.LoadString(IDS_NO_RECORD_FOUND);
		break;
	case PS_READ_FAILED:
		s.LoadString(IDS_READ_FAILED);
		break;

	default:
		if (m_Sequences.GetSize()>0)
		{
			s.LoadString(IDS_WAIT);

#ifdef _DEBUG
			s += NameOfEnum(m_PlayStatus);
#endif
		}
		else
		{
			s.LoadString(IDS_NO_PICTURES);
		}
		break;
	}
	return GetName() + _T("\n") + s;
}
//////////////////////////////////////////////////////////////////////////
CString CPlayWindow::GetAudioCompressionType()
{
	if (m_pAudioRecord && m_pAudioRecord->IsLongHeader())
	{
		CIPCAudioDVClient*pMedia = theApp.GetLocalAudio();
		if (pMedia)
		{
			CString sACT = pMedia->GetCompressionType(m_pAudioRecord);
			if (sACT != m_sAudioCompressionType)
			{
				m_bReInitAudio = true;
			}
			m_sAudioCompressionType = sACT;
		}
	}
	return m_sAudioCompressionType;
}
/////////////////////////////////////////////////////////////////////////////
COLORREF CPlayWindow::GetOSDColor()
{
	COLORREF col;

	if (IsActive())
		col = RGB(0,0,192);
	else
		col = RGB(0,0,128);
	return col;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::IsShowable()
{
	return (m_PlayStatus != PS_EJECT);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::IndicateSequenceList(int iNumRecords, const CIPCSequenceRecord data[])
{
	int iNum = 0;
	BOOL bSetCurrentSequenceNr = FALSE;
	m_Sequences.Lock(_T(__FUNCTION__));
	bSetCurrentSequenceNr = m_Sequences.GetSize()==0;
	m_Sequences.DeleteAll();
	for (int i=0;i<iNumRecords;i++)
	{
		/*
		TRACE(_T("Sequence %04hx, %04hx , %s Bytes\n"),data[i].GetArchiveNr(),
			data[i].GetSequenceNr(),data[i].GetSize().Format(TRUE));
			*/
		m_Sequences.Add(new CIPCSequenceRecord(data[i]));
	}
	m_Sequences.Sort();
	iNum = m_Sequences.GetSize(); 

	m_evRequestSequenceList.SetEvent();

//	WK_TRACE(_T("%s has %d seq\n"),GetName(),m_Sequences.GetSize());

	if (m_Sequences.GetSequence(GetID().GetSubID(),m_wCurrentSequence)==NULL)
	{
		// current Sequence no longer valid
		bSetCurrentSequenceNr = TRUE;
	}

	if (   bSetCurrentSequenceNr
		&& (iNum>0)
		)
	{
		m_wCurrentSequence = m_Sequences.GetAtFast(0)->GetSequenceNr();
	}
	m_Sequences.Unlock();

	if (IsWindowVisible())
	{
		TRACE(_T("play status is %s\n"),NameOfEnum(m_PlayStatus));
		DoRedraw();
	}
	UpdatePanel();	    

	
	return iNum>0;
}
/////////////////////////////////////////////////////////////////////////////
CSystemTime CPlayWindow::GetSequenceEndtime(WORD wSequenceNr)
{
	CSystemTime t;
	m_bGetSequenceEndtime = TRUE;
	GetServer()->GetDatabase()->DoRequestRecordNr(GetNr(),wSequenceNr,1,1,(DWORD)(-1-NAVIGATION_LAST));
	if (WAIT_OBJECT_0 == WaitForSingleObject(m_evGetSequenceEndtime,1000))
	{
		t = m_stGetSequenceEndtime;
	}
	m_bGetSequenceEndtime = FALSE;
	return t;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::RecordNr(WORD wSequenceNr, DWORD dwRecordNr, DWORD dwNrOfRecords,
						   const CIPCPictureRecord &pict, int iNumFields,
						   const CIPCField fields[])
{
#ifdef _DEBUG
		CSystemTime t;
		t.GetLocalTime();
		//TRACE(_T("##### confirm record: S=%d R=%d %s,%03d\n"),wSequenceNr,dwRecordNr,t.GetTime(),t.GetMilliseconds());
#endif

	//check for tasha picture, important for playing fastforward, fastback and rectangle-search
	m_bCanTashaRectangleSearch = FALSE;
	if(pict.GetCompressionType() == COMPRESSION_MPEG4)
	{
		m_bCanTashaRectangleSearch = TRUE;
	}
	BOOL bRet = FALSE;
	if (m_bGetSequenceEndtime)
	{
		TRACE(_T("confirmed sequence end time is %s\n"),pict.GetTimeStamp().GetDateTime());
		m_stGetSequenceEndtime = pict.GetTimeStamp();
		m_evGetSequenceEndtime.SetEvent();
		bRet = TRUE;
	}
	else
	{
		DWORD dwX,dwY;
		dwX = dwY = 0;

		//		TRACE(_T("*** RecordNr: Sequ: %i  RecNr: %i\n"),wSequenceNr,dwRecordNr );
		// delete last request
		m_wLastSequenceRequest = 0;
		m_dwLastRecordRequest = 0;
		m_dwLastNewRecordRequest = 0;
		m_dwLastCamRequest = 0;
		
		/*
		TRACE(_T("Confirm A=%04x, S=%d R=%d %s\n"),
		m_ArchivRecord.GetSubID(),wSequenceNr,dwRecordNr,NameOfEnum(m_PlayStatus));
		*/
		m_wCurrentSequence = wSequenceNr;
		m_dwCurrentRecord = dwRecordNr;

		m_Sequences.Lock(_T(__FUNCTION__));
		CIPCSequenceRecord* pSR = m_Sequences.GetSequence(GetID().GetSubID(),wSequenceNr);
		if(pSR)
		{
			pSR->SetNrOfPictures(dwNrOfRecords);
		}
		m_Sequences.Unlock();
	

		CIPCFields f;
		f.FromArray(iNumFields,fields);
		CString sDBFieldX;
		CString sDBFieldY;
		CIPCField *pFX,*pFY,*pFS;

		if(m_bCanRectSearch && !theApp.GetMainFrame()->IsInBackup())
		{
			//sichere alle Fadenkreuze des Bildes
			pFX = f.GetCIPCField(_T("DBD_MD_X"));
			if (pFX == NULL)
			{
				pFX = f.GetCIPCField(_T("DVD_MD_X"));
				sDBFieldX = _T("DVD_MD_X");
			}
			else
			{
				sDBFieldX = _T("DBD_MD_X");
			}
			
			pFY = f.GetCIPCField(_T("DBD_MD_Y"));
			if (pFY == NULL)
			{
				pFY = f.GetCIPCField(_T("DVD_MD_Y"));
				sDBFieldY = _T("DVD_MD_Y");
			}
			else
			{
				sDBFieldY = _T("DBD_MD_Y");
			}
			
			if (pFX && pFY)
			{
				//setze Fadenkreuze
				CIPCField *pFMoreX = NULL;
				CIPCField *pFMoreY = NULL;
				RemoveAllMDValues();
				for(int i=0; i<=4; i++)
				{
					CString s, sX, sY;;
					sX = sDBFieldX;
					sY = sDBFieldY;
					if(i>0)
					{			
						s.Format(_T("%i"), i+1);
						sX = sDBFieldX + s;
						sY = sDBFieldY + s;
					}
					
					pFMoreX = f.GetCIPCField(sX);
					pFMoreY = f.GetCIPCField(sY);
					
					if(pFMoreX && pFMoreY)
					{
						_stscanf(pFMoreX->GetValue(),_T("%x"),&dwX);
						_stscanf(pFMoreY->GetValue(),_T("%x"),&dwY);
					}
					else
					{
						if(theApp.TKR_Trace())
						{
							TRACE(_T("TKR ------------ RecordNr() keine weiteren Kreuze\n"));
						}
					}
					
					if ((dwX>0) && (dwY>0))
					{
						SetAllMDValues((WORD)dwX, (WORD)dwY);
					}
					else
					{
						SetAllMDValues(0, 0);
					}
				}
			}
		}
		else
		{
			pFX = f.GetCIPCField(_T("DBD_MD_X"));
			if (pFX == NULL)
			{
				pFX = f.GetCIPCField(_T("DVD_MD_X"));
			}
			pFY = f.GetCIPCField(_T("DBD_MD_Y"));
			if (pFY == NULL)
			{
				pFY = f.GetCIPCField(_T("DVD_MD_Y"));
			}
			if (pFX && pFY)
			{
				_stscanf(pFX->GetValue(),_T("%04x"),&dwX);
				_stscanf(pFY->GetValue(),_T("%04x"),&dwY);
			}
		}
		
		pFS = f.GetCIPCField(_T("DVD_MD_S"));
		if (pFS && pFS->GetValue() == _T("2"))
		{
			m_bSuspect = TRUE;
		}
		else
		{
			m_bSuspect = FALSE;
		}
		
		m_bManipulated = FALSE;
		
		CIPCField *pError = f.GetCIPCField(_T("DVD_ERROR"));
		if (   pError 
			&& pError->GetValue() == _T("CHECKSUM"))
		{
			m_bManipulated = TRUE;
		}

		if (   (m_PlayStatus == PS_FAST_FORWARD)
			|| (m_PlayStatus == PS_FAST_BACK)
			)
		{
			if(m_bCanTashaRectangleSearch)
			{
				//bei Tasha Bildern (MPEG4) darf bei Anforderung des nächsten FastForward-Bildes
				//nicht mit der StepWith = 2 begonnen werden, da in diesem Fall stets das IFRame
				//geliefert wird und dementsprechend das angezeigte Datenbankbild IMMER
				//das selbe ist. Erst nach erreichen der StepWith = 5 wird das nächste IFrame
				//angefordert.
				
				if (m_iFFPicts<150)
				{
					m_iFFPicts++;
					m_iFFStepWidth = 5 + (m_iFFPicts/10);
				}
			}
			else
			{
				if (m_iFFPicts<180)
				{
					m_iFFPicts++;
					m_iFFStepWidth = 2 + (m_iFFPicts/10);
				}
			}

		}
		
		WK_DELETE(m_pAudioRecord);

#ifdef BUFERED_PICTURE_IF_AUDIO
		if (m_bDoPrefetch)									// if the records are prefetched from the database
		{													// picture recors may arrive too early
			if (GetTimeSpan(m_dwLastRequest) <= (DWORD)theApp.m_nTimeOutPlay)	
			{
				m_csPictureRecord.Lock(_T(__FUNCTION__));					// they are buffered in this case
				m_PictBuffer.AddTail(new CIPCPictureRecord(pict));
//				TRACE(_T("Video added to buffer %d\n"), m_PictBuffer.GetCount());
				m_csPictureRecord.Unlock();
				if(theApp.TKR_Trace())
				{
					CString sFrame = _T("P-Frame");
					if(pict.GetPictureType()==SPT_FULL_PICTURE)
					{
						sFrame = _T("I-Frame");
					}
					TRACE(_T("TKR ######### PUFFER AddTail RecordNr (Picture): SequNr: %d  RecNr: %d   %s \n"), m_wCurrentSequence, m_dwCurrentRecord,sFrame);
				}

				return TRUE;
			}
		}
#endif

		bRet = CDisplayWindow::PictureData(&pict,dwX,dwY);
		if (bRet)
		{
			if (pict.GetCompressionType() == COMPRESSION_JPEG)
			{
#ifdef _DEBUG
				CSystemTime t;
				t.GetLocalTime();
				TRACE(_T("##### post chg video: S=%d R=%d %s,%03d\n"),wSequenceNr,dwRecordNr,t.GetTime(),t.GetMilliseconds());
#endif
				PostMessage(WM_USER, ID_CHANGE_VIDEO);
			}
			else if (pict.GetCompressionType() == COMPRESSION_MPEG4)
			{
#ifdef _DEBUG
				CSystemTime t;
				t.GetLocalTime();
				if(theApp.TKR_Trace())
				{
					//TRACE(_T("##### confirm recor2: S=%d R=%d %s,%03d  Picture Time: %s - %d \n"),wSequenceNr,dwRecordNr,t.GetTime(),t.GetMilliseconds()
					//	,pict.GetTimeStamp().GetDateTime(), pict.GetTimeStamp().GetMilliseconds());
				}

#endif
				BOOL bDecode = DecodeMpeg(pict, dwX, dwY);
				if(bDecode)
				{
					if(theApp.TKR_Trace())
					{
						CString sFrame = _T("P-Frame");
						if(pict.GetPictureType()==SPT_FULL_PICTURE)
						{
							sFrame = _T("I-Frame");
						}
						TRACE(_T("TKR ######### DECODE RecordNr (Picture): SequNr: %d  RecNr: %d   %s \n"), m_wCurrentSequence, m_dwCurrentRecord,sFrame);
					}
				}
			}
			else
			{
				TRACE(_T("unknown picture type in playwindow\n"));
			}

			if (   m_dwCurrentRecord == 1
				|| m_dwCurrentRecord == 2
				|| m_dwCurrentRecord == 3
				|| m_dwCurrentRecord == dwNrOfRecords
				|| m_dwCurrentRecord == dwNrOfRecords-1
				|| m_dwCurrentRecord == dwNrOfRecords-2
				|| m_LastPlayStatus == PS_QUERY
				)
			{
				UpdatePanel();
			}
			
			Sleep(0);
			if(m_bCanRectSearch && !theApp.GetMainFrame()->IsInBackup())
			{
				//Wechsel in neuen Zustand, PS_RECT_PLAY_FORWARD kennzeichnet neuen Zustand
				if(    m_RectPlayStatus == PS_RECT_PLAY_FORWARD 
					&& m_PlayStatus != PS_PLAY_FORWARD)
				{
					ChangePlayStatus(PS_PLAY_FORWARD);
				}
				
				//Wechsel in neuen Zustand, PS_RECT_PLAY_BACK kennzeichnet neuen Zustand
				if(    m_RectPlayStatus == PS_RECT_PLAY_BACK
					&& m_PlayStatus != PS_PLAY_BACK)
				{
					ChangePlayStatus(PS_PLAY_BACK);
				}
				
				//Wechsel in neuen Zustand, PS_RECT_QUERY_RECT kennzeichnet neuen Zustand
				if(m_RectPlayStatus == PS_RECT_QUERY_RECT)
				{
					m_RectPlayStatus = PS_RECT_STOP;
					ChangePlayStatus(PS_QUERY_RECT);
				}
			}
		}
		else
		{
			if (m_iRequests>0)
			{
				m_iRequests--;
			}
		}
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::RecordNr(WORD wSequenceNr, DWORD dwRecordNr, DWORD dwNrOfRecords, const CIPCMediaSampleRecord &media)
{
	if (m_PlayStatus != PS_PLAY_FORWARD)
	{
		m_bGetPicture = TRUE;
	}

	m_bContainsAudio  = true;

	m_wLastSequenceRequest = 0;
	m_dwLastRecordRequest = 0;
	m_dwLastNewRecordRequest = 0;
	m_dwLastCamRequest = 0;

	m_wCurrentSequence = wSequenceNr;
	m_dwCurrentRecord = dwRecordNr;
	
	m_Sequences.Lock(_T(__FUNCTION__));
	CIPCSequenceRecord* pSR = m_Sequences.GetSequence(GetID().GetSubID(),wSequenceNr);
	if(pSR)
	{
		pSR->SetNrOfPictures(dwNrOfRecords);
	}
	m_Sequences.Unlock();

	if(theApp.TKR_Trace())
	{
		TRACE("TKR #### RecordNr (Audio): SequNr: %d  RecNr: %d\n", m_wCurrentSequence, m_dwCurrentRecord);
	}
	Request();

	BOOL bDeliver = (m_dwCurrentAudioRecord != dwRecordNr);
	// wenn während des vorwärts abspielens festegestellt wird, dass es ein gemischtes
	// archiv ist, wird auf prefetch umgeschaltet und möglicherweise samples doppelt
	// abgefragt.
	m_dwCurrentAudioRecord = dwRecordNr;

	if (m_iRequests>0)
	{
		m_iRequests--;
	}

	CIPCAudioDVClient*pMedia = theApp.GetLocalAudio();
	if (pMedia && pMedia->GetDefaultOutputID() != SECID_NO_ID)
	{
		CAutoCritSec cs(&m_csPictureRecord);
		
		WK_DELETE(m_pAudioRecord);
		m_pAudioRecord = new CIPCMediaSampleRecord(media);
		
		if (m_PlayStatus == PS_PLAY_FORWARD && bDeliver)
		{
			if (m_sAudioCompressionType.IsEmpty() || m_bReInitAudio)	// noch nicht initialisiert
			{
				if (m_pAudioRecord->IsLongHeader())
				{
					GetAudioCompressionType();
					m_bReInitAudio = false;
				}
				else
				{
					return 0;
				}
			}
			if (m_bSetSyncPoint)
			{
//				TRACE(_T("### RKE Test:Setting SyncPoint(%d:1), cd:%d\n"), m_pAudioRecord->GetIsSyncPoint(), m_pAudioRecord->ContainsData());
				pMedia->ResetSentSamples();
				pMedia->SetActiveDisplayWindow(this);
				m_pAudioRecord->SetContainsData(true);
				m_pAudioRecord->SetSyncPoint(true);
				m_bSetSyncPoint = false;
			}
			m_pAudioRecord->UpdateFlagsToBubble();
			pMedia->DoRequestMediaDecoding(*m_pAudioRecord);
			if (m_pPictureRecord == NULL)
			{
				PostMessage(WM_USER, ID_REDRAW_VIDEO);
			}
		}
		else if (m_pAudioRecord->IsLongHeader())
		{
			if (m_sAudioCompressionType.IsEmpty() || m_bReInitAudio)	// noch nicht initialisiert
			{
//				TRACE(_T("### RKE Test:InitAudio without data\n"));
				m_pAudioRecord->SetContainsData(false);
				m_pAudioRecord->UpdateFlagsToBubble();
				pMedia->DoRequestMediaDecoding(*m_pAudioRecord);
				GetAudioCompressionType();
				m_bReInitAudio = false;
			}
		}
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CPlayWindow::OnDraw(CDC* pDC)
{
	CDisplayWindow::OnDraw(pDC);

	if (m_bManipulated)
	{	
		CRect rect;
		CPoint p;
		CSize s;
		GetClientRect(rect);
		p = rect.CenterPoint();

		CFont font;
		font.CreatePointFont(10*10,GetFixFontFaceName());
		CFont* pOldFont = pDC->SelectObject(&font);
		CString t;
		t.LoadString(IDS_IMAGE_MANIPULATED);
		s = pDC->GetOutputTextExtent(t);

		rect.top = p.y - s.cy/2 - 5;
		rect.bottom = p.y + s.cy/2 + 5;
		rect.left = p.x - s.cx/2 - 5;
		rect.right = p.x + s.cx/2 + 5;
		CBrush b;
		b.CreateSolidBrush(RGB(255,0,0));
		pDC->FillRect(rect,&b);
		b.DeleteObject();


		COLORREF oldBk = pDC->SetBkColor(RGB(255,0,0));	 
		pDC->TextOut(rect.left+5,rect.top+5,t);
		pDC->SetBkColor(oldBk);
		pDC->SelectObject(pOldFont);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::IndicateNewSequence(const CIPCSequenceRecord& data,
									  WORD  wPrevSequenceNr,
									  DWORD dwNrOfRecords)
{
	m_Sequences.Lock(_T(__FUNCTION__));
	CIPCSequenceRecord* pSequence = m_Sequences.GetSequence(data.GetArchiveNr(),data.GetSequenceNr());
	if (pSequence)
	{
		*pSequence = data;
		WK_TRACE_ERROR(_T("Sequence already in Array %s\n"),GetName());
	}
	else
	{

		m_Sequences.Add(new CIPCSequenceRecord(data));
		m_Sequences.Sort();
	}
	if (wPrevSequenceNr>0 && dwNrOfRecords>0)
	{
		CIPCSequenceRecord* pPrev = m_Sequences.GetSequence(data.GetArchiveNr(),wPrevSequenceNr);
		if (pPrev)
		{
			pPrev->SetNrOfPictures(dwNrOfRecords);
		}
	}

/*
#ifdef _DEBUG
	TRACE(_T("%s new sequence %02x, %s\n"),GetName(),
			data.GetFlags(),data.GetTime().GetDateTime());
	
	for (int i=0;i<m_Sequences.GetSize();i++)
	{
		TRACE(_T("SEQUENCE %s , %s\n"),
				GetName(),
				m_Sequences.GetAtFast(i)->GetTime().GetDateTime());
	}
#endif
	*/

	CIPCSequenceRecord* pCurrentSequence = NULL;
	BOOL bPlayByBackup = FALSE;
	if (m_wCurrentSequence!=0)
	{
		pCurrentSequence = m_Sequences.GetSequence(m_ArchivRecord.GetID(),
												   m_wCurrentSequence);
	}
	if (pCurrentSequence)
	{
		if (!pCurrentSequence->IsBackup() && data.IsBackup())
		{
			bPlayByBackup = TRUE;
		}
	}
	else
	{
		bPlayByBackup = data.IsBackup();
	}
	
	if (bPlayByBackup)
	{
		WK_TRACE(_T("play sequence %s by backup %s\n"),data.GetName(),GetName());
	}


	if (m_Sequences.GetSize() == 1 || bPlayByBackup)
	{
		m_Sequences.Unlock();
		// was the first sequence
		m_wCurrentSequence = data.GetSequenceNr();
		m_dwCurrentRecord = 1;
		if (bPlayByBackup)
		{
			WK_TRACE(_T("%s new backup sequence PS_STOP\n"),GetName());
			m_bEjectAfterBackupRemoved = !IsWindowVisible();
			SetPlayStatus(PS_STOP);
			Navigate(m_wCurrentSequence,m_dwCurrentRecord);
		}
		else
		{
			if (m_PlayStatus != PS_EJECT)
			{
				Navigate(m_wCurrentSequence,m_dwCurrentRecord);
			}
			DoRedraw();
			UpdatePanel();
		}
	}
	else
	{
		m_Sequences.Unlock();
	}

	if(m_bCanRectSearch && !theApp.GetMainFrame()->IsInBackup())
	{
		if(m_pTracker && !m_pTracker->GetRect()->IsRectEmpty())
		{
			//set to TRUE to alow enable buttons in panel if buttons were disabled
			m_bIsNextForwardQueryRectResult = TRUE;
		}
	}

	DoRedraw();
	UpdatePanel();

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::IndicateDeleteSequence(WORD wSequenceNr)
{
	TRACE(_T("%s indicate delete sequence %d\n"),GetName(),wSequenceNr);
	int i;

	if (m_PlayStatus == PS_DELETING)
	{
		m_csSequencesToDelete.Lock();
		for (i=0;i<m_waSequencesToDelete.GetSize();i++)
		{
			if (m_waSequencesToDelete[i] == wSequenceNr)
			{
				m_waSequencesToDelete.RemoveAt(i);
				break;
			}
		}
		if (m_waSequencesToDelete.GetSize() == 0)
		{
			m_dwStartDeleted = GetTickCount();
			ChangePlayStatus(PS_DELETED);
		}
		m_csSequencesToDelete.Unlock();
	}

	m_Sequences.Lock(_T(__FUNCTION__));
	CIPCSequenceRecord* pSR = NULL;
	BOOL bWasBackup = FALSE;

	for (i=0;i<m_Sequences.GetSize();i++)
	{
		pSR = m_Sequences.GetAtFast(i);
		bWasBackup = pSR->IsBackup();
		if (pSR->GetSequenceNr() == wSequenceNr)
		{
			if (wSequenceNr == m_wCurrentSequence)
			{
				m_bSuspect = FALSE;
				DeletePicture(TRUE);

				if (   !bWasBackup 
					|| !m_bEjectAfterBackupRemoved)
				{
					CIPCSequenceRecord* pNew = GetNextSequence(m_wCurrentSequence);
					if (pNew == NULL)
					{
						pNew = GetPrevSequence(m_wCurrentSequence);
					}
					if (pNew)
					{
						m_wCurrentSequence = pNew->GetSequenceNr();
						m_dwCurrentRecord = 1;
					}
				}
			}
			m_Sequences.RemoveAt(i);
			WK_DELETE(pSR);
			break;
		}
	}
	m_Sequences.Sort();

	if (m_Sequences.GetSize() == 0)
	{
		m_Sequences.Unlock();
		m_wCurrentSequence = 0;
		m_dwCurrentRecord = 1;
		if (m_PlayStatus != PS_DELETED)
		{
			// ML 19.08.2004: PS_STOP führt an dieser Stelle zum Öffnen des Playwindows
			// ChangePlayStatus(PS_STOP);
			ChangePlayStatus(PS_EJECT);
		}
	}
	else
	{
		m_Sequences.Unlock();
		if (   bWasBackup 
			&& m_bEjectAfterBackupRemoved)
		{
			m_bEjectAfterBackupRemoved = FALSE;
			SetPlayStatus(PS_EJECT);
			CPanel* pPanel = theApp.GetPanel();
			if(pPanel)
			{
				//Damit nach entfernen der CD, die Archive enthält, die anderen Kameras bzw.
				//geöffneten Archive korrekt nachrücken (als Hauptfenster/aktives Fenster) hier
				//pPanel->SetCamera aufrufen. Dann ist der Ablauf identisch, als wenn der
				//Eject-Button im Panel geklickt wird
				if(IsActive())
				{
					pPanel->SetCamera(GetNr(), PlayLive);
				}
			}
		}
		else
		{
			if (   (m_PlayStatus != PS_DELETING)
				&& (m_PlayStatus != PS_DELETED))
			{
				Navigate(m_wCurrentSequence,m_dwCurrentRecord);
			}
		}
	}
		if(m_bCanRectSearch && !theApp.GetMainFrame()->IsInBackup())
		{
			if(m_pTracker && !m_pTracker->GetRect()->IsRectEmpty())
			{
				//set to TRUE to alow enable buttons in panel if buttons were disabled
				m_bIsNextForwardQueryRectResult = TRUE;
			}
		}

	UpdatePanel();

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CPlayWindow::InitialRequests()
{
	RequestSequenceList(theApp.IsReadOnly());
}
/////////////////////////////////////////////////////////////////////////////
void CPlayWindow::RequestSequenceList(BOOL bWait)
{
	// request initial sequence list
	if (m_pServer && m_pServer->IsDatabaseConnected())
	{
		TRACE(_T("requesting sequence list for %04x %s\n"),GetID().GetSubID(),GetName());
		m_pServer->GetDatabase()->DoRequestSequenceList(GetID().GetSubID());
		if (bWait)
		{
			m_evRequestSequenceList.ResetEvent();
			DWORD dwRet = WaitForSingleObject(m_evRequestSequenceList, 5000);
			if (dwRet == WAIT_TIMEOUT)
			{
				WK_TRACE_WARNING(_T("DoRequestSequenceList timeout. No confirm\n"));
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CPlayWindow::ChangePlayStatus(PlayStatus playstatus)
{

	//TRACE(_T("#### ChangePlayStatus from %s to %s  ID: %d\n"), 
	//	NameOfEnum(m_PlayStatus), NameOfEnum(playstatus), GetID());


	if (m_PlayStatus != playstatus)
	{
		m_PlayStatus = playstatus;

		if (playstatus == PS_STOP)
		{
			CIPCAudioDVClient* pMedia = theApp.GetLocalAudio();
			if (pMedia)
			{
				pMedia->DoRequestStopMediaDecoding();
				pMedia->SetActiveDisplayWindow(NULL);
			}
			theApp.SetTimeOutValue();
		}

		if(m_bCanRectSearch && !theApp.GetMainFrame()->IsInBackup())
		{
			if(    m_RectPlayStatus != PS_RECT_PLAY_FORWARD
				&& m_RectPlayStatus != PS_RECT_PLAY_BACK)
			{
				UpdatePanel();
			}

			if (playstatus == PS_STOP)
			{
				m_bStopRectPlay = FALSE;
			}

			//Nur hier ein Redraw durchführen, da sonst der Hilfetext im Alarm-Backup-Fenster
			//nicht aktualisiert wird
			if (WK_IS_WINDOW(this) && IsAlarm())
			{
				DoRedraw();
			}
		}
		else
		{
			UpdatePanel();

			if (WK_IS_WINDOW(this))
			{
				DoRedraw();
			}
		}

		theApp.ForceOnIdle();
		if(theApp.TKR_Trace())
		{
			TRACE(_T("new automatic %s %s\n"),GetName(),NameOfEnum(m_PlayStatus));
		}
		if (   (m_PlayStatus != PS_FAST_FORWARD)
			&& (m_PlayStatus != PS_FAST_BACK)
			)
		{
			m_iFFPicts = 0;
			m_iFFStepWidth = 2;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CPlayWindow::SetPlayStatus(CPlayWindow::PlayStatus playstatus)
{
	if (   (m_PlayStatus != playstatus)
		&& CanPlayStatus(playstatus)
		)
	{
		m_LastPlayStatusFromCmd = playstatus;
		if (playstatus == PS_PLAY_FORWARD)
		{
			m_bSetSyncPoint    = true;
			BOOL bCanDecode = FALSE;
			CIPCAudioDVClient*pAudio = theApp.GetLocalAudio();
			if (pAudio)
			{
				bCanDecode = pAudio->CanDecode();					
			}
			if (m_bContainsAudio && 							// archive with audio & video
				!bCanDecode)									// canot decode
			{
				m_bGetPicture   = TRUE;							// retrieve only pictures
			}
			else
			{
				m_bGetPicture   = FALSE;						// get also audio
				m_dwRequestedRecord = m_dwCurrentRecord;
				m_wRequestedSequence = m_wCurrentSequence;
			}
		}
		else
		{
			m_iRequests = 0;
			m_bGetPicture = m_bContainsAudio;
#ifdef BUFERED_PICTURE_IF_AUDIO
			if (m_bDoPrefetch)
			{
				m_PlayStatus = playstatus;
				m_bDoPrefetch = false;

				m_csPictureRecord.Lock();
				int nCount = m_PictBuffer.GetCount();
				if (nCount)
				{
					BOOL bNewer = FALSE;
					CIPCPictureRecord*pPict = (CIPCPictureRecord*) m_PictBuffer.RemoveTail();
					if (m_pPictureRecord)
					{
						CSystemTime stPB = pPict->GetTimeStamp();
						CSystemTime stPR = m_pPictureRecord->GetTimeStamp();
						bNewer = stPB > stPR;
					}

					if (bNewer && CDisplayWindow::PictureData(*pPict,0,0))
					{
						TRACE(_T("Video updated from buffer %d\n"), nCount);
						if (pPict->GetCompressionType() == COMPRESSION_MPEG4)
						{
							BOOL bDecode = DecodeMpeg(*pPict, 0, 0);
							if(bDecode)
							{
								if(theApp.TKR_Trace())
								{
									CString sFrame = _T("P-Frame");
									if(pPict->GetPictureType()==SPT_FULL_PICTURE)
									{
										sFrame = _T("I-Frame");
									}
									TRACE(_T("TKR ######### DECODE Video updated from buffer RecordNr (Picture): SequNr: %d  RecNr: %d   %s \n"), m_wCurrentSequence, m_dwCurrentRecord,sFrame);
								}
							}
							else
							{
								if(theApp.TKR_Trace())
								{
									TRACE(_T("TKR ######### DECODE Failed Video updated from buffer RecordNr (Picture): SequNr: %d  RecNr: %d\n"), m_wCurrentSequence, m_dwCurrentRecord);
								}
							}
						}
						else
						{
							PostMessage(WM_USER, ID_CHANGE_VIDEO);
						}
					}
					WK_DELETE(pPict);

					while (m_PictBuffer.GetCount())
					{
						pPict = (CIPCPictureRecord*) m_PictBuffer.RemoveHead();
						WK_DELETE(pPict);
					}
				}
				m_csPictureRecord.Unlock();
			}
#else
			m_bDoPrefetch = false;
#endif
		}
//		TRACE(_T("### RKE Test: Set %s, %s\n"), NameOfEnum(playstatus), m_bGetPicture ? _T("only pictures") : _T("get all"));
		if (playstatus == PS_STOP)
		{
			CIPCAudioDVClient* pAudio = theApp.GetLocalAudio();
			if (pAudio && pAudio->GetActiveDisplayWindow() == this)
			{
				pAudio->DoRequestStopMediaDecoding();
				pAudio->SetActiveDisplayWindow(NULL);
			}
		}


//TODO TKR für Sync Abspielen
/*
		if(	theApp.GetPanel()->CanSyncPlay())
		{
			if(playstatus != PS_STOP)
			{
				m_bCanMakeSyncBigOne = FALSE;
			}
		}
*/
		if ((playstatus == PS_STOP))
		{
			CPanel* pPanel = theApp.GetPanel();
			if (pPanel)
			{
				WORD wCam = (WORD)(GetID().GetSubID() & (MAX_CAM-1));
				pPanel->SetCamera(wCam, Play);
			}
		}

		if(m_bCanRectSearch && !theApp.GetMainFrame()->IsInBackup())
		{
			if(m_pTracker && !m_pTracker->GetRect()->IsRectEmpty())
			{
				//Das Abspielen des nächsten Bildes bei der Rechtecksuche immer wieder enablen,
				//wenn die Rechtecksuche gestoppt wird 
				if(playstatus == PS_STOP)
				{
					m_bPlayNextRect = TRUE;
					if(m_RectPlayStatus != PS_RECT_QUERY_RECT)
					{
						m_RectPlayStatus = PS_RECT_STOP;
					}
					m_bStopRectPlay = TRUE;
				}
				else
				{				
					m_LastPlayStatus = m_PlayStatus;
					m_PlayStatus = playstatus;
					TRACE(_T("SetPlayStatus(): playstatus: from %s -> to: %s  (01)ID: %d\n"),
						NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus), GetID());
				}
			}
			else
			{
				m_LastPlayStatus = m_PlayStatus;
				m_PlayStatus = playstatus;
				TRACE(_T("SetPlayStatus(): playstatus: from %s -> to: %s  (02)ID: %d\n"),
					NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus), GetID());
			}
		}
		else
		{
			m_LastPlayStatus = m_PlayStatus;
			m_PlayStatus = playstatus;
			TRACE(_T("SetPlayStatus(): playstatus: from %s -> to: %s  (03)ID: %d\n"),
				NameOfEnum(m_LastPlayStatus), NameOfEnum(m_PlayStatus), GetID());
		}

		//Wartezeit der Textanzeige _T("Kein Bild gefunden") hier wieder auf 0 setzten,
		//damit bei nächster ergebnisloser Bild-Suche die Anzeige wieder 10 Sekunden 
		//angezeigt wird. Wurde innerhalb dieser 10 Sekunden (nach z.B. 7 Sek.) 
		//das Bild gewechselt (Zurückbutton) und erneut ergebnislos gesucht, verblieb die Anzeige 
		//_T("Kein Bild gefudnen") nur für 3 Sek.
		if(m_LastPlayStatus == PS_QUERY_FAILED)
		{
			m_dwStartQueryFailed = 0;
		}
		
		//Hier ebendso für die Anzeige _T("Es wurden keine weiteren Bilder...") Variable auf 0 setzen
		if(m_LastPlayStatus == PS_NAVIGATE_FAILED)
		{
			m_dwNavigateFailedStart = 0;
		}

		if(theApp.TKR_Trace())
		{
			TRACE(_T("new manual %s %s\n"),GetName(),NameOfEnum(m_PlayStatus));
		}
		if (   (m_PlayStatus != PS_FAST_FORWARD)
			&& (m_PlayStatus != PS_FAST_BACK)
			)
		{
			m_iFFPicts = 0;
			m_iFFStepWidth = 2;
		}
		UpdatePanel();
	}
	else
	{
		TRACE(_T("cannot change %s to %s\n"),GetName(),NameOfEnum(playstatus));
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::CanPlayStatus(PlayStatus playstatus)
{
	BOOL bRet = FALSE;
	switch (playstatus)
	{
	case PS_EJECT:
		bRet = !IsDeleting();
		break;
	case PS_QUERY:
	case PS_QUERY_RECT:
		bRet = (m_Sequences.GetSize()>0) && !IsDeleting();
		break;
	case PS_QUERY_FAILED:
		bRet = (m_PlayStatus == PS_QUERY);
		break;
	case PS_STOP:
		bRet = !IsWindowVisible() || m_Sequences.GetSize()>0;
		break;
	case PS_PLAY_BACK:
	case PS_SINGLE_BACK:
		bRet = CanBack() && !IsDeleting();
		break;

	case PS_FAST_BACK:
		if(m_bCanRectSearch && !theApp.GetMainFrame()->IsInBackup())
		{
			if(m_pTracker && m_pTracker->GetRect()->IsRectEmpty())
			{
				bRet = CanBack() && !IsDeleting();		
			}
			else
			{
				//disable button FAST_BACK on Rectangle Search
				bRet = FALSE;
			}
		}
		else
		{
			bRet = CanBack() && !IsDeleting();
		}

		break;

	case PS_SKIP_BACK:
		if(m_bCanRectSearch && !theApp.GetMainFrame()->IsInBackup())
		{
			if(m_pTracker && m_pTracker->GetRect()->IsRectEmpty())
			{
				bRet = CanBack() && !IsDeleting();
				if (IsAlarm())
				{
					bRet = bRet && (0!=GetPrevTrackSequence());
				}
			}
			else
			{
				//disable button SKIP_BACK on Rectangle Search
				bRet = FALSE;
			}
		}
		else
		{
			bRet = CanBack() && !IsDeleting();
			if (IsAlarm())
			{
				bRet = bRet && (0!=GetPrevTrackSequence());
			}
		}

		break;
	case PS_SINGLE_FORWARD:
	case PS_PLAY_FORWARD:
		bRet = CanForward() && !IsDeleting();
		break;
		
	case PS_FAST_FORWARD:
		if(m_bCanRectSearch && !theApp.GetMainFrame()->IsInBackup())
		{
			if(m_pTracker && m_pTracker->GetRect()->IsRectEmpty())
			{
				bRet = CanForward() && !IsDeleting();
			}
			else
			{
				//disable button FAST_FORWARD on Rectangle Search
				bRet = FALSE;
			}
		}
		else
		{
			bRet = CanForward() && !IsDeleting();
		}
		break;

	case PS_SKIP_FORWARD:
		if(m_bCanRectSearch && !theApp.GetMainFrame()->IsInBackup())
		{
			if(m_pTracker && m_pTracker->GetRect()->IsRectEmpty())
			{
				bRet = CanForward() && !IsDeleting();
				if (IsAlarm())
				{
					bRet = bRet && (0!=GetNextTrackSequence());
				}
			}
			else
			{
				//disable button SKIP_FORWARD on Rectangle Search
				bRet = FALSE;
			}
		}
		else
		{
			bRet = CanForward() && !IsDeleting();
			if (IsAlarm())
			{
				bRet = bRet && (0!=GetNextTrackSequence());
			}
		}
		break;

	case PS_DELETE_ALARM:
		bRet = IsAlarm() && !m_bSuspect && (m_Sequences.GetSize()>0);
		break;
	case PS_SHOW_SUSPECT:
		bRet =    IsAlarm() 
			   && !m_bSuspect 
			   && !IsDeleting()
			   && CanSuspect();
		break;
	case PS_DELETE_SUSPECT:
		bRet =    IsAlarm() 
			   && m_bSuspect 
			   && (m_Sequences.GetSize()>0) 
			   && !IsDeleting();
		break;
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::CanChangeMode()
{
	return    !IsDeleting() 
		   && (m_PlayStatus != PS_QUERY);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::CanBack()
{
	BOOL bRet = FALSE;
	bRet =    (m_dwCurrentRecord>1) 
		   || (GetPrevSequence(m_wCurrentSequence)!=NULL)
		   || (!IsWindowVisible());

	//sind keine weiteren Bilder verfügbar, PlayStatus auf PS_STOP setzen
	//da sonst, wenn wieder Bilder verfügbar sind, z.B. wieder mit PS_PLAY_BACK
	//weitergemacht wird
	if(     !bRet
		&& (   (m_PlayStatus == PS_PLAY_BACK)
			|| (m_PlayStatus == PS_FAST_BACK)))
	{
		ChangePlayStatus(PS_STOP);
	}

	if(m_bCanRectSearch && !theApp.GetMainFrame()->IsInBackup())
	{
		if(m_pTracker && !m_pTracker->GetRect()->IsRectEmpty())
		{
			return bRet && !IsDeleting() && m_bIsNextBackQueryRectResult;
		}
		else
		{
			return bRet && !IsDeleting();
		}
	}
	else
	{	

		return bRet && !IsDeleting();
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::CanForward()
{
	if (IsWindowVisible())
	{
		BOOL bRet = FALSE;
		m_Sequences.Lock(_T(__FUNCTION__));
		CIPCSequenceRecord* pSR = m_Sequences.GetSequence(m_ArchivRecord.GetID(), m_wCurrentSequence);
		if (pSR)
		{
			bRet =    (m_dwCurrentRecord<pSR->GetNrOfPictures()) 
				   || (GetNextSequence(m_wCurrentSequence)!=NULL);
		}
		m_Sequences.Unlock();

		//sind keine weiteren Bilder verfügbar, PlayStatus auf PS_STOP setzen
		//da sonst, wenn wieder Bilder verfügbar sind, z.B. wieder mit PS_PLAY_FORWARD
		//weitergemacht wird
		if(     !bRet
			&& (   (m_PlayStatus == PS_PLAY_FORWARD)
			    || (m_PlayStatus == PS_FAST_FORWARD)))
		{
			ChangePlayStatus(PS_STOP);
		}

		if(m_bCanRectSearch && !theApp.GetMainFrame()->IsInBackup())
		{
			if(m_pTracker && !m_pTracker->GetRect()->IsRectEmpty())
			{
				return bRet && !IsDeleting() && m_bIsNextForwardQueryRectResult;
			}
			else
			{
				return bRet && !IsDeleting();
			}
		}
		else
		{

			return bRet && !IsDeleting();

		}
	}
	else
	{
		return TRUE;
	}
}
//////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::CanNavigate()
{
	BOOL bDeliver = m_bDoPrefetch;
	bDeliver = (m_iRequests < theApp.m_nMaxRequestedRecords);

//aus Idip TODO TKR Abspielen gemischter Archive
/*
	if(    m_PlayStatus == PS_PLAY_BACK
		|| m_PlayStatus == PS_FAST_BACK
		|| m_PlayStatus == PS_FAST_FORWARD)
	{
		if(m_pMpeg)
		{
			//playback && fastforward no prefetch
			bDeliver =  (m_iRequests < 1) 
						&& (m_pMpeg->GetDecoderQueueLength()<5);
		}
	}
//ende
*/

	if (!CanSendAudioSamples())
	{
		bDeliver = FALSE;
	}
	
	if (   m_pMpeg
		&& m_pMpeg->GetDecoderQueueLength() > 5)
	{
		TRACE(_T("##### MPeg4 Decoder QueueLength: %d\n"),m_pMpeg->GetDecoderQueueLength());
		bDeliver = FALSE;
	}

	return bDeliver;




}
//////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::CanSendAudioSamples()
{
	if (m_pAudioRecord)
	{
		CIPCAudioDVClient* pMedia = theApp.GetLocalAudio();
		if (pMedia && pMedia->DoNotSendSamples())
		{
			return FALSE;
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::CanSuspect()
{
	BOOL bRet = FALSE;
	m_Sequences.Lock(_T(__FUNCTION__));
	for (int i=0;i<m_Sequences.GetSize();i++)
	{
		if (m_Sequences.GetAtFast(i)->IsSuspect())
		{
			bRet = TRUE;
			break;
		}
	}
	m_Sequences.Unlock();
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::Request(int iMax/*=-1*/)
{
	iMax = 0;
	BOOL bRet = FALSE;
	
	switch (m_PlayStatus)
	{
	case PS_EJECT:
		bRet = OnEject();
		break;
	case PS_STOP:
		bRet = OnStop();
		break;
	case PS_PLAY_BACK:
		bRet = OnPlayBack();
		break;
	case PS_PLAY_FORWARD:
		bRet = OnPlayForward();
		break;
	case PS_SINGLE_BACK:
		bRet = OnSingleBack();
		break;
	case PS_SINGLE_FORWARD:
		bRet = OnSingleForward();
		break;
	case PS_SKIP_BACK:
		bRet = OnSkipBack();
		break;
	case PS_SKIP_FORWARD:
		bRet = OnSkipForward();
		break;
	case PS_FAST_BACK:
        bRet = OnFastBack();
		break;
	case PS_FAST_FORWARD:
		bRet = OnFastForward();
		break;
	case PS_DELETE_ALARM:
		OnDeleteCurrentAlarm();
		break;
	case PS_DELETING:
		OnDeleting();
		break;
	case PS_DELETED:
		OnDeleted();
		break;
	case PS_ACCESS_DENIED:
		bRet = OnAccessDenied();
		break;
	case PS_SHOW_SUSPECT:
		bRet = OnShowSuspect();
		break;
	case PS_DELETE_SUSPECT:
		bRet = OnDeleteSuspect();
		break;
	case PS_NAVIGATE_FAILED:
		bRet = OnNavigateFailed();
		break;
	case PS_READ_FAILED:
		bRet = OnReadFailed();
		break;
	case PS_QUERY_FAILED:
		bRet = OnQueryFailed();
		break;
	case PS_QUERY:
		bRet = OnQuery();
		break;
	case PS_QUERY_RECT:
		bRet = OnQueryRect();
		break;
	default:
		TRACE(_T("play status not handled in Request %s\n"),NameOfEnum(m_PlayStatus));
		break;
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::Activate()
{
//	TRACE(_T("CPlayWindow::Activate()\n"));
	if (!m_pMainFrame)
		return FALSE;

	CPanel *pPanel = theApp.GetPanel();
	if (!pPanel)
		 return FALSE;

	BOOL	bRet = FALSE;
	int		nViewMode	= m_pMainFrame->GetViewMode();
	int		nActiveCam	= pPanel->GetActiveCamera();
	int		nCurrentCam	= GetID().GetSubID() & 0xff;

	if ((nViewMode == View1pX) ||
		(nViewMode == ViewXxX) ||
		((nViewMode == ViewFull) && (nCurrentCam == nActiveCam)))
	{
		if (   !IsWindowVisible()
			&& (   (m_LastPlayStatus == PS_EJECT)
			    || (m_LastPlayStatus == PS_QUERY)
				)
			)
		{
			m_LastPlayStatus = m_PlayStatus;
			ShowWindow(SW_SHOW);
			m_pMainFrame->SetDisplayActive(this);
			m_pMainFrame->DoAutoViewMode();
			
			//start playrealtime thread on opening play window
			//playrealtime thread will be stopped again on playstatus == PS_EJECT;

//TODO TKR den ThreadStart einbauen, sobald der Absturz des Processes 0x111318F5 geklärt ist
/*
			CPlayRealTime* pPlayRealTime = GetPlayRealTime();
			if(pPlayRealTime)
			{
				pPlayRealTime->StartThread();
				TRACE(_T("#####  START THREAD playrealtime  #####\n"));
			}
*/
		}
		bRet = TRUE;
	}
	else
	{
		bRet = FALSE;															  
	}
	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::OnEject()
{
	//Falls das Window nicht mehr sichtbar sein sollte (wird im Panel in den SetWindowSize_xxx 
	//Funktionen versteckt (SW_HIDE)) - kommt vor wenn OnEject zu spät erreicht wird, da 
	//manchmal falsches timing) - hier noch prüfen, ob vorheriger Zustand PS_EJECT war.
	if (IsWindowVisible() || m_LastPlayStatus != PS_EJECT)
	{
		m_bIsActive = FALSE;
		m_iRequests = 0;
		m_wCurrentSequence = 0;
		m_dwCurrentRecord = 1;
		ShowWindow(SW_HIDE);
		DeletePicture(FALSE);
		m_bSuspect = FALSE;
		m_pMainFrame->SetMustAutoViewMode();
		m_LastPlayStatus = m_PlayStatus;
		
		//falls gezoomt, zurücksetzten, damit bei nächsten Öffnen nicht
		//wieder gezoomt dargestellt wird
		m_rcZoom = CRect(0,0,0,0); 

		if(m_bCanRectSearch)
		{
			SetRectPlayStatus(PS_RECT_NO_RECT);
			if(theApp.TKR_Trace())
			{
				TRACE(_T("TKR ---- RectPlayStatus rset auf PS_RECT_NO_RECT\n"));
			}
			ResetTracker();
			ResetRectMembersInPlayWnd();
		}

//TODO TKR den ThreadStart einbauen, sobald der Absturz des Processes 0x111318F5 geklärt ist
/*		CPlayRealTime* pPlayRealTime = GetPlayRealTime();
		if(pPlayRealTime)
		{
			pPlayRealTime->StopThread();
			WK_TRACE(_T("#####  STOP THREAD playrealtime  #####\n"));
		}
*/
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::OnReadFailed()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::OnNavigateFailed()
{
	if (m_dwNavigateFailedStart == 0)
	{
		m_dwNavigateFailedStart = GetTickCount();
		DeletePicture(TRUE);
	}
	else
	{
		if (GetTimeSpan(m_dwNavigateFailedStart)>5*1000)
		{
			m_dwNavigateFailedStart = 0;
			Navigate(m_wCurrentSequence,m_dwCurrentRecord);
			ChangePlayStatus(PS_STOP);
		}
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::OnAccessDenied()
{
	TRACE(_T("OnAccessDenied()\n"));
	BOOL bRet = FALSE;
	ChangePlayStatus(m_LastPlayStatus);
	if (m_wLastSequenceRequest!=0)
	{
		bRet = RequestRecord(m_wLastSequenceRequest,m_dwLastRecordRequest,
							 m_dwLastNewRecordRequest,m_dwLastCamRequest);
	}
	if (!bRet)
	{
		//ChangePlayStatus(PS_ACCESS_DENIED);
		ChangePlayStatus(PS_STOP);
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::OnStop()
{
	//if realtime play was active, stop it
	if(	   theApp.CanRealtime()
		&& GetPlayRealTime()
		&& m_pPlayRealTime->IsRealTimePictureDateTime())
	{
		TRACE(_T("STOP playing realtime\n"));
		m_pPlayRealTime->SetRealTimePictureDateTime(FALSE);
	}

	if (!IsWindowVisible() || theApp.IsReadOnly())
	{
		if (m_pPictureRecord == NULL)
		{
			if (Activate())
			{
				if (m_pAudioRecord)
				{
					m_bGetPicture = TRUE;
					BOOL bRet = Navigate(m_wCurrentSequence,m_dwCurrentRecord,1);
					m_bGetPicture = FALSE;
					return bRet;
				}
				return Navigate(m_wCurrentSequence,m_dwCurrentRecord);
			}
		}
	}
	else if (IsWindowVisible())
	{
		if (m_pPictureRecord == NULL)
		{
			if(m_bCanRectSearch && !theApp.GetMainFrame()->IsInBackup())
			{
				WORD wCam = GetID().GetSubID();
				CPlayWindow* pPlayWnd;
				pPlayWnd = theApp.GetMainFrame()->GetPlayWindow(wCam);
				if(pPlayWnd && pPlayWnd->IsWindowVisible())
				{
					if(pPlayWnd->GetTracker() && !pPlayWnd->GetTracker()->GetRect()->IsRectEmpty())
					{
						CWnd* pWnd = GetFocus();
						pPlayWnd->SetFocus();
						pPlayWnd->GetTracker()->Draw(GetDC());
						if(theApp.TKR_Trace())
						{
							TRACE(_T("TKR############# OnStop() Cam: %i\n"), wCam);
						}
						pPlayWnd->Invalidate();
						pPlayWnd->UpdateWindow();
						if(pWnd)
						{
							pWnd->SetFocus();
							Invalidate();
						}
					}
				}
			}

			if (m_bContainsAudio)
			{
				int nRecCount = 0;
				switch (m_LastPlayStatusFromCmd)
				{
					case PS_SKIP_BACK:
					case PS_SINGLE_BACK:
						nRecCount = -1;
						break;
					case PS_SKIP_FORWARD:
					case PS_SINGLE_FORWARD:
					case PS_PLAY_FORWARD:
					case PS_FAST_FORWARD:
						nRecCount = -1;
						break;
				}
				if (nRecCount)
				{
					m_iRequests = 0;
				}
				TRACE(_T("### RKE Test:Navigate(Sequ:%d, Rec:%d, Count:%d LastPS:%s)\n"), 
					m_wCurrentSequence, m_dwCurrentRecord, nRecCount, NameOfEnum(m_LastPlayStatusFromCmd));
				return Navigate(m_wCurrentSequence, m_dwCurrentRecord, nRecCount);
			}

			if(m_LastPlayStatus != PS_QUERY)
			{
					//TRACE(_T("### TKR Test:Navigate(Sequ:%d, Rec:%d, LastPS:%s)\n"), 
					//	m_wCurrentSequence, m_dwCurrentRecord, NameOfEnum(m_LastPlayStatus));
					return Navigate(m_wCurrentSequence,m_dwCurrentRecord);
			}
		}
		else
		{
//			TRACE(_T("******** OnStop() visible with m_pPictureRecord\n"));
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::OnPlayBack()
{
//TODO TKR hier synchrones Abspielen
/*

	BOOL bSync = theApp.GetPanel()->CanSyncPlay();
	if(bSync)
	{
		BOOL bForward = FALSE;
		WORD wPlayingKam = 0;
		CSystemTime st;

		CDisplayWindow* pDW = theApp.GetMainFrame()->FindOldestDisplayWindow(st, bForward, wPlayingKam);

		WORD wOldestCameraNr = pDW->GetID().GetSubID();
		WORD wBigOne = theApp.GetMainFrame()->GetBigOne()->GetID().GetSubID();
		WORD wCurrentCamNr = GetID().GetSubID();
		TRACE(_T("--- OnPlayBack() curr Kamera: %d  oldest Kamera: %d  bigone: %d \n"), 
			wCurrentCamNr &= (MAX_CAM-1), 
			wOldestCameraNr &= (MAX_CAM-1),
			wBigOne & (MAX_CAM-1));
		if(wCurrentCamNr != wOldestCameraNr)
		{
			//spiele andere als aktuelle Kamera ab, diese hier stoppen
			TRACE(_T("++ stop current Cam: %d\n"), wCurrentCamNr &= (MAX_CAM-1));
			ChangePlayStatus(PS_STOP);

			//spiele nun andere Kamera mit ältestem Bild ab
			((CPlayWindow*)pDW)->SetPlayStatus(PS_PLAY_BACK);
			TRACE(_T("### OnPlayBack() SetPlayStatus old. Kam. %d\n"),  wOldestCameraNr &= (MAX_CAM-1));
			return Navigate(m_wCurrentSequence,m_dwCurrentRecord,0);
		}
		else
		{
			if(GetPlayStatus() != PS_PLAY_BACK)
			{
				ChangePlayStatus(PS_PLAY_BACK);
			}
			TRACE(_T("++ curr = oldest -> abspielen ps = %d \n"), ((CPlayWindow*)pDW)->GetPlayStatus());
		}
	}
*/

	if (Activate())
	{
		if(    m_bCanRectSearch 
		    && !theApp.GetMainFrame()->IsInBackup())
		{
			CQueryRectTracker* pQRT = GetTracker();
			if(pQRT && !pQRT->GetRectPromilleImage()->IsRectEmpty())
			{	
				if(m_bPlayNextRect && m_iRequests == 0)
				{
					if(!m_bStopRectPlay)
					{
						//start rectangle search for next picture
						CSystemTime stOneMilli;
						stOneMilli.wMilliseconds = 1;
						CSystemTime st	   = GetTimeStamp(); //get time from current image
						st = st - stOneMilli;				 //subtract one milli second
						m_RectPlayStatus = PS_RECT_PLAY_BACK;
						m_bPlayNextRect = FALSE; //is set to TRUE when new picture is on screen
						return StartDatabaseSearch(st);
					}
					else
					{
						m_RectPlayStatus = PS_RECT_STOP;
						ChangePlayStatus(PS_STOP);
						return Navigate(m_wCurrentSequence,m_dwCurrentRecord,0);
					}
				}
			}
			else
			{
				return Navigate(m_wCurrentSequence,m_dwCurrentRecord,-1);
			}
		}
		else
		{
			return Navigate(m_wCurrentSequence,m_dwCurrentRecord,-1);
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::OnPlayForward()
{
//TODO TKR hier synchrones Abspielen
/*
	BOOL bSync = theApp.GetPanel()->CanSyncPlay();
	if(bSync)
	{
		BOOL bForward = TRUE;
		WORD wPlayingKam = 0;
		CSystemTime st;

		CDisplayWindow* pDW = theApp.GetMainFrame()->FindOldestDisplayWindow(st, bForward, wPlayingKam);

		WORD wOldestCameraNr = pDW->GetID().GetSubID();
		WORD wCurrentCamNr = GetID().GetSubID();
		TRACE(_T("--- OnPlayForward() curr Kamera: %d  oldest Kamera: %d\n"), wCurrentCamNr &= (MAX_CAM-1), wOldestCameraNr &= 0x0F);
		if(wCurrentCamNr != wOldestCameraNr)
		{
			//spiele andere als aktuelle Kamera ab, diese hier stoppen
			TRACE(_T("++ stop current Cam: %d\n"), wCurrentCamNr &= (MAX_CAM-1));
			ChangePlayStatus(PS_STOP);

			//spiele nun andere Kamera mit ältestem Bild ab
			((CPlayWindow*)pDW)->SetPlayStatus(PS_PLAY_FORWARD);
			TRACE(_T("### OnPlayForward() SetPlayStatus old. Kam. %d\n"),  wOldestCameraNr &= (MAX_CAM-1));
			return Navigate(m_wCurrentSequence,m_dwCurrentRecord,0);
		}
		else
		{
			TRACE(_T("++ curr = oldest -> abspielen Kam: %d\n"), wCurrentCamNr & 0x0F);
		}
	}
*/

	if (Activate())
	{
		if(    m_bCanRectSearch 
			&& !theApp.GetMainFrame()->IsInBackup())
		{
			CQueryRectTracker* pQRT = GetTracker();
			if(pQRT && !pQRT->GetRectPromilleImage()->IsRectEmpty())
			{
				if(m_bPlayNextRect && m_iRequests == 0) 
				{
					if(!m_bStopRectPlay)
					{
						//start rectangle search for next picture
						CSystemTime stOneMilli;
						stOneMilli.wMilliseconds = 1;
						CSystemTime st	   = GetTimeStamp(); //get time from current image
						st = st + stOneMilli;				 //add one milli second
						m_RectPlayStatus = PS_RECT_PLAY_FORWARD;
						m_bPlayNextRect = FALSE; //is set to TRUE when new picture is on screen
						return StartDatabaseSearch(st);
					}
					else
					{
						m_RectPlayStatus = PS_RECT_STOP;
						ChangePlayStatus(PS_STOP);
						return Navigate(m_wCurrentSequence,m_dwCurrentRecord,0);
					}
				}
			}
			else if (m_bContainsAudio && !m_bGetPicture)
			{
				if (CanNavigate() && CanForward())
				{							// mixed archives are prefetched to get the audio records
					m_bDoPrefetch = true;	// early enough

					//set the Record to search to the current record, also sequence
					m_dwRequestedRecord = m_dwCurrentRecord; 
					m_wRequestedSequence = m_wCurrentSequence;
					
					Navigate(m_wRequestedSequence, m_dwRequestedRecord++, 1);
#ifndef BUFERED_PICTURE_IF_AUDIO
					m_bDoPrefetch = false;
#endif
					if(m_pAudioRecord)
					{ 
						//only direct request (without updating window after each media sample)
						//when we have a media sample. 
						//If requestung also on picture records, update window (in OnChangeVideo)
						//is too slow to update the window before the next picture record arives
						theApp.GetMainFrame()->DoRequest();
					}

				}
				return TRUE;
			}
			else
			{
				return Navigate(m_wCurrentSequence,m_dwCurrentRecord,1);
			}
		}
		else
		{
			return Navigate(m_wCurrentSequence,m_dwCurrentRecord,1);
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::OnFastBack()
{
	BOOL bRet = FALSE;

//TODO TKR hier synchrones Abspielen
/*
	BOOL bSync = theApp.GetPanel()->CanSyncPlay();
	if(bSync)
	{
		BOOL bForward = FALSE;
		WORD wPlayingKam = 0;
		CSystemTime st;

		CDisplayWindow* pDW = theApp.GetMainFrame()->FindOldestDisplayWindow(st, bForward, wPlayingKam);

		WORD wOldestCameraNr = pDW->GetID().GetSubID();
		WORD wCurrentCamNr = GetID().GetSubID();
		TRACE(_T("--- OnFastBack() curr Kamera: %d  oldest Kamera: %d\n"), wCurrentCamNr &= (MAX_CAM-1), wOldestCameraNr &= 0x0F);
		if(wCurrentCamNr != wOldestCameraNr)
		{
			//spiele andere als aktuelle Kamera ab, diese hier stoppen
			TRACE(_T("++ stop current Cam: %d\n"), wCurrentCamNr &= (MAX_CAM-1));
			ChangePlayStatus(PS_STOP);

			//spiele nun andere Kamera mit ältestem Bild ab
			((CPlayWindow*)pDW)->SetPlayStatus(PS_FAST_BACK);
			TRACE(_T("### OnFastBack() SetPlayStatus old. Kam. %d\n"),  wOldestCameraNr &= (MAX_CAM-1));
			return Navigate(m_wCurrentSequence,m_dwCurrentRecord,0);
		}
		else
		{
			TRACE(_T("++ curr = oldest -> abspielen\n"));
		}
	}
*/

	if (Activate())
	{
		bRet = Navigate(m_wCurrentSequence,m_dwCurrentRecord,-m_iFFStepWidth);
//		TRACE(_T("NAVI back %d\n"),m_iFFStepWidth);
/*
		if (IsMD() || m_bCanMDTrack)
		{
			bRet = Navigate(m_wCurrentSequence,m_dwCurrentRecord,
							-1,
							(DWORD)(-1-NAVIGATION_FULL_HOUR));
		}
		else
		{
			CIPCSequenceRecord* pSR = GetPrevSequence(m_wCurrentSequence);
			if (pSR)
			{
				bRet = Navigate(pSR->GetSequenceNr(),1);
			}
			else
			{
				ChangePlayStatus(PS_NAVIGATE_FAILED);
			}
		}
*/
	}
	m_LastPlayStatus = m_PlayStatus;
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::OnFastForward()
{
	BOOL bRet = FALSE;

//TODO TKR hier synchrones Abspielen
/*
	BOOL bSync = theApp.GetPanel()->CanSyncPlay();
	if(bSync)
	{
		BOOL bForward = TRUE;
		WORD wPlayingKam = 0;
		CSystemTime st;

		CDisplayWindow* pDW = theApp.GetMainFrame()->FindOldestDisplayWindow(st, bForward, wPlayingKam);

		WORD wOldestCameraNr = pDW->GetID().GetSubID();
		WORD wCurrentCamNr = GetID().GetSubID();
		TRACE(_T("--- OnPlayForward() curr Kamera: %d  oldest Kamera: %d\n"), wCurrentCamNr &= (MAX_CAM-1), wOldestCameraNr &= 0x0F);
		if(wCurrentCamNr != wOldestCameraNr)
		{
			//spiele andere als aktuelle Kamera ab, diese hier stoppen
			TRACE(_T("++ stop current Cam: %d\n"), wCurrentCamNr &= (MAX_CAM-1));
			ChangePlayStatus(PS_STOP);

			//spiele nun andere Kamera mit ältestem Bild ab
			((CPlayWindow*)pDW)->SetPlayStatus(PS_FAST_FORWARD);
			TRACE(_T("### OnPlayForward() SetPlayStatus old. Kam. %d\n"),  wOldestCameraNr &= (MAX_CAM-1));
			return Navigate(m_wCurrentSequence,m_dwCurrentRecord,0);
		}
		else
		{
			TRACE(_T("++ curr = oldest -> abspielen\n"));
		}
	}
*/
	
	if (Activate())
	{
		bRet = Navigate(m_wCurrentSequence,m_dwCurrentRecord,m_iFFStepWidth);
//		TRACE(_T("NAVI FF %d\n"),m_iFFStepWidth);
		// alter Modus der vollen Stundenbilder
/*
		if (IsMD() || m_bCanMDTrack)
		{
			bRet = Navigate(m_wCurrentSequence,m_dwCurrentRecord,
							1,
							(DWORD)(-1-NAVIGATION_FULL_HOUR));
		}
		else
		{
			CIPCSequenceRecord* pSR = GetNextSequence(m_wCurrentSequence);
			if (pSR)
			{
				bRet = Navigate(pSR->GetSequenceNr(),1);
			}
			else
			{
				ChangePlayStatus(PS_NAVIGATE_FAILED);
			}
		}
		*/
	}
	m_LastPlayStatus = m_PlayStatus;
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::OnSingleBack()
{
	BOOL bRet = FALSE;

	if (Activate())
	{
		if(m_bCanRectSearch && !theApp.GetMainFrame()->IsInBackup())
		{
			CQueryRectTracker* pQRT = GetTracker();
			if(pQRT && !pQRT->GetRectPromilleImage()->IsRectEmpty())
			{	
				CSystemTime stOneMilli;
				stOneMilli.wMilliseconds = 1;
				CSystemTime st	   = GetTimeStamp(); //get time from current image
				st = st - stOneMilli;				 //subtract one milli second
				m_RectPlayStatus = PS_RECT_SINGLE_BACK;
				bRet = StartDatabaseSearch(st);
			}
			else
			{
				bRet = Navigate(m_wCurrentSequence,m_dwCurrentRecord,-1);
				ChangePlayStatus(PS_STOP);
			}
		}
		else
		{
			bRet = Navigate(m_wCurrentSequence,m_dwCurrentRecord,-1);
			ChangePlayStatus(PS_STOP);
		}
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::OnSingleForward()
{
	BOOL bRet = FALSE;
	
	if (Activate())
	{
		if(m_bCanRectSearch && !theApp.GetMainFrame()->IsInBackup())
		{
			CQueryRectTracker* pQRT = GetTracker();

			if(pQRT && pQRT->GetRectPromilleImage()->IsRectEmpty())
				
			{
				TRACE(_T("***** CPlayWindow::OnSingleForward()\n"));
				bRet = Navigate(m_wCurrentSequence,m_dwCurrentRecord,1);
				ChangePlayStatus(PS_STOP);
			}
			else
			{
				CSystemTime stOneMilli;
				stOneMilli.wMilliseconds = 1;
				CSystemTime st	   = GetTimeStamp(); //get time from current image
				st = st + stOneMilli;				 //add one milli second
				m_RectPlayStatus = PS_RECT_SINGLE_FORWARD;
				bRet = StartDatabaseSearch(st);
			}
		}
		else
		{
			bRet = Navigate(m_wCurrentSequence,m_dwCurrentRecord,1);
			ChangePlayStatus(PS_STOP);
		}
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::OnSkipBack()
{
	BOOL bRet = FALSE;
	if (Activate())
	{
		if (IsMD() || m_bCanMDTrack)
		{
			bRet = Navigate(m_wCurrentSequence,m_dwCurrentRecord,
							-1,(DWORD)(-1-NAVIGATION_TRACK));
		}
		else
		{
			WORD wSequenceNr = GetPrevTrackSequence();
			if (wSequenceNr != 0)
			{
				bRet = Navigate(wSequenceNr,1);
			}
		}
	}
	m_LastPlayStatus = m_PlayStatus;
	ChangePlayStatus(PS_STOP);
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
WORD CPlayWindow::GetNextTrackSequence()
{
	WORD wReturnSequenceNr = 0;
	// calc next alarm/verdacht
	CIPCSequenceRecord* pSRNext = NULL;

	CAutoCritSec acs(&m_Sequences.m_cs);
	CIPCSequenceRecord* pSR = m_Sequences.GetSequence(GetID().GetSubID(),m_wCurrentSequence);
	WORD wSequenceNr = m_wCurrentSequence;

	if (pSR)
	{
		if (pSR->IsSuspect())
		{
			// zum nächsten Verdachtsbild springen
			while (NULL!=(pSRNext = GetNextSequence(wSequenceNr)))
			{
				wSequenceNr = pSRNext->GetSequenceNr();
				if (pSRNext->IsSuspect())
				{
					wReturnSequenceNr = wSequenceNr;
					break;
				}
			}
		}
		else if (pSR->IsSafeRing())
		{
			// dem Vorring nachfolgenden Alarm suchen
			while (NULL!=(pSRNext = GetNextSequence(wSequenceNr)))
			{
				wSequenceNr = pSRNext->GetSequenceNr();
				if (pSRNext->IsAlarm())
				{
					break;
				}
			}
			// diesen Alarm und Verdachtsbilder 
			// überlesen und nach nächstem Vorring suchen.
			while (NULL!=(pSRNext = GetNextSequence(wSequenceNr)))
			{
				wSequenceNr = pSRNext->GetSequenceNr();
				if (pSRNext->IsSafeRing())
				{
					wReturnSequenceNr = wSequenceNr;
					break;
				}
			}
		}
		else if (pSR->IsAlarm())
		{
			// zum nächsten Vorring
			while (NULL!=(pSRNext = GetNextSequence(wSequenceNr)))
			{
				wSequenceNr = pSRNext->GetSequenceNr();
				if (pSRNext->IsSafeRing())
				{
					wReturnSequenceNr = wSequenceNr;
					break;
				}
			}
		}
	}
	return wReturnSequenceNr;
}
/////////////////////////////////////////////////////////////////////////////
WORD CPlayWindow::GetPrevTrackSequence()
{
	WORD wReturnSequenceNr = 0;
	// calc prev alarm/verdacht
	CIPCSequenceRecord* pSRPrev = NULL;
	CAutoCritSec acs(&m_Sequences.m_cs);
	CIPCSequenceRecord* pSR = m_Sequences.GetSequence(GetID().GetSubID(),m_wCurrentSequence);
	WORD wSequenceNr = m_wCurrentSequence;

	if (pSR)
	{
		if (pSR->IsSuspect())
		{
			// zum vorigen Verdachtsbild springen
			while (NULL!=(pSRPrev = GetPrevSequence(wSequenceNr)))
			{
				wSequenceNr = pSRPrev->GetSequenceNr();
				if (pSRPrev->IsSuspect())
				{
					wReturnSequenceNr = wSequenceNr;
					break;
				}
			}
		}
		if (pSR->IsAlarm())
		{
			// zum nächsten Vorring
			while (NULL!=(pSRPrev = GetPrevSequence(wSequenceNr)))
			{
				wSequenceNr = pSRPrev->GetSequenceNr();
				if (pSRPrev->IsSafeRing())
				{
					break;
				}
			}
			pSR = pSRPrev;
			// weiter Behandlung im nächsten if!
		}
		if (pSR && pSR->IsSafeRing())
		{
			// dem Vorring vorigen Alarm suchen
			while (NULL!=(pSRPrev = GetPrevSequence(wSequenceNr)))
			{
				wSequenceNr = pSRPrev->GetSequenceNr();
				if (pSRPrev->IsAlarm())
				{
					break;
				}
			}
			// diesen Alarm überlesen und nach nächstem
			// vorring suchen.
			while (NULL!=(pSRPrev = GetPrevSequence(wSequenceNr)))
			{
				wSequenceNr = pSRPrev->GetSequenceNr();
				if (pSRPrev->IsSafeRing())
				{
					break;
				}
			}
			// von diesem Vorring den Anfang suchen.
			// erst mal bis zum Alarm davor, oder bis zum 
			// gesamten Anfang
			while (NULL!=(pSRPrev = GetPrevSequence(wSequenceNr)))
			{
				if (pSRPrev->IsAlarm())
				{
					break;
				}
				wSequenceNr = pSRPrev->GetSequenceNr();
			}
			// und dann einfach wieder nach vorne bis zum
			// entsprechenden Vorring
			pSR = m_Sequences.GetSequence(GetID().GetSubID(),wSequenceNr);
			if (pSR && pSR->IsSafeRing())
			{
				// gefunden
				wReturnSequenceNr = wSequenceNr;
			}
			else
			{
				CIPCSequenceRecord* pSRNext;
				while (NULL!=(pSRNext = GetNextSequence(wSequenceNr)))
				{
					wSequenceNr = pSRNext->GetSequenceNr();
					if (pSRNext->IsSafeRing())
					{
						// gefunden
						wReturnSequenceNr = wSequenceNr;
						break;
					}
				}
			}
		}
	}
	if (wReturnSequenceNr == m_wCurrentSequence)
	{
		//nur wenn es keine vorherige Sequence gab, die aktuelle Sequence 
		//(ist dann ja die allererste) benutzen.
		//Unterscheidung wurde hier nötig, da ansonsten der Skip-Back-Button in Alarmarchiven
		//erst dann enabled wurde, wenn es eine vorherige Sequence gibt. Es soll aber auch innerhalb
		//der allerersten Sequence zum Beginn des Voralarms gesprungen werden können
		if(pSRPrev != NULL)
		{
			wReturnSequenceNr = 0;
		}
	}
	return wReturnSequenceNr;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::OnSkipForward()
{
	BOOL bRet = FALSE;
	if (Activate())
	{
		if (IsMD() || m_bCanMDTrack)
		{
			bRet = Navigate(m_wCurrentSequence,m_dwCurrentRecord,
							1,(DWORD)(-1-NAVIGATION_TRACK));
		}
		else
		{
			WORD wSequenceNr = GetNextTrackSequence();
			if (wSequenceNr != 0)
			{
				bRet = Navigate(wSequenceNr,1);
			}
		}
	}
	m_LastPlayStatus = m_PlayStatus;
	ChangePlayStatus(PS_STOP);
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::OnQueryFailed()
{
	if (m_dwStartQueryFailed == 0)
	{
		m_dwStartQueryFailed = GetTickCount();
	}
	
	if (!IsWindowVisible())
	{
		return Activate();
	}

	if (GetTimeSpan(m_dwStartQueryFailed)>10*1000)
	{
		m_dwStartQueryFailed = 0;
		ChangePlayStatus(PS_STOP);
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::OnQuery()
{
	return Activate();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::OnQueryRect()
{
	BOOL bRet = FALSE;

	if(   m_bCanRectSearch 
	   && !m_pMainFrame->IsInBackup())
	{
		if(Activate())
		{
			CQueryRectTracker* pQRT = GetTracker();
			if(pQRT && !pQRT->GetRectPromilleImage()->IsRectEmpty())
			{
				bRet = StartDatabaseSearch(m_stQueryRect);
			}
		}		
	}
	else
	{
		bRet = Activate();
	}
	
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
CIPCSequenceRecord* CPlayWindow::GetNextSequence(WORD wSequenceNr)
{
	CIPCSequenceRecord* pSR = NULL;

	m_Sequences.Lock(_T(__FUNCTION__));
	for (int i=0;i<m_Sequences.GetSize();i++)
	{
		if (m_Sequences.GetAtFast(i)->GetSequenceNr() == wSequenceNr)
		{
			if (i+1<m_Sequences.GetSize())
			{
				pSR = m_Sequences.GetAtFast(i+1);
			}
			// always break if found
			break;
		}
	}
	if (   (pSR == NULL)
		&& (wSequenceNr == 0)
		&& (m_Sequences.GetSize()>0))
	{
		pSR = m_Sequences.GetAtFast(0);
	}
	m_Sequences.Unlock();


	return pSR;
}
/////////////////////////////////////////////////////////////////////////////
CIPCSequenceRecord* CPlayWindow::GetPrevSequence(WORD wSequenceNr)
{
	CIPCSequenceRecord* pSR = NULL;

	m_Sequences.Lock(_T(__FUNCTION__));
	for (int i=0;i<m_Sequences.GetSize();i++)
	{
		if (m_Sequences.GetAtFast(i)->GetSequenceNr() == wSequenceNr)
		{
			if (i>0)
			{
				pSR = m_Sequences.GetAtFast(i-1);
			}
			// always break if found
			break;
		}
	}
	if (   (pSR == NULL)
		&& (wSequenceNr == 0)
		&& (m_Sequences.GetSize()>0))
	{
		pSR = m_Sequences.GetAtFast(m_Sequences.GetSize()-1);
	}
	m_Sequences.Unlock();

	return pSR;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::Navigate(WORD wSequenceNr, DWORD dwRecordNr, 
						   int nRecCount /*=0*/, DWORD dwCam /*= (DWORD)(-1)*/)
{
	//TRACE(_T("#### Navigate %s %08lx\n"),NameOfEnum(m_PlayStatus),GetCurrentThreadId());
	BOOL bRet = FALSE;

	if (!CanNavigate())
	{
		return FALSE;
	}

	if (   m_iRequests > 5 
		&& !m_bDoPrefetch)
	{
		if(!theApp.CanRealtime())
		{
			TRACE(_T("can't navigate pending requests(%d) %s,%d,%d\n"),
				m_iRequests,GetName(),wSequenceNr,dwRecordNr);
		}
		return FALSE; 
	}
	if (!IsWindowVisible())
	{
		TRACE(_T("can't navigate window not visible %s\n"),GetName());
		return FALSE;
	}

	if (   (m_pServer != NULL)
		&& (m_pServer->IsDatabaseConnected())
		)
	{
		if (   m_pPictureRecord == NULL 
			&& m_pAudioRecord == NULL)
		{
			if (nRecCount>0)
			{
				nRecCount = 0;
			}
			if (dwCam != (DWORD)(-1-NAVIGATION_ALL))
			{
				dwCam = (DWORD)(-1-NAVIGATION_ALL);
			}
		}
		if (   m_bGetPicture 
			&& m_PlayStatus != PS_SKIP_BACK && m_PlayStatus != PS_SKIP_FORWARD)
		{
			CSecID sID(dwCam);
			dwCam = CSecID(SECID_GROUP_OUTPUT, sID.GetSubID()).GetID();
		}

		WORD wArchivNr = m_ArchivRecord.GetSubID(); 
	
		CAutoCritSec acs(&m_Sequences.m_cs);
		CIPCSequenceRecord* pSR = m_Sequences.GetSequence(wArchivNr,wSequenceNr);

		if (pSR != NULL)
		{
			// Vorwärts...
			if (nRecCount>0)
			{
				DWORD dwNewRecord = dwRecordNr + nRecCount;
				// Das Ende einer Sequenz erreicht?
				if (dwNewRecord > pSR->GetNrOfPictures())
				{
					// Ersten Record der nächsten Sequenz benutzen.
					pSR = GetNextSequence(wSequenceNr);
					if (pSR == NULL)
					{
						pSR = m_Sequences.GetSequence(wArchivNr,wSequenceNr);
						dwNewRecord = pSR->GetNrOfPictures();
					}
					else
					{
						dwRecordNr	= 0;
						dwNewRecord = 1;
						m_wRequestedSequence = pSR->GetSequenceNr();
					}

					m_dwRequestedRecord = dwRecordNr;
				}

				// Bild aus der Datenbank holen
				if (   dwNewRecord > 0
					&& dwNewRecord != dwRecordNr)
				{
					bRet = RequestRecord(pSR->GetSequenceNr(),
										 dwRecordNr,   // current
										 dwNewRecord, // new
										 dwCam);
					TRACE(_T("#### PlayWindow::RequestRecord() CurRec: %d  NewRec: %d  m_iFFStepWidth: %d\n"), 
										dwRecordNr, dwNewRecord, m_iFFStepWidth);
				}
				else
				{
					ChangePlayStatus(PS_STOP);
				}
			}
			else if (nRecCount<0) // Rückwärts...
			{
				DWORD dwNewRecord = 0;
				int iRecordNr = dwRecordNr;
				// Den Anfang der Sequenz erreicht?
				if (   !m_bNavigateToLastRecord
					&& (iRecordNr<=-nRecCount))
				{
					// Letzten Record der vorherigen Sequenz nehmen.
					pSR = GetPrevSequence(wSequenceNr);
					if (!pSR)
					{
						pSR = m_Sequences.GetSequence(wArchivNr,wSequenceNr);
						dwNewRecord = 1;
					}
					else
					{
						dwRecordNr	= 0;
						dwNewRecord	= pSR->GetNrOfPictures();
					}
				}
				else
				{
					//in Audio Archiven:
					//es soll zum letzen Bild in der Sequenz navigiert werden
					//aber das aktuelle record ist dwRecordNr == 1
					//hier ist beim Rücksprung nRecCount == -1 sodass 
					//die Summe 0 ergibt, was falsch ist, denn damit wird nie ein Bild gefunden
					//deshalb an dieser Stelle im else-Zweig korrekte Werte zuweisen,
					//damit im RequestRecord() bei m_bNavigateToLastRecord == TRUE zum
					//letzen Bild der sequenz gesprungen werden kann
					
					if(dwRecordNr + nRecCount > 0)
					{
						dwNewRecord = dwRecordNr + nRecCount;
					}
					else
					{
						dwRecordNr	= 0;
						dwNewRecord = dwRecordNr + 1;
					}
					
				}

				// Bild aus der Datenbank holen
				if (   dwNewRecord > 0
					&& dwNewRecord != dwRecordNr)
				{
					if(pSR)
					{
						bRet = RequestRecord(pSR->GetSequenceNr(),
											 dwRecordNr,
											 dwNewRecord,
											 dwCam);

					}
				}
				else
				{
					ChangePlayStatus(PS_STOP);
				}
			}
			else
			{
				if(pSR)
				{
					BOOL bNavigateForward = (    (m_RectPlayStatus == PS_RECT_PLAY_FORWARD) 
						|| (m_RectPlayStatus == PS_RECT_SINGLE_FORWARD)
						);
					//falls bei der Rechtecksuche mittels der Vorwärtsspiel-Buttons gesucht wird,
					//muss diese Richtungsangabe geprüft werden, da beim Vorwärtsspielen
					//mit Rechtecksuche bei Tashabildern immer wieder das IFrame geliefert wird.
					//Ein Vorwärtsspielen ist somit nicht möglich, wenn die Differenzbilder aufgrund 
					//der rechtecksuche einen Treffer liefern. Bei Rückwärtsspielen wird bei Tashabildern
					//immer das vorherige IFrame geliefert.					
					
					if(bNavigateForward && dwRecordNr-1 > 0)
					{
						bRet = RequestRecord(pSR->GetSequenceNr(),dwRecordNr-1,dwRecordNr,dwCam);
					}
					else
					{
						//rückwarts Rechtecksuche oder normale Suche mittels Suchbutton
						bRet = RequestRecord(pSR->GetSequenceNr(),0,dwRecordNr,dwCam);
						//TRACE(_T("######### Request Rectangle: Sequ: %d  Rec: %d\n"), pSR->GetSequenceNr(),dwRecordNr);
					}
				}
			}
		}
		else
		{
			// keine Sequence 
//			TRACE(_T("keine Sequence\n"));
		}
	}
	else
	{
		WK_TRACE_WARNING(_T("can't navigate not connected\n"));
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::RequestRecord(WORD wSequenceNr, 
								DWORD dwRecordNr, 
								DWORD dwNewRecord, 
								DWORD dwCam)
{
	BOOL bRet = FALSE;
	BOOL  bRequest = FALSE;
	DWORD dwDiff = GetTimeSpan(m_dwLastRequest);
	DWORD dwDiffReference = m_bDoPrefetch ? theApp.m_nTimeOutPlay : REQUEST_TIMEOUT_PLAY;

/*
	if (   (m_PlayStatus == PS_FAST_FORWARD)
		|| (m_PlayStatus == PS_FAST_BACK)
		)
	{
		dwDiffReference = REQUEST_TIMEOUT_FF;
	}
	*/

	if (dwDiff > dwDiffReference)							// if the request is in time
	{
		m_dwLastRequest = GetTickCount();
		bRequest = TRUE;
		
#ifdef BUFERED_PICTURE_IF_AUDIO
		if (m_bDoPrefetch)									// during prefetch of mixed archives
		{
			m_csPictureRecord.Lock();
			int nCount = m_PictBuffer.GetCount();
			while (nCount)									// look up for buffered pictures
			{												// take the newest picture
				CIPCPictureRecord*pPict = (CIPCPictureRecord*) m_PictBuffer.RemoveTail();
				nCount = m_PictBuffer.GetCount();
				BOOL bNewer = FALSE;
				if (m_pPictureRecord)						// and compare with the current
				{
					CSystemTime stPB = pPict->GetTimeStamp();
					CSystemTime stPR = m_pPictureRecord->GetTimeStamp();
					bNewer = stPB > stPR;
				}
				else
				{
					WK_TRACE(_T("No Picture record to compare\n"), nCount);
				}
															// actualize only if it is newer
				if (bNewer && CDisplayWindow::PictureData(*pPict,0,0))
				{
					TRACE(_T("Video removed from buffer(%d) and inserted\n"), nCount);
					if (pPict->GetCompressionType() == COMPRESSION_MPEG4)
					{
						//TODO TKR das current zu decodierende record wieder setzen
						//diese info ist aber in Pict nicht vorhanden
						//m_wCurrentSequence = wSequenceNr;
						//m_dwCurrentRecord = dwRecordNr;

						BOOL bDecode = DecodeMpeg(*pPict, 0, 0);
						if(bDecode)
						{
							if(theApp.TKR_Trace())
							{
								CString sFrame = _T("P-Frame");
								if(pPict->GetPictureType()==SPT_FULL_PICTURE)
								{
									sFrame = _T("I-Frame");
								}
								TRACE(_T("TKR ######### DECODE Video removed from buffer RecordNr (Picture): SequNr: %d  RecNr: %d   %s \n"), m_wCurrentSequence, m_dwCurrentRecord,sFrame);
							}
						}
						else
						{
							if(theApp.TKR_Trace())
							{
								TRACE(_T("TKR ######### DECODE Failed Video removed from buffer RecordNr (Picture): SequNr: %d  RecNr: %d\n"), m_wCurrentSequence, m_dwCurrentRecord);
							}
						}
					}
					else
					{
						PostMessage(WM_USER, ID_CHANGE_VIDEO);
					}
				}
				else										// older pictures are deleted
				{
					if (m_iRequests > 0)
					{
						m_iRequests--;
					}
//					TRACE(_T("Older Video removed from buffer(%d)\n"), nCount);
				}
				WK_DELETE(pPict);
			}
			m_csPictureRecord.Unlock();
		}
#endif
	}
	else if (m_bDoPrefetch)
	{
		bRequest = TRUE;
	}

	if (bRequest)
	{
		if (m_bDoPrefetch && dwDiff > 0)
		{
//			TRACE(_T("### RKE Test:Prefetch:%d ms\n"), dwDiff);
		}
		m_wLastSequenceRequest = wSequenceNr;
		m_dwLastRecordRequest = dwRecordNr;
		if (dwNewRecord != m_dwLastNewRecordRequest)
		{
			m_dwLastNewRecordRequest = dwNewRecord;
			m_dwLastCamRequest = dwCam;
			m_LastPlayStatus = m_PlayStatus;
			if (m_bNavigateToLastRecord)
			{
				TRACE(_T("navigate to last record of cam %s\n"),GetName());
				dwCam = (DWORD)(-1-NAVIGATION_LAST);
				m_bNavigateToLastRecord = FALSE;
			}

#ifdef _DEBUG
			CSystemTime st;
			st.GetLocalTime();
			//TRACE(_T("\n"));
			//TRACE(_T("##### request record: S=%d R=%d %s,%03d\n"),
			//	wSequenceNr,dwNewRecord,st.GetTime(),st.wMilliseconds);
#endif
			m_iRequests++;
			m_pServer->GetDatabase()->DoRequestRecordNr(m_ArchivRecord.GetID(),
														wSequenceNr,
														dwRecordNr,   // current
														dwNewRecord, // new
														dwCam);
			bRet = TRUE;
		}
	}
	else
	{
		// waiting for some time elapsed
		// a simple brake
		// so the system is idle 
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::StartDatabaseSearch(const CSystemTime &st)
{
	if (   m_bCanRectSearch 
		&& !m_pMainFrame->IsInBackup())
	{
		if (m_PlayStatus == PS_QUERY)
		{
			return FALSE;
		}

		if (!m_pServer->IsDatabaseConnected())
		{
			return FALSE;
		}

/* TODO UF Das Decodieren in einen SingleThread stecken, damit die queue nicht immer voll läuft
		if(m_pMpeg)
		{
			TRACE(_T("######## m_pMpeg->GetDecoderQueueLength(): %d \n"),m_pMpeg->GetDecoderQueueLength());
			if(m_pMpeg->GetDecoderQueueLength() > 5)
			{
				TRACE(_T("######## StartDatabaseSearch() return FALSE\n"));
				Sleep(100);
			}
		}
*/
		m_sQuery = st.GetDateTime();


		if(m_RectPlayStatus == PS_RECT_NO_RECT)
		{
			//Image nur löschen bei Suche via Suchbutton
			DeletePicture(FALSE);
		}

		m_bSuspect = FALSE;

		ChangePlayStatus(PS_QUERY);
		if(theApp.TKR_Trace())
		{
			TRACE(_T("TKR ### StartDatabaseSearch ChangePlayStatus nach PS_QUERY, ID: %d\n"), GetID());
		}

		CIPCDatabaseDVClient* pDatabase = m_pServer->GetDatabase();
		WORD w[1];
		CIPCFields fields;
		CString sDate(st.GetDBDate());
		CString sTime(st.GetDBTime());

		//abhängig von der Suchrichtung wird die Datenbank
		//vorwärts bzw. rückwärts durchsucht
		char op = _T('{');
		
		if(    m_RectPlayStatus == PS_RECT_SINGLE_BACK
			|| m_RectPlayStatus == PS_RECT_PLAY_BACK)
		{
			//rückwärts
//			TRACE(_T("******** suche rückwärts\n"));
			op = _T('{');
		}
		else
		{
			//vorwärts
//			TRACE(_T("******** suche vorwärts\n"));
			op = _T('}');
		}
		fields.SafeAdd(new CIPCField(_T("DVR_DATE"),sDate,op));
		fields.SafeAdd(new CIPCField(_T("DVR_TIME"),sTime,op));

		//if rectangle search, add rectangle and to the query fields
		CQueryRectTracker* pQRT = GetTracker();
		if(pQRT && !pQRT->GetRectPromilleImage()->IsRectEmpty())
		{
			if(theApp.TKR_Trace())
			{
				TRACE(_T("*** Start Rect Suche ****\n"));
			}
			//PS_RECT_QUERY_RECT heißt Suche mittels Suchbutton, also ohne seconds und milliseconds
			if(m_RectPlayStatus != PS_RECT_QUERY_RECT)
			{	
				WORD wMilliSec = st.GetMilliseconds();
				CString sMilliSec;
				sMilliSec.Format(_T("%03i"), wMilliSec);
				
				WK_TRACE(_T("query Date=%s, Time=%s Milli:=%s\n"),sDate,sTime,sMilliSec);
				if(    m_RectPlayStatus == PS_RECT_SINGLE_BACK
					|| m_RectPlayStatus == PS_RECT_PLAY_BACK)
				{
					//suche rückwärts
					fields.SafeAdd(new CIPCField(_T("DVR_MS"),sMilliSec,_T('{')));
				}
				else
				{
					//suche vorwärts
					fields.SafeAdd(new CIPCField(_T("DVR_MS"),sMilliSec,_T('}')));
				}

//				TRACE(_T("*** Rect Suche: ms: %s\n"), sMilliSec);
			}

			AddRectToDatabaseSearch(fields);

			//schraffiere das Rechteck zu Beginn einer Rechtecksuche
			//Die Schraffierung wird nach Ende der Suche wieder aufgehoben
//TODO tkr hier einbauen, dass das Rechteck bei einer längeren Suche schraffiert wird
			//pQRT->m_nStyle = CRectTracker::hatchInside;
		}

		w[0] = m_ArchivRecord.GetID();
		pDatabase->DoRequestNewQuery(GetID().GetID(),1,w,fields,1);
		if(theApp.TKR_Trace())
		{
			TRACE(_T("TKR ------- Start NewQuery: ID: %d \n"), GetID().GetID());
		}
	}
	else
	{
		TRACE(_T("#### PlayStatus=%s\n"), NameOfEnum(m_PlayStatus));

		if (m_PlayStatus == PS_QUERY)
		{
			return FALSE;
		}

		if (!m_pServer->IsDatabaseConnected())
		{
			return FALSE;
		}

		m_sQuery = st.GetDateTime();

		DeletePicture(FALSE);

		m_bSuspect = FALSE;

		ChangePlayStatus(PS_QUERY);

		CIPCDatabaseDVClient* pDatabase = m_pServer->GetDatabase();
		WORD w[1];
		CIPCFields fields;
		CString sDate(st.GetDBDate());
		CString sTime(st.GetDBTime());

		WK_TRACE(_T("query Date=%s, Time=%s\n"),sDate,sTime);
		fields.SafeAdd(new CIPCField(_T("DVR_DATE"),sDate,_T('}')));
		fields.SafeAdd(new CIPCField(_T("DVR_TIME"),sTime,_T('}')));
		w[0] = m_ArchivRecord.GetID();
		
		pDatabase->DoRequestNewQuery(GetID().GetID(),1,w,fields,1);

		DoRedraw();
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CPlayWindow::AddRectToDatabaseSearch(CIPCFields &fields)
{
/*			Y-Achse
			
			^
	   sY1__|_ _ _ _ _______________
			|		|				|
			|		|				|
			|		|	       _|_..|.... Fadenkreuz auf Bild
			|		|			|	|	  mit Koordinaten
			|		|				|	  DBD_MD_X und DBD_MD_Y
	   sY2__|_ _ _ _|_______________|
			|		|				|
			|
			|_______|_______________|__ > X-Achse
					
					|				|
				   sX1			   sX2
*/	

	if(m_bCanRectSearch && !theApp.GetMainFrame()->IsInBackup())
	{
		//search only within the rectangle
		CQueryRectTracker* pQRT = GetTracker();
		if(pQRT && !pQRT->GetRectPromilleImage()->IsRectEmpty())
		{
			CString sX1, sY1, sX2, sY2;
			//hole Promille Werte des Rechtecks bez. auf das Image
			CRect rcQuery = pQRT->GetRectPromilleImage();
			
			sX1.Format(_T("%04x"), rcQuery.left);
			fields.Add(new CIPCField(_T("DVD_MD_X"),
						sX1,
						4, _T('}')));
			sY1.Format(_T("%04x"), rcQuery.top);
			fields.Add(new CIPCField(_T("DVD_MD_Y"),
						sY1,
						4, _T('}')));
			sX2.Format(_T("%04x"), rcQuery.right);
			fields.Add(new CIPCField(_T("DVD_MD_X"),
						sX2,
						4, _T('{')));
			sY2.Format(_T("%04x"), rcQuery.bottom);
			fields.Add(new CIPCField(_T("DVD_MD_Y"),
						sY2,
						4, _T('{')));

/*			TRACE(_T("*** übergebene Rechteck Werte*****\n"));
			TRACE(_T("*** left: %i  top: %i  right: %i  bottom: %i \n"),
				rcQuery.left, rcQuery.top, rcQuery.right, rcQuery.bottom);
*/
			//bei Tasha und Rechtecksuche nur nach IFRames suchen

			if(m_bCanTashaRectangleSearch)
			{	
				CString sIFrameOnly = _T("2"); //1 = Differenzbild
				fields.Add(new CIPCField(_T("DVR_TYP"),
					sIFrameOnly,
					4, _T('=')));
			}
		}
	}

}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::IndicateError(CIPCError error, int iErrorCode, const CString &sErrorMessage)
{
	if (error == CIPC_ERROR_INVALID_VALUE)
	{
		switch (iErrorCode)
		{
		case 1:
			TRACE(_T("no record found for cam\n"));
			m_iRequests = 0;
			break;
		case 2:
			TRACE(_T("record nr out of range\n"));
			m_iRequests = 0;
			break;
		case 3: 
			TRACE(_T("sequence nr out of range\n"));
			m_iRequests = 0;
			break;
		case 4: 
			TRACE(_T("archiv nr out of range\n"));
			m_iRequests = 0;
			break;
		case 5:
			TRACE(_T("no record found for %s\n"),NameOfEnum(m_LastPlayStatus));
			m_iRequests = 0;
			{
				CIPCSequenceRecord* pSR = GetNextSequence(m_wCurrentSequence);
				if (pSR != NULL)
				{
					m_wCurrentSequence = pSR->GetSequenceNr();
					m_dwCurrentRecord = 1;
					ChangePlayStatus(m_LastPlayStatus);
				}
				else
				{
					WK_TRACE(_T("no record found for %s\n"),NameOfEnum(m_LastPlayStatus));
					ChangePlayStatus(PS_NAVIGATE_FAILED);
				}
			}
			break;
		case 6:
			TRACE(_T("no record found for %s\n"),NameOfEnum(m_LastPlayStatus));
			m_iRequests = 0;
			{
				CIPCSequenceRecord* pSR = GetPrevSequence(m_wCurrentSequence);
				if (pSR != NULL)
				{
					m_wCurrentSequence = pSR->GetSequenceNr();
					m_dwCurrentRecord = pSR->GetNrOfPictures();
					ChangePlayStatus(m_LastPlayStatus);
				}
				else
				{
					WK_TRACE(_T("no record found for %s\n"),NameOfEnum(m_LastPlayStatus));
					ChangePlayStatus(PS_NAVIGATE_FAILED);
				}
			}
			break;
		case 7:
			WK_TRACE(_T("cannot open sequence dbf file error %s\n"),GetName());
			ChangePlayStatus(PS_READ_FAILED);
			m_iRequests = 0;
			break;
		case 8:
			WK_TRACE(_T("cannot open sequence dat file error %s\n"),GetName());
			ChangePlayStatus(PS_READ_FAILED);
			m_iRequests = 0;
			break;
		default:
			m_iRequests = 0;
			WK_TRACE(_T("error invalid value: %s\n"), LPCTSTR(sErrorMessage));
			break;
		}
	}
	else if (error == CIPC_ERROR_ACCESS_DENIED)
	{
		TRACE(_T("access denied PlayStatus is %s\n"),NameOfEnum(m_PlayStatus));
		m_iRequests = 0;
		ChangePlayStatus(PS_ACCESS_DENIED);
	}

	if (WK_IS_WINDOW(this))
	{
		DoRedraw();
	}

	Sleep(20);

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::IndicateQueryResult(WORD wSequenceNr, DWORD dwRecordNr)
{
	// Achtung im CIPC Thread keine GUI Funktionen aufrufen!
	if(theApp.TKR_Trace())
	{
		TRACE(_T("TKR ### IndicateQueryResult PlayStatus: %s  ID: %d\n"), NameOfEnum(m_PlayStatus), GetID());
		TRACE(_T("RECTPLAYSTATUS: %d\n"), m_RectPlayStatus);
	}
	if (   m_PlayStatus == PS_QUERY
		|| m_RectPlayStatus != PS_RECT_NO_RECT) //bei Rechtecksuche ist beim Abspielen vorwärts/rückwärts
												//an dieser Stelle m_PlayStatus = PS_PLAY_FORWARD bzw. 
												//PS_PLAY_BACK. Deshalb hier m_RectPlayStatus mit 
												//auswerten, ob gerade eine Rechtecksuche läuft
	{
		if (IsWindowVisible())
		{
			Navigate(wSequenceNr,dwRecordNr);
			if(m_bCanRectSearch && !theApp.GetMainFrame()->IsInBackup())
			{
				BOOL bForward = m_RectPlayStatus == PS_RECT_PLAY_FORWARD;
				BOOL bBack = m_RectPlayStatus == PS_RECT_PLAY_BACK;
				BOOL bQuery = m_RectPlayStatus == PS_RECT_QUERY_RECT;
				CQueryRectTracker* pQRT = GetTracker();
				
				if(bForward || bBack || bQuery)
				{
					if(pQRT)
					{
						if(pQRT->GetRectPromilleImage()->IsRectEmpty())
						{
							//do nothing, only show last picture
							m_RectPlayStatus = PS_RECT_STOP;
							ChangePlayStatus(PS_STOP);
						}
						else
						{
							if(bForward)
							{
								//go on with play forward
								ChangePlayStatus(PS_PLAY_FORWARD);
							}
							if(bBack)
							{
								//go on with play back
								ChangePlayStatus(PS_PLAY_BACK);
							}
						}
					}			
				}
				else
				{
					ChangePlayStatus(PS_STOP);
					if(pQRT && !pQRT->GetRectPromilleImage()->IsRectEmpty())
					{
						m_RectPlayStatus = PS_RECT_STOP;
					}
				}

				//es gab ein Suchergebnis
				if(pQRT && !pQRT->GetRectPromilleImage()->IsRectEmpty())
				{
					m_bIsNextForwardQueryRectResult = TRUE;
					m_bIsNextBackQueryRectResult = TRUE;
					
					//Die zu Beginn der Suche gesetzte Schraffierung des Rechtecks
					//hier wieder aufheben
//TODO tkr hier einbauen, dass das Rechteck bei einer längeren Suche schraffiert wird
/*					pQRT->m_nStyle = CRectTracker::hatchedBorder;
					pQRT->m_nStyle ^= CRectTracker::resizeOutside;
					Sleep(1000);
					pQRT->Draw(GetDC());
*/
				}
			}
		}
		else
		{
			m_wCurrentSequence = wSequenceNr;
			m_dwCurrentRecord = dwRecordNr;
			ChangePlayStatus(PS_STOP);
		}
		if(!m_bCanRectSearch && theApp.GetMainFrame()->IsInBackup())
		{
			ChangePlayStatus(PS_STOP);
		}

		return TRUE;
	}
	else
	{
		// Es kann sein, das vor dem Stoppen
		// der Suche noch mehr Ergebnisse 
		// indiziert werden, das interessiert uns aber nicht
 		TRACE(_T("unexpected query result %d,%d\n"),wSequenceNr,dwRecordNr);
		return FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::ConfirmQuery()
{
	if(m_bCanRectSearch)
	{
		//vorherige Suche ist beendet,
		//nächste Rechtecksuche kann gestartet werden
		m_bPlayNextRect = TRUE;
	}

	if (m_PlayStatus == PS_QUERY)
	{
		WK_TRACE(_T("nothing found at query %s\n"),GetName());

		if(m_bCanRectSearch && !theApp.GetMainFrame()->IsInBackup())
		{
			switch(m_RectPlayStatus)
			{
			case PS_RECT_NO_RECT:
				//no rectangle search
				NavigateLastPicture(PS_QUERY_FAILED);
				break;

			case PS_RECT_SINGLE_FORWARD:
			case PS_RECT_PLAY_FORWARD:
			case PS_RECT_PLAY_BACK:
			case PS_RECT_SINGLE_BACK:
			case PS_RECT_QUERY_RECT:
				{
					//do nothing, stay at last picture
					TRACE(_T("*** ConfirmQuery: lastSequ: %i  lastRec: %i\n"),
							m_wCurrentSequence, m_dwCurrentRecord);
					m_bPlayNextRect = TRUE;
					m_bStopRectPlay = FALSE;

					if(    m_RectPlayStatus == PS_RECT_PLAY_BACK
						|| m_RectPlayStatus == PS_RECT_SINGLE_BACK)
					{
						m_bIsNextBackQueryRectResult = FALSE;
					}
					else if(m_RectPlayStatus == PS_RECT_QUERY_RECT)
					{
						m_bIsNextBackQueryRectResult = TRUE;
						m_bIsNextForwardQueryRectResult = TRUE;
					}
					else
					{
						m_bIsNextForwardQueryRectResult = FALSE;
					}

					m_RectPlayStatus = PS_RECT_STOP;
					ChangePlayStatus(PS_STOP);		
				}
				break;
				
			default:
				m_RectPlayStatus = PS_RECT_STOP;
				ChangePlayStatus(PS_STOP);
				break;
			}
			DoRedraw();
		}
		else
		{
			NavigateLastPicture(PS_QUERY_FAILED);
			DoRedraw();
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("ID: %d ,no longer in PS_QUERY, now %s\n"),GetID(), NameOfEnum(m_PlayStatus));
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::ConfirmGetValue(const CString &sKey,const CString &sValue)
{
	BOOL bRet = FALSE;
	if (sKey == CSD_NAME)
	{
		m_csPictureRecord.Lock();
		m_ArchivRecord.SetName(sValue);
		m_csPictureRecord.Unlock();
		DoRedraw();
		bRet = TRUE;
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::OnDeleteCurrentAlarm()
{
	BOOL bRet = FALSE;
	
	if (IsAlarm())
	{
		// ist ein Alarmarchiv
		DeletePicture(FALSE);
		m_bSuspect = FALSE;

		m_csSequencesToDelete.Lock();
		m_waSequencesToDelete.RemoveAll();

		m_Sequences.Lock(_T(__FUNCTION__));
		if (m_Sequences.GetSize() > 0)
		{
			// es gibt überhaupt Sequenzen
			CIPCSequenceRecord* pCurrentSequence = NULL;
			int iIndex = -1;
			// suche den Index der aktuellen Sequenz
			for (int i=0;i<m_Sequences.GetSize();i++)
			{
				if (m_Sequences.GetAtFast(i)->GetSequenceNr() == m_wCurrentSequence)
				{
					pCurrentSequence = m_Sequences.GetAtFast(i);
					iIndex = i;
					break;
				}
			}

			if (pCurrentSequence)
			{
				// es gibt die aktuelle Sequenz
				if (pCurrentSequence->IsSafeRing())
				{
					// ist ein Vorring
					CalcPreAlarm(iIndex,m_waSequencesToDelete);
				}
				else if (pCurrentSequence->IsAlarm())
				{
					// ist ein Alarm
					CalcAlarm(iIndex,m_waSequencesToDelete);
				}
				else
				{
					// Ring oder Verdacht
				}
			}

		} // Sequenzen
		m_Sequences.Unlock();

		// nun löschen 
		m_dwStartDeleting = GetTickCount();
		ChangePlayStatus(PS_DELETING);
		RequestDeleteSequences(m_waSequencesToDelete);
		m_csSequencesToDelete.Unlock();

		DoRedraw();

	} // IsAlarm
	else
	{
		ChangePlayStatus(PS_STOP);
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CPlayWindow::CalcPreAlarm(int iIndex,CWordArray& numbers)
{
	m_Sequences.Lock(_T(__FUNCTION__));
	// alle Sequenz Nummern zum Array hinzufügen
	CIPCSequenceRecord* pSR = NULL;

	int i;
	// 1. zurück bis zum ersten nicht Voralarm
	for (i=iIndex-1;i>=0;i--)
	{
		pSR = m_Sequences.GetAtFast(i);
		if (pSR->IsSafeRing())
		{
			// Vorring und löschen
			numbers.Add(pSR->GetSequenceNr());
		}
		else
		{
			// Ring, Alarm, Verdacht
			break;
		}
	}

	// 2. vorwärts bis zum ersten nicht Voralarm
    for (i=iIndex;i<m_Sequences.GetSize();i++)
	{
		pSR = m_Sequences.GetAtFast(i);
		if (pSR->IsSafeRing())
		{
			// Vorring und löschen
			numbers.Add(pSR->GetSequenceNr());
		}
		else
		{
			// Ring, Alarm, Verdacht
			break;
		}
	}

	// 3. und weiter bis zum ersten Voralarm
    for (;i<m_Sequences.GetSize();i++)
	{
		pSR = m_Sequences.GetAtFast(i);
		if (pSR->IsSafeRing())
		{
			// Vorring und Ende
			break;
		}
		else if (pSR->IsAlarm())
		{
			// Alarm und löschen
			numbers.Add(pSR->GetSequenceNr());
		}
		else
		{
			// Ring, Verdacht
		}
	}
	m_Sequences.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CPlayWindow::CalcAlarm(int iIndex, CWordArray& numbers)
{
	m_Sequences.Lock(_T(__FUNCTION__));
	// alle Sequenz Nummern zum Array hinzufügen
	CIPCSequenceRecord* pSR = NULL;
	int i;

	// 1. vorwärts bis zum ersten Voralarm
	for (i=iIndex;i<m_Sequences.GetSize();i++)
	{
		pSR = m_Sequences.GetAtFast(i);
		if (pSR->IsSafeRing())
		{
			// Vorring und Ende
			break;
		}
		else if (pSR->IsAlarm())
		{
			// Alarm und löschen
			numbers.Add(pSR->GetSequenceNr());
		}
		else
		{
			// Ring oder Verdacht ignorieren
		}
	}

	// 2. zurück bis zum ersten Voralarm
	for (i=iIndex-1;i>=0;i--)
	{
		pSR = m_Sequences.GetAtFast(i);
		if (pSR->IsSafeRing())
		{
			// Vorring und Ende
			break;
		}
		else if (pSR->IsAlarm())
		{
			// Alarm und löschen
			numbers.Add(pSR->GetSequenceNr());
		}
		else
		{
			// Ring oder Verdacht ignorieren
		}
	}

	// 3. und zurück bis zum ersten nicht Voralarm
	for (;i>=0;i--)
	{
		pSR = m_Sequences.GetAtFast(i);
		if (pSR->IsSafeRing())
		{
			// Vorring und löschen
			numbers.Add(pSR->GetSequenceNr());
		}
		else
		{
			// Alarm, Ring oder Verdacht Ende
			break;
		}
	}
	m_Sequences.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
void CPlayWindow::RequestDeleteSequences(CWordArray& numbers)
{
	if (m_pServer->IsDatabaseConnected())
	{
		CIPCDatabaseDVClient* pDC = m_pServer->GetDatabase();
		for (int i=0;i<numbers.GetSize();i++)
		{
			TRACE(_T("deleting %04x %d\n"),m_ArchivRecord.GetID(),numbers[i]);
			pDC->DoRequestDeleteSequence(m_ArchivRecord.GetID(),numbers[i]);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::OnDeleting()
{
	BOOL bRet = FALSE;

	// Time Out deleting sequences
	if (GetTimeSpan(m_dwStartDeleting) > 30*1000)
	{
		m_dwStartDeleted = GetTickCount();
		ChangePlayStatus(PS_DELETED);
		Invalidate();
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::OnDeleted()
{
	BOOL bRet = FALSE;

	// Time Out deleted sequences
	if (GetTimeSpan(m_dwStartDeleted) > 20*1000)
	{
		ChangePlayStatus(PS_STOP);
		Invalidate();
		Navigate(m_wCurrentSequence,m_dwCurrentRecord);
	}

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::GetAlarmStartandEndTime(CSystemTime& start,CSystemTime& end)
{
	BOOL bRet = FALSE;

	if (m_ArchivRecord.IsAlarm())
	{
		CDWordArray	dwaSequencesToBackup;
		CalcBackupAlarm(dwaSequencesToBackup);

		CIPCSequenceRecords sequences;

		if (dwaSequencesToBackup.GetSize()>0)
		{
			CAutoCritSec acs(&m_Sequences.m_cs);
			for (int i=0;i<dwaSequencesToBackup.GetSize();i++)
			{
				WORD wSequenceNr = LOWORD(dwaSequencesToBackup.GetAt(i));
				sequences.Add(m_Sequences.GetSequence(GetNr(),wSequenceNr));
			}
			sequences.Sort();
			if (sequences.GetSize())
			{
				CIPCSequenceRecord* pFirstSequence = sequences.GetAtFast(0);
				start = pFirstSequence->GetTime();
				CIPCSequenceRecord* pLastSequence = sequences.GetAtFast(sequences.GetSize()-1);
				end = GetSequenceEndtime(pLastSequence->GetSequenceNr());
				
				WK_TRACE(_T("Alarm Backup: start: %s -> end: %s\n"), start.GetDateTime(),
																 end.GetDateTime());
				bRet = TRUE;
			}
		}
	}


	return bRet;
}
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
CString CPlayWindow::NameOfEnum(PlayStatus ps)
{
	switch (ps)
	{
	case PS_STOP:						return _T("PS_STOP");
	case PS_PLAY_BACK:					return _T("PS_PLAY_BACK");
	case PS_PLAY_FORWARD:				return _T("PS_PLAY_FORWARD");
	case PS_FAST_BACK:					return _T("PS_FAST_BACK");
	case PS_FAST_FORWARD:				return _T("PS_FAST_FORWARD");
	case PS_EJECT:						return _T("PS_EJECT");
	case PS_SKIP_BACK:					return _T("PS_SKIP_BACK");
	case PS_SKIP_FORWARD:				return _T("PS_SKIP_FORWARD");
	case PS_SINGLE_BACK:				return _T("PS_SINGLE_BACK");
	case PS_SINGLE_FORWARD:				return _T("PS_SINGLE_FORWARD");
	case PS_QUERY:						return _T("PS_QUERY");
	case PS_QUERY_FAILED:				return _T("PS_QUERY_FAILED");
	case PS_DELETE_ALARM:				return _T("PS_DELETE_ALARM");
	case PS_DELETE_SUSPECT:				return _T("PS_DELETE_SUSPECT");
	case PS_DELETING:					return _T("PS_DELETING");
	case PS_DELETED:					return _T("PS_DELETED");
	case PS_ACCESS_DENIED:				return _T("PS_ACCESS_DENIED");
	case PS_SHOW_SUSPECT:				return _T("PS_SHOW_SUSPECT");
	case PS_NAVIGATE_FAILED:			return _T("PS_NAVIGATE_FAILED");
	case PS_READ_FAILED:				return _T("PS_READ_FAILED");
	case PS_QUERY_RECT:					return _T("PS_QUERY_RECT");

	default:							return _T("unknown playstatus");
	}
}	

/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::NavigateLastPicture(PlayStatus playstatus /*= PS_STOP*/)
{
	CIPCSequenceRecord* pSR = NULL;
	BOOL bRet				= FALSE;
	WORD wSeqNr				= 0;
	DWORD dwPicNr			= 0;

	m_Sequences.Lock(_T(__FUNCTION__));
	int nSize = m_Sequences.GetSize();
	if (nSize > 0)
	{
		pSR = m_Sequences.GetAtFast(nSize-1);	
		if (pSR)
		{
			wSeqNr = pSR->GetSequenceNr();
			dwPicNr = pSR->GetNrOfPictures();

			if(dwPicNr == 1)
			{
				WK_TRACE_ERROR(_T("### NavigateLastPicture() NumberOfRecords == 1\n"));
			}

//			if ((wSeqNr >= 0) && (dwPicNr >= 0)) // ML Ein 'WORD' ist immer >= 0
			{
				// aufs letzte Bild springen
				m_wCurrentSequence = wSeqNr;
				m_dwCurrentRecord = dwPicNr;
				m_bNavigateToLastRecord = TRUE;
				if (playstatus == PS_STOP)
				{
					Navigate(m_wCurrentSequence,m_dwCurrentRecord);
				}
				//hier nur rein bei Rechtecksuche mittels Suchbutton und Zeitpunkt hinter
				//dem letzten Bild des Archivs liegt. Dann zuerst zum Ende des Archivs
				//springen und anschließend die Rechtecksuche rückwärts durchführen um zum
				//letzten Bild zu gelangen, das der Rechtecksuche entspricht
				if(playstatus == PS_SINGLE_BACK && m_RectPlayStatus == PS_RECT_SINGLE_BACK)
				{
					Navigate(m_wCurrentSequence,m_dwCurrentRecord);

					//wird auf TRUE gesetzt und in IndicateQueryResult ausgewertet.
					//dementsprechend werden dann die Vorwärts-Button disabled
					m_bIsNextBackQueryRectResult = TRUE;
				}

				bRet = TRUE;
			}
		}
	}
	m_Sequences.Unlock();

	SetPlayStatus(playstatus);

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::NavigateFirstPicture(PlayStatus playstatus /*= PS_STOP*/)
{
	CIPCSequenceRecord* pSR = NULL;
	BOOL bRet				= FALSE;
	WORD wSeqNr				= 0;
	DWORD dwPicNr			= 0;

	m_Sequences.Lock(_T(__FUNCTION__));
	int nSize = m_Sequences.GetSize();
	if (nSize > 0)
	{
		pSR = m_Sequences.GetAtFast(0);	
		if (pSR)
		{
			wSeqNr = pSR->GetSequenceNr();
			dwPicNr = 1;

//			if ((wSeqNr >= 0) && (dwPicNr >= 0)) // ML Ein 'WORD' ist immer >= 0
			{
				// aufs erste Bild springen
				m_wCurrentSequence = wSeqNr;
				m_dwCurrentRecord = dwPicNr;
				if (playstatus == PS_STOP)
				{
					Navigate(m_wCurrentSequence,m_dwCurrentRecord);
				}
				bRet = TRUE;
			}
		}
	}
	m_Sequences.Unlock();

	SetPlayStatus(playstatus);

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::OnShowSuspect()
{
	CIPCSequenceRecord* pSR = NULL;
	BOOL bRet				= FALSE;

	if (Activate())
	{
		m_Sequences.Lock(_T(__FUNCTION__));

		pSR = m_Sequences.GetSequence(GetID().GetSubID(),m_wCurrentSequence);

		// vorwärts suchen
		while (pSR)
		{
			if (pSR && pSR->IsSuspect())
			{
				break;
			}

			pSR = GetNextSequence(pSR->GetSequenceNr());
		}
		if (!pSR)
		{
			// nichts gefunden, also nochmal rückwärts
			pSR = m_Sequences.GetSequence(GetID().GetSubID(),m_wCurrentSequence);
			while (pSR)
			{
				if (pSR && pSR->IsSuspect())
				{
					break;
				}

				pSR = GetPrevSequence(pSR->GetSequenceNr());
			}
		}

		if (pSR)
		{
			// was gefunden, auf das
			// erste Bild der Sequenz navigieren!
			Navigate(pSR->GetSequenceNr(),1,0);
		}

		m_Sequences.Unlock();
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::OnDeleteSuspect()
{
	BOOL bRet = FALSE;
	
	if (IsAlarm())
	{
		DeletePicture(FALSE);
		m_bSuspect = FALSE;

		m_csSequencesToDelete.Lock();
		m_waSequencesToDelete.RemoveAll();

		m_Sequences.Lock(_T(__FUNCTION__));
		CIPCSequenceRecord* pSR = m_Sequences.GetSequence(GetID().GetSubID(),m_wCurrentSequence);
		if (pSR && pSR->IsSuspect())
		{
			m_waSequencesToDelete.Add(pSR->GetSequenceNr());
		}
		m_Sequences.Unlock();

		// nun löschen 
		m_dwStartDeleting = GetTickCount();
		ChangePlayStatus(PS_DELETING);
		RequestDeleteSequences(m_waSequencesToDelete);
		m_csSequencesToDelete.Unlock();

		DoRedraw();
	} // IsAlarm
	else
	{
		ChangePlayStatus(PS_STOP);
	}

	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
__int64 CPlayWindow::CalcBackup(const CSystemTime& stStart,
							    const CSystemTime& stEnd,
							    CDWordArray& dwaSequences)
{
	// Actualize or Request New Sequence List!
	RequestSequenceList(TRUE);

	__int64 iRet = 0;

	m_Sequences.Lock(_T(__FUNCTION__));
	CSystemTime stSequenceStartTime,stSequenceEndTime;

	TRACE(_T("%s -> %s\n"),stStart.GetDateTime(),stEnd.GetDateTime());
	WK_TRACE(_T("CalcBackup(): Cam: %s\n"), GetName());
	for (int i=0;i<m_Sequences.GetSize();i++)
	{
		stSequenceStartTime = m_Sequences.GetAtFast(i)->GetTime();
		if (i+1<m_Sequences.GetSize())
		{
			stSequenceEndTime = m_Sequences[i+1]->GetTime();
		}
		else
		{
			// assume last sequence is still recording
			stSequenceEndTime.GetLocalTime();
		}

		TRACE(_T("test  %s->%s\n"),stSequenceStartTime.GetDateTime(),
			stSequenceEndTime.GetDateTime());

		if (   (stSequenceStartTime <= stEnd)
			&& (stSequenceEndTime >= stStart))
		{
			TRACE(_T("added %s -> %s\n"),
				stSequenceStartTime.GetDateTime(),
				stSequenceEndTime.GetDateTime());
			iRet += m_Sequences.GetAtFast(i)->GetSize().GetInt64();

			WK_TRACE(_T("CalcBackup(): SeqNr: %d  mb: %s\n"),
				m_Sequences.GetAtFast(i)->GetSequenceNr(), 
				CIPCInt64(m_Sequences.GetAtFast(i)->GetSize().GetInt64()).Format(TRUE));

			dwaSequences.Add(MAKELONG(m_Sequences.GetAtFast(i)->GetSequenceNr(),GetNr()));
		}
	}
	m_Sequences.Unlock();

	return iRet;
}
/////////////////////////////////////////////////////////////////////////////
void CPlayWindow::CalcBackupAlarm(CDWordArray& dwaSequences)
{
	RequestSequenceList(TRUE);

	m_Sequences.Lock(_T(__FUNCTION__));
	// ist ein Alarmarchiv
	if (m_Sequences.GetSize() > 0)
	{
		// es gibt überhaupt Sequenzen
		CIPCSequenceRecord* pCurrentSequence = NULL;
		int iIndex = -1;
		for (int i=0;i<m_Sequences.GetSize();i++)
		{
			if (m_Sequences.GetAtFast(i)->GetSequenceNr() == m_wCurrentSequence)
			{
				pCurrentSequence = m_Sequences.GetAtFast(i);
				iIndex = i;
				break;
			}
		}

		if (pCurrentSequence)
		{
			CWordArray waSequencesToBackup;
			if (pCurrentSequence->IsSafeRing())
			{
				CalcPreAlarm(iIndex,waSequencesToBackup);
			}
			else
			{
				CalcAlarm(iIndex,waSequencesToBackup);
			}
			CSystemTime BackupAlarmStart;
			CSystemTime BackupAlarmEnd;
			for (int i=0;i<waSequencesToBackup.GetSize();i++)
			{

				WORD wSequenceNr = waSequencesToBackup[i];
				DWORD dwID = MAKELONG(wSequenceNr,GetNr());
				dwaSequences.Add(dwID);
				CIPCSequenceRecord* pCIPCSequenceRecord;
				pCIPCSequenceRecord = m_Sequences.GetSequence(GetNr(),wSequenceNr);
				
				if (pCIPCSequenceRecord)
				{
					if (i==0) //AlarmBackup End Time
					{
						BackupAlarmEnd = pCIPCSequenceRecord->GetTime();
					}
					if (i== (waSequencesToBackup.GetSize()-1)) //AlarmBackup Start Time
					{
						BackupAlarmStart = pCIPCSequenceRecord->GetTime();
					}
				}
			}
			if(BackupAlarmEnd < BackupAlarmStart)
			{
				CSystemTime s;
				s = BackupAlarmEnd;
				BackupAlarmEnd = BackupAlarmStart;
				BackupAlarmStart = s;
			}
		}

	} // Sequenzen
	m_Sequences.Unlock();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::UpdatePanel()
{
	CPanel* pPanel = theApp.GetPanel();
	if (!pPanel)
		return FALSE;

//TODO TKR bei SyncPlay muss das PanelMenu auch aktualisiert werden, wenn
	//das aktive Fenster klein ist
/*
	BOOL bSync = theApp.GetPanel()->CanSyncPlay();
	if(bSync)
	{
		//bei SyncPlay immer das Panel updaten, da die Buttons gleichzeitig für alle PlayWindows stehen
		pPanel->UpdateMenu();
	}
	else
	{
		// Update Menu calls Update Buttons too !!
		if (pPanel->GetActiveCamera() == (GetNr() & (MAX_CAM-1)))
		{
			pPanel->UpdateMenu();
		}
	}
	TRACE(_T("+++++++++ active Cam: %d  CurrPlayWnd: %d \n"), pPanel->GetActiveCamera(), (GetNr() & (MAX_CAM-1)));
*/
	// Update Menu calls Update Buttons too !!
	if (pPanel->GetActiveCamera() == (GetNr() & (MAX_CAM-1)))
	{
		pPanel->UpdateMenu();
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CPlayWindow::SetRectPlayStatus(RectPlayStatus rectplaystatus)
{
	m_RectPlayStatus = rectplaystatus;
}

/////////////////////////////////////////////////////////////////////////////
void CPlayWindow::InitRectQueryMembers()
{
	//members for rectangle search
	m_bPlayNextRect = TRUE;
	m_bStopRectPlay = FALSE;
	m_bIsNextForwardQueryRectResult = TRUE;
	m_bIsNextBackQueryRectResult = TRUE;
	m_bCanTashaRectangleSearch = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CPlayWindow::SetQueryRectSystemTime(CSystemTime stQueryRect)
{
	m_stQueryRect = stQueryRect;
}

/////////////////////////////////////////////////////////////////////////////
void CPlayWindow::SetNextQueryRectResult(BOOL bForward, BOOL bBack)
{
	m_bIsNextForwardQueryRectResult = bForward;
	m_bIsNextBackQueryRectResult = bBack;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::CanMilliSeconds()
{
	return m_bCanMilliSeconds;
}
/////////////////////////////////////////////////////////////////////////////
CPlayRealTime* CPlayWindow::GetPlayRealTime()
{
	if (m_pPlayRealTime == NULL)
	{
		m_pPlayRealTime	= new CPlayRealTime();
	}
	return m_pPlayRealTime;
}
/////////////////////////////////////////////////////////////////////////////
long CPlayWindow::OnPlayRealTime(WPARAM wParam, LPARAM lParam)
{
	WORD wArchivNr		= 0;
	WORD wSequenceNr	= 0;
	DWORD dwRecordNr	= 0;
	DWORD dwNrOfRecords	= 0;
	const CIPCPictureRecord* ppict = NULL;
	int iNumFields		= 0; 
	const CIPCFields* pfields = NULL;
	
	if (GetPlayRealTime())
	{
		m_pPlayRealTime->GetRealTimePicture(wArchivNr, wSequenceNr, dwRecordNr, dwNrOfRecords,
			ppict, iNumFields, pfields);

		//TODO TKR fieldarray dynamisch an größe anpassen
		CIPCField fields[100];

		if(iNumFields > 0)
		{
			fields[1] = *pfields->GetAtFast(0); 

			for(int i = 2; i < iNumFields; i++)
			{
				fields[i] = *pfields->GetAtFast(i-1);
			}
		}

		RecordNr(wSequenceNr,dwRecordNr,dwNrOfRecords,*ppict,iNumFields,fields);
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::ContainsAudio()
{
	return m_bContainsAudio;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::DecodeMpeg(CIPCPictureRecord pict, DWORD dwX, DWORD dwY)
{
	BOOL bDecode = FALSE;
	// TODO! RKE: Decode in PlayWindowThread (decouple cipc thread)
	if (m_pMpeg)
	{
		bDecode = m_pMpeg->DecodeMpeg4FromMemory((BYTE*)pict.GetData(),
						pict.GetDataLength());
	}
	PostMessage(WM_USER, ID_CHANGE_VIDEO);

	return bDecode;
}

//TODO TKR für Sync Abspielen
/*
/////////////////////////////////////////////////////////////////////////////
void CPlayWindow::SetSyncBigOne(BOOL bCanBigOne)
{
	m_bCanMakeSyncBigOne = bCanBigOne;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPlayWindow::CanMakeSyncBigOne()
{
	return m_bCanMakeSyncBigOne;
}
*/

