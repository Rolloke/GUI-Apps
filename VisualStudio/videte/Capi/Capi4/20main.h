/* GlobalReplace: m_dwTmpMsgDataLen --> m_dwTmpMsgMaxDataLen */
////////////////////////////////////////////////////////////////////////////
// capiwnd.h

#ifndef _header20main_h
#define _header20main_h

#include 	"20def.h"
#include 	"dataq.h"
#include 	"20conn.h"
#include 	"20capi.h"
#include "20info.h"

#include "wk.h"
#include "wkclasses\wk_trace.h"
#include "AbstractCapi.h"

////////////////////////////////////////////////////////////////////////////
// defines                  
#define CAPI_CMD        	 	WM_USER + 100


#define 	MAX_MOVING_BLOCKS	10		// Wenn iNrRecBlocks MAX_MOVING_BLOCKS erreicht hat, carry on senden.

// und laegere waitq
#define		DOSQ1LEN			32
#define		DOSQ3LEN			32
#define 	DOSQ3MAX			32
#define		MAX_OPEN_REQ		7 // original 4, war aber schon in Hawks Version nicht benutzt 


// Protocol's defines
#define		PROT_NOHANDSAKE		0


// Childfenster ID's
// seems to be independend of traffic OOPS
#define 	CAPITIMER			(1000*60)
#define		TIME_CHECK			(1000*60*3)

#define		TIME_AUFBAU			30000
#define		TIME_STATUS			(1000*60*3)

#define 	TIME_TIMEOUT		0
#define 	TIME_REEBOOT		-60000 
#define 	CAPI_TIMERID		1555



////////////////////////////////////////////////////////////////////////////
// funktions
extern LONG CALLBACK CapiWndProc(HWND hWnd,UINT message, WPARAM wParam,LPARAM lParam);

////////////////////////////////////////////////////////////////////////////
// class CCapi

// GF OOPS
// The whole class is only used as CMyParamsQueue m_paramsQueue
// without any usage, just .SetAutoDelete(TRUE)...
class CMyParams {
	CMyParams();
	CMyParams(WPARAM wp, LPARAM lp);
	//
	inline WPARAM GetWPARAM() const;
	inline LPARAM GetLPARAM() const;
private:
	WPARAM m_wparam;
	LPARAM m_lparam;
};

typedef CMyParams *CMyParamsPtr;
WK_PTR_LIST_CS(CMyParamsQueuePlain,CMyParamsPtr,CMyParamsQueue);

inline WPARAM CMyParams::GetWPARAM() const
{ return m_wparam; }
inline LPARAM CMyParams::GetLPARAM() const
{ return m_lparam; }


class CCp
{       
public:
	CCp(CAbstractCapiClient *pWrapper=NULL);	// OBSOLETE ?
	~CCp();

	int	 Create(HWND hWnd, UINT uMsgCommand, WORD wMaxBCh, WORD wXBuffers, 
			WORD wMyProtocol, WORD wMyMaxMovingBlocks, 
			BOOL bChannelBundeling,
			BOOL bClientWantsRawData);
	void SetPasswort(LPCTSTR szId);
	BOOL Open(LPCTSTR sAddress, WORD wBCh); 
	void Close(BOOL bCloseOnlyOne=FALSE);
	UINT Reset(BOOL bShutDown=FALSE);
	int Poll();
	inline BOOL OnCallback();
	BOOL ListenRequest(LPCTSTR pOwnNumber, BYTE byController=0, DWORD dwServiceMask=CAPI20_CIP_UNREST_DIGITAL, DWORD dwInfoMask=0);
	LONG CapiCmd(BYTE cmd, BYTE subcmd);
	inline BOOL CapiActive();
	BOOL	SendData(LPDEFHDR lpHdr, LPBYTE lpData=NULL, int iPrior=PRIOR_LOW);
	LPCTSTR	GetVersion();
	CString GetCapi4Version();
	CLimitedString GetManufacturer();
	inline int GetNumOpenRequests() const;
	void	OnCapiDataSend(BYTE byService);
	BOOL	OnSendDataToCapi(LPBYTE lpData, WORD wLen);
	void	SetConnectionTimeout(LONG lTimeout) { m_lTimeout = lTimeout; }
	LONG	GetConnectionTimeout() { return m_lTimeout; }

	//
	void EnableRawData(BOOL bEnable);
	inline BOOL IsRawDataEnabled() const;
public:
	CConnect	m_CC;
	HWND		m_hWndParent;
	UINT		m_uMsgCommand;
	
private:
	BOOL RealOnCallback(LPVOID pVoid);
	long DoExCapiWndProcCapiCmdHandling(WPARAM wParam, LPARAM lParam);
	// Ersatz für altes Hawksches Handling in CapiWndProc
	// Eingeführt um die PostMessages in Funktionsaufrufe umsetzen zu können

	inline void OnDataB3Conf(WORD wNr);
	void OnConnectInd();
	void StartTimer();
	void SetDefault();
	void NewSetDefault();
	BOOL SendToCapi();
	void DoDisconnectStuff();
	void DoConnectStuff(); 
	void ReceiveData(LPDEFHDR lpHdr);   
	WORD NextMsgNr()		{ return ((m_wReqCnt>=MAXINT)?(m_wReqCnt=0):(++m_wReqCnt)); }
	void OnCapiDataReceive(LPDEFHDR lpHdr);
	void DisconnectAllActiveChannels();
	void SendInfo();
	//
	inline void CheckForOldReceivedBlocks();
	void RealCheckForOldReceivedBlocks();
	BOOL IsRealOwnCallingPartyNumber();

private:
	BOOL		m_bChannelBundeling; 
	WORD		m_wMyMaxMovingBlocks;// Max Anzahl von Blocks die unterwegs sein durfen.
	WORD		m_wMaxMovingBlocks;	// Max Anzahl von Blocks die unterwegs sein durfen.
	int			m_iNrTraBlocks;		// Anzahl von Blocks die unterwegs sind. 
	int 		m_iNrRecBlocks;		// Anzahl von empfangenen Blocks.


	BYTE		m_byReceiveNr;
	BYTE		m_bySendNr;

	WORD		m_wReqCnt;
	WORD   		m_wIndCnt;		// INDication-Counter
	WORD		m_wInfo;
	WORD		m_wMaxBCh;
	WORD		m_wMaxBChannelsWanted;
	WORD		m_wProtocol;
	WORD		m_wMyProtocol;
	DWORD		m_dwApplID;		// Von CAPI-TSR vergebene (api_register(..)) Applikationsnummer
	CAPI20_MSG	m_PutM;			// Aktuelle zu versendende Message an CAPI-TSR 
	LPCAPI20_MSG	m_pMsg;			// Aktuelle von CAPI-TSR empfangene Message

	CAPI20_MSG	m_TmpMsg;
	BYTE		*m_pTmpMsgData;
	DWORD		m_dwTmpMsgMaxDataLen;

	BYTE		m_byLastBlockNr;
	CDataQ		m_DosQ1; 
	CDataQ		m_DosQ3; 
	// DOROPPED CDataQ		m_WaitQ; 
	CQueueBlockArray m_waitQueue;	// NEW HEDU 310397
    TIMERPROC	m_lpCapiTimerProc;
    UINT		m_uTimerID;
	CLimitedString64 m_sBuffer;
	CLimitedString64 m_sId;
	CLimitedString32 m_sOwnNumber;

	int 		m_iReqOpen;
	BOOL		m_bCreateOk;
	WORD		m_wXBuffers;
	HWND		m_hWnd;     
	DWORD		m_dwController;		
	DWORD		m_dwInfoMask;

	LONG		m_lTimeout;
	//
	CAbstractCapiClient *m_pWrapper;	// NULL by default
	BOOL		m_bClientWantsRawData;
	BOOL		m_bCapiDebugTrace;
	BOOL		m_bDisconnectOnlyOne;

// GF OOPS This member is not used anywhere else, only .SetAutoDelete(TRUE)
	CMyParamsQueue	m_paramsQueue;
};

inline int CCp::GetNumOpenRequests() const
{
	return m_iReqOpen;
}
inline BOOL CCp::CapiActive()
{
	if (m_CC.CountConnections()>0) {
		return TRUE;
	}
	return FALSE;
}

inline BOOL CCp::OnCallback()
{
	LPVOID pVoid = NULL;

	m_wInfo = (WORD)CAPI_GET_MESSAGE(m_dwApplID, &pVoid);

	if (m_wInfo==INFO_MSG_Q_EMPTY)
	{
		// empty queue
		return FALSE;
	}
	else if (m_wInfo==INFO_MSG_ILL_APPL_NR)
	{
		WK_TRACE_ERROR(_T("Invalid ID in GET_MESSAGE\n"));
		if (m_pWrapper)
		{
			m_pWrapper->OnCapiGetMessageError();
		}
		return FALSE;
	}
	else if (pVoid==NULL) 
	{
		WK_TRACE_WARNING(_T("No data in CAPI_GET_MESSAGE\n"));
		if (m_pWrapper) 
		{
			m_pWrapper->OnCapiGetMessageError();
		}
		return FALSE;
	}
	else
	{
		return RealOnCallback(pVoid);
	}
}

inline void CCp::CheckForOldReceivedBlocks()
{
	if (m_waitQueue.GetSize()) {
		RealCheckForOldReceivedBlocks();	// call non inlined fn
	}
}


inline void CCp::OnDataB3Conf(WORD wNr)
{
	m_iReqOpen--;
	if (m_iReqOpen<0){
		WK_TRACE_ERROR(_T("m_iReqOpen < 0\n"));
		m_iReqOpen = 0;
	}
	m_CC.DecBuf();

	if (m_bClientWantsRawData==FALSE) {
		if (!m_DosQ3.FreeBuffer((BYTE)wNr))
		{
			if (!m_DosQ1.FreeBuffer((BYTE)wNr))
			{
				WK_TRACE_ERROR(_T("DataB3Conf: FreeBuffer not found\n"));
			} 

		}
	} else {
		// OOPS is it that simple ? Only counting request
	}
}

inline BOOL CCp::IsRawDataEnabled() const
{
	return m_bClientWantsRawData;
}

#endif