//
//  Sourcefile for Pager Class
//
#include "stdafx.h"
#include "pager.h"


CPager::CPager()  
{
	wnd_parent = NULL;
	messagesend = 0;
}

CPager::~CPager()
{
}

void CPager::OnCONNECT()
{								
	PagerMsg(MSG_ONCONNECT,0);
}

void CPager::OnDISCONNECT()
{
	Vclose();
	PagerMsg(MSG_ONDISCONNECT,0);
}

void CPager::OnNONSTD()
{
	char buf[256],tmp[256];
	
	Vgetpar("NONSTD",buf);
	Vgetpar(buf,tmp);
	if (stricmp(buf,"tap.messageid") == 0) {
		strcpy(MessageID,tmp);
		PagerMsg(MSG_MESSID,0);
	}
	if (strstr(_strlwr(buf),_strlwr("providerresp")) != NULL ) {
		strcpy(providerresp,tmp);
		PagerMsg(MSG_PROVRESP,0);
	}
}

void CPager::OnERROR()
{
	Verror(errorstr);
	PagerMsg(MSG_ONERROR,0);
	Vclose();
}

BOOL CPager::cancel(CWnd *parent_wnd)
{
	wnd_parent = parent_wnd;		// Remember this for WM_PAGER
	
	if ( Vctl(V_ABORT,1000) < 0) {
		return FALSE;
	}
	return TRUE;
}

char *CPager::get_last_error()
{
	return errorstr;
}

char *CPager::get_providerresp()
{
	return providerresp;
}

char *CPager::get_message()
{
	if (strlen(readmsg) > 0){
			return readmsg;
	} else return "";
}

char *CPager::get_msg_id()
{
	return MessageID;
}

void CPager::set_providerid(CString idstring)
{
	provider = idstring;
}
CString CPager::get_providerid()
{
	return provider;
}
void CPager::set_receiverid(CString idstring)
{
	receiver = idstring;
}
void CPager::set_mode(int mod)
{
	mode = mod;
}
CString CPager::get_mode()
{
	CString sMode;
	if ((mode & CAPI) == CAPI)
	{
		sMode += "CAPI ";
	}
	if ((mode & MODEM) == MODEM)
	{
		sMode += "MODEM ";
	}
	if ((mode & UCP) == UCP)
	{
		sMode += "UCP ";
	}
	if ((mode & TAP) == TAP)
	{
		sMode += "TAP ";
	}
	if ((mode & GSM) == GSM)
	{
		sMode += "GSM ";
	}
	if ((mode & SKYPER) == SKYPER)
	{
		sMode += "SKYPER ";
	}
	if ((mode & SCALL) == SCALL)
	{
		sMode += "SCALL ";
	}
	if ((mode & NORMAL) == NORMAL)
	{
		sMode += "NORMAL ";
	}
	return sMode;
}
void CPager::set_pin(CString pinin)
{
	pin = pinin;
}
void CPager::set_port(CString portin)
{
	port = portin;
}
void CPager::set_line(CString lin)
{
	line = lin;
}
CString CPager::get_line()
{
	return line;
}
void CPager::set_dialprefix(CString pre)
{
	prefix = pre;
}
CString CPager::get_dialprefix()
{
	return prefix;
}
void CPager::set_x75_t70nl(CString t)
{
	t70nl = t;
}
CString CPager::get_x75_t70nl()
{
	return t70nl;
}
void CPager::set_msn(CString n)
{
	msn = n;
}
CString CPager::get_msn()
{
	return msn;
}
void CPager::set_dialmode(CString dmode)
{
	dialmode = dmode;
}
void CPager::set_speed(CString spd)
{
	speed = spd;
}

void CPager::set_frame(CString frm)
{
	frame = frm;
}

void CPager::PagerMsg(int msg, int num)
{
	if (wnd_parent != NULL) wnd_parent->PostMessage(WM_PAGER,msg,num); 
}

void CPager::OnCANWRITE()
{
	int res = 0;
	if ((messagesend == 0) && (sendmode == SND ))
	{
		CWK_String sPT(PageText);
		LPCSTR szPageText = sPT;
		res = Vwrite(szPageText, strlen(szPageText));
		WK_TRACE(_T("SEND <%s>\n"), LPCTSTR(PageText));
		messagesend = 1;
		if (res < 0 ) 
		{
			Verror(errorstr);
			messagesend = 0;
		}
	}
	if (messagesend == 1) 
	{
		Vctl(V_DISCONNECT,0);
	}
}

void CPager::OnCANREAD()
{
	int res;
	char tmp[256];
	PagerMsg(MSG_ONCANREAD,0);
	res = Vread(tmp,sizeof(tmp));
	CWK_String sTxt(tmp);
	WK_TRACE(_T("READ <%s>\n"), LPCTSTR(sTxt));
}

CString CPager::protocol_linkid(CString service)
{
	CString modus;
	CString protocol;

	if (mode & GSM) {
		protocol = _T("SMS:")+receiver;
		return protocol;
	}

	if (mode & TAP)			{ protocol =_T("TAP:")+receiver+_T(",SERVICE=")+service; } 
	else if (mode & UCP)	{ protocol =_T("UCP:")+receiver; }	

	if (sendmode == SND)		{ modus =_T(",MODE=SEND");  }
	else if (sendmode == QRY)	{ 
		if (mode & TAP)			{ modus = _T(",MODE=QUERY,MESSAGEID=")+pin; } 
		else if (mode & UCP)	{ modus = _T(",MODE=QUERY,AUTHENTIFICATION=")+pin; }	
	}
	else if (sendmode == DEL)	{ 
		if (mode & TAP)			{ modus = _T(",MODE=DELETE,MESSAGEID=")+pin; } 
		else if (mode & UCP)	{ return _T("");}	
	}

	protocol = protocol+modus;
	
	return protocol;
}

CString CPager::service_linkid()
{
	
	if (mode & SKYPER) { return _T("SKYPER"); }
	else if (mode & SCALL) { return _T("SCALL"); } 
	return _T("NORMAL");
}

CString CPager::line_linkid()
{
	if (mode & CAPI) {
		CString sRet = _T("/X.75,T70NL=") + t70nl + _T("/CAPI:") + provider + _T(",LINE=") + line;
		if (msn.IsEmpty() == FALSE)
		{
			sRet += _T(",MSN=") + msn;
		}
		if (prefix.IsEmpty() == FALSE)
		{
			sRet += _T(",DIALPREFIX=") + prefix;
		}
		return sRet;
	} else if (mode & MODEM){
		return (_T("/ATMODEM:")+provider+_T(",LINE=")+line+_T(",DIALPREFIX=")+prefix+_T(",DIALMODE=")+dialmode+
				_T("/ASYNC/COM:")+port+_T(",SPEED=")+speed+_T(",FRAME=")+frame);
	} else if (mode & GSM) {
		return (_T("/GSM:")+provider+_T("/ASYNC/COM:")+port);
	}

	return _T("");
}

BOOL CPager::send(CWnd *parent_wnd, const CString text)
{
	long error;
	CWK_String link_id;
	
	PageText = text;
	wnd_parent = parent_wnd;		// Remember this for WM_PAGER
	sendmode = SND;
	messagesend = 0;
	link_id = protocol_linkid(service_linkid());
	if (link_id == _T("")) return FALSE;
	link_id = link_id + line_linkid();
	WK_TRACE(_T("link_id is <%s>\n"), LPCTSTR(link_id));
	LPCSTR szLinkID = link_id;
	error = Vopen(szLinkID);
	Verror(errorstr);
	if (error < 0) 
	{
		PagerMsg(MSG_ERROR,error);
		WK_TRACE_ERROR(_T("Vopen <%s>\n"), LPCTSTR(link_id));
		return FALSE;
	} 
	::Vctl(error,V_RTIMEOUT,10*V_SECOND);
	::Vctl(error,V_WTIMEOUT,10*V_SECOND);
	::Vctl(error,V_PTIMEOUT,10*V_SECOND);
	return TRUE;
}

BOOL CPager::query(CWnd *parent_wnd)
{
	long error;
	CString link_id;

	wnd_parent = parent_wnd;		// Remember this for WM_PAGER
	sendmode = QRY;
	messagesend = 0;
	link_id = protocol_linkid(service_linkid());
	if (link_id == _T("")) return FALSE;
	link_id = link_id + line_linkid();
	CWK_String sLID(link_id);
	LPCSTR szLinkID = sLID;
	error = Vopen(szLinkID);
	Verror(errorstr);
	if (error < 0) 
	{
		PagerMsg(MSG_ERROR,error);
		WK_TRACE_ERROR(_T("Vopen <%s>\n"), LPCTSTR(link_id));
		return FALSE;
	} 
	return TRUE;
}

BOOL CPager::delete_msg(CWnd *parent_wnd)
{
	long error;
	CString link_id;

	wnd_parent = parent_wnd;		// Remember this for WM_PAGER
	sendmode = DEL;
	messagesend = 0;
	link_id = protocol_linkid(service_linkid());
	if (link_id == _T("")) return FALSE;
	link_id = link_id + line_linkid();
	CWK_String sLID(link_id);
	LPCSTR szLinkID = sLID;
	error = Vopen(szLinkID);
	Verror(errorstr);
	if (error < 0) {
		PagerMsg(MSG_ERROR,error);
		return FALSE;
	} 
	return TRUE;
}

