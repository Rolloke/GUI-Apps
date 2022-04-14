#ifndef INC_BLDMSG
#define INC_BLDMSG

extern void connect_req			(LPCAPI20_MSG m,
								WORD wApplid,
								WORD msg_nbr,
								DWORD dwController,
								const char* pOwnNumber,	// CAVEAT: Keep char*!
								const char* called		// CAVEAT: Keep char*!
								);
extern void connect_resp		(LPCAPI20_MSG m,WORD wApplid,WORD msg_nbr,DWORD dwPlci,WORD wReject);
extern void disconnect_req		(LPCAPI20_MSG m, WORD wApplid, WORD msg_nbr, DWORD dwPlci); 
extern void disconnect_resp		(LPCAPI20_MSG m, WORD wApplid, WORD msg_nbr, DWORD dwPlci);
extern void connect_active_resp	(LPCAPI20_MSG m,WORD wApplid,WORD msg_nbr,DWORD dwPlci);
extern void listen_req			(LPCAPI20_MSG m,
								WORD wApplid,
								WORD msg_nbr,
								DWORD dwController,
								DWORD dwCip_mask,
								DWORD dwCip_mask2,
								DWORD dwInfo_mask	);

extern void connect_b3_req		(LPCAPI20_MSG m,WORD wApplid,WORD msg_nbr,DWORD dwPlci);
extern void connect_b3_active_resp(LPCAPI20_MSG m,WORD wApplid,WORD msg_nbr,DWORD dwNcci);
extern void connect_b3_resp		(LPCAPI20_MSG m,WORD wApplid,WORD msg_nbr,DWORD dwNcci,WORD wReject);
extern void disconnect_b3_req	(LPCAPI20_MSG m,WORD wApplid,WORD msg_nbr, DWORD dwNcci); 
extern void disconnect_b3_resp	(LPCAPI20_MSG m,WORD wApplid,WORD msg_nbr, DWORD dwNcci);
extern void data_b3_resp		(LPCAPI20_MSG m,WORD wApplid,WORD msg_nbr,WORD wNumber,DWORD dwNcci);
extern void reset_resp			(LPCAPI20_MSG m,WORD wApplid,WORD msg_nbr,DWORD dwNcci);

#endif

