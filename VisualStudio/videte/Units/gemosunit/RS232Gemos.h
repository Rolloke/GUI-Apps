// RS232Gemos.h: interface for the CRS232Gemos class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RS232GEMOS_H__BDA78038_4B4D_11D3_9979_004005A19028__INCLUDED_)
#define AFX_RS232GEMOS_H__BDA78038_4B4D_11D3_9979_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRS232Gemos : public CWK_RS232  
{
	// construction / destruction
public:
	BOOL TestingConnection();
	CRS232Gemos();
	virtual ~CRS232Gemos();

	BOOL	SendCommand(const CString &sSendCommand);
	CString GetAndRemoveCommand();

	void SetTraceENQ(BOOL bTrace){m_bTraceENQ = bTrace;}
	void SetTraceACK(BOOL bTrace){m_bTraceACK = bTrace;}
	void SetTraceNAK(BOOL bTrace){m_bTraceNAK = bTrace;}
	void SetTraceSYN(BOOL bTrace){m_bTraceSYN = bTrace;}
	void SetTraceDC3(BOOL bTrace){m_bTraceDC3 = bTrace;}
	void SetTraceEOT(BOOL bTrace){m_bTraceEOT = bTrace;}
	void SetTraceCMD(BOOL bTrace){m_bTraceCMD = bTrace;}

	BOOL GetTraceENQ(){return m_bTraceENQ;}
	BOOL GetTraceACK(){return m_bTraceACK;}
	BOOL GetTraceNAK(){return m_bTraceNAK;}
	BOOL GetTraceSYN(){return m_bTraceSYN;}
	BOOL GetTraceDC3(){return m_bTraceDC3;}
	BOOL GetTraceEOT(){return m_bTraceEOT;}
	BOOL GetTraceCMD(){return m_bTraceCMD;}

	// virtual functions
protected:
	virtual void OnReceivedData(LPBYTE pData, DWORD dwLen);

	// implementation
protected:
	virtual void OnErrorReceived(const CString &sErr);
	void OnReceivedData(BYTE data);
	void OnENQ();
	void OnACK();
	void OnNAK();
	void OnSYN();
	void OnDC3();
	void OnEOT();
	void OnPackage(BYTE data);
	void ProcessCommand();
	DWORD WriteByte(BYTE b);
	DWORD WriteString(const CString &sCmd);

	// member
private:
	enum GemosState 
	{
		GEMOS_STX,
		GEMOS_DATA,
		GEMOS_BCC1,
		GEMOS_BCC2,
	};

	GemosState		m_eState;
	BYTE			m_bCalculatedCheckSum;
	CString			m_sTransmittedCheckSum;
	CString			m_sReceiveCommand;
	CStringArray	m_sReceiveCommandList;

	HANDLE 			m_hSYNEvent;
	HANDLE 			m_hACKEvent;
	HANDLE 			m_hNAKEvent;
	HANDLE 			m_hDC3Event;
	int				m_nMaxENQRepeats;
	int				m_nMaxNAKRepeats;
	int				m_nMaxTimeOutRepeats;
			  
	CCriticalSection m_csFetchCmd;
	
	BOOL			m_bTraceENQ;
	BOOL			m_bTraceACK;
	BOOL			m_bTraceNAK;
	BOOL			m_bTraceSYN;
	BOOL			m_bTraceDC3;
	BOOL			m_bTraceEOT;
	BOOL			m_bTraceCMD;
};

#endif // !defined(AFX_RS232GEMOS_H__BDA78038_4B4D_11D3_9979_004005A19028__INCLUDED_)
