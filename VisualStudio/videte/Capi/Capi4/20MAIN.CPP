/* GlobalReplace: m_dwTmpMsgDataLen --> m_dwTmpMsgMaxDataLen */
#include <afxwin.h>         // MFC core and standard components

#include <ctype.h>
#include <dos.h>
#include <stdlib.h>

#include "DecodeCapiMessages.h"
#include 	"capi4.h"
#include 	"hdr.h"
#include 	"dataq.h"
#include	"20main.h"
#include	"20info.h"
#include	"20capi.h"
#include    "20msg.h"
#include 	"string.h"

#include "wkclasses\WK_Trace.h"

#include "wkclasses\WK_Profile.h"
#include "wkclasses\WK_Util.h"

#include <string.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
// Modul Statics
static  _TCHAR 		szCapiClass[]	= _T("capi4wnd");     
static	HMODULE		hTheDll			= NULL;
static	DWORD		theDwError		= 0;

///////////////////////////////////////////////////////////////////////////
// Modul Funktions
//////////////////////////////////////////////////////////////////////////////
// pointers to functions globals!
CAPI_REGISTER_PTR pCAPI_REGISTER=NULL;
CAPI_RELEASE_PTR pCAPI_RELEASE=NULL;
CAPI_PUT_MESSAGE_PTR pCAPI_PUT_MESSAGE=NULL;
CAPI_GET_MESSAGE_PTR pCAPI_GET_MESSAGE=NULL;
CAPI_WAIT_FOR_SIGNAL_PTR pCAPI_WAIT_FOR_SIGNAL=NULL;
CAPI_GET_MANUFACTURER_PTR pCAPI_GET_MANUFACTURER=NULL;
CAPI_GET_VERSION_PTR pCAPI_GET_VERSION=NULL;
CAPI_GET_SERIAL_NUMBER_PTR pCAPI_GET_SERIAL_NUMBER=NULL;
CAPI_GET_PROFILE_PTR pCAPI_GET_PROFILE=NULL;
CAPI_INSTALLED_PTR pCAPI_INSTALLED=NULL;
//////////////////////////////////////////////////////////////////////////////
void DllError(LPCTSTR sError)
{
	WK_TRACE(_T("No function %s\n"), LPCTSTR(sError));	// NOT YET release
}
//////////////////////////////////////////////////////////////////////////////
DWORD APIENTRY CAPI_REGISTER (DWORD MessageBufferSize,DWORD maxLogicalConnection, 
							  DWORD maxBDataBlocks,DWORD maxBDataLen,DWORD * pApplID)
{
	if (pCAPI_REGISTER) {
		return pCAPI_REGISTER(MessageBufferSize,maxLogicalConnection,
						maxBDataBlocks, maxBDataLen, pApplID);
	} else {
		DllError(_T("CAPI_REGISTER"));
		return 0xffff;
	}
}
//////////////////////////////////////////////////////////////////////////////
DWORD APIENTRY CAPI_RELEASE (DWORD ApplID)
{
	if (pCAPI_RELEASE ) {
		return pCAPI_RELEASE(ApplID); 
	} else {
		DllError(_T("CAPI_RELEASE "));
		return 0xffff;
	}
}
//////////////////////////////////////////////////////////////////////////////
DWORD APIENTRY CAPI_WAIT_FOR_SIGNAL (DWORD ApplID)
{
	if (pCAPI_WAIT_FOR_SIGNAL ) {
		return pCAPI_WAIT_FOR_SIGNAL(ApplID);
	} else {
		DllError(_T("CAPI_WAIT_FOR_SIGNAL "));
		return 0xffff;
	}
}
//////////////////////////////////////////////////////////////////////////////
VOID APIENTRY CAPI_GET_MANUFACTURER (char * SzBuffer)
{
	if (pCAPI_GET_MANUFACTURER ) {
		pCAPI_GET_MANUFACTURER(SzBuffer); 
	} else {
		DllError(_T("CAPI_GET_MANUFACTURER "));
	}
}
//////////////////////////////////////////////////////////////////////////////
DWORD APIENTRY CAPI_GET_VERSION (DWORD * pCAPIMajor,DWORD * pCAPIMinor,
								 DWORD * pManufacturerMajor,DWORD * pManufacturerMinor)
{
	if (pCAPI_GET_VERSION ) {
		return pCAPI_GET_VERSION(pCAPIMajor,pCAPIMinor,
								 pManufacturerMajor,pManufacturerMinor
								);
	} else {
		DllError(_T("CAPI_GET_VERSION "));
		return 0xffff;
	}
}
//////////////////////////////////////////////////////////////////////////////
DWORD APIENTRY CAPI_GET_SERIAL_NUMBER (char * SzBuffer)
{
	if (pCAPI_GET_SERIAL_NUMBER ) {
		return pCAPI_GET_SERIAL_NUMBER(SzBuffer); 
	} else {
		DllError(_T("CAPI_GET_SERIAL_NUMBER "));
		return 0xffff;
	}
}
//////////////////////////////////////////////////////////////////////////////
DWORD APIENTRY CAPI_GET_PROFILE (PVOID SzBuffer,DWORD CtrlNr )
{
	if (pCAPI_GET_PROFILE) {
		return pCAPI_GET_PROFILE(SzBuffer,CtrlNr);
	} else {
		DllError(_T("CAPI_GET_PROFILE"));
		return 0xffff;
	}
}
//////////////////////////////////////////////////////////////////////////////
DWORD APIENTRY CAPI_INSTALLED (VOID)
{
	if (pCAPI_INSTALLED) {
		return pCAPI_INSTALLED();
	} else {
		DllError(_T("CAPI_INSTALLED"));
		return 0xffff;
	}
}
//////////////////////////////////////////////////////////////////////////////
BOOL LoadDll()
{
	hTheDll = LoadLibrary(_T("Capi2032.dll"));
	    
    if (hTheDll== NULL) 
	{
		WK_TRACE(_T("Could not load capi2023.dll\n"));
	} 
	else 
	{
		void *pProc;
		pProc = ::GetProcAddress(hTheDll,"CAPI_REGISTER");
		pCAPI_REGISTER= (CAPI_REGISTER_PTR)pProc;
		//
		pProc = ::GetProcAddress(hTheDll,"CAPI_RELEASE");
		pCAPI_RELEASE= (CAPI_RELEASE_PTR)pProc;
		//
		pProc = ::GetProcAddress(hTheDll,"CAPI_PUT_MESSAGE");
		pCAPI_PUT_MESSAGE = (CAPI_PUT_MESSAGE_PTR)pProc;
		//
		pProc = ::GetProcAddress(hTheDll,"CAPI_GET_MESSAGE");
		pCAPI_GET_MESSAGE= (CAPI_GET_MESSAGE_PTR)pProc;
		//
		pProc = ::GetProcAddress(hTheDll,"CAPI_WAIT_FOR_SIGNAL");
		pCAPI_WAIT_FOR_SIGNAL = (CAPI_WAIT_FOR_SIGNAL_PTR)pProc;
		//
		pProc = ::GetProcAddress(hTheDll,"CAPI_GET_MANUFACTURER");
		pCAPI_GET_MANUFACTURER= (CAPI_GET_MANUFACTURER_PTR)pProc;
		//
		pProc = ::GetProcAddress(hTheDll,"CAPI_GET_VERSION");
		pCAPI_GET_VERSION= (CAPI_GET_VERSION_PTR)pProc;
		//
		pProc = ::GetProcAddress(hTheDll,"CAPI_GET_SERIAL_NUMBER");
		pCAPI_GET_SERIAL_NUMBER= (CAPI_GET_SERIAL_NUMBER_PTR)pProc;
		//
		pProc = ::GetProcAddress(hTheDll,"CAPI_GET_PROFILE");
		pCAPI_GET_PROFILE= (CAPI_GET_PROFILE_PTR)pProc;
		//
		pProc = ::GetProcAddress(hTheDll,"CAPI_INSTALLED");
		pCAPI_INSTALLED = (CAPI_INSTALLED_PTR)pProc;

		DWORD dwOK = CAPI_INSTALLED();
		if (dwOK == 0)
		{
			WK_TRACE(_T("Capi installed\n"));

			char szManu[64] = {0};
			CAPI_GET_MANUFACTURER(szManu);
			CLimitedString sManu(szManu);
			WK_TRACE(_T("Capi Manufacturer: %s\n"), LPCTSTR(sManu));

			DWORD dwCapiMajor=0, dwCapiMinor=0, dwManuMajor=0, dwManuMinor=0;
			CAPI_GET_VERSION(&dwCapiMajor, &dwCapiMinor, &dwManuMajor, &dwManuMinor);
			WK_TRACE(_T("Capi Version     : Capi %d.%d ; Manufacturer  %d.%d\n"),
							dwCapiMajor, dwCapiMinor, dwManuMajor, dwManuMinor);

			char szSerial[8] = {0};
			CAPI_GET_SERIAL_NUMBER(szSerial);
			CLimitedString sSerial(szSerial);
			WK_TRACE(_T("Capi Serial No.  : %s\n"), LPCTSTR(sSerial));
		}
		else
		{
			WK_TRACE_ERROR(_T("Capi NOT installed %04x\n"), dwOK);
		}
	}

	return (hTheDll!=NULL);    	
}
//////////////////////////////////////////////////////////////////////////////
void UnloadDll()
{
	if (hTheDll) {
		FreeLibrary(hTheDll);
		hTheDll=NULL;
	} else {
		WK_TRACE_ERROR(_T("Invalid unload Capi\n"));
	}
}
//////////////////////////////////////////////////////////////////////////////
// Funktionen für CCp class
CCp::CCp(CAbstractCapiClient *pWrapper/*=NULL*/)
{
	m_pWrapper = pWrapper;
	m_bChannelBundeling = TRUE;
	m_wMaxBCh = MY_N_B3_CON;
	m_dwApplID = 0;
	m_bClientWantsRawData = FALSE;
	m_byReceiveNr = 0;
	m_bySendNr = 0; 
	m_iReqOpen = 0;
	m_wReqCnt = MAXINT;
	m_wIndCnt = MAXINT;             

	m_wMaxMovingBlocks = MAX_MOVING_BLOCKS;
	m_wMyMaxMovingBlocks = MAX_MOVING_BLOCKS;
	m_iNrTraBlocks = -((int)m_wMyMaxMovingBlocks);
	m_iNrRecBlocks = 0; 
	m_wMyProtocol = PROT_NOHANDSAKE;
	m_wProtocol = PROT_NOHANDSAKE;
	m_dwController = 1L; 
	m_dwInfoMask = 0L;
	m_bCreateOk = FALSE;
    m_wXBuffers = MY_N_XBUF;   
	m_hWnd = NULL;
	m_uMsgCommand = 0;    
	m_hWndParent = NULL;

	m_lTimeout = TIME_STATUS;
	//@INIT m_sBuffer
	//@INIT m_sId

	m_pTmpMsgData = NULL;
	m_dwTmpMsgMaxDataLen=0;

// GF OOPS This member is not used anywhere else!
	m_paramsQueue.SetAutoDelete(TRUE);

	CWK_Profile wkp;
	m_bCapiDebugTrace = TRUE;
	m_bCapiDebugTrace = (BOOL)wkp.GetInt(_T("ISDNUnit\\Debug"), _T("CapiDebugTrace"), m_bCapiDebugTrace);
	wkp.WriteInt(_T("ISDNUnit\\Debug"), _T("CapiDebugTrace"), m_bCapiDebugTrace);
	WK_TRACE(_T("DoCapiDebugTrace %i\n"), m_bCapiDebugTrace); 
	m_bDisconnectOnlyOne = FALSE;
}
//////////////////////////////////////////////////////////////////////////////
LPCTSTR CCp::GetVersion()
{
	CString sVersion;
	if (m_dwApplID)
	{
		DWORD w0=0;
		DWORD w1=0;
		DWORD w2=0;
		DWORD w3=0;
		CAPI_GET_VERSION(&w0, &w1, &w2, &w3);
		sVersion.Format(_T("CAPI %i.%i  Manufacturer %i.%i"), w0, w1, w2, w3);
	}
	return sVersion;
}
//////////////////////////////////////////////////////////////////////////////
CLimitedString CCp::GetManufacturer()
{              
   CLimitedString sManufacturer;
	if (m_dwApplID)
	{
		char tmp[64+10];	// as defined in the spec
		CAPI_GET_MANUFACTURER(tmp);
		sManufacturer.SetText(tmp);
	}
	return sManufacturer;
}
//////////////////////////////////////////////////////////////////////////////
static DWORD CapiRegister(void *goo, DWORD dwMessageBufferSize,
				DWORD dwMaxB, DWORD dwXBuffers,
				DWORD *pDW)
{
	BYTE tmpArrayA[100];
	BYTE tmpArrayB[100];
	void *tmp=goo;
	BYTE ti;
	for (ti=0;ti<100;ti++) 
	{
		tmpArrayA[ti]=ti;
		tmpArrayB[ti]=ti;
	}
	theDwError = CAPI_REGISTER(	dwMessageBufferSize,
									dwMaxB,
									dwXBuffers,
									MY_BLK_LEN,
									pDW);         
	
	for (ti=0;ti<100;ti++) 
	{
		if (tmpArrayA[ti]!=ti) 
		{
			WK_TRACE(_T("Tmp error A at %d, found %x\n"),(int)ti, (int)tmpArrayA[ti]);
		}
		if (tmpArrayB[ti]!=ti) 
		{
			WK_TRACE(_T("Tmp error B at %d, found %x\n"),(int)ti, (int)tmpArrayB[ti]);
		}
	}
	if (tmp!=goo) 
	{
		WK_TRACE(_T("smashed foo\n"));
	}	
	return theDwError;
}
//////////////////////////////////////////////////////////////////////////////
UINT CCp::Reset(BOOL bShutdown/*=FALSE*/)
{       
	if (m_dwApplID!=0)
	{                           
		DWORD dwID = m_dwApplID;
		m_wInfo = (WORD)CAPI_RELEASE(dwID);
		if (m_wInfo)
		{
			SendInfo();
		}
		m_dwApplID = 0;
		if (bShutdown)
		{
			return 0;
		}
	}
	DWORD dwMaxB = m_wMaxBCh;
	DWORD dwXBuffers = m_wXBuffers;
	DWORD dwMessageBufferSize = MY_N_MSG+(1024L*m_wMaxBCh);
	DWORD dwError;
	BYTE ti;
	BYTE tmpArray[100];
	DWORD dw= 0;
	for (ti=0;ti<100;ti++) 
	{
		tmpArray[ti]=ti;
	}
	dwError = CapiRegister(this, dwMessageBufferSize,
					dwMaxB,
					dwXBuffers,
					&dw);      
	dwError = theDwError;
	
	for (ti=0;ti<100;ti++) 
	{
		if (tmpArray[ti]!=ti)
		{
			WK_TRACE(_T("WK_Tmp error at %d, found %x\n"),(int)ti, (int)tmpArray[ti]);
		}
	}
	m_dwApplID = dw;
	// UNUSED CCp* pTemp = this;
	if (dwError) 
	{
		WK_TRACE_ERROR(_T("CAPI Reset: Apiregfailure\n")); 
	}
	else          
	{
		NewSetDefault();
	}
	return (UINT)dwError;
}
//////////////////////////////////////////////////////////////////////////////
int CCp::Create(HWND hWnd, UINT uMsgCommand, 
				WORD wMaxBCh, WORD wXBuffers, 
				WORD wMyProtocol, WORD wMyMaxMovingBlocks, 
				BOOL bChannelBundeling,
				BOOL bClientWantsRawData
				)
{                   
	WNDCLASS wc;
	
	wc.style				=	CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		=	CapiWndProc;
	wc.cbClsExtra		=	0;
	wc.cbWndExtra		=	sizeof(this);
	wc.hInstance		=	(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
	wc.hIcon				=	NULL;
	wc.hCursor			=	LoadCursor ( NULL, IDC_ARROW );
	wc.hbrBackground	=	(HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wc.lpszMenuName	=	NULL;
	wc.lpszClassName	=	szCapiClass;
	if (!RegisterClass(&wc))
	{
		DWORD dwError = GetLastError();
		// special case 1410 class already exists is OK
		if (dwError != ERROR_CLASS_ALREADY_EXISTS)
		{
			WK_TRACE_ERROR(_T("RegisterClass FAILED %s %lu %s\n"), szCapiClass, dwError, GetLastErrorString(dwError));
			return 10;
		}
	}

	WK_TRACE(_T("RegisterClass %s\n"), szCapiClass);

	// if registered, create window, so it can be unregistered at the destructor!
	m_hWnd = CreateWindow(szCapiClass,_T(""),WS_CHILD, 0, 0, 200, 100, hWnd, NULL, 
		(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);

	if (m_hWnd==NULL) {
		return -1;   
	}
	SetWindowLong(m_hWnd, 0, (LONG)this);

	m_uMsgCommand = uMsgCommand;
	m_hWndParent = hWnd;
	m_wMaxBCh = wMaxBCh; 
	m_wXBuffers = wXBuffers;
	m_wMyProtocol = wMyProtocol;                                  
	m_wMyMaxMovingBlocks = wMyMaxMovingBlocks;              
	m_bChannelBundeling = bChannelBundeling;
	m_bClientWantsRawData = bClientWantsRawData;
	//-------------------- DOS-Speicher alloziieren ----------------------
	if (!m_DosQ1.Create	(DOSQ1LEN, 	MY_BLK_LEN)	)	
		return 2;                                           
	if (!m_DosQ3.Create	(DOSQ3LEN, 	MY_BLK_LEN)	)	
		return 4;                                           
	//--------------------------------------------------------------------
	if (!m_CC.Create(m_wXBuffers))
		return 9;	
	int iRet = Reset();
	if (iRet)
		return iRet;

	return 0;
}
//////////////////////////////////////////////////////////////////////////////
CCp::~CCp()
{
	SetDefault();                  

	if (m_dwApplID!=0)
	{
		DWORD dwID = m_dwApplID;
		CAPI_RELEASE(dwID);
	}
	m_waitQueue.DeleteAll();
	
	WK_DELETE_ARRAY(m_pTmpMsgData);
	m_dwTmpMsgMaxDataLen=0;

	if (m_hWnd)
	{
		HINSTANCE hInstance = (HINSTANCE)GetWindowLong(m_hWnd, GWL_HINSTANCE);
		DestroyWindow(m_hWnd);
		if (UnregisterClass(szCapiClass, hInstance))
		{
			WK_TRACE(_T("UnregisterClass %s\n"), szCapiClass);
		}
		else
		{
			WK_TRACE_ERROR(_T("UnregisterClass FAILED %s\n"), szCapiClass);
		}
	}
	if (m_hWndParent)
	{
		DestroyWindow(m_hWndParent);
	}
}
//////////////////////////////////////////////////////////////////////////////
BOOL CCp::ListenRequest(LPCTSTR pOwnNumber, BYTE byController, DWORD dwServiceMask, DWORD dwInfoMask)
{                        
	if (pOwnNumber == NULL)
	{
		m_sOwnNumber.SetText(LISTEN_REJECTALL);
	}
	else
	{
		ASSERT(_tcslen(pOwnNumber) < BUFFER_32);
		m_sOwnNumber.SetText(pOwnNumber);
	}
	m_dwController = (DWORD)byController;	
	m_dwInfoMask = dwInfoMask;
	if (m_CC.CountConnections()==0)
	{
		return (BOOL)(::PostMessage(m_hWnd, CAPI_CMD, MAKEWPARAM(REQ,LISTEN), 0L));
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////
BOOL CCp::SendToCapi()
{			
	WORD wCmd = 0;
	DWORD dwPlci = 0;
	if (m_bCapiDebugTrace)
	{
		wCmd = MAKEWORD(m_PutM.subcommand, m_PutM.command);
		switch (wCmd)
		{
			case MAKEWORD(REQ, DATA_B3):	// Send and receive cmds
			case MAKEWORD(RESP, DATA_B3):
			if ((m_bCapiDebugTrace & 2) == 0)
			{
				wCmd = 0;
			} break;
			default:						// other cmds
			if (m_bCapiDebugTrace & 1)
			{	// eg. connection and disconnection
				dwPlci = m_PutM.body.conInd.dwPlci;
			}
			else
			{	
				wCmd = 0;
			} break;
		}
	}
	if (wCmd)
	{
		CLimitedString sSub(Decode_Sub(m_PutM.subcommand));
		CLimitedString sCmd(Decode_Command(m_PutM.command));
		WK_TRACE(_T("SendToCapi %s %s (%x)\n"), LPCTSTR(sSub), LPCTSTR(sCmd), dwPlci);
	}
	DWORD dwID = m_dwApplID;
	m_wInfo = (WORD)CAPI_PUT_MESSAGE(dwID, &m_PutM);
	if (m_wInfo)
	{                    
		WK_TRACE_ERROR(_T(" CAPI_PUT_MESSAGE failed %X\n"), m_wInfo);
		WK_STAT_PEAK(_T("Reset"),2,_T("PutMessage"));
		SendInfo();
		return FALSE;
	}
	else 
	{
		//WK_STAT_PEAK(_T("Reset"),1,_T("PutMessage"));
		return TRUE;
	}
}               
//////////////////////////////////////////////////////////////////////////////
BOOL CCp::OnSendDataToCapi(LPBYTE lpData, WORD wLen)
{                                    
	if (lpData)	
	{  
		m_PutM.wMessagenumber			= 	NextMsgNr();
		m_PutM.wTotal_length			=	CAPI20_HDRLEN + sizeof(CAPI20_DATA_B3_REQ_BODY);
		CheckIfWord(m_dwApplID);
		m_PutM.wApplid					=	(WORD)m_dwApplID;	// OOPS HEDU
		m_PutM.command					=	DATA_B3;
		m_PutM.subcommand				=	REQ;
		m_PutM.body.dataB3Req.dwNcci	=	m_CC.Ncci();
		m_PutM.body.dataB3Req.flags	=	0;
		m_PutM.body.dataB3Req.number	=	m_bySendNr;

		if (wLen>MY_BLK_LEN) 
		{
			WK_TRACE_ERROR(_T("Block size error %d>%d\n"),wLen,MY_BLK_LEN),
			m_PutM.body.dataB3Req.length	=	MY_BLK_LEN;
		}
		else 
		{
			m_PutM.body.dataB3Req.length	=	wLen;
		}
		m_PutM.body.dataB3Req.ptr		=	lpData;
		// WK_TRACE(_T("DataB3Req Len %d Nr %d\n"),m_PutM.body.dataB3Req.number);

		BOOL bOkay = SendToCapi();
		if (bOkay && m_bClientWantsRawData) 
		{
			m_iReqOpen++;
		}
		return bOkay;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////////
// called from inlined OnCallback, if there is something to handle
BOOL CCp::RealOnCallback(LPVOID pVoid)
{              
	//WK_STAT_PEAK(_T("Reset"),1,_T("CapiGetMessage"));

    m_pMsg = (LPCAPI20_MSG)pVoid;
	if (m_bCapiDebugTrace & 4)
	{
		WK_TRACE(_T("GET_MESSAGE returned %x cmd %x sub %x\n"),
			m_wInfo, m_pMsg->command, m_pMsg->subcommand);
	}
////////////////////////////////////////////////////////////////////////
//	_IND Message			Anwendung  <<<  CAPI
////////////////////////////////////////////////////////////////////////		
	WORD wCmd = 0;
	DWORD dwPlci = 0;
	if (m_bCapiDebugTrace)
	{
		wCmd = MAKEWORD(m_pMsg->subcommand, m_pMsg->command);
		switch (wCmd)
		{
			case MAKEWORD(IND, DATA_B3):	// Send and receive cmds
			case MAKEWORD(CONF, DATA_B3):
			if ((m_bCapiDebugTrace & 2) == 0)
			{
				wCmd = 0;
			} break;
			default:						// other cmds
			if (m_bCapiDebugTrace & 1)
			{	// eg. connection and disconnection
				dwPlci = m_pMsg->body.conInd.dwPlci;
			}
			else
			{	
				wCmd = 0;
			} break;
		}
	}
	if (wCmd)
	{
		CLimitedString sSub(Decode_Sub(m_pMsg->subcommand));
		CLimitedString sCmd(Decode_Command(m_pMsg->command));
		WK_TRACE(_T("ReceiveFromCapi: %s %s (%x)\n"), LPCTSTR(sSub), LPCTSTR(sCmd), dwPlci);
	}

	if (m_pMsg->subcommand == IND)	
	{
		m_wIndCnt = m_pMsg->wMessagenumber;
		switch (m_pMsg->command)
		{	
			case CONNECT:			
				OnConnectInd();
				return TRUE;
			case CONNECT_ACTIVE:	
				CapiCmd( CONNECT_ACTIVE, RESP );		
				return TRUE;
			case CONNECT_B3:		
				CapiCmd( CONNECT_B3, RESP );			
				return TRUE;
			case CONNECT_B3_ACTIVE:	
				CapiCmd( CONNECT_B3_ACTIVE, RESP );	
				return TRUE;
			case DATA_B3:			
				// WK_TRACE(_T("DataB3Ind  %d\n"),m_pMsg->body.dataB3Ind.number);
				CapiCmd( DATA_B3, RESP );			
				return TRUE;
			case DISCONNECT:		// DISCONNECT_IND
				m_wInfo = m_pMsg->body.discInd.wReason;
				if (m_wInfo)
				{
					SendInfo();
				}
				CapiCmd(DISCONNECT, RESP);	// falls plci falsch, trotzdem antworten.
				return TRUE;
			case DISCONNECT_B3:		// DISCONNECT_B3_IND
				m_wInfo = m_pMsg->body.discB3Ind.wReason;
				if (m_wInfo)
				{
					SendInfo();
				}
				CapiCmd( DISCONNECT_B3, RESP ); 
				return TRUE;
			case RESET_B3:		// RESET_B3_IND
				// OOPS gf was not handled before 20010305
				CapiCmd( RESET_B3, RESP ); 
				return TRUE;
			default:
			{
				// GET_PARAMS      			0x06	not at CAPI 2.0
				// INFO                    	0x08
				// FACILITY					0x80
				// LISTEN_B3       			0x81	not at CAPI 2.0
				// CONNECT_B3_T90_ACTIVE	0x88
				// MANUFACTURER        		0xFF
				CLimitedString sCmd(Decode_Command(m_pMsg->command));
				WK_TRACE_ERROR(_T("Unknown IND Message command 0x%02x %s, close connection\n"),
									m_pMsg->command, LPCTSTR(sCmd));
				Close();
				return TRUE;
			}
		} 	
	}
	else 			
	////////////////////////////////////////////////////////////////////////////////
	//	_CONF Message			Anwendung  <<<  CAPI
	////////////////////////////////////////////////////////////////////////////////
	{
		switch ( m_pMsg->command )
		{
			case LISTEN:            // LISTEN_CONF
				m_wInfo = m_pMsg->body.lisConf.info;
 				if ( m_wInfo != 0)
				{
					Reset();
					::PostMessage(m_hWnd, CAPI_CMD, MAKEWPARAM(REQ, LISTEN), 0L);
				}
				::PostMessage(m_hWndParent, m_uMsgCommand, C_LISTENREQUEST, (LPARAM)m_wInfo);
				break;
				
			case CONNECT:  			// CONNECT_CONF   PLCI wird geliefert.
				m_wInfo = m_pMsg->body.conConf.info;                              
				if (m_wInfo)
				{   
					SendInfo();
					DoDisconnectStuff();
					break; 
				}
				if (m_CC.CreateChannel(	m_sBuffer,					// tel Number 
										m_pMsg->body.conConf.dwPlci, // plci
										TRUE, 						// Aufbau Aktiv
										FALSE, 						// kein Reject
										TIME_AUFBAU))
				{   
					StartTimer();
					m_CC.SetCommands(CONNECT, CONF);
					if (m_CC.GetConnections()==1)
					{
						::PostMessage(m_hWndParent,m_uMsgCommand,C_OUTGOING,(LPARAM)LPCTSTR(m_sBuffer));	// OOPS HEDU
					}
					::PostMessage(m_hWndParent, m_uMsgCommand, C_NR_CONNECT, (LPARAM)m_CC.GetConnections());
				}
				else
				{
					Close();
				}
				break;
			case CONNECT_B3:		// CONNECT_B3_CONF
				m_wInfo = m_pMsg->body.conB3Conf.info;
				if ( m_wInfo==0 && m_CC.FindPlci(m_pMsg->body.conB3Conf.dwNcci) && m_CC.Cmd()==CONNECT_B3 && m_CC.Subcmd()==REQ)
				{
					m_CC.SetNewNcci(m_pMsg->body.conB3Conf.dwNcci);
					m_CC.SetCommands(CONNECT_B3, CONF);
					if (m_pWrapper)
					{
						m_pWrapper->OnConnectB3();
					}
				}
				else 
				{
					WK_TRACE_ERROR(_T("CONNECT_B3_CONF Error, close connection\n"));
					Close();
				}
				break;
			case DISCONNECT:		// DISCONNECT_CONF 
				m_wInfo = m_pMsg->body.discConf.info;
				if (m_CC.FindPlci(m_pMsg->body.discConf.dwPlci))
				{                                   
					m_CC.SetCommands(DISCONNECT, CONF);
					m_CC.SetAbbau(TRUE);
				}
				break;
			case DISCONNECT_B3:		// DISCONNECT_B3_CONF
				m_wInfo = m_pMsg->body.discB3Conf.wInfo;
				if ( m_wInfo==0 && m_CC.FindNcci(m_pMsg->body.discB3Conf.dwNcci) && m_CC.Cmd()==DISCONNECT_B3 && m_CC.Subcmd()==REQ)
				{
					m_CC.SetCommands(DISCONNECT_B3, CONF);
				}
				else
				{
					WK_TRACE_ERROR(_T("DISCONNECT_B3_CONF Error\n"));
				}
				break;
			case DATA_B3:			// DATA_B3_CONF	Hier bestätigt CAPI die Sendung der Daten.
				//WK_TRACE(_T("DataB3Conf %d\n"),m_pMsg->body.dataB3Conf.number);
				m_wInfo = m_pMsg->body.dataB3Conf.info;
            if ( m_CC.FindNcci(m_pMsg->body.dataB3Conf.dwNcci) ) 
				{
					OnDataB3Conf(m_pMsg->body.dataB3Conf.number);
				}
				break;

			default:
			{
				// ALERT					0x01
				// GET_PARAMS      			0x06	not at CAPI 2.0
				// INFO                    	0x08
				// SELECT_B_PROTOCOL		0x41  
				// FACILITY					0x80
				// LISTEN_B3       			0x81	not at CAPI 2.0
				// RESET_B3					0x87 
				// MANUFACTURER        		0xFF
				CLimitedString sCmd(Decode_Command(m_pMsg->command));
				WK_TRACE_ERROR(_T("Unknown CONF Message command 0x%02x %s, close connection\n"),
									m_pMsg->command, LPCTSTR(sCmd));
				Close();
				return TRUE;
			}
		}  // switch für CONF
	} 
	if (m_wInfo != 0)
	{
		SendInfo();
	}
	return TRUE;
} // end of OnCallback
//////////////////////////////////////////////////////////////////////////
LONG CCp::CapiCmd(BYTE command, BYTE subcommand)
{
	if (m_bCapiDebugTrace & 4)
	{
		CLimitedString sSub(Decode_Sub(subcommand));
		CLimitedString sCmd(Decode_Command(command));
		WK_TRACE(_T("CapiCmd %s %s\n"), LPCTSTR(sSub), LPCTSTR(sCmd));
	}
	//	REQ Commands		Anwendung  >>>  CAPI
	//---------------------------------------------------------------------
	BOOL bAlreadySend=FALSE;

	if (subcommand==REQ)
	{
		switch (command)
		{
			case LISTEN:				// LISTEN_REQ 
				if (m_CC.CountConnections()>0)
				{
					return 0;
				}
				CheckIfWord(m_dwApplID);
				listen_req(	&m_PutM,
							(WORD)m_dwApplID,
							NextMsgNr(),
							m_dwController, 
							m_dwInfoMask,				// Infomask
							// OLD CAPI20_CIP_MASK_ANY|CAPI20_CIP_MASK_UNREST_DIGITAL,  // Dienst
							// NEW 090597 no speech connections please !
							(CAPI20_CIP_MASK_UNREST_DIGITAL | CAPI20_CIP_MASK_REST_DIGITAL),
							0L);						// Additional Services
				break;                           
				
			case CONNECT:			// CONNECT_REQ 
				{
					CheckIfWord(m_dwApplID);
					const char* szOwnNumber = "";
					if (IsRealOwnCallingPartyNumber())
					{
						szOwnNumber = (LPCSTR)m_sOwnNumber;
					}
					connect_req(&m_PutM,
								(WORD)m_dwApplID,
								NextMsgNr(),
								m_CC.GetNextController(),
								szOwnNumber,
								(const char*)m_sBuffer);
				} 
				break;
			    			            
			case CONNECT_B3:			// CONNECT_B3_REQ
				if (!m_CC.AbbauAktiv() && !m_CC.Reject() && m_CC.AufbauAktiv() && m_CC.Cmd()==CONNECT_ACTIVE && m_CC.Subcmd()==RESP)
				{
					connect_b3_req(&m_PutM,(WORD)m_dwApplID,NextMsgNr(),m_CC.Plci());
					m_CC.SetCommands(CONNECT_B3, REQ);
					break; 
				}
				return 0;
				
			case DISCONNECT:			// DISCONNECT_REQ 
				if (m_CC.GetConnections()>0 && m_CC.Cmd()>0)
				{
//					TRACE(_T("Diconnect: con:%d, c: %d, ai:%d\n"), m_CC.GetConnections(), m_CC.GetChannel(), m_dwApplID);
					m_CC.SetAbbau(TRUE);
					m_CC.SetCommands(DISCONNECT, REQ);
					disconnect_req ( &m_PutM, (WORD)m_dwApplID, NextMsgNr(), m_CC.Plci());
					break;
				}
				return 0;  
				              
			case DISCONNECT_B3:			// DISCONNECT_B3_REQ
				if (m_CC.GetConnections()>0 && m_CC.Cmd()==CONNECT_B3_ACTIVE)
				{
					m_CC.SetAbbau(TRUE);
					m_CC.SetCommands(DISCONNECT_B3, REQ);
					disconnect_b3_req ( &m_PutM,(WORD)m_dwApplID,NextMsgNr(),m_CC.Ncci());
					break;  
				}
				return 0;
							
			case DATA_B3:				// DATA_B3_REQ  Hier werden die Daten zu CAPI gesendet
				return 0;

			default:
			{
				// ALERT					0x01
				// GET_PARAMS      			0x06	not at CAPI 2.0
				// INFO                    	0x08
				// SELECT_B_PROTOCOL		0x41  
				// FACILITY					0x80
				// LISTEN_B3       			0x81	not at CAPI 2.0
				// RESET_B3					0x87 
				// MANUFACTURER        		0xFF
				CLimitedString sCmd(Decode_Command(command));
				WK_TRACE_ERROR(_T("Unknown REQ Message command 0x%02x %s, not handled\n"),
									command, LPCTSTR(sCmd));
				return 0;
			}
		}
	} 
	else
	{   
	/////////////////////////////////////////////////////////////////////////////
	//	RESP Commands		Anwendung  >>>  CAPI
	/////////////////////////////////////////////////////////////////////////////
		switch ( command )
		{
			case CONNECT:        	// CONNECT_RESP
				connect_resp(&m_PutM,(WORD)m_dwApplID,m_wIndCnt,m_pMsg->body.conInd.dwPlci,m_CC.Reject());
				break;
				
			case CONNECT_ACTIVE:  	// CONNECT_ACTIVE_RESP
				connect_active_resp(&m_PutM,(WORD)m_dwApplID,m_wIndCnt,m_pMsg->body.actInd.dwPlci);
				if (m_CC.FindPlci(m_pMsg->body.actInd.dwPlci))
				{
					if (   m_CC.AufbauAktiv()
						&& !m_CC.AbbauAktiv()
						&& m_CC.Cmd()==CONNECT
						&& m_CC.Subcmd()==CONF
						)
					{
						PostMessage(m_hWnd, CAPI_CMD, MAKEWPARAM(REQ,CONNECT_B3), (LPARAM)m_CC.GetChannel());
					}
					m_CC.SetCommands(CONNECT_ACTIVE, RESP);    
				}
				else 
				{ 
					WK_TRACE_ERROR(_T("CONNECT_ACTIVE_RESP bad plci, close connection\n"));
					Close();
				}
				break; 
				
			case DISCONNECT:		// DISCONNECT_RESP
				// fill m_PutM with response msg
				disconnect_resp(&m_PutM,(WORD)m_dwApplID,m_wIndCnt, m_pMsg->body.discInd.dwPlci);
				WK_TRACE(_T("DISCONNECT_RESP %x, reason %x numConn %d\n"),
					m_pMsg->body.discInd.dwPlci,
					m_pMsg->body.discInd.wReason,
					m_CC.GetConnections()
					);

				// is already handled in RealOnCallback which calls CapiCmd
//				if (m_pMsg->body.discInd.wReason)
//				{
//					m_wInfo = m_pMsg->body.discInd.wReason;	// OOPS; 
//					SendInfo();
//				}

				if ( m_CC.FindPlci(m_pMsg->body.discInd.dwPlci) )
				{
					m_CC.SetAbbau(TRUE);
					TRACE (_T("Diconnect Channel %d\n"), m_CC.GetChannel());
					if (!m_bDisconnectOnlyOne)
					{
						DisconnectAllActiveChannels();
					}
					DoDisconnectStuff();
				}
				else 
				{
					WK_TRACE_ERROR(_T("DISCONNECT_IND bad plci 0x%x\n"),m_pMsg->body.discInd.dwPlci);
				}
				break;
			case CONNECT_B3:		// CONNECT_B3_RESP
				connect_b3_resp(&m_PutM,(WORD)m_dwApplID,m_wIndCnt,m_pMsg->body.conB3Ind.dwNcci,m_CC.Reject());
				if (  m_CC.FindPlci(m_pMsg->body.conB3Ind.dwNcci)
					&& m_CC.Cmd()==CONNECT_ACTIVE
					&& m_CC.Subcmd()==RESP
					)
				{
					m_CC.SetNewNcci(m_pMsg->body.conB3Ind.dwNcci);	
					m_CC.SetCommands(CONNECT_B3, RESP);
				}
				else                                                       
				{
					WK_TRACE_ERROR(_T("CONNECT_B3_RESP Error, close connection\n"));
					Close();
				}
				break; 
				
			case CONNECT_B3_ACTIVE:	// CONNECT_B3_ACTIVE_RESP 
				connect_b3_active_resp(&m_PutM,(WORD)m_dwApplID,m_wIndCnt,m_pMsg->body.conB3ActInd.dwNcci);
				if (   m_CC.FindNcci(m_pMsg->body.conB3ActInd.dwNcci)
					&& m_CC.Cmd()==CONNECT_B3	// wird direkt von b3activeind gesetzt
					)
				{
					if (!m_CC.Active())
					{
						DoConnectStuff();
					}
					m_CC.SetCommands(CONNECT_B3_ACTIVE, RESP);
					// OLD if (m_sId.IsEmpty())
					// NEW no support for CAPI_ID, CAPI_IDOK, 
					// should be implemented at higher level
					{
						m_CC.SetPasswort(TRUE);
						// NEW 0804 don't forget the second channel
						if (m_CC.AufbauAktiv() && m_CC.GetConnections()<m_wMaxBChannelsWanted)
						{
							WK_TRACE(_T("ChannelBundling, try next channel\n"));
							::PostMessage( m_hWnd, CAPI_CMD, MAKEWPARAM(REQ,CONNECT), 0L); 
						}

						if (m_CC.AufbauAktiv())
						{
							PostMessage(m_hWndParent, m_uMsgCommand, C_OUTGOINGOK, (LPARAM)m_CC.TelNumber());
						}
						else
						{
							PostMessage(m_hWndParent, m_uMsgCommand, C_INCOMMINGOK, (LPARAM)m_CC.TelNumber());
						}
					}
				}
				else
				{
					WK_TRACE_ERROR(_T("CONNECT_B3_ACTIVE_IND Error, close connection\n"));	
					Close();
				}
				break;		

			case DISCONNECT_B3:	// DISCONNECT_B3_RESP 
				disconnect_b3_resp ( &m_PutM,(WORD)m_dwApplID,m_wIndCnt,m_pMsg->body.discB3Ind.dwNcci ); 
				if ( m_CC.FindNcci(m_pMsg->body.discB3Ind.dwNcci) )
				{
					// CAVEAT:
					// gf todo Disconnect_B3 kann auch mal zwischendurch kommen
					// mit direkt folgendem Connect_B3_Ind!
					m_CC.SetAbbau(TRUE);   
					m_CC.SetCommands(DISCONNECT_B3, RESP);
					if (!m_bDisconnectOnlyOne)
					{
						DisconnectAllActiveChannels();
					}
				}
				break;
				
			case DATA_B3:		// DATA_B3_RESP 
			{
				// WK_TRACE(_T("DataB3 len %d number %d\n"),m_pMsg->wTotal_length,m_pMsg->body.dataB3Ind.number);
				data_b3_resp(&m_PutM, (WORD)m_dwApplID, m_wIndCnt, m_pMsg->body.dataB3Ind.number, m_pMsg->body.dataB3Ind.dwNcci);
				// create a local copy  and answer as fast as possible
			/*
			typedef struct
			{
			   ULWORD       dwNcci ;  // network connection ident.
			   BYTE FAR*	ptr ;    // pointer to received data
			   UWORD       	data_length ;     // size of received B3 data block 
			   UWORD       	number ;     // datablock number 
			   UWORD       	flags ;
			} CAPI20_DATA_B3_IND_BODY;
			*/
				DWORD dwDataLength = m_pMsg->body.dataB3Ind.data_length;
				if (m_dwTmpMsgMaxDataLen<dwDataLength)
				{
					WK_DELETE_ARRAY(m_pTmpMsgData);
					m_dwTmpMsgMaxDataLen = dwDataLength;
					m_pTmpMsgData = new BYTE[m_dwTmpMsgMaxDataLen];

				}
				m_TmpMsg = *m_pMsg;	// copy message
				// copy data!
				CopyMemory(m_pTmpMsgData,m_pMsg->body.dataB3Ind.ptr,dwDataLength);
				// redirect data ptr in TmpMsg !
				m_TmpMsg.body.dataB3Ind.ptr = m_pTmpMsgData;

				SendToCapi();	// and send response
				bAlreadySend=TRUE;

				if (m_CC.FindNcci(m_TmpMsg.body.dataB3Ind.dwNcci) &&  m_CC.Cmd()==CONNECT_B3_ACTIVE)
				{
					if (m_CC.PasswortOk())
					{
						m_CC.SetTimer(m_lTimeout);
					}
					if (m_bClientWantsRawData==FALSE)
					{
						ReceiveData((LPDEFHDR)(m_TmpMsg.body.dataB3Ind.ptr));
					}
					else
					{
						if (m_pWrapper) 
						{
							// Raw data receive!
							CapiHandle ch=0;
							m_pWrapper->OnReceiveData(ch, m_pTmpMsgData,dwDataLength);
						}
						else
						{
							WK_TRACE_ERROR(_T("Raw data without wrapper\n"));
						}
					}
				}
			}
                break;

			case RESET_B3:	// RESET_B3
				// OOPS gf was not handled before 20010305
				reset_resp(&m_PutM,(WORD)m_dwApplID,m_wIndCnt,m_pMsg->body.resetInd.dwNcci);
				break;		

			default:
			{
				// GET_PARAMS      			0x06	not at CAPI 2.0
				// INFO                    	0x08
				// FACILITY					0x80
				// LISTEN_B3       			0x81	not at CAPI 2.0
				// CONNECT_B3_T90_ACTIVE	0x88
				// MANUFACTURER        		0xFF
				CLimitedString sCmd(Decode_Command(command));
				WK_TRACE_ERROR(_T("Unknown RESP Message command 0x%02x %s, not handled\n"),
									command, LPCTSTR(sCmd));
				return 0;
			}
		}
	}
	if (bAlreadySend==FALSE)
	{
		SendToCapi();
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////
void CCp::NewSetDefault()
{
	m_pMsg				= NULL;
	m_wIndCnt			= MAXINT;
	m_wReqCnt			= MAXINT;
	m_byReceiveNr 		= 0;
	m_bySendNr			= 0; 
	m_iReqOpen			= 0;

	m_iNrTraBlocks 		= -((int)m_wMyMaxMovingBlocks);
	m_iNrRecBlocks		= 0;
	m_dwController		= 1L;
	m_dwInfoMask 		= 0L;
	m_waitQueue.DeleteAll();
	m_DosQ1.Reset();
	m_DosQ3.Reset();
	m_CC.Reset(); 
}			
//////////////////////////////////////////////////////////////////////////////
void CCp::SetDefault()
{
	m_pMsg				= NULL;
	m_wIndCnt			= MAXINT;
	m_wReqCnt			= MAXINT;
	m_byReceiveNr 		= 0;
	m_bySendNr			= 0; 
	m_iReqOpen			= 0;

	m_iNrTraBlocks 		= -((int)m_wMyMaxMovingBlocks);
	m_iNrRecBlocks		= 0;
	m_dwController		= 1L;
	m_dwInfoMask 		= 0L;
	m_waitQueue.DeleteAll();
	m_DosQ1.Reset();
	m_DosQ3.Reset();
	m_CC.Reset(); 
}			
//////////////////////////////////////////////////////////////////////////////
void CCp::OnConnectInd()
{
	if (m_CC.GetConnections()==0 || !m_CC.FindPlci(m_pMsg->body.conInd.dwPlci))
	{
		CALLINFO s;                                                                   
		memset(&s, 0, sizeof(CALLINFO));
		//-----------------  Telefonnummer der Gegenstation ----------------
		LPCALLING_PARTY_NUM lpCallingParty = (LPCALLING_PARTY_NUM) &m_pMsg->body.conInd.rest[m_pMsg->body.conInd.rest[0]+1];
		// Telefonnummer Filter...
		int i = 0; 
		while (	lpCallingParty->bySize-2>i && 				// Angegebene Telefonnummerlänge
				isdigit(lpCallingParty->szNumber[i]) &&  	// Alles Ziffern ?
				i<30)                                   	// Obere Grenze (kein Nummer enthält mehr als 30 Ziffern)
			i++;     
		lstrcpynA(s.szCallingParty, lpCallingParty->szNumber, i+1);

#ifdef _DEBUG	// test no number transmitted
//		s.szCallingParty[0] = 0;
#endif

		//----------------------- Verbindungs-Nummer -----------------------
		LPCALLED_PARTY_NUM lpCalledParty = (LPCALLED_PARTY_NUM) &m_pMsg->body.conInd.rest[0];
		// Telefonnummer Filter...
		i = 0; 
		while (	lpCalledParty->bySize-1>i && 				// Angegebene Telefonnummerlänge
				isdigit(lpCalledParty->szNumber[i]) &&  	// Alles Ziffern ?
				i<30) {                                  	// Obere Grenze (kein Nummer enthält mehr als 30 Ziffern)
			i++; 
		}
		lstrcpynA(s.szCalledParty, lpCalledParty->szNumber, i+1);
#ifdef _DEBUG	// test no own number
//		s.szCalledParty[0] = 0;
#endif
		if (strlen(s.szCalledParty) == 0)
		{
			s.szCallingParty[0] = 0;
		}
		//----------------------- Reject untersuchen -----------------------
		WORD wReject = CAPI20_REJECT_ACCEPT;
		// NEW 020497 HEDU,
		if (m_pWrapper) 
		{
			CAbstractCapiCallInfo callInfo;
			callInfo.SetCallingParty(s.szCallingParty);
			callInfo.SetCalledParty(s.szCalledParty);

			CapiIncomingAnswer answer;
			answer = m_pWrapper->OnIncomingCall(0,callInfo);
			// convert to internal answer
			switch(answer)
			{
				case INCOMING_ACCEPTED:
					wReject = CAPI20_REJECT_ACCEPT;
				break;
				case INCOMING_WAITING:
					// NOT YET supported
					wReject = CAPI20_REJECT_CLEAR;
				break;
				case INCOMING_REJECTED:
					wReject = CAPI20_REJECT_CLEAR;
				break;
				case INCOMING_REJECTED_NO_CHANNEL:
					wReject = CAPI20_REJECT_NOCHANNEL;
				break;
				case INCOMING_REJECTED_IGNORE:
					wReject = CAPI20_REJECT_IGNORE;
				break;
				case INCOMING_REJECTED_BUSY:
					wReject = CAPI20_REJECT_BUSY;
				break;
				default:
					WK_TRACE_ERROR(_T("Invalid answer %d\n"),answer);
					wReject = CAPI20_REJECT_CLEAR;
			}	// end of convert switch
		}
		else 
		{		// no m_pWrapper oldStyle Message interface
			CString sCallingParty(s.szCallingParty);

			if (LPCSTR(m_sOwnNumber)[0] == 'A')
			{ 
				if (!SendMessage(m_hWndParent,m_uMsgCommand, C_INCOMMING, (LPARAM)(LPCALLINFO)&s))
				{
					wReject = CAPI20_REJECT_CLEAR;
				}
			}
			else if (lpCalledParty->bySize==0) 
			{
	    		wReject = CAPI20_REJECT_IGNORE;
			}
			else if (strstr(m_sOwnNumber, s.szCalledParty)==0)
			{ // OOPS strstr ??? HEDU
	    		wReject = CAPI20_REJECT_IGNORE; 
			}
			else if (m_CC.GetConnections() >= m_wMaxBCh)
			{
	    		wReject = CAPI20_REJECT_NOCHANNEL;
			}
			else if (   (strncmp(m_sOwnNumber, "R", 1) == 0) 
				     || (m_CC.GetConnections()!=0 && !m_CC.Active())
						||  m_CC.AufbauAktiv()
						|| (m_CC.GetConnections()!=0 && sCallingParty != m_CC.TelNumber()) 
						|| (m_CC.GetConnections()==0 && !SendMessage(m_hWndParent,m_uMsgCommand,C_INCOMMING, (LPARAM)(LPCALLINFO)&s)))
			{
	    		wReject = CAPI20_REJECT_BUSY;  								// mit 2 boards ist lstrcmp on telnr ein problem.
			}
		} // end of check for incoming (old style)
	    	
		if (wReject == CAPI20_REJECT_ACCEPT)
		{
			CLimitedString sCallingParty(s.szCallingParty);
			WK_TRACE(_T("Create new channel for %s\n"), LPCTSTR(sCallingParty));
			if (m_CC.CreateChannel(	s.szCallingParty,
									m_pMsg->body.conInd.dwPlci,
									FALSE,						// Aufbau Passiv
									FALSE,						// Kein Reject
									TIME_AUFBAU))
			{   
				m_CC.SetCommands(CONNECT, IND);
				StartTimer();
				CapiCmd( CONNECT, RESP );
			}
			else
			{
				WK_TRACE(_T("CreateChannel failed\n"));
				Close();                   
			}
		}
		else
		{	// so it's not REJECT_ACCEPT
			CLimitedString sCallingParty(s.szCallingParty);
			WK_TRACE(_T("REJECT %s [%d]\n"), LPCTSTR(sCallingParty), (int)wReject);
			// NOT YET any m_pWrapper action
			SendMessage(m_hWndParent,m_uMsgCommand,C_REJECTING, (LPARAM)(LPCALLINFO)&s);
			if (m_CC.CreateChannel(	s.szCallingParty,
			                        m_pMsg->body.conInd.dwPlci,
			                        FALSE,		// Aufbau Passiv
									wReject,	// Call Rejected
									TIME_AUFBAU))
			{   
				m_CC.SetCommands(CONNECT, IND);
				CapiCmd( CONNECT, RESP ); 
			}
			else
			{
				Close();
			}
		} 
		TRACE(_T("OnConnectInd(%d)\n"), m_CC.GetConnections());
		::PostMessage(m_hWndParent, m_uMsgCommand, C_NR_CONNECT, (LPARAM)m_CC.GetConnections());
	}
	else
	{	// Spezialfall
		// no connections or no FindPlci
		WK_TRACE_ERROR(_T("Internal error 7\n"));
		connect_resp(&m_PutM,(WORD)m_dwApplID,m_wIndCnt,m_pMsg->body.conInd.dwPlci,TRUE);
		SendToCapi(); 
		Close();
	}
}
//////////////////////////////////////////////////////////////////////////////
void CCp::EnableRawData(BOOL bEnable)
{
	// NOT YET any validations, already connected and the like
	m_bClientWantsRawData = bEnable;
}
//////////////////////////////////////////////////////////////////////////////
BOOL CCp::IsRealOwnCallingPartyNumber()
{
	BOOL bReturn = TRUE;
	if (   (m_sOwnNumber == LISTEN_ALL)
		|| (m_sOwnNumber == LISTEN_REJECTALL)
		)
	{
		bReturn = FALSE;
	}
	return bReturn;
}
