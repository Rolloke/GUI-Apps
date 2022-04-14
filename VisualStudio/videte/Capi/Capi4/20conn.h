////////////////////////////////////////////////////////////////////////////
// cconnect.h

#ifndef _CBChannel_H
#define _CBChannel_H

#include "20def.h"
#include "LimitedString.h"

// OLD #define		NR_B_CHANNELS	7
// NEW 140297 2 are not enough even for REJECT a pseuo channel is created
#define		NR_B_CHANNELS	3

class CBChannel
{
private:
    // HEDU char    m_isdn_nr[BUFFER_32];// Telefonnummer der aktuellen Verbindung
	// HEDU char 	m_szId[BUFFER_32];			// Id dieser Verbindung;
	CLimitedString32 m_sIsdnNr;
	CLimitedString32 m_sId;
    DWORD   m_dwNcciPlciCntrl;	// 32 bit Maske mit 31--NCCI--16--PLCI--8--ext./int.--6--Cntrl--0
    WORD    m_info_mask;		// Info-Maske der Anwendung
	LONG	m_lTimer;			// Aufbautimer des Kanals. 
    BOOL    m_bAufbau;
    BOOL	m_bAbbau;			// True wenn Kanal abgebaut wird.
    BYTE    m_controller;		// Boardnummer des anzusprechenden Tina-Boardes (0)
    BYTE    m_srv;
    BYTE    m_srv_aio;
    WORD    m_reject;			// 0 Verbindung wird angenommen 1 Verbindung wird abgelehnt
	int		m_iBufValid;		// Anz. Sendebuffers die valid sind.
	BYTE	m_byCmd;                 
	BYTE	m_bySubcmd;			// REQ, CONF, IND, RESP                    
	BOOL	m_bPasswortOk;

public:
	CBChannel();
	void	Create(LPCSTR szTelNr, DWORD wPlci, BOOL bAufbau, WORD wReject, LONG lTimeout, WORD wInfo);
	void	SetNewCommands(BYTE byCmd, BYTE bySubcmd);    
	BYTE 	GetCmd()						{ return m_byCmd; }	
	BYTE 	GetSubcmd()						{ return m_bySubcmd; }	
	void	SetNcci(DWORD dwNcci)			{ m_dwNcciPlciCntrl = dwNcci; }              
	DWORD	GetPlci()						{ return m_dwNcciPlciCntrl & 0x0000FFFF; }
	DWORD	GetNcci()						{ return m_dwNcciPlciCntrl; }
	void	SetAbbauFlag(BOOL bAbbau)		{ m_bAbbau = bAbbau; }
	BOOL	GetAufbau()						{ return m_bAufbau; }
	BOOL	GetAbbau()						{ return m_bAbbau; }
	void	IncBufCount()					{ m_iBufValid++; }
	void	DecBufCount()					{ m_iBufValid = m_iBufValid>0?m_iBufValid-1:0; }
	WORD 	GetBufCount()					{ return (WORD)(m_iBufValid>0?m_iBufValid:0); }
	LPCTSTR	GetTelNumber()					{ return (LPCTSTR)m_sIsdnNr; } 
	WORD	GetInfomask()					{ return m_info_mask; }
	DWORD 	GetController()					{ return m_dwNcciPlciCntrl & 0x0000007F; } 
	BYTE 	GetSrv()						{ return m_srv; }
	BYTE	GetSrvAio()						{ return m_srv_aio; }
	void	RejectCall(WORD wReject)		{ m_reject = wReject; }             
	WORD	GetReject()						{ return m_reject; }
	void	SetKanalTimer(LONG lNewTimer)	{ m_lTimer = lNewTimer; }
	LONG	GetKanalTimer()					{ return m_lTimer; } 
	void	SetKanalId(LPCTSTR lpId);
	LPCTSTR	GetKanalId()					{ return (LPCTSTR)m_sId; }
	void	SetPasswortOk(BOOL bPasswort) 	{ m_bPasswortOk = bPasswort; }
	BOOL	GetPasswortOk()					{ return m_bPasswortOk; }
};	                                                               
	
class CConnect
{
private:      
	UINT			m_uNrMax;
	UINT			m_uNr;
	UINT			m_uControllerInc;
	CBChannel*		m_pB[NR_B_CHANNELS];
	BOOL			m_bActive; 
	WORD			m_wMaxNrXBuf;

private:
	inline void		SetNextValidChannel();
	
public:
	CConnect();
	~CConnect(); 
	BOOL	Create(WORD wXBuffers);
	// Call diese Routine first...
	
	BOOL	CreateChannel(	LPCSTR szTelNr,
							DWORD dwPlci,
							BOOL bAufbau, 
							WORD wReject = 0, 
							LONG lTimeout = 0L,
							WORD wInfo = 0);
	void	SetNewNcci(DWORD dwNcci)  { if (m_pB[m_uNr]!=NULL) m_pB[m_uNr]->SetNcci(dwNcci); }
	// Set NCCI für den Kanal.

	DWORD	Plci()			{ return m_pB[m_uNr]?m_pB[m_uNr]->GetPlci():(DWORD)0xFFFFFFFF; }
	// Gibt zurück PLCI für den Kanal. Wenn Kanal ungultig, 0xFFFF.
	
	DWORD	Ncci()			{ return m_pB[m_uNr]?m_pB[m_uNr]->GetNcci():(DWORD)0xFFFFFFFF; }
	// Gibt zurück NCCI für den Kanal. Wenn Kanal ungultig, 0xFFFF.
	
	void	DestroyChannel();					   
	// Löscht den Kanal auf. m_pB = NULL.            
	
	void	Reset();
	
	UINT 	GetConnections()	{ return m_uNrMax; }     
	// return:	0 Keine Connection, 1 Eine Connection usw.   

	inline UINT 	CountConnections();     
	// return:	0 Keine Connection, 1 Eine Connection usw.   
	
	BOOL	FindPlci(DWORD dwPlci);
	// Sucht nach passendem PLCI innerhalb der Kanäle. FALSE nicht gefunden. m_uNr wird gesetzt.
	
	inline BOOL	FindNcci(DWORD dwNcci);
	// Sucht nach passendem NCCI innerhalb der Kanäle. FALSE nicht gefunden. m_uNr wird gesetzt.
		
	inline BYTE	Cmd(int iKanal=-1);
	// return: Letztes Command die ausgeführt wurde.  iKanal=0, Erste B-Kanal usw.  -1 = aktuelle B-Kanal.

	inline BYTE	Subcmd(int iKanal=-1);
	// return: Letztes Command die ausgeführt wurde.  iKanal=0, Erste B-Kanal usw.  -1 = aktuelle B-Kanal.

	void	SetCommands(BYTE byCmd, BYTE bySubcmd);
	// Setzt die aktuellen Commands für diesen Kanal.

	BOOL	Active()		{ return m_bActive; }
	// return: TRUE wenn eine vollständige verbindung vorhanden ist mit Passw. ok.
		
	void	SetAbbau(BOOL bAbbau);	
	// bAufbau TRUE: Aktivaufbau.
	
	BOOL	AbbauAktiv()	{ return m_pB[m_uNr]?m_pB[m_uNr]->GetAbbau()		:FALSE; }			
	// TRUE: Kanal hat ein CONNECT_REQ bekommen und wird abgebaut.
	
	BOOL	AufbauAktiv()	{ return m_pB[m_uNr]?m_pB[m_uNr]->GetAufbau()		:FALSE; }
	// TRUE: Aktivaufbau. FALSE: Passivaufbau.
	
	void	IncBuf()		{ m_pB[m_uNr]->IncBufCount(); }
	// Erhöht die Anz der VALID X-Buf.
	
	void	DecBuf()		{ m_pB[m_uNr]->DecBufCount(); }
	// Erniedrigt die Anz der VALID X-Buf.

	WORD 	GetBuf()		{ return m_pB[m_uNr] ? m_pB[m_uNr]->GetBufCount()		:m_wMaxNrXBuf; }
	
	WORD	Reject()		{ return m_pB[m_uNr] ? m_pB[m_uNr]->GetReject()		: (WORD)1; }
	DWORD	Controller()	{ return m_pB[m_uNr] ? m_pB[m_uNr]->GetController()	: 0; }
	void	IncController()	{ m_uControllerInc++; } 
	DWORD	GetNextController();
	WORD	Infomask()		{ return m_pB[m_uNr] ? m_pB[m_uNr]->GetInfomask()	: (WORD)0; }
	LPCTSTR	TelNumber()		{ return m_pB[m_uNr] ? m_pB[m_uNr]->GetTelNumber()	: _T(""); }
	UINT 	GetChannel()			{ return m_uNr; }
	BYTE	GetNextChannelNr();
	void	SetId(LPCTSTR szId)		{ if (m_pB[m_uNr]) m_pB[m_uNr]->SetKanalId(szId); } 
	LPCTSTR	GetId()					{ return m_pB[m_uNr] ? m_pB[m_uNr]->GetKanalId() : _T(""); } 
	void	SetChannel(UINT uNewNr);
	// Setzt die B-kanal Verbindung auf Kanal nr iNewNr (0...NR_B_CHANNELS-1).
	
	inline BOOL	SetBestChannel();
	// Setzt den B-Kanal mit die meisten freien Buffer als den aktuellen B-Kanal.
	// Wenn alle Buffers voll: return FALSE.         
	
	void	SetTimer(LONG lTime) 	{ if (m_pB[m_uNr]) m_pB[m_uNr]->SetKanalTimer(lTime); } 
	LONG	GetTimer() 				{ return m_pB[m_uNr]?m_pB[m_uNr]->GetKanalTimer():0L; }
	void	SetPasswort(BOOL bOk)	{ if (m_pB[m_uNr]) { m_pB[m_uNr]->SetPasswortOk(bOk); if (bOk) m_bActive=TRUE; } }
	BOOL	PasswortOk()			{ return (m_pB[m_uNr])?m_pB[m_uNr]->GetPasswortOk():FALSE; }
};

inline UINT CConnect::CountConnections()
{
	m_uNrMax = 0;
	for (UINT i=0;i<NR_B_CHANNELS;i++)
	{
		if (m_pB[i]) {
			m_uNrMax++;
		}
	}
	return m_uNrMax;
}

inline void CConnect::SetNextValidChannel()
{
	for (m_uNr=0;m_uNr<NR_B_CHANNELS;m_uNr++)
	{
		if (m_pB[m_uNr]) {
			return;
		}
	}
	m_uNr = 0;
}                 

inline BYTE CConnect::Subcmd(int iKanal)
{             
	iKanal = (iKanal==-1)?m_uNr:iKanal;
	return ( m_pB[iKanal] ? m_pB[iKanal]->GetSubcmd() : (BYTE)0);        
}                                         

inline BYTE CConnect::Cmd(int iKanal)
{             
	if (iKanal==-1) {
		if (m_pB[m_uNr]) {
			return m_pB[m_uNr]->GetCmd();        	
		}
	} else if (m_pB[iKanal]) {
		return m_pB[iKanal]->GetCmd();        	
	} 
	
	return 0;
}                                         

inline BOOL CConnect::SetBestChannel()
{
	UINT uTempNr = m_uNr;
	WORD wBufCount = m_wMaxNrXBuf;
#if 1
	//
	int qLen0 = -1 , qLen1 = -1;
	// optimization check the queue length of the first two channels
	// don't know if m_uNr is need in one of the following calls
	m_uNr = 0;	
	if (Cmd()==CONNECT_B3_ACTIVE 
		&& m_pB[m_uNr]->GetPasswortOk()
		&& m_pB[m_uNr]->GetBufCount() < wBufCount) {
		qLen0 = m_pB[m_uNr]->GetBufCount();
	}
	// don't know if m_uNr is need in one of the following calls
	m_uNr = 1;	
	if (Cmd()==CONNECT_B3_ACTIVE 
		&& m_pB[m_uNr]->GetPasswortOk()
		&& m_pB[m_uNr]->GetBufCount() < wBufCount) {
		qLen1 = m_pB[m_uNr]->GetBufCount();
	}

	if (qLen0 != -1) {	 // a valid queue in Channel0
		if ( qLen1!= -1) {	// a valid queue in Channel1
			if (qLen0 <= qLen1) {
				m_uNr = 0;
				return TRUE;	// <<< EXIT >>>
			} else {
				m_uNr = 1;
				return TRUE;	// <<< EXIT >>>
			}
		} else {
			// loop code would take Channel0 anyway
			m_uNr = 0;
			return TRUE;	// <<< EXIT >>>
		}
	} else {	// no valid queue in Channel0 fall through
	}
#endif

	BOOL bRet =  FALSE;
	// old 'loop code'
	for (m_uNr=0;m_uNr<NR_B_CHANNELS;m_uNr++)
	{       
		// OOPS no check on m_pB !=NULL ?
		if (Cmd()==CONNECT_B3_ACTIVE 
			&& m_pB[m_uNr]->GetPasswortOk() 
			&& m_pB[m_uNr]->GetBufCount() < wBufCount)
		{ 
			uTempNr = m_uNr;
			wBufCount = m_pB[m_uNr]->GetBufCount();
			bRet = TRUE;
		}
	}                   
	m_uNr = uTempNr;    
	return bRet;
}

inline BOOL CConnect::FindNcci(DWORD dwNcci)
{
	ASSERT(NR_B_CHANNELS>=2);

	// handcrafted unroll loop for the first two channels
	m_uNr=0;
	if (m_pB[m_uNr] && m_pB[m_uNr]->GetNcci() == dwNcci) {
		return TRUE;	// <<< EXIT >>>
	} else {
		m_uNr=1;
		if (m_pB[m_uNr] && m_pB[m_uNr]->GetNcci() == dwNcci) {
			return TRUE; // <<< EXIT >>>
		}
		
		// try the remaining channels
		for (m_uNr=2;m_uNr<NR_B_CHANNELS;m_uNr++)
		{
			if (m_pB[m_uNr] && m_pB[m_uNr]->GetNcci() == dwNcci)
				return TRUE;	// <<< EXIT >>>
		}         
	}

	SetNextValidChannel();                     
	return FALSE;
}

			

#endif