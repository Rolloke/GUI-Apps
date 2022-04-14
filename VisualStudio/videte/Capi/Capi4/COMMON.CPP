/////////////////////////////////////////////////////////////////////////////////
// common.cpp
// Inhält Routinen die von Capi 1.1 und Capi 2.0 unabhängig sind

#include <afxwin.h>         // MFC core and standard components
#include "DecodeCapiMessages.h"

#include <ctype.h>
#include <dos.h>

#include "hdr.h"
#include "capi4.h"

#include "AbstractCapi.h"
#include "wkclasses\WK_Trace.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// OLD version.h
#define rmj		1
#define rmm		0
#define rup		2
#define szVerName	_T("Capi4 by videte IT")

#ifdef _CAPI20_
#include "20main.h"
#endif

#define CHB 1

static HWND hWndGlobal = NULL; 
static DEFHDR Hdr;		

void CCp::OnCapiDataSend(BYTE byService)
{ 
	DEFHDR Hdr;
	Hdr.m_byID = ID_CAPI;
	Hdr.byChannelNr = (BYTE)m_CC.GetChannel();
	Hdr.wDataLen = 0;
	Hdr.byService = byService;
	Hdr.byNr = 0;	// set later in OnSend;

	switch (Hdr.byService)
	{           
		case CAPI_CHECKCONNECTION:
		case CAPI_CONNECTIONOK:
			// PutData is below
			break;
		default:               
			return;
	}
	if (!m_DosQ1.PutData(&Hdr, NULL)) {
//		OutputDebugString("IDIP3DosQ1.PutData failed\n");
	}
}

void CCp::OnCapiDataReceive(LPDEFHDR lpHdr)
{           
	UINT uCh = m_CC.GetChannel();
	m_CC.SetChannel(lpHdr->byChannelNr);                           
	switch (lpHdr->byService)
	{           
		case CAPI_CHECKCONNECTION:          
			OnCapiDataSend(CAPI_CONNECTIONOK); 
			break;
		case CAPI_CONNECTIONOK:
			break;
		default:
			WK_TRACE_ERROR(_T("Invalid CAPI_XXX %d\n"),lpHdr->byService);
	}				     
	m_CC.SetChannel(uCh);
}

void CCp::RealCheckForOldReceivedBlocks()
{
	if (m_waitQueue.GetSize() && m_bChannelBundeling  )
	{       
		for (int i=0;i<m_waitQueue.GetSize();i++)
		{
			CQueueBlock *pQueueBlock = m_waitQueue[i];
			LPDEFHDR lpDHdr = (LPDEFHDR) pQueueBlock->pData;           
			if (lpDHdr->byNr==m_byReceiveNr)
			{
				// CHB WK_TRACE(_T("Q>%3i ReceiveNr %3i\n"), lpDHdr->byNr, m_byReceiveNr);
				if (lpDHdr->m_byID==ID_CAPI)
				{
					OnCapiDataReceive(lpDHdr);
				}
				else
				{
					if (m_pWrapper)
					{// NEW 100397 HEDU
						m_pWrapper->OnReceiveData(
									0,	// CapiHandle ch, 
									lpDHdr	// DEFHDR *pData, 
									);

					}
					else
					{// old style via message
						SendMessage(m_hWndParent, m_uMsgCommand, C_DATA, (LPARAM)lpDHdr);
					}
				}
				m_byReceiveNr++;
				m_waitQueue.RemoveAt(i);
				i=0-1;	// restart loop pre-add i++
				WK_DELETE(pQueueBlock);
			}
		}	// end of loop over waitQueue
	}
}

void CCp::ReceiveData(LPDEFHDR lpHdr)
{   
	lpHdr->byChannelNr = (BYTE)m_CC.GetChannel();
	
	
	switch (lpHdr->m_byID)
	{
		case ID_CAPI:
		case ID_CIPC:
		case ID_CIPC_COMP:
		case ID_CIPC_COMP_SEG:
			break;
		default:
		SendMessage(m_hWndParent, m_uMsgCommand, C_UNKNOWNDATA, (LPARAM)lpHdr);	 
		// OOPS whow can delete that data
		// OOPS what kind of destructor
		m_byReceiveNr++;	// OOPS
		return;
	}
  	// Daten untersuchen.  Ist das die erwartete Block ?
	if (lpHdr->byNr==m_byReceiveNr)                                           
	{
		CheckForOldReceivedBlocks();

		// CHB WK_TRACE(_T("~%3i ReceiveNr %3i\n"), lpHdr->byNr, m_byReceiveNr);

		if (lpHdr->m_byID==ID_CAPI)
		{
			OnCapiDataReceive(lpHdr);
		}
		else
		{
			if (m_pWrapper)
			{
				m_pWrapper->OnReceiveData(
								0,	// CapiHandle ch, 
								(DEFHDR *)lpHdr	// DEFHDR *pData, 
								);
			}
			else
			{
				// old style
				SendMessage(m_hWndParent, m_uMsgCommand, C_DATA, (LPARAM)lpHdr);
			}
		}
		m_byReceiveNr++;

		CheckForOldReceivedBlocks();

	}
	else if (m_bChannelBundeling)
	{           
		// add data to waitQueue
		LPBYTE p = (LPBYTE)lpHdr;
		// NOT YET use minimal block size
		CQueueBlock *pNewBlock = new CQueueBlock(MY_BLK_LEN);
		pNewBlock->FillFromHeader((LPDEFHDR)lpHdr, &p[HDRSIZE]);
		m_waitQueue.Add(pNewBlock);
		// CHB WK_TRACE(_T("+%3i ReceiveNr %3i\n"), lpHdr->byNr, m_byReceiveNr);
		CheckForOldReceivedBlocks();
	}
}

BOOL CCp::SendData(LPDEFHDR lpHdr, LPBYTE lpData, int iPrior)
{
	if (lpHdr)
	{
		if (iPrior==PRIOR_HIGH)
		{
			return m_DosQ1.PutData(lpHdr, lpData);	/// <<< EXIT >>>
		}
		if (m_CC.Active())
		{       
			// NEW ignow iPrior, mean DosQ2 and DosQ3 are merged
			if (m_DosQ3.GetValidBuffers()<DOSQ3MAX-1)
			{	// OOPS TESTHACK -1
				return m_DosQ3.PutData(lpHdr, lpData);
			}
			else
			{
				// OOPS no free buffers or invalid prior
			}
		}          
	} 
	return FALSE;
}

void CCp::DisconnectAllActiveChannels()
{
	if (!m_CC.Active()) 						// Falls keine vollst. Verb. ist, und diese Kanal jetzt
	{										// abgebaut wird, alle andere Kanäle die aktive Aufbau                  
		UINT uChannel = m_CC.GetChannel();	// haben, abbauen.
		for (UINT i=0;i<NR_B_CHANNELS;i++)                      
		{
			m_CC.SetChannel(i);
			if (!m_CC.AbbauAktiv() && m_CC.AufbauAktiv() && m_CC.Cmd(i)>0)
			{
				::PostMessage(m_hWnd, CAPI_CMD, MAKEWPARAM(REQ, DISCONNECT), (LPARAM)i);
				m_CC.SetAbbau(TRUE);
			}
		}
		m_CC.SetChannel(uChannel);
	}
}

void CCp::DoConnectStuff()
{
	m_DosQ1.Reset(); 
	m_DosQ3.Reset(); 
	m_waitQueue.DeleteAll();
	m_byReceiveNr = 0; 
	m_bySendNr = 0;
	m_iReqOpen = 0;
	m_iNrTraBlocks = -((int)m_wMyMaxMovingBlocks); 	// erst minus damit immer blocks unterwegs sind.
	m_iNrRecBlocks = 0;
}

void CCp::SendInfo()
{       
#if _DEBUG
	CLimitedString sInfo(Decode_Info(m_wInfo));
	WK_TRACE(_T("Capi info: %s\n"), LPCTSTR(sInfo));
#endif

	if (m_pWrapper)
	{
		switch (m_wInfo)
		{
		case INF_20_CALL_REJECTED:
			m_pWrapper->OnCallRejected();
		break;
		case INF_20_USER_BUSY:
			m_pWrapper->OnUserBusy();
		break;
		case INF_CONN_NOT_POSSIBLE:
			m_pWrapper->OnUnableToConnect();
			break;
		default:
			;	// see below
		}	// end of switch over m_wInfo

		// OnInfo is always called !
		m_pWrapper->OnInfo(m_wInfo);
	}
	else
	{
		// no wrapper, old style via messages
		if (m_wInfo==INF_20_CALL_REJECTED)
		{
			SendMessage(m_hWndParent, m_uMsgCommand, C_CALL_REJECTED, 0L);
		}
		else if (m_wInfo==INF_20_USER_BUSY)
		{
			SendMessage(m_hWndParent, m_uMsgCommand, C_USER_BUSY, 0L);
		}
	}

	m_wInfo = 0;
}

void CCp::DoDisconnectStuff()
{                             
	TRACE(_T("### DoDisconnectStuff()\n"));
	SendMessage(m_hWndParent, m_uMsgCommand, C_CHANNEL_HANGUP, (LPARAM)m_CC.TelNumber());
	m_CC.DestroyChannel();
	::PostMessage(m_hWndParent, m_uMsgCommand, C_NR_CONNECT, (LPARAM)m_CC.GetConnections());

	if (m_CC.GetConnections()==0)
	{
		m_waitQueue.DeleteAll();
		m_DosQ1.Reset();
		m_DosQ3.Reset();
		m_CC.Reset();
		m_wProtocol = PROT_NOHANDSAKE;                          
		m_wMaxMovingBlocks = m_wMyMaxMovingBlocks;
		::PostMessage(m_hWndParent, m_uMsgCommand, C_HANGUP, 0L);
	}
}


void CCp::SetPasswort(LPCTSTR pId)
{                    
	if (pId==NULL)
	{
		m_sId.Empty();
	}
	else
	{
		ASSERT(_tcslen(pId) < BUFFER_32);
		m_sId.SetText(pId);
	}
}

CString CCp::GetCapi4Version()
{
	CString sResult;
	sResult.Format(_T("%s Version %1i.%2i.%2i"), szVerName, rmj, rmm, rup); 
	return sResult;
}

void CCp::StartTimer()
{
	if (m_CC.CountConnections()>0) 
	{
		KillTimer(m_hWnd, CAPI_TIMERID);
	}
	if (SetTimer(m_hWnd, CAPI_TIMERID, CAPITIMER, NULL)==NULL) 
	{
		WK_TRACE(_T("CAPI:StartTimer: SetTimer failed\n"));
	}
}

BOOL CCp::Open(LPCTSTR sAddress, WORD wBCh)
{
	if (sAddress == NULL || wBCh == 0)
	{
		UINT i;
		for (i=1;i<NR_B_CHANNELS;i++)
		{ 
			m_CC.SetChannel(i);
			if (m_CC.GetChannel() != i) 
			{
				::PostMessage(m_hWnd, CAPI_CMD, MAKEWPARAM(REQ, CONNECT), (LPARAM)i);
				m_bDisconnectOnlyOne = TRUE;
				break;
			}
		}

		return FALSE;
	}
	m_bDisconnectOnlyOne = FALSE;
	// wBCh has to be greater-or equal 1 and less-or-equal m_wMacBCh
	m_wMaxBChannelsWanted = min(wBCh, m_wMaxBCh);

	int iLen = _tcslen(sAddress);
	for (int i=0 ; i<iLen ; i++)
	{
		if (isdigit(sAddress[i])==0)
			return FALSE;
	}          
	ASSERT(iLen < BUFFER_64);
	m_sBuffer.SetText(sAddress);

	if ( m_CC.CountConnections()!=0)
	{
		WK_TRACE(_T("m_CC.CountConnections() !=0 beim CCp::Open\n"));
		return FALSE;
	}

	if (::PostMessage(m_hWnd, CAPI_CMD, MAKEWPARAM(REQ, CONNECT), (LPARAM)m_CC.GetChannel()))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CCp::Close(BOOL bCloseOnlyOne/*=FALSE*/)
{
	m_bDisconnectOnlyOne = FALSE;
	int i=0;
	if (bCloseOnlyOne)
	{
		i = 1;
	}
	for (;i<NR_B_CHANNELS;i++)
	{ 
		m_CC.SetChannel(i);
		if (m_CC.Cmd(i)>0 && (!m_CC.AbbauAktiv() || m_CC.Subcmd()!=REQ)) 
		{
			::PostMessage(m_hWnd, CAPI_CMD, MAKEWPARAM(REQ, DISCONNECT), (LPARAM)i);
			if (bCloseOnlyOne)
			{
				m_bDisconnectOnlyOne = TRUE;
				break;
			}
		}
	}
}

static BOOL DoDelay();

int CCp::Poll()
{
	WORD wLen;
	CDataQ*	lpDosQ;
	LPBYTE lpData;     
	LPDEFHDR lpHdr;
	int iNumActions=0;


#if 0
	m_wCallbackCounter++;
	// if there are messages handle up to 10 then continue
	if (OnCallback(0,0L) && m_wCallbackCounter<5)
	{
		return;
	}
#else
	BOOL bGot;

	int iNumMaxRetries = 5+GetNumOpenRequests();
	for (int iNumRetries=0;iNumRetries<iNumMaxRetries;iNumRetries++)
	{
		// OnCallback returns FALSE if the queue is empty
		bGot = OnCallback();
		if (bGot==FALSE)
		{
			// DoDelay();
		}
		else
		{
			iNumActions++;
		}
	}
#endif
	// outer loop tries to reduce reaction time REQ IND RESP CONF
	// by trying it more often before thre thread fall into a sleep
	for (int x=0;x<10;x++)	{ // outer repeat loop

	BOOL bSendOkay=TRUE;
	while (bSendOkay && m_CC.GetConnections() && m_iReqOpen<MAX_OPEN_REQ)
	{                                                  				
		lpData = m_DosQ1.GetData(wLen);
		if (lpData)
		{	// data in m_DosQ1
			lpDosQ = &m_DosQ1; 
			lpHdr = (LPDEFHDR)lpDosQ->GetHeader();                
			if (lpHdr->m_byID== ID_CAPI)
			{
				m_CC.SetChannel(lpHdr->byChannelNr);
			}
		}
		else
		{	// no dosQ1 data
			BOOL bFine = m_CC.SetBestChannel();
			if (bFine)
			{
				if (m_iNrTraBlocks>=(int)m_wMaxMovingBlocks)
				{
					WK_TRACE(_T("Can't send %d >= %d\n"),
							m_iNrTraBlocks,m_wMaxMovingBlocks
							);
					return iNumActions;	// <<< EXIT >>>
				}
				else if ((lpData = m_DosQ3.GetData(wLen))!=NULL)
				{
					lpDosQ = &m_DosQ3;
				}
				else
				{
					// no data at all ?
					return iNumActions;	// <<< EXIT >>>
				}
				lpHdr = (LPDEFHDR)lpDosQ->GetHeader();                
			}
			else
			{
				// SetBestChannel failed OOPS
				// WK_STAT_PEAK(_T("Reset"),1,_T("SetBestChannelFailed"));
				return iNumActions;
			}
		}
		if (m_CC.GetBuf()>=m_wXBuffers)
		{ // Für jeden Kanal immer ein Buffer frei, deswegen -1
			WK_TRACE(_T("No xbuffers\n"));
			return iNumActions;             
		}

		lpHdr->byNr = m_bySendNr;		// Nr für channelbundeling.
		if (OnSendDataToCapi(lpData, wLen))
		{
			m_CC.IncBuf();                                     
			lpDosQ->MarkBuffer();
			m_bySendNr++;	// HERE increment sendNr
			m_iReqOpen++;
			bSendOkay=TRUE;
		}
		else
		{
			// Send data failed
			bSendOkay=FALSE;
		}
	}	// end of while, still free open requests

	
	// aftermath, check for received answers right now
	for (int iNumRetriesAfter=0;m_iReqOpen && iNumRetriesAfter<5;iNumRetriesAfter++)
	{
		// OnCallback returns FALSE if the queue is empty
		bGot = OnCallback();
		if (bGot==FALSE)
		{
			DoDelay();
		}
		else
		{
			iNumActions++;
		}
	}

		// some kind od delay but no thread switch please
	for (int d=0;m_iReqOpen && d<100;d++)
	{
		DoDelay();
	}
	}	// end of outer repeat loop over x

	return iNumActions;
}

static BOOL DoDelay()
{
#if 0
	for (double dd=0.0;dd<500;dd+=1.23)
	{
	}
	return (dd!=0);
#else
	// sigh, forget about Delay, ProcessorUsage, increaes too much
	return TRUE;
#endif
}
LONG CALLBACK CapiWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	LONG lResult;

	CCp* lpCapi = (CCp*)GetWindowLong(hWnd, 0);
	switch ( message )
	{                                
		case WM_CREATE:
			hWndGlobal = hWnd;
			lResult = 0L;
			break;
		case WM_TIMER: 	// Kommt jede 2te sekunde.
			if (lpCapi->m_CC.CountConnections()>0)
			{
				UINT uChannel = lpCapi->m_CC.GetChannel();             
				for (UINT i=0;i<NR_B_CHANNELS;i++)
				{
					lpCapi->m_CC.SetChannel(i);            
					BYTE byCmd = lpCapi->m_CC.Cmd(i);
					if (byCmd)
					{
						LONG lTimer = lpCapi->m_CC.GetTimer();
						if (lpCapi->IsRawDataEnabled()==FALSE)
						{
							// iST protocol
							if (lTimer<=TIME_CHECK && lpCapi && lpCapi->m_CC.PasswortOk())
							{
								lpCapi->OnCapiDataSend(CAPI_CHECKCONNECTION); 
							}
							if (lTimer<=TIME_TIMEOUT)
							{
								WK_TRACE(_T("Channel timeout, disconnecting...\n"));
								lpCapi->CapiCmd(DISCONNECT,REQ);
							}
							if (lTimer<=TIME_REEBOOT && lpCapi)
							{
								// NEW 060597 message added
								WK_TRACE(_T("Channel fatal timeout, disconnecting...\n"));
								lpCapi->Reset();
								::PostMessage(lpCapi->m_hWndParent, lpCapi->m_uMsgCommand, C_NODISCONNECT, (LPARAM)lpCapi->m_CC.TelNumber());
							}
						}
						else
						{
							// raw data mode
							// NOT YET any timeout check
						}
						lpCapi->m_CC.SetTimer(lTimer-CAPITIMER);
					}
					else
					{
						// no cmd, no check
					}
				}   
				lpCapi->m_CC.SetChannel(uChannel);
			}
			else if (KillTimer(hWnd, (UINT)wParam) == FALSE)
			{
				WK_TRACE_ERROR(_T("CAPI WM_TIMER: KillTimer failed\n"));
			}
			lResult=0L;
			break;
		case CAPI_CMD:   
			if (wParam)
			{
				lpCapi->m_CC.SetChannel((UINT)lParam);  
				return (lpCapi->CapiCmd((BYTE)HIWORD(wParam), (BYTE)LOWORD(wParam)));
			}
			lResult=0L;
			break;
		default:
			lResult = ( DefWindowProc ( hWnd, message, wParam, lParam ) );
	} /* switch message */

	return lResult;
}     

long CCp::DoExCapiWndProcCapiCmdHandling(WPARAM wParam, LPARAM lParam)
{
	m_CC.SetChannel((UINT)lParam);  
	return (CapiCmd((BYTE)HIWORD(wParam), (BYTE)LOWORD(wParam)));
}

