#include <afxwin.h>         // MFC core and standard components

#include "wkclasses\WK_Trace.h"


#include "DecodeCapiMessages.h"
#include "20conn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////
// cconnect.cpp

///////////////////////////////////////////////////////////////////////////
// Funktionen für CConnect Class

CConnect::CConnect()
{                       
	for (m_uNr=0;m_uNr<NR_B_CHANNELS;m_uNr++) {
		m_pB[m_uNr] = NULL;
	}
	m_uNr = 0;
	m_uNrMax = 0;
	m_bActive = FALSE;
	m_uControllerInc = 1;
	m_wMaxNrXBuf = MY_N_XBUF;
}

BOOL CConnect::Create(WORD wXBuffers)
{
	m_wMaxNrXBuf = wXBuffers;
	return TRUE;
}
	        
CConnect::~CConnect()
{
	for (m_uNr=0;m_uNr<NR_B_CHANNELS;m_uNr++)
	{                                       
		if (m_pB[m_uNr] != NULL){
			delete m_pB[m_uNr];
			m_pB[m_uNr] = NULL;
		}
	}
}
// OLD CODE 	{ m_uNr=(uNewNr<NR_B_CHANNELS && m_pB[uNewNr])?uNewNr:0; }
void	CConnect::SetChannel(UINT uNewNr)	
{ 
	if (uNewNr>=NR_B_CHANNELS) 
	{
		WK_TRACE(_T("Invalid id in SetChannel %d\n"),uNewNr);
		uNewNr=0;
	} 
	else if (m_pB[uNewNr]==NULL) 
	{
		// OOPS no msg, it's called in WM_TIMER....
		// WK_TRACE(_T("Empty channel selected in SetChannel %d\n"),uNewNr);
		uNewNr=0;
	}
	m_uNr= uNewNr;
}

	                                  
DWORD CConnect::GetNextController()
{
	return (DWORD)(CountConnections()/2+m_uControllerInc);
}	                                  

BYTE CConnect::GetNextChannelNr()
{
	if 		(m_uNrMax==0) return 0x81;	// First B-Channel
	else if (m_uNrMax==1) return 0x82;	// Second B-Channel
	else 				  return 0x83;	// Next B-Channel
}

BOOL CConnect::CreateChannel(	LPCSTR szTelNr,
								DWORD dwPlci, 
								BOOL bAufbau,
								WORD wReject,
								LONG lTimeout,
								WORD wInfo)
{
	for (m_uNr=0;m_uNr<NR_B_CHANNELS;m_uNr++)
	{
		if (m_pB[m_uNr] && m_pB[m_uNr]->GetPlci() == dwPlci) // ist PLCI schon vorhanden?
		{
			WK_TRACE_ERROR(_T("CreateChannel known PLCI\n"));
			return FALSE;                              
		}
	}
	for (m_uNr=0;m_uNr<NR_B_CHANNELS;m_uNr++)
	{
		if (m_pB[m_uNr] == NULL)
		{
			m_pB[m_uNr] = new CBChannel;
			m_pB[m_uNr]->Create(szTelNr, dwPlci, bAufbau, wReject, lTimeout, wInfo); 
			CountConnections();
			return TRUE;		// EXIT
		}	
    }    

	WK_TRACE_ERROR(_T("CreateChannel failed\n"));
    SetNextValidChannel();
    return FALSE;
} 

void CConnect::DestroyChannel()
{
	if (m_pB[m_uNr] && m_uNr<NR_B_CHANNELS)      
	{	                                    
		delete m_pB[m_uNr];
		m_pB[m_uNr] = NULL;  
		// OOPS HEDU how about SetNextValidChannel ?
		m_uNr = m_uNr==0?0:m_uNr-1; 
		CountConnections();
	}
}	            

void CConnect::Reset()
{ 
	for (int i=0;i<NR_B_CHANNELS;i++) 
	{                    
		if (m_pB[i])
		{
			delete m_pB[i];
			m_pB[i]=NULL; 
		}
	}
	m_uNr = 0;
	m_uNrMax = 0;
	m_uControllerInc = 1; 
}   

                          
         
BOOL CConnect::FindPlci(DWORD dwPlci)
{
	for (m_uNr=0;m_uNr<NR_B_CHANNELS;m_uNr++)
	{
		if (m_pB[m_uNr] && (m_pB[m_uNr]->GetPlci() == (dwPlci&0x0000FFFFL)))
			return TRUE;
	}
	SetNextValidChannel();                     
	return FALSE;
}
		
                     


void CConnect::SetCommands(BYTE byCmd, BYTE bySubcmd)                      
{ 
	if (m_pB[m_uNr]) 
	{
		m_pB[m_uNr]->SetNewCommands(byCmd, bySubcmd); 
	}
	for (UINT i=0;i<NR_B_CHANNELS;i++)
	{
		if (m_pB[i] && m_pB[i]->GetPasswortOk() && m_pB[i]->GetCmd()==CONNECT_B3_ACTIVE)
		{
			m_bActive = TRUE;
			return;		// <<<EXIT>>>
		} 
		else 
		{
			m_bActive = FALSE;
			if (m_pB[i]) 
			{
				m_pB[i]->SetPasswortOk(FALSE);
			}
		}	
	}
}                                     
                                    
void CConnect::SetAbbau(BOOL bAbbau)
{ 
	if (m_pB[m_uNr]) 
	{ 
		m_pB[m_uNr]->SetAbbauFlag(bAbbau); 
		if (bAbbau) 
			m_pB[m_uNr]->SetPasswortOk(FALSE); 
	} 
}
                                    
///////////////////////////////////////////////////////////////////////////
// Funktions für CBChannel Class

CBChannel::CBChannel()
{
	//@INIT m_sIsdnNr is a CLimitedString
    // HEDU memset(m_isdn_nr, 0, BUFFER_32);// Telefonnummer der aktuellen Verbindung
    m_dwNcciPlciCntrl = 0L;		// plci der aktiven Verbindung
    m_info_mask 	= 0;			// Info-Maske der Anwendung
    m_bAufbau	 	= FALSE;	
	m_bAbbau		= FALSE;
    m_controller 	= 0;			// Boardnummer des anzusprechenden Tina-Boardes (0)
    m_reject 		= FALSE;		// 0 Verbindung wird angenommen 1 Verbindung wird abgelehnt
	m_iBufValid 	= 0;			// Anz. Sendebuffers die valid sind.
	m_byCmd		 	= 0;
	m_bySubcmd		= 0xFF; 
	m_lTimer		= 0L;  
	m_bPasswortOk	= FALSE;
}

void CBChannel::Create(	LPCSTR szTelNr, DWORD dwPlci, 
						BOOL bAufbau,	WORD wReject, 
						LONG lTimeout,  WORD wInfo)
{     
	m_sIsdnNr.SetText(szTelNr);
	m_dwNcciPlciCntrl = dwPlci;
	m_bAufbau	= bAufbau;
	m_reject 	= wReject;
	m_info_mask = wInfo;    
	m_bAbbau	= FALSE;           
	m_lTimer	= lTimeout;
	m_byCmd		= 0;
	m_bySubcmd	= 0xFF; 
	m_bPasswortOk	= FALSE;
}                       

void CBChannel::SetKanalId(LPCTSTR szId)			
{ 
	ASSERT(szId && _tcslen(szId) < BUFFER_32);
	m_sId.SetText(szId);
}

void CBChannel::SetNewCommands(BYTE byCmd, BYTE bySubcmd)
{ 
	m_byCmd = byCmd;
	m_bySubcmd = bySubcmd;
}			
		
