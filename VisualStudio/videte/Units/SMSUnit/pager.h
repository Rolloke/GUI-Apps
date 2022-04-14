//
//
// Class for Pager
//
//

#define WM_PAGER			(WM_USER+25)
#define MSG_ONCONNECT		1
#define MSG_ONDISCONNECT	2
#define MSG_ONCANWRITE		3
#define MSG_ONCANREAD		4
#define MSG_ONNONSTD		5
#define MSG_ONERROR			6
#define MSG_ERROR			7
#define MSG_MESSID			8
#define MSG_PROVRESP		9

#define SND			1
#define QRY			2
#define DEL			3

#define NORMAL		0x01
#define SCALL		0x02
#define SKYPER		0x04
#define GSM			0x08
#define TAP			0x10
#define UCP			0x20
#define MODEM		0x40
#define CAPI		0x80

#define MSG_SIZE	1024 
class CPager;

class CPager : public Vport
{
public:
	CPager();
	~CPager();
	
	char *get_last_error();
	char *get_msg_id();
	char *get_message();
	char *get_providerresp();

	void set_receiverid(CString);
	void set_providerid(CString);
	CString get_providerid();
	void set_pin(CString);
	void set_mode(int);
	CString get_mode();
	
	void set_port(CString);
	void set_speed(CString);
	void set_frame(CString);

	void set_line(CString);
	CString get_line();
	void set_dialprefix(CString);
	CString get_dialprefix();
	void set_dialmode(CString);
	void set_x75_t70nl(CString);
	CString get_x75_t70nl();
	void set_msn(CString);
	CString get_msn();

	BOOL send(CWnd *parent_wnd, const CString);
	BOOL query(CWnd *parent_wnd);
	BOOL delete_msg(CWnd *parent_wnd);
	BOOL cancel(CWnd *parent_wnd);

	void OnCONNECT();
	void OnDISCONNECT();
	void OnERROR();
	void OnCANWRITE();
	void OnNONSTD();
	void OnCANREAD();

private:
	CWnd *wnd_parent;
	char errorstr[256];
	char providerresp[256];
	char MessageID[256];
	char readmsg[MSG_SIZE];
	int messagesend;
	int sendmode;
	/* Paging */		
	CString provider;
	CString receiver;
	CString pin;
	int mode;	
	/* Comport */
	CString port;
	CString speed;
	CString frame;
	/* ISDN */
	CString t70nl;
	CString msn;
	/* Lineparameter */
	CString line;
	CString prefix;
	CString dialmode;
	/* Shortmessage */
	CString	PageText;
void CPager::PagerMsg(int, int);
CString CPager::protocol_linkid(CString);
CString CPager::service_linkid();
CString CPager::line_linkid();
};
