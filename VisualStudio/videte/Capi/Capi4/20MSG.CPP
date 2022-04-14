#include <afxwin.h>         // MFC core and standard components

#include "20def.h"
#include "20msg.h"

#include <wkclasses\wk_trace.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void connect_req(LPCAPI20_MSG m,
					WORD wApplid,
					WORD msg_nbr,
					DWORD dwController,
					const char* pOwnNumber,	// CAVEAT: Keep char*!
					const char* called		// CAVEAT: Keep char*!
					)
{
    int l=0;	// Hilfsvariable
    
    m->wApplid  				= wApplid;
    m->command      			= CONNECT;
    m->subcommand   			= REQ;
    m->wMessagenumber			= msg_nbr;
    
    m->body.conReq.dwController = dwController;					// Controller
    m->body.conReq.wCipValue  	= CAPI20_CIP_UNREST_DIGITAL;				
    
   int n=0;	// Position in rest[]
 	//---------------------- Called Party Number -------------------------
    l = lstrlenA(called);
	if (l>=253)
	{
		TRACE(_T("Invalid length %d\n"), l);
		l=0;
	}
    if (l)
    {
		m->body.conReq.rest[n++] = (BYTE)(1+l);					// Called Party Number len
		m->body.conReq.rest[n++] = 0x80;				// Called Party Number Type
	    for (int i=0;i<l;i++)
	    	m->body.conReq.rest[n++] = called[i];		// Called Party Number
	}
	else {
		m->body.conReq.rest[n++] = 0;
	}
	
	//---------------------- Calling Party Number -------------------------
	l = lstrlenA(pOwnNumber);
	if (   (l >= 253)
		|| ((n+l) >= 255)
		)
	{
		TRACE(_T("Invalid length %d\n"),l);
		l=0;
	}
	if (l)
	{
		m->body.conReq.rest[n++] = (BYTE)(2+l);					// Calling Party Number len
		m->body.conReq.rest[n++] = 0x00;				// Calling Party Number Type
		m->body.conReq.rest[n++] = 0x80;				// Calling Party Number Presentation
	    for (int i=0;i<l;i++)
	    	m->body.conReq.rest[n++] = pOwnNumber[i];		// Called Party Number
    }
    else {
		m->body.conReq.rest[n++] = 0;
	}
	//---------------------- Called Party Subadress -------------------------
    l = 0;
    if (l)
    {
		m->body.conReq.rest[n++] = (BYTE)(1+l);					// Called Party Subadress len
		m->body.conReq.rest[n++] = 0x80;				// Called Party Subadress Type
	    for (int i=0;i<l;i++)
	    	m->body.conReq.rest[n++] = called[i];		// Called Party Subadress
	}
	else {
		m->body.conReq.rest[n++] = 0;
	}
	
	//---------------------- Calling Party Subadress -------------------------
    l = 0;
    if (l)
    {
		m->body.conReq.rest[n++] = (BYTE)(1+l);					// Calling Party Subadress len
		m->body.conReq.rest[n++] = 0x80;				// Calling Party Subadress Type
	    for (int i=0;i<l;i++)
	    	m->body.conReq.rest[n++] = called[i];		// Calling Party Subadress
	}
	else {
		m->body.conReq.rest[n++] = 0;
	}

	//---------------------- B Channel Protocols -----------------------------
	m->body.conReq.rest[n++] = 0;	
	//--------------------------------- BC ----------------------------------
	m->body.conReq.rest[n++] = 0;				// BC
	//--------------------------------- LLC ---------------------------------
	m->body.conReq.rest[n++] = 0;				// LLC
	//--------------------------------- HLC ---------------------------------
	m->body.conReq.rest[n++] = 0;				// HLC
	//------------------------ Additional Info ------------------------------
	m->body.conReq.rest[n++] = 0;				// Additional info
                                
    m->wTotal_length = (WORD)(CAPI20_HDRLEN + 6 + n);
}

void connect_resp(LPCAPI20_MSG m,WORD wApplid,WORD msg_nbr,DWORD dwPlci,WORD wReject)
{
    int n = 0;

    m->wApplid  			= wApplid;
    m->command      		= CONNECT;
    m->subcommand   		= RESP;
    m->wMessagenumber		= msg_nbr;
    m->body.conResp.dwPlci  = dwPlci;
	m->body.conResp.reject 	= wReject;
	
	//------------------- B Protocol ---------------------
	m->body.conResp.rest[n++] = 0;
	
	//--------------- Connected Number -------------------
	m->body.conResp.rest[n++] = 0;

	//--------------- Connected Subadr -------------------
	m->body.conResp.rest[n++] = 0;

	//---------------------- LLC -------------------------
	m->body.conResp.rest[n++] = 0;     
	
	//---------------- Additional Info -------------------
	m->body.conResp.rest[n++] = 0;     

    m->wTotal_length = (WORD)(CAPI20_HDRLEN + n + 6);
}

void disconnect_req(LPCAPI20_MSG m, WORD wApplid, WORD msg_nbr, DWORD dwPlci)
{
    m->wApplid  			= wApplid;
    m->command      		= DISCONNECT;
    m->subcommand   		= REQ;
    m->wMessagenumber		= msg_nbr;
    m->body.discReq.dwPlci  = dwPlci;
    m->body.discReq.addInfoLen = 0;
    m->wTotal_length = CAPI20_HDRLEN + 5;
}

void disconnect_resp(LPCAPI20_MSG m, WORD wApplid, WORD msg_nbr, DWORD dwPlci)

{
    m->wApplid  = wApplid;
    m->command      = DISCONNECT;
    m->subcommand   = RESP;
    m->wMessagenumber	= msg_nbr;
    m->body.discResp.dwPlci  = dwPlci;
    m->wTotal_length = CAPI20_HDRLEN+4;
}

void connect_active_resp(LPCAPI20_MSG m,WORD wApplid,WORD msg_nbr,DWORD dwPlci)

{
	m->wApplid  = wApplid;
	m->command      = CONNECT_ACTIVE;
	m->subcommand   = RESP;
	m->wMessagenumber	= msg_nbr;
	m->body.actResp.dwPlci = dwPlci;
	m->wTotal_length = CAPI20_HDRLEN + 4;
}

void listen_req(LPCAPI20_MSG m,
				WORD wApplid,
				WORD msg_nbr,
				DWORD dwController,
				DWORD dwInfo_mask,
				DWORD dwCip_mask,
				DWORD dwCip_mask2 )
{                                       
	
    m->wApplid  				= wApplid;
	m->command      			= LISTEN;
    m->subcommand   			= REQ;
    m->wMessagenumber			= msg_nbr;

    m->body.lisReq.dwController = dwController;
    m->body.lisReq.dwInfomask  	= dwInfo_mask;
    m->body.lisReq.dwCipMask   	= dwCip_mask;
    m->body.lisReq.dwCipMask2  	= dwCip_mask2;

	int n = 0;
    m->body.lisReq.callingParty[n++] = 0;		// Calling Party Number (only used for Handsets)
    m->body.lisReq.callingParty[n++] = 0;		// Calling Party Subadress (only used for Handsets)

    m->wTotal_length = (WORD)(CAPI20_HDRLEN + 4*4 + n);
}

void connect_b3_req(LPCAPI20_MSG m,WORD wApplid,WORD msg_nbr,DWORD dwPlci)
{                                           
    m->wApplid  				= wApplid;
    m->command      			= CONNECT_B3;
    m->subcommand   			= REQ;
    m->wMessagenumber			= msg_nbr;

    m->body.conB3Req.dwPlci 	= dwPlci;
    m->body.conB3Req.byNcpilen 	= 0;
    m->body.conB3Req.ncpi[0] 	= 0;
    m->wTotal_length = CAPI20_HDRLEN + 5;
}

void connect_b3_active_resp(LPCAPI20_MSG m,WORD wApplid,WORD msg_nbr,DWORD dwNcci)
{
        m->wApplid  				= wApplid;
    	m->command      			= CONNECT_B3_ACTIVE;
        m->subcommand   			= RESP;
        m->wMessagenumber			= msg_nbr;
        m->body.conB3ActResp.dwNcci = dwNcci;

        m->wTotal_length = CAPI20_HDRLEN + 4;
}

void connect_b3_resp(LPCAPI20_MSG m,WORD wApplid,WORD msg_nbr,DWORD dwNcci,WORD wReject)
{
        m->wApplid  			= wApplid;
    	m->command      		= CONNECT_B3;
        m->subcommand   		= RESP;
        m->wMessagenumber		= msg_nbr;

        m->body.conB3Resp.dwNcci 	= dwNcci;
    	m->body.conB3Resp.wReject 	= wReject;
        m->body.conB3Resp.byNcpilen = 0;

        m->wTotal_length = CAPI20_HDRLEN + 7;
}

void disconnect_b3_req(LPCAPI20_MSG m,WORD wApplid,WORD msg_nbr, DWORD dwNcci)
{
	m->wApplid  				= wApplid;
	m->command      			= DISCONNECT_B3;
	m->subcommand   			= REQ;
    m->wMessagenumber			= msg_nbr;
	m->body.discB3Req.dwNcci	= dwNcci;
	m->body.discB3Req.byNcpilen	= 0;
	m->wTotal_length			= CAPI20_HDRLEN+5;  
}

void disconnect_b3_resp(LPCAPI20_MSG m,WORD wApplid,WORD msg_nbr, DWORD dwNcci)
{
	m->wApplid  				= wApplid;
	m->command      			= DISCONNECT_B3;
	m->subcommand   			= RESP;
    m->wMessagenumber			= msg_nbr;
	m->body.discB3Resp.dwNcci	= dwNcci;
	m->wTotal_length			= CAPI20_HDRLEN+4;  
}

void data_b3_resp(LPCAPI20_MSG m,WORD wApplid,WORD msg_nbr,WORD wNumber,DWORD dwNcci)
{
	m->wApplid						= wApplid;
	m->command						= DATA_B3;
	m->subcommand					= RESP;
	m->wMessagenumber				= msg_nbr;
	m->body.dataB3Resp.dwNcci		= dwNcci;
	m->body.dataB3Resp.wNumber		= wNumber;
	m->wTotal_length				= CAPI20_HDRLEN + 6;
}

void reset_resp(LPCAPI20_MSG m,WORD wApplid,WORD msg_nbr, DWORD dwNcci)
{
	m->wApplid  				= wApplid;
	m->command      			= RESET_B3;
	m->subcommand   			= RESP;
    m->wMessagenumber			= msg_nbr;
	m->body.discB3Resp.dwNcci	= dwNcci;
	m->wTotal_length			= CAPI20_HDRLEN+4;  
}

